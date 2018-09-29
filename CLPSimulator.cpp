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
#include <otawa/willie.h>


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
namespace otawa { namespace tricore16P {

Identifier<bool> INITIALIZED("INITIALIZED", false);
Identifier<bool> COVERED("COVERED", false);

extern unsigned long ENDNUM;
static avl::Set<unsigned int> accessAddrs;
static void spyx(tricore_memory_t *mem, tricore_address_t addr, tricore_size_t size, tricore_access_t access, void *data);

static bool debug = false;
static bool debug_cfg = false;

class CLPSimulator: public otawa::Processor {
public:
	static p::declare reg;
	CLPSimulator(void): otawa::Processor(reg) { }

	static int BIGNUM;
	static int BIGNUMa;
	static int currPC;
	static bool logging;
	static tricore_sim_t *sim;

protected:

	virtual void configure(const PropList& props) {
		otawa::Processor::configure(props);
	}



	void extractState(tricore_sim_t *sim, clp::State& clpState) {
		if(debug) {
			elm::cout << "Access addrs: ";
			for(avl::Set<unsigned int>::Iterator a(accessAddrs); a; a++)
				elm::cout << hex(*a) << " ";
			elm::cout << endl;
		}

		for(int regIndex = 0; regIndex < workspace()->process()->platform()->regCount(); regIndex++) {
			clp::Value actualValue;
			if(regIndex >= 0 && regIndex <= 15) { // Data register
				actualValue = clp::Value(sim->state->D[regIndex]);
				//elm::cout << "D" << regIndex << " = " << hex(sim->state->D[regIndex]) << endl;
			}
			else if(regIndex >= 16 && regIndex <= 31) // Address register
				actualValue = clp::Value(sim->state->A[regIndex-16]);
			else if(regIndex == 32) // PSW register
				continue; // actualValue = clp::Value(sim->state->PSW);
			else if(regIndex == 33) { // PC register
				//elm::cout << "PC" << " = " << hex(sim->state->PC) << endl;
				continue; // actualValue = clp::Value(sim->state->PC);
			}
			else if(regIndex == 34) // FCX register
				actualValue = clp::Value(sim->state->FCX);
			else if(regIndex == 36)
				actualValue = clp::Value(sim->state->CTX);
			else
				continue;
			clpState.set(clp::Value(clp::REG, regIndex), actualValue);
		}
	}


	virtual void processWorkSpace(WorkSpace *fw) {
		unsigned int verifiedReg = 0;
		unsigned int verifiedMem = 0;


		int exit = 0;

		const CFGCollection *cfgs = INVOLVED_CFGS(fw);
		ASSERT(cfgs);

		bool first = true;
		Address addressToTrigger = 0;
		CFG* working_cfg = 0;
		BasicBlock* working_bb = 0;
		bool enterNewBB = true;
		Inst* working_inst = 0;
		int totalInst = 0;
		int totaltotalInst = 0;
		Vector<Block*> callStack;

		// Visit CFG
		for(CFGCollection::Iter cfg(cfgs); cfg; cfg++) {
			if (first) {
				first = false;
				addressToTrigger = cfg->address();
				working_cfg = cfg;
				working_bb = cfg->entry()->outs()->target()->toBasic();
				working_inst = working_bb->first();
			}
//			elm::cout << "\tprocess CFG " << cfg->label() << io::endl;
			for(CFG::BlockIter bb = cfg->blocks(); bb; bb++) {
				if(bb->isBasic())
					totaltotalInst = totaltotalInst + bb->toBasic()->count();
//				elm::cout << "\t\tprocess " << *bb << io::endl;
//				clp::State clpState = clp::STATE_IN(bb);
//
//				elm::cout << "\t\t\t";
//				clpState.print(elm::cout, workspace()->platform());
//				elm::cout << io::endl;
			}
		}

		elm::cout << "Address to trigger = " << addressToTrigger << endl;
		elm::cout << "There are " << totaltotalInst << " instructions in this program" << endl;


		bool verbose = true;
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
		unsigned int datax;
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
			printf("copy_src = 0x%X, copy_dest = 0x%X, copy_size = 0x%X\n", copy_src, copy_dest, copy_size);
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
		// ===========================================================================

		unsigned long data;
		tricore_mem_set_spy(sim->state->M, spyx, (void*)&data);
        tricore_inst_t *inst;
        int count = 1;
        bool following = false;
        clp::Manager clpManager(workspace());

        bool init = true;
        logging = false;

        while(!tricore_is_sim_ended(sim)) // execution loop
		{

            inst = tricore_next_inst(sim);


            COVERAGE[inst->ident] = 1;


            // ==============================================================
            // printing out the state of the processor
            // ==============================================================
            if(verbose && (count >= BIGNUM))
            {
                fprintf(stdout, "| PC  = 0x%-8X  ", sim->state->PC);
                fprintf(stdout, "| PCSX= 0x%-8X  ", ((sim->state->PCXI >> 16) & 0xF));
                fprintf(stdout, "| PCSO= 0x%-8X  ", (sim->state->PCXI & 0xFFFF));
                fprintf(stdout, "| PCXI= 0x%-8X  ", sim->state->PCXI);
                fprintf(stdout, "| FCX = 0x%-8X  ", sim->state->FCX);
                fprintf(stdout, "| PSW = 0x%08x  ", sim->state->PSW);
                fprintf(stdout, "| IO  = 0x%-8X  ", ((sim->state->PSW >> 10)&3));
                printf("|\n");

                for(int ih = 0; ih < 16; ih++) {
                	fprintf(stdout, "| D%-2d = 0x%-8X  ", ih, sim->state->D[ih]);
                	if(ih == 7) printf("|\n");
                }
                printf("|\n");

                for(int ih = 0; ih < 16; ih++) {
                	fprintf(stdout, "| A%-2d = 0x%-8X  ", ih, sim->state->A[ih]);
                	if(ih == 7) printf("|\n");
                }
                printf("|\n");


                tricore_disasm(buffer, inst);
                fprintf(stdout, "running %08x: %s\n", tricore_next_addr(sim),  buffer);
            }
            // ==============================================================




//            if(debug_cfg)
//            {
//            	elm::cout << "count = " << count << endl;
//                fprintf(stdout, "| PC  = 0x%-8X  ", sim->state->PC);
//                fprintf(stdout, "| PCSX= 0x%-8X  ", ((sim->state->PCXI >> 16) & 0xF));
//                fprintf(stdout, "| PCSO= 0x%-8X  ", (sim->state->PCXI & 0xFFFF));
//                fprintf(stdout, "| PCXI= 0x%-8X  ", sim->state->PCXI);
//                fprintf(stdout, "| FCX = 0x%-8X  ", sim->state->FCX);
//                fprintf(stdout, "| PSW = 0x%08x  ", sim->state->PSW);
//                fprintf(stdout, "| IO  = 0x%-8X  ", ((sim->state->PSW >> 10)&3));
//                printf("|\n");
//
//                for(int ih = 0; ih < 16; ih++) {
//                	fprintf(stdout, "| D%-2d = 0x%-8X  ", ih, sim->state->D[ih]);
//                	if(ih == 7) printf("|\n");
//                }
//                printf("|\n");
//
//                for(int ih = 0; ih < 16; ih++) {
//                	fprintf(stdout, "| A%-2d = 0x%-8X  ", ih, sim->state->A[ih]);
//                	if(ih == 7) printf("|\n");
//                }
//                printf("|\n");
//            }

            if(!following && (inst->addr == addressToTrigger.offset())) {
            	following = true;
            }

            if(following) {
                // begining of the BB
                if(enterNewBB) {
                	COVERED(working_bb) = true;
                	enterNewBB = false;
                	clp::State clpState = clp::STATE_IN(working_bb);
                	if(!INITIALIZED(working_bb)) { // use the current state as the STATE_IN
                		INITIALIZED(working_bb) = true;
                		totalInst = totalInst + working_bb->count();
                		clp::State currentCLPState;
                		extractState(sim, currentCLPState);
                		//currentCLPState.print(elm::cout, workspace()->process()->platform()); elm::cout << endl;
                		clp::STATE_IN(working_bb) = currentCLPState;
                		// elm::cout << "extracting state for " << working_bb << endl;
                	}
                	else { // STATE_IN' = STATE_IN joining incoming state
                		clp::State orig = clp::STATE_IN(working_bb);
                		clp::State come;
                		extractState(sim, come);
                		orig.join(come);
                		clp::STATE_IN(working_bb) = orig;
                	}
                }

                //elm::cout << "processing working_inst " << working_inst << " @ " << working_inst->address()<< endl;
                if(!INITIALIZED(working_inst)) {
                	INITIALIZED(working_inst) = true;
                	clp::State currentCLPState;
					extractState(sim, currentCLPState);
					clp::STATE_IN(working_inst) = currentCLPState;
					//currentCLPState.print(elm::cout, workspace()->platform()); elm::cout << endl;
                }
                else {
            		clp::State orig = clp::STATE_IN(working_inst);
            		clp::State come;
            		extractState(sim, come);

            		//elm::cout << "orig = "; orig.print(elm::cout, workspace()->platform()); elm::cout << endl;
            		//elm::cout << "come = "; come.print(elm::cout, workspace()->platform()); elm::cout << endl;

            		orig.join(come);
            		clp::STATE_IN(working_inst) = orig;
            		//elm::cout << "resu = "; orig.print(elm::cout, workspace()->platform()); elm::cout << endl;
                }



            	// output the instruction info
                if(debug) {
					elm::cout << __GREEN__ << "executing " << Address(inst->addr) << " towards to " << working_bb->last()->address() << " of CFG " << working_cfg->index() << " BB " << working_bb->index() << " ";
					tricore_disasm(buffer, inst);
					elm::cout << buffer << " @ " << count << " => ";
					unsigned int codes = 0;
					unsigned int codesX = 0;
					tricore_mem_read(sim->state->M, inst->addr, (void*)&codes, 4);
					for(int wis = 0; wis < working_inst->size(); wis++) {
						int shift = (8*(working_inst->size() - wis - 1));
						elm::cout <<hex((codes >> shift) &0xFF)<< " ";
					}
					elm::cout << __RESET__ << io::endl;
                }
            }


			unsigned int oldPC = sim->state->PC;
			if(following)
				logging = true;
			tricore_step(sim);
			logging = false;
			currPC = sim->state->PC;

			if(following) {

                // reacheds the end of BB
            	if(Address(inst->addr) == working_bb->last()->address())  {
    				// need to decide the following block
    				Address nextAddress = Address(sim->state->PC);
    				for (Block::EdgeIter outEdge = working_bb->outs(); outEdge; outEdge++) {
    					if(outEdge->target()->isSynth()) {
    						if(debug_cfg)
    							elm::cout << __RED__ << "    Making a call" << __RESET__ << endl;
    						working_cfg = outEdge->target()->toSynth()->callee();
    						working_bb = working_cfg->entry()->outs()->target()->toBasic();
    						working_inst = working_bb->first();
    						callStack.push(outEdge->target()->outs()->target());
    					}
    					else if (outEdge->target()->isExit() && callStack.count()) {
    						if(debug_cfg)
    							elm::cout << __RED__ << "    Leaving a call" << __RESET__ << endl;
    						working_bb = nullptr;
    						Block* returnBlock = callStack.pop();
							if(returnBlock->isSynth()) {
								if(debug_cfg)
									elm::cout << __RED__ << "    Sequential call!" << __RESET__ << endl;
							}
							else if (returnBlock->isExit()) {
								if(debug_cfg)
									elm::cout << __RED__ << "    Sequential exit!" << __RESET__ << endl;
							}
							else if(returnBlock->address() == nextAddress) {
								working_bb = returnBlock->toBasic();
								working_cfg = working_bb->cfg();
								working_inst = working_bb->first();
							}
    						ASSERT(working_bb);
    					}
    					else if(outEdge->target()->isExit() && callStack.count() == 0) {
    						exit = 1;
    					}
    					else {
    						if(outEdge->target()->toBasic()->first()->address() == nextAddress) {
    							working_bb = outEdge->target()->toBasic();
    							working_inst = working_bb->first();
    							if(debug_cfg)
    								elm::cout << __RED__ << "    continue to a normal basic block" << __RESET__ << endl;
    							break;
    						}
    					}
    				}
    				enterNewBB = true;
            	} // reaching the end of BB
            	else {
            		working_inst = working_inst->nextInst();
            	}

            }




            if(following) {
            	if((exit != 1) && (Address(sim->state->PC) != 0)) {
            		if((Address(sim->state->PC) >= working_bb->address()) && (Address(sim->state->PC) <= working_bb->last()->address())) {}
            		else {
            			elm::cout << "count = " << count << endl;
            			elm::cout << "Address " << Address(sim->state->PC) << " is out of the current BB: CFG " << working_cfg->index() << " " << working_bb << endl;
            			ASSERT(0);
            		}
            	}
            } // if following

        	count++;
        	BIGNUMa = count;

        	ENDNUM = 193487525;


        	if(count > ENDNUM) {
				exit = 1;
        	}

			if(sim->state->PC == 0)
			{
				exit = 1;
			}

			if(exit == 1) {

				int coveredBB = 0;
				int totalBB = 0;
				for(CFGCollection::BlockIter cfgcbi(cfgs); cfgcbi; cfgcbi++) {
					if(cfgcbi->isBasic()) {
						totalBB++;
						if(COVERED(*cfgcbi))
							coveredBB++;
					}
				}
				elm::cout << "Running " << coveredBB << " BBs of out " << totalBB << " BBs , covering rate = " << ((float)coveredBB / (float)totalBB * 100.0) << " % " << endl;
				elm::cout << "Running " << totalInst << " Insts of out " << totaltotalInst << " Insts , covering rate = " << ((float)totalInst / (float)totaltotalInst * 100.0) << " % " << endl;


				int coverageC = 0;
				for(int i = 0; i < TRICORE_INSTRUCTIONS_NB; i++) {
					// printf("covered %d\n", COVERAGE[i]);
					if(COVERAGE[i])
						coverageC++;
				}
				elm::cout << "Running " << count << " instructions, covering " << coverageC << "/" << TRICORE_INSTRUCTIONS_NB << " instruction set, rate = " << ((float)coverageC / (float)TRICORE_INSTRUCTIONS_NB * 100.0) << endl;
				elm::cout << "verifiedReg = " << verifiedReg << ", verifiedMem = " << verifiedMem << endl;
				elm::cout << "End of the execution" << endl;
				//std::exit(0);
				break;
			}












#ifdef TWIRTEE_FIRST_PART
            if(count == 9961) {
            	unsigned char data_dump = 0;
            	for(unsigned long addr_dump = 0x60000B98; addr_dump < 0x60000B98 + 460; addr_dump++) {
            		tricore_mem_read(sim->state->M, addr_dump, (void*)&data_dump, 1);
            		printf("dump 0x%x:0x%x\n", addr_dump, data_dump);
            	}
            }

            if(count == 18477) {
            	unsigned char data_dump = 0;
            	for(unsigned long addr_dump = 0x600009ac; addr_dump < 0x600009ac + 460; addr_dump++) {
            		tricore_mem_read(sim->state->M, addr_dump, (void*)&data_dump, 1);
            		printf("dump2 0x%x:0x%x\n", addr_dump, data_dump);
            	}
            }
#endif

#ifdef TWIRTEE_SECOND_PART
			if(sim->state->PC == 0x80005322) {
				unsigned char data_dump = 0;
				unsigned long addr_start = 0x60000078; // iterate
				// iteration
				printf("%d iteration 0x", count);
				for(unsigned long addr_dump = addr_start; addr_dump < addr_start + 4; addr_dump++) {
					tricore_mem_read(sim->state->M, addr_dump, (void*)&data_dump, 1);
					printf("%02x", data_dump);
				}

				addr_start = 0x600009a0; // kalman_position
				printf(",0x");
				for(unsigned long addr_dump = addr_start; addr_dump < addr_start + 12; addr_dump++) {
					tricore_mem_read(sim->state->M, addr_dump, (void*)&data_dump, 1);
					printf("%02x", data_dump);
				}

				addr_start = 0x60000994; // position_sp
				printf(",0x");
				for(unsigned long addr_dump = addr_start; addr_dump < addr_start + 12; addr_dump++) {
					tricore_mem_read(sim->state->M, addr_dump, (void*)&data_dump, 1);
					printf("%02x", data_dump);
				}

				addr_start = 0x6000007c; // sgui_out
				printf(",0x");
				for(unsigned long addr_dump = addr_start; addr_dump < addr_start + 12; addr_dump++) {
					tricore_mem_read(sim->state->M, addr_dump, (void*)&data_dump, 1);
					printf("%02x", data_dump);
				}

				addr_start = 0x60000988; // s_out
				printf(",0x");
				for(unsigned long addr_dump = addr_start; addr_dump < addr_start + 12; addr_dump++) {
					tricore_mem_read(sim->state->M, addr_dump, (void*)&data_dump, 1);
					printf("%02x", data_dump);
				}

				printf("\n");
			}

			if(count == 193487525) {
				printf("reaching the first iteration end, quit\n");
				int coverageC = 0;
				for(int i = 0; i < TRICORE_INSTRUCTIONS_NB; i++)
					if(COVERAGE[i])
						coverageC++;
				printf("Covering %d/%d instruction, rate = %f %\n", coverageC, TRICORE_INSTRUCTIONS_NB, ((float)coverageC / (float)TRICORE_INSTRUCTIONS_NB * 100.0));

				return 0;
			}
#endif


			// ==========================================
            tricore_free_inst(inst);
		} // while loop

		/* close loader file */
	    tricore_loader_close(loader);

	    elm::cout << "Finish processing at otawa::tricore16P::CLPSimulator" << endl;


	}


private:
};

int CLPSimulator::BIGNUM = 0x7FFFFFFF; // -1
int CLPSimulator::BIGNUMa = 0;
int CLPSimulator::currPC = 0;
bool CLPSimulator::logging = false;
tricore_sim_t *CLPSimulator::sim = 0;

static void spyx(tricore_memory_t *mem, tricore_address_t addr, tricore_size_t size, tricore_access_t access, void *data) {

	if(!CLPSimulator::logging)
		return;

	if(CLPSimulator::currPC == addr) // do not print the instruction loading
		return;

	uint8_t tmp1 = 0;
	uint8_t tmp2 = 0;
	uint8_t tmp3 = 0;
	uint8_t tmp4 = 0;
	unsigned int result = 0;

	for(int iii = 0; iii < 65536 /* HASHTABLE_SIZE */; iii++) {
		page_entry_t* pagex = CLPSimulator::sim->state->M->hashtable[iii];
		int found = 0;
		while(pagex) {
			if(pagex->addr == (addr & 0xFFFFF000)) {
				found = 1;
				int offset = addr & 0xFFF;
				tmp1 = pagex->storage[MEM_PAGE_SIZE-offset-1];
				tmp2 = pagex->storage[MEM_PAGE_SIZE-offset-1-1];
				tmp3 = pagex->storage[MEM_PAGE_SIZE-offset-2-1];
				tmp4 = pagex->storage[MEM_PAGE_SIZE-offset-3-1];

				if(size == 1)
					result = tmp1;
				else if(size == 2)
					result = tmp1 | (tmp2 << 8);
				else if(size == 3)
					result = tmp1 | (tmp2 << 8) | (tmp3 << 16);
				else if(size == 4)
					result = tmp1 | (tmp2 << 8) | (tmp3 << 16) | (tmp4 << 24);

				break;
			}
			pagex = pagex->next;
		}
		if(found)
			break;
	}


	accessAddrs.add(addr);
	if(debug) {
		if(access == tricore_access_read) {
			elm::cout << "read " << size << " B @ 0x" << hex(addr) << " with data 0x" << hex(result) << endl;
		}
		else {
			elm::cout << "write " << size << " B @ 0x" << hex(addr) << " with data 0x" << hex(result) << endl;
		}
	}
}

p::declare CLPSimulator::reg = p::init("otawa::tricore16P::CLPSimulator", Version(1, 0, 0))
		.base(otawa::Processor::reg)
		.maker<CLPSimulator>()
		.require(COLLECTED_CFG_FEATURE)
		.require(LOOP_INFO_FEATURE)
		.provide(clp::CLP_ANALYSIS_FEATURE);

}}


