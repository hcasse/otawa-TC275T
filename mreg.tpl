#include <otawa/loader/gliss.h>
#include <tricore/api.h>
#include <tricore/macros.h>
#include <tricore/grt.h>

typedef int (*mreg_fun_t)(tricore_inst_t *inst);

/* functions */
static int mreg_unknown(tricore_inst_t *inst) {
	return -1;
}

$(foreach instructions)
static int mreg_$(IDENT)(tricore_inst_t *inst) {
$(mreg)
}

$(end)

/* table */
static mreg_fun_t mreg_tab[] = {
	mreg_unknown$(foreach instructions),
	mreg_$(IDENT)$(end)
};

int tricore_mreg(otawa::gliss::Info *info, otawa::Inst *inst) {
	tricore_inst_t *desc;
	info->decode(inst, desc);
	int r = mreg_tab[desc->ident](desc);
	info->free(desc);
	return r;
}
