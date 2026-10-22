/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_encoding.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_fp.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap_ldst.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_unpriv.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_platform.h>

/**
 * Load emulator callback:
 *   Refer to comments of `sbi_platform_emulate_load`.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*sbi_trap_ld_emulator)(ulong insn, ਪੂਰਨ rlen, ulong raddr,
				    ਸੰਘ sbi_ldst_ਅੰਕੜੇ *ਬਾਹਰਲਾ_val,
				    ਬਣਤਰ sbi_trap_context *tcntx);

/**
 * Store emulator callback:
 *   Refer to comments of `sbi_platform_emulate_store`.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*sbi_trap_st_emulator)(ulong insn, ਪੂਰਨ wlen, ulong waddr,
				    ਸੰਘ sbi_ldst_ਅੰਕੜੇ ਵਿੱਚ_val,
				    ਬਣਤਰ sbi_trap_context *tcntx);

/**
 * Handling of misaligned fault is done by a collection of smaller, but
 * aligned load/store(s). Another fault (load/store, page fault...) can
 * arise from any of them, then the handling gets aborted. We must fixup
 * the tinst to pretend the fault was rised from the original insn.
 * Specifically, fixup the offset field using the tval diff between the
 * new trap and the original one (if required).
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ sbi_misaligned_tinst_fixup(
			ਅਚਲ ਬਣਤਰ sbi_trap_info *orig_trap,
			      ਬਣਤਰ sbi_trap_info *uptrap)
{
	ulong ਵਿੱਥ = uptrap->tval - orig_trap->tval;

	/*
	 * The function is called in code path for handling a scalar
	 * load/store misaligned fault, thus the new uptrap can't have
	 * custom value of tinst
	 */
	ਜੇ (uptrap->tinst == INSN_PSEUDO_VS_ਲੱਦੋ ||
	    uptrap->tinst == INSN_PSEUDO_VS_STORE)
		/* Use uptrap as-is for guest-page faults */
		ਵਾਪਸ;
	/*
	 * Only fixup if orig tinst is valid. Otherwise, discard the
	 * new tinst to be on the safe side. Never use new tinst as-is!
	 * It's load/store width surely mismatches the original width.
	 * For vector, discard it regardless. It doesn't make sense to
	 * have a transformed tinst
	 */
	ਨਹੀਂਤਾਂ ਜੇ (orig_trap->tinst == 0)
		uptrap->tinst = 0;
	ਨਹੀਂਤਾਂ
		uptrap->tinst = orig_trap->tinst | (ਵਿੱਥ << SH_RS1);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool sbi_trap_tinst_valid(ulong tinst)
{
	/*
	 * Bit[0] == 1 implies trapped instruction value is
	 * transformed instruction or custom instruction.
	 * Also do proper checking per Privileged ISA 19.6.3,
	 * and make sure high 32 bits of tinst is 0
	 */
	ਵਾਪਸ tinst == (uint32_t)tinst && (tinst & 0x1);
}

ਸਥਿਰ ਪੂਰਨ sbi_trap_emulate_ਲੱਦੋ(ਬਣਤਰ sbi_trap_context *tcntx,
				 sbi_trap_ld_emulator emu)
{
	ਅਚਲ ਬਣਤਰ sbi_trap_info *orig_trap = &tcntx->trap;
	ਬਣਤਰ sbi_trap_regs *regs = &tcntx->regs;
	ulong insn, insn_len, imm = 0, ਖਿਸਕਾਓ = 0, off = 0;
	ਸੰਘ sbi_ldst_ਅੰਕੜੇ val = { 0 };
	ਬਣਤਰ sbi_trap_info uptrap;
	bool xform = false, fp = false, c_ਲੱਦੋ = false, c_ldsp = false;
	ਪੂਰਨ rc, len = 0, prev_xlen = 0;

	ਜੇ (sbi_trap_tinst_valid(orig_trap->tinst)) {
		xform	 = true;
		insn	 = orig_trap->tinst | INSN_16_ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ;
		insn_len = (orig_trap->tinst & 0x2) ? INSN_LEN(insn) : 2;
	} ਨਹੀਂਤਾਂ {
		/* trapped instruction value is zero or special value */
		insn = sbi_get_insn(regs->mepc, &uptrap);
		ਜੇ (uptrap.cause) {
			ਵਾਪਸ sbi_trap_redirect(regs, &uptrap);
		}
		insn_len = INSN_LEN(insn);
	}

	/**
	 * Common for RV32/RV64:
	 *    lb, lbu, lh, lhu, lw, flw, flw
	 *    c.lbu, c.lh, c.lhu, c.lw, c.lwsp, c.fld, c.fldsp
	 */
	ਜੇ ((insn & INSN_ਨਕਾਬ_LB) == INSN_ਮੇਲ_LB) {
		len = -1;
	} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_LBU) == INSN_ਮੇਲ_LBU) {
		len = 1;
	} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_C_LBU) == INSN_ਮੇਲ_C_LBU) {
		/* Zcb */
		len = 1;
		imm = RVC_LB_IMM(insn);
		c_ਲੱਦੋ = true;
	} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_LH) == INSN_ਮੇਲ_LH) {
		len = -2;
	} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_C_LH) == INSN_ਮੇਲ_C_LH) {
		/* Zcb */
		len = -2;
		imm = RVC_LH_IMM(insn);
		c_ਲੱਦੋ = true;
	} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_LHU) == INSN_ਮੇਲ_LHU) {
		len = 2;
	} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_C_LHU) == INSN_ਮੇਲ_C_LHU) {
		/* Zcb */
		len = 2;
		imm = RVC_LH_IMM(insn);
		c_ਲੱਦੋ = true;
	} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_LW) == INSN_ਮੇਲ_LW) {
		len = -4;
	} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_C_LW) == INSN_ਮੇਲ_C_LW) {
		/* Zca */
		len = -4;
		imm = RVC_LW_IMM(insn);
		c_ਲੱਦੋ = true;
	} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_C_LWSP) == INSN_ਮੇਲ_C_LWSP &&
		GET_RD_ਅੰਕ(insn)) {
		/* Zca */
		len = -4;
		imm = RVC_LWSP_IMM(insn);
		c_ldsp = true;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __riscv_flen
	} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_FLW) == INSN_ਮੇਲ_FLW) {
		len = 4;
		fp = true;
	} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_FLD) == INSN_ਮੇਲ_FLD) {
		len = 8;
		fp = true;
	} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_C_FLD) == INSN_ਮੇਲ_C_FLD) {
		/* Zcd */
		len = 8;
		imm = RVC_LD_IMM(insn);
		c_ਲੱਦੋ = true;
		fp = true;
	} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_C_FLDSP) == INSN_ਮੇਲ_C_FLDSP) {
		/* Zcd */
		len = 8;
		imm = RVC_LDSP_IMM(insn);
		c_ldsp = true;
		fp = true;
#ਜੇ_ਅੰਤ
	} ਨਹੀਂਤਾਂ {
		prev_xlen = sbi_regs_prev_xlen(regs);
	}

	/**
	 * Must distinguish between rv64 and rv32, RVC instructions have
	 * overlapping encoding:
	 *     c.ld in rv64 == c.flw in rv32
	 *     c.ldsp in rv64 == c.flwsp in rv32
	 */
	ਜੇ (prev_xlen == 64) {
		/* RV64 Only: lwu, ld, c.ld, c.ldsp  */
		ਜੇ ((insn & INSN_ਨਕਾਬ_LWU) == INSN_ਮੇਲ_LWU) {
			len = 4;
		} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_LD) == INSN_ਮੇਲ_LD) {
			len = 8;
		} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_C_LD) == INSN_ਮੇਲ_C_LD) {
			/* Zca */
			len = 8;
			imm = RVC_LD_IMM(insn);
			c_ਲੱਦੋ = true;
		} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_C_LDSP) == INSN_ਮੇਲ_C_LDSP &&
			GET_RD_ਅੰਕ(insn)) {
			/* Zca */
			len = 8;
			imm = RVC_LDSP_IMM(insn);
			c_ldsp = true;
		}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __riscv_flen
	} ਨਹੀਂਤਾਂ ਜੇ (prev_xlen == 32) {
		/* RV32 Only: c.flw, c.flwsp */
		ਜੇ ((insn & INSN_ਨਕਾਬ_C_FLW) == INSN_ਮੇਲ_C_FLW) {
			/* Zcf */
			len = 4;
			imm = RVC_LW_IMM(insn);
			c_ਲੱਦੋ = true;
			fp = true;
		} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_C_FLWSP) == INSN_ਮੇਲ_C_FLWSP) {
			/* Zcf */
			len = 4;
			imm = RVC_LWSP_IMM(insn);
			c_ldsp = true;
			fp = true;
		}
#ਜੇ_ਅੰਤ
	}

	ਜੇ (len < 0) {
		len = -len;
		ਖਿਸਕਾਓ = 8 * (ਆਕਾਰ(ulong) - len);
	}

	ਜੇ (!len) // Unknown instruction
		ਜਾਓ do_emu;

#ਜੇ !defined(OPENSBI_DEBUG)
	/**
	 * For misaligned faults. Skip offset calculation unless DEBUG
	 * builds. It helps validating OpenSBI and HW.
	 */
	ਜੇ (orig_trap->cause == CAUSE_MISALIGNED_ਲੱਦੋ)
		ਜਾਓ do_emu;
#ਜੇ_ਅੰਤ

	ਜੇ (xform)
		/* Transformed insn */
		off = GET_RS1_ਅੰਕ(insn);
	ਨਹੀਂਤਾਂ ਜੇ (c_ਲੱਦੋ)
		/* non SP-based compressed load */
		off = orig_trap->tval - GET_RS1S(insn, regs) - imm;
	ਨਹੀਂਤਾਂ ਜੇ (c_ldsp)
		/* SP-based compressed load */
		off = orig_trap->tval - REG_VAL(2, regs) - imm;
	ਨਹੀਂਤਾਂ
		/* I-type non-compressed load */
		off = orig_trap->tval - GET_RS1(insn, regs) - (ulong)IMM_I(insn);
	/**
	 * Normalize offset, in case the XLEN of unpriv mode is smaller,
	 * and/or pointer masking is in effect
	 */
	off &= (len - 1);

do_emu:
	rc = emu(insn, len, orig_trap->tval - off, &val, tcntx);
	ਜੇ (rc <= 0)
		ਵਾਪਸ rc;
	ਜੇ (!len)
		ਜਾਓ epc_fixup;

	ਜੇ (!fp) {
		ulong v = ((ਲੰਮਾ)(val.ਅੰਕੜੇ_ulong << ਖਿਸਕਾਓ)) >> ਖਿਸਕਾਓ;

		ਜੇ (c_ਲੱਦੋ)
			ਸੈੱਟ_RDS(insn, regs, v);
		ਨਹੀਂਤਾਂ
			ਸੈੱਟ_RD(insn, regs, v);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __riscv_flen
	} ਨਹੀਂਤਾਂ ਜੇ (len == 8) {
		ਜੇ (c_ਲੱਦੋ)
			ਸੈੱਟ_F64_RDS(insn, regs, val.ਅੰਕੜੇ_u64);
		ਨਹੀਂਤਾਂ
			ਸੈੱਟ_F64_RD(insn, regs, val.ਅੰਕੜੇ_u64);
	} ਨਹੀਂਤਾਂ {
		ਜੇ (c_ਲੱਦੋ)
			ਸੈੱਟ_F32_RDS(insn, regs, val.ਅੰਕੜੇ_ulong);
		ਨਹੀਂਤਾਂ
			ਸੈੱਟ_F32_RD(insn, regs, val.ਅੰਕੜੇ_ulong);
#ਜੇ_ਅੰਤ
	}

epc_fixup:
	regs->mepc += insn_len;

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ sbi_trap_emulate_store(ਬਣਤਰ sbi_trap_context *tcntx,
				  sbi_trap_st_emulator emu)
{
	ਅਚਲ ਬਣਤਰ sbi_trap_info *orig_trap = &tcntx->trap;
	ਬਣਤਰ sbi_trap_regs *regs = &tcntx->regs;
	ulong insn, insn_len, imm = 0, off = 0;
	ਸੰਘ sbi_ldst_ਅੰਕੜੇ val;
	ਬਣਤਰ sbi_trap_info uptrap;
	bool xform = false, fp = false, c_store = false, c_stsp = false;
	ਪੂਰਨ rc, len = 0, prev_xlen = 0;

	ਜੇ (sbi_trap_tinst_valid(orig_trap->tinst)) {
		xform	 = true;
		insn	 = orig_trap->tinst | INSN_16_ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ;
		insn_len = (orig_trap->tinst & 0x2) ? INSN_LEN(insn) : 2;
	} ਨਹੀਂਤਾਂ {
		/* trapped instruction value is zero or special value */
		insn = sbi_get_insn(regs->mepc, &uptrap);
		ਜੇ (uptrap.cause) {
			ਵਾਪਸ sbi_trap_redirect(regs, &uptrap);
		}
		insn_len = INSN_LEN(insn);
	}

	/**
	 * Common for RV32/RV64:
	 *    sb, sh, sw, fsw, fsd
	 *    c.sb, c.sh, c.sw, c.swsp, c.fsd, c.fsdsp
	 */
	ਜੇ ((insn & INSN_ਨਕਾਬ_SB) == INSN_ਮੇਲ_SB) {
		len = 1;
	} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_C_SB) == INSN_ਮੇਲ_C_SB) {
		/* Zcb */
		len = 1;
		imm = RVC_SB_IMM(insn);
		c_store = true;
	} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_SH) == INSN_ਮੇਲ_SH) {
		len = 2;
	} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_C_SH) == INSN_ਮੇਲ_C_SH) {
		/* Zcb */
		len = 2;
		imm = RVC_SH_IMM(insn);
		c_store = true;
	} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_SW) == INSN_ਮੇਲ_SW) {
		len = 4;
	} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_C_SW) == INSN_ਮੇਲ_C_SW) {
		/* Zca */
		len = 4;
		imm = RVC_SW_IMM(insn);
		c_store = true;
	} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_C_SWSP) == INSN_ਮੇਲ_C_SWSP) {
		/* Zca */
		len = 4;
		imm = RVC_SWSP_IMM(insn);
		c_stsp = true;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __riscv_flen
	} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_FSW) == INSN_ਮੇਲ_FSW) {
		len = 4;
		fp = true;
	} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_FSD) == INSN_ਮੇਲ_FSD) {
		len = 8;
		fp = true;
	} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_C_FSD) == INSN_ਮੇਲ_C_FSD) {
		/* Zcd */
		len = 8;
		imm = RVC_SD_IMM(insn);
		c_store = true;
		fp = true;
	} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_C_FSDSP) == INSN_ਮੇਲ_C_FSDSP) {
		/* Zcd */
		len = 8;
		imm = RVC_SDSP_IMM(insn);
		c_stsp = true;
		fp = true;
#ਜੇ_ਅੰਤ
	} ਨਹੀਂਤਾਂ {
		prev_xlen = sbi_regs_prev_xlen(regs);
	}

	/**
	 * Must distinguish between rv64 and rv32, RVC instructions have
	 * overlapping encoding:
	 *     c.sd in rv64 == c.fsw in rv32
	 *     c.sdsp in rv64 == c.fswsp in rv32
	 */
	ਜੇ (prev_xlen == 64) {
		/* RV64 Only: sd, c.sd, c.sdsp */
		ਜੇ ((insn & INSN_ਨਕਾਬ_SD) == INSN_ਮੇਲ_SD) {
			len = 8;
		} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_C_SD) == INSN_ਮੇਲ_C_SD) {
			/* Zca */
			len = 8;
			imm = RVC_SD_IMM(insn);
			c_store = true;
		} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_C_SDSP) == INSN_ਮੇਲ_C_SDSP) {
			/* Zca */
			len = 8;
			imm = RVC_SDSP_IMM(insn);
			c_stsp = true;
		}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __riscv_flen
	} ਨਹੀਂਤਾਂ ਜੇ (prev_xlen == 32) {
		/* RV32 Only: c.fsw, c.fswsp */
		ਜੇ ((insn & INSN_ਨਕਾਬ_C_FSW) == INSN_ਮੇਲ_C_FSW) {
			/* Zcf */
			len = 4;
			imm = RVC_SW_IMM(insn);
			c_store = true;
			fp = true;
		} ਨਹੀਂਤਾਂ ਜੇ ((insn & INSN_ਨਕਾਬ_C_FSWSP) == INSN_ਮੇਲ_C_FSWSP) {
			/* Zcf */
			len = 4;
			imm = RVC_SWSP_IMM(insn);
			c_stsp = true;
			fp = true;
		}
#ਜੇ_ਅੰਤ
	}

	ਜੇ (!fp) {
		ਜੇ (c_store)
			val.ਅੰਕੜੇ_ulong = GET_RS2S(insn, regs);
		ਨਹੀਂਤਾਂ ਜੇ (c_stsp)
			val.ਅੰਕੜੇ_ulong = GET_RS2C(insn, regs);
		ਨਹੀਂਤਾਂ
			val.ਅੰਕੜੇ_ulong = GET_RS2(insn, regs);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __riscv_flen
	} ਨਹੀਂਤਾਂ ਜੇ (len == 8) {
		ਜੇ (c_store)
			val.ਅੰਕੜੇ_u64 = GET_F64_RS2S(insn, regs);
		ਨਹੀਂਤਾਂ ਜੇ (c_stsp)
			val.ਅੰਕੜੇ_u64 = GET_F64_RS2C(insn, regs);
		ਨਹੀਂਤਾਂ
			val.ਅੰਕੜੇ_u64 = GET_F64_RS2(insn, regs);
	} ਨਹੀਂਤਾਂ {
		ਜੇ (c_store)
			val.ਅੰਕੜੇ_ulong = GET_F32_RS2S(insn, regs);
		ਨਹੀਂਤਾਂ ਜੇ (c_stsp)
			val.ਅੰਕੜੇ_ulong = GET_F32_RS2C(insn, regs);
		ਨਹੀਂਤਾਂ
			val.ਅੰਕੜੇ_ulong = GET_F32_RS2(insn, regs);
#ਜੇ_ਅੰਤ
	}

	ਜੇ (!len) // Unknown instruction
		ਜਾਓ do_emu;

#ਜੇ !defined(OPENSBI_DEBUG)
	/**
	 * For misaligned faults. Skip offset calculation unless DEBUG
	 * builds. It helps validating OpenSBI and HW.
	 */
	ਜੇ (orig_trap->cause == CAUSE_MISALIGNED_STORE)
		ਜਾਓ do_emu;
#ਜੇ_ਅੰਤ

	ਜੇ (xform)
		/* Transformed insn */
		off = GET_RS1_ਅੰਕ(insn);
	ਨਹੀਂਤਾਂ ਜੇ (c_store)
		/* non SP-based compressed store */
		off = orig_trap->tval - GET_RS1S(insn, regs) - imm;
	ਨਹੀਂਤਾਂ ਜੇ (c_stsp)
		/* SP-based compressed store */
		off = orig_trap->tval - REG_VAL(2, regs) - imm;
	ਨਹੀਂਤਾਂ
		/* S-type non-compressed store */
		off = orig_trap->tval - GET_RS1(insn, regs) - (ulong)IMM_S(insn);
	/**
	 * Normalize offset, in case the XLEN of unpriv mode is smaller,
	 * and/or pointer masking is in effect
	 */
	off &= (len - 1);

do_emu:
	rc = emu(insn, len, orig_trap->tval - off, val, tcntx);
	ਜੇ (rc <= 0)
		ਵਾਪਸ rc;

	regs->mepc += insn_len;

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ sbi_misaligned_ld_emulator(ulong insn, ਪੂਰਨ rlen, ulong addr,
				      ਸੰਘ sbi_ldst_ਅੰਕੜੇ *ਬਾਹਰਲਾ_val,
				      ਬਣਤਰ sbi_trap_context *tcntx)
{
	ਅਚਲ ਬਣਤਰ sbi_trap_info *orig_trap = &tcntx->trap;
	ਬਣਤਰ sbi_trap_regs *regs = &tcntx->regs;
	ਬਣਤਰ sbi_trap_info uptrap;

	ਜੇ (!rlen) {
		ਜੇ (IS_ਸਦਿਸ਼_ਲੱਦੋ_STORE(insn))
			ਵਾਪਸ sbi_misaligned_v_ld_emulator(insn, tcntx);
		ਨਹੀਂਤਾਂ
			/* Unrecognized instruction. Can't emulate it. */
			ਵਾਪਸ sbi_trap_redirect(regs, orig_trap);
	}
	/* For misaligned fault, addr must be the same as orig_trap->tval */
	ਜੇ (addr != orig_trap->tval)
		ਵਾਪਸ SBI_EFAIL;

	sbi_ਲੱਦੋ_loop(ਬਾਹਰਲਾ_val->ਅੰਕੜੇ_ਬਾਈਟਾਂ, addr, rlen, &uptrap);
	ਜੇ (uptrap.cause) {
		sbi_misaligned_tinst_fixup(orig_trap, &uptrap);
		ਵਾਪਸ sbi_trap_redirect(regs, &uptrap);
	}
	ਵਾਪਸ rlen;
}

ਪੂਰਨ sbi_misaligned_ਲੱਦੋ_handler(ਬਣਤਰ sbi_trap_context *tcntx)
{
	ਵਾਪਸ sbi_trap_emulate_ਲੱਦੋ(tcntx, sbi_misaligned_ld_emulator);
}

ਸਥਿਰ ਪੂਰਨ sbi_misaligned_st_emulator(ulong insn, ਪੂਰਨ wlen, ulong addr,
				      ਸੰਘ sbi_ldst_ਅੰਕੜੇ ਵਿੱਚ_val,
				      ਬਣਤਰ sbi_trap_context *tcntx)
{
	ਅਚਲ ਬਣਤਰ sbi_trap_info *orig_trap = &tcntx->trap;
	ਬਣਤਰ sbi_trap_regs *regs = &tcntx->regs;
	ਬਣਤਰ sbi_trap_info uptrap;

	ਜੇ (!wlen) {
		ਜੇ (IS_ਸਦਿਸ਼_ਲੱਦੋ_STORE(insn))
			ਵਾਪਸ sbi_misaligned_v_st_emulator(insn, tcntx);
		ਨਹੀਂਤਾਂ
			/* Unrecognized instruction. Can't emulate it. */
			ਵਾਪਸ sbi_trap_redirect(regs, orig_trap);
	}
	/* For misaligned fault, addr must be the same as orig_trap->tval */
	ਜੇ (addr != orig_trap->tval)
		ਵਾਪਸ SBI_EFAIL;

	sbi_store_loop(ਵਿੱਚ_val.ਅੰਕੜੇ_ਬਾਈਟਾਂ, addr, wlen, &uptrap);
	ਜੇ (uptrap.cause) {
		sbi_misaligned_tinst_fixup(orig_trap, &uptrap);
		ਵਾਪਸ sbi_trap_redirect(regs, &uptrap);
	}
	ਵਾਪਸ wlen;
}

ਪੂਰਨ sbi_misaligned_store_handler(ਬਣਤਰ sbi_trap_context *tcntx)
{
	ਵਾਪਸ sbi_trap_emulate_store(tcntx, sbi_misaligned_st_emulator);
}

ਸਥਿਰ ਪੂਰਨ sbi_ld_access_emulator(ulong insn, ਪੂਰਨ rlen, ulong addr,
				  ਸੰਘ sbi_ldst_ਅੰਕੜੇ *ਬਾਹਰਲਾ_val,
				  ਬਣਤਰ sbi_trap_context *tcntx)
{
	ਅਚਲ ਬਣਤਰ sbi_trap_info *orig_trap = &tcntx->trap;
	ਬਣਤਰ sbi_trap_regs *regs = &tcntx->regs;
	ਪੂਰਨ rc;

	/* If fault came from M mode, just fail */
	ਜੇ (sbi_mstatus_prev_ਬਹੁਲਕ(regs->mstatus) == PRV_M)
		ਵਾਪਸ SBI_EINVAL;

	rc = sbi_platform_emulate_ਲੱਦੋ(sbi_platform_thishart_ptr(),
				       insn, rlen, addr, ਬਾਹਰਲਾ_val, tcntx);

	/* If platform emulator failed, we redirect instead of fail */
	ਜੇ (rc < 0)
		ਵਾਪਸ sbi_trap_redirect(regs, orig_trap);

	ਵਾਪਸ rc;
}

ਪੂਰਨ sbi_ਲੱਦੋ_access_handler(ਬਣਤਰ sbi_trap_context *tcntx)
{
	ਵਾਪਸ sbi_trap_emulate_ਲੱਦੋ(tcntx, sbi_ld_access_emulator);
}

ਸਥਿਰ ਪੂਰਨ sbi_st_access_emulator(ulong insn, ਪੂਰਨ wlen, ulong addr,
				  ਸੰਘ sbi_ldst_ਅੰਕੜੇ ਵਿੱਚ_val,
				  ਬਣਤਰ sbi_trap_context *tcntx)
{
	ਅਚਲ ਬਣਤਰ sbi_trap_info *orig_trap = &tcntx->trap;
	ਬਣਤਰ sbi_trap_regs *regs = &tcntx->regs;
	ਪੂਰਨ rc;

	/* If fault came from M mode, just fail */
	ਜੇ (sbi_mstatus_prev_ਬਹੁਲਕ(regs->mstatus) == PRV_M)
		ਵਾਪਸ SBI_EINVAL;

	rc = sbi_platform_emulate_store(sbi_platform_thishart_ptr(),
					insn, wlen, addr, ਵਿੱਚ_val, tcntx);

	/* If platform emulator failed, we redirect instead of fail */
	ਜੇ (rc < 0)
		ਵਾਪਸ sbi_trap_redirect(regs, orig_trap);

	ਵਾਪਸ rc;
}

ਪੂਰਨ sbi_store_access_handler(ਬਣਤਰ sbi_trap_context *tcntx)
{
	ਵਾਪਸ sbi_trap_emulate_store(tcntx, sbi_st_access_emulator);
}
