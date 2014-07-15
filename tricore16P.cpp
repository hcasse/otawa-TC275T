#include <otawa/parexegraph/GraphBBTime.h>
#include <otawa/parexegraph/ParExeGraph.h>
#include <otawa/proc/ProcessorPlugin.h>
#include <otawa/branch/BranchBuilder.h>
#include <otawa/util/LBlockBuilder.h>
#include <otawa/branch/features.h>
#include <otawa/etime/EdgeTimeBuilder.h>
#include <otawa/etime/features.h>
#include <otawa/cache/cat2/features.h>
#include <otawa/cfg/features.h>


#include "prod16P.h"

using namespace otawa;

namespace continental { namespace tricore16P {



typedef enum {
	PF_NONE = 0,	// invalid value
	PF_NC = 1,		// not classified
	PF_ALWAYS = 2,	// always pre-fetched
	PF_NEVER = 3	// never pre-fetched
} pf_t;

typedef Pair<Address, pf_t> pf_info_t;
Identifier<pf_t> PREFETCHED("continental::tricore16P::PREFETECHED", PF_NONE);

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

	virtual etime::kind_t kind(void) const { return etime::FETCH; }
	virtual ot::time cost(void) const { return 0; }		// TO DO
	virtual etime::type_t type(void) const { return etime::BLOCK; }

	virtual etime::occurrence_t occurrence(void) const {
		switch(pf) {
		case PF_NC:		return etime::SOMETIMES;
		case PF_ALWAYS:	return etime::ALWAYS;
		case PF_NEVER:	return etime::NEVER;
		default:		ASSERT(false); return etime::SOMETIMES;
		}
	}

	virtual cstring name(void) const { return "PFlash Prefetch"; }

	virtual bool isEstimating(bool on) { return on; }

	virtual void estimate(ilp::Constraint *cons, bool on) {
		ASSERT(on);
		ASSERT(MISS_VAR(lb));
		cons->addLeft(1, MISS_VAR(lb));
	}

	virtual int weight(void) const {
		switch(otawa::CATEGORY(lb)) {
		case INVALID_CATEGORY:		ASSERT(false); return 0;
		case ALWAYS_HIT:			return 0;
		case FIRST_MISS:
			{ BasicBlock *pbb = ENCLOSING_LOOP_HEADER(otawa::CATEGORY_HEADER(lb)); if(pbb) return WEIGHT(pbb); else return 1; }
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

	virtual void processBB(WorkSpace *ws, CFG *cfg, BasicBlock *bb) {
		if(bb->isEnd())
			return;

		// get L-Blocks
		genstruct::AllocatedTable<LBlock* >* blocks = BB_LBLOCKS(bb);

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

	pf_t findPrefetchCategory(int i, genstruct::AllocatedTable<LBlock* >* blocks, BasicBlock *bb) {
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

			for(BasicBlock::InIterator edge(bb); edge; edge++) {
				BasicBlock *inbb = edge->source();

				// not in sequence: never prefetched
				if(inbb->topAddress() != bb->address())
					prefetch_cat = join(prefetch_cat, PF_NEVER);

				// else examine what happens before
				else {

					// get last LBlock of previous BB category
					genstruct::AllocatedTable<LBlock* >* blocks_prev = BB_LBLOCKS(inbb);
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

Feature<PrefetchCategoryAnalysis> PREFETCH_CATEGORY_FEATURE("continental::tricore16P::PREFETCH_CATEGORY_FEATURE");


p::declare PrefetchCategoryAnalysis::reg = p::init("continental::tricore16P::PrefetchCategoryAnalysis", Version(1, 0, 0))
	.maker<PrefetchCategoryAnalysis>()
	.base(BBProcessor::reg)
	.provide(PREFETCH_CATEGORY_FEATURE)
	.require(otawa::ICACHE_CATEGORY2_FEATURE);


class ExeGraph: public ParExeGraph {
public:
	ExeGraph(
		WorkSpace *ws,
		ParExeProc *proc,
		ParExeSequence *seq,
		const PropList &props = PropList::EMPTY): otawa::ParExeGraph(ws, proc, seq, props)
	{
		this->setBranchPenalty(0);
	}

	virtual ~ExeGraph(void) { }

	virtual void findDataDependencies(void) {
		gliss::Info *info = gliss::INFO(_ws->process());
		
		// initialize register table
		regs = new ParExeNode *[_ws->platform()->regCount()];
		for(int i = 0; i < _ws->platform()->regCount(); i++)
			regs[i] = 0;
		
		// build the producer list
		for(InstIterator inst(this); inst; inst++) {

			// process read registers
			const elm::genstruct::Table<hard::Register *>& reads = inst->inst()->readRegs();
			for(int i = 0; i < reads.count(); i++) {
				ParExeNode *producer = regs[reads[i]->platformNumber()];
				if(producer != NULL) {
					findExe(inst)->addProducer(producer);
					new ParExeEdge(producer, findExe(inst), ParExeEdge::SOLID);
				}
			}
			
			// apply production on register table
			int wstage = tricore_prod(info, inst->inst());
			const elm::genstruct::Table<hard::Register *>& writes = inst->inst()->writtenRegs();
			for(int i = 0; i < writes.count(); i++)
				regs[writes[i]->platformNumber()] = findExe(inst, wstage); 
		}
	}

private:
	ParExeNode **regs;

	ParExeNode *findExe(ParExeInst *inst) {
		for(InstNodeIterator node(inst); node; node++) {
			string name = node->stage()->name();
			if(name == "Integer Unit3" || name == "Loop Pipeline Unit3" || name == "Load Store Unit3" || name == "Float Point Unit3"){
				return node;
			}
		}
		ASSERT(false);
		return 0;
	}
	
	ParExeNode *findExe(ParExeInst *inst, int num) {
		gliss::Info *info = gliss::INFO(_ws->process());
		for(InstNodeIterator node(inst); node; node++) {
			string name = node->stage()->name();
			if(name == "Integer Unit3" || name == "Loop Pipeline Unit3" || name == "Load Store Unit3" || name == "Float Point Unit3") {
				int wstage = tricore_prod(info, inst->inst());
				
				if(wstage > 1) {
					node++;
					node->setLatency (node->latency()+(wstage - 1));
					return node;
				}
				else{
					while(num) {
						node++;
						num--;
					}
				}
				return node;
			}
		}
		ASSERT(false);
		return 0;
	}
};


class BBTimer: public etime::EdgeTimeBuilder {
public:
	static p::declare reg;
	BBTimer(void): etime::EdgeTimeBuilder(reg) { }

protected:
	virtual ParExeGraph *make(ParExeSequence *seq) {
		PropList props;
		ParExeGraph *graph = new ExeGraph(this->workspace(), _microprocessor, seq, props);
		graph->build(seq);
		return graph;
	}

	virtual void clean(ParExeGraph *graph) {
		delete graph;
	}

};

p::declare BBTimer::reg = p::init("continental::tricore16P::BBTimer", Version(1, 0, 0))
		.base(etime::EdgeTimeBuilder::reg)
		.require(otawa::hard::CACHE_CONFIGURATION_FEATURE)
		.require(otawa::branch::CONSTRAINTS_FEATURE)
		.require(otawa::gliss::INFO_FEATURE)
		//.require(continental::tricore16P::PREFETCH_CATEGORY_FEATURE)
		.maker<BBTimer>();

class Plugin: public ProcessorPlugin {
public:
	typedef elm::genstruct::Table<AbstractRegistration * > procs_t;

	Plugin(void): ProcessorPlugin("continental::tricore16P", Version(1, 0, 0), OTAWA_PROC_VERSION) { }
	virtual procs_t& processors (void) const { return procs_t::EMPTY; };
};

} }	// continental::tricore16P

continental::tricore16P::Plugin OTAWA_PROC_HOOK;
continental::tricore16P::Plugin& contintal_tricore = OTAWA_PROC_HOOK;
