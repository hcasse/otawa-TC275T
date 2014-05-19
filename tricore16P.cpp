#include <otawa/parexegraph/GraphBBTime.h>
#include <otawa/parexegraph/ParExeGraph.h>
#include <otawa/proc/ProcessorPlugin.h>
#include <otawa/branch/BranchBuilder.h>
#include <otawa/util/LBlockBuilder.h>
#include <otawa/branch/features.h>


#include "prod16P.h"

using namespace otawa;

namespace continental { namespace tricore16P {


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









class ExeGraph: public ParExeGraph {
public:
	ExeGraph(
		WorkSpace *ws,
		ParExeProc *proc,
		ParExeSequence *seq,
		const PropList &props = PropList::EMPTY): otawa::ParExeGraph(ws, proc, seq, props) { }

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

#define BY_CONTEXT_ONLY
#define BY_SPLIT


/**
 * Represents a configuration for computing cost of a BB.
 * Each bits represents the state of an event:
 * @li 0	event not activated
 * @li 1	event activated
 */
class Config {
public:
	inline Config(void): b(0) { }
	inline Config(const Config& conf): b(conf.b) { }
	inline t::uint32 bits(void) const { return b; }
	inline void set(int n) { b |= 1 << n; }
	inline void clear(int n) { b &= ~(1 << n); }
	inline bool bit(int n) { return b & (1 << n); }

	string toString(int n) {
		StringBuffer buf;
		for(int i = 0; i < n; i++)
			buf << (bit(i) ? "+" : "-");
		return buf.toString();
	}

private:
	t::uint32 b;
};


/**
 * Set of configurations with the same or with a maximum of time.
 */
class ConfigSet {
public:
	ConfigSet(void): t(0) { }
	ConfigSet(ot::time time): t(time) { }
	ConfigSet(const ConfigSet& set): t(set.t), confs(set.confs) { }
	inline ot::time time(void) const { return t; }
	inline int count(void) const { return confs.length(); }
	inline void add(Config conf) { confs.add(conf); }
	inline void add(ConfigSet& set) { t = max(t, set.time()); confs.addAll(set.confs); }

	t::uint32 posConst(void)  {
		t::uint32 r = 0xffffffff;
		for(int i = 0; i < confs.length(); i++)
			r &= confs[i].bits();
		return r;
	}

	t::uint32 negConst(void) {
		t::uint32 r = 0;
		for(int i = 0; i < confs.length(); i++)
			r |= confs[i].bits();
		return ~r;
	}

	t::uint32 unused(t::uint32 neg, t::uint32 pos, int n) {
		t::uint32 mask = neg | pos;
		t::uint32 r = 0;
		for(int i = 0; i < n; i++)
			if(!(mask & (1 << i))) {
				genstruct::Vector<t::uint32> nconf, pconf;
				for(int j = 0; j < confs.length(); j++) {
					t::uint32 c = confs[j].bits();
					if(c & (1 << i)) {
						c &= ~(1 << i);
						if(nconf.contains(c))
							nconf.remove(c);
						else
							pconf.add(c);
					}
					else {
						if(pconf.contains(c))
							pconf.remove(c);
						else
							nconf.add(c);
					}
				}
				if(!pconf && !nconf)
					r |= 1 << i;
			}
		return r;
	}

	t::uint32 complex(t::uint32 neg, t::uint32 pos, t::uint32 unused, int n) {
		return ((1 << n) - 1) & ~neg & ~pos & ~unused;
	}

	class Iter: public genstruct::Vector<Config>::Iterator {
	public:
		inline Iter(const ConfigSet& set): genstruct::Vector<Config>::Iterator(set.confs) { }
	};

private:
	ot::time t;
	genstruct::Vector<Config> confs;
};

/**
 * Account for a time reduction on an edge.
 */
class TimeReduction {
public:
	TimeReduction(void): t(0), v(0) { }
	TimeReduction(ot::time& time, ilp::Var *var): t(time), v(var) { }
	TimeReduction(const TimeReduction& r): t(r.t), v(r.v) { }
	TimeReduction& operator=(const TimeReduction& r) { t = r.t; v = r.v; return *this; }
	inline ot::time time(void) const { return t; }
	inline ilp::Var *var(void) const { return v; }
private:
	ot::time t;
	ilp::Var *v;
};
/*template <class T>
bool operator==(const T& v1, const T& v2) { return memcmp(&v1, &v2, sizeof(T)); }*/
Identifier<TimeReduction> TIME_REDUCTION("otawa::TIME_REDUCTION", TimeReduction());

/**
 * Abstract class describing an event arising on an instruction.
 */
class Event {
public:
	Event(void): act(false) { }
	Event(const string& name): _name(name), act(false) { }
	inline bool activated(void) const { return act; }
	inline const string& name(void) const { return _name; }
	inline void setName(const string& name) { _name = name; }
	virtual void apply(void) { act = true; }
	virtual void rollback(void) { act = false; }
private:
	string _name;
	bool act;
};

class EdgeEvent: public Event {
public:
	EdgeEvent(ParExeEdge *edge, ot::time time): _edge(edge), _time(time) { }

	virtual void apply(void) {
		Event::apply();
		doIt();
	}

	virtual void rollback(void) {
		Event::rollback();
		doIt();
	}

private:

	void doIt(void) {
		ot::time old = _edge->latency();
		_edge->setLatency(_time);
		_time = old;
	}

	ParExeEdge *_edge;
	ot::time _time;
};

class NodeEvent: public Event {
public:
	NodeEvent(ParExeNode *node, ot::time time): _node(node), _time(time) { }

	virtual void apply(void) {
		Event::apply();
		doIt();
	}

	virtual void rollback(void) {
		Event::rollback();
		doIt();
	}

private:

	void doIt(void) {
		ot::time old = _node->latency();
		_node->setLatency(_time);
		_time = old;
	}

	ParExeNode *_node;
	ot::time _time;
};


class BBTimer: public GraphBBTime<ExeGraph> {
public:
	static p::declare reg;
	BBTimer(void): GraphBBTime<ExeGraph>(reg) { }
protected:

	virtual void processBB(WorkSpace *ws, CFG *cfg, BasicBlock *bb) {
		if(bb->isEnd())
			return;

		// compute the maximum of t for each context
		ot::time t = 0;
		for(BasicBlock::InIterator edge(bb); edge; edge++) {
			ot::time nt = compute(edge);
			if(nt >= t)
				t = nt;
#			ifdef BY_CONTEXT_ONLY
				ipet::TIME_DELTA(edge) = nt;
#			endif
		}
		if(isVerbose())
			log << "\t\t\tcost = " << t << io::endl;
		ipet::TIME(bb) = t;

		// fixes the time delata
#		ifdef BY_CONTEXT_ONLY
			for(BasicBlock::InIterator edge(bb); edge; edge++) {
				ipet::TIME_DELTA(edge) -= t;
				if(isVerbose())
					log << "\t\t\tcost of " << edge->source() << "->" << edge->target() << " = " << ipet::TIME_DELTA(edge) << io::endl;
			}
#		endif
	}

	/**
	 * Compute the cost for the given edge sink with the given edge source
	 * as context.
	 * @param edge		Context edge.
	 * @return			Cost for the current context edge.
	 */
	ot::time compute(Edge *_edge) {
		int index = 0;
		conf = Config();
		confs.clear();
		if(isVerbose())
			log << "\t\t\tcontext " << _edge << io::endl;

		// clear the old state
		edge = _edge;
		seq = new ParExeSequence();
		last = 0;
		first = 0;
		events.clear();

		// fill with previous
		prev = edge->source();
		if(!prev->isEnd())
			for(BasicBlock::InstIterator inst(prev); inst; inst++) {
				ParExeInst * par_exe_inst = new ParExeInst(inst, prev, PROLOGUE, index++);
				seq->addLast(par_exe_inst);
				last = par_exe_inst;
			}

		// fill with current block
		cur = edge->target();
		for(BasicBlock::InstIterator inst(cur); inst; inst++) {
			ParExeInst * par_exe_inst = new ParExeInst(inst, cur, BODY, index++);
			seq->addLast(par_exe_inst);
			if(!first)
				first = par_exe_inst;
		}

		// prepare the graph
		PropList props;
		ExeGraph graph(this->workspace(), _microprocessor, seq, props);
		graph.build();

		// events for branch prediction
		computeBranchPred(graph);

		// events for fetch (ICache and PFlash prefetch)
		ParExeSequence::InstIterator inst(seq);
		genstruct::AllocatedTable<LBlock* >* blocks;
		if(last) {
			blocks = BB_LBLOCKS(prev);
			for(int i = 0; i < blocks->count(); i++) {
				LBlock *block = blocks->get(i);
				ParExeInst *last_inst = inst;
				while(inst && inst->inst()->address() < block->address()) {
					inst++;
					if(inst)
						last_inst = *inst;
				}
				computeICache(block, last_inst);
				pf_infos_t table = PF_INFOS(prev);
				computePFlashPrefetch(table.get(i).snd, last_inst);
			}
		}
		blocks = BB_LBLOCKS(cur);
		for(int i = 0; i < blocks->count(); i++) {
			LBlock *block = blocks->get(i);
			ParExeInst *last_inst = inst;
			while(inst && inst->inst()->address() < block->address()) {
				inst++;
				if(inst)
					last_inst = *inst;
			}
			computeICache(block, last_inst);
			pf_infos_t table = PF_INFOS(cur);
			
			//test PFlash Prefetch Category
			/*cout << "DEBUG : BBTimer : testing values of table " << io::endl;
			cout << "DEBUG : " << table.get(i).fst << "  ";
			if(table.get(i).snd == PF_A)
				cout << "PF_A" << io::endl;
			else
				cout << "PF_N" << io::endl;*/
					
			computePFlashPrefetch(table.get(i).snd, last_inst);
			
			
			
		}
		
		
		

		// perform the computation
		ot::time cost = apply(graph, 0);
#		ifdef BY_SPLIT
			computeSplit(cost);
#		endif

		// cleanup and quit
		//delete seq;
		return cost;
	}

	ot::time apply(ParExeGraph& graph, int i) {
		if(i >= events.length()) {
			ot::time cost = graph.analyze();
			if(events && isVerbose()) {
				log << "\t\t\t\t";
				for(int j = 0; j < events.length(); j++)
					log << (events[j]->activated() ? "+" : "-");
				log << " cost = " << cost << io::endl;
			}
			addConf(cost, conf);
			return cost;
		}
		else {
			conf.clear(i);
			ot::time without = apply(graph, i + 1);
			events[i]->apply();
			conf.set(i);
			ot::time with =  apply(graph, i + 1);
			events[i]->rollback();
			return max(with, without);
		}
	}

	/**
	 * Prepare events and graph for branch prediction.
	 * If any, add the event to the events attribute.
	 * @param graph		Exe graph to look for branch event in.
	 */
	void computeBranchPred(ParExeGraph& graph) {
		if(last == 0)
			return;
		if(!last->inst()->isControl())
			return;

		// collect data
		branch::category_t cat = branch::CATEGORY(prev);
		BasicBlock *h = branch::HEADER(prev);
		bool is_taken = edge->kind() != Edge::NOT_TAKEN;

		// process the category
		switch(cat) {
		case branch::ALWAYS_D:
			new ParExeEdge(last->firstNode(), first->firstNode(), ParExeEdge::SOLID, is_taken ? 2 : 0);
			break;
		case branch::ALWAYS_H:
			ASSERTP(false, "unsupported category ALWAYS_H");
			break;
		case branch::FIRST_UNKNOWN:
			if(isVerbose())
				log << "\t\t\t\tbranch: first-unknown\n";
			goto doit;
		case branch::NOT_CLASSIFIED:
			if(isVerbose())
				log << "\t\t\t\tbranch: NC\n";
		doit: {
				ParExeEdge *edge = new ParExeEdge(last->firstNode(), first->firstNode(), ParExeEdge::SOLID, is_taken ? 1 : 0);
				events.add(new EdgeEvent(edge, 2));
			}
			break;
		}
	}


	/**
	 * Handle events for a LBlock for the given instruction.
	 * If any, add the event to the events list.
	 * @param block	L-Block supporting the event.
	 * @param inst	Instruction starting the LBlock.
	 */
	void computeICache(LBlock *block, ParExeInst *inst) {

		// collect data
		cache::category_t cat = cache::CATEGORY(block);
		BasicBlock *h = cache::CATEGORY_HEADER(block);

		// apply the category
		switch(cat) {
		case cache::INVALID_CATEGORY:
			ASSERTP(false, "invalid_category found");
			break;
		case cache::ALWAYS_HIT:
			// nothing to do
			break;
		case cache::ALWAYS_MISS:
			inst->firstNode()->setLatency(1+memoryLatency(block->address()));
			break;
		case cache::FIRST_MISS:
			if(isVerbose())
				log << "\t\t\t\tI$: FM at " << block->address() << io::endl;
			goto doit;
		case cache::FIRST_HIT:
			if(isVerbose())
				log << "\t\t\t\tI$: FH at " << block->address() << io::endl;
			goto doit;
		case cache::NOT_CLASSIFIED:
			if(isVerbose())
				log << "\t\t\t\tI$: NC at " << block->address() << io::endl;
		doit:
			events.add(new NodeEvent(inst->firstNode(), 1+memoryLatency(block->address())));
			break;
		}
	}
	
	void computePFlashPrefetch(pf_t cat, ParExeInst *inst) {
		switch(cat) {
		case PF_NONE:
			ASSERTP(false, "invalid_category found");
			break;
		case PF_NC:
			ASSERTP(false, "invalid_category found");
			break;
		case PF_A:
			//replace memory latency with bus latency
			inst->firstNode()->setLatency(1);
			break;
		case PF_N:
			//nothing to do (?)
			break;
		}
	}

	/**
	 * Add a new configuration with its time.
	 * @param cost	Cost of the configuration.
	 * @param conf	Configuration to add.
	 */
	void addConf(ot::time cost, Config conf) {

		// look all config set
		for(int i = 0; i < confs.length(); i++)
			if(confs[i].time() >= cost) {
				if(confs[i].time() != cost)
					confs.insert(i, ConfigSet(cost));
				confs[i].add(conf);
				return;
			}

		// add a new one at end
		confs.add(ConfigSet(cost));
		confs[confs.length() - 1].add(conf);
	}

	/**
	 * Display the list of configuration sorted by cost.
	 */
	void displayConfs(void) {
		for(int i = 0; i < confs.length(); i++) {
			log << "\t\t\t\t" << confs[i].time() << " -> ";
			for(ConfigSet::Iter conf(confs[i]); conf; conf++)
				log << " " << (*conf).toString(events.length());
			log << io::endl;
		}
	}

	/**
	 * Split the configurations according to the best results.
	 * @return	Last index after the lower part of the configurations.
	 */
	int splitConfs(void) {
		static const float	sep_factor = 1,
							over_factor = 1.5;

		// initialization
		int p = 1, best_p = 1;
		float best_cost = type_info<float>::min;
		int min_low = confs[0].time(), max_low = confs[0].time(), cnt_low = confs[0].count();
		int min_high = confs[1].time(), max_high = confs[confs.length() - 1].time();
		int cnt_high = 0;
		for(int i = 1; i < confs.length(); i++)
			cnt_high += confs[i].count();

		// computation
		while(p < confs.length()) {

			// select the best split
			float cost = sep_factor * (max_low - min_high) + over_factor * (max_low - min_low);
			if(cost > best_cost) {
				best_p = p;
				best_cost = cost;
			}

			// prepare next split
			max_low = confs[p].time();
			p++;
			if(p < confs.length())
				min_high = confs[p].time();
		}
		return best_p;
	}

	/**
	 * Split times in two families attempting to maximize the precision
	 * while preserving the soudness. Time/configuration are grouped in two sets:
	 * low time set (LTS) and high time set (HTS). For both sets, the time
	 * is the max of costs of configuration in each set.
	 * 
	 * As a default, this algorithm try to compute a lower bound of the number of times
	 * LTS arises.
	 * 
	 * @param cost	Maximum of cost.
	 */
	void computeSplit(ot::time cost) {

		// get the best split factor
		if(isVerbose())
			displayConfs();
		int p;
		if(confs.length() < 2)
			return;
		else if(confs.length() == 2)
			p = 1;
		else
			p = splitConfs();

		// build the lower set
		ConfigSet set;
		for(int i = 0; i < p; i++)
			set.add(confs[i]);
		if(isVerbose()) {
			log << "\t\t\t\t" << "time is " << set.time() << "(delta = " << (set.time() - cost) << ") for { ";
			bool fst = true;
			for(ConfigSet::Iter conf(set); conf; conf++) {
				if(fst)
					fst = false;
				else
					log << ", ";
				log << (*conf).toString(events.length());
			}
			log << " }\n";
		}

		// prepare the constraints
		t::uint32 pos = set.posConst();
		t::uint32 neg = set.negConst();
		t::uint32 unu = set.unused(neg, pos, events.length());
		t::uint32 com = set.complex(neg, pos, unu, events.length());
		if(isVerbose())
			log << "\t\t\t\tpos = " << maskToString(pos) << ", neg = " << maskToString(neg)
				<< ", unused = " << maskToString(unu) << ", complex = " << maskToString(com) << io::endl;
		if(com) {
			if(isVerbose())
				log << "\t\t\t\ttoo complex: " << io::hex(com) << io::endl;
			return;
		}
	}

	string maskToString(t::uint32 mask) {
		StringBuffer buf;
		for(int i = 0; i < events.length(); i++)
			buf << ((mask & (1 << i)) ? "!" : "_");
		return buf.toString();
	}

	// context management
	Edge *edge;
	BasicBlock *prev, *cur;
	ParExeSequence *seq;
	ParExeInst *last, *first;
	genstruct::Vector<Event *> events;
	Config conf;
	genstruct::Vector<ConfigSet> confs;

};

p::declare BBTimer::reg = p::init("continental::tricore16P::BBTimer", Version(1, 0, 0))
		.base(GraphBBTime<ParExeGraph>::reg)
		.require(otawa::hard::CACHE_CONFIGURATION_FEATURE)
		.require(otawa::gliss::INFO_FEATURE)
		.require(otawa::branch::CONSTRAINTS_FEATURE)
		.require(continental::tricore16P::PREFETCH_CATEGORY_FEATURE)
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
