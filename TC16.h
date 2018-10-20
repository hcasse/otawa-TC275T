#include <otawa/loader/gliss.h>
#include <tricore/api.h>
#include <tricore/macros.h>
#include <tricore/grt.h>

typedef int (*prod_fun_t)(tricore_inst_t *inst, bool coreE);
int tricore_prod(otawa::gliss::Info *info, otawa::Inst *inst, bool coreE);
typedef int (*pipe_fun_t)(tricore_inst_t *inst);
int tricore_pipe(otawa::gliss::Info *info, otawa::Inst *inst);
typedef int (*mreg_fun_t)(tricore_inst_t *inst);
int tricore_mreg(otawa::gliss::Info *info, otawa::Inst *inst);
