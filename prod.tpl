#include <otawa/loader/gliss.h>
#include <tricore/api.h>
#include <tricore/macros.h>
#include <tricore/grt.h>

typedef int (*prod_fun_t)(tricore_inst_t *inst, bool coreE);

/* functions */
static int prod_unknown(tricore_inst_t *inst, bool coreE) {
	return 2;
}

$(foreach instructions)
static int prod_$(IDENT)(tricore_inst_t *inst, bool coreE) {
$(prod)
}

$(end)

/* table */
static prod_fun_t prod_tab[] = {
	prod_unknown$(foreach instructions),
	prod_$(IDENT)$(end)
};

int tricore_prod(otawa::gliss::Info *info, otawa::Inst *inst, bool coreE) {
	tricore_inst_t *desc;
	info->decode(inst, desc);
	int r = prod_tab[desc->ident](desc, coreE);
	info->free(desc);
	return r;
}
