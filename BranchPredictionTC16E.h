#ifndef __BRANCH_PREDICTION_TC16E_H__
#define __BRANCH_PREDICTION_TC16E_H__

#include <otawa/branch/features.h>
#include <otawa/hard/BHT.h>
#include <otawa/proc/BBProcessor.h>


namespace otawa { namespace tricore16 {

class BranchPreductionTC16E: public otawa::BBProcessor {

public:
	static p::declare reg;
	BranchPreductionTC16E(void);

protected:
	virtual void processBB(WorkSpace *ws, CFG *cfd, Block *bb);


};


}
}

#endif
