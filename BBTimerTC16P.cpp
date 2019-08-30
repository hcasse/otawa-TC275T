#include <otawa/parexegraph/GraphBBTime.h>
#include <otawa/parexegraph/ParExeGraph.h>
#include <otawa/proc/ProcessorPlugin.h>
#include <otawa/proc/BBProcessor.h>
#include <otawa/branch/BranchBuilder.h>
#include <otawa/branch/features.h>
#include <otawa/dcache/features.h>
#include <otawa/etime/EdgeTimeBuilder.h>
#include <otawa/etime/features.h>
#include <otawa/cache/cat2/features.h>
#include <otawa/cfg/features.h>
#include <otawa/loader/gliss.h>
#include <elm/data/HashMap.h>
#include "Prefetch.h"
#include "TC16.h"

using namespace otawa;

namespace otawa { namespace tricore16 {

Identifier<int> CORE("otawa::tricore16::CORE", 0);

#define FIRST_EXE_STAGE 0
#define SECOND_EXE_STAGE 1
#define F1_STAGE 1
#define F2_STAGE 2
#define PD_STAGE 3
#define EX2_STAGE 5

#define HISTORY_SIZE 7

/*
 * TIMING of Instructions
 *
 * In the Aurix documentation, each instruction timings are defined
 * by four numbers (lP, lE, rP, rE) that means:
 *   * result latency core P, core E,
 *   * repear rate core P, core E
 *
 * Mapping them to the structure of the pipeline:
 * 	 * core P	MAC					bit processor		ALU
 * 	 			xxx					Address ALU			EA (memory)
 * 	 			xxx					xxx					loop execution
 *	 * core E	MAC					bit processor		ALU
 * 	 			loop execution		Address ALU			EA (memory)
 *
 * These number could be interpreted as follows:
 *	* for madd (multiply and add) operation
 *		input registers read at MAC
 *		output registers written at ALU
 *	* for other integer ALU operation (l latency)
 *		input registers read at ALU - max(l - 2, 2)
 *		output registers written at ALU
 *		if latency > 3 then latency of ALU is l - 2
 *	* for loop instruction
 *		register read/written at corresponding "loop execution" stage
 *	* for load instruction
 *		result register written at EA
 *		other registers read/written at "Address ALU"
 *		memory action is performed at EA
 *	* for store instruction
 *		register containing value to store ar read at EA
 *		other registers read/written at "Address ALU"
 *		memory action is performed at EA
 *	* for non-loop branch instruction
 *		look with branch-prediction
 */


typedef enum {
	IP = 0,
	LS = 1,
	LP = 2,
	FP = 3,
	BR = 4
} pipe_t;





class ExeGraph16P: public etime::EdgeTimeGraph {
public:
	typedef Vector<const hard::Register *> reg_set_t;

	ExeGraph16P(
		WorkSpace *ws,
		ParExeProc *proc,
		elm::Vector<Resource *> *hw_resources,
		ParExeSequence *seq,
		const PropList &props = PropList::EMPTY,
		bool coreE = true)
	: etime::EdgeTimeGraph(ws, proc, hw_resources, seq, props), D(0), A(0), cur_inst(0), PSW(0), _coreE(coreE), ip(0), ls(0), lp(0), fp(0), nca(0), ncai(0), ncaiMax(0)
	{
		info = gliss::INFO(_ws->process());

		// set branch penalty
		// setBranchPenalty(0);
		// FIXME: what is a branch penalty

		// look for register banks
		const hard::Platform::banks_t &banks = ws->process()->platform()->banks();
		for(int i = 0; i < banks.count(); i++) {
			if(banks[i]->name() == "D")
				D = banks[i];
			else if(banks[i]->name() == "A")
				A = banks[i];
			else if(banks[i]->name() == "misc")
				for(int j = 0; j < banks[i]->count(); j++) {
					if(banks[i]->get(j)->name() == "PSW")
						PSW = banks[i]->get(j);
				}
		}
		ASSERT(D);
		ASSERT(A);
		ASSERT(PSW);

		// initialize register table
		regs = new ParExeNode *[_ws->platform()->regCount()];
		for(int i = 0; i < _ws->platform()->regCount(); i++)
			regs[i] = 0;

		// look for FU
		for(ParExePipeline::StageIterator stage(_microprocessor->pipeline()); stage(); stage++)
			if(stage->category() == ParExeStage::EXECUTE)
				for(int i = 0; i < stage->numFus(); i++) {
					ParExePipeline *pfu = stage->fu(i);
					if(pfu->firstStage()->name().startsWith("EXE_I"))
						ip = pfu;
					else if(pfu->firstStage()->name().startsWith("EXE_L"))
						lp = pfu;
					else if(pfu->firstStage()->name().startsWith("EXE_M"))
						ls = pfu;
					else if(pfu->firstStage()->name().startsWith("EXE_F"))
						fp = pfu;
					else
						ASSERTP(false, pfu->firstStage()->name());
				}
		if(_coreE)
			lp = ls;
		ASSERT(ip);
		ASSERT(ls);
		ASSERT(lp);
		ASSERT(fp);

		// compute worst_mem_delay
		mem = hard::MEMORY_FEATURE.get(ws);
		ASSERT(mem);
		//worst_mem_delay = mem->worstAccess();
		worst_store_delay = mem->worstWriteTime();
		worst_load_delay = mem->worstReadTime();
	}

	virtual ~ExeGraph16P(void) { }

	/**
	 * Test if the graph is done for core E processor.
	 * @return	True for core E, false for core P.
	 */
	inline bool isCoreE(void) const { return _coreE; }

	/**
	 */
	virtual ParExePipeline *pipeline(ParExeStage *stage, ParExeInst *inst) {
		switch(tricore_pipe(info, inst->inst())) {
		case IP:	return ip;
		case LS:	return ls;
		case LP:	return lp;
		case FP:	return fp;
		case BR:	return lp;		// TO FIX: no known solution: branches may in any LS, LP or IP pipeline according to availability
		default:	ASSERT(false); return 0;
		}
	}

	/**
	 * Get register from mangled form.
	 */
	void regOf(int r, reg_set_t& set) {
		int type = r >> 4, num = r & 0xf;
		switch(type) {
		case 0:		set.add(D->get(num)); break;
		case 1:		set.add(A->get(num)); break;
		case 2:		set.add(D->get(num)); set.add(D->get((num + 1) & 0xf)); break;
		case 3:		set.add(A->get(num)); set.add(A->get((num + 1) & 0xf)); break;
		default:	ASSERT(false); break;
		}
	}


	virtual void createNodes(void) {
		_resources.setLength(1); // only keep the first resource, the "start" resource
		ParExeGraph::createNodes();
	}

	/**
	 * Set the delay of a stage.
	 * @param stage		Execution stage number to set delay for.
	 * @param delay		Delay in cycles.
	 */
//	void setDelay(int stage, int delay) {
//		findExeAt(cur_inst, stage)->setLatency(delay);
//	}


	void addEdgesForProgramOrder(List<ParExeStage *> *list_of_stages) { // for P-core we need to consider superscalar
		static string program_order("program order");
		static string superscalar("superscalar");
		static string fetch_fifo("fetch fifo");

		int numOfStages = 0;
		for(InstIterator inst(this); inst(); inst++) {
			for(ParExeInst::NodeIterator node(*inst); node(); node++) {
				numOfStages++;
			}
			break;
		}

//		ParExeNode *previous[numOfStages] = { nullptr };
//		ParExeNode *previous2[numOfStages] = { nullptr };

		ParExeNode *previous[HISTORY_SIZE][numOfStages];
		for(int i = 0; i < 6 ; i++)
			for(int j = 0; j < numOfStages ; j++)
				previous[i][j] = nullptr;

		bool prevSuperScalar = false;
		for(InstIterator inst(this); inst(); inst++) {

			int index = 0;
			int superScalar = -1;
			int stage = 0;

			if((superScalar == -1) && previous[0][index] && ((previous[0][index]->inst()->inst()->kind() & 0xE0000000) == 0) && ((inst->inst()->kind() & 0xE0000000) == 0x40000000))
				superScalar = 1;
			else if (superScalar == -1)
				superScalar = 0;


			for(ParExeInst::NodeIterator node(*inst); node(); node++) {
				stage++;

				if(previous[0][index]) {

					if((stage >= PD_STAGE) && (superScalar == 1)) { // LS follows by IP
						new ParExeEdge(previous[0][index], *node, ParExeEdge::SLASHED, 0, superscalar);
						//elm::cout << previous[index]->inst()->inst() << " @ " << previous[index]->inst()->inst()->address() << " has super scalar effect" << endl;
					}
					else {
						if(stage == F1_STAGE) { // F1
							// Donothing, let PFlash analysis handles it
						}
						else if(stage == F2_STAGE) { // F2
							new ParExeEdge(previous[0][index], *node, ParExeEdge::SLASHED, 0, fetch_fifo);
						}
						else {
							new ParExeEdge(previous[0][index], *node, ParExeEdge::SOLID, 0, program_order);
						}
					}

					if(prevSuperScalar && previous[1][index] && (stage > EX2_STAGE))
						new ParExeEdge(previous[1][index], *node, ParExeEdge::SOLID, 0, superscalar);

/*
					// previous 3 E2 -> current PD (the current instruction can enter the PD stage once the 4th instruction prior to the current one has completed its EX2)
					if((stage == 3) && previous[3][5]) {
						new ParExeEdge(previous[3][5], *node, ParExeEdge::SOLID, 0, "cong");
					}

					// previous 6 PD -> current F2 (the current instruction can enter the FIFO once the 7th instruction prior to the current one has completes its PD)
					if((stage == 1) && previous[6][2]) {
						new ParExeEdge(previous[6][2], *node, ParExeEdge::SOLID, 0, "fill");
					}
*/
				}

				for(int i = HISTORY_SIZE - 1; i > 0 ; i--)
					previous[i][index] = previous[i-1][index];
				previous[0][index] = *node;
				index++;
			}

			if(superScalar)
				prevSuperScalar = true;
			else
				prevSuperScalar = false;

		}


		// instruction FIFO
		int stage = 0;
		for(InstIterator inst(this); inst(); inst++) {
			for(ParExeInst::NodeIterator node(*inst); node(); node++) {

			}
		}


	}

	void addEdgesForFetchPrime(void) {
		static string cache_trans_msg = "cache", cache_inter_msg = "line", branch_msg = "branch", cache_intra_msg = "intra";
		static string in_order = "in-order";

		// get fetch stage
		ParExeStage *fetch_stage = _microprocessor->fetchStage();
		ParExeNode * first_cache_line_node = fetch_stage->firstNode();
		ParExeStage *branch_stage = _microprocessor->branchStage();

		// compute current cache line
		Address current_cache_line = 0;
		if(_cache_line_size != 0)
			current_cache_line = first_cache_line_node->inst()->inst()->address().offset() / _cache_line_size;

		bool cached = true;
		ParExeNode *previous = nullptr;
		for (ParExeStage::NodeIterator node(fetch_stage) ; node() ; node++) {
			if (previous){

				// Address addr = inst->address();
				const hard::Bank *bank = mem->get(node->inst()->inst()->address());
				if(!bank)
					elm::cerr << "\t\t\t\t" << "WARNING: no memory bank for code at " << node->inst()->inst()->address() << ": block considered as cached.\n";
				else if(!bank->isCached()) {
					cached = false;
					elm::cerr << "\t\t\t\t" << "Address " << node->inst()->inst()->address() << " not cached. No need to create edge for cache\n";
				}

				// branch case
				if (previous->inst()->inst()->topAddress() != node->inst()->inst()->address()) {

					// look for the branch stage node
					ParExeNode *branching_node = nullptr;
					for(auto node = previous->inst()->nodes(); node(); node++)
						if(node->stage() == branch_stage) {
							branching_node = *node;
							break;
						}
					ASSERT(branching_node != nullptr);

					// create the edges
					if(_branch_penalty && cached)
						new ParExeEdge(branching_node, *node, ParExeEdge::SOLID, _branch_penalty, branch_msg);
	//				if(_cache_line_size != 0 && cached)
	//					new ParExeEdge(first_cache_line_node, *node, ParExeEdge::SOLID, _branch_penalty, comment(cache_inter_msg));
				}

				// no branch
				else {
					// no cache
					if(_cache_line_size == 0) {
						if(previous != nullptr)
							new ParExeEdge(previous, *node, ParExeEdge::SOLID, 0, in_order);
					}
				}

				// cache bound edges
				if (cached) {
					Address cache_line = node->inst()->inst()->address().offset() / _cache_line_size;
					if(cache_line != current_cache_line) {
						new ParExeEdge(first_cache_line_node, *node, ParExeEdge::SOLID, 0, cache_trans_msg); // between the 1st appearence of instructions from different cache set

						if(first_cache_line_node != previous)
							new ParExeEdge(previous, *node, ParExeEdge::SOLID, 0, cache_inter_msg); // the last instruction of the difference cache set to the 1st instruction of the new cache set
						first_cache_line_node = *node;
						current_cache_line = cache_line;
					}
					else // same cache line
						new ParExeEdge(previous, *node, ParExeEdge::SLASHED, 0, cache_intra_msg); // within the same cache set
				}

			}
			previous = *node;
		}
	}



	virtual void addEdgesForFetch(void) {
		// common part
		//ParExeGraph::addEdgesForFetch();
		addEdgesForFetchPrime();






		if(!isCoreE())
			return;

		// core E: simple static branch prediction scheme.
		//
		// Prediction Policy
		// (a) 	16 bit instruction format (either direction) and 32 bit format backwards
		//		conditional branches are predicted taken.
		// (b)	32 bit format forwards conditional branches are predicted not taken.
		//
		// Latencies
		// * correctly predicted, not taken: 1
		// * correctly predicted, taken: 2
		// * incorrectly predicted: 3
		ParExeInst *prev = 0;
		for(InstIterator inst(this); inst(); inst++) {
			if(prev && prev->inst()->isControl()) {
				ot::time delay;

				// type determination
				bool taken =
						   prev->inst()->size() == 2
						|| !prev->inst()->target()		// indirect branch considered mispredicted
						|| prev->inst()->target()->address() < prev->inst()->topAddress();

				// branching
				bool branching = prev->inst()->topAddress() != inst->inst()->address();

				// look prediction
				if(taken != branching)
					delay = 3;		// misprediction
				else if(branching)
					delay = 2;		// branch good prediction
				else
					delay = 1;		// sequence good prediction

				// elm::cout << prev->inst()->address() << " taken = " << taken << ", branching = " << branching << ", cost = " << delay << endl;

				// create the edge
				static string msg = "static branch prediction";
				if(delay >= 2)
					new ParExeEdge(prev->node(2), inst->fetchNode(), ParExeEdge::SOLID, delay - 2, msg);
			}
			prev = *inst;
		}
	}

	virtual void addEdgesForMemoryOrder(void) {
		// not implemented, but used by the void ParExeGraph::build()

	}


	virtual void addEdgesForDataDependencies(void) {
		findDataDependencies();
	}

	virtual void findDataDependencies(void) {

		BasicBlock* currBB = nullptr;
		BasicBlock* prevBB = nullptr;

		for(InstIterator inst(this); inst(); inst++) {
			//cerr << "DEBUG: " << inst->inst() << io::endl;
			if(inst->inst()->isMem()) {
				prevBB = currBB;
				currBB = inst->basicBlock();
				if(prevBB != currBB) {
					Pair<int, dcache::NonCachedAccess *> ncaa = dcache::NC_DATA_ACCESSES(currBB);
					nca = ncaa.snd;
					ncaiMax = ncaa.fst;
					ncai = 0;
				}

				if(inst->inst()->isLoad()) {
					if(inst->inst()->isStore())
						dependenciesForLoadStore(*inst);
					else
						dependenciesForLoad(*inst);
				}
				else
					dependenciesForStore(*inst);
			}
			else if(inst->inst()->getKind().meets(Inst::IS_FLOAT))
				dependenciesForFloat(*inst);
			else if(inst->inst()->isControl()) {
				if(tricore_prod(info, inst->inst(), _coreE) < 0)
					dependenciesForLoop(*inst);
				else
					dependenciesForBranch(*inst);
			}
			else
				dependenciesForALU(*inst);
		}
	}

	/**
	 * Consider that the given node consume all read registers of the instruction.
	 * @param inst		Considered instruction.
	 * @param node		Consuming node.
	 * @param excluded	Register to exclude (once).
	 */
	void consume_edge(Inst *inst, ParExeNode *node, reg_set_t& excluded) {
		const Array<hard::Register *>& reads = inst->readRegs();
		for(int i = 0; i < reads.count(); i++) {
			if(excluded.contains(reads[i])) {
				excluded.remove(reads[i]);
				continue;
			}
			consume_edge(reads[i], node);
		}
	}

	/**
	 * If needed, creates an edge between the producer of reg and the current node.
	 * @param reg	Consumed register.
	 * @param node	Consumer.
	 */
	void consume_edge(const hard::Register *reg, ParExeNode *node) {
		//cerr << "DEBUG:\tconsume " << reg->name() << io::endl;
		ParExeNode *producer = regs[reg->platformNumber()];
		if(producer != NULL) {
			node->addProducer(producer);

			bool edgeExisted = false;
			// only creates edge when necessary
			for(ParExeGraph::Successor succ(producer); succ(); succ++)
				if(*succ == node && succ.edge()->latency() == 0 && succ.edge()->type() == ParExeEdge::SOLID) {
					edgeExisted = true;
				}

			if(!edgeExisted)
				new ParExeEdge(producer, node, ParExeEdge::SOLID, 0, reg->name());
		}
	}

	/**
	 * Consider that the given node produce all writeen registers of the instruction.
	 * @param inst	Considered instruction.
	 * @param node	Producing node.
	 * @param excluded	Register to exclude (once).
	 */
	void produce(Inst *inst, ParExeNode *node, reg_set_t& excluded) {
		const Array<hard::Register *>& writes = inst->writtenRegs();
		for(int i = 0; i < writes.count(); i++) {
			if(excluded.contains(writes[i])) {
				excluded.remove(writes[i]);
				continue;
			}
			produce(writes[i], node);
		}
	}

	/**
	 * Record the producer of a register.
	 * @param reg	Produced register.
	 * @param nod	Producer.
	 */
	void produce(const hard::Register *reg, ParExeNode *node) {
		//cerr << "DEBUG:\tproduce " << reg->name() << io::endl;
		regs[reg->platformNumber()] = node;
	}

	/**
	 * Generate dependencies for float instructions.
	 * @param inst	Concerned instruction.
	 */
	void dependenciesForFloat(ParExeInst *inst) {
		dependenciesForALU(inst);
	}

	/**
	 * Generate dependencies for ALU instructions.
	 * @param inst	Concerned instruction.
	 * The ALU instruction generally have the results ready at the EX1
	 */
	void dependenciesForALU(ParExeInst *inst) {
		int time = tricore_prod(info, inst->inst(), _coreE);
		ParExeNode *cons_node = findExeAt(inst, FIRST_EXE_STAGE); // will consume at the first node

		//ParExeNode *prod_node = findExeAt(inst, min(SECOND_EXE_STAGE, time));
		ParExeNode *prod_node = NULL;
		if(time < 1)
			prod_node = findExeAt(inst, FIRST_EXE_STAGE);
		else
			prod_node = findExeAt(inst, SECOND_EXE_STAGE);

		reg_set_t null;
		consume_edge(inst->inst(), cons_node, null);
		produce(inst->inst(), prod_node, null);
		if(time > 1)
			prod_node->setLatency(time - 1); // EX1 already takes 1 cycle, so the rest will be applied on EX2
	}

	/**
	 * Generate dependencies for load instructions.
	 * @param inst	Concerned instruction.
	 */
	void dependenciesForLoad(ParExeInst *inst) {
		ParExeNode *addr_node = findExeAt(inst, FIRST_EXE_STAGE);
		ParExeNode *mem_node = findExeAt(inst, SECOND_EXE_STAGE);

		// consume registers
		reg_set_t null;
		consume_edge(inst->inst(), addr_node, null);

		// produce registers
		reg_set_t regs;
		regOf(tricore_mreg(info, inst->inst()), regs);
		for(int i = 0; i < regs.length(); i++)
			produce(regs[i], mem_node);
		produce(inst->inst(), mem_node, regs);

		// time
		while((ncaiMax > 0) && (nca[ncai].instruction()->address() < inst->inst()->address()))
			ncai++;

		if((ncai != ncaiMax) && (nca[ncai].instruction()->address() == inst->inst()->address())) {
			// elm::cout << nca[ncai] << " has access cost of " << costOf(nca[ncai].getAddresses()[0], false) << endl;
			mem_node->setLatency(tricore_prod(info, inst->inst(), true)+ costOf(nca[ncai].getAddresses()[0], false));
		}
		else {
			// elm::cout << "The access for inst " << inst->inst() << " @ " << inst->inst()->address() << " can not be identified" << endl;
			mem_node->setLatency(tricore_prod(info, inst->inst(), true)+ worst_load_delay);
		}
	}

	/**
	 * Generate dependencies for store instructions.
	 * @param inst	Concerned instruction.
	 */
	void dependenciesForStore(ParExeInst *inst) {
		ParExeNode *addr_node = findExeAt(inst, FIRST_EXE_STAGE);
		ParExeNode *mem_node = findExeAt(inst, SECOND_EXE_STAGE);

		// consume registers
		reg_set_t regs;
		regOf(tricore_mreg(info, inst->inst()), regs);
		for(int i = 0; i < regs.length(); i++)
			consume_edge(regs[i], mem_node);
		consume_edge(inst->inst(), addr_node, regs);

		// produce registers
		reg_set_t null;
		produce(inst->inst(), addr_node, null);

		// time
		while((ncaiMax > 0) && (nca[ncai].instruction()->address() < inst->inst()->address()))
			ncai++;

		if((ncai != ncaiMax) && (nca[ncai].instruction()->address() == inst->inst()->address())) {
			// elm::cout << nca[ncai] << " has access cost of " << costOf(nca[ncai].getAddresses()[0], true) << endl;
			// mem_node->setLatency(tricore_prod(info, inst->inst(), true)+ costOf(nca[ncai].getAddresses()[0] + 1, true));
		}
		else {
			// elm::cout << "The access for inst " << inst->inst() << " @ " << inst->inst()->address() << " can not be identified" << endl;
			mem_node->setLatency(tricore_prod(info, inst->inst(), true) + worst_store_delay + 1); // need to do Store buffer analysis
		}
	}

	/**
	 * Generate dependencies for load-store instructions.
	 * @param inst	Concerned instruction.
	 */
	void dependenciesForLoadStore(ParExeInst *inst) {
		ParExeNode *addr_node = findExeAt(inst, FIRST_EXE_STAGE);
		ParExeNode *mem_node = findExeAt(inst, SECOND_EXE_STAGE);
		reg_set_t regs;
		regOf(tricore_mreg(info, inst->inst()), regs);
		consume_edge(inst->inst(), addr_node, regs);
		produce(inst->inst(), addr_node, regs);
		for(int i = 0; i < regs.length(); i++)
			produce(regs[i], mem_node);
		ASSERT(0); // check who is a load also a store
	}

	/**
	 * Generate dependencies for loop instructions.
	 * @param inst	Concerned instruction.
	 */
	void dependenciesForLoop(ParExeInst *inst) {
		ParExeNode *node = findExeAt(inst, isCoreE() ? 0 : 2);
		reg_set_t null;
		consume_edge(inst->inst(), node, null);
		produce(inst->inst(), node, null);
	}

	/**
	 * Generate dependencies for branch instructions.
	 * @param inst	Concerned instruction.
	 */
	void dependenciesForBranch(ParExeInst *inst) {
		// TODO
		int time = tricore_prod(info, inst->inst(), _coreE);
		ParExeNode *cons_node = findExeAt(inst, FIRST_EXE_STAGE);
		ParExeNode *prod_node = findExeAt(inst, min(SECOND_EXE_STAGE, time));
		reg_set_t null;
		consume_edge(inst->inst(), cons_node, null);
		prod_node->setLatency(time);
	}

private:
	ParExeNode **regs;
	gliss::Info *info;
	const hard::RegBank *A, *D;
	const hard::Register *PSW;
	otawa::ParExeInst *cur_inst;
	bool _coreE;
	ParExePipeline *ip, *ls, *lp, *fp;
	const hard::Memory *mem;
	int worst_store_delay;
	int worst_load_delay;

	/**
	 * Find a specific execution stage.
	 * @param inst	Instruction to look in.
	 * @param num	Number of the execution stage.
	 */
	ParExeNode *findExeAt(ParExeInst *inst, int num) {
		for(ParExeInst::NodeIterator node(inst); node(); node++) {
			if(node->stage()->isFuStage()) {
				while(num) {
					node++;
					num--;
				}
				return *node;
			}
		}
		ASSERT(false);
		return 0;
	}

	ot::time costOf(Address addr, bool write) {
		const hard::Bank *bank = mem->get(addr);
		if(!bank)
			return write ? mem->worstWriteTime() : mem->worstReadTime();
		return write ? bank->writeLatency() : bank->latency();
	}

	dcache::NonCachedAccess* nca;
	int ncai;
	int ncaiMax;
};

/**
 * Determine the type of core.
 */
//Identifier<int> CORE("otawa::tricore16::CORE", 1);

class BBTimerTC16P: public etime::EdgeTimeBuilder {
public:
	static p::declare reg;
	BBTimerTC16P(void): etime::EdgeTimeBuilder(reg) { }

protected:
	virtual etime::EdgeTimeGraph *make(ParExeSequence *seq) {

		ExeGraph16P *graph = new ExeGraph16P(this->workspace(), _microprocessor, ressources(), seq, _props, core == 0);
		graph->setExplicit(true);
		graph->build();

		return graph;
	}

	// Don't build resources for hardware stages or queues
	virtual void BuildVectorOfHwResources() {
		_hw_resources.add(new StartResource("start", 0)); // build the start resource
	}

	virtual void configure(const PropList& props) {
		etime::EdgeTimeBuilder::configure(props);
		core = CORE(props);
		_props = props;
	}

	virtual void clean(ParExeGraph *graph) {
		delete graph;
	}

private:
	int core;
	PropList _props;
};


p::declare BBTimerTC16P::reg = p::init("otawa::tricore16::BBTimerTC16P", Version(1, 0, 0))
		.base(etime::EdgeTimeBuilder::reg)
		.require(otawa::hard::CACHE_CONFIGURATION_FEATURE)
		//.require(otawa::branch::CONSTRAINTS_FEATURE)
		.require(otawa::gliss::INFO_FEATURE)
		//.require(otawa::tricore16::PREFETCH_CATEGORY_FEATURE)
		.maker<BBTimerTC16P>();


} }	// otawa::tricore16

