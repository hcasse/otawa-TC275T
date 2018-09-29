/*
 * $Id$
 * Wei-Tsun SUN <weitsun.sun@irt-saintexupery.com>
 *
 * ContextValueInfo class for associating Flow Fact States with Basic Blocks.
 */
#ifndef OTAWA_FLOW_FACT_CONTEXT_VALUE_INFO_H
#define OTAWA_FLOW_FACT_CONTEXT_VALUE_INFO_H

#include <otawa/cfg/features.h>
#include <otawa/proc/BBProcessor.h>
#include <otawa/proc/Feature.h>
#include <otawa/prop/ContextualProperty.h>
#include <otawa/data/clp/features.h>
#include <otawa/util/FlowFactLoader.h> // EXIST_PROVIDED_STATE
#include <otawa/willie.h>

#include "TC275T.h"

namespace otawa {

using namespace elm;

namespace tricore16P {

// ContextValueInfo class
class ContextValueInfo: public BBProcessor {
public:
	static p::declare reg;
	ContextValueInfo(p::declare& r = reg);

protected:
	virtual void setup(WorkSpace *ws);
	virtual void processBB(WorkSpace *ws, CFG *cfg, Block *bb);

private:
};

Identifier<Vector<ContextualPath>*> CONTEXTUAL_PATHS("otawa::tricore16P::CONTEXUTUAL_PATHS", nullptr);
p::feature CONTEXT_VALUE_INFO_FEATURE("otawa::tricore16P::CONTEXT_VALUE_INFO_FEATURE", new Maker<ContextValueInfo>());


p::declare ContextValueInfo::reg = p::init("otawa::tricore16P::ContextValueInfo", Version(2, 0, 0))
	.base(BBProcessor::reg)
	.maker<ContextValueInfo>()
	.provide(otawa::tricore16P::CONTEXT_VALUE_INFO_FEATURE);
	;

ContextValueInfo::ContextValueInfo(p::declare& r)
:	BBProcessor(r)
{}

/**
 */
static void buildContextualPaths(CFG* cfg, ContextualPath& path) {
WILLIE_BEGIN_CONTEXTUAL_PATH3(
	elm::cout << __SOURCE_INFO__ << "entering " << cfg << " with context " << path << endl;
)WILLIE_END()
	Vector<ContextualPath>* vcp = CONTEXTUAL_PATHS(cfg);
	if(vcp == nullptr) {
		vcp = new Vector<ContextualPath>();
		CONTEXTUAL_PATHS(cfg) = vcp;
		ContextualPath cp;
		vcp->addFirst(cp);
	}
	vcp->addLast(path);

	for(CFG::BlockIter cfgbi = cfg->blocks();cfgbi;cfgbi++) {
		if(cfgbi->isSynth()) {
			SynthBlock *sb = cfgbi->toSynth();
			path.push(ContextualStep::CALL, sb->callInst()->address());
			path.push(ContextualStep::FUNCTION, sb->callee()->address());
			buildContextualPaths(sb->callee(), path);
			path.pop();
			path.pop();
		} // end of processing synth block
	} // end of processing each block
}



static void buildContextualPaths2(CFG* cfg, ContextualPath& path) {
	for(int i = 0; i < path.count();i++)
		elm::cout << "\t";
	elm::cout << "<function name=\"" << cfg->name() << "\" address=\"0x" << cfg->address() << "\" executed=\"true\" extern=\"false\">" << endl;

	Vector<ContextualPath>* vcp = CONTEXTUAL_PATHS(cfg);
	if(vcp == nullptr) {
		vcp = new Vector<ContextualPath>();
		CONTEXTUAL_PATHS(cfg) = vcp;
		ContextualPath cp;
		vcp->addFirst(cp);
	}
	vcp->addLast(path);

	for(CFG::BlockIter cfgbi = cfg->blocks();cfgbi;cfgbi++) {
		if(cfgbi->isSynth()) {
			SynthBlock *sb = cfgbi->toSynth();

			path.push(ContextualStep::CALL, sb->callInst()->address());

			for(int i = 0; i < path.count();i++)
				elm::cout << "\t";
			elm::cout << "<call name=\"" << sb->callee()->name()  << "\" address=\"0x" << sb->callInst()->address() <<  "\">" << endl;

			path.push(ContextualStep::FUNCTION, sb->callee()->address());
			buildContextualPaths2(sb->callee(), path);
			path.pop();

			for(int i = 0; i < path.count();i++)
				elm::cout << "\t";
			elm::cout << "</call>" << endl;
			path.pop();
		}
	}
	for(int i = 0; i < path.count();i++)
		elm::cout << "\t";
	elm::cout << "</function>" << endl;
}

void ContextValueInfo::setup(WorkSpace *ws) {
	const CFGCollection *cfgc = INVOLVED_CFGS(ws);
	CFG* cfg = *cfgc[0];

	ContextualPath path2;
	path2.push(ContextualStep::FUNCTION, cfg->address());
	buildContextualPaths2(cfg, path2);
	path2.pop();

	ContextualPath path;
	path.push(ContextualStep::FUNCTION, cfg->address());
	buildContextualPaths(cfg, path);
	path.pop();
}


/**
 */
void ContextValueInfo::processBB(WorkSpace *ws, CFG *cfg, Block *b) {

	if(!b->isBasic())
		return;

	BasicBlock *bb = b->toBasic();
	Vector<clp::FlowFactStateInfo>* map = clp::FLOW_FACT_STATE_INFO(bb);

	if(!map) {
		map = new Vector<clp::FlowFactStateInfo>();
		clp::FLOW_FACT_STATE_INFO(bb) = map;
	}
WILLIE_BEGIN_CONTEXTUAL_PATH1(
	elm::cout << __SOURCE_INFO__ << "process CFG " << bb->cfg()->index() << " BB " << bb->index() << endl;
)WILLIE_END()
	for(BasicBlock::InstIter inst(bb); inst; inst++) {
		if(!EXIST_PROVIDED_STATE(inst))
			continue;

WILLIE_BEGIN_CONTEXTUAL_PATH1(
		elm::cout << __SOURCE_INFO__ << "    processing inst @ " << inst->address() << endl;
)WILLIE_END()
		Vector<ContextualPath> *vcp = CONTEXTUAL_PATHS(cfg);
		for(Vector<ContextualPath>::Iter vcpi(*vcp);vcpi;vcpi++) {
			ContextualPath pathx = *vcpi;
			dfa::State *state = pathx(PROVIDED_STATE, inst);
			if(state) {
				map->add(clp::FlowFactStateInfo(*inst, pathx, state));
WILLIE_BEGIN_CONTEXTUAL_PATH1(
				elm::cout << __SOURCE_INFO__ << "    with context " << *vcpi << endl;
				for(dfa::State::RegIter r(state); r; r++)
					elm::cout << __SOURCE_INFO__ << "        " << (*r).fst->name() << " = " <<  (*r).snd.base() << "," << (*r).snd.delta() << "," << (*r).snd.count() << endl;
				for(dfa::State::MemIter m(state); m; m++)
					elm::cout << __SOURCE_INFO__ << "        MEM[" << (*m).address() << "] = " <<  (*m).value().base() << "," << (*m).value().delta() << "," << (*m).value().count() << endl; // "prob.initialize((*m).address(), (*m).value());
)WILLIE_END()
			}
		}
	} // end of each instruction
} // end of function processBB


} } // otawa::ipet

#endif // OTAWA_FLOW_FACT_CONTEXT_VALUE_INFO_H