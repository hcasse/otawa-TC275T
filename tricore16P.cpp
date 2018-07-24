#include <otawa/parexegraph/GraphBBTime.h>
#include <otawa/parexegraph/ParExeGraph.h>
#include <otawa/proc/ProcessorPlugin.h>
#include <otawa/proc/BBProcessor.h>
#include <otawa/branch/BranchBuilder.h>
#include <otawa/branch/features.h>
#include <otawa/etime/EdgeTimeBuilder.h>
#include <otawa/etime/features.h>
#include <otawa/cache/cat2/features.h>
#include <otawa/cfg/features.h>
#include <otawa/loader/gliss.h>
#include <elm/data/HashMap.h>

using namespace otawa;

#include "prod16P.h"
#include "mreg.h"
#include "pipe.h"

namespace otawa { namespace tricore16P {

typedef enum {
	IP = 0,
	LS = 1,
	LP = 2,
	FP = 3,
	BR = 4
} pipe_t;

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
 *		input registers read at ALU - max(l - 1, 2)
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
	PF_NONE = 0,	// invalid value
	PF_NC = 1,		// not classified
	PF_ALWAYS = 2,	// always pre-fetched
	PF_NEVER = 3	// never pre-fetched
} pf_t;

typedef Pair<Address, pf_t> pf_info_t;
Identifier<pf_t> PREFETCHED("otawa::tricore16P::PREFETECHED", PF_NONE);

io::Output& operator<<(io::Output& out, pf_t p) {
	static cstring msgs[] = {
		"none",
		"not-classified",
		"always",
		"never"
	};
	ASSERT(p >= 0 && p <= PF_NEVER);
	out << msgs[p];
	return out;
}


class PrefetchEvent: public otawa::etime::Event {
public:
	PrefetchEvent(Inst *inst, pf_t prefetch, LBlock *lblock): Event(inst), pf(prefetch), lb(lblock) { }
	// the Event in general is associated with the edge
	// if it is not the case, the Event will be associated with where the instruction
	// belongs to, e.g. the PREFIX or the BLOCK
	// FIXME for the moment being, the 'place' for Event will be NO_PLACE
	virtual etime::kind_t kind(void) const { return etime::FETCH; }
	virtual ot::time cost(void) const { return 0; }		// TO DO
	virtual etime::type_t type(void) const { return etime::LOCAL; /* etime::BLOCK; */ }

	virtual etime::occurrence_t occurrence(void) const {
		switch(pf) {
		case PF_NC:		return etime::SOMETIMES;
		case PF_ALWAYS:	return etime::NEVER;	// this occurrence describes the high time frequency (always pre-feched -> never long time to prefetch)
		case PF_NEVER:	return etime::ALWAYS;	// this occurrence describes the high time frequency (never pre-feched -> always long time to prefetch)
		default:		ASSERT(false); return etime::SOMETIMES;
		}
	}

	virtual cstring name(void) const { return "PFlash Prefetch"; }
	virtual string detail(void) const { return name(); }

	virtual bool isEstimating(bool on) { return on; }

	virtual void estimate(ilp::Constraint *cons, bool on) {
		ASSERT(on);
		ASSERT(MISS_VAR(lb));
		cons->addLeft(1, MISS_VAR(lb));
	}

	virtual int weight(void) const {
		switch(cache::CATEGORY(lb)) {
		case INVALID_CATEGORY:		ASSERT(false); return 0;
		case ALWAYS_HIT:			return 0;
		case FIRST_MISS: {
				Block *b = ENCLOSING_LOOP_HEADER(cache::CATEGORY_HEADER(lb));
				BasicBlock *pbb = b->toBasic();
				if(pbb) return WEIGHT(pbb); else return 1;
			}
		case ALWAYS_MISS:
		case FIRST_HIT:
		case NOT_CLASSIFIED:		return WEIGHT(lb->bb());
		}
	}

private:
	pf_t pf;
	LBlock *lb;
};


class PrefetchCategoryAnalysis: public BBProcessor {
public:
	static p::declare reg;
	PrefetchCategoryAnalysis(void): BBProcessor(reg) {
	}

protected:

	virtual void processBB(WorkSpace *ws, CFG *cfg, Block *b) {

		if(!b->isBasic())
			return;
			// ASSERTP(0, << b << " is not a basic block.");

		BasicBlock *bb = b->toBasic();

		if(bb->isEnd())
			return;


		// get L-Blocks
		// genstruct::AllocatedTable<LBlock* >* blocks = BB_LBLOCKS(bb);
		AllocArray<LBlock* >* blocks = BB_LBLOCKS(bb);

		// process each BB
		for(int i = 0; i < blocks->size(); i++){

			// get LBlock
			LBlock *block = blocks->get(i);
			cache::category_t cat = cache::CATEGORY(block);

			// compute prefetched
			pf_t prefetch_cat = PF_NONE;
			switch(cat) {
			case cache::FIRST_HIT:
			case cache::INVALID_CATEGORY:	ASSERTP(false, "invalid_category found"); break;
			case cache::ALWAYS_HIT:			prefetch_cat = PF_NONE; break;
			case cache::ALWAYS_MISS:
			case cache::FIRST_MISS:
			case cache::NOT_CLASSIFIED:		prefetch_cat = findPrefetchCategory(i, blocks, bb); break;
			}

			// hook it
			if(prefetch_cat != PF_NONE) {
				PREFETCHED(block) = prefetch_cat;
				etime::EVENT(bb).add(new PrefetchEvent(block->instruction(), prefetch_cat, block));
			}
			if(logFor(LOG_BB))
				log << "\t\t\t" << block->address() << "\t" << prefetch_cat << io::endl;
		}
	}

private:

	pf_t join(pf_t p1, pf_t p2) {
		if(p1 == p2)
			return p1;
		else if(p1 == PF_NONE)
			return p2;
		else if(p2 == PF_NONE)
			return p1;
		else
			return PF_NC;
	}

	// pf_t findPrefetchCategory(int i, genstruct::AllocatedTable<LBlock* >* blocks, BasicBlock *bb) {
	pf_t findPrefetchCategory(int i, AllocArray<LBlock* >* blocks, BasicBlock *bb) {
		pf_t prefetch_cat = PF_NONE;
		LBlock *block;

		// find previous LBlock
		if(i != 0) {
			block = blocks->get(i-1);

			//find category of previous LBlock
			cache::category_t cat = cache::CATEGORY(block);
			switch(cat) {
			case cache::INVALID_CATEGORY:
			case cache::FIRST_HIT:			ASSERTP(false, "invalid_category found"); break;
			case cache::ALWAYS_HIT:			prefetch_cat = PF_NEVER; break;
			case cache::ALWAYS_MISS:		prefetch_cat = PF_ALWAYS; break;
			case cache::FIRST_MISS:			prefetch_cat = PF_ALWAYS; break;
			case cache::NOT_CLASSIFIED:		prefetch_cat = PF_NC; break;
			}
		}

		// find last LBlock of previous BBlock
		else {
			for(BasicBlock::EdgeIter edge = bb->ins(); edge; edge++) {

				if(!edge->source()->isBasic())
					continue;

				BasicBlock *inbb = edge->source()->toBasic(); // FIXME: if not a basic block and then...?

				// not in sequence: never prefetched
				if(inbb->topAddress() != bb->address())
					prefetch_cat = join(prefetch_cat, PF_NEVER);

				// else examine what happens before
				else {

					// get last LBlock of previous BB category
					// genstruct::AllocatedTable<LBlock* >* blocks_prev = BB_LBLOCKS(inbb);
					AllocArray<LBlock* >* blocks_prev = BB_LBLOCKS(inbb);
					LBlock *block_prev = blocks_prev->get(blocks_prev->size()-1);
					cache::category_t cat = cache::CATEGORY(block_prev);

					// examine the category
					switch(cat) {
					case cache::FIRST_HIT:
					case cache::INVALID_CATEGORY:	ASSERTP(false, "invalid_category found"); break;
					case cache::ALWAYS_HIT:			prefetch_cat = join(prefetch_cat, PF_NEVER); break;
					case cache::FIRST_MISS:
					case cache::ALWAYS_MISS:		prefetch_cat = join(prefetch_cat, PF_ALWAYS); break;
					case cache::NOT_CLASSIFIED:		prefetch_cat = join(prefetch_cat, PF_NC); break;
					}

				}
			}
		}

		return prefetch_cat;
	}
};

Feature<PrefetchCategoryAnalysis> PREFETCH_CATEGORY_FEATURE("otawa::tricore16P::PREFETCH_CATEGORY_FEATURE");


p::declare PrefetchCategoryAnalysis::reg = p::init("otawa::tricore16P::PrefetchCategoryAnalysis", Version(1, 0, 0))
	.make<PrefetchCategoryAnalysis>()
	.provide(PREFETCH_CATEGORY_FEATURE)
	.require(otawa::ICACHE_CATEGORY2_FEATURE);

class ExeGraph: public etime::EdgeTimeGraph {
public:
	typedef genstruct::Vector<const hard::Register *> reg_set_t;

	ExeGraph(
		WorkSpace *ws,
		ParExeProc *proc,
		elm::Vector<Resource *> *hw_resources,
		ParExeSequence *seq,
		const PropList &props = PropList::EMPTY,
		bool coreE = false)
	: etime::EdgeTimeGraph(ws, proc, hw_resources, seq, props), D(0), A(0), cur_inst(0), PSW(0), _coreE(coreE), ip(0), ls(0), lp(0), fp(0)
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
		for(ParExePipeline::StageIterator stage(_microprocessor->pipeline()); stage; stage++)
			if(stage->category() == ParExeStage::EXECUTE)
				for(int i = 0; i < stage->numFus(); i++) {
					ParExePipeline *pfu = stage->fu(i);
					if(pfu->firstStage()->name().startsWith("IP"))
						ip = pfu;
					else if(pfu->firstStage()->name().startsWith("LP"))
						lp = pfu;
					else if(pfu->firstStage()->name().startsWith("LS"))
						ls = pfu;
					else if(pfu->firstStage()->name().startsWith("FP"))
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
		const hard::Memory *mem = hard::MEMORY(ws);
		ASSERT(mem);
		worst_mem_delay = mem->worstAccess();
	}

	virtual ~ExeGraph(void) { }

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

	/**
	 * Set the delay of a stage.
	 * @param stage		Execution stage number to set delay for.
	 * @param delay		Delay in cycles.
	 */
	void setDelay(int stage, int delay) {
		findExeAt(cur_inst, stage)->setLatency(delay);
	}

	void addEdgesForProgramOrder(List<ParExeStage *> *list_of_stages) {
		ParExeGraph::addEdgesForProgramOrder(list_of_stages);

		// prepare map
		elm::HashMap<ParExeStage *, ParExeNode *> nodes;
		ParExePipeline *pipes[] = { ip, ls, lp, fp };
		for(int i = 0; i < 4; i++) {
			ParExePipeline::StageIterator stage(pipes[i]);
			stage++;
			nodes.put(*stage, 0);
			stage++;
			nodes.put(*stage, 0);
		}

		// add missing edges
		for(InstIterator inst(this); inst; inst++)
			for(ParExeInst::NodeIterator node(inst); node; node++) {
				Option<ParExeNode *> prev = nodes.get(node->stage());
				if(prev) {
					if(*prev)
						new ParExeEdge(*prev, *node, ParExeEdge::SOLID);
					nodes.put(node->stage(), *node);
				}
			}
	}

	void addEdgesForFetch(void) {

		// common part
		ParExeGraph::addEdgesForFetch();
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
		for(InstIterator inst(this); inst; inst++) {
			if(prev && prev->inst()->isControl()) {
				ot::time delay;

				// type determination
				bool taken =
						   inst->inst()->size() == 2
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

				// create the edge
				static string msg = "static branch prediction";
				if(delay >= 2)
					new ParExeEdge(prev->node(1), inst->fetchNode(), ParExeEdge::SOLID, delay - 2, msg);
			}
			prev = *inst;
		}
	}

	virtual void addEdgesForMemoryOrder(void) { }

	virtual void findDataDependencies(void) {
		for(InstIterator inst(this); inst; inst++) {
			//cerr << "DEBUG: " << inst->inst() << io::endl;
			if(inst->inst()->isMem()) {
				if(inst->inst()->isLoad()) {
					if(inst->inst()->isStore())
						dependenciesForLoadStore(inst);
					else
						dependenciesForLoad(inst);
				}
				else
					dependenciesForStore(inst);
			}
			else if(inst->inst()->getKind().meets(Inst::IS_FLOAT))
				dependenciesForFloat(inst);
			else if(inst->inst()->isControl()) {
				if(tricore_prod(info, inst->inst(), _coreE) < 0)
					dependenciesForLoop(inst);
				else
					dependenciesForBranch(inst);
			}
			else
				dependenciesForALU(inst);
		}
	}

	/**
	 * Consider that the given node consume all read registers of the instruction.
	 * @param inst		Considered instruction.
	 * @param node		Consuming node.
	 * @param excluded	Register to exclude (once).
	 */
	void consume(Inst *inst, ParExeNode *node, reg_set_t& excluded) {
		const elm::genstruct::Table<hard::Register *>& reads = inst->readRegs();
		for(int i = 0; i < reads.count(); i++) {
			if(excluded.contains(reads[i])) {
				excluded.remove(reads[i]);
				continue;
			}
			consume(reads[i], node);
		}
	}

	/**
	 * If needed, creates an edge between the producer of reg and the current node.
	 * @param reg	Consumed register.
	 * @param node	Consumer.
	 */
	void consume(const hard::Register *reg, ParExeNode *node) {
		//cerr << "DEBUG:\tconsume " << reg->name() << io::endl;
		ParExeNode *producer = regs[reg->platformNumber()];
		if(producer != NULL) {
			node->addProducer(producer);
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
		const elm::genstruct::Table<hard::Register *>& writes = inst->writtenRegs();
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
	 */
	void dependenciesForALU(ParExeInst *inst) {
		int time = tricore_prod(info, inst->inst(), _coreE);
		ParExeNode *cons_node = findExeAt(inst, max(0, 2 - time));
		ParExeNode *prod_node = findExeAt(inst, 2);
		reg_set_t null;
		consume(inst->inst(), cons_node, null);
		produce(inst->inst(), prod_node, null);
		if(time > 2)
			prod_node->setLatency(time - 1);
	}

	/**
	 * Generate dependencies for load instructions.
	 * @param inst	Concerned instruction.
	 */
	void dependenciesForLoad(ParExeInst *inst) {
		ParExeNode *addr_node = findExeAt(inst, 1);
		ParExeNode *mem_node = findExeAt(inst, 2);

		// consume registers
		reg_set_t null;
		consume(inst->inst(), addr_node, null);

		// produce registers
		reg_set_t regs;
		regOf(tricore_mreg(info, inst->inst()), regs);
		for(int i = 0; i < regs.length(); i++)
			produce(regs[i], mem_node);
		produce(inst->inst(), addr_node, regs);

		// time
		mem_node->setLatency(
			tricore_prod(info, inst->inst(), this)
			+ worst_mem_delay
			- 1);
	}

	/**
	 * Generate dependencies for store instructions.
	 * @param inst	Concerned instruction.
	 */
	void dependenciesForStore(ParExeInst *inst) {
		ParExeNode *addr_node = findExeAt(inst, 1);
		ParExeNode *mem_node = findExeAt(inst, 2);

		// consume registers
		reg_set_t regs;
		regOf(tricore_mreg(info, inst->inst()), regs);
		for(int i = 0; i < regs.length(); i++)
			consume(regs[i], mem_node);
		consume(inst->inst(), addr_node, regs);

		// produce registers
		reg_set_t null;
		produce(inst->inst(), addr_node, null);

		// time
		mem_node->setLatency(
			tricore_prod(info, inst->inst(), this) + 1
			+ worst_mem_delay
			- 1);
	}

	/**
	 * Generate dependencies for store instructions.
	 * @param inst	Concerned instruction.
	 */
	void dependenciesForLoadStore(ParExeInst *inst) {
		ParExeNode *addr_node = findExeAt(inst, 1);
		ParExeNode *mem_node = findExeAt(inst, 2);
		reg_set_t regs;
		regOf(tricore_mreg(info, inst->inst()), regs);
		consume(inst->inst(), addr_node, regs);
		produce(inst->inst(), addr_node, regs);
		for(int i = 0; i < regs.length(); i++)
			produce(regs[i], mem_node);
	}

	/**
	 * Generate dependencies for loop instructions.
	 * @param inst	Concerned instruction.
	 */
	void dependenciesForLoop(ParExeInst *inst) {
		ParExeNode *node = findExeAt(inst, isCoreE() ? 0 : 2);
		reg_set_t null;
		consume(inst->inst(), node, null);
		produce(inst->inst(), node, null);
	}

	/**
	 * Generate dependencies for branch instructions.
	 * @param inst	Concerned instruction.
	 */
	void dependenciesForBranch(ParExeInst *inst) {
		// TODO
	}

private:
	ParExeNode **regs;
	gliss::Info *info;
	const hard::RegBank *A, *D;
	const hard::Register *PSW;
	otawa::ParExeInst *cur_inst;
	bool _coreE;
	ParExePipeline *ip, *ls, *lp, *fp;
	int worst_mem_delay;

	/**
	 * Find a specific execution stage.
	 * @param inst	Instruction to look in.
	 * @param num	Number of the execution stage.
	 */
	ParExeNode *findExeAt(ParExeInst *inst, int num) {
		for(ParExeInst::NodeIterator node(inst); node; node++) {
			if(node->stage()->isFuStage()) {
				while(num) {
					node++;
					num--;
				}
				return node;
			}
		}
		ASSERT(false);
		return 0;
	}

};

/**
 * Determine the type of core.
 */
Identifier<int> CORE("otawa::tricore16P::CORE", 1);

class BBTimer: public etime::EdgeTimeBuilder {
public:
	static p::declare reg;
	BBTimer(void): etime::EdgeTimeBuilder(reg) { }

protected:
	virtual etime::EdgeTimeGraph *make(ParExeSequence *seq) {
		PropList props;

		ExeGraph *graph = new ExeGraph(this->workspace(), _microprocessor, ressources(), seq, props, core == 0);
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
	}

	virtual void clean(ParExeGraph *graph) {
		delete graph;
	}

private:
	int core;
};


p::declare BBTimer::reg = p::init("otawa::tricore16P::BBTimer", Version(1, 0, 0))
		.base(etime::EdgeTimeBuilder::reg)
		.require(otawa::hard::CACHE_CONFIGURATION_FEATURE)
		.require(otawa::branch::CONSTRAINTS_FEATURE)
		.require(otawa::gliss::INFO_FEATURE)
		//.require(otawa::tricore16P::PREFETCH_CATEGORY_FEATURE)
		.maker<BBTimer>();

class Plugin: public ProcessorPlugin {
public:
	//typedef elm::genstruct::Table<AbstractRegistration * > procs_t;

	//Plugin(void): ProcessorPlugin("otawa::tricore16P", Version(1, 0, 0), OTAWA_PROC_VERSION) { }
	//virtual procs_t& processors (void) const { return procs_t::EMPTY; };

	typedef elm::genstruct::Table<AbstractRegistration *> procs_t;

	Plugin(void): ProcessorPlugin(make("otawa::tricore16P", OTAWA_PROC_VERSION)
		.version(1, 0, 0)
		.description("timing analyses for TriCore")
		.license(Manager::copyright)) {
	}

};

} }	// otawa::tricore16P

otawa::tricore16P::Plugin otawa_tricore16P;
ELM_PLUGIN(otawa_tricore16P, OTAWA_PROC_HOOK);



