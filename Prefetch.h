#ifndef __TC275_PREFETCH_H__
#define __TC275_PREFETCH_H__

namespace otawa {
namespace tricore16 {


typedef enum {
	PF_NONE = 0,	// invalid value
	PF_NC = 1,		// not classified
	PF_ALWAYS = 2,	// always pre-fetched
	PF_NEVER = 3	// never pre-fetched
} pf_t;


}}

#endif
