#include <otawa/ilp/Constraint.h>

namespace otawa {
namespace tricore16 {

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

}} // end namespace otawa::tricore16
