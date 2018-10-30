#include <elm/avl/Set.h>
#include <otawa/parexegraph/GraphBBTime.h>
#include <otawa/parexegraph/ParExeGraph.h>
#include <otawa/proc/ProcessorPlugin.h>
#include <otawa/proc/Processor.h>
#include <otawa/branch/BranchBuilder.h>
#include <otawa/branch/features.h>
#include <otawa/cfg/features.h>
#include <otawa/loader/gliss.h>
#include <elm/avl/Map.h>
#include <otawa/data/clp/features.h>
#include <otawa/dfa/State.h>


extern "C" {
#include <tricore/api.h>
#include <tricore/macros.h>
#include <tricore/loader.h>
#include <tricore/id.h>
#include <tricore/mem.h>
#include <tricore/used_regs.h>

#define MEM_PAGE_SIZE 4096

typedef struct page_entry_t
{
    tricore_address_t      addr;
    struct page_entry_t* next;
    uint8_t*             storage;//[MEM_PAGE_SIZE];// ça change quoi de faire un tableau

} page_entry_t;

typedef struct tricore_memory_t
{
	void* image_link; /* link to a generic image data resource of the memory
	                     it permits to fetch informations about image structure
	                     via an optionnal external system */
    page_entry_t* hashtable[65536];
#	ifdef STATS
	int stats_pages[HASHTABLE_SIZE];
	int stats_accesses[HASHTABLE_SIZE];
#	endif
#ifdef TRICORE_MEM_SPY
	tricore_mem_spy_t spy_fun;	/** spy function */
	void *spy_data;				/** spy data */
#endif
} memory_64_t;

}



using namespace otawa;
namespace otawa { namespace tricore16 {


Identifier<unsigned int> CLP_INITIATOR_STOP_POINT("otawa::tricore16::CLP_INITIATOR_STOP_POINT", 0xFFFFFFFF);

static avl::Set<unsigned int> accessAddrs;
static Vector<unsigned int> hardwareWrites;
static void spyx(tricore_memory_t *mem, tricore_address_t addr, tricore_size_t size, tricore_access_t access, void *data);

static bool debug = false;

class CLPInitiator: public otawa::Processor {
public:
	static p::declare reg;
	CLPInitiator(void): otawa::Processor(reg), stop(0xFFFFFFFF) { }
	static unsigned int currPC;
	static tricore_sim_t *sim;
	static dfa::State *dfaState;
	unsigned int stop;

protected:

	virtual void configure(const PropList& props) {
		otawa::Processor::configure(props);
		stop = CLP_INITIATOR_STOP_POINT(props);
		elm::cout << "stop = " << hex(stop) << endl;
	}

	// ==============================================================
	// printing out the state of the processor
	// ==============================================================
	void trace() {
		elm::cout << "| PC  = 0x" << io::align(io::LEFT, io::width(8, hex( (sim->state->PC)              ) )) << "  ";
		elm::cout << "| PCSX= 0x" << io::align(io::LEFT, io::width(8, hex( (sim->state->PCXI >> 16) & 0xF) )) << "  ";
		elm::cout << "| PCSO= 0x" << io::align(io::LEFT, io::width(8, hex( (sim->state->PCXI & 0xFFFF)   ) )) << "  ";
		elm::cout << "| PCXI= 0x" << io::align(io::LEFT, io::width(8, hex( (sim->state->PCXI)            ) )) << "  ";
		elm::cout << "| FCX = 0x" << io::align(io::LEFT, io::width(8, hex( (sim->state->FCX)             ) )) << "  ";
		elm::cout << "| PSW = 0x" << io::align(io::LEFT, io::width(8, hex( (sim->state->PSW)             ) )) << "  ";
		elm::cout << "| IO  = 0x" << io::align(io::LEFT, io::width(8, hex( (sim->state->PSW >> 10)&3     ) )) << "  ";
		elm::cout << "| CTX = 0x" << io::align(io::LEFT, io::width(8, hex( (sim->state->CTX )            ) )) << "  ";
		elm::cout << "|" << endl;

		for(int ih = 0; ih < 16; ih++) {
			elm::cout << "| D" << io::align(io::LEFT, io::width(2, ih)) << " = 0x" << io::align(io::LEFT, io::width(8, hex(sim->state->D[ih]))) << "  ";
			if(ih == 7) elm::cout << "|" << endl;
		}
		elm::cout << "|" << endl;

		for(int ih = 0; ih < 16; ih++) {
			elm::cout << "| A" << io::align(io::LEFT, io::width(2, ih)) << " = 0x" << io::align(io::LEFT, io::width(8, hex(sim->state->A[ih]))) << "  ";
			if(ih == 7) elm::cout << "|" << endl;
		}
		elm::cout << "|" << endl;
	}

	void extractState(tricore_sim_t *sim, clp::State& clpState) {
		for(avl::Set<unsigned int>::Iterator a(accessAddrs); a; a++) {
			clp::Value addr(*a);
			unsigned char data_dump = 0xDB;
			tricore_mem_read(sim->state->M, *a, (void*)&data_dump, 1);
			clpState.set(addr, clp::Value(data_dump));
		}

		clp::Value actualValue;
		actualValue = clp::Value(sim->state->A[10]); // A10
		clpState.set(clp::Value(clp::REG, 26), actualValue);

		// actualValue = clp::Value(sim->state->FCX); // 34
		actualValue = clp::Value(sim->state->CTX); // CTX
		clpState.set(clp::Value(clp::REG, 36), actualValue);

	}

	bool isInvariant(unsigned int addr) {
		if((addr >= 0x60006bcc) && (addr <= (0x60006bcc + 48))) return true; // g_appVadc_kernel_motor1 and its groups
		else if((addr >= (0x60006bcc + 52*1)) && (addr <= (0x60006bcc + 52*1 + 8))) return true; // g_appVadc_kernel_motor1.channel[0]
		else if((addr >= (0x60006bcc + 52*2)) && (addr <= (0x60006bcc + 52*2 + 8))) return true; // g_appVadc_kernel_motor1.channel[1]
		else if((addr >= (0x60006bcc + 52*3)) && (addr <= (0x60006bcc + 52*3 + 8))) return true; // g_appVadc_kernel_motor1.channel[2]
		else if(addr == (0x60006c9c + 92)) return true; // g_motor1.sensors(.encoder)(.base).functions
		else if(addr == (0x60006c9c + 264)) return true; // g_motor1.analogInputs.currentA
		else if(addr == (0x60006c9c + 268)) return true; // g_motor1.analogInputs.currentB
		else if(addr == (0x60006c9c + 272)) return true; // g_motor1.analogInputs.currentC
		else if(addr == (0x60006c9c + 276)) return true; // g_motor1.analogInputs.sensorOffset
		else if(addr == (0x60006c9c + 280)) return true; // g_motor1.analogInputs.vdc
		else if(addr == (0x60006c9c + 0)) return true; // g_motor1(.inverter)(.pwm)
		else if(addr == (0x70019518 + 0)) return true; // AppPwmDriver_3ChPwm(.tmr)(.base)(.driver)
		else if((addr >= (0x70019518 + 4)) && (addr <= (0x70019518 + 68))) return true; // AppPwmDriver_3ChPwm.tmr.base.functions
		else if(addr == (0x70019518 + 88)) return true; // AppPwmDriver_3ChPwm(.tmr)(.gtm)
		else if(addr == (0x70019518 + 92)) return true; // AppPwmDriver_3ChPwm(.tmr)(.tom)
		else if(addr == (0x70019518 + 96)) return true; // AppPwmDriver_3ChPwm(.tmr)(.tgc) // TGC registers
		else if(addr == (0x70019518 + 116)) return true; // AppPwmDriver_3ChPwm(.ch)(.base).driver
		else if(addr == (0x70019518 + 120)) return true; // AppPwmDriver_3ChPwm(.ch)(.base).timer
		else if(addr == (0x70019518 + 188)) return true; // AppPwmDriver_3ChPwm(.ch).update
		else if(addr == (0x70019518 + 192)) return true; // AppPwmDriver_3ChPwm(.ch).tom
		else if(addr == (0x70019518 + 196)) return true; // AppPwmDriver_3ChPwm(.ch).tgc
		else if(addr == (0x70019518 + 224)) return true; // AppPwmDriver_3ChPwm(.ch).ccxTemp
		else if(addr == (0x70019518 + 228)) return true; // AppPwmDriver_3ChPwm(.ch).coutxTemp
		else if((addr >= (0x70019518 + 156)) && (addr <= (0x70019518 + 184))) return true; // AppPwmDriver_3ChPwm.ch.base.functions

		else if(addr == (0x60000018)) return true; // cosTable

		return false;

	}

	void extractDFAState(tricore_sim_t *sim, dfa::State* dfaState) {

		unsigned int currAddr = 0xFFFFFFFF;
		unsigned int currOffset = 0xFFFFFFFF;
		unsigned int currData = 0xCAFEBABE;
		for(avl::Set<unsigned int>::Iterator a(accessAddrs); a; a++) {

			if((*a) % 4 == 0) {
				currOffset = 0;
				currAddr = *a;
				tricore_mem_read(sim->state->M, currAddr, (void*)&currData, 4);
				if(isInvariant(currAddr))
					dfaState->record(dfa::MemCell(Address(currAddr), &Type::uint32_type, dfa::Value(currData)));
			}
			else {
				currOffset = (*a) % 4;
				ASSERTP(currAddr == ((*a) - currOffset), "Address is not aligned");

			}


			//unsigned char data_dump = 0xDB;
			//tricore_mem_read(sim->state->M, *a, (void*)&data_dump, 1);
			//dfaState->record(dfa::MemCell(Address(*a), &Type::uint8_type, dfa::Value(data_dump)));

		}

		const hard::Register *regA10 = workspace()->process()->platform()->findReg(26);
		dfaState->set(regA10, dfa::Value(sim->state->A[10]));

		// FCX 34
		// CTX 36
		const hard::Register *regCTX = workspace()->process()->platform()->findReg(36);
		dfaState->set(regCTX, dfa::Value(sim->state->CTX));
	}

	virtual void processWorkSpace(WorkSpace *fw) {

		dfaState = dfa::INITIAL_STATE(fw);

		unsigned int verifiedReg = 0;
		unsigned int verifiedMem = 0;
		int exit = 0;
		char buffer[256];


	    tricore_state_t *state = 0;
	    tricore_platform_t *platform = 0;
		tricore_loader_t *loader = 0;
	    tricore_address_t addr_start = 0;
	    tricore_address_t addr_exit = 0;
	    int sym_exit = 0, sym_start = 0;

	    Process::FileIter file(workspace()->process());
	    loader = tricore_loader_open(file->name());
        addr_start = tricore_loader_start(loader);
		printf("START=0x%08x\n", addr_start);

		// ================== /* search symbol _exit */ ============================
		int found = 0;

		for(sym_exit = 0; sym_exit < tricore_loader_count_syms(loader); sym_exit++) {
			tricore_loader_sym_t data;
			tricore_loader_sym(loader, sym_exit, &data);
			if(strcmp(data.name, "_exit") == 0) {
				/* we found _exit */
				addr_exit = data.value;
				found = 1;
				break;
			}
		}
		/* check for error */
		if(!found)
			ASSERTP(0, "ERROR: cannot find the \"_exit\" symbol and no exit address is given.\n");
		printf("EXIT=0x%08x\n", addr_exit);

		/* make the platform */
	    platform = tricore_new_platform();
		ASSERTP(platform, "ERROR: cannot create platform\n");

		tricore_load_platform(platform, file->name());

		/* make the state depending on the platform */
		state = tricore_new_state(platform);
		ASSERTP(state, "ERROR: cannot create state\n");

		/* make the simulator */
	    sim = tricore_new_sim(state, addr_start, addr_exit);
		ASSERTP(sim, "ERROR: cannot create simulator\n");
		int COVERAGE[TRICORE_INSTRUCTIONS_NB+1];
		for(int i = 0; i < TRICORE_INSTRUCTIONS_NB; i++)
			COVERAGE[i] = 0;

		// ===============================  copy table (ref. linker script) =========================
		// search symbol __copy_table
		unsigned long datax;
		unsigned long addrx = 0;
		found = 0;
        for(sym_exit = 0; sym_exit < tricore_loader_count_syms(loader); sym_exit++) {
            tricore_loader_sym_t data;
            tricore_loader_sym(loader, sym_exit, &data);
			if(strcmp(data.name, "__copy_table") == 0) {
				addrx = data.value;
				found = 1;
				break;
			}
		}
        if(!found) { ASSERTP(0, "__copy_table is not found\n"); }
		unsigned int copy_src = 0;
		unsigned int copy_dest = 0;
		unsigned int copy_size = 0;
		while(1) {
			tricore_mem_read(sim->state->M, addrx, (void*)&copy_src, 4);
			addrx += 4;
			tricore_mem_read(sim->state->M, addrx, (void*)&copy_dest, 4);
			addrx += 4;
			tricore_mem_read(sim->state->M, addrx, (void*)&copy_size, 4);
			addrx += 4;
			elm::cout << "copy_src = 0x" << hex(copy_src) << ", copy_dest = 0x" << hex(copy_dest) << ", copy_size = 0x" << hex(copy_size) << endl;
			if(copy_size == 0xFFFFFFFF)
				break;
			else {
				int count = 0;
				while(count < copy_size) {
					tricore_mem_read(sim->state->M, copy_dest+count, (void*)&datax, 4);
					tricore_mem_write(sim->state->M, copy_src+count, (void*)&datax, 4);
					count++;
				}
			}
		}
		// =============================== finish copy table (ref. linker script) =========================

		// =============================== set CORE ID to 0  =========================
		datax = 0;
		tricore_mem_write(sim->state->M, 0xf881fe1c, (void*)&datax, 4); // CSFR_Base + FE1C -> CORE ID, set to 0, as we are simulating processor 0.
		datax = 0x01120148;
		tricore_mem_write(sim->state->M, 0xf0036030, (void*)&datax, 4); // reset value of SCU_CCUCON0 p7-48
		// ===========================================================================

		unsigned long data;
		tricore_mem_set_spy(sim->state->M, spyx, (void*)&data);
        tricore_inst_t *inst;

        bool init = true;

        while(!tricore_is_sim_ended(sim) && !exit) // execution loop
		{
        	// hardware info process
        	if(hardwareWrites.count()) {
        		unsigned long hardwareAddr = hardwareWrites.pop();
        		if(hardwareAddr == 0xF000064C) { // ASCLIN_CSR
        			unsigned long data_to_read = 0;
        			tricore_mem_read(sim->state->M, hardwareAddr, (void*)&data_to_read, 4);
        			if((data_to_read & 0x1F) == 0) { // no clock source selected
        				unsigned char data_to_write = 0x0;
        				tricore_mem_write(sim->state->M, hardwareAddr+3, (void*)&data_to_write, 1); // 0xF000064F
        			}
        			else { // the clock is started
        				unsigned char data_to_write = 0x80;
        				tricore_mem_write(sim->state->M, hardwareAddr+3, (void*)&data_to_write, 1); // 0xF000064F
        			}
        		}
        	}
        	// end of hardware

            inst = tricore_next_inst(sim);
            COVERAGE[inst->ident] = 1;

//            trace();
//    		tricore_disasm(buffer, inst);
//            elm::cout << "running " << hex(tricore_next_addr(sim)) <<": " << buffer << endl;

			tricore_step(sim);
			currPC = sim->state->PC;

			if(sim->state->PC == 0) { // end of the program
				exit = 1;
			}
			else if(sim->state->PC == stop) { // reaches the specific PC
				exit = 1;
			}

            tricore_free_inst(inst); // free each instruction after use
		} // while loop

        trace();
        clp::State clpState;
        //extractState(sim, clpState);
        //clpState.print(elm::cout, workspace()->platform());

        extractDFAState(sim, dfaState);

		/* close loader file */
	    tricore_loader_close(loader);

	    elm::cout << "Finish processing at otawa::tricore16::CLPInitiator" << endl;

//		for(int i = 0; i < TRICORE_INSTRUCTIONS_NB; i++)
//			elm::cout << COVERAGE[i] << endl;

//		for(avl::Set<unsigned int>::Iterator a(accessAddrs); a; a++) {
//			unsigned char data_dump = 0xDB;
//			tricore_mem_read(sim->state->M, *a, (void*)&data_dump, 1);
//			elm::cout << hex(*a) << " = " << hex(data_dump) << endl;
//		}

	}
private:
};

unsigned int CLPInitiator::currPC = 0;
tricore_sim_t *CLPInitiator::sim = 0;
dfa::State *CLPInitiator::dfaState = 0;

static void hardwareAddr(tricore_address_t addr) {
	if(addr == 0xF000064C) {
		hardwareWrites.push(addr);
	}
}

static void spyx(tricore_memory_t *mem, tricore_address_t addr, tricore_size_t size, tricore_access_t access, void *data) {
	if(access == tricore_access_write) {
		hardwareAddr(addr);
		accessAddrs.add(addr);
		//elm::cout << "writes to " << hex(addr) << endl;
	}
}

p::declare CLPInitiator::reg = p::init("otawa::tricore16::CLPInitiator", Version(1, 0, 0))
		.base(otawa::Processor::reg)
		.maker<CLPInitiator>()
		.require(dfa::INITIAL_STATE_FEATURE)
		;
}}


