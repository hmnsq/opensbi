/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_encoding.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_fp.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap_ldst.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_unpriv.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_platform.h>

/**
 * Load emulator callback:
 *   Refer to comments of `sbi_platform_emulate_load`.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*sbi_trap_ld_emulator)(ulong insn, ಪೂರ್ಣಾಙ್ಕ rlen, ulong raddr,
				    ಸಙ್ಘ sbi_ldst_data *out_val,
				    ಸಂರಚನಾ sbi_trap_context *tcntx);

/**
 * Store emulator callback:
 *   Refer to comments of `sbi_platform_emulate_store`.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*sbi_trap_st_emulator)(ulong insn, ಪೂರ್ಣಾಙ್ಕ wlen, ulong waddr,
				    ಸಙ್ಘ sbi_ldst_data in_val,
				    ಸಂರಚನಾ sbi_trap_context *tcntx);

/**
 * Handling of misaligned fault is done by a collection of smaller, but
 * aligned load/store(s). Another fault (load/store, page fault...) can
 * arise from any of them, then the handling gets aborted. We must fixup
 * the tinst to pretend the fault was rised from the original insn.
 * Specifically, fixup the offset field using the tval diff between the
 * new trap and the original one (if required).
 */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ sbi_misaligned_tinst_fixup(
			ಧ್ರುವ ಸಂರಚನಾ sbi_trap_info *orig_trap,
			      ಸಂರಚನಾ sbi_trap_info *uptrap)
{
	ulong offset = uptrap->tval - orig_trap->tval;

	/*
	 * The function is called in code path for handling a scalar
	 * load/store misaligned fault, thus the new uptrap can't have
	 * custom value of tinst
	 */
	ಯದಿ (uptrap->tinst == INSN_PSEUDO_VS_LOAD ||
	    uptrap->tinst == INSN_PSEUDO_VS_STORE)
		/* Use uptrap as-is for guest-page faults */
		ಪ್ರತ್ಯಾಗಮ;
	/*
	 * Only fixup if orig tinst is valid. Otherwise, discard the
	 * new tinst to be on the safe side. Never use new tinst as-is!
	 * It's load/store width surely mismatches the original width.
	 * For vector, discard it regardless. It doesn't make sense to
	 * have a transformed tinst
	 */
	ಅನ್ಯಥಾ ಯದಿ (orig_trap->tinst == 0)
		uptrap->tinst = 0;
	ಅನ್ಯಥಾ
		uptrap->tinst = orig_trap->tinst | (offset << SH_RS1);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool sbi_trap_tinst_valid(ulong tinst)
{
	/*
	 * Bit[0] == 1 implies trapped instruction value is
	 * transformed instruction or custom instruction.
	 * Also do proper checking per Privileged ISA 19.6.3,
	 * and make sure high 32 bits of tinst is 0
	 */
	ಪ್ರತ್ಯಾಗಮ tinst == (uint32_t)tinst && (tinst & 0x1);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_trap_emulate_load(ಸಂರಚನಾ sbi_trap_context *tcntx,
				 sbi_trap_ld_emulator emu)
{
	ಧ್ರುವ ಸಂರಚನಾ sbi_trap_info *orig_trap = &tcntx->trap;
	ಸಂರಚನಾ sbi_trap_regs *regs = &tcntx->regs;
	ulong insn, insn_len, imm = 0, shift = 0, off = 0;
	ಸಙ್ಘ sbi_ldst_data val = { 0 };
	ಸಂರಚನಾ sbi_trap_info uptrap;
	bool xform = false, fp = false, c_load = false, c_ldsp = false;
	ಪೂರ್ಣಾಙ್ಕ rc, len = 0, prev_xlen = 0;

	ಯದಿ (sbi_trap_tinst_valid(orig_trap->tinst)) {
		xform	 = true;
		insn	 = orig_trap->tinst | INSN_16BIT_MASK;
		insn_len = (orig_trap->tinst & 0x2) ? INSN_LEN(insn) : 2;
	} ಅನ್ಯಥಾ {
		/* trapped instruction value is zero or special value */
		insn = sbi_get_insn(regs->mepc, &uptrap);
		ಯದಿ (uptrap.cause) {
			ಪ್ರತ್ಯಾಗಮ sbi_trap_redirect(regs, &uptrap);
		}
		insn_len = INSN_LEN(insn);
	}

	/**
	 * Common for RV32/RV64:
	 *    lb, lbu, lh, lhu, lw, flw, flw
	 *    c.lbu, c.lh, c.lhu, c.lw, c.lwsp, c.fld, c.fldsp
	 */
	ಯದಿ ((insn & INSN_MASK_LB) == INSN_MATCH_LB) {
		len = -1;
	} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_LBU) == INSN_MATCH_LBU) {
		len = 1;
	} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_C_LBU) == INSN_MATCH_C_LBU) {
		/* Zcb */
		len = 1;
		imm = RVC_LB_IMM(insn);
		c_load = true;
	} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_LH) == INSN_MATCH_LH) {
		len = -2;
	} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_C_LH) == INSN_MATCH_C_LH) {
		/* Zcb */
		len = -2;
		imm = RVC_LH_IMM(insn);
		c_load = true;
	} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_LHU) == INSN_MATCH_LHU) {
		len = 2;
	} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_C_LHU) == INSN_MATCH_C_LHU) {
		/* Zcb */
		len = 2;
		imm = RVC_LH_IMM(insn);
		c_load = true;
	} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_LW) == INSN_MATCH_LW) {
		len = -4;
	} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_C_LW) == INSN_MATCH_C_LW) {
		/* Zca */
		len = -4;
		imm = RVC_LW_IMM(insn);
		c_load = true;
	} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_C_LWSP) == INSN_MATCH_C_LWSP &&
		GET_RD_ಅಂಕೆ(insn)) {
		/* Zca */
		len = -4;
		imm = RVC_LWSP_IMM(insn);
		c_ldsp = true;
#ifdef __riscv_flen
	} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_FLW) == INSN_MATCH_FLW) {
		len = 4;
		fp = true;
	} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_FLD) == INSN_MATCH_FLD) {
		len = 8;
		fp = true;
	} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_C_FLD) == INSN_MATCH_C_FLD) {
		/* Zcd */
		len = 8;
		imm = RVC_LD_IMM(insn);
		c_load = true;
		fp = true;
	} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_C_FLDSP) == INSN_MATCH_C_FLDSP) {
		/* Zcd */
		len = 8;
		imm = RVC_LDSP_IMM(insn);
		c_ldsp = true;
		fp = true;
#endif
	} ಅನ್ಯಥಾ {
		prev_xlen = sbi_regs_prev_xlen(regs);
	}

	/**
	 * Must distinguish between rv64 and rv32, RVC instructions have
	 * overlapping encoding:
	 *     c.ld in rv64 == c.flw in rv32
	 *     c.ldsp in rv64 == c.flwsp in rv32
	 */
	ಯದಿ (prev_xlen == 64) {
		/* RV64 Only: lwu, ld, c.ld, c.ldsp  */
		ಯದಿ ((insn & INSN_MASK_LWU) == INSN_MATCH_LWU) {
			len = 4;
		} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_LD) == INSN_MATCH_LD) {
			len = 8;
		} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_C_LD) == INSN_MATCH_C_LD) {
			/* Zca */
			len = 8;
			imm = RVC_LD_IMM(insn);
			c_load = true;
		} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_C_LDSP) == INSN_MATCH_C_LDSP &&
			GET_RD_ಅಂಕೆ(insn)) {
			/* Zca */
			len = 8;
			imm = RVC_LDSP_IMM(insn);
			c_ldsp = true;
		}
#ifdef __riscv_flen
	} ಅನ್ಯಥಾ ಯದಿ (prev_xlen == 32) {
		/* RV32 Only: c.flw, c.flwsp */
		ಯದಿ ((insn & INSN_MASK_C_FLW) == INSN_MATCH_C_FLW) {
			/* Zcf */
			len = 4;
			imm = RVC_LW_IMM(insn);
			c_load = true;
			fp = true;
		} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_C_FLWSP) == INSN_MATCH_C_FLWSP) {
			/* Zcf */
			len = 4;
			imm = RVC_LWSP_IMM(insn);
			c_ldsp = true;
			fp = true;
		}
#endif
	}

	ಯದಿ (len < 0) {
		len = -len;
		shift = 8 * (ಪರಿಮಾಣ(ulong) - len);
	}

	ಯದಿ (!len) // Unknown instruction
		ಗಚ್ಛ do_emu;

#ಯದಿ !defined(OPENSBI_DEBUG)
	/**
	 * For misaligned faults. Skip offset calculation unless DEBUG
	 * builds. It helps validating OpenSBI and HW.
	 */
	ಯದಿ (orig_trap->cause == CAUSE_MISALIGNED_LOAD)
		ಗಚ್ಛ do_emu;
#endif

	ಯದಿ (xform)
		/* Transformed insn */
		off = GET_RS1_ಅಂಕೆ(insn);
	ಅನ್ಯಥಾ ಯದಿ (c_load)
		/* non SP-based compressed load */
		off = orig_trap->tval - GET_RS1S(insn, regs) - imm;
	ಅನ್ಯಥಾ ಯದಿ (c_ldsp)
		/* SP-based compressed load */
		off = orig_trap->tval - REG_VAL(2, regs) - imm;
	ಅನ್ಯಥಾ
		/* I-type non-compressed load */
		off = orig_trap->tval - GET_RS1(insn, regs) - (ulong)IMM_I(insn);
	/**
	 * Normalize offset, in case the XLEN of unpriv mode is smaller,
	 * and/or pointer masking is in effect
	 */
	off &= (len - 1);

do_emu:
	rc = emu(insn, len, orig_trap->tval - off, &val, tcntx);
	ಯದಿ (rc <= 0)
		ಪ್ರತ್ಯಾಗಮ rc;
	ಯದಿ (!len)
		ಗಚ್ಛ epc_fixup;

	ಯದಿ (!fp) {
		ulong v = ((long)(val.data_ulong << shift)) >> shift;

		ಯದಿ (c_load)
			SET_RDS(insn, regs, v);
		ಅನ್ಯಥಾ
			SET_RD(insn, regs, v);
#ifdef __riscv_flen
	} ಅನ್ಯಥಾ ಯದಿ (len == 8) {
		ಯದಿ (c_load)
			SET_F64_RDS(insn, regs, val.data_u64);
		ಅನ್ಯಥಾ
			SET_F64_RD(insn, regs, val.data_u64);
	} ಅನ್ಯಥಾ {
		ಯದಿ (c_load)
			SET_F32_RDS(insn, regs, val.data_ulong);
		ಅನ್ಯಥಾ
			SET_F32_RD(insn, regs, val.data_ulong);
#endif
	}

epc_fixup:
	regs->mepc += insn_len;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_trap_emulate_store(ಸಂರಚನಾ sbi_trap_context *tcntx,
				  sbi_trap_st_emulator emu)
{
	ಧ್ರುವ ಸಂರಚನಾ sbi_trap_info *orig_trap = &tcntx->trap;
	ಸಂರಚನಾ sbi_trap_regs *regs = &tcntx->regs;
	ulong insn, insn_len, imm = 0, off = 0;
	ಸಙ್ಘ sbi_ldst_data val;
	ಸಂರಚನಾ sbi_trap_info uptrap;
	bool xform = false, fp = false, c_store = false, c_stsp = false;
	ಪೂರ್ಣಾಙ್ಕ rc, len = 0, prev_xlen = 0;

	ಯದಿ (sbi_trap_tinst_valid(orig_trap->tinst)) {
		xform	 = true;
		insn	 = orig_trap->tinst | INSN_16BIT_MASK;
		insn_len = (orig_trap->tinst & 0x2) ? INSN_LEN(insn) : 2;
	} ಅನ್ಯಥಾ {
		/* trapped instruction value is zero or special value */
		insn = sbi_get_insn(regs->mepc, &uptrap);
		ಯದಿ (uptrap.cause) {
			ಪ್ರತ್ಯಾಗಮ sbi_trap_redirect(regs, &uptrap);
		}
		insn_len = INSN_LEN(insn);
	}

	/**
	 * Common for RV32/RV64:
	 *    sb, sh, sw, fsw, fsd
	 *    c.sb, c.sh, c.sw, c.swsp, c.fsd, c.fsdsp
	 */
	ಯದಿ ((insn & INSN_MASK_SB) == INSN_MATCH_SB) {
		len = 1;
	} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_C_SB) == INSN_MATCH_C_SB) {
		/* Zcb */
		len = 1;
		imm = RVC_SB_IMM(insn);
		c_store = true;
	} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_SH) == INSN_MATCH_SH) {
		len = 2;
	} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_C_SH) == INSN_MATCH_C_SH) {
		/* Zcb */
		len = 2;
		imm = RVC_SH_IMM(insn);
		c_store = true;
	} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_SW) == INSN_MATCH_SW) {
		len = 4;
	} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_C_SW) == INSN_MATCH_C_SW) {
		/* Zca */
		len = 4;
		imm = RVC_SW_IMM(insn);
		c_store = true;
	} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_C_SWSP) == INSN_MATCH_C_SWSP) {
		/* Zca */
		len = 4;
		imm = RVC_SWSP_IMM(insn);
		c_stsp = true;
#ifdef __riscv_flen
	} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_FSW) == INSN_MATCH_FSW) {
		len = 4;
		fp = true;
	} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_FSD) == INSN_MATCH_FSD) {
		len = 8;
		fp = true;
	} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_C_FSD) == INSN_MATCH_C_FSD) {
		/* Zcd */
		len = 8;
		imm = RVC_SD_IMM(insn);
		c_store = true;
		fp = true;
	} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_C_FSDSP) == INSN_MATCH_C_FSDSP) {
		/* Zcd */
		len = 8;
		imm = RVC_SDSP_IMM(insn);
		c_stsp = true;
		fp = true;
#endif
	} ಅನ್ಯಥಾ {
		prev_xlen = sbi_regs_prev_xlen(regs);
	}

	/**
	 * Must distinguish between rv64 and rv32, RVC instructions have
	 * overlapping encoding:
	 *     c.sd in rv64 == c.fsw in rv32
	 *     c.sdsp in rv64 == c.fswsp in rv32
	 */
	ಯದಿ (prev_xlen == 64) {
		/* RV64 Only: sd, c.sd, c.sdsp */
		ಯದಿ ((insn & INSN_MASK_SD) == INSN_MATCH_SD) {
			len = 8;
		} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_C_SD) == INSN_MATCH_C_SD) {
			/* Zca */
			len = 8;
			imm = RVC_SD_IMM(insn);
			c_store = true;
		} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_C_SDSP) == INSN_MATCH_C_SDSP) {
			/* Zca */
			len = 8;
			imm = RVC_SDSP_IMM(insn);
			c_stsp = true;
		}
#ifdef __riscv_flen
	} ಅನ್ಯಥಾ ಯದಿ (prev_xlen == 32) {
		/* RV32 Only: c.fsw, c.fswsp */
		ಯದಿ ((insn & INSN_MASK_C_FSW) == INSN_MATCH_C_FSW) {
			/* Zcf */
			len = 4;
			imm = RVC_SW_IMM(insn);
			c_store = true;
			fp = true;
		} ಅನ್ಯಥಾ ಯದಿ ((insn & INSN_MASK_C_FSWSP) == INSN_MATCH_C_FSWSP) {
			/* Zcf */
			len = 4;
			imm = RVC_SWSP_IMM(insn);
			c_stsp = true;
			fp = true;
		}
#endif
	}

	ಯದಿ (!fp) {
		ಯದಿ (c_store)
			val.data_ulong = GET_RS2S(insn, regs);
		ಅನ್ಯಥಾ ಯದಿ (c_stsp)
			val.data_ulong = GET_RS2C(insn, regs);
		ಅನ್ಯಥಾ
			val.data_ulong = GET_RS2(insn, regs);
#ifdef __riscv_flen
	} ಅನ್ಯಥಾ ಯದಿ (len == 8) {
		ಯದಿ (c_store)
			val.data_u64 = GET_F64_RS2S(insn, regs);
		ಅನ್ಯಥಾ ಯದಿ (c_stsp)
			val.data_u64 = GET_F64_RS2C(insn, regs);
		ಅನ್ಯಥಾ
			val.data_u64 = GET_F64_RS2(insn, regs);
	} ಅನ್ಯಥಾ {
		ಯದಿ (c_store)
			val.data_ulong = GET_F32_RS2S(insn, regs);
		ಅನ್ಯಥಾ ಯದಿ (c_stsp)
			val.data_ulong = GET_F32_RS2C(insn, regs);
		ಅನ್ಯಥಾ
			val.data_ulong = GET_F32_RS2(insn, regs);
#endif
	}

	ಯದಿ (!len) // Unknown instruction
		ಗಚ್ಛ do_emu;

#ಯದಿ !defined(OPENSBI_DEBUG)
	/**
	 * For misaligned faults. Skip offset calculation unless DEBUG
	 * builds. It helps validating OpenSBI and HW.
	 */
	ಯದಿ (orig_trap->cause == CAUSE_MISALIGNED_STORE)
		ಗಚ್ಛ do_emu;
#endif

	ಯದಿ (xform)
		/* Transformed insn */
		off = GET_RS1_ಅಂಕೆ(insn);
	ಅನ್ಯಥಾ ಯದಿ (c_store)
		/* non SP-based compressed store */
		off = orig_trap->tval - GET_RS1S(insn, regs) - imm;
	ಅನ್ಯಥಾ ಯದಿ (c_stsp)
		/* SP-based compressed store */
		off = orig_trap->tval - REG_VAL(2, regs) - imm;
	ಅನ್ಯಥಾ
		/* S-type non-compressed store */
		off = orig_trap->tval - GET_RS1(insn, regs) - (ulong)IMM_S(insn);
	/**
	 * Normalize offset, in case the XLEN of unpriv mode is smaller,
	 * and/or pointer masking is in effect
	 */
	off &= (len - 1);

do_emu:
	rc = emu(insn, len, orig_trap->tval - off, val, tcntx);
	ಯದಿ (rc <= 0)
		ಪ್ರತ್ಯಾಗಮ rc;

	regs->mepc += insn_len;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_misaligned_ld_emulator(ulong insn, ಪೂರ್ಣಾಙ್ಕ rlen, ulong addr,
				      ಸಙ್ಘ sbi_ldst_data *out_val,
				      ಸಂರಚನಾ sbi_trap_context *tcntx)
{
	ಧ್ರುವ ಸಂರಚನಾ sbi_trap_info *orig_trap = &tcntx->trap;
	ಸಂರಚನಾ sbi_trap_regs *regs = &tcntx->regs;
	ಸಂರಚನಾ sbi_trap_info uptrap;

	ಯದಿ (!rlen) {
		ಯದಿ (IS_VECTOR_LOAD_STORE(insn))
			ಪ್ರತ್ಯಾಗಮ sbi_misaligned_v_ld_emulator(insn, tcntx);
		ಅನ್ಯಥಾ
			/* Unrecognized instruction. Can't emulate it. */
			ಪ್ರತ್ಯಾಗಮ sbi_trap_redirect(regs, orig_trap);
	}
	/* For misaligned fault, addr must be the same as orig_trap->tval */
	ಯದಿ (addr != orig_trap->tval)
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;

	sbi_load_loop(out_val->data_bytes, addr, rlen, &uptrap);
	ಯದಿ (uptrap.cause) {
		sbi_misaligned_tinst_fixup(orig_trap, &uptrap);
		ಪ್ರತ್ಯಾಗಮ sbi_trap_redirect(regs, &uptrap);
	}
	ಪ್ರತ್ಯಾಗಮ rlen;
}

ಪೂರ್ಣಾಙ್ಕ sbi_misaligned_load_handler(ಸಂರಚನಾ sbi_trap_context *tcntx)
{
	ಪ್ರತ್ಯಾಗಮ sbi_trap_emulate_load(tcntx, sbi_misaligned_ld_emulator);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_misaligned_st_emulator(ulong insn, ಪೂರ್ಣಾಙ್ಕ wlen, ulong addr,
				      ಸಙ್ಘ sbi_ldst_data in_val,
				      ಸಂರಚನಾ sbi_trap_context *tcntx)
{
	ಧ್ರುವ ಸಂರಚನಾ sbi_trap_info *orig_trap = &tcntx->trap;
	ಸಂರಚನಾ sbi_trap_regs *regs = &tcntx->regs;
	ಸಂರಚನಾ sbi_trap_info uptrap;

	ಯದಿ (!wlen) {
		ಯದಿ (IS_VECTOR_LOAD_STORE(insn))
			ಪ್ರತ್ಯಾಗಮ sbi_misaligned_v_st_emulator(insn, tcntx);
		ಅನ್ಯಥಾ
			/* Unrecognized instruction. Can't emulate it. */
			ಪ್ರತ್ಯಾಗಮ sbi_trap_redirect(regs, orig_trap);
	}
	/* For misaligned fault, addr must be the same as orig_trap->tval */
	ಯದಿ (addr != orig_trap->tval)
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;

	sbi_store_loop(in_val.data_bytes, addr, wlen, &uptrap);
	ಯದಿ (uptrap.cause) {
		sbi_misaligned_tinst_fixup(orig_trap, &uptrap);
		ಪ್ರತ್ಯಾಗಮ sbi_trap_redirect(regs, &uptrap);
	}
	ಪ್ರತ್ಯಾಗಮ wlen;
}

ಪೂರ್ಣಾಙ್ಕ sbi_misaligned_store_handler(ಸಂರಚನಾ sbi_trap_context *tcntx)
{
	ಪ್ರತ್ಯಾಗಮ sbi_trap_emulate_store(tcntx, sbi_misaligned_st_emulator);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ld_access_emulator(ulong insn, ಪೂರ್ಣಾಙ್ಕ rlen, ulong addr,
				  ಸಙ್ಘ sbi_ldst_data *out_val,
				  ಸಂರಚನಾ sbi_trap_context *tcntx)
{
	ಧ್ರುವ ಸಂರಚನಾ sbi_trap_info *orig_trap = &tcntx->trap;
	ಸಂರಚನಾ sbi_trap_regs *regs = &tcntx->regs;
	ಪೂರ್ಣಾಙ್ಕ rc;

	/* If fault came from M mode, just fail */
	ಯದಿ (sbi_mstatus_prev_mode(regs->mstatus) == PRV_M)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	rc = sbi_platform_emulate_load(sbi_platform_thishart_ptr(),
				       insn, rlen, addr, out_val, tcntx);

	/* If platform emulator failed, we redirect instead of fail */
	ಯದಿ (rc < 0)
		ಪ್ರತ್ಯಾಗಮ sbi_trap_redirect(regs, orig_trap);

	ಪ್ರತ್ಯಾಗಮ rc;
}

ಪೂರ್ಣಾಙ್ಕ sbi_load_access_handler(ಸಂರಚನಾ sbi_trap_context *tcntx)
{
	ಪ್ರತ್ಯಾಗಮ sbi_trap_emulate_load(tcntx, sbi_ld_access_emulator);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_st_access_emulator(ulong insn, ಪೂರ್ಣಾಙ್ಕ wlen, ulong addr,
				  ಸಙ್ಘ sbi_ldst_data in_val,
				  ಸಂರಚನಾ sbi_trap_context *tcntx)
{
	ಧ್ರುವ ಸಂರಚನಾ sbi_trap_info *orig_trap = &tcntx->trap;
	ಸಂರಚನಾ sbi_trap_regs *regs = &tcntx->regs;
	ಪೂರ್ಣಾಙ್ಕ rc;

	/* If fault came from M mode, just fail */
	ಯದಿ (sbi_mstatus_prev_mode(regs->mstatus) == PRV_M)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	rc = sbi_platform_emulate_store(sbi_platform_thishart_ptr(),
					insn, wlen, addr, in_val, tcntx);

	/* If platform emulator failed, we redirect instead of fail */
	ಯದಿ (rc < 0)
		ಪ್ರತ್ಯಾಗಮ sbi_trap_redirect(regs, orig_trap);

	ಪ್ರತ್ಯಾಗಮ rc;
}

ಪೂರ್ಣಾಙ್ಕ sbi_store_access_handler(ಸಂರಚನಾ sbi_trap_context *tcntx)
{
	ಪ್ರತ್ಯಾಗಮ sbi_trap_emulate_store(tcntx, sbi_st_access_emulator);
}
