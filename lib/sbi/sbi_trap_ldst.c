/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_encoding.h>
#ضمن <sbi/riscv_fp.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_trap_ldst.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/sbi_unpriv.h>
#ضمن <sbi/sbi_platform.h>

/**
 * Load emulator callback:
 *   Refer to comments of `sbi_platform_emulate_load`.
 */
تعريفنوع صحيح (*sbi_trap_ld_emulator)(ulong insn, صحيح rlen, ulong raddr,
				    اتحاد sbi_ldst_انکڑے *باہرلا_val,
				    بنية sbi_trap_context *tcntx);

/**
 * Store emulator callback:
 *   Refer to comments of `sbi_platform_emulate_store`.
 */
تعريفنوع صحيح (*sbi_trap_st_emulator)(ulong insn, صحيح wlen, ulong waddr,
				    اتحاد sbi_ldst_انکڑے وچ_val,
				    بنية sbi_trap_context *tcntx);

/**
 * Handling of misaligned fault is done by a collection of smaller, but
 * aligned load/store(s). Another fault (load/store, page fault...) can
 * arise from any of them, then the handling gets aborted. We must fixup
 * the tinst to pretend the fault was rised from the original insn.
 * Specifically, fixup the offset field using the tval diff between the
 * new trap and the original one (if required).
 */
ساكن مضمن فراغ sbi_misaligned_tinst_fixup(
			ثابت بنية sbi_trap_info *orig_trap,
			      بنية sbi_trap_info *uptrap)
{
	ulong وتھ = uptrap->tval - orig_trap->tval;

	/*
	 * The function is called in code path for handling a scalar
	 * load/store misaligned fault, thus the new uptrap can't have
	 * custom value of tinst
	 */
	إذا (uptrap->tinst == INSN_PSEUDO_VS_لدو ||
	    uptrap->tinst == INSN_PSEUDO_VS_STORE)
		/* Use uptrap as-is for guest-page faults */
		أرجع;
	/*
	 * Only fixup if orig tinst is valid. Otherwise, discard the
	 * new tinst to be on the safe side. Never use new tinst as-is!
	 * It's load/store width surely mismatches the original width.
	 * For vector, discard it regardless. It doesn't make sense to
	 * have a transformed tinst
	 */
	وإلا إذا (orig_trap->tinst == 0)
		uptrap->tinst = 0;
	وإلا
		uptrap->tinst = orig_trap->tinst | (وتھ << SH_RS1);
}

ساكن مضمن bool sbi_trap_tinst_valid(ulong tinst)
{
	/*
	 * Bit[0] == 1 implies trapped instruction value is
	 * transformed instruction or custom instruction.
	 * Also do proper checking per Privileged ISA 19.6.3,
	 * and make sure high 32 bits of tinst is 0
	 */
	أرجع tinst == (uint32_t)tinst && (tinst & 0x1);
}

ساكن صحيح sbi_trap_emulate_لدو(بنية sbi_trap_context *tcntx,
				 sbi_trap_ld_emulator emu)
{
	ثابت بنية sbi_trap_info *orig_trap = &tcntx->trap;
	بنية sbi_trap_regs *regs = &tcntx->regs;
	ulong insn, insn_len, imm = 0, کھسکاؤ = 0, off = 0;
	اتحاد sbi_ldst_انکڑے val = { 0 };
	بنية sbi_trap_info uptrap;
	bool xform = false, fp = false, c_لدو = false, c_ldsp = false;
	صحيح rc, len = 0, prev_xlen = 0;

	إذا (sbi_trap_tinst_valid(orig_trap->tinst)) {
		xform	 = true;
		insn	 = orig_trap->tinst | INSN_16_ثنائی_ہندسہ_نقاب;
		insn_len = (orig_trap->tinst & 0x2) ? INSN_LEN(insn) : 2;
	} وإلا {
		/* trapped instruction value is zero or special value */
		insn = sbi_get_insn(regs->mepc, &uptrap);
		إذا (uptrap.cause) {
			أرجع sbi_trap_redirect(regs, &uptrap);
		}
		insn_len = INSN_LEN(insn);
	}

	/**
	 * Common for RV32/RV64:
	 *    lb, lbu, lh, lhu, lw, flw, flw
	 *    c.lbu, c.lh, c.lhu, c.lw, c.lwsp, c.fld, c.fldsp
	 */
	إذا ((insn & INSN_نقاب_LB) == INSN_میل_LB) {
		len = -1;
	} وإلا إذا ((insn & INSN_نقاب_LBU) == INSN_میل_LBU) {
		len = 1;
	} وإلا إذا ((insn & INSN_نقاب_C_LBU) == INSN_میل_C_LBU) {
		/* Zcb */
		len = 1;
		imm = RVC_LB_IMM(insn);
		c_لدو = true;
	} وإلا إذا ((insn & INSN_نقاب_LH) == INSN_میل_LH) {
		len = -2;
	} وإلا إذا ((insn & INSN_نقاب_C_LH) == INSN_میل_C_LH) {
		/* Zcb */
		len = -2;
		imm = RVC_LH_IMM(insn);
		c_لدو = true;
	} وإلا إذا ((insn & INSN_نقاب_LHU) == INSN_میل_LHU) {
		len = 2;
	} وإلا إذا ((insn & INSN_نقاب_C_LHU) == INSN_میل_C_LHU) {
		/* Zcb */
		len = 2;
		imm = RVC_LH_IMM(insn);
		c_لدو = true;
	} وإلا إذا ((insn & INSN_نقاب_LW) == INSN_میل_LW) {
		len = -4;
	} وإلا إذا ((insn & INSN_نقاب_C_LW) == INSN_میل_C_LW) {
		/* Zca */
		len = -4;
		imm = RVC_LW_IMM(insn);
		c_لدو = true;
	} وإلا إذا ((insn & INSN_نقاب_C_LWSP) == INSN_میل_C_LWSP &&
		GET_RD_عدد(insn)) {
		/* Zca */
		len = -4;
		imm = RVC_LWSP_IMM(insn);
		c_ldsp = true;
#جے_تعریف_شدہ __riscv_flen
	} وإلا إذا ((insn & INSN_نقاب_FLW) == INSN_میل_FLW) {
		len = 4;
		fp = true;
	} وإلا إذا ((insn & INSN_نقاب_FLD) == INSN_میل_FLD) {
		len = 8;
		fp = true;
	} وإلا إذا ((insn & INSN_نقاب_C_FLD) == INSN_میل_C_FLD) {
		/* Zcd */
		len = 8;
		imm = RVC_LD_IMM(insn);
		c_لدو = true;
		fp = true;
	} وإلا إذا ((insn & INSN_نقاب_C_FLDSP) == INSN_میل_C_FLDSP) {
		/* Zcd */
		len = 8;
		imm = RVC_LDSP_IMM(insn);
		c_ldsp = true;
		fp = true;
#جے_انت
	} وإلا {
		prev_xlen = sbi_regs_prev_xlen(regs);
	}

	/**
	 * Must distinguish between rv64 and rv32, RVC instructions have
	 * overlapping encoding:
	 *     c.ld in rv64 == c.flw in rv32
	 *     c.ldsp in rv64 == c.flwsp in rv32
	 */
	إذا (prev_xlen == 64) {
		/* RV64 Only: lwu, ld, c.ld, c.ldsp  */
		إذا ((insn & INSN_نقاب_LWU) == INSN_میل_LWU) {
			len = 4;
		} وإلا إذا ((insn & INSN_نقاب_LD) == INSN_میل_LD) {
			len = 8;
		} وإلا إذا ((insn & INSN_نقاب_C_LD) == INSN_میل_C_LD) {
			/* Zca */
			len = 8;
			imm = RVC_LD_IMM(insn);
			c_لدو = true;
		} وإلا إذا ((insn & INSN_نقاب_C_LDSP) == INSN_میل_C_LDSP &&
			GET_RD_عدد(insn)) {
			/* Zca */
			len = 8;
			imm = RVC_LDSP_IMM(insn);
			c_ldsp = true;
		}
#جے_تعریف_شدہ __riscv_flen
	} وإلا إذا (prev_xlen == 32) {
		/* RV32 Only: c.flw, c.flwsp */
		إذا ((insn & INSN_نقاب_C_FLW) == INSN_میل_C_FLW) {
			/* Zcf */
			len = 4;
			imm = RVC_LW_IMM(insn);
			c_لدو = true;
			fp = true;
		} وإلا إذا ((insn & INSN_نقاب_C_FLWSP) == INSN_میل_C_FLWSP) {
			/* Zcf */
			len = 4;
			imm = RVC_LWSP_IMM(insn);
			c_ldsp = true;
			fp = true;
		}
#جے_انت
	}

	إذا (len < 0) {
		len = -len;
		کھسکاؤ = 8 * (حجم(ulong) - len);
	}

	إذا (!len) // Unknown instruction
		اذهب do_emu;

#إذا !defined(OPENSBI_DEBUG)
	/**
	 * For misaligned faults. Skip offset calculation unless DEBUG
	 * builds. It helps validating OpenSBI and HW.
	 */
	إذا (orig_trap->cause == CAUSE_MISALIGNED_لدو)
		اذهب do_emu;
#جے_انت

	إذا (xform)
		/* Transformed insn */
		off = GET_RS1_عدد(insn);
	وإلا إذا (c_لدو)
		/* non SP-based compressed load */
		off = orig_trap->tval - GET_RS1S(insn, regs) - imm;
	وإلا إذا (c_ldsp)
		/* SP-based compressed load */
		off = orig_trap->tval - REG_VAL(2, regs) - imm;
	وإلا
		/* I-type non-compressed load */
		off = orig_trap->tval - GET_RS1(insn, regs) - (ulong)IMM_I(insn);
	/**
	 * Normalize offset, in case the XLEN of unpriv mode is smaller,
	 * and/or pointer masking is in effect
	 */
	off &= (len - 1);

do_emu:
	rc = emu(insn, len, orig_trap->tval - off, &val, tcntx);
	إذا (rc <= 0)
		أرجع rc;
	إذا (!len)
		اذهب epc_fixup;

	إذا (!fp) {
		ulong v = ((لما)(val.انکڑے_ulong << کھسکاؤ)) >> کھسکاؤ;

		إذا (c_لدو)
			سیٹ_RDS(insn, regs, v);
		وإلا
			سیٹ_RD(insn, regs, v);
#جے_تعریف_شدہ __riscv_flen
	} وإلا إذا (len == 8) {
		إذا (c_لدو)
			سیٹ_F64_RDS(insn, regs, val.انکڑے_u64);
		وإلا
			سیٹ_F64_RD(insn, regs, val.انکڑے_u64);
	} وإلا {
		إذا (c_لدو)
			سیٹ_F32_RDS(insn, regs, val.انکڑے_ulong);
		وإلا
			سیٹ_F32_RD(insn, regs, val.انکڑے_ulong);
#جے_انت
	}

epc_fixup:
	regs->mepc += insn_len;

	أرجع 0;
}

ساكن صحيح sbi_trap_emulate_store(بنية sbi_trap_context *tcntx,
				  sbi_trap_st_emulator emu)
{
	ثابت بنية sbi_trap_info *orig_trap = &tcntx->trap;
	بنية sbi_trap_regs *regs = &tcntx->regs;
	ulong insn, insn_len, imm = 0, off = 0;
	اتحاد sbi_ldst_انکڑے val;
	بنية sbi_trap_info uptrap;
	bool xform = false, fp = false, c_store = false, c_stsp = false;
	صحيح rc, len = 0, prev_xlen = 0;

	إذا (sbi_trap_tinst_valid(orig_trap->tinst)) {
		xform	 = true;
		insn	 = orig_trap->tinst | INSN_16_ثنائی_ہندسہ_نقاب;
		insn_len = (orig_trap->tinst & 0x2) ? INSN_LEN(insn) : 2;
	} وإلا {
		/* trapped instruction value is zero or special value */
		insn = sbi_get_insn(regs->mepc, &uptrap);
		إذا (uptrap.cause) {
			أرجع sbi_trap_redirect(regs, &uptrap);
		}
		insn_len = INSN_LEN(insn);
	}

	/**
	 * Common for RV32/RV64:
	 *    sb, sh, sw, fsw, fsd
	 *    c.sb, c.sh, c.sw, c.swsp, c.fsd, c.fsdsp
	 */
	إذا ((insn & INSN_نقاب_SB) == INSN_میل_SB) {
		len = 1;
	} وإلا إذا ((insn & INSN_نقاب_C_SB) == INSN_میل_C_SB) {
		/* Zcb */
		len = 1;
		imm = RVC_SB_IMM(insn);
		c_store = true;
	} وإلا إذا ((insn & INSN_نقاب_SH) == INSN_میل_SH) {
		len = 2;
	} وإلا إذا ((insn & INSN_نقاب_C_SH) == INSN_میل_C_SH) {
		/* Zcb */
		len = 2;
		imm = RVC_SH_IMM(insn);
		c_store = true;
	} وإلا إذا ((insn & INSN_نقاب_SW) == INSN_میل_SW) {
		len = 4;
	} وإلا إذا ((insn & INSN_نقاب_C_SW) == INSN_میل_C_SW) {
		/* Zca */
		len = 4;
		imm = RVC_SW_IMM(insn);
		c_store = true;
	} وإلا إذا ((insn & INSN_نقاب_C_SWSP) == INSN_میل_C_SWSP) {
		/* Zca */
		len = 4;
		imm = RVC_SWSP_IMM(insn);
		c_stsp = true;
#جے_تعریف_شدہ __riscv_flen
	} وإلا إذا ((insn & INSN_نقاب_FSW) == INSN_میل_FSW) {
		len = 4;
		fp = true;
	} وإلا إذا ((insn & INSN_نقاب_FSD) == INSN_میل_FSD) {
		len = 8;
		fp = true;
	} وإلا إذا ((insn & INSN_نقاب_C_FSD) == INSN_میل_C_FSD) {
		/* Zcd */
		len = 8;
		imm = RVC_SD_IMM(insn);
		c_store = true;
		fp = true;
	} وإلا إذا ((insn & INSN_نقاب_C_FSDSP) == INSN_میل_C_FSDSP) {
		/* Zcd */
		len = 8;
		imm = RVC_SDSP_IMM(insn);
		c_stsp = true;
		fp = true;
#جے_انت
	} وإلا {
		prev_xlen = sbi_regs_prev_xlen(regs);
	}

	/**
	 * Must distinguish between rv64 and rv32, RVC instructions have
	 * overlapping encoding:
	 *     c.sd in rv64 == c.fsw in rv32
	 *     c.sdsp in rv64 == c.fswsp in rv32
	 */
	إذا (prev_xlen == 64) {
		/* RV64 Only: sd, c.sd, c.sdsp */
		إذا ((insn & INSN_نقاب_SD) == INSN_میل_SD) {
			len = 8;
		} وإلا إذا ((insn & INSN_نقاب_C_SD) == INSN_میل_C_SD) {
			/* Zca */
			len = 8;
			imm = RVC_SD_IMM(insn);
			c_store = true;
		} وإلا إذا ((insn & INSN_نقاب_C_SDSP) == INSN_میل_C_SDSP) {
			/* Zca */
			len = 8;
			imm = RVC_SDSP_IMM(insn);
			c_stsp = true;
		}
#جے_تعریف_شدہ __riscv_flen
	} وإلا إذا (prev_xlen == 32) {
		/* RV32 Only: c.fsw, c.fswsp */
		إذا ((insn & INSN_نقاب_C_FSW) == INSN_میل_C_FSW) {
			/* Zcf */
			len = 4;
			imm = RVC_SW_IMM(insn);
			c_store = true;
			fp = true;
		} وإلا إذا ((insn & INSN_نقاب_C_FSWSP) == INSN_میل_C_FSWSP) {
			/* Zcf */
			len = 4;
			imm = RVC_SWSP_IMM(insn);
			c_stsp = true;
			fp = true;
		}
#جے_انت
	}

	إذا (!fp) {
		إذا (c_store)
			val.انکڑے_ulong = GET_RS2S(insn, regs);
		وإلا إذا (c_stsp)
			val.انکڑے_ulong = GET_RS2C(insn, regs);
		وإلا
			val.انکڑے_ulong = GET_RS2(insn, regs);
#جے_تعریف_شدہ __riscv_flen
	} وإلا إذا (len == 8) {
		إذا (c_store)
			val.انکڑے_u64 = GET_F64_RS2S(insn, regs);
		وإلا إذا (c_stsp)
			val.انکڑے_u64 = GET_F64_RS2C(insn, regs);
		وإلا
			val.انکڑے_u64 = GET_F64_RS2(insn, regs);
	} وإلا {
		إذا (c_store)
			val.انکڑے_ulong = GET_F32_RS2S(insn, regs);
		وإلا إذا (c_stsp)
			val.انکڑے_ulong = GET_F32_RS2C(insn, regs);
		وإلا
			val.انکڑے_ulong = GET_F32_RS2(insn, regs);
#جے_انت
	}

	إذا (!len) // Unknown instruction
		اذهب do_emu;

#إذا !defined(OPENSBI_DEBUG)
	/**
	 * For misaligned faults. Skip offset calculation unless DEBUG
	 * builds. It helps validating OpenSBI and HW.
	 */
	إذا (orig_trap->cause == CAUSE_MISALIGNED_STORE)
		اذهب do_emu;
#جے_انت

	إذا (xform)
		/* Transformed insn */
		off = GET_RS1_عدد(insn);
	وإلا إذا (c_store)
		/* non SP-based compressed store */
		off = orig_trap->tval - GET_RS1S(insn, regs) - imm;
	وإلا إذا (c_stsp)
		/* SP-based compressed store */
		off = orig_trap->tval - REG_VAL(2, regs) - imm;
	وإلا
		/* S-type non-compressed store */
		off = orig_trap->tval - GET_RS1(insn, regs) - (ulong)IMM_S(insn);
	/**
	 * Normalize offset, in case the XLEN of unpriv mode is smaller,
	 * and/or pointer masking is in effect
	 */
	off &= (len - 1);

do_emu:
	rc = emu(insn, len, orig_trap->tval - off, val, tcntx);
	إذا (rc <= 0)
		أرجع rc;

	regs->mepc += insn_len;

	أرجع 0;
}

ساكن صحيح sbi_misaligned_ld_emulator(ulong insn, صحيح rlen, ulong addr,
				      اتحاد sbi_ldst_انکڑے *باہرلا_val,
				      بنية sbi_trap_context *tcntx)
{
	ثابت بنية sbi_trap_info *orig_trap = &tcntx->trap;
	بنية sbi_trap_regs *regs = &tcntx->regs;
	بنية sbi_trap_info uptrap;

	إذا (!rlen) {
		إذا (IS_سمتیہ_لدو_STORE(insn))
			أرجع sbi_misaligned_v_ld_emulator(insn, tcntx);
		وإلا
			/* Unrecognized instruction. Can't emulate it. */
			أرجع sbi_trap_redirect(regs, orig_trap);
	}
	/* For misaligned fault, addr must be the same as orig_trap->tval */
	إذا (addr != orig_trap->tval)
		أرجع SBI_EFAIL;

	sbi_لدو_loop(باہرلا_val->انکڑے_بائٹاں, addr, rlen, &uptrap);
	إذا (uptrap.cause) {
		sbi_misaligned_tinst_fixup(orig_trap, &uptrap);
		أرجع sbi_trap_redirect(regs, &uptrap);
	}
	أرجع rlen;
}

صحيح sbi_misaligned_لدو_handler(بنية sbi_trap_context *tcntx)
{
	أرجع sbi_trap_emulate_لدو(tcntx, sbi_misaligned_ld_emulator);
}

ساكن صحيح sbi_misaligned_st_emulator(ulong insn, صحيح wlen, ulong addr,
				      اتحاد sbi_ldst_انکڑے وچ_val,
				      بنية sbi_trap_context *tcntx)
{
	ثابت بنية sbi_trap_info *orig_trap = &tcntx->trap;
	بنية sbi_trap_regs *regs = &tcntx->regs;
	بنية sbi_trap_info uptrap;

	إذا (!wlen) {
		إذا (IS_سمتیہ_لدو_STORE(insn))
			أرجع sbi_misaligned_v_st_emulator(insn, tcntx);
		وإلا
			/* Unrecognized instruction. Can't emulate it. */
			أرجع sbi_trap_redirect(regs, orig_trap);
	}
	/* For misaligned fault, addr must be the same as orig_trap->tval */
	إذا (addr != orig_trap->tval)
		أرجع SBI_EFAIL;

	sbi_store_loop(وچ_val.انکڑے_بائٹاں, addr, wlen, &uptrap);
	إذا (uptrap.cause) {
		sbi_misaligned_tinst_fixup(orig_trap, &uptrap);
		أرجع sbi_trap_redirect(regs, &uptrap);
	}
	أرجع wlen;
}

صحيح sbi_misaligned_store_handler(بنية sbi_trap_context *tcntx)
{
	أرجع sbi_trap_emulate_store(tcntx, sbi_misaligned_st_emulator);
}

ساكن صحيح sbi_ld_access_emulator(ulong insn, صحيح rlen, ulong addr,
				  اتحاد sbi_ldst_انکڑے *باہرلا_val,
				  بنية sbi_trap_context *tcntx)
{
	ثابت بنية sbi_trap_info *orig_trap = &tcntx->trap;
	بنية sbi_trap_regs *regs = &tcntx->regs;
	صحيح rc;

	/* If fault came from M mode, just fail */
	إذا (sbi_mstatus_prev_منوال(regs->mstatus) == PRV_M)
		أرجع SBI_EINVAL;

	rc = sbi_platform_emulate_لدو(sbi_platform_thishart_ptr(),
				       insn, rlen, addr, باہرلا_val, tcntx);

	/* If platform emulator failed, we redirect instead of fail */
	إذا (rc < 0)
		أرجع sbi_trap_redirect(regs, orig_trap);

	أرجع rc;
}

صحيح sbi_لدو_access_handler(بنية sbi_trap_context *tcntx)
{
	أرجع sbi_trap_emulate_لدو(tcntx, sbi_ld_access_emulator);
}

ساكن صحيح sbi_st_access_emulator(ulong insn, صحيح wlen, ulong addr,
				  اتحاد sbi_ldst_انکڑے وچ_val,
				  بنية sbi_trap_context *tcntx)
{
	ثابت بنية sbi_trap_info *orig_trap = &tcntx->trap;
	بنية sbi_trap_regs *regs = &tcntx->regs;
	صحيح rc;

	/* If fault came from M mode, just fail */
	إذا (sbi_mstatus_prev_منوال(regs->mstatus) == PRV_M)
		أرجع SBI_EINVAL;

	rc = sbi_platform_emulate_store(sbi_platform_thishart_ptr(),
					insn, wlen, addr, وچ_val, tcntx);

	/* If platform emulator failed, we redirect instead of fail */
	إذا (rc < 0)
		أرجع sbi_trap_redirect(regs, orig_trap);

	أرجع rc;
}

صحيح sbi_store_access_handler(بنية sbi_trap_context *tcntx)
{
	أرجع sbi_trap_emulate_store(tcntx, sbi_st_access_emulator);
}
