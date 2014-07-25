#include <otawa/loader/gliss.h>
#include <tricore/api.h>
#include <tricore/macros.h>
#include <tricore/grt.h>

typedef int (*pipe_fun_t)(tricore_inst_t *inst);

/* functions */
static int pipe_unknown(tricore_inst_t *inst) {
	return 0;
}

$(foreach instructions)
static int pipe_$(IDENT)(tricore_inst_t *inst) {
$(pipe)
}

$(end)

/* table */
static pipe_fun_t pipe_tab[] = {
	pipe_unknown$(foreach instructions),
	pipe_$(IDENT)$(end)
};

int tricore_pipe(otawa::gliss::Info *info, otawa::Inst *inst) {
	tricore_inst_t *desc;
	info->decode(inst, desc);
	int r = pipe_tab[desc->ident](desc);
	info->free(desc);
	return r;
}
