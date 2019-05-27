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
#include <otawa/flowfact/features.h> // EXIST_PROVIDED_STATE

#include "TC275T.h"

namespace otawa {

using namespace elm;

namespace tricore16 {

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

Identifier<Vector<ContextualPath>*> CONTEXTUAL_PATHS("otawa::tricore16::CONTEXUTUAL_PATHS", nullptr);
p::feature CONTEXT_VALUE_INFO_FEATURE("otawa::tricore16::CONTEXT_VALUE_INFO_FEATURE", new Maker<ContextValueInfo>());


p::declare ContextValueInfo::reg = p::init("otawa::tricore16::ContextValueInfo", Version(2, 0, 0))
	.base(BBProcessor::reg)
	.maker<ContextValueInfo>()
	.provide(otawa::tricore16::CONTEXT_VALUE_INFO_FEATURE);
	;

ContextValueInfo::ContextValueInfo(p::declare& r)
:	BBProcessor(r)
{}

/**
 */
static void buildContextualPaths(CFG* cfg, ContextualPath& path) {
	Vector<ContextualPath>* vcp = CONTEXTUAL_PATHS(cfg);
	if(vcp == nullptr) {
		vcp = new Vector<ContextualPath>();
		CONTEXTUAL_PATHS(cfg) = vcp;
		ContextualPath cp;
		vcp->addFirst(cp);
	}
	vcp->addLast(path);

	for(CFG::BlockIter cfgbi = cfg->blocks();cfgbi();cfgbi++) {
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

	for(CFG::BlockIter cfgbi = cfg->blocks();cfgbi();cfgbi++) {
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

	int iii = 0;
	int bbx = 0;
	for(CFGCollection::BlockIter bbb(cfgc); bbb(); bbb++)
		if(bbb->isBasic()) {
		iii = iii + bbb->toBasic()->count();
		bbx++;
		}

	elm::cout << iii << " instructions on " << bbx << " BBs" << endl;

	CFG* cfg = (*cfgc)[0];

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
	for(BasicBlock::InstIter inst(bb); inst(); inst++) {
		if(!EXIST_PROVIDED_STATE(*inst))
			continue;

		Vector<ContextualPath> *vcp = CONTEXTUAL_PATHS(cfg);
		for(Vector<ContextualPath>::Iter vcpi(*vcp);vcpi();vcpi++) {
			ContextualPath pathx = *vcpi;
			dfa::State *state = pathx(PROVIDED_STATE, *inst);
			if(state) {
				map->add(clp::FlowFactStateInfo(*inst, pathx, state));
			}
		}
	} // end of each instruction
} // end of function processBB


} } // otawa::ipet

#endif // OTAWA_FLOW_FACT_CONTEXT_VALUE_INFO_H
