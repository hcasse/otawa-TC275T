//#include <otawa/cache/features.h>
#include <otawa/prop/PropList.h>
//#include <otawa/dfa/BitSet.h>
//#include <otawa/hard/Cache.h>
#include <otawa/flowfact/features.h>
//#include <otawa/hard/CacheConfiguration.h>
//#include <otawa/hard/Cache.h>
//#include <otawa/hard/Memory.h>
//#include <otawa/hard/Platform.h>
#include <otawa/prog/WorkSpace.h>
#include <otawa/prog/Process.h>
//#include <otawa/dcache/AddressExtracter.h>

#include <otawa/proc/BBProcessor.h>
#include <otawa/data/clp/features.h>
#include <otawa/willie.h>

using namespace otawa::clp;
using namespace elm::io;

namespace otawa { namespace tricore16P {

Identifier<bool> REWIND("REWIND", false);

// AddressExtracter class
class AddressExtracter: public BBProcessor {
public:
	static p::declare reg;
	AddressExtracter(p::declare& r = reg);

protected:
	virtual void setup(WorkSpace *ws);
	virtual void cleanup(WorkSpace *ws);
	virtual void processBB(WorkSpace *ws, CFG *cfg, otawa::Block *bb);
	virtual void processWorkSpace(WorkSpace *fw);

private:
	const hard::Memory *mem;
	clp::Manager *man;

	int loadC;
	int storeC;
	int loadCT;
	int storeCT;
};

p::declare AddressExtracter::reg = p::init("otawa::tricore16P::AddressExtracter", Version(1, 0, 0))
	.base(BBProcessor::reg)
	.maker<AddressExtracter>()
//	.provide(DATA_BLOCK_FEATURE)
//	.provide(CLP_BLOCK_FEATURE)
//	.require(otawa::clp::CLP_ANALYSIS_FEATURE)
//	.require(hard::CACHE_CONFIGURATION_FEATURE)
//	.require(hard::MEMORY_FEATURE);
	;


/**
 * This feature ensures that accessed data blocks have been built for data cache analysis based
 * on the reference obtaine from CLP analysis.
 *
 * @par Default Processor
 * @li @ref AddressExtracter
 *
 * @see clp
 * @ingroup dcache
 */
p::feature ADDRESS_EXTRACTION_FEATURE("otawa::tricore16P::ADDRESS_EXTRACTION_FEATURE", new Maker<AddressExtracter>());


/**
 */
AddressExtracter::AddressExtracter(p::declare& r): BBProcessor(r), mem(0), man(0) {
	loadC = 0;
	storeC = 0;
	loadCT = 0;
	storeCT = 0;
}


/**
 */
void AddressExtracter::setup(WorkSpace *ws) {
	// allocate the manager
	man = new clp::Manager(ws);
}


/**
 */
void AddressExtracter::cleanup(WorkSpace *ws) {
	if(man)
		delete man;
}


void AddressExtracter::processWorkSpace(WorkSpace *fw) {
	BBProcessor::processWorkSpace(fw);
	elm::cout << __SOURCE_INFO__ << "Finishing processing otawa::tricore16P::AddressExtracter" << endl;
	elm::cout << __SOURCE_INFO__ << "total access: " << (loadC + storeC) << " L: " << loadC << "(" << (loadC*100/(loadC+storeC)) << "%), S: " << storeC << "(" << (storeC*100/(loadC+storeC)) << "%)" << endl;
	elm::cout << __SOURCE_INFO__ << "total access to top: " << (loadCT + storeCT) << "(" << ((loadCT + storeCT)*100/(loadC+storeC)) << "%), L: " << loadCT << "(" << ((loadCT + storeCT)==0?0:(loadCT*100/(loadCT+storeCT))) << "%), S: " << storeCT << "(" << ((loadCT + storeCT)==0?0:(storeCT*100/(loadCT+storeCT))) << "%)" << endl;
}

/**
 */
void AddressExtracter::processBB (WorkSpace *ws, CFG *cfg, otawa::Block *b) {
	if(!b->isBasic())
		return;
	BasicBlock *bb = b->toBasic();

	elm::cout << __SOURCE_INFO__ << __GREEN__ << "Processing CFG " << bb->cfg()->index() << " BB " << bb->index() << __RESET__ << endl;


	clp::State ccll = clp::STATE_IN(bb);
	elm::cout << __SOURCE_INFO__ << "CFG " << bb->cfg()->index() << " BB " << bb->index() << " chk in " << ccll << endl;
	elm::cout << __SOURCE_INFO__ << __GREEN__ << "process " << bb->first() << " @ " << bb->first()->address() << __RESET__ << endl;

	clp::Manager::step_t step = man->start(bb);
	Vector<Pair<clp::Value, int> > addrs;

	while(step) {
		if(man->state()->equals(clp::State::EMPTY)) {
			step = man->next();
			continue;
		}


		// scan the instruction
		sem::inst i = man->sem();
		int action = 0;
		switch(i.op) {
		case sem::LOAD:
			action = 1;
			loadC++;
			break;
		case sem::STORE:
			action = 2;
			storeC++;
			break;
		}

		// add the access
		if(action && (i.memIndex() != 0)) {
			// clp::Value addr = man->state()->get(clp::Value(clp::REG, i.addr())); // if LOAD T1, T1, uint32, then T1 is already over-written
			clp::Value addr = man->getCurrentAccessAddress();

			if(addr == clp::Value::all) {
				Pair<Address, Address> accessRange = otawa::ACCESS_RANGE(man->inst());
				if(accessRange.fst != Address::null && accessRange.snd != Address::null)
				{
					addr = Value(VAL, accessRange.fst.offset(), 1, accessRange.snd.offset() - accessRange.fst.offset());
				}
			}


			elm::cout << __SOURCE_INFO__ << __YELLOW__ << man->inst() << " access " << addr << " for " << man->inst() << " @ " << man->inst()->address() << __RESET__ << endl;

			if(addr == clp::Value::all) {
				if(action == 1)
					loadCT++;
				else if(action == 2)
					storeCT++;

			}
#ifdef REWIND
			if((addr == clp::Value::all) && !REWIND(man->inst()) ) {
				clp::State rState = clp::STATE_IN(man->inst());
				elm::cout << "rState1 = "; rState.print(elm::cout, workspace()->platform()); elm::cout << endl;
				elm::cout << "rState2 = "; man->state()->print(elm::cout, workspace()->platform()); elm::cout << endl;
				rState.augment(*(man->state()));
				elm::cout << "rStatex = "; rState.print(elm::cout, workspace()->platform()); elm::cout << endl;
				man->rewind(rState);
				REWIND(man->inst()) = true;
			}
#endif
		}

		if((step & clp::Manager::END_INST) && (man->inst()->address() < bb->last()->address())) {
			elm::cout << __SOURCE_INFO__ << __GREEN__ << "process " << man->inst()->nextInst() << " @ " << man->inst()->nextInst()->address() << __RESET__ << endl;
		}

		// next step
		step = man->next();
	}
}


}} // otawa::tricore16P

