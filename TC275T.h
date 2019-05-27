#ifndef OTAWA_TRICORE16P_TC275T_H
#define OTAWA_TRICORE16P_TC275T_H

#include <elm/data/HashMap.h>

using namespace elm;

namespace otawa { namespace tricore16P {

template <class T1, class T2, class T3>
class Triplet {
public:
	T1 fst;
	T2 snd;
	T3 trd;
	inline Triplet(void) { }
	inline Triplet(const T1& _fst, const T2& _snd, const T3& _trd): fst(_fst), snd(_snd), trd(_trd) { }
	inline Triplet(const Triplet<T1, T2, T3>& triplet): fst(triplet.fst), snd(triplet.snd), trd(triplet.trd) { }
	inline Triplet<T1, T2, T3>& operator=(const Triplet<T1, T2, T3>& triplet) { fst = triplet.fst; snd = triplet.snd; trd = triplet.trd; return *this; }
	inline bool operator==(const Triplet<T1, T2, T3>& triplet) const { return ((fst== triplet.fst) && (snd == triplet.snd) && (trd == triplet.trd)); }
};


extern Identifier<Vector<ContextualPath>*> CONTEXTUAL_PATHS;
extern p::feature CONTEXT_VALUE_INFO_FEATURE;

}}




#endif
