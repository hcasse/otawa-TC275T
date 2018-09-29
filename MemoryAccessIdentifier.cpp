//#include <otawa/cache/features.h>
#include <otawa/prop/PropList.h>
//#include <otawa/dfa/BitSet.h>
//#include <otawa/hard/Cache.h>

//#include <otawa/hard/CacheConfiguration.h>
//#include <otawa/hard/Cache.h>
//#include <otawa/hard/Memory.h>
//#include <otawa/hard/Platform.h>
#include <otawa/prog/WorkSpace.h>
#include <otawa/prog/Process.h>
//#include <otawa/dcache/MemoryAccessIdentifier.h>

#include <otawa/proc/BBProcessor.h>
#include <otawa/data/clp/features.h>
#include <otawa/willie.h>

namespace otawa { namespace tricore16P {


// MemoryAccessIdentifier class
class MemoryAccessIdentifier: public BBProcessor {
public:
	static p::declare reg;
	MemoryAccessIdentifier(p::declare& r = reg);

protected:
	virtual void setup(WorkSpace *ws);
	virtual void cleanup(WorkSpace *ws);
	virtual void processBB(WorkSpace *ws, CFG *cfg, otawa::Block *bb);
	virtual void processWorkSpace(WorkSpace *fw);

private:
//	const hard::Cache *cache;
	const hard::Memory *mem;
//	Vector<BlockAccess> accs;
//	BlockCollection *colls;
	clp::Manager *man;

	int loadC;
	int storeC;
	int loadCT;
	int storeCT;
};

p::declare MemoryAccessIdentifier::reg = p::init("otawa::tricore16P::MemoryAccessIdentifier", Version(1, 0, 0))
	.base(BBProcessor::reg)
	.maker<MemoryAccessIdentifier>()
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
 * @li @ref MemoryAccessIdentifier
 *
 * @see clp
 * @ingroup dcache
 */
p::feature MEMORY_ACCESS_IDENTIFY_FEATURE("otawa::tricore16P::MEMORY_ACCESS_IDENTIFY_FEATURE", new Maker<MemoryAccessIdentifier>());


/**
 */
MemoryAccessIdentifier::MemoryAccessIdentifier(p::declare& r): BBProcessor(r), mem(0), man(0) {
	loadC = 0;
	storeC = 0;
	loadCT = 0;
	storeCT = 0;
}


/**
 */
void MemoryAccessIdentifier::setup(WorkSpace *ws) {

//	// get cache
//	cache = hard::CACHE_CONFIGURATION(ws)->dataCache();
//	if(!cache)
//		throw otawa::Exception("no data cache !");
//	if(cache->replacementPolicy() != hard::Cache::LRU)
//		throw otawa::Exception("unsupported replacement policy in data cache !");
//
//	// get memory
//	mem = hard::MEMORY(ws);
//
//	// build the block collection
//	colls = new BlockCollection[cache->rowCount()];
//	DATA_BLOCK_COLLECTION(ws) = colls;
//	for(int i = 0; i < cache->rowCount(); i++)
//		colls[i].setSet(i);
//
	// allocate the manager
	man = new clp::Manager(ws);
}


/**
 */
void MemoryAccessIdentifier::cleanup(WorkSpace *ws) {
	if(man)
		delete man;
}


void MemoryAccessIdentifier::processWorkSpace(WorkSpace *fw) {


	elm::cout << "XML<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>" << endl;
	elm::cout << "XML<flowfacts>" << endl;
	BBProcessor::processWorkSpace(fw);

	elm::cout << "XML<reg-set name=\"a10\"><value address=\"0x70019600\"/></reg-set>" << endl;
	elm::cout << "XML<reg-set name=\"CTX\"><value address=\"0x70019C00\"/></reg-set>" << endl;
	elm::cout << "XML</flowfacts>" << endl;

	elm::cout << "Finishing processing otawa::tricore16P::MemoryAccessIdentifier" << endl;
}

/**
 */
void MemoryAccessIdentifier::processBB (WorkSpace *ws, CFG *cfg, otawa::Block *b) {
	if(!b->isBasic())
		return;
	BasicBlock *bb = b->toBasic();

	elm::cout << __GREEN__ << "Processing CFG " << bb->cfg()->index() << " BB " << bb->index() << __RESET__ << endl;

	for(BasicBlock::InstIter bbii = bb->insts(); bbii; bbii++) {
		bool mem = false;

		if(bbii->kind() & Inst::IS_CONTROL) {
			continue;
		}


		elm::cout << "XML<mem-access address=\"0x" << hex(bbii->address().offset()) << "\">" << endl;
		elm::cout << "XML\t<low address=\"lowaddress\"/>" << endl;
		elm::cout << "XML\t<high address=\"highaddress\"/>" << endl;
		elm::cout << "XML</mem-access>" << endl;

		if(bbii->kind() & Inst::IS_LOAD) {
			elm::cout << __YELLOW__ << *bbii << " @ " << bbii->address() << " loads memory " << __RESET__ << endl;
			mem = true;
		}
		if(bbii->kind() & Inst::IS_STORE) {
			elm::cout << __YELLOW__ << *bbii << " @ " << bbii->address() << " store memory " << __RESET__ << endl;
			mem = true;
		}
	}


}


}} // otawa::tricore16P

