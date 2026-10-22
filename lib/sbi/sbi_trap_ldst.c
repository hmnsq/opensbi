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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*sbi_trap_ld_emulator)(ulong insn, ಪೂರ್ಣಾಂಕ rlen, ulong raddr,
				    ಒಕ್ಕೂಟ sbi_ldst_ದತ್ತಾಂಶ *ಹೊರಹೋಗು_val,
				    ರಚನೆ sbi_trap_context *tcntx);

/**
 * Store emulator callback:
 *   Refer to comments of `sbi_platform_emulate_store`.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*sbi_trap_st_emulator)(ulong insn, ಪೂರ್ಣಾಂಕ wlen, ulong waddr,
				    ಒಕ್ಕೂಟ sbi_ldst_ದತ್ತಾಂಶ ಒಳ_val,
				    ರಚನೆ sbi_trap_context *tcntx);

/**
 * Handling of misaligned fault is done by a collection of smaller, but
 * aligned load/store(s). Another fault (load/store, page fault...) can
 * arise from any of them, then the handling gets aborted. We must fixup
 * the tinst to pretend the fault was rised from the original insn.
 * Specifically, fixup the offset field using the tval diff between the
 * new trap and the original one (if required).
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ sbi_misaligned_tinst_fixup(
			ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_trap_info *orig_trap,
			      ರಚನೆ sbi_trap_info *uptrap)
{
	ulong ಸ್ಥಾನಾಂತರ = uptrap->tval - orig_trap->tval;

	/*
	 * The function is called in code path for handling a scalar
	 * load/store misaligned fault, thus the new uptrap can't have
	 * custom value of tinst
	 */
	ಆದರೆ (uptrap->tinst == INSN_PSEUDO_VS_ತುಂಬು ||
	    uptrap->tinst == INSN_PSEUDO_VS_STORE)
		/* Use uptrap as-is for guest-page faults */
		ಹಿಂತಿರುಗಿ;
	/*
	 * Only fixup if orig tinst is valid. Otherwise, discard the
	 * new tinst to be on the safe side. Never use new tinst as-is!
	 * It's load/store width surely mismatches the original width.
	 * For vector, discard it regardless. It doesn't make sense to
	 * have a transformed tinst
	 */
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (orig_trap->tinst == 0)
		uptrap->tinst = 0;
	ಇಲ್ಲದಿದ್ದರೆ
		uptrap->tinst = orig_trap->tinst | (ಸ್ಥಾನಾಂತರ << SH_RS1);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool sbi_trap_tinst_valid(ulong tinst)
{
	/*
	 * Bit[0] == 1 implies trapped instruction value is
	 * transformed instruction or custom instruction.
	 * Also do proper checking per Privileged ISA 19.6.3,
	 * and make sure high 32 bits of tinst is 0
	 */
	ಹಿಂತಿರುಗಿ tinst == (uint32_t)tinst && (tinst & 0x1);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_trap_emulate_ತುಂಬು(ರಚನೆ sbi_trap_context *tcntx,
				 sbi_trap_ld_emulator emu)
{
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_trap_info *orig_trap = &tcntx->trap;
	ರಚನೆ sbi_trap_regs *regs = &tcntx->regs;
	ulong insn, insn_len, imm = 0, ಸರಿಸು = 0, off = 0;
	ಒಕ್ಕೂಟ sbi_ldst_ದತ್ತಾಂಶ val = { 0 };
	ರಚನೆ sbi_trap_info uptrap;
	bool xform = false, fp = false, c_ತುಂಬು = false, c_ldsp = false;
	ಪೂರ್ಣಾಂಕ rc, len = 0, prev_xlen = 0;

	ಆದರೆ (sbi_trap_tinst_valid(orig_trap->tinst)) {
		xform	 = true;
		insn	 = orig_trap->tinst | INSN_16_ಬಿಟ್_ಮುಖವಾಡ;
		insn_len = (orig_trap->tinst & 0x2) ? INSN_LEN(insn) : 2;
	} ಇಲ್ಲದಿದ್ದರೆ {
		/* trapped instruction value is zero or special value */
		insn = sbi_get_insn(regs->mepc, &uptrap);
		ಆದರೆ (uptrap.cause) {
			ಹಿಂತಿರುಗಿ sbi_trap_redirect(regs, &uptrap);
		}
		insn_len = INSN_LEN(insn);
	}

	/**
	 * Common for RV32/RV64:
	 *    lb, lbu, lh, lhu, lw, flw, flw
	 *    c.lbu, c.lh, c.lhu, c.lw, c.lwsp, c.fld, c.fldsp
	 */
	ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_LB) == INSN_ಹೊಂದಿಕೆ_LB) {
		len = -1;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_LBU) == INSN_ಹೊಂದಿಕೆ_LBU) {
		len = 1;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_C_LBU) == INSN_ಹೊಂದಿಕೆ_C_LBU) {
		/* Zcb */
		len = 1;
		imm = RVC_LB_IMM(insn);
		c_ತುಂಬು = true;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_LH) == INSN_ಹೊಂದಿಕೆ_LH) {
		len = -2;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_C_LH) == INSN_ಹೊಂದಿಕೆ_C_LH) {
		/* Zcb */
		len = -2;
		imm = RVC_LH_IMM(insn);
		c_ತುಂಬು = true;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_LHU) == INSN_ಹೊಂದಿಕೆ_LHU) {
		len = 2;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_C_LHU) == INSN_ಹೊಂದಿಕೆ_C_LHU) {
		/* Zcb */
		len = 2;
		imm = RVC_LH_IMM(insn);
		c_ತುಂಬು = true;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_LW) == INSN_ಹೊಂದಿಕೆ_LW) {
		len = -4;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_C_LW) == INSN_ಹೊಂದಿಕೆ_C_LW) {
		/* Zca */
		len = -4;
		imm = RVC_LW_IMM(insn);
		c_ತುಂಬು = true;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_C_LWSP) == INSN_ಹೊಂದಿಕೆ_C_LWSP &&
		GET_RD_ಅಂಕೆ(insn)) {
		/* Zca */
		len = -4;
		imm = RVC_LWSP_IMM(insn);
		c_ldsp = true;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __riscv_flen
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_FLW) == INSN_ಹೊಂದಿಕೆ_FLW) {
		len = 4;
		fp = true;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_FLD) == INSN_ಹೊಂದಿಕೆ_FLD) {
		len = 8;
		fp = true;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_C_FLD) == INSN_ಹೊಂದಿಕೆ_C_FLD) {
		/* Zcd */
		len = 8;
		imm = RVC_LD_IMM(insn);
		c_ತುಂಬು = true;
		fp = true;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_C_FLDSP) == INSN_ಹೊಂದಿಕೆ_C_FLDSP) {
		/* Zcd */
		len = 8;
		imm = RVC_LDSP_IMM(insn);
		c_ldsp = true;
		fp = true;
#ಆದರೆ_ಅಂತ್ಯ
	} ಇಲ್ಲದಿದ್ದರೆ {
		prev_xlen = sbi_regs_prev_xlen(regs);
	}

	/**
	 * Must distinguish between rv64 and rv32, RVC instructions have
	 * overlapping encoding:
	 *     c.ld in rv64 == c.flw in rv32
	 *     c.ldsp in rv64 == c.flwsp in rv32
	 */
	ಆದರೆ (prev_xlen == 64) {
		/* RV64 Only: lwu, ld, c.ld, c.ldsp  */
		ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_LWU) == INSN_ಹೊಂದಿಕೆ_LWU) {
			len = 4;
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_LD) == INSN_ಹೊಂದಿಕೆ_LD) {
			len = 8;
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_C_LD) == INSN_ಹೊಂದಿಕೆ_C_LD) {
			/* Zca */
			len = 8;
			imm = RVC_LD_IMM(insn);
			c_ತುಂಬು = true;
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_C_LDSP) == INSN_ಹೊಂದಿಕೆ_C_LDSP &&
			GET_RD_ಅಂಕೆ(insn)) {
			/* Zca */
			len = 8;
			imm = RVC_LDSP_IMM(insn);
			c_ldsp = true;
		}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __riscv_flen
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (prev_xlen == 32) {
		/* RV32 Only: c.flw, c.flwsp */
		ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_C_FLW) == INSN_ಹೊಂದಿಕೆ_C_FLW) {
			/* Zcf */
			len = 4;
			imm = RVC_LW_IMM(insn);
			c_ತುಂಬು = true;
			fp = true;
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_C_FLWSP) == INSN_ಹೊಂದಿಕೆ_C_FLWSP) {
			/* Zcf */
			len = 4;
			imm = RVC_LWSP_IMM(insn);
			c_ldsp = true;
			fp = true;
		}
#ಆದರೆ_ಅಂತ್ಯ
	}

	ಆದರೆ (len < 0) {
		len = -len;
		ಸರಿಸು = 8 * (ಗಾತ್ರ(ulong) - len);
	}

	ಆದರೆ (!len) // Unknown instruction
		ಹೋಗು do_emu;

#ಆದರೆ !defined(OPENSBI_DEBUG)
	/**
	 * For misaligned faults. Skip offset calculation unless DEBUG
	 * builds. It helps validating OpenSBI and HW.
	 */
	ಆದರೆ (orig_trap->cause == CAUSE_MISALIGNED_ತುಂಬು)
		ಹೋಗು do_emu;
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (xform)
		/* Transformed insn */
		off = GET_RS1_ಅಂಕೆ(insn);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c_ತುಂಬು)
		/* non SP-based compressed load */
		off = orig_trap->tval - GET_RS1S(insn, regs) - imm;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c_ldsp)
		/* SP-based compressed load */
		off = orig_trap->tval - REG_VAL(2, regs) - imm;
	ಇಲ್ಲದಿದ್ದರೆ
		/* I-type non-compressed load */
		off = orig_trap->tval - GET_RS1(insn, regs) - (ulong)IMM_I(insn);
	/**
	 * Normalize offset, in case the XLEN of unpriv mode is smaller,
	 * and/or pointer masking is in effect
	 */
	off &= (len - 1);

do_emu:
	rc = emu(insn, len, orig_trap->tval - off, &val, tcntx);
	ಆದರೆ (rc <= 0)
		ಹಿಂತಿರುಗಿ rc;
	ಆದರೆ (!len)
		ಹೋಗು epc_fixup;

	ಆದರೆ (!fp) {
		ulong v = ((ದೀರ್ಘ)(val.ದತ್ತಾಂಶ_ulong << ಸರಿಸು)) >> ಸರಿಸು;

		ಆದರೆ (c_ತುಂಬು)
			ಗಣ_RDS(insn, regs, v);
		ಇಲ್ಲದಿದ್ದರೆ
			ಗಣ_RD(insn, regs, v);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __riscv_flen
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (len == 8) {
		ಆದರೆ (c_ತುಂಬು)
			ಗಣ_F64_RDS(insn, regs, val.ದತ್ತಾಂಶ_u64);
		ಇಲ್ಲದಿದ್ದರೆ
			ಗಣ_F64_RD(insn, regs, val.ದತ್ತಾಂಶ_u64);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆದರೆ (c_ತುಂಬು)
			ಗಣ_F32_RDS(insn, regs, val.ದತ್ತಾಂಶ_ulong);
		ಇಲ್ಲದಿದ್ದರೆ
			ಗಣ_F32_RD(insn, regs, val.ದತ್ತಾಂಶ_ulong);
#ಆದರೆ_ಅಂತ್ಯ
	}

epc_fixup:
	regs->mepc += insn_len;

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_trap_emulate_store(ರಚನೆ sbi_trap_context *tcntx,
				  sbi_trap_st_emulator emu)
{
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_trap_info *orig_trap = &tcntx->trap;
	ರಚನೆ sbi_trap_regs *regs = &tcntx->regs;
	ulong insn, insn_len, imm = 0, off = 0;
	ಒಕ್ಕೂಟ sbi_ldst_ದತ್ತಾಂಶ val;
	ರಚನೆ sbi_trap_info uptrap;
	bool xform = false, fp = false, c_store = false, c_stsp = false;
	ಪೂರ್ಣಾಂಕ rc, len = 0, prev_xlen = 0;

	ಆದರೆ (sbi_trap_tinst_valid(orig_trap->tinst)) {
		xform	 = true;
		insn	 = orig_trap->tinst | INSN_16_ಬಿಟ್_ಮುಖವಾಡ;
		insn_len = (orig_trap->tinst & 0x2) ? INSN_LEN(insn) : 2;
	} ಇಲ್ಲದಿದ್ದರೆ {
		/* trapped instruction value is zero or special value */
		insn = sbi_get_insn(regs->mepc, &uptrap);
		ಆದರೆ (uptrap.cause) {
			ಹಿಂತಿರುಗಿ sbi_trap_redirect(regs, &uptrap);
		}
		insn_len = INSN_LEN(insn);
	}

	/**
	 * Common for RV32/RV64:
	 *    sb, sh, sw, fsw, fsd
	 *    c.sb, c.sh, c.sw, c.swsp, c.fsd, c.fsdsp
	 */
	ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_SB) == INSN_ಹೊಂದಿಕೆ_SB) {
		len = 1;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_C_SB) == INSN_ಹೊಂದಿಕೆ_C_SB) {
		/* Zcb */
		len = 1;
		imm = RVC_SB_IMM(insn);
		c_store = true;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_SH) == INSN_ಹೊಂದಿಕೆ_SH) {
		len = 2;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_C_SH) == INSN_ಹೊಂದಿಕೆ_C_SH) {
		/* Zcb */
		len = 2;
		imm = RVC_SH_IMM(insn);
		c_store = true;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_SW) == INSN_ಹೊಂದಿಕೆ_SW) {
		len = 4;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_C_SW) == INSN_ಹೊಂದಿಕೆ_C_SW) {
		/* Zca */
		len = 4;
		imm = RVC_SW_IMM(insn);
		c_store = true;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_C_SWSP) == INSN_ಹೊಂದಿಕೆ_C_SWSP) {
		/* Zca */
		len = 4;
		imm = RVC_SWSP_IMM(insn);
		c_stsp = true;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __riscv_flen
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_FSW) == INSN_ಹೊಂದಿಕೆ_FSW) {
		len = 4;
		fp = true;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_FSD) == INSN_ಹೊಂದಿಕೆ_FSD) {
		len = 8;
		fp = true;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_C_FSD) == INSN_ಹೊಂದಿಕೆ_C_FSD) {
		/* Zcd */
		len = 8;
		imm = RVC_SD_IMM(insn);
		c_store = true;
		fp = true;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_C_FSDSP) == INSN_ಹೊಂದಿಕೆ_C_FSDSP) {
		/* Zcd */
		len = 8;
		imm = RVC_SDSP_IMM(insn);
		c_stsp = true;
		fp = true;
#ಆದರೆ_ಅಂತ್ಯ
	} ಇಲ್ಲದಿದ್ದರೆ {
		prev_xlen = sbi_regs_prev_xlen(regs);
	}

	/**
	 * Must distinguish between rv64 and rv32, RVC instructions have
	 * overlapping encoding:
	 *     c.sd in rv64 == c.fsw in rv32
	 *     c.sdsp in rv64 == c.fswsp in rv32
	 */
	ಆದರೆ (prev_xlen == 64) {
		/* RV64 Only: sd, c.sd, c.sdsp */
		ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_SD) == INSN_ಹೊಂದಿಕೆ_SD) {
			len = 8;
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_C_SD) == INSN_ಹೊಂದಿಕೆ_C_SD) {
			/* Zca */
			len = 8;
			imm = RVC_SD_IMM(insn);
			c_store = true;
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_C_SDSP) == INSN_ಹೊಂದಿಕೆ_C_SDSP) {
			/* Zca */
			len = 8;
			imm = RVC_SDSP_IMM(insn);
			c_stsp = true;
		}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __riscv_flen
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (prev_xlen == 32) {
		/* RV32 Only: c.fsw, c.fswsp */
		ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_C_FSW) == INSN_ಹೊಂದಿಕೆ_C_FSW) {
			/* Zcf */
			len = 4;
			imm = RVC_SW_IMM(insn);
			c_store = true;
			fp = true;
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((insn & INSN_ಮುಖವಾಡ_C_FSWSP) == INSN_ಹೊಂದಿಕೆ_C_FSWSP) {
			/* Zcf */
			len = 4;
			imm = RVC_SWSP_IMM(insn);
			c_stsp = true;
			fp = true;
		}
#ಆದರೆ_ಅಂತ್ಯ
	}

	ಆದರೆ (!fp) {
		ಆದರೆ (c_store)
			val.ದತ್ತಾಂಶ_ulong = GET_RS2S(insn, regs);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c_stsp)
			val.ದತ್ತಾಂಶ_ulong = GET_RS2C(insn, regs);
		ಇಲ್ಲದಿದ್ದರೆ
			val.ದತ್ತಾಂಶ_ulong = GET_RS2(insn, regs);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __riscv_flen
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (len == 8) {
		ಆದರೆ (c_store)
			val.ದತ್ತಾಂಶ_u64 = GET_F64_RS2S(insn, regs);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c_stsp)
			val.ದತ್ತಾಂಶ_u64 = GET_F64_RS2C(insn, regs);
		ಇಲ್ಲದಿದ್ದರೆ
			val.ದತ್ತಾಂಶ_u64 = GET_F64_RS2(insn, regs);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆದರೆ (c_store)
			val.ದತ್ತಾಂಶ_ulong = GET_F32_RS2S(insn, regs);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c_stsp)
			val.ದತ್ತಾಂಶ_ulong = GET_F32_RS2C(insn, regs);
		ಇಲ್ಲದಿದ್ದರೆ
			val.ದತ್ತಾಂಶ_ulong = GET_F32_RS2(insn, regs);
#ಆದರೆ_ಅಂತ್ಯ
	}

	ಆದರೆ (!len) // Unknown instruction
		ಹೋಗು do_emu;

#ಆದರೆ !defined(OPENSBI_DEBUG)
	/**
	 * For misaligned faults. Skip offset calculation unless DEBUG
	 * builds. It helps validating OpenSBI and HW.
	 */
	ಆದರೆ (orig_trap->cause == CAUSE_MISALIGNED_STORE)
		ಹೋಗು do_emu;
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (xform)
		/* Transformed insn */
		off = GET_RS1_ಅಂಕೆ(insn);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c_store)
		/* non SP-based compressed store */
		off = orig_trap->tval - GET_RS1S(insn, regs) - imm;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c_stsp)
		/* SP-based compressed store */
		off = orig_trap->tval - REG_VAL(2, regs) - imm;
	ಇಲ್ಲದಿದ್ದರೆ
		/* S-type non-compressed store */
		off = orig_trap->tval - GET_RS1(insn, regs) - (ulong)IMM_S(insn);
	/**
	 * Normalize offset, in case the XLEN of unpriv mode is smaller,
	 * and/or pointer masking is in effect
	 */
	off &= (len - 1);

do_emu:
	rc = emu(insn, len, orig_trap->tval - off, val, tcntx);
	ಆದರೆ (rc <= 0)
		ಹಿಂತಿರುಗಿ rc;

	regs->mepc += insn_len;

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_misaligned_ld_emulator(ulong insn, ಪೂರ್ಣಾಂಕ rlen, ulong addr,
				      ಒಕ್ಕೂಟ sbi_ldst_ದತ್ತಾಂಶ *ಹೊರಹೋಗು_val,
				      ರಚನೆ sbi_trap_context *tcntx)
{
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_trap_info *orig_trap = &tcntx->trap;
	ರಚನೆ sbi_trap_regs *regs = &tcntx->regs;
	ರಚನೆ sbi_trap_info uptrap;

	ಆದರೆ (!rlen) {
		ಆದರೆ (IS_ಸದಿಶ_ತುಂಬು_STORE(insn))
			ಹಿಂತಿರುಗಿ sbi_misaligned_v_ld_emulator(insn, tcntx);
		ಇಲ್ಲದಿದ್ದರೆ
			/* Unrecognized instruction. Can't emulate it. */
			ಹಿಂತಿರುಗಿ sbi_trap_redirect(regs, orig_trap);
	}
	/* For misaligned fault, addr must be the same as orig_trap->tval */
	ಆದರೆ (addr != orig_trap->tval)
		ಹಿಂತಿರುಗಿ SBI_EFAIL;

	sbi_ತುಂಬು_loop(ಹೊರಹೋಗು_val->ದತ್ತಾಂಶ_ಬೈಟುಗಳು, addr, rlen, &uptrap);
	ಆದರೆ (uptrap.cause) {
		sbi_misaligned_tinst_fixup(orig_trap, &uptrap);
		ಹಿಂತಿರುಗಿ sbi_trap_redirect(regs, &uptrap);
	}
	ಹಿಂತಿರುಗಿ rlen;
}

ಪೂರ್ಣಾಂಕ sbi_misaligned_ತುಂಬು_handler(ರಚನೆ sbi_trap_context *tcntx)
{
	ಹಿಂತಿರುಗಿ sbi_trap_emulate_ತುಂಬು(tcntx, sbi_misaligned_ld_emulator);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_misaligned_st_emulator(ulong insn, ಪೂರ್ಣಾಂಕ wlen, ulong addr,
				      ಒಕ್ಕೂಟ sbi_ldst_ದತ್ತಾಂಶ ಒಳ_val,
				      ರಚನೆ sbi_trap_context *tcntx)
{
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_trap_info *orig_trap = &tcntx->trap;
	ರಚನೆ sbi_trap_regs *regs = &tcntx->regs;
	ರಚನೆ sbi_trap_info uptrap;

	ಆದರೆ (!wlen) {
		ಆದರೆ (IS_ಸದಿಶ_ತುಂಬು_STORE(insn))
			ಹಿಂತಿರುಗಿ sbi_misaligned_v_st_emulator(insn, tcntx);
		ಇಲ್ಲದಿದ್ದರೆ
			/* Unrecognized instruction. Can't emulate it. */
			ಹಿಂತಿರುಗಿ sbi_trap_redirect(regs, orig_trap);
	}
	/* For misaligned fault, addr must be the same as orig_trap->tval */
	ಆದರೆ (addr != orig_trap->tval)
		ಹಿಂತಿರುಗಿ SBI_EFAIL;

	sbi_store_loop(ಒಳ_val.ದತ್ತಾಂಶ_ಬೈಟುಗಳು, addr, wlen, &uptrap);
	ಆದರೆ (uptrap.cause) {
		sbi_misaligned_tinst_fixup(orig_trap, &uptrap);
		ಹಿಂತಿರುಗಿ sbi_trap_redirect(regs, &uptrap);
	}
	ಹಿಂತಿರುಗಿ wlen;
}

ಪೂರ್ಣಾಂಕ sbi_misaligned_store_handler(ರಚನೆ sbi_trap_context *tcntx)
{
	ಹಿಂತಿರುಗಿ sbi_trap_emulate_store(tcntx, sbi_misaligned_st_emulator);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ld_access_emulator(ulong insn, ಪೂರ್ಣಾಂಕ rlen, ulong addr,
				  ಒಕ್ಕೂಟ sbi_ldst_ದತ್ತಾಂಶ *ಹೊರಹೋಗು_val,
				  ರಚನೆ sbi_trap_context *tcntx)
{
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_trap_info *orig_trap = &tcntx->trap;
	ರಚನೆ sbi_trap_regs *regs = &tcntx->regs;
	ಪೂರ್ಣಾಂಕ rc;

	/* If fault came from M mode, just fail */
	ಆದರೆ (sbi_mstatus_prev_ಬಹುಲಕ(regs->mstatus) == PRV_M)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	rc = sbi_platform_emulate_ತುಂಬು(sbi_platform_thishart_ptr(),
				       insn, rlen, addr, ಹೊರಹೋಗು_val, tcntx);

	/* If platform emulator failed, we redirect instead of fail */
	ಆದರೆ (rc < 0)
		ಹಿಂತಿರುಗಿ sbi_trap_redirect(regs, orig_trap);

	ಹಿಂತಿರುಗಿ rc;
}

ಪೂರ್ಣಾಂಕ sbi_ತುಂಬು_access_handler(ರಚನೆ sbi_trap_context *tcntx)
{
	ಹಿಂತಿರುಗಿ sbi_trap_emulate_ತುಂಬು(tcntx, sbi_ld_access_emulator);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_st_access_emulator(ulong insn, ಪೂರ್ಣಾಂಕ wlen, ulong addr,
				  ಒಕ್ಕೂಟ sbi_ldst_ದತ್ತಾಂಶ ಒಳ_val,
				  ರಚನೆ sbi_trap_context *tcntx)
{
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_trap_info *orig_trap = &tcntx->trap;
	ರಚನೆ sbi_trap_regs *regs = &tcntx->regs;
	ಪೂರ್ಣಾಂಕ rc;

	/* If fault came from M mode, just fail */
	ಆದರೆ (sbi_mstatus_prev_ಬಹುಲಕ(regs->mstatus) == PRV_M)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	rc = sbi_platform_emulate_store(sbi_platform_thishart_ptr(),
					insn, wlen, addr, ಒಳ_val, tcntx);

	/* If platform emulator failed, we redirect instead of fail */
	ಆದರೆ (rc < 0)
		ಹಿಂತಿರುಗಿ sbi_trap_redirect(regs, orig_trap);

	ಹಿಂತಿರುಗಿ rc;
}

ಪೂರ್ಣಾಂಕ sbi_store_access_handler(ರಚನೆ sbi_trap_context *tcntx)
{
	ಹಿಂತಿರುಗಿ sbi_trap_emulate_store(tcntx, sbi_st_access_emulator);
}
