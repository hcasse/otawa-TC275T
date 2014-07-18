#include <otawa/loader/gliss.h>
#include <tricore/api.h>
#include <tricore/macros.h>
#include <tricore/grt.h>

typedef int (*prod_fun_t)(tricore_inst_t *inst);

/* functions */
static int prod_unknown(tricore_inst_t *inst) {
	return 2;
}

static int prod_RFE(tricore_inst_t *inst) {
	return 3;

}

static int prod_BISR__ID(tricore_inst_t *inst) {
	return 3;

}

static int prod_NOP(tricore_inst_t *inst) {
	return 0;

}

static int prod_DEBUG(tricore_inst_t *inst) {
	return 0;

}

static int prod_FRET(tricore_inst_t *inst) {
	return 0;

}

static int prod_LOOP_AD__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_RET(tricore_inst_t *inst) {
	return 3;

}

static int prod_JI_AD(tricore_inst_t *inst) {
	return 0;

}

static int prod_JNZ_T_D15__D__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JNZ_A_AD__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JNZ_DD__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JNZ_D15__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JZ_A_DD__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JZ_D15__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JZ_DD__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JZ_T_D15__D__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JLTZ_DD__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JNE_D15__D__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JNE_D15__DD__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JNE_D15__D__08X_0(tricore_inst_t *inst) {
	return 0;

}

static int prod_JNE_D15__DD__08X_0(tricore_inst_t *inst) {
	return 0;

}

static int prod_JLEZ_DD__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JGTZ_DD__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JGEZ_DD__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JEQ_D15__DD__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JEQ_D15__DD__08X_0(tricore_inst_t *inst) {
	return 0;

}

static int prod_JEQ_D15___ID__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JEQ_D15___ID__08X_0(tricore_inst_t *inst) {
	return 0;

}

static int prod_J_08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_CALL_08X(tricore_inst_t *inst) {
	return 3;

}

static int prod_ST_H___LT_AD_RT__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_H_A15_D_AD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_H_AD_D_D15(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_H___LT_AD_P__RT__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_A_A15__LT_A10_RT__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_A_AD__LT_AD_P__RT_(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_A_AD__LT_AD_RT_(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_A_AD_D_A15(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_A_A15_D_AD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_W_D15__LT_AD_RT__D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_W_DD__LT_A15_RT__D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_W_DD__LT_AD_P__RT_(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_W__LT_DD_RT__AD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_W__LT_A10_RT___ID_D15(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_B__LT_AD_RT_D_D15(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_B_D__LT_A15_RT__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_B_AD__LT_DD_P__RT_(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_B_AD__LT_DD_RT_(tricore_inst_t *inst) {
	return 0;

}

static int prod_LD_H_DD__LT_AD_RT_(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_H_DD__LT_A15_RT__D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_H_DD__LT_AD_P__RT_(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_H_D15__LT_AD_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_A_A15__LT_A10_RT__ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_LD_A_AD__LT_AD_RT_(tricore_inst_t *inst) {
	return 2;

}

static int prod_LD_A_AD__LT_AD_P__RT_(tricore_inst_t *inst) {
	return 2;

}

static int prod_LD_A_AD__LT_A15_RT_D(tricore_inst_t *inst) {
	return 2;

}

static int prod_LD_A_A15__LT_AD_RT_D(tricore_inst_t *inst) {
	return 2;

}

static int prod_LD_W_D15__LT_AD_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_W_DD__LT_A15_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_W_AD__LT_DD_P__RT_(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_W_AD__LT_DD_RT_(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_W_D15__LT_A10_RT__ID(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_BU_D15__LT_AD_RT__D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_BU_DD__LT_A15_RT__D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_BU_DD__LT_AD_P__RT_(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_BU_DD__LT_AD_RT_(tricore_inst_t *inst) {
	return 1;

}

static int prod_SAT_HU_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SAT_H_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SAT_BU_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SAT_B_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_CADDN_DD__D15__D(tricore_inst_t *inst) {
	return 0;

}

static int prod_MOV_D_DD_AD(tricore_inst_t *inst) {
	return 0;

}

static int prod_MOV_AA_AD_AD(tricore_inst_t *inst) {
	return 0;

}

static int prod_MOV_A_AD_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_MOV_A_AD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_MOV_ED__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_MOV_D15___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_MOV_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_MOV_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SHA_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_SH_DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_LT_D15__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_LT_D15__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_NOT_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_XOR_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_OR_D15__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_OR_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_EQ_D15_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_EQ_D15_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_AND_D15__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_AND_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_MUL_DD_DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_CMOVN_DD__D15__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_CMOVN_DD__D15___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_CMOV_DD__D15___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_CMOV_DD__D15__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SUBS_DD__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_RSUB_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SUB_A_A10__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_SUB_DD_D15_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SUB_D15_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SUB_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ADDS_DD_DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_CADD_DD_D15__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_ADDSC_A_AD_AD_D15_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_ADD_A_AD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_ADD_A_AD_AD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ADD_DD_D15__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_ADD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ADD_D15_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_ADD_D15_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ADD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_ADD_DD_DD_0(tricore_inst_t *inst) {
	return 0;

}

static int prod_YIELD(tricore_inst_t *inst) {
return 2;
}

static int prod_TLBPROBE_I_DD(tricore_inst_t *inst) {
return 2;
}

static int prod_TLBPROBE_A_DD(tricore_inst_t *inst) {
return 2;
}

static int prod_TLBMAP_ED(tricore_inst_t *inst) {
return 2;
}

static int prod_TLBFLUSH_B(tricore_inst_t *inst) {
return 2;
}

static int prod_TLBFLUSH_A(tricore_inst_t *inst) {
return 2;
}

static int prod_TLBDEMAP_DD(tricore_inst_t *inst) {
return 2;
}

static int prod_WAIT(tricore_inst_t *inst) {
	return 0;

}

static int prod_SWAPMSK_W__LT__P_AD_RT_D__ED(tricore_inst_t *inst) {
	return 2;

}

static int prod_SWAPMSK_W__LT_AD_P__RT_D__ED(tricore_inst_t *inst) {
	return 2;

}

static int prod_SWAPMSK_W__LT_PD_P_C_RT_D__ED(tricore_inst_t *inst) {
	return 2;

}

static int prod_SWAPMSK_W__LT_PD_P_R_RT___ED(tricore_inst_t *inst) {
	return 2;

}

static int prod_SWAPMSK_W__LT_AD_RT_D__ED(tricore_inst_t *inst) {
	return 2;

}

static int prod_CRC32_DD__DD__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_CMPSWAP_W__LT__P_AD_RT_D__ED(tricore_inst_t *inst) {
	return 2;

}

static int prod_CMPSWAP_W__LT_AD_P__RT_D__ED(tricore_inst_t *inst) {
	return 2;

}

static int prod_CMPSWAP_W__LT_PD_P_C_RT_D__ED(tricore_inst_t *inst) {
	return 2;

}

static int prod_CMPSWAP_W__LT_PD_P_R_RT___ED(tricore_inst_t *inst) {
	return 2;

}

static int prod_CMPSWAP_W__LT_AD_RT_D__ED(tricore_inst_t *inst) {
	return 2;

}

static int prod_CACHEI_WI__LT__P_AD_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_CACHEI_WI__LT_AD_P__RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_CACHEI_WI_AD__D(tricore_inst_t *inst) {
	return 0;

}

static int prod_CACHEI_I__LT__P_AD_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_CACHEI_I__LT_AD_P__RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_CACHEI_I_AD__D(tricore_inst_t *inst) {
	return 0;

}

static int prod_CACHEI_W__LT__P_AD_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_CACHEI_W__LT_AD_P__RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_CACHEI_W_AD__D(tricore_inst_t *inst) {
	return 0;

}

static int prod_CACHEA_WI__LT__P_AD_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_CACHEA_WI__LT_AD_P__RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_CACHEA_WI__LT_PD_P_C_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_CACHEA_WI__LT_PD_P_R_RT_(tricore_inst_t *inst) {
	return 0;

}

static int prod_CACHEA_WI_AD__D(tricore_inst_t *inst) {
	return 0;

}

static int prod_CACHEA_W__LT__P_AD_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_CACHEA_W__LT_AD_P__RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_CACHEA_W__LT_PD_P_C_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_CACHEA_W__LT_PD_P_R_RT_(tricore_inst_t *inst) {
	return 0;

}

static int prod_CACHEA_W_AD__D(tricore_inst_t *inst) {
	return 0;

}

static int prod_CACHEA_I__LT__P_AD_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_CACHEA_I__LT_AD_P__RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_CACHEA_I__LT_PD_P_C_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_CACHEA_I__LT_PD_P_R_RT_(tricore_inst_t *inst) {
	return 0;

}

static int prod_CACHEA_I_AD__D(tricore_inst_t *inst) {
	return 0;

}

static int prod_SVLCX(tricore_inst_t *inst) {
	return 3;

}

static int prod_SWAP_W__LT__P_AD_RT_D__DD(tricore_inst_t *inst) {
	return 2;

}

static int prod_SWAP_W__LT_AD_P__RT_D__DD(tricore_inst_t *inst) {
	return 2;

}

static int prod_SWAP_W__LT_PD_P_C_RT_D__DD(tricore_inst_t *inst) {
	return 2;

}

static int prod_SWAP_W__LT_PD_P_R_RT___DD(tricore_inst_t *inst) {
	return 2;

}

static int prod_SWAP_W__LT_AD_RT_D__DD(tricore_inst_t *inst) {
	return 2;

}

static int prod_SWAP_W_D__DD(tricore_inst_t *inst) {
	return 2;

}

static int prod_LEA_AD__LT_AD_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_LEA_AD__AD__D(tricore_inst_t *inst) {
	return 0;

}

static int prod_LEA_AD__D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_Q_DD__LT__P_PD_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_Q_DD__LT_AD_P__RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_Q_DD__LT_PD_P_C_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_Q_DD__LT_PD_P_R_RT_(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_Q_DD__LT_AD_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_Q_DD__LT_D_RT_(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_DA_PD__LT_PD_P_C_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_DA_PD__LT_PD_P_R_RT_(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_DA_PD__LT_AD_P__RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_DA_PD__LT__P_AD_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_DA_PD__D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_DA__LT_AD_RT__D_PD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_T_D_D_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_W_DD__LT_AD_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_W_DD__LT_PD_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_W__LT__P_AD_RT__D_ED(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_W__LT_AD_P__RT__D_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_W_DD__LT_PD_RT_(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_W__LT_AD_RT__D_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_W_DD__LT_D_RT_(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_H_DD__LT_PD_P_C_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_H_DD__LT_PD_P_R_RT_(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_H_DD___LT_AD_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_H_DD__LT__P_AD_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_H_DD__LT_AD_P__RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_H__LT_AD_RT__D_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_H_DD__LT_D_RT_(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_D_ED__LT_PD_P_C_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_D_ED__LT_PD_P_R_RT_(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_D_ED__LT_AD_P__RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_D_ED__LT__P_AD_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_D_ED__D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_D__LT_AD_RT__D_ED(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_B_AD___LT_PD_P_C_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_B_AD___LT_PD_P_R_RT_(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_B__LT_AD_RT_D__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_B_DD___LT__P_AD_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_B_DD___LT_AD_P__RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_B__LT_AD_RT_D__DD_0(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_B_DD_LT_D_RT_(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_A__LT_AD_RT_D__AD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_A_AD___LT__P_PD_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_A_AD___LT_AD_P__RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_A_AD___LT_PD_P_C_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_A_AD___LT_PD_P_R_RT_(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_A__LT_AD_RT_D__AD_0(tricore_inst_t *inst) {
	return 0;

}

static int prod_ST_A_AD___LT_D_RT_(tricore_inst_t *inst) {
	return 0;

}

static int prod_LD_Q_DD__LT__P_PD_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_Q_DD__LT_AD_P__RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_Q_DD__LT_PD_P_C_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_Q_DD__LT_PD_P_R_RT_(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_Q_DD__LT_AD_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_Q_DD__LT_D_RT_(tricore_inst_t *inst) {
	return 1;

}

static int prod_LDMST_ED__LT_AD_P_C_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_LDMST_ED__LT_AD_P_R_RT_(tricore_inst_t *inst) {
	return 0;

}

static int prod_LDMST_ED__LT__P_AD_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_LDMST_DD__LT_PD_P__RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_LDMST_DD__LT_AD_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_LDMST_ED__LT_D_RT_(tricore_inst_t *inst) {
	return 0;

}

static int prod_LD_W_DD__LT_PD_P_C_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_W_DD__LT_PD_P_R_RT_(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_W_DD__LT_AD_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_W_DD__LT__P_AD_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_W_DD__LT_AD_P__RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_W_DD__LT_AD_RT_D_0(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_W_DD_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_HU_DD___LT_AD_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_HU_DD__LT__P_PD_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_HU_DD__LT_AD_P__RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_HU_DD__LT_PD_P_C_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_HU_DD__LT_PD_P_R_RT_(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_HU_DD__LT_AD_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_HU_DD__LT_D_RT_(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_H_DD___LT_AD_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_H_DD__LT__P_PD_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_H_DD__LT_AD_P__RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_H_DD__LT_PD_P_C_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_H_DD__LT_PD_P_R_RT_(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_H_DD__LT_AD_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_H_DD__LT_D_RT_(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_DA_PD__LT_PD_P_C_RT_D(tricore_inst_t *inst) {
	return 2;

}

static int prod_LD_DA_PD__LT_PD_P_R_RT_(tricore_inst_t *inst) {
	return 2;

}

static int prod_LD_DA_PD__LT__P_AD_RT_D(tricore_inst_t *inst) {
	return 2;

}

static int prod_LD_DA_PD__D(tricore_inst_t *inst) {
	return 2;

}

static int prod_LD_DA_PD__LT_AD_P__RT_D(tricore_inst_t *inst) {
	return 2;

}

static int prod_LD_DA_PD__LT_AD_RT_D(tricore_inst_t *inst) {
	return 2;

}

static int prod_LD_D_ED__LT_PD_P_C_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_D_ED__LT_PD_P_R_RT_(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_D_ED__LT__P_AD_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_D_ED__D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_D_ED__LT_AD_P__RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_D_ED__LT_AD_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_BU_AD__LT_PD_P_R_RT_(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_BU_AD__LT_PD_P_C_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_BU_DD__LT_AD_P__RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_BU_DD__LT__P_AD_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_BU_DD___LT_AD_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_BU_DD__LT_AD_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_BU_DD__LT_D_RT_(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_B_AD__LT_PD_P_C_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_B_AD__LT_PD_P_R_RT_(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_B_DD___LT_AD_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_B_DD__LT_D_RT_(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_B_DD__LT__P_AD_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_B_DD__LT_AD_P__RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_B_DD__LT_AD_RT_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_LD_A_AD__LT_PD_P_R_RT_(tricore_inst_t *inst) {
	return 2;

}

static int prod_LD_A_AD__LT_PD_P_C_RT_D(tricore_inst_t *inst) {
	return 2;

}

static int prod_LD_A_AD__LT_AD_P__RT_D(tricore_inst_t *inst) {
	return 2;

}

static int prod_LD_A_AD__LT_AD_RT_D(tricore_inst_t *inst) {
	return 2;

}

static int prod_LD_A_AD__LT__P_AD_RT_D(tricore_inst_t *inst) {
	return 2;

}

static int prod_LD_A_AD__LT_AD_RT_D_0(tricore_inst_t *inst) {
	return 2;

}

static int prod_LD_A_AD__D(tricore_inst_t *inst) {
	return 2;

}

static int prod_TRAPV(tricore_inst_t *inst) {
	return 0;

}

static int prod_TRAPSV(tricore_inst_t *inst) {
	return 0;

}

static int prod_RSLCX(tricore_inst_t *inst) {
	return 3;

}

static int prod_RFM(tricore_inst_t *inst) {
	return 3;

}

static int prod_RFE_0(tricore_inst_t *inst) {
	return 3;

}

static int prod_BISR__ID_0(tricore_inst_t *inst) {
	return 3;

}

static int prod_STUCX__LT_AD_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_STUCX_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_STLCX__LT_AD_RT_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_STLCX_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_LDUCX__LT_AD_RT_D(tricore_inst_t *inst) {
	return 8;

}

static int prod_LDUCX_D(tricore_inst_t *inst) {
	return 8;

}

static int prod_LDLCX__LT_AD_RT_D(tricore_inst_t *inst) {
	return 8;

}

static int prod_LDLCX_D(tricore_inst_t *inst) {
	return 8;

}

static int prod_RSTV(tricore_inst_t *inst) {
	return 0;

}

static int prod_UNTIE(tricore_inst_t *inst) {
return 2;
}

static int prod_TIE(tricore_inst_t *inst) {
return 2;
}

static int prod_SYSCALL_D(tricore_inst_t *inst) {
	return 3;

}

static int prod_NOP_0(tricore_inst_t *inst) {
	return 0;

}

static int prod_ENABLE(tricore_inst_t *inst) {
	return 0;

}

static int prod_RESTORE_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_DISABLE_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_DISABLE(tricore_inst_t *inst) {
	return 0;

}

static int prod_DEBUG_0(tricore_inst_t *inst) {
	return 0;

}

static int prod_MFCR_DD_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_MTCR_D_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ISYNC(tricore_inst_t *inst) {
	return 0;

}

static int prod_DSYNC(tricore_inst_t *inst) {
	return 0;

}

static int prod_FRET_0(tricore_inst_t *inst) {
	return 0;

}

static int prod_FCALLI_AD(tricore_inst_t *inst) {
	return 0;

}

static int prod_FCALLA_08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_FCALL_08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JLI_AD(tricore_inst_t *inst) {
	return 0;

}

static int prod_JLA_08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_LOOPU_08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_LOOP_AD__08X_0(tricore_inst_t *inst) {
	return 0;

}

static int prod_RET_0(tricore_inst_t *inst) {
	return 3;

}

static int prod_JZ_T_DD_D__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JZ_A_AD__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JNZ_T_DD__D__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JNZ_A_AD__08X_0(tricore_inst_t *inst) {
	return 0;

}

static int prod_JNEI_DD__D__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JNEI_DD__DD__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JNED_DD__D__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JNED_DD__DD__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JNE_A_AD__AD__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JNE_DD__D__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JNE_DD__DD__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JLT_U_DD__D__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JLT_U_DD__DD__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JLT_DD__D__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JLT_DD__DD__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JL_08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JI_AD_0(tricore_inst_t *inst) {
	return 0;

}

static int prod_JGE_U_DD__D__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JGE_U_DD__DD__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JGE_DD__D__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JGE_DD__DD__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JEQ_A_AD__AD__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JEQ_DD__DD__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JEQ_DD___ID__08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_JA_08X(tricore_inst_t *inst) {
	return 0;

}

static int prod_J_08X_0(tricore_inst_t *inst) {
	return 0;

}

static int prod_CALLA_08X(tricore_inst_t *inst) {
	return 3;

}

static int prod_CALLI_AD(tricore_inst_t *inst) {
	return 3;

}

static int prod_CALL_08X_0(tricore_inst_t *inst) {
	return 3;

}

static int prod_IXMIN_U_ED__ED__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_IXMIN_ED__ED__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_IXMAX_U_ED__ED__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_IXMAX_ED__ED__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_SAT_HU_DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SAT_H_DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SAT_BU_DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SAT_B_DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_PARITY_DD__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_SUB_F_DD__DD__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_UTOF_DD__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_UPDFL_DD(tricore_inst_t *inst) {
return 2;
}

static int prod_QSEED_F_DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_Q31TOF_DD__DD__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_MSUB_F_DD__DD__DD__DD(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADD_F_DD__DD__DD__DD(tricore_inst_t *inst) {
	return 2;

}

static int prod_ITOF_DD__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_FTOUZ_DD__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_FTOU_DD__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_FTOQ31Z_DD__DD__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_FTOQ31_DD__DD__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_FTOIZ_DD__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_FTOI_DD__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_DIV_F_DD__DD__DD(tricore_inst_t *inst) {
	return 7;

}

static int prod_MUL_F_DD__DD__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_ADD_F_DD__DD__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_CMP_F_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_MADDSURS_H_DD__DD__DD__DDUU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDSURS_H_DD__DD__DD__DDUL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDSURS_H_DD__DD__DD__DDLU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDSURS_H_DD__DD__DD__DDLL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDSUR_H_DD__DD__DD__DDUU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDSUR_H_DD__DD__DD__DDUL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDSUR_H_DD__DD__DD__DDLU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDSUR_H_DD__DD__DD__DDLL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDSUMS_H_ED__ED__DD__DDUU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDSUMS_H_ED__ED__DD__DDUL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDSUMS_H_ED__ED__DD__DDLU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDSUMS_H_ED__ED__DD__DDLL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDSUM_H_ED__ED__DD__DDUU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDSUM_H_ED__ED__DD__DDUL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDSUM_H_ED__ED__DD__DDLU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDSUM_H_ED__ED__DD__DDLL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDSUS_H_ED__ED__DD__DDUU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDSUS_H_ED__ED__DD__DDUL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDSUS_H_ED__ED__DD__DDLU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDSUS_H_ED__ED__DD__DDLL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDSU_H_ED__ED__DD__DDUU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDSU_H_ED__ED__DD__DDUL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDSU_H_ED__ED__DD__DDLU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDSU_H_ED__ED__DD__DDLL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDRS_Q_DD__DD__DDU__DDU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDRS_Q_DD__DD__DDL__DDL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDR_Q_DD__DD__DDU__DDU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDR_Q_DD__DD__DDL__DDL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDRS_H_DD__ED__DD__DDUL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDRS_H_DD__DD__DD__DDUU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDRS_H_DD__DD__DD__DDUL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDRS_H_DD__DD__DD__DDLU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDRS_H_DD__DD__DD__DDLL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDR_H_DD__ED__DD__DDUL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDR_H_DD__DD__DD__DDUU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDR_H_DD__DD__DD__DDUL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDR_H_DD__DD__DD__DDLU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDR_H_DD__DD__DD__DDLL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDS_Q_ED__ED__DDU__DDU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDS_Q_DD__DD__DDU__DDU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDS_Q_ED__ED__DDL__DDL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDS_Q_DD__DD__DDL__DDL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDS_Q_ED__ED__DD__DDU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDS_Q_DD__DD__DD__DDU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDS_Q_ED__ED__DD__DDL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDS_Q_DD__DD__DD__DDL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDS_Q_ED__ED__DD__DD___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDS_Q_DD__DD__DD__DD___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADD_Q_ED__ED__DDU__DDU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADD_Q_DD__DD__DDU__DDU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADD_Q_ED__ED__DDL__DDL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADD_Q_DD__DD__DDL__DDL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADD_Q_ED__ED__DD__DDU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADD_Q_DD__DD__DD__DDU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADD_Q_ED__ED__DD__DDL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADD_Q_DD__DD__DD__DDL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADD_Q_ED__ED__DD__DD___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADD_Q_DD__DD__DD__DD___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDS_H_ED__ED__DD__DDUU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDS_H_ED__ED__DD__DDUL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDS_H_ED__ED__DD__DDLU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDS_H_ED__ED__DD__DDLL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADD_H_ED__ED__DD__DDUU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADD_H_ED__ED__DD__DDUL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADD_H_ED__ED__DD__DDLU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADD_H_ED__ED__DD__DDLL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDMS_H_ED__ED__DD__DD_LU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDMS_H_ED__ED__DD__DD_UL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDMS_H_ED__ED__DD__DD_UU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDMS_H_ED__ED__DD__DD_LL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDM_H_ED__ED__DD__DD_UU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDM_H_ED__ED__DD__DD_UL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDM_H_ED__ED__DD__DD_LU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDM_H_ED__ED__DD__DD_LL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDS_U_ED__ED__DD___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDS_U_ED__ED__DD__DD(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDS_U_DD__DD__DD___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDS_U_DD__DD__DD__DD(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDS_ED__ED__DD___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDS_ED__ED__DD__DD(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDS_DD__DD__DD___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADDS_DD__DD__DD__DD(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADD_ED__ED__DD___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADD_ED__ED__DD__DD(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADD_DD__DD__DD___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADD_DD__DD__DD__DD(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADD_U_ED__ED__DD___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MADD_U_ED__ED__DD__DD(tricore_inst_t *inst) {
	return 2;

}

static int prod_CLO_H_DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_CLO_DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_CLZ_H_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_CLZ_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_CLS_H_DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_CLS_DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_UNPACK_ED__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_PACK_DD__ED__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_BSPLIT_ED__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_BMERGE_DD__DD__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_IMASK_ED_DD_DD__ID(tricore_inst_t *inst) {
	return 1;

}

static int prod_IMASK_ED_DD__ID__ID(tricore_inst_t *inst) {
	return 1;

}

static int prod_IMASK_ED__ID_DD__ID(tricore_inst_t *inst) {
	return 1;

}

static int prod_IMASK_ED__ID__ID__ID(tricore_inst_t *inst) {
	return 1;

}

static int prod_SH_XOR_T_DD__DD__DD___ID___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_SH_XNOR_T_DD__DD__DD___ID___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_SH_ORN_T_DD__DD__DD___ID___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_SH_OR_T_DD__DD__DD___ID___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_SH_NOR_T_DD__DD__DD___ID___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_SH_NAND_T_DD__DD__DD___ID___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_SH_ANDN_T_DD__DD__DD___ID___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_SH_AND_T_DD__DD__DD___ID___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_SH_NE_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_SH_NE_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SH_LT_U_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_SH_LT_U_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SH_LT_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_SH_LT_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SH_H_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_SH_H_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SH_GE_U_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_SH_GE_U_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SH_GE_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_SH_GE_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SH_EQ_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SH_EQ_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_SHAS_DD__DD___ID(tricore_inst_t *inst) {
	return 1;

}

static int prod_SHAS_DD__DD__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_SHA_H_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_SHA_H_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SHA_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_SHA_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SH_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_SH_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_INSN_T_DD_DD__ID_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_INS_T_DD_DD__ID_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_INSERT_DD_DD__ID__ID__ID(tricore_inst_t *inst) {
	return 1;

}

static int prod_INSERT_DD_DD__ID_DD__ID(tricore_inst_t *inst) {
	return 1;

}

static int prod_INSERT_DD_DD__ID_ED(tricore_inst_t *inst) {
	return 1;

}

static int prod_INSERT_DD_DD_DD__ID__ID(tricore_inst_t *inst) {
	return 1;

}

static int prod_INSERT_DD_DD_DD_DD__ID(tricore_inst_t *inst) {
	return 1;

}

static int prod_INSERT_DD_DD_DD_ED(tricore_inst_t *inst) {
	return 1;

}

static int prod_EXTR_DD_DD_DD__ID(tricore_inst_t *inst) {
	return 1;

}

static int prod_EXTR_DD_DD_ED(tricore_inst_t *inst) {
	return 1;

}

static int prod_EXTR_U_DD_DD_D_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_EXTR_U_DD_DD_DD__ID(tricore_inst_t *inst) {
	return 1;

}

static int prod_EXTR_U_DD_DD_ED(tricore_inst_t *inst) {
	return 1;

}

static int prod_EXTR_DD_DD_D__ID(tricore_inst_t *inst) {
	return 1;

}

static int prod_DEXTR_DD_DD_DD_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_DEXTR_DD_DD_DD_DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_NAND_T_DD__DD___ID__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_NAND_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_NAND_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_MAX_HU_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_MAX_H_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_MAX_BU_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_MAX_B_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_MAX_U_DD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_MAX_U_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_MAX_DD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_MAX_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_NOR_T_DD_DD__ID_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_NOR_DD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_NOR_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_MIN_HU_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_MIN_H_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_MIN_B_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_MIN_BU_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_MIN_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_MIN_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_MIN_U_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_MIN_U_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ORN_T_DD__DD___ID__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_ORN_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ORN_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_LT_WU_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_LT_W_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_LT_H_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_LT_H_DD__DD__DD_0(tricore_inst_t *inst) {
	return 0;

}

static int prod_LT_BU_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_LT_B_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_LT_A_DD__AD__AD(tricore_inst_t *inst) {
	return 0;

}

static int prod_LT_U_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_LT_U_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_LT_DD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_LT_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_XOR_LT_U_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_XOR_LT_U_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_XOR_LT_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_XOR_LT_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_XOR_GE_U_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_XOR_GE_U_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_XOR_GE_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_XOR_GE_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_XOR_NE_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_XOR_NE_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_XOR_EQ_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_XOR_EQ_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_XNOR_T_DD__DD___ID__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_XNOR_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_XNOR_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_XOR_T_DD__DD___ID__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_XOR_DD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_XOR_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_OR_OR_T_DD__DD___ID__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_OR_NOR_T_DD__DD___ID__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_OR_ANDN_T_DD__DD___ID__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_OR_AND_T_DD__DD___ID__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_OR_EQ_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_OR_EQ_DD__DD__D(tricore_inst_t *inst) {
	return 0;

}

static int prod_OR_T_DD_DD__ID_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_OR_NE_DD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_OR_NE_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_OR_GE_U_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_OR_GE_U_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_OR_GE_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_OR_GE_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_OR_LT_U_DD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_OR_LT_U_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_OR_LT_DD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_OR_LT_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_OR_DD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_OR_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_NE_A_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_NEZ_A_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_NE_DD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_NE_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_GE_U_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_GE_A_DD_AD_AD(tricore_inst_t *inst) {
	return 0;

}

static int prod_GE_U_DD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_GE_DD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_GE_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_EQZ_A_DD__AD(tricore_inst_t *inst) {
	return 0;

}

static int prod_EQ_B_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_EQ_H_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_EQ_W_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_EQ_A_DD__AD__AD(tricore_inst_t *inst) {
	return 0;

}

static int prod_EQANY_H_DD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_EQANY_H_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_EQANY_B_DD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_EQANY_B_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_EQ_DD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_EQ_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_AND_GE_U_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_AND_GE_U_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_AND_GE_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_AND_GE_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_AND_NE_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_AND_NE_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_AND_OR_T_DD__DD___ID__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_AND_ANDN_T_DD__DD___ID__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_AND_AND_T_DD__DD___ID__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_AND_EQ_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_AND_EQ_DD__DD__D(tricore_inst_t *inst) {
	return 0;

}

static int prod_AND_T_DD_DD__ID_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_AND_NOR_T_DD_DD_D_DD_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ANDN_T_DD__DD___ID__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_ANDN_DD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_ANDN_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_AND_LT_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_AND_LT_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_AND_LT_U_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_AND_LT_U_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_AND_DD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_AND_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SUBS_U_DD__DD__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_SUBS_HU_DD_DD_DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_SUBS_H_DD_DD_DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_SUBS_DD__DD__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_SUB_H_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SUB_B_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_RSUBS_U_DD__DD___ID(tricore_inst_t *inst) {
	return 1;

}

static int prod_RSUBS_DD__DD___ID(tricore_inst_t *inst) {
	return 1;

}

static int prod_RSUB_DD__DD___ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_MSUBRS_Q_DD__DD__DDU__DDU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBRS_Q_DD__DD__DDL__DDL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBR_Q_DD__DD__DDU__DDU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBR_Q_DD__DD__DDL__DDL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBRS_H_DD__ED__DD__DDUL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBR_H_DD__ED__DD__DDUL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBRS_H_DD__DD__DD__DDUU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBRS_H_DD__DD__DD__DDUL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBRS_H_DD__DD__DD__DDLU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBRS_H_DD__DD__DD__DDLL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBR_H_DD__DD__DD__DDUU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBR_H_DD__DD__DD__DDUL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBR_H_DD__DD__DD__DDLU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBR_H_DD__DD__DD__DDLL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBMS_H_ED__ED__DD__DDUU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBMS_H_ED__ED__DD__DDUL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBMS_H_ED__ED__DD__DDLU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBMS_H_ED__ED__DD__DDLL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBM_H_ED__ED__DD__DDUU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBM_H_ED__ED__DD__DDUL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBM_H_ED__ED__DD__DDLU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBM_H_ED__ED__DD__DDLL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBADRS_H_DD__DD__DD__DDUU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBADRS_H_DD__DD__DD__DDUL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBADRS_H_DD__DD__DD__DDLU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBADRS_H_DD__DD__DD__DDLL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBADR_H_DD__DD__DD__DDUU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBADR_H_DD__DD__DD__DDUL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBADR_H_DD__DD__DD__DDLU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBADR_H_DD__DD__DD__DDLL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBADMS_H_ED__ED__DD__DDUU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBADMS_H_ED__ED__DD__DDUL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBADMS_H_ED__ED__DD__DDLU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBADMS_H_ED__ED__DD__DDLL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBADM_H_ED__ED__DD__DDUU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBADM_H_ED__ED__DD__DDUL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBADM_H_ED__ED__DD__DDLU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBADM_H_ED__ED__DD__DDLL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBADS_H_ED__ED__DD__DDUU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBADS_H_ED__ED__DD__DDUL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBADS_H_ED__ED__DD__DDLU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBADS_H_ED__ED__DD__DDLL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBAD_H_ED__ED__DD__DDUU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBAD_H_ED__ED__DD__DDUL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBAD_H_ED__ED__DD__DDLU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBAD_H_ED__ED__DD__DDLL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBS_Q_ED__ED__DDU__DDU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBS_Q_DD__DD__DDU__DDU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBS_Q_ED__ED__DDL__DDL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBS_Q_DD__DD__DDL__DDL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBS_Q_ED__ED__DD__DDU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBS_Q_DD__DD__DD__DDU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBS_Q_ED__ED__DD__DDL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBS_Q_DD__DD__DD__DDL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBS_Q_ED__ED__DD__DD___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBS_Q_DD__DD__DD__DD___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUB_Q_ED__ED__DDU__DDU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUB_Q_DD__DD__DDU__DDU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUB_Q_ED__ED__DDL__DDL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUB_Q_DD__DD__DDL__DDL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUB_Q_ED__ED__DD__DDU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUB_Q_DD__DD__DD__DDU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUB_Q_ED__ED__DD__DDL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUB_Q_DD__DD__DD__DDL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUB_Q_ED__ED__DD__DD___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUB_Q_DD__DD__DD__DD___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBS_H_ED__ED__DD__DDUU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBS_H_ED__ED__DD__DDUL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBS_H_ED__ED__DD__DDLU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBS_H_ED__ED__DD__DDLL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUB_H_ED__ED__DD__DDUU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUB_H_ED__ED__DD__DDUL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUB_H_ED__ED__DD__DDLU___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUB_H_ED__ED__DD__DDLL___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBS_U_ED__ED__DD___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBS_U_ED__ED__DD__DD(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBS_U_DD__DD__DD___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBS_U_DD__DD__DD__DD(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUB_U_ED__ED__DD___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUB_U_ED__ED__DD__DD(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBS_ED__ED__DD___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBS_ED__ED__DD__DD(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBS_DD__DD__DD___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUBS_DD__DD__DD__DD(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUB_ED__ED__DD___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUB_ED__ED__DD__DD(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUB_DD__DD__DD___ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MSUB_DD__DD__DD__DD(tricore_inst_t *inst) {
	return 2;

}

static int prod_CSUBN_DD_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_CSUB_DD_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SUB_A_AD_AD_AD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SUBX_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SUBC_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SUB_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SELN_DD_DD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_SELN_DD_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_SEL_DD_DD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_SEL_DD_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_MULR_Q_DD__DDU__DDU__D(tricore_inst_t *inst) {
	return 2;

}

static int prod_MULR_Q_DD__DDL__DDL__D(tricore_inst_t *inst) {
	return 2;

}

static int prod_MULR_H_DD__DD__DDUU__D(tricore_inst_t *inst) {
	return 2;

}

static int prod_MULR_H_DD__DD__DDUL__D(tricore_inst_t *inst) {
	return 2;

}

static int prod_MULR_H_DD__DD__DDLU__D(tricore_inst_t *inst) {
	return 2;

}

static int prod_MULR_H_DD__DD__DDLL__D(tricore_inst_t *inst) {
	return 2;

}

static int prod_MULM_H_ED__DD__DDUU__D(tricore_inst_t *inst) {
	return 2;

}

static int prod_MULM_H_ED__DD__DDUL__D(tricore_inst_t *inst) {
	return 2;

}

static int prod_MULM_H_ED__DD__DDLU__D(tricore_inst_t *inst) {
	return 2;

}

static int prod_MULM_H_ED__DD__DDLL__D(tricore_inst_t *inst) {
	return 2;

}

static int prod_MUL_Q_DD__DDU__DDU__D(tricore_inst_t *inst) {
	return 2;

}

static int prod_MUL_Q_DD__DDL__DDL__D(tricore_inst_t *inst) {
	return 2;

}

static int prod_MUL_Q_ED__DD__DDU__D(tricore_inst_t *inst) {
	return 2;

}

static int prod_MUL_Q_DD__DD__DDU__D(tricore_inst_t *inst) {
	return 2;

}

static int prod_MUL_Q_ED__DD__DDL__D(tricore_inst_t *inst) {
	return 2;

}

static int prod_MUL_Q_DD__DD__DDL__D(tricore_inst_t *inst) {
	return 2;

}

static int prod_MUL_Q_ED__DD__DD__D(tricore_inst_t *inst) {
	return 2;

}

static int prod_MUL_Q_DD__DD__DD__D(tricore_inst_t *inst) {
	return 2;

}

static int prod_MUL_H_ED_DD_DD_UU_D(tricore_inst_t *inst) {
	return 2;

}

static int prod_MUL_H_ED_DD_DD_UL_D(tricore_inst_t *inst) {
	return 2;

}

static int prod_MUL_H_ED_DD_DD_LU_D(tricore_inst_t *inst) {
	return 2;

}

static int prod_MUL_H_ED_DD_DD_LL_D(tricore_inst_t *inst) {
	return 2;

}

static int prod_MULS_U_DD_DD__ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MULS_U_DD_DD_DD(tricore_inst_t *inst) {
	return 2;

}

static int prod_MULS_DD_DD__ID(tricore_inst_t *inst) {
	return 2;

}

static int prod_MULS_DD_DD_DD(tricore_inst_t *inst) {
	return 2;

}

static int prod_MUL_U_ED_DD_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_MUL_U_ED_DD_DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_MUL_ED_DD__ID(tricore_inst_t *inst) {
	return 1;

}

static int prod_MUL_DD_DD__ID(tricore_inst_t *inst) {
	return 1;

}

static int prod_MUL_ED_DD_DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_MUL_DD_DD_DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_MOV_U_DD_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_MOVH_A_AD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_MOVH_DD_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_MOV_D_DD_AD_0(tricore_inst_t *inst) {
	return 0;

}

static int prod_MOV_AA_AD_AD_0(tricore_inst_t *inst) {
	return 0;

}

static int prod_MOV_A_AD_DD_0(tricore_inst_t *inst) {
	return 0;

}

static int prod_MOV_ED__DD__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_MOV_ED_DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_MOV_ED__ID_0(tricore_inst_t *inst) {
	return 1;

}

static int prod_MOV_DD__ID_0(tricore_inst_t *inst) {
	return 0;

}

static int prod_MOV_DD_DD_0(tricore_inst_t *inst) {
	return 0;

}

static int prod_DIV_U_ED__DD__DD(tricore_inst_t *inst) {
	return 10;

}

static int prod_DIV_ED__DD__DD(tricore_inst_t *inst) {
	return 10;

}

static int prod_DVSTEP_U_ED_ED_DD(tricore_inst_t *inst) {
	return 5;

}

static int prod_DVSTEP_ED_ED_DD(tricore_inst_t *inst) {
	return 5;

}

static int prod_DVINIT_U_ED_DD_DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_DVINIT_HU_ED_DD_DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_DVINIT_H_ED_DD_DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_DVINIT_B_ED_DD_DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_DVINIT_BU_ED_DD_DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_DVINIT_ED_DD_DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_DVADJ_ED_ED_DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_ADD_H_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ADD_B_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ADDS_U_DD_DD__ID(tricore_inst_t *inst) {
	return 1;

}

static int prod_ADDS_U_DD_DD_DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_CADDN_DD_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_CADDN_DD_DD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_ADDX_DD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_ADDSC_AT_AD_AD_AD(tricore_inst_t *inst) {
	return 1;

}

static int prod_ADDS_HU_DD_DD_DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_ADDS_H_DD_DD_DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_ADDS_DD_DD__ID(tricore_inst_t *inst) {
	return 1;

}

static int prod_ADDS_DD_DD_DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_CADD_DD_DD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_CADD_DD_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ADDX_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ADDSC_A_AD_AD_AD_D(tricore_inst_t *inst) {
	return 1;

}

static int prod_ADDIH_A_AD_AD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_ADDIH_DD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_ADDI_DD_DD_D(tricore_inst_t *inst) {
	return 0;

}

static int prod_ADDC_DD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_ADDC_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ADD_A_AD_AD_AD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ADD_DD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_ADD_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ABSS_H_DD__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_ABS_H_DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ABS_B_DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ABSS_DD__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_ABS_DD_DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ABSDIFS_H_DD__DD__DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_ABSDIF_H_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ABSDIF_B_DD__DD__DD(tricore_inst_t *inst) {
	return 0;

}

static int prod_ABSDIFS_DD_DD__ID(tricore_inst_t *inst) {
	return 1;

}

static int prod_ABSDIFS_DD_DD_DD(tricore_inst_t *inst) {
	return 1;

}

static int prod_ABSDIF_DD_DD__ID(tricore_inst_t *inst) {
	return 0;

}

static int prod_ABSDIF_DD_DD_DD(tricore_inst_t *inst) {
	return 0;

}



/* table */
static prod_fun_t prod_tab[] = {
	prod_unknown,
	prod_RFE,
	prod_BISR__ID,
	prod_NOP,
	prod_DEBUG,
	prod_FRET,
	prod_LOOP_AD__08X,
	prod_RET,
	prod_JI_AD,
	prod_JNZ_T_D15__D__08X,
	prod_JNZ_A_AD__08X,
	prod_JNZ_DD__08X,
	prod_JNZ_D15__08X,
	prod_JZ_A_DD__08X,
	prod_JZ_D15__08X,
	prod_JZ_DD__08X,
	prod_JZ_T_D15__D__08X,
	prod_JLTZ_DD__08X,
	prod_JNE_D15__D__08X,
	prod_JNE_D15__DD__08X,
	prod_JNE_D15__D__08X_0,
	prod_JNE_D15__DD__08X_0,
	prod_JLEZ_DD__08X,
	prod_JGTZ_DD__08X,
	prod_JGEZ_DD__08X,
	prod_JEQ_D15__DD__08X,
	prod_JEQ_D15__DD__08X_0,
	prod_JEQ_D15___ID__08X,
	prod_JEQ_D15___ID__08X_0,
	prod_J_08X,
	prod_CALL_08X,
	prod_ST_H___LT_AD_RT__DD,
	prod_ST_H_A15_D_AD,
	prod_ST_H_AD_D_D15,
	prod_ST_H___LT_AD_P__RT__DD,
	prod_ST_A_A15__LT_A10_RT__ID,
	prod_ST_A_AD__LT_AD_P__RT_,
	prod_ST_A_AD__LT_AD_RT_,
	prod_ST_A_AD_D_A15,
	prod_ST_A_A15_D_AD,
	prod_ST_W_D15__LT_AD_RT__D,
	prod_ST_W_DD__LT_A15_RT__D,
	prod_ST_W_DD__LT_AD_P__RT_,
	prod_ST_W__LT_DD_RT__AD,
	prod_ST_W__LT_A10_RT___ID_D15,
	prod_ST_B__LT_AD_RT_D_D15,
	prod_ST_B_D__LT_A15_RT__DD,
	prod_ST_B_AD__LT_DD_P__RT_,
	prod_ST_B_AD__LT_DD_RT_,
	prod_LD_H_DD__LT_AD_RT_,
	prod_LD_H_DD__LT_A15_RT__D,
	prod_LD_H_DD__LT_AD_P__RT_,
	prod_LD_H_D15__LT_AD_RT_D,
	prod_LD_A_A15__LT_A10_RT__ID,
	prod_LD_A_AD__LT_AD_RT_,
	prod_LD_A_AD__LT_AD_P__RT_,
	prod_LD_A_AD__LT_A15_RT_D,
	prod_LD_A_A15__LT_AD_RT_D,
	prod_LD_W_D15__LT_AD_RT_D,
	prod_LD_W_DD__LT_A15_RT_D,
	prod_LD_W_AD__LT_DD_P__RT_,
	prod_LD_W_AD__LT_DD_RT_,
	prod_LD_W_D15__LT_A10_RT__ID,
	prod_LD_BU_D15__LT_AD_RT__D,
	prod_LD_BU_DD__LT_A15_RT__D,
	prod_LD_BU_DD__LT_AD_P__RT_,
	prod_LD_BU_DD__LT_AD_RT_,
	prod_SAT_HU_DD,
	prod_SAT_H_DD,
	prod_SAT_BU_DD,
	prod_SAT_B_DD,
	prod_CADDN_DD__D15__D,
	prod_MOV_D_DD_AD,
	prod_MOV_AA_AD_AD,
	prod_MOV_A_AD_D,
	prod_MOV_A_AD_DD,
	prod_MOV_ED__ID,
	prod_MOV_D15___ID,
	prod_MOV_DD__ID,
	prod_MOV_DD_DD,
	prod_SHA_DD__ID,
	prod_SH_DD___ID,
	prod_LT_D15__DD___ID,
	prod_LT_D15__DD__DD,
	prod_NOT_DD,
	prod_XOR_DD_DD,
	prod_OR_D15__ID,
	prod_OR_DD_DD,
	prod_EQ_D15_DD__ID,
	prod_EQ_D15_DD_DD,
	prod_AND_D15__ID,
	prod_AND_DD_DD,
	prod_MUL_DD_DD,
	prod_CMOVN_DD__D15__DD,
	prod_CMOVN_DD__D15___ID,
	prod_CMOV_DD__D15___ID,
	prod_CMOV_DD__D15__DD,
	prod_SUBS_DD__DD,
	prod_RSUB_DD,
	prod_SUB_A_A10__ID,
	prod_SUB_DD_D15_DD,
	prod_SUB_D15_DD_DD,
	prod_SUB_DD_DD,
	prod_ADDS_DD_DD,
	prod_CADD_DD_D15__ID,
	prod_ADDSC_A_AD_AD_D15_D,
	prod_ADD_A_AD__ID,
	prod_ADD_A_AD_AD,
	prod_ADD_DD_D15__ID,
	prod_ADD_DD_DD,
	prod_ADD_D15_DD__ID,
	prod_ADD_D15_DD_DD,
	prod_ADD_DD__ID,
	prod_ADD_DD_DD_0,
	prod_YIELD,
	prod_TLBPROBE_I_DD,
	prod_TLBPROBE_A_DD,
	prod_TLBMAP_ED,
	prod_TLBFLUSH_B,
	prod_TLBFLUSH_A,
	prod_TLBDEMAP_DD,
	prod_WAIT,
	prod_SWAPMSK_W__LT__P_AD_RT_D__ED,
	prod_SWAPMSK_W__LT_AD_P__RT_D__ED,
	prod_SWAPMSK_W__LT_PD_P_C_RT_D__ED,
	prod_SWAPMSK_W__LT_PD_P_R_RT___ED,
	prod_SWAPMSK_W__LT_AD_RT_D__ED,
	prod_CRC32_DD__DD__DD,
	prod_CMPSWAP_W__LT__P_AD_RT_D__ED,
	prod_CMPSWAP_W__LT_AD_P__RT_D__ED,
	prod_CMPSWAP_W__LT_PD_P_C_RT_D__ED,
	prod_CMPSWAP_W__LT_PD_P_R_RT___ED,
	prod_CMPSWAP_W__LT_AD_RT_D__ED,
	prod_CACHEI_WI__LT__P_AD_RT_D,
	prod_CACHEI_WI__LT_AD_P__RT_D,
	prod_CACHEI_WI_AD__D,
	prod_CACHEI_I__LT__P_AD_RT_D,
	prod_CACHEI_I__LT_AD_P__RT_D,
	prod_CACHEI_I_AD__D,
	prod_CACHEI_W__LT__P_AD_RT_D,
	prod_CACHEI_W__LT_AD_P__RT_D,
	prod_CACHEI_W_AD__D,
	prod_CACHEA_WI__LT__P_AD_RT_D,
	prod_CACHEA_WI__LT_AD_P__RT_D,
	prod_CACHEA_WI__LT_PD_P_C_RT_D,
	prod_CACHEA_WI__LT_PD_P_R_RT_,
	prod_CACHEA_WI_AD__D,
	prod_CACHEA_W__LT__P_AD_RT_D,
	prod_CACHEA_W__LT_AD_P__RT_D,
	prod_CACHEA_W__LT_PD_P_C_RT_D,
	prod_CACHEA_W__LT_PD_P_R_RT_,
	prod_CACHEA_W_AD__D,
	prod_CACHEA_I__LT__P_AD_RT_D,
	prod_CACHEA_I__LT_AD_P__RT_D,
	prod_CACHEA_I__LT_PD_P_C_RT_D,
	prod_CACHEA_I__LT_PD_P_R_RT_,
	prod_CACHEA_I_AD__D,
	prod_SVLCX,
	prod_SWAP_W__LT__P_AD_RT_D__DD,
	prod_SWAP_W__LT_AD_P__RT_D__DD,
	prod_SWAP_W__LT_PD_P_C_RT_D__DD,
	prod_SWAP_W__LT_PD_P_R_RT___DD,
	prod_SWAP_W__LT_AD_RT_D__DD,
	prod_SWAP_W_D__DD,
	prod_LEA_AD__LT_AD_RT_D,
	prod_LEA_AD__AD__D,
	prod_LEA_AD__D,
	prod_ST_Q_DD__LT__P_PD_RT_D,
	prod_ST_Q_DD__LT_AD_P__RT_D,
	prod_ST_Q_DD__LT_PD_P_C_RT_D,
	prod_ST_Q_DD__LT_PD_P_R_RT_,
	prod_ST_Q_DD__LT_AD_RT_D,
	prod_ST_Q_DD__LT_D_RT_,
	prod_ST_DA_PD__LT_PD_P_C_RT_D,
	prod_ST_DA_PD__LT_PD_P_R_RT_,
	prod_ST_DA_PD__LT_AD_P__RT_D,
	prod_ST_DA_PD__LT__P_AD_RT_D,
	prod_ST_DA_PD__D,
	prod_ST_DA__LT_AD_RT__D_PD,
	prod_ST_T_D_D_D,
	prod_ST_W_DD__LT_AD_RT_D,
	prod_ST_W_DD__LT_PD_RT_D,
	prod_ST_W__LT__P_AD_RT__D_ED,
	prod_ST_W__LT_AD_P__RT__D_DD,
	prod_ST_W_DD__LT_PD_RT_,
	prod_ST_W__LT_AD_RT__D_DD,
	prod_ST_W_DD__LT_D_RT_,
	prod_ST_H_DD__LT_PD_P_C_RT_D,
	prod_ST_H_DD__LT_PD_P_R_RT_,
	prod_ST_H_DD___LT_AD_RT_D,
	prod_ST_H_DD__LT__P_AD_RT_D,
	prod_ST_H_DD__LT_AD_P__RT_D,
	prod_ST_H__LT_AD_RT__D_DD,
	prod_ST_H_DD__LT_D_RT_,
	prod_ST_D_ED__LT_PD_P_C_RT_D,
	prod_ST_D_ED__LT_PD_P_R_RT_,
	prod_ST_D_ED__LT_AD_P__RT_D,
	prod_ST_D_ED__LT__P_AD_RT_D,
	prod_ST_D_ED__D,
	prod_ST_D__LT_AD_RT__D_ED,
	prod_ST_B_AD___LT_PD_P_C_RT_D,
	prod_ST_B_AD___LT_PD_P_R_RT_,
	prod_ST_B__LT_AD_RT_D__DD,
	prod_ST_B_DD___LT__P_AD_RT_D,
	prod_ST_B_DD___LT_AD_P__RT_D,
	prod_ST_B__LT_AD_RT_D__DD_0,
	prod_ST_B_DD_LT_D_RT_,
	prod_ST_A__LT_AD_RT_D__AD,
	prod_ST_A_AD___LT__P_PD_RT_D,
	prod_ST_A_AD___LT_AD_P__RT_D,
	prod_ST_A_AD___LT_PD_P_C_RT_D,
	prod_ST_A_AD___LT_PD_P_R_RT_,
	prod_ST_A__LT_AD_RT_D__AD_0,
	prod_ST_A_AD___LT_D_RT_,
	prod_LD_Q_DD__LT__P_PD_RT_D,
	prod_LD_Q_DD__LT_AD_P__RT_D,
	prod_LD_Q_DD__LT_PD_P_C_RT_D,
	prod_LD_Q_DD__LT_PD_P_R_RT_,
	prod_LD_Q_DD__LT_AD_RT_D,
	prod_LD_Q_DD__LT_D_RT_,
	prod_LDMST_ED__LT_AD_P_C_RT_D,
	prod_LDMST_ED__LT_AD_P_R_RT_,
	prod_LDMST_ED__LT__P_AD_RT_D,
	prod_LDMST_DD__LT_PD_P__RT_D,
	prod_LDMST_DD__LT_AD_RT_D,
	prod_LDMST_ED__LT_D_RT_,
	prod_LD_W_DD__LT_PD_P_C_RT_D,
	prod_LD_W_DD__LT_PD_P_R_RT_,
	prod_LD_W_DD__LT_AD_RT_D,
	prod_LD_W_DD__LT__P_AD_RT_D,
	prod_LD_W_DD__LT_AD_P__RT_D,
	prod_LD_W_DD__LT_AD_RT_D_0,
	prod_LD_W_DD_D,
	prod_LD_HU_DD___LT_AD_RT_D,
	prod_LD_HU_DD__LT__P_PD_RT_D,
	prod_LD_HU_DD__LT_AD_P__RT_D,
	prod_LD_HU_DD__LT_PD_P_C_RT_D,
	prod_LD_HU_DD__LT_PD_P_R_RT_,
	prod_LD_HU_DD__LT_AD_RT_D,
	prod_LD_HU_DD__LT_D_RT_,
	prod_LD_H_DD___LT_AD_RT_D,
	prod_LD_H_DD__LT__P_PD_RT_D,
	prod_LD_H_DD__LT_AD_P__RT_D,
	prod_LD_H_DD__LT_PD_P_C_RT_D,
	prod_LD_H_DD__LT_PD_P_R_RT_,
	prod_LD_H_DD__LT_AD_RT_D,
	prod_LD_H_DD__LT_D_RT_,
	prod_LD_DA_PD__LT_PD_P_C_RT_D,
	prod_LD_DA_PD__LT_PD_P_R_RT_,
	prod_LD_DA_PD__LT__P_AD_RT_D,
	prod_LD_DA_PD__D,
	prod_LD_DA_PD__LT_AD_P__RT_D,
	prod_LD_DA_PD__LT_AD_RT_D,
	prod_LD_D_ED__LT_PD_P_C_RT_D,
	prod_LD_D_ED__LT_PD_P_R_RT_,
	prod_LD_D_ED__LT__P_AD_RT_D,
	prod_LD_D_ED__D,
	prod_LD_D_ED__LT_AD_P__RT_D,
	prod_LD_D_ED__LT_AD_RT_D,
	prod_LD_BU_AD__LT_PD_P_R_RT_,
	prod_LD_BU_AD__LT_PD_P_C_RT_D,
	prod_LD_BU_DD__LT_AD_P__RT_D,
	prod_LD_BU_DD__LT__P_AD_RT_D,
	prod_LD_BU_DD___LT_AD_RT_D,
	prod_LD_BU_DD__LT_AD_RT_D,
	prod_LD_BU_DD__LT_D_RT_,
	prod_LD_B_AD__LT_PD_P_C_RT_D,
	prod_LD_B_AD__LT_PD_P_R_RT_,
	prod_LD_B_DD___LT_AD_RT_D,
	prod_LD_B_DD__LT_D_RT_,
	prod_LD_B_DD__LT__P_AD_RT_D,
	prod_LD_B_DD__LT_AD_P__RT_D,
	prod_LD_B_DD__LT_AD_RT_D,
	prod_LD_A_AD__LT_PD_P_R_RT_,
	prod_LD_A_AD__LT_PD_P_C_RT_D,
	prod_LD_A_AD__LT_AD_P__RT_D,
	prod_LD_A_AD__LT_AD_RT_D,
	prod_LD_A_AD__LT__P_AD_RT_D,
	prod_LD_A_AD__LT_AD_RT_D_0,
	prod_LD_A_AD__D,
	prod_TRAPV,
	prod_TRAPSV,
	prod_RSLCX,
	prod_RFM,
	prod_RFE_0,
	prod_BISR__ID_0,
	prod_STUCX__LT_AD_RT_D,
	prod_STUCX_D,
	prod_STLCX__LT_AD_RT_D,
	prod_STLCX_D,
	prod_LDUCX__LT_AD_RT_D,
	prod_LDUCX_D,
	prod_LDLCX__LT_AD_RT_D,
	prod_LDLCX_D,
	prod_RSTV,
	prod_UNTIE,
	prod_TIE,
	prod_SYSCALL_D,
	prod_NOP_0,
	prod_ENABLE,
	prod_RESTORE_DD,
	prod_DISABLE_DD,
	prod_DISABLE,
	prod_DEBUG_0,
	prod_MFCR_DD_D,
	prod_MTCR_D_DD,
	prod_ISYNC,
	prod_DSYNC,
	prod_FRET_0,
	prod_FCALLI_AD,
	prod_FCALLA_08X,
	prod_FCALL_08X,
	prod_JLI_AD,
	prod_JLA_08X,
	prod_LOOPU_08X,
	prod_LOOP_AD__08X_0,
	prod_RET_0,
	prod_JZ_T_DD_D__08X,
	prod_JZ_A_AD__08X,
	prod_JNZ_T_DD__D__08X,
	prod_JNZ_A_AD__08X_0,
	prod_JNEI_DD__D__08X,
	prod_JNEI_DD__DD__08X,
	prod_JNED_DD__D__08X,
	prod_JNED_DD__DD__08X,
	prod_JNE_A_AD__AD__08X,
	prod_JNE_DD__D__08X,
	prod_JNE_DD__DD__08X,
	prod_JLT_U_DD__D__08X,
	prod_JLT_U_DD__DD__08X,
	prod_JLT_DD__D__08X,
	prod_JLT_DD__DD__08X,
	prod_JL_08X,
	prod_JI_AD_0,
	prod_JGE_U_DD__D__08X,
	prod_JGE_U_DD__DD__08X,
	prod_JGE_DD__D__08X,
	prod_JGE_DD__DD__08X,
	prod_JEQ_A_AD__AD__08X,
	prod_JEQ_DD__DD__08X,
	prod_JEQ_DD___ID__08X,
	prod_JA_08X,
	prod_J_08X_0,
	prod_CALLA_08X,
	prod_CALLI_AD,
	prod_CALL_08X_0,
	prod_IXMIN_U_ED__ED__DD,
	prod_IXMIN_ED__ED__DD,
	prod_IXMAX_U_ED__ED__DD,
	prod_IXMAX_ED__ED__DD,
	prod_SAT_HU_DD__DD,
	prod_SAT_H_DD__DD,
	prod_SAT_BU_DD__DD,
	prod_SAT_B_DD__DD,
	prod_PARITY_DD__DD,
	prod_SUB_F_DD__DD__DD,
	prod_UTOF_DD__DD,
	prod_UPDFL_DD,
	prod_QSEED_F_DD__DD,
	prod_Q31TOF_DD__DD__DD,
	prod_MSUB_F_DD__DD__DD__DD,
	prod_MADD_F_DD__DD__DD__DD,
	prod_ITOF_DD__DD,
	prod_FTOUZ_DD__DD,
	prod_FTOU_DD__DD,
	prod_FTOQ31Z_DD__DD__DD,
	prod_FTOQ31_DD__DD__DD,
	prod_FTOIZ_DD__DD,
	prod_FTOI_DD__DD,
	prod_DIV_F_DD__DD__DD,
	prod_MUL_F_DD__DD__DD,
	prod_ADD_F_DD__DD__DD,
	prod_CMP_F_DD__DD__DD,
	prod_MADDSURS_H_DD__DD__DD__DDUU___ID,
	prod_MADDSURS_H_DD__DD__DD__DDUL___ID,
	prod_MADDSURS_H_DD__DD__DD__DDLU___ID,
	prod_MADDSURS_H_DD__DD__DD__DDLL___ID,
	prod_MADDSUR_H_DD__DD__DD__DDUU___ID,
	prod_MADDSUR_H_DD__DD__DD__DDUL___ID,
	prod_MADDSUR_H_DD__DD__DD__DDLU___ID,
	prod_MADDSUR_H_DD__DD__DD__DDLL___ID,
	prod_MADDSUMS_H_ED__ED__DD__DDUU___ID,
	prod_MADDSUMS_H_ED__ED__DD__DDUL___ID,
	prod_MADDSUMS_H_ED__ED__DD__DDLU___ID,
	prod_MADDSUMS_H_ED__ED__DD__DDLL___ID,
	prod_MADDSUM_H_ED__ED__DD__DDUU___ID,
	prod_MADDSUM_H_ED__ED__DD__DDUL___ID,
	prod_MADDSUM_H_ED__ED__DD__DDLU___ID,
	prod_MADDSUM_H_ED__ED__DD__DDLL___ID,
	prod_MADDSUS_H_ED__ED__DD__DDUU___ID,
	prod_MADDSUS_H_ED__ED__DD__DDUL___ID,
	prod_MADDSUS_H_ED__ED__DD__DDLU___ID,
	prod_MADDSUS_H_ED__ED__DD__DDLL___ID,
	prod_MADDSU_H_ED__ED__DD__DDUU___ID,
	prod_MADDSU_H_ED__ED__DD__DDUL___ID,
	prod_MADDSU_H_ED__ED__DD__DDLU___ID,
	prod_MADDSU_H_ED__ED__DD__DDLL___ID,
	prod_MADDRS_Q_DD__DD__DDU__DDU___ID,
	prod_MADDRS_Q_DD__DD__DDL__DDL___ID,
	prod_MADDR_Q_DD__DD__DDU__DDU___ID,
	prod_MADDR_Q_DD__DD__DDL__DDL___ID,
	prod_MADDRS_H_DD__ED__DD__DDUL___ID,
	prod_MADDRS_H_DD__DD__DD__DDUU___ID,
	prod_MADDRS_H_DD__DD__DD__DDUL___ID,
	prod_MADDRS_H_DD__DD__DD__DDLU___ID,
	prod_MADDRS_H_DD__DD__DD__DDLL___ID,
	prod_MADDR_H_DD__ED__DD__DDUL___ID,
	prod_MADDR_H_DD__DD__DD__DDUU___ID,
	prod_MADDR_H_DD__DD__DD__DDUL___ID,
	prod_MADDR_H_DD__DD__DD__DDLU___ID,
	prod_MADDR_H_DD__DD__DD__DDLL___ID,
	prod_MADDS_Q_ED__ED__DDU__DDU___ID,
	prod_MADDS_Q_DD__DD__DDU__DDU___ID,
	prod_MADDS_Q_ED__ED__DDL__DDL___ID,
	prod_MADDS_Q_DD__DD__DDL__DDL___ID,
	prod_MADDS_Q_ED__ED__DD__DDU___ID,
	prod_MADDS_Q_DD__DD__DD__DDU___ID,
	prod_MADDS_Q_ED__ED__DD__DDL___ID,
	prod_MADDS_Q_DD__DD__DD__DDL___ID,
	prod_MADDS_Q_ED__ED__DD__DD___ID,
	prod_MADDS_Q_DD__DD__DD__DD___ID,
	prod_MADD_Q_ED__ED__DDU__DDU___ID,
	prod_MADD_Q_DD__DD__DDU__DDU___ID,
	prod_MADD_Q_ED__ED__DDL__DDL___ID,
	prod_MADD_Q_DD__DD__DDL__DDL___ID,
	prod_MADD_Q_ED__ED__DD__DDU___ID,
	prod_MADD_Q_DD__DD__DD__DDU___ID,
	prod_MADD_Q_ED__ED__DD__DDL___ID,
	prod_MADD_Q_DD__DD__DD__DDL___ID,
	prod_MADD_Q_ED__ED__DD__DD___ID,
	prod_MADD_Q_DD__DD__DD__DD___ID,
	prod_MADDS_H_ED__ED__DD__DDUU___ID,
	prod_MADDS_H_ED__ED__DD__DDUL___ID,
	prod_MADDS_H_ED__ED__DD__DDLU___ID,
	prod_MADDS_H_ED__ED__DD__DDLL___ID,
	prod_MADD_H_ED__ED__DD__DDUU___ID,
	prod_MADD_H_ED__ED__DD__DDUL___ID,
	prod_MADD_H_ED__ED__DD__DDLU___ID,
	prod_MADD_H_ED__ED__DD__DDLL___ID,
	prod_MADDMS_H_ED__ED__DD__DD_LU___ID,
	prod_MADDMS_H_ED__ED__DD__DD_UL___ID,
	prod_MADDMS_H_ED__ED__DD__DD_UU___ID,
	prod_MADDMS_H_ED__ED__DD__DD_LL___ID,
	prod_MADDM_H_ED__ED__DD__DD_UU___ID,
	prod_MADDM_H_ED__ED__DD__DD_UL___ID,
	prod_MADDM_H_ED__ED__DD__DD_LU___ID,
	prod_MADDM_H_ED__ED__DD__DD_LL___ID,
	prod_MADDS_U_ED__ED__DD___ID,
	prod_MADDS_U_ED__ED__DD__DD,
	prod_MADDS_U_DD__DD__DD___ID,
	prod_MADDS_U_DD__DD__DD__DD,
	prod_MADDS_ED__ED__DD___ID,
	prod_MADDS_ED__ED__DD__DD,
	prod_MADDS_DD__DD__DD___ID,
	prod_MADDS_DD__DD__DD__DD,
	prod_MADD_ED__ED__DD___ID,
	prod_MADD_ED__ED__DD__DD,
	prod_MADD_DD__DD__DD___ID,
	prod_MADD_DD__DD__DD__DD,
	prod_MADD_U_ED__ED__DD___ID,
	prod_MADD_U_ED__ED__DD__DD,
	prod_CLO_H_DD__DD,
	prod_CLO_DD__DD,
	prod_CLZ_H_DD_DD,
	prod_CLZ_DD_DD,
	prod_CLS_H_DD__DD,
	prod_CLS_DD__DD,
	prod_UNPACK_ED__DD,
	prod_PACK_DD__ED__DD,
	prod_BSPLIT_ED__DD,
	prod_BMERGE_DD__DD__DD,
	prod_IMASK_ED_DD_DD__ID,
	prod_IMASK_ED_DD__ID__ID,
	prod_IMASK_ED__ID_DD__ID,
	prod_IMASK_ED__ID__ID__ID,
	prod_SH_XOR_T_DD__DD__DD___ID___ID,
	prod_SH_XNOR_T_DD__DD__DD___ID___ID,
	prod_SH_ORN_T_DD__DD__DD___ID___ID,
	prod_SH_OR_T_DD__DD__DD___ID___ID,
	prod_SH_NOR_T_DD__DD__DD___ID___ID,
	prod_SH_NAND_T_DD__DD__DD___ID___ID,
	prod_SH_ANDN_T_DD__DD__DD___ID___ID,
	prod_SH_AND_T_DD__DD__DD___ID___ID,
	prod_SH_NE_DD__DD___ID,
	prod_SH_NE_DD__DD__DD,
	prod_SH_LT_U_DD__DD___ID,
	prod_SH_LT_U_DD__DD__DD,
	prod_SH_LT_DD__DD___ID,
	prod_SH_LT_DD__DD__DD,
	prod_SH_H_DD__DD___ID,
	prod_SH_H_DD__DD__DD,
	prod_SH_GE_U_DD__DD___ID,
	prod_SH_GE_U_DD__DD__DD,
	prod_SH_GE_DD__DD___ID,
	prod_SH_GE_DD__DD__DD,
	prod_SH_EQ_DD__DD__DD,
	prod_SH_EQ_DD__DD___ID,
	prod_SHAS_DD__DD___ID,
	prod_SHAS_DD__DD__DD,
	prod_SHA_H_DD__DD___ID,
	prod_SHA_H_DD__DD__DD,
	prod_SHA_DD__DD___ID,
	prod_SHA_DD__DD__DD,
	prod_SH_DD__DD___ID,
	prod_SH_DD__DD__DD,
	prod_INSN_T_DD_DD__ID_DD__ID,
	prod_INS_T_DD_DD__ID_DD__ID,
	prod_INSERT_DD_DD__ID__ID__ID,
	prod_INSERT_DD_DD__ID_DD__ID,
	prod_INSERT_DD_DD__ID_ED,
	prod_INSERT_DD_DD_DD__ID__ID,
	prod_INSERT_DD_DD_DD_DD__ID,
	prod_INSERT_DD_DD_DD_ED,
	prod_EXTR_DD_DD_DD__ID,
	prod_EXTR_DD_DD_ED,
	prod_EXTR_U_DD_DD_D_D,
	prod_EXTR_U_DD_DD_DD__ID,
	prod_EXTR_U_DD_DD_ED,
	prod_EXTR_DD_DD_D__ID,
	prod_DEXTR_DD_DD_DD_D,
	prod_DEXTR_DD_DD_DD_DD,
	prod_NAND_T_DD__DD___ID__DD___ID,
	prod_NAND_DD__DD___ID,
	prod_NAND_DD__DD__DD,
	prod_MAX_HU_DD__DD__DD,
	prod_MAX_H_DD__DD__DD,
	prod_MAX_BU_DD__DD__DD,
	prod_MAX_B_DD__DD__DD,
	prod_MAX_U_DD_DD__ID,
	prod_MAX_U_DD_DD_DD,
	prod_MAX_DD_DD__ID,
	prod_MAX_DD_DD_DD,
	prod_NOR_T_DD_DD__ID_DD__ID,
	prod_NOR_DD_DD__ID,
	prod_NOR_DD_DD_DD,
	prod_MIN_HU_DD__DD__DD,
	prod_MIN_H_DD__DD__DD,
	prod_MIN_B_DD__DD__DD,
	prod_MIN_BU_DD__DD__DD,
	prod_MIN_DD__DD___ID,
	prod_MIN_DD__DD__DD,
	prod_MIN_U_DD__DD___ID,
	prod_MIN_U_DD_DD_DD,
	prod_ORN_T_DD__DD___ID__DD___ID,
	prod_ORN_DD__DD__DD,
	prod_ORN_DD__DD___ID,
	prod_LT_WU_DD__DD__DD,
	prod_LT_W_DD__DD__DD,
	prod_LT_H_DD__DD__DD,
	prod_LT_H_DD__DD__DD_0,
	prod_LT_BU_DD__DD__DD,
	prod_LT_B_DD__DD__DD,
	prod_LT_A_DD__AD__AD,
	prod_LT_U_DD__DD___ID,
	prod_LT_U_DD__DD__DD,
	prod_LT_DD_DD__ID,
	prod_LT_DD_DD_DD,
	prod_XOR_LT_U_DD__DD___ID,
	prod_XOR_LT_U_DD__DD__DD,
	prod_XOR_LT_DD__DD___ID,
	prod_XOR_LT_DD__DD__DD,
	prod_XOR_GE_U_DD__DD___ID,
	prod_XOR_GE_U_DD__DD__DD,
	prod_XOR_GE_DD__DD___ID,
	prod_XOR_GE_DD__DD__DD,
	prod_XOR_NE_DD__DD__DD,
	prod_XOR_NE_DD__DD___ID,
	prod_XOR_EQ_DD__DD__DD,
	prod_XOR_EQ_DD__DD___ID,
	prod_XNOR_T_DD__DD___ID__DD___ID,
	prod_XNOR_DD__DD___ID,
	prod_XNOR_DD__DD__DD,
	prod_XOR_T_DD__DD___ID__DD___ID,
	prod_XOR_DD_DD__ID,
	prod_XOR_DD_DD_DD,
	prod_OR_OR_T_DD__DD___ID__DD___ID,
	prod_OR_NOR_T_DD__DD___ID__DD___ID,
	prod_OR_ANDN_T_DD__DD___ID__DD___ID,
	prod_OR_AND_T_DD__DD___ID__DD___ID,
	prod_OR_EQ_DD__DD__DD,
	prod_OR_EQ_DD__DD__D,
	prod_OR_T_DD_DD__ID_DD__ID,
	prod_OR_NE_DD_DD__ID,
	prod_OR_NE_DD_DD_DD,
	prod_OR_GE_U_DD__DD___ID,
	prod_OR_GE_U_DD__DD__DD,
	prod_OR_GE_DD__DD___ID,
	prod_OR_GE_DD__DD__DD,
	prod_OR_LT_U_DD_DD__ID,
	prod_OR_LT_U_DD_DD_DD,
	prod_OR_LT_DD_DD__ID,
	prod_OR_LT_DD_DD_DD,
	prod_OR_DD_DD__ID,
	prod_OR_DD_DD_DD,
	prod_NE_A_DD__DD__DD,
	prod_NEZ_A_DD_DD,
	prod_NE_DD_DD__ID,
	prod_NE_DD_DD_DD,
	prod_GE_U_DD_DD_DD,
	prod_GE_A_DD_AD_AD,
	prod_GE_U_DD_DD__ID,
	prod_GE_DD_DD__ID,
	prod_GE_DD_DD_DD,
	prod_EQZ_A_DD__AD,
	prod_EQ_B_DD__DD__DD,
	prod_EQ_H_DD__DD__DD,
	prod_EQ_W_DD__DD__DD,
	prod_EQ_A_DD__AD__AD,
	prod_EQANY_H_DD_DD__ID,
	prod_EQANY_H_DD_DD_DD,
	prod_EQANY_B_DD_DD__ID,
	prod_EQANY_B_DD_DD_DD,
	prod_EQ_DD_DD__ID,
	prod_EQ_DD_DD_DD,
	prod_AND_GE_U_DD__DD___ID,
	prod_AND_GE_U_DD__DD__DD,
	prod_AND_GE_DD__DD___ID,
	prod_AND_GE_DD__DD__DD,
	prod_AND_NE_DD__DD___ID,
	prod_AND_NE_DD__DD__DD,
	prod_AND_OR_T_DD__DD___ID__DD___ID,
	prod_AND_ANDN_T_DD__DD___ID__DD___ID,
	prod_AND_AND_T_DD__DD___ID__DD___ID,
	prod_AND_EQ_DD__DD__DD,
	prod_AND_EQ_DD__DD__D,
	prod_AND_T_DD_DD__ID_DD__ID,
	prod_AND_NOR_T_DD_DD_D_DD_D,
	prod_ANDN_T_DD__DD___ID__DD___ID,
	prod_ANDN_DD_DD__ID,
	prod_ANDN_DD_DD_DD,
	prod_AND_LT_DD__DD___ID,
	prod_AND_LT_DD__DD__DD,
	prod_AND_LT_U_DD__DD___ID,
	prod_AND_LT_U_DD__DD__DD,
	prod_AND_DD_DD__ID,
	prod_AND_DD_DD_DD,
	prod_SUBS_U_DD__DD__DD,
	prod_SUBS_HU_DD_DD_DD,
	prod_SUBS_H_DD_DD_DD,
	prod_SUBS_DD__DD__DD,
	prod_SUB_H_DD_DD_DD,
	prod_SUB_B_DD_DD_DD,
	prod_RSUBS_U_DD__DD___ID,
	prod_RSUBS_DD__DD___ID,
	prod_RSUB_DD__DD___ID,
	prod_MSUBRS_Q_DD__DD__DDU__DDU___ID,
	prod_MSUBRS_Q_DD__DD__DDL__DDL___ID,
	prod_MSUBR_Q_DD__DD__DDU__DDU___ID,
	prod_MSUBR_Q_DD__DD__DDL__DDL___ID,
	prod_MSUBRS_H_DD__ED__DD__DDUL___ID,
	prod_MSUBR_H_DD__ED__DD__DDUL___ID,
	prod_MSUBRS_H_DD__DD__DD__DDUU___ID,
	prod_MSUBRS_H_DD__DD__DD__DDUL___ID,
	prod_MSUBRS_H_DD__DD__DD__DDLU___ID,
	prod_MSUBRS_H_DD__DD__DD__DDLL___ID,
	prod_MSUBR_H_DD__DD__DD__DDUU___ID,
	prod_MSUBR_H_DD__DD__DD__DDUL___ID,
	prod_MSUBR_H_DD__DD__DD__DDLU___ID,
	prod_MSUBR_H_DD__DD__DD__DDLL___ID,
	prod_MSUBMS_H_ED__ED__DD__DDUU___ID,
	prod_MSUBMS_H_ED__ED__DD__DDUL___ID,
	prod_MSUBMS_H_ED__ED__DD__DDLU___ID,
	prod_MSUBMS_H_ED__ED__DD__DDLL___ID,
	prod_MSUBM_H_ED__ED__DD__DDUU___ID,
	prod_MSUBM_H_ED__ED__DD__DDUL___ID,
	prod_MSUBM_H_ED__ED__DD__DDLU___ID,
	prod_MSUBM_H_ED__ED__DD__DDLL___ID,
	prod_MSUBADRS_H_DD__DD__DD__DDUU___ID,
	prod_MSUBADRS_H_DD__DD__DD__DDUL___ID,
	prod_MSUBADRS_H_DD__DD__DD__DDLU___ID,
	prod_MSUBADRS_H_DD__DD__DD__DDLL___ID,
	prod_MSUBADR_H_DD__DD__DD__DDUU___ID,
	prod_MSUBADR_H_DD__DD__DD__DDUL___ID,
	prod_MSUBADR_H_DD__DD__DD__DDLU___ID,
	prod_MSUBADR_H_DD__DD__DD__DDLL___ID,
	prod_MSUBADMS_H_ED__ED__DD__DDUU___ID,
	prod_MSUBADMS_H_ED__ED__DD__DDUL___ID,
	prod_MSUBADMS_H_ED__ED__DD__DDLU___ID,
	prod_MSUBADMS_H_ED__ED__DD__DDLL___ID,
	prod_MSUBADM_H_ED__ED__DD__DDUU___ID,
	prod_MSUBADM_H_ED__ED__DD__DDUL___ID,
	prod_MSUBADM_H_ED__ED__DD__DDLU___ID,
	prod_MSUBADM_H_ED__ED__DD__DDLL___ID,
	prod_MSUBADS_H_ED__ED__DD__DDUU___ID,
	prod_MSUBADS_H_ED__ED__DD__DDUL___ID,
	prod_MSUBADS_H_ED__ED__DD__DDLU___ID,
	prod_MSUBADS_H_ED__ED__DD__DDLL___ID,
	prod_MSUBAD_H_ED__ED__DD__DDUU___ID,
	prod_MSUBAD_H_ED__ED__DD__DDUL___ID,
	prod_MSUBAD_H_ED__ED__DD__DDLU___ID,
	prod_MSUBAD_H_ED__ED__DD__DDLL___ID,
	prod_MSUBS_Q_ED__ED__DDU__DDU___ID,
	prod_MSUBS_Q_DD__DD__DDU__DDU___ID,
	prod_MSUBS_Q_ED__ED__DDL__DDL___ID,
	prod_MSUBS_Q_DD__DD__DDL__DDL___ID,
	prod_MSUBS_Q_ED__ED__DD__DDU___ID,
	prod_MSUBS_Q_DD__DD__DD__DDU___ID,
	prod_MSUBS_Q_ED__ED__DD__DDL___ID,
	prod_MSUBS_Q_DD__DD__DD__DDL___ID,
	prod_MSUBS_Q_ED__ED__DD__DD___ID,
	prod_MSUBS_Q_DD__DD__DD__DD___ID,
	prod_MSUB_Q_ED__ED__DDU__DDU___ID,
	prod_MSUB_Q_DD__DD__DDU__DDU___ID,
	prod_MSUB_Q_ED__ED__DDL__DDL___ID,
	prod_MSUB_Q_DD__DD__DDL__DDL___ID,
	prod_MSUB_Q_ED__ED__DD__DDU___ID,
	prod_MSUB_Q_DD__DD__DD__DDU___ID,
	prod_MSUB_Q_ED__ED__DD__DDL___ID,
	prod_MSUB_Q_DD__DD__DD__DDL___ID,
	prod_MSUB_Q_ED__ED__DD__DD___ID,
	prod_MSUB_Q_DD__DD__DD__DD___ID,
	prod_MSUBS_H_ED__ED__DD__DDUU___ID,
	prod_MSUBS_H_ED__ED__DD__DDUL___ID,
	prod_MSUBS_H_ED__ED__DD__DDLU___ID,
	prod_MSUBS_H_ED__ED__DD__DDLL___ID,
	prod_MSUB_H_ED__ED__DD__DDUU___ID,
	prod_MSUB_H_ED__ED__DD__DDUL___ID,
	prod_MSUB_H_ED__ED__DD__DDLU___ID,
	prod_MSUB_H_ED__ED__DD__DDLL___ID,
	prod_MSUBS_U_ED__ED__DD___ID,
	prod_MSUBS_U_ED__ED__DD__DD,
	prod_MSUBS_U_DD__DD__DD___ID,
	prod_MSUBS_U_DD__DD__DD__DD,
	prod_MSUB_U_ED__ED__DD___ID,
	prod_MSUB_U_ED__ED__DD__DD,
	prod_MSUBS_ED__ED__DD___ID,
	prod_MSUBS_ED__ED__DD__DD,
	prod_MSUBS_DD__DD__DD___ID,
	prod_MSUBS_DD__DD__DD__DD,
	prod_MSUB_ED__ED__DD___ID,
	prod_MSUB_ED__ED__DD__DD,
	prod_MSUB_DD__DD__DD___ID,
	prod_MSUB_DD__DD__DD__DD,
	prod_CSUBN_DD_DD_DD_DD,
	prod_CSUB_DD_DD_DD_DD,
	prod_SUB_A_AD_AD_AD,
	prod_SUBX_DD_DD_DD,
	prod_SUBC_DD__DD__DD,
	prod_SUB_DD_DD_DD,
	prod_SELN_DD_DD_DD__ID,
	prod_SELN_DD_DD_DD_DD,
	prod_SEL_DD_DD_DD__ID,
	prod_SEL_DD_DD_DD_DD,
	prod_MULR_Q_DD__DDU__DDU__D,
	prod_MULR_Q_DD__DDL__DDL__D,
	prod_MULR_H_DD__DD__DDUU__D,
	prod_MULR_H_DD__DD__DDUL__D,
	prod_MULR_H_DD__DD__DDLU__D,
	prod_MULR_H_DD__DD__DDLL__D,
	prod_MULM_H_ED__DD__DDUU__D,
	prod_MULM_H_ED__DD__DDUL__D,
	prod_MULM_H_ED__DD__DDLU__D,
	prod_MULM_H_ED__DD__DDLL__D,
	prod_MUL_Q_DD__DDU__DDU__D,
	prod_MUL_Q_DD__DDL__DDL__D,
	prod_MUL_Q_ED__DD__DDU__D,
	prod_MUL_Q_DD__DD__DDU__D,
	prod_MUL_Q_ED__DD__DDL__D,
	prod_MUL_Q_DD__DD__DDL__D,
	prod_MUL_Q_ED__DD__DD__D,
	prod_MUL_Q_DD__DD__DD__D,
	prod_MUL_H_ED_DD_DD_UU_D,
	prod_MUL_H_ED_DD_DD_UL_D,
	prod_MUL_H_ED_DD_DD_LU_D,
	prod_MUL_H_ED_DD_DD_LL_D,
	prod_MULS_U_DD_DD__ID,
	prod_MULS_U_DD_DD_DD,
	prod_MULS_DD_DD__ID,
	prod_MULS_DD_DD_DD,
	prod_MUL_U_ED_DD_D,
	prod_MUL_U_ED_DD_DD,
	prod_MUL_ED_DD__ID,
	prod_MUL_DD_DD__ID,
	prod_MUL_ED_DD_DD,
	prod_MUL_DD_DD_DD,
	prod_MOV_U_DD_D,
	prod_MOVH_A_AD__ID,
	prod_MOVH_DD_D,
	prod_MOV_D_DD_AD_0,
	prod_MOV_AA_AD_AD_0,
	prod_MOV_A_AD_DD_0,
	prod_MOV_ED__DD__DD,
	prod_MOV_ED_DD,
	prod_MOV_ED__ID_0,
	prod_MOV_DD__ID_0,
	prod_MOV_DD_DD_0,
	prod_DIV_U_ED__DD__DD,
	prod_DIV_ED__DD__DD,
	prod_DVSTEP_U_ED_ED_DD,
	prod_DVSTEP_ED_ED_DD,
	prod_DVINIT_U_ED_DD_DD,
	prod_DVINIT_HU_ED_DD_DD,
	prod_DVINIT_H_ED_DD_DD,
	prod_DVINIT_B_ED_DD_DD,
	prod_DVINIT_BU_ED_DD_DD,
	prod_DVINIT_ED_DD_DD,
	prod_DVADJ_ED_ED_DD,
	prod_ADD_H_DD__DD__DD,
	prod_ADD_B_DD__DD__DD,
	prod_ADDS_U_DD_DD__ID,
	prod_ADDS_U_DD_DD_DD,
	prod_CADDN_DD_DD_DD_DD,
	prod_CADDN_DD_DD_DD__ID,
	prod_ADDX_DD_DD__ID,
	prod_ADDSC_AT_AD_AD_AD,
	prod_ADDS_HU_DD_DD_DD,
	prod_ADDS_H_DD_DD_DD,
	prod_ADDS_DD_DD__ID,
	prod_ADDS_DD_DD_DD,
	prod_CADD_DD_DD_DD__ID,
	prod_CADD_DD_DD_DD_DD,
	prod_ADDX_DD_DD_DD,
	prod_ADDSC_A_AD_AD_AD_D,
	prod_ADDIH_A_AD_AD__ID,
	prod_ADDIH_DD_DD__ID,
	prod_ADDI_DD_DD_D,
	prod_ADDC_DD_DD__ID,
	prod_ADDC_DD_DD_DD,
	prod_ADD_A_AD_AD_AD,
	prod_ADD_DD_DD__ID,
	prod_ADD_DD_DD_DD,
	prod_ABSS_H_DD__DD,
	prod_ABS_H_DD__DD,
	prod_ABS_B_DD__DD,
	prod_ABSS_DD__DD,
	prod_ABS_DD_DD,
	prod_ABSDIFS_H_DD__DD__DD,
	prod_ABSDIF_H_DD__DD__DD,
	prod_ABSDIF_B_DD__DD__DD,
	prod_ABSDIFS_DD_DD__ID,
	prod_ABSDIFS_DD_DD_DD,
	prod_ABSDIF_DD_DD__ID,
	prod_ABSDIF_DD_DD_DD
};

int tricore_prod(otawa::gliss::Info *info, otawa::Inst *inst) {
	tricore_inst_t *desc;
	info->decode(inst, desc);
	int r = prod_tab[desc->ident](desc);
	info->free(desc);
	return r;
}
