/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_encoding.h>
#అన్తర్భావ <sbi/riscv_fp.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_trap_ldst.h>
#అన్తర్భావ <sbi/sbi_trap.h>
#అన్తర్భావ <sbi/sbi_unpriv.h>
#అన్తర్భావ <sbi/sbi_platform.h>

/**
 * Load emulator callback:
 *   Refer to comments of `sbi_platform_emulate_load`.
 */
రకనిర్వచనం పూర్ణాంకం (*sbi_trap_ld_emulator)(ulong insn, పూర్ణాంకం rlen, ulong raddr,
				    సంఘం sbi_ldst_దత్తాంశం *బహిర్గతం_val,
				    నిర్మాణం sbi_trap_context *tcntx);

/**
 * Store emulator callback:
 *   Refer to comments of `sbi_platform_emulate_store`.
 */
రకనిర్వచనం పూర్ణాంకం (*sbi_trap_st_emulator)(ulong insn, పూర్ణాంకం wlen, ulong waddr,
				    సంఘం sbi_ldst_దత్తాంశం లోపల_val,
				    నిర్మాణం sbi_trap_context *tcntx);

/**
 * Handling of misaligned fault is done by a collection of smaller, but
 * aligned load/store(s). Another fault (load/store, page fault...) can
 * arise from any of them, then the handling gets aborted. We must fixup
 * the tinst to pretend the fault was rised from the original insn.
 * Specifically, fixup the offset field using the tval diff between the
 * new trap and the original one (if required).
 */
స్థిర అంతఃవరుస శూన్య sbi_misaligned_tinst_fixup(
			స్థిరరాశి నిర్మాణం sbi_trap_info *orig_trap,
			      నిర్మాణం sbi_trap_info *uptrap)
{
	ulong స్థానభ్రంశం = uptrap->tval - orig_trap->tval;

	/*
	 * The function is called in code path for handling a scalar
	 * load/store misaligned fault, thus the new uptrap can't have
	 * custom value of tinst
	 */
	ఒకవేళ (uptrap->tinst == INSN_PSEUDO_VS_ఎక్కించు ||
	    uptrap->tinst == INSN_PSEUDO_VS_STORE)
		/* Use uptrap as-is for guest-page faults */
		తిరిగి;
	/*
	 * Only fixup if orig tinst is valid. Otherwise, discard the
	 * new tinst to be on the safe side. Never use new tinst as-is!
	 * It's load/store width surely mismatches the original width.
	 * For vector, discard it regardless. It doesn't make sense to
	 * have a transformed tinst
	 */
	లేకపోతే ఒకవేళ (orig_trap->tinst == 0)
		uptrap->tinst = 0;
	లేకపోతే
		uptrap->tinst = orig_trap->tinst | (స్థానభ్రంశం << SH_RS1);
}

స్థిర అంతఃవరుస bool sbi_trap_tinst_valid(ulong tinst)
{
	/*
	 * Bit[0] == 1 implies trapped instruction value is
	 * transformed instruction or custom instruction.
	 * Also do proper checking per Privileged ISA 19.6.3,
	 * and make sure high 32 bits of tinst is 0
	 */
	తిరిగి tinst == (uint32_t)tinst && (tinst & 0x1);
}

స్థిర పూర్ణాంకం sbi_trap_emulate_ఎక్కించు(నిర్మాణం sbi_trap_context *tcntx,
				 sbi_trap_ld_emulator emu)
{
	స్థిరరాశి నిర్మాణం sbi_trap_info *orig_trap = &tcntx->trap;
	నిర్మాణం sbi_trap_regs *regs = &tcntx->regs;
	ulong insn, insn_len, imm = 0, జరుపు = 0, off = 0;
	సంఘం sbi_ldst_దత్తాంశం val = { 0 };
	నిర్మాణం sbi_trap_info uptrap;
	bool xform = false, fp = false, c_ఎక్కించు = false, c_ldsp = false;
	పూర్ణాంకం rc, len = 0, prev_xlen = 0;

	ఒకవేళ (sbi_trap_tinst_valid(orig_trap->tinst)) {
		xform	 = true;
		insn	 = orig_trap->tinst | INSN_16_బిట్టు_ముసుగు;
		insn_len = (orig_trap->tinst & 0x2) ? INSN_LEN(insn) : 2;
	} లేకపోతే {
		/* trapped instruction value is zero or special value */
		insn = sbi_get_insn(regs->mepc, &uptrap);
		ఒకవేళ (uptrap.cause) {
			తిరిగి sbi_trap_redirect(regs, &uptrap);
		}
		insn_len = INSN_LEN(insn);
	}

	/**
	 * Common for RV32/RV64:
	 *    lb, lbu, lh, lhu, lw, flw, flw
	 *    c.lbu, c.lh, c.lhu, c.lw, c.lwsp, c.fld, c.fldsp
	 */
	ఒకవేళ ((insn & INSN_ముసుగు_LB) == INSN_సరిపోలిక_LB) {
		len = -1;
	} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_LBU) == INSN_సరిపోలిక_LBU) {
		len = 1;
	} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_C_LBU) == INSN_సరిపోలిక_C_LBU) {
		/* Zcb */
		len = 1;
		imm = RVC_LB_IMM(insn);
		c_ఎక్కించు = true;
	} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_LH) == INSN_సరిపోలిక_LH) {
		len = -2;
	} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_C_LH) == INSN_సరిపోలిక_C_LH) {
		/* Zcb */
		len = -2;
		imm = RVC_LH_IMM(insn);
		c_ఎక్కించు = true;
	} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_LHU) == INSN_సరిపోలిక_LHU) {
		len = 2;
	} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_C_LHU) == INSN_సరిపోలిక_C_LHU) {
		/* Zcb */
		len = 2;
		imm = RVC_LH_IMM(insn);
		c_ఎక్కించు = true;
	} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_LW) == INSN_సరిపోలిక_LW) {
		len = -4;
	} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_C_LW) == INSN_సరిపోలిక_C_LW) {
		/* Zca */
		len = -4;
		imm = RVC_LW_IMM(insn);
		c_ఎక్కించు = true;
	} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_C_LWSP) == INSN_సరిపోలిక_C_LWSP &&
		GET_RD_అంకె(insn)) {
		/* Zca */
		len = -4;
		imm = RVC_LWSP_IMM(insn);
		c_ldsp = true;
#నిర్వచితమైతే __riscv_flen
	} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_FLW) == INSN_సరిపోలిక_FLW) {
		len = 4;
		fp = true;
	} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_FLD) == INSN_సరిపోలిక_FLD) {
		len = 8;
		fp = true;
	} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_C_FLD) == INSN_సరిపోలిక_C_FLD) {
		/* Zcd */
		len = 8;
		imm = RVC_LD_IMM(insn);
		c_ఎక్కించు = true;
		fp = true;
	} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_C_FLDSP) == INSN_సరిపోలిక_C_FLDSP) {
		/* Zcd */
		len = 8;
		imm = RVC_LDSP_IMM(insn);
		c_ldsp = true;
		fp = true;
#అయితే_ముగింపు
	} లేకపోతే {
		prev_xlen = sbi_regs_prev_xlen(regs);
	}

	/**
	 * Must distinguish between rv64 and rv32, RVC instructions have
	 * overlapping encoding:
	 *     c.ld in rv64 == c.flw in rv32
	 *     c.ldsp in rv64 == c.flwsp in rv32
	 */
	ఒకవేళ (prev_xlen == 64) {
		/* RV64 Only: lwu, ld, c.ld, c.ldsp  */
		ఒకవేళ ((insn & INSN_ముసుగు_LWU) == INSN_సరిపోలిక_LWU) {
			len = 4;
		} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_LD) == INSN_సరిపోలిక_LD) {
			len = 8;
		} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_C_LD) == INSN_సరిపోలిక_C_LD) {
			/* Zca */
			len = 8;
			imm = RVC_LD_IMM(insn);
			c_ఎక్కించు = true;
		} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_C_LDSP) == INSN_సరిపోలిక_C_LDSP &&
			GET_RD_అంకె(insn)) {
			/* Zca */
			len = 8;
			imm = RVC_LDSP_IMM(insn);
			c_ldsp = true;
		}
#నిర్వచితమైతే __riscv_flen
	} లేకపోతే ఒకవేళ (prev_xlen == 32) {
		/* RV32 Only: c.flw, c.flwsp */
		ఒకవేళ ((insn & INSN_ముసుగు_C_FLW) == INSN_సరిపోలిక_C_FLW) {
			/* Zcf */
			len = 4;
			imm = RVC_LW_IMM(insn);
			c_ఎక్కించు = true;
			fp = true;
		} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_C_FLWSP) == INSN_సరిపోలిక_C_FLWSP) {
			/* Zcf */
			len = 4;
			imm = RVC_LWSP_IMM(insn);
			c_ldsp = true;
			fp = true;
		}
#అయితే_ముగింపు
	}

	ఒకవేళ (len < 0) {
		len = -len;
		జరుపు = 8 * (పరిమాణం(ulong) - len);
	}

	ఒకవేళ (!len) // Unknown instruction
		వెళ్ళు do_emu;

#ఒకవేళ !defined(OPENSBI_DEBUG)
	/**
	 * For misaligned faults. Skip offset calculation unless DEBUG
	 * builds. It helps validating OpenSBI and HW.
	 */
	ఒకవేళ (orig_trap->cause == CAUSE_MISALIGNED_ఎక్కించు)
		వెళ్ళు do_emu;
#అయితే_ముగింపు

	ఒకవేళ (xform)
		/* Transformed insn */
		off = GET_RS1_అంకె(insn);
	లేకపోతే ఒకవేళ (c_ఎక్కించు)
		/* non SP-based compressed load */
		off = orig_trap->tval - GET_RS1S(insn, regs) - imm;
	లేకపోతే ఒకవేళ (c_ldsp)
		/* SP-based compressed load */
		off = orig_trap->tval - REG_VAL(2, regs) - imm;
	లేకపోతే
		/* I-type non-compressed load */
		off = orig_trap->tval - GET_RS1(insn, regs) - (ulong)IMM_I(insn);
	/**
	 * Normalize offset, in case the XLEN of unpriv mode is smaller,
	 * and/or pointer masking is in effect
	 */
	off &= (len - 1);

do_emu:
	rc = emu(insn, len, orig_trap->tval - off, &val, tcntx);
	ఒకవేళ (rc <= 0)
		తిరిగి rc;
	ఒకవేళ (!len)
		వెళ్ళు epc_fixup;

	ఒకవేళ (!fp) {
		ulong v = ((దీర్ఘ)(val.దత్తాంశం_ulong << జరుపు)) >> జరుపు;

		ఒకవేళ (c_ఎక్కించు)
			సమితి_RDS(insn, regs, v);
		లేకపోతే
			సమితి_RD(insn, regs, v);
#నిర్వచితమైతే __riscv_flen
	} లేకపోతే ఒకవేళ (len == 8) {
		ఒకవేళ (c_ఎక్కించు)
			సమితి_F64_RDS(insn, regs, val.దత్తాంశం_u64);
		లేకపోతే
			సమితి_F64_RD(insn, regs, val.దత్తాంశం_u64);
	} లేకపోతే {
		ఒకవేళ (c_ఎక్కించు)
			సమితి_F32_RDS(insn, regs, val.దత్తాంశం_ulong);
		లేకపోతే
			సమితి_F32_RD(insn, regs, val.దత్తాంశం_ulong);
#అయితే_ముగింపు
	}

epc_fixup:
	regs->mepc += insn_len;

	తిరిగి 0;
}

స్థిర పూర్ణాంకం sbi_trap_emulate_store(నిర్మాణం sbi_trap_context *tcntx,
				  sbi_trap_st_emulator emu)
{
	స్థిరరాశి నిర్మాణం sbi_trap_info *orig_trap = &tcntx->trap;
	నిర్మాణం sbi_trap_regs *regs = &tcntx->regs;
	ulong insn, insn_len, imm = 0, off = 0;
	సంఘం sbi_ldst_దత్తాంశం val;
	నిర్మాణం sbi_trap_info uptrap;
	bool xform = false, fp = false, c_store = false, c_stsp = false;
	పూర్ణాంకం rc, len = 0, prev_xlen = 0;

	ఒకవేళ (sbi_trap_tinst_valid(orig_trap->tinst)) {
		xform	 = true;
		insn	 = orig_trap->tinst | INSN_16_బిట్టు_ముసుగు;
		insn_len = (orig_trap->tinst & 0x2) ? INSN_LEN(insn) : 2;
	} లేకపోతే {
		/* trapped instruction value is zero or special value */
		insn = sbi_get_insn(regs->mepc, &uptrap);
		ఒకవేళ (uptrap.cause) {
			తిరిగి sbi_trap_redirect(regs, &uptrap);
		}
		insn_len = INSN_LEN(insn);
	}

	/**
	 * Common for RV32/RV64:
	 *    sb, sh, sw, fsw, fsd
	 *    c.sb, c.sh, c.sw, c.swsp, c.fsd, c.fsdsp
	 */
	ఒకవేళ ((insn & INSN_ముసుగు_SB) == INSN_సరిపోలిక_SB) {
		len = 1;
	} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_C_SB) == INSN_సరిపోలిక_C_SB) {
		/* Zcb */
		len = 1;
		imm = RVC_SB_IMM(insn);
		c_store = true;
	} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_SH) == INSN_సరిపోలిక_SH) {
		len = 2;
	} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_C_SH) == INSN_సరిపోలిక_C_SH) {
		/* Zcb */
		len = 2;
		imm = RVC_SH_IMM(insn);
		c_store = true;
	} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_SW) == INSN_సరిపోలిక_SW) {
		len = 4;
	} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_C_SW) == INSN_సరిపోలిక_C_SW) {
		/* Zca */
		len = 4;
		imm = RVC_SW_IMM(insn);
		c_store = true;
	} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_C_SWSP) == INSN_సరిపోలిక_C_SWSP) {
		/* Zca */
		len = 4;
		imm = RVC_SWSP_IMM(insn);
		c_stsp = true;
#నిర్వచితమైతే __riscv_flen
	} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_FSW) == INSN_సరిపోలిక_FSW) {
		len = 4;
		fp = true;
	} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_FSD) == INSN_సరిపోలిక_FSD) {
		len = 8;
		fp = true;
	} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_C_FSD) == INSN_సరిపోలిక_C_FSD) {
		/* Zcd */
		len = 8;
		imm = RVC_SD_IMM(insn);
		c_store = true;
		fp = true;
	} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_C_FSDSP) == INSN_సరిపోలిక_C_FSDSP) {
		/* Zcd */
		len = 8;
		imm = RVC_SDSP_IMM(insn);
		c_stsp = true;
		fp = true;
#అయితే_ముగింపు
	} లేకపోతే {
		prev_xlen = sbi_regs_prev_xlen(regs);
	}

	/**
	 * Must distinguish between rv64 and rv32, RVC instructions have
	 * overlapping encoding:
	 *     c.sd in rv64 == c.fsw in rv32
	 *     c.sdsp in rv64 == c.fswsp in rv32
	 */
	ఒకవేళ (prev_xlen == 64) {
		/* RV64 Only: sd, c.sd, c.sdsp */
		ఒకవేళ ((insn & INSN_ముసుగు_SD) == INSN_సరిపోలిక_SD) {
			len = 8;
		} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_C_SD) == INSN_సరిపోలిక_C_SD) {
			/* Zca */
			len = 8;
			imm = RVC_SD_IMM(insn);
			c_store = true;
		} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_C_SDSP) == INSN_సరిపోలిక_C_SDSP) {
			/* Zca */
			len = 8;
			imm = RVC_SDSP_IMM(insn);
			c_stsp = true;
		}
#నిర్వచితమైతే __riscv_flen
	} లేకపోతే ఒకవేళ (prev_xlen == 32) {
		/* RV32 Only: c.fsw, c.fswsp */
		ఒకవేళ ((insn & INSN_ముసుగు_C_FSW) == INSN_సరిపోలిక_C_FSW) {
			/* Zcf */
			len = 4;
			imm = RVC_SW_IMM(insn);
			c_store = true;
			fp = true;
		} లేకపోతే ఒకవేళ ((insn & INSN_ముసుగు_C_FSWSP) == INSN_సరిపోలిక_C_FSWSP) {
			/* Zcf */
			len = 4;
			imm = RVC_SWSP_IMM(insn);
			c_stsp = true;
			fp = true;
		}
#అయితే_ముగింపు
	}

	ఒకవేళ (!fp) {
		ఒకవేళ (c_store)
			val.దత్తాంశం_ulong = GET_RS2S(insn, regs);
		లేకపోతే ఒకవేళ (c_stsp)
			val.దత్తాంశం_ulong = GET_RS2C(insn, regs);
		లేకపోతే
			val.దత్తాంశం_ulong = GET_RS2(insn, regs);
#నిర్వచితమైతే __riscv_flen
	} లేకపోతే ఒకవేళ (len == 8) {
		ఒకవేళ (c_store)
			val.దత్తాంశం_u64 = GET_F64_RS2S(insn, regs);
		లేకపోతే ఒకవేళ (c_stsp)
			val.దత్తాంశం_u64 = GET_F64_RS2C(insn, regs);
		లేకపోతే
			val.దత్తాంశం_u64 = GET_F64_RS2(insn, regs);
	} లేకపోతే {
		ఒకవేళ (c_store)
			val.దత్తాంశం_ulong = GET_F32_RS2S(insn, regs);
		లేకపోతే ఒకవేళ (c_stsp)
			val.దత్తాంశం_ulong = GET_F32_RS2C(insn, regs);
		లేకపోతే
			val.దత్తాంశం_ulong = GET_F32_RS2(insn, regs);
#అయితే_ముగింపు
	}

	ఒకవేళ (!len) // Unknown instruction
		వెళ్ళు do_emu;

#ఒకవేళ !defined(OPENSBI_DEBUG)
	/**
	 * For misaligned faults. Skip offset calculation unless DEBUG
	 * builds. It helps validating OpenSBI and HW.
	 */
	ఒకవేళ (orig_trap->cause == CAUSE_MISALIGNED_STORE)
		వెళ్ళు do_emu;
#అయితే_ముగింపు

	ఒకవేళ (xform)
		/* Transformed insn */
		off = GET_RS1_అంకె(insn);
	లేకపోతే ఒకవేళ (c_store)
		/* non SP-based compressed store */
		off = orig_trap->tval - GET_RS1S(insn, regs) - imm;
	లేకపోతే ఒకవేళ (c_stsp)
		/* SP-based compressed store */
		off = orig_trap->tval - REG_VAL(2, regs) - imm;
	లేకపోతే
		/* S-type non-compressed store */
		off = orig_trap->tval - GET_RS1(insn, regs) - (ulong)IMM_S(insn);
	/**
	 * Normalize offset, in case the XLEN of unpriv mode is smaller,
	 * and/or pointer masking is in effect
	 */
	off &= (len - 1);

do_emu:
	rc = emu(insn, len, orig_trap->tval - off, val, tcntx);
	ఒకవేళ (rc <= 0)
		తిరిగి rc;

	regs->mepc += insn_len;

	తిరిగి 0;
}

స్థిర పూర్ణాంకం sbi_misaligned_ld_emulator(ulong insn, పూర్ణాంకం rlen, ulong addr,
				      సంఘం sbi_ldst_దత్తాంశం *బహిర్గతం_val,
				      నిర్మాణం sbi_trap_context *tcntx)
{
	స్థిరరాశి నిర్మాణం sbi_trap_info *orig_trap = &tcntx->trap;
	నిర్మాణం sbi_trap_regs *regs = &tcntx->regs;
	నిర్మాణం sbi_trap_info uptrap;

	ఒకవేళ (!rlen) {
		ఒకవేళ (IS_సదిశ_ఎక్కించు_STORE(insn))
			తిరిగి sbi_misaligned_v_ld_emulator(insn, tcntx);
		లేకపోతే
			/* Unrecognized instruction. Can't emulate it. */
			తిరిగి sbi_trap_redirect(regs, orig_trap);
	}
	/* For misaligned fault, addr must be the same as orig_trap->tval */
	ఒకవేళ (addr != orig_trap->tval)
		తిరిగి SBI_EFAIL;

	sbi_ఎక్కించు_loop(బహిర్గతం_val->దత్తాంశం_బైట్లు, addr, rlen, &uptrap);
	ఒకవేళ (uptrap.cause) {
		sbi_misaligned_tinst_fixup(orig_trap, &uptrap);
		తిరిగి sbi_trap_redirect(regs, &uptrap);
	}
	తిరిగి rlen;
}

పూర్ణాంకం sbi_misaligned_ఎక్కించు_handler(నిర్మాణం sbi_trap_context *tcntx)
{
	తిరిగి sbi_trap_emulate_ఎక్కించు(tcntx, sbi_misaligned_ld_emulator);
}

స్థిర పూర్ణాంకం sbi_misaligned_st_emulator(ulong insn, పూర్ణాంకం wlen, ulong addr,
				      సంఘం sbi_ldst_దత్తాంశం లోపల_val,
				      నిర్మాణం sbi_trap_context *tcntx)
{
	స్థిరరాశి నిర్మాణం sbi_trap_info *orig_trap = &tcntx->trap;
	నిర్మాణం sbi_trap_regs *regs = &tcntx->regs;
	నిర్మాణం sbi_trap_info uptrap;

	ఒకవేళ (!wlen) {
		ఒకవేళ (IS_సదిశ_ఎక్కించు_STORE(insn))
			తిరిగి sbi_misaligned_v_st_emulator(insn, tcntx);
		లేకపోతే
			/* Unrecognized instruction. Can't emulate it. */
			తిరిగి sbi_trap_redirect(regs, orig_trap);
	}
	/* For misaligned fault, addr must be the same as orig_trap->tval */
	ఒకవేళ (addr != orig_trap->tval)
		తిరిగి SBI_EFAIL;

	sbi_store_loop(లోపల_val.దత్తాంశం_బైట్లు, addr, wlen, &uptrap);
	ఒకవేళ (uptrap.cause) {
		sbi_misaligned_tinst_fixup(orig_trap, &uptrap);
		తిరిగి sbi_trap_redirect(regs, &uptrap);
	}
	తిరిగి wlen;
}

పూర్ణాంకం sbi_misaligned_store_handler(నిర్మాణం sbi_trap_context *tcntx)
{
	తిరిగి sbi_trap_emulate_store(tcntx, sbi_misaligned_st_emulator);
}

స్థిర పూర్ణాంకం sbi_ld_access_emulator(ulong insn, పూర్ణాంకం rlen, ulong addr,
				  సంఘం sbi_ldst_దత్తాంశం *బహిర్గతం_val,
				  నిర్మాణం sbi_trap_context *tcntx)
{
	స్థిరరాశి నిర్మాణం sbi_trap_info *orig_trap = &tcntx->trap;
	నిర్మాణం sbi_trap_regs *regs = &tcntx->regs;
	పూర్ణాంకం rc;

	/* If fault came from M mode, just fail */
	ఒకవేళ (sbi_mstatus_prev_బహుళకం(regs->mstatus) == PRV_M)
		తిరిగి SBI_EINVAL;

	rc = sbi_platform_emulate_ఎక్కించు(sbi_platform_thishart_ptr(),
				       insn, rlen, addr, బహిర్గతం_val, tcntx);

	/* If platform emulator failed, we redirect instead of fail */
	ఒకవేళ (rc < 0)
		తిరిగి sbi_trap_redirect(regs, orig_trap);

	తిరిగి rc;
}

పూర్ణాంకం sbi_ఎక్కించు_access_handler(నిర్మాణం sbi_trap_context *tcntx)
{
	తిరిగి sbi_trap_emulate_ఎక్కించు(tcntx, sbi_ld_access_emulator);
}

స్థిర పూర్ణాంకం sbi_st_access_emulator(ulong insn, పూర్ణాంకం wlen, ulong addr,
				  సంఘం sbi_ldst_దత్తాంశం లోపల_val,
				  నిర్మాణం sbi_trap_context *tcntx)
{
	స్థిరరాశి నిర్మాణం sbi_trap_info *orig_trap = &tcntx->trap;
	నిర్మాణం sbi_trap_regs *regs = &tcntx->regs;
	పూర్ణాంకం rc;

	/* If fault came from M mode, just fail */
	ఒకవేళ (sbi_mstatus_prev_బహుళకం(regs->mstatus) == PRV_M)
		తిరిగి SBI_EINVAL;

	rc = sbi_platform_emulate_store(sbi_platform_thishart_ptr(),
					insn, wlen, addr, లోపల_val, tcntx);

	/* If platform emulator failed, we redirect instead of fail */
	ఒకవేళ (rc < 0)
		తిరిగి sbi_trap_redirect(regs, orig_trap);

	తిరిగి rc;
}

పూర్ణాంకం sbi_store_access_handler(నిర్మాణం sbi_trap_context *tcntx)
{
	తిరిగి sbi_trap_emulate_store(tcntx, sbi_st_access_emulator);
}
