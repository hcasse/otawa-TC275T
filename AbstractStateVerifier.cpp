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

Vector<Address> modifiedAddress;
bool modifiedAddressb = false;


unsigned long ENDNUM = 0xFFFFFFFF;
static void spyx(tricore_memory_t *mem, tricore_address_t addr, tricore_size_t size, tricore_access_t access, void *data);

class AbstractStateVerifier: public otawa::Processor {
public:
	static p::declare reg;
	AbstractStateVerifier(void): otawa::Processor(reg) { }

	static int BIGNUM;
	static int BIGNUMa;
	static int currPC;
	static tricore_sim_t *sim;

protected:

	virtual void configure(const PropList& props) {
		otawa::Processor::configure(props);
	}

	virtual void processWorkSpace(WorkSpace *fw) {
		unsigned int verifiedReg = 0;
		unsigned int verifiedMem = 0;




		const CFGCollection *cfgs = INVOLVED_CFGS(fw);
		ASSERT(cfgs);

		bool first = true;
		Address addressToTrigger = 0;
		CFG* working_cfg = 0;
		BasicBlock* working_bb = 0;
		bool enterNewBB = true;
		Inst* working_inst = 0;

		// Visit CFG
		for(CFGCollection::Iter cfg(cfgs); cfg(); cfg++) {
			if (first) {
				first = false;
				addressToTrigger = cfg->address();
				working_cfg = *cfg;
				working_bb = cfg->entry()->outs()->target()->toBasic();
				working_inst = working_bb->first();
			}
			elm::cout << "\tprocess CFG " << cfg->label() << io::endl;
			for(CFG::BlockIter bb = cfg->blocks(); bb(); bb++) {
				elm::cout << "\t\tprocess " << *bb << io::endl;
				clp::State clpState = clp::STATE_IN(*bb);

				elm::cout << "\t\t\t";
				clpState.print(elm::cout, workspace()->platform());
				elm::cout << io::endl;
			}
		}

		elm::cout << "Address to trigger = " << addressToTrigger << endl;


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


            unsigned int oldPC = sim->state->PC;



            tricore_step(sim);
            currPC = sim->state->PC;




            if(!following && (inst->addr == addressToTrigger.offset())) {
            	following = true;
            	modifiedAddressb = true;
            }

            if(following) {

            	// output the instruction info
            	elm::cout << __GREEN__ << "executing " << Address(inst->addr) << " towards to " << working_bb->last()->address() << " ";
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



            	// verify
                clp::Manager::step_t step;
            	if(enterNewBB) {
            		enterNewBB = false;
            		// start CLP manager
            		step = clpManager.start(working_bb);
            		elm::cout << __CYAN__ << "restart clpManager with state:";
    				clp::State clpState = clp::STATE_IN(working_bb);
    				clpState.print(elm::cout, workspace()->platform());
    				elm::cout << __RESET__ << io::endl;

            	}

    			sem::Block sb;
    			working_inst->semInsts(sb);
    			if(sb.count() == 0) {
        			tricore_disasm(buffer, inst);
        			elm::cerr << __SOURCE_INFO__ << __YELLOW__ << "No semantic instruction for " << buffer << ", of type " << (int)(inst->ident) << " .... program terminates" << __RESET__ << endl;
        			std::exit(1);
        		}
#ifdef PRINT_EACH_STEP
    			sb.print(elm::cout, workspace()->platform());
#endif

        		Inst* currentInst = clpManager.inst();
        		clp::State currentCLPState;
				while(step) {
//#define PRINT_EACH_STEP
#ifdef PRINT_EACH_STEP
					clpManager.sem().print(elm::cout, workspace()->platform());
					elm::cout << " for instruction " << clpManager.inst() << endl;
					elm::cout << __CYAN__ << "    ";
					clpManager.state()->print(elm::cout, workspace()->platform());
					elm::cout << __RESET__ << endl;
#endif
					currentCLPState = *(clpManager.state());
					step = clpManager.next();
					if(step & clp::Manager::NEW_INST)
						break;
				}


//				for(Vector<Address>::Iter vai(modifiedAddress); vai; vai++)
//					elm::cout << "need to check " << vai << endl;
				elm::cout << "need to check " <<  modifiedAddress.count() << " nodes" << endl;

        		// check actual state vs abstract state at the end of each instruction
				for(clp::State::Iter clpsi(currentCLPState); clpsi(); clpsi++) {
					clp::Value actualValue;
					if(clpsi.id().kind() == clp::REG) {
						int regID = clpsi.id().lower();
						if(regID >= 0 && regID <= 15) // Data register
							actualValue = clp::Value(sim->state->D[regID]);
						else if(regID >= 16 && regID <= 31) // Address register
							actualValue = clp::Value(sim->state->A[regID-16]);
						else if(regID == 32) // PSW register
							actualValue = clp::Value(sim->state->PSW);
						else if(regID == 33) // PSW register
							actualValue = clp::Value(sim->state->PC);
						else if(regID == 34) // FCX register
							continue; // actualValue = clp::Value(sim->state->FCX);
						else
							continue;

						// now see if the actual value falls into the abstract domain
						clp::Value actualValue2 = actualValue; // need another value because inter changes the calling object
						if(actualValue2.inter(*clpsi) != actualValue) {
								elm::cerr << "REG " << workspace()->process()->platform()->findReg(regID) << ", actual value = " << actualValue << ", abstract Value = " << (*clpsi) << ", actualValue2 = " << actualValue2 << endl;
								elm::cerr << __GREEN__ << "executing " << Address(inst->addr) << " @ CFG " << working_cfg->index() << " BB " << working_bb->index() << " ";
				                tricore_disasm(buffer, inst);
				                elm::cerr << buffer << " @ " << count << " => ";
				                unsigned int codes = 0;
				                unsigned int codesX = 0;
				                tricore_mem_read(sim->state->M, inst->addr, (void*)&codes, 4);
				                for(int wis = 0; wis < working_inst->size(); wis++) {
				                	int shift = (8*(working_inst->size() - wis - 1));
				                	elm::cerr <<hex((codes >> shift) &0xFF)<< " ";
				                }
				                elm::cerr << __RESET__ << io::endl;
								std::exit(1);
						}
						verifiedReg++;
					}
					else { // if it is memory
						// First need to check the size of the abstract domain
						clp::Value abstractValue = *clpsi;
						unsigned int abstractValueMax = abstractValue.upper();
						int byteCount = 0;
						if (abstractValueMax > 0x1000000)
							byteCount = 4;
						else if(abstractValueMax > 0x10000)
							byteCount = 3;
						else if(abstractValueMax > 0x100)
							byteCount = 2;
						else
							byteCount = 1;

						unsigned int abstractValueMin = abstractValue.lower();
						int byteCount2 = 0;
						if (abstractValueMin > 0x1000000)
							byteCount2 = 4;
						else if(abstractValueMin > 0x10000)
							byteCount2 = 3;
						else if(abstractValueMin > 0x100)
							byteCount2 = 2;
						else
							byteCount2 = 1;

						if(byteCount2 > byteCount) {
							byteCount = byteCount2;
							elm::cout << "for address 0x" << hex(clpsi.id().lower()) << ", abstractValueMin = 0x" << hex(abstractValueMin) << ", byteCount = " << byteCount << endl;
						}
						else {
							elm::cout << "for address 0x" << hex(clpsi.id().lower()) << ", abstractValueMax = 0x" << hex(abstractValueMax) << ", byteCount = " << byteCount << endl;
						}



						unsigned int memoryContent = 0;
						unsigned int memoryAddress = clpsi.id().lower();
						tricore_mem_read(sim->state->M, memoryAddress, (void*)&memoryContent, byteCount);
						actualValue = clp::Value(memoryContent);
						clp::Value actualValue2 = actualValue;
						if(actualValue2.inter(*clpsi) != actualValue) {
								elm::cerr << "not match M[0x" << hex(memoryAddress) << "]  actual value = " << actualValue << ", abstract Value = " << (*clpsi) << endl;
								elm::cerr << __GREEN__ << "executing " << Address(inst->addr) << " @ CFG " << working_cfg->index() << " BB " << working_bb->index() << " ";
				                tricore_disasm(buffer, inst);
				                elm::cerr << buffer << " @ " << count << " => ";
				                unsigned int codes = 0;
				                unsigned int codesX = 0;
				                tricore_mem_read(sim->state->M, inst->addr, (void*)&codes, 4);
				                for(int wis = 0; wis < working_inst->size(); wis++) {
				                	int shift = (8*(working_inst->size() - wis - 1));
				                	elm::cerr <<hex((codes >> shift) &0xFF)<< " ";
				                }
				                elm::cerr << __RESET__ << io::endl;
				                elm::cerr << "byteCount = " << byteCount << endl;
								std::exit(1);
						}
						verifiedMem++;
					}
				}



            	if(Address(inst->addr) == working_bb->last()->address())  { // reacheds the end of BB
    				// need to decide the following block
    				Address nextAddress = Address(sim->state->PC);
    				for (Block::EdgeIter outEdge = working_bb->outs(); outEdge(); outEdge++) {
    					if(outEdge->target()->isSynth()) {
    						working_cfg = outEdge->target()->toSynth()->callee();
    						working_bb = working_cfg->entry()->outs()->target()->toBasic();
    						working_inst = working_bb->first();
    						elm::cout << "Making a call CFG " << working_bb->cfg()->index() << " BB " << working_bb->index() << endl;
    					}
    					else if (outEdge->target()->isExit()) {
    						elm::cout << "Leaving a call" << endl;
    						for(CFG::CallerIter ci(outEdge->target()->cfg()->callers()); ci(); ci++) {
    							for (Block::EdgeIter ciOut = ci->outs(); ciOut(); ciOut++) {
    								if(ciOut->target()->isSynth()) {
    									elm::cout << "Sequential call!" << endl;
    								}
    								else if (ciOut->target()->isExit()) {
    									elm::cout << "Sequential exit!" << endl;
    								}
    								else if(ciOut->target()->toBasic()->address() == nextAddress) {
    									working_bb = ciOut->target()->toBasic();
    									working_cfg = working_bb->cfg();
    									working_inst = working_bb->first();
    									break;
    								}
    							}
    						}
    					}
    					else {
    						if(outEdge->target()->toBasic()->first()->address() == nextAddress) {
    							working_bb = outEdge->target()->toBasic();
    							working_inst = working_bb->first();
    							elm::cout << __RED__ << "    continue to a normal basic block CFG " << working_bb->cfg()->index() << " BB " << working_bb->index() << __RESET__ << endl;
    							break;
    						}
    					}
    				}
    				enterNewBB = true;
            	} // reaching the end of BB
            	else {
            		working_inst = working_inst->nextInst();
            	}



            	if(Address(sim->state->PC) != 0)
            		ASSERTP((Address(sim->state->PC) >= working_bb->address()) && (Address(sim->state->PC) <= working_bb->last()->address()), "Address " << Address(sim->state->PC) << " is out of the current BB: CFG " << working_cfg->index() << " " << working_bb);
            } // if following











            if(count >= BIGNUM) {
				printf("execute %d %08x", count, oldPC);
				tricore_used_regs_read_t rds, wrs;
				tricore_used_regs(inst, rds, wrs);
				printf(" rd:");
				int init = 1;
				for(int i = 0; i < TRICORE_REG_READ_MAX; i++) {
					if(rds[i] == -1 ) break;
					if(rds[i] >= 12 && rds[i] <= 27) {
						if(init) {init = 0; } else { printf(","); }
						printf("a%d[%08x]", rds[i]-12, sim->state->A[rds[i]-12]);
					}
					else if(rds[i] >= 32 && rds[i] <= 47) {
						if(init) {init = 0; } else { printf(","); }
						printf("d%d[%08x]", rds[i]-32, sim->state->D[rds[i]-32]);
					}
				}

				printf(" wb:");
				init = 1;
				for(int i = 0; i < TRICORE_REG_WRITE_MAX; i++) {
					if(wrs[i] == -1 ) break;
					if(wrs[i] >= 12 && wrs[i] <= 27) {
						if(init) {init = 0; } else { printf(",");}
						printf("a%d[%08x]", wrs[i]-12, sim->state->A[wrs[i]-12]);
					}
					else if(wrs[i] >= 32 && wrs[i] <= 47) {
						if(init) {init = 0; } else { printf(","); }
						printf("d%d[%08x]", wrs[i]-32, sim->state->D[wrs[i]-32]);
					}
				}

				printf(" psw:");
				printf("%08x", sim->state->PSW);

				printf("\n");
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

        	count++;
        	BIGNUMa = count;

			int exit = 0;
        	if(count > ENDNUM) {
				exit = 1;
        	}

			if(sim->state->PC == 0)
			{
				exit = 1;
			}

			if(exit == 1) {
				int coverageC = 0;
				for(int i = 0; i < TRICORE_INSTRUCTIONS_NB; i++) {
					// printf("covered %d\n", COVERAGE[i]);
					if(COVERAGE[i])
						coverageC++;
				}
				elm::cout << "Running " << count << " instructions, covering " << coverageC << "/" << TRICORE_INSTRUCTIONS_NB << " instruction set, rate = " << ((float)coverageC / (float)TRICORE_INSTRUCTIONS_NB * 100.0) << endl;
				elm::cout << "verifiedReg = " << verifiedReg << ", verifiedMem = " << verifiedMem << endl;
				elm::cout << "End of the execution" << endl;
				std::exit(0);
			}

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

#ifdef CALL_LEVELS
			if(inst->ident == TRICORE_CALLA_08X) {
				levelx++; for(int i = 0; i < levelx; i++) printf("  "); printf("call to 0x%X\n", sim->state->PC);
			}
			else if(inst->ident == TRICORE_CALLI_AD ) {
				levelx++; for(int i = 0; i < levelx; i++) printf("  "); printf("call to 0x%X\n", sim->state->PC);
			}
			else if(inst->ident == TRICORE_CALL_08X_0 ) {
				levelx++; for(int i = 0; i < levelx; i++) printf("  "); printf("call to 0x%X\n", sim->state->PC);
			}
			else if(inst->ident == TRICORE_CALL_08X ) {
				levelx++; for(int i = 0; i < levelx; i++) printf("  "); printf("call to 0x%X\n", sim->state->PC);
			}

			else if(inst->ident == TRICORE_RET  ) {
				levelx--;
			}
			else if(inst->ident == TRICORE_RET_0  ) {
				levelx--;
			}
#endif

			// ==========================================
            tricore_free_inst(inst);
		}

		/* close loader file */
	    tricore_loader_close(loader);




	}


private:
};

int AbstractStateVerifier::BIGNUM = 200000000; // -1
int AbstractStateVerifier::BIGNUMa = 0;
int AbstractStateVerifier::currPC = 0;
tricore_sim_t *AbstractStateVerifier::sim = 0;

static void spyx(tricore_memory_t *mem, tricore_address_t addr, tricore_size_t size, tricore_access_t access, void *data) {

//	if(AbstractStateVerifier::BIGNUMa < AbstractStateVerifier::BIGNUM)
//		return;
//
//	if(AbstractStateVerifier::currPC == addr) // do not print the instruction loading
//		return;

	uint8_t tmp1 = 0;
	uint8_t tmp2 = 0;
	uint8_t tmp3 = 0;
	uint8_t tmp4 = 0;
	unsigned int result = 0;

	for(int iii = 0; iii < 65536 /* HASHTABLE_SIZE */; iii++) {
		page_entry_t* pagex = AbstractStateVerifier::sim->state->M->hashtable[iii];
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


	if(access == tricore_access_read)
		{} // printf("read %dB @ 0x%X with data 0x%X\n", size, addr, result);
	else {
		printf("write %dB @ 0x%X with data 0x%X\n", size, addr, result);
		if(modifiedAddressb)
			modifiedAddress.add(Address(addr));
	}
}

p::declare AbstractStateVerifier::reg = p::init("otawa::tricore16::AbstractStateVerifier", Version(1, 0, 0))
		.base(otawa::Processor::reg)
//		.require(otawa::hard::CACHE_CONFIGURATION_FEATURE)
//		.require(otawa::branch::CONSTRAINTS_FEATURE)
//		.require(otawa::gliss::INFO_FEATURE)
		.maker<AbstractStateVerifier>();

}}


