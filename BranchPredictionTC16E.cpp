
#include "BranchPredictionTC16E.h"


using namespace otawa;

namespace otawa { namespace tricore16 {


BranchPreductionTC16E::BranchPreductionTC16E(void): otawa::BBProcessor(reg) { }

void BranchPreductionTC16E::processBB(WorkSpace *ws, CFG *cfd, Block *b) {
	if(b->isBasic()) {
		BasicBlock *bb = b->toBasic();
		Inst *binst = bb->control();
		if(binst && binst->isConditional()) {

			otawa::branch::category_t cat;
			elm::cout << binst << " is with size = " << binst->size() << endl;

			if(binst->size() == 2) {
				cat = otawa::branch::STATIC_TAKEN;

			}
			else {
				if(binst->target()->address() < binst->address())
					cat = otawa::branch::STATIC_TAKEN;
				else
					cat = otawa::branch::STATIC_NOT_TAKEN;
			}
			otawa::branch::CATEGORY(bb) = cat;
		} // end of if(binst && binst->isConditional()) {
	}
}

p::declare BranchPreductionTC16E::reg = p::init("otawa::tricore16::BranchPreductionTC16E", Version(1, 0, 0))
		.base(otawa::BBProcessor::reg)
		.require(hard::BHT_FEATURE) // read the BHT configuration
		.provide(otawa::branch::CONSTRAINTS_FEATURE)
		.maker<BranchPreductionTC16E>();


}}
