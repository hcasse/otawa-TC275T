#include <otawa/proc/BBProcessor.h>
#include <otawa/cache/features.h>
#include <otawa/cache/cat2/features.h>
#include <otawa/etime/features.h>
#include "Prefetch.h"
#include "PrefetchEvent.h"

using namespace otawa;
using namespace otawa::cache;

namespace otawa {
namespace tricore16P {

Identifier<pf_t> PREFETCHED("otawa::tricore16P::PREFETECHED", PF_NONE);

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
			case cache::ALWAYS_HIT:			prefetch_cat = PF_NONE; break; // hit in cache, no need to load from P-FLASH
			case cache::ALWAYS_MISS:
			case cache::FIRST_MISS:
			case cache::NOT_CLASSIFIED:		prefetch_cat = findPrefetchCategory(i, blocks, bb); break;
			}

			// hook the prefetch event which will be used by the BB time.
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


}}
