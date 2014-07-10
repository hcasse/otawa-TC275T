#include <otawa/parexegraph/GraphBBTime.h>
#include <otawa/parexegraph/ParExeGraph.h>
#include <otawa/proc/ProcessorPlugin.h>
#include <otawa/branch/BranchBuilder.h>
#include <otawa/util/LBlockBuilder.h>
#include <otawa/branch/features.h>
#include <otawa/etime/EdgeTimeBuilder.h>


#include "prod16P.h"

using namespace otawa;

namespace continental { namespace tricore16P {


#if 0
typedef enum {
	PF_NONE = 0,
	PF_NC = 1,
	PF_A = 2,
	PF_N = 3
} pf_t;

class PrefetchCategoryAnalysis: public BBProcessor {
public:
	static p::declare reg;
	PrefetchCategoryAnalysis(void): BBProcessor(reg) { }
protected:
	virtual void processBB(WorkSpace *ws, CFG *cfg, BasicBlock *bb);
private:
	pf_t findPrefetchCategory(int i, genstruct::AllocatedTable<LBlock* >* blocks, BasicBlock *bb);
};

Feature<PrefetchCategoryAnalysis> PREFETCH_CATEGORY_FEATURE("continental::tricore16P::PREFETCH_CATEGORY_FEATURE");



typedef Pair<Address, pf_t> pf_info_t;
typedef genstruct::Table<pf_info_t> pf_infos_t;
Identifier<pf_infos_t> PF_INFOS("continental::tricore16P::PF_INFOS");


p::declare PrefetchCategoryAnalysis::reg = p::init("continental::tricore16P::PrefetchCategoryAnalysis", Version(1, 0, 0))
	.maker<PrefetchCategoryAnalysis>()
	.base(BBProcessor::reg)
	.provide(PREFETCH_CATEGORY_FEATURE)
	.require(otawa::ICACHE_CATEGORY2_FEATURE);


//TODO : check cache::first_hit & cache::first_miss
void PrefetchCategoryAnalysis::processBB(WorkSpace *ws, CFG *cfg, BasicBlock *bb) {
	log << "DEBUG : starting Prefetch Analysis for block " << bb->address() << io::endl;
	
	/*//tests
	log << "TESTS: size " << bb->size() << io::endl;
	log	<< "TESTS: inst count " << bb->countInstructions() << io::endl;*/


	genstruct::AllocatedTable<LBlock* >* blocks = BB_LBLOCKS(bb);
	
	pf_t prefetch_cat=PF_NONE;
	
	if(bb->isVirtual()==true){
		//log << "DEBUG : Number of LBlocks : " << blocks->size() << io::endl;
			
		pf_info_t *tab = new pf_info_t[blocks->size()];	
			
		for(int i=0; i<blocks->size(); i++){
			LBlock *block = blocks->get(i);
			//log << "DEBUG : address of LBlock : " << block->address() << io::endl;
			cache::category_t cat = cache::CATEGORY(block);
				
			switch(cat) {
			case cache::INVALID_CATEGORY:
				ASSERTP(false, "invalid_category found");
				break;
			case cache::ALWAYS_HIT:
				//log << "DEBUG : cache always hit" << io::endl;
				prefetch_cat=PF_N;
				break;
			case cache::FIRST_HIT:
				//log << "DEBUG : cache first hit" << io::endl;
				prefetch_cat=PF_N; // (?)
				break;
			case cache::ALWAYS_MISS:
				//log << "DEBUG : cache always miss" << io::endl;
				prefetch_cat=findPrefetchCategory(i,blocks,bb);
				break;
			case cache::FIRST_MISS:
				//log << "DEBUG : cache first miss" << io::endl;
				prefetch_cat=findPrefetchCategory(i,blocks,bb); //(?)
				break;
			}
			
			//create the pair address/category and put it in the table
			tab[i].fst = block->address();
			tab[i].snd = prefetch_cat;
		}
		
		//attach the table to the block
		pf_infos_t table(tab,blocks->size());
		PF_INFOS(bb) = table;
		
		//testing table
		log << "DEBUG : testing values of table " << io::endl;
		for(int i=0; i<blocks->size(); i++){
			log << "DEBUG : " << table.get(i).fst << "  ";
			if(table.get(i).snd == PF_A)
				log << "PF_A" << io::endl;
			else
				log << "PF_N" << io::endl;
		}
	}
}



pf_t PrefetchCategoryAnalysis::findPrefetchCategory(int i, genstruct::AllocatedTable<LBlock* >* blocks, BasicBlock *bb){
	pf_t prefetch_cat = PF_NONE;
	LBlock *block;
	
	//find previous LBlock
	if(i!=0){
		block = blocks->get(i-1);
		
		//find category of previous LBlock
		cache::category_t cat = cache::CATEGORY(block);
		switch(cat) {
		case cache::INVALID_CATEGORY:
			ASSERTP(false, "invalid_category found");
			break;
		case cache::ALWAYS_HIT:
			//cout << "DEBUG : PF_N" << io::endl;
			prefetch_cat = PF_N;
			break;
		case cache::ALWAYS_MISS:
			//cout << "DEBUG : PF_A" << io::endl;
			prefetch_cat = PF_A;
			break;
		case cache::FIRST_HIT:
			//cout << "DEBUG : PF_N" << io::endl;
			prefetch_cat = PF_N;
			break;
		case cache::FIRST_MISS:
			//cout << "DEBUG : PF_A" << io::endl;
			prefetch_cat = PF_A;
			break;
		}
	}
	
	//find last LBlock of previous BBlock
	else{ 
		//cout << "DEBUG : First LBlock" << io::endl;
			
		for(BasicBlock::InIterator edge(bb); edge; edge++) {
			BasicBlock *inbb = edge->source();
			
			if(inbb->isVirtual()==true){
				//cout << "DEBUG : Address of previous Block : " << inbb->address() << io::endl;
					
				//get last LBlock of previous BasicBlock
				genstruct::AllocatedTable<LBlock* >* blocks_prev = BB_LBLOCKS(inbb);
				LBlock *block_prev = blocks_prev->get(blocks_prev->size()-1);
				
				cache::category_t cat = cache::CATEGORY(block_prev);
				prefetch_cat = PF_A;
				
				if(cat == cache::INVALID_CATEGORY) {
					ASSERTP(false, "invalid_category found");
				}
				
				else if(cat == cache::ALWAYS_HIT || cat == cache::FIRST_HIT){
					prefetch_cat = PF_N;
					//cout << "DEBUG : PF_N" << io::endl;
				}
				
				else{
					//cout << "DEBUG : Previous LBlock is cache miss" << io::endl;
					//check if last LBlock of previous BBlock is just before first LBlock of current BBlock
					block = blocks->get(0);
					if(block->address() != (block_prev->address()+block_prev->size())){
						prefetch_cat = PF_N;
						//cout << "DEBUG : PF_N" << io::endl;
					}
				}
			}
			
			//if entry basic block then instruction is never prefetched
			else
				prefetch_cat = PF_N;
		}
		
		/*if(prefetch_cat == PF_N)
			cout << "DEBUG : PF_N"<< io::endl;
		else if(prefetch_cat == PF_A)
			cout << "DEBUG : PF_A"<< io::endl;*/
	}
	
	
			
	return prefetch_cat;
}


#endif





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
