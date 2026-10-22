/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_encoding.h>
#شامل <sbi/riscv_fp.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_trap_ldst.h>
#شامل <sbi/sbi_trap.h>
#شامل <sbi/sbi_unpriv.h>
#شامل <sbi/sbi_platform.h>

/**
 * Load emulator callback:
 *   Refer to comments of `sbi_platform_emulate_load`.
 */
قسمتعریف صحیح (*sbi_trap_ld_emulator)(ulong insn, صحیح rlen, ulong raddr,
				    اتحاد sbi_ldst_ڈیٹا *برآمد_val,
				    ساخت sbi_trap_context *tcntx);

/**
 * Store emulator callback:
 *   Refer to comments of `sbi_platform_emulate_store`.
 */
قسمتعریف صحیح (*sbi_trap_st_emulator)(ulong insn, صحیح wlen, ulong waddr,
				    اتحاد sbi_ldst_ڈیٹا اندر_val,
				    ساخت sbi_trap_context *tcntx);

/**
 * Handling of misaligned fault is done by a collection of smaller, but
 * aligned load/store(s). Another fault (load/store, page fault...) can
 * arise from any of them, then the handling gets aborted. We must fixup
 * the tinst to pretend the fault was rised from the original insn.
 * Specifically, fixup the offset field using the tval diff between the
 * new trap and the original one (if required).
 */
ساکن اندرونی خالی sbi_misaligned_tinst_fixup(
			ثابت ساخت sbi_trap_info *orig_trap,
			      ساخت sbi_trap_info *uptrap)
{
	ulong انحراف = uptrap->tval - orig_trap->tval;

	/*
	 * The function is called in code path for handling a scalar
	 * load/store misaligned fault, thus the new uptrap can't have
	 * custom value of tinst
	 */
	اگر (uptrap->tinst == INSN_PSEUDO_VS_بھرو ||
	    uptrap->tinst == INSN_PSEUDO_VS_STORE)
		/* Use uptrap as-is for guest-page faults */
		واپس;
	/*
	 * Only fixup if orig tinst is valid. Otherwise, discard the
	 * new tinst to be on the safe side. Never use new tinst as-is!
	 * It's load/store width surely mismatches the original width.
	 * For vector, discard it regardless. It doesn't make sense to
	 * have a transformed tinst
	 */
	ورنہ اگر (orig_trap->tinst == 0)
		uptrap->tinst = 0;
	ورنہ
		uptrap->tinst = orig_trap->tinst | (انحراف << SH_RS1);
}

ساکن اندرونی bool sbi_trap_tinst_valid(ulong tinst)
{
	/*
	 * Bit[0] == 1 implies trapped instruction value is
	 * transformed instruction or custom instruction.
	 * Also do proper checking per Privileged ISA 19.6.3,
	 * and make sure high 32 bits of tinst is 0
	 */
	واپس tinst == (uint32_t)tinst && (tinst & 0x1);
}

ساکن صحیح sbi_trap_emulate_بھرو(ساخت sbi_trap_context *tcntx,
				 sbi_trap_ld_emulator emu)
{
	ثابت ساخت sbi_trap_info *orig_trap = &tcntx->trap;
	ساخت sbi_trap_regs *regs = &tcntx->regs;
	ulong insn, insn_len, imm = 0, سرکاؤ = 0, off = 0;
	اتحاد sbi_ldst_ڈیٹا val = { 0 };
	ساخت sbi_trap_info uptrap;
	bool xform = false, fp = false, c_بھرو = false, c_ldsp = false;
	صحیح rc, len = 0, پچھلا_xlen = 0;

	اگر (sbi_trap_tinst_valid(orig_trap->tinst)) {
		xform	 = true;
		insn	 = orig_trap->tinst | INSN_16_ثنائی_ہندسہ_نقاب;
		insn_len = (orig_trap->tinst & 0x2) ? INSN_LEN(insn) : 2;
	} ورنہ {
		/* trapped instruction value is zero or special value */
		insn = sbi_get_insn(regs->mepc, &uptrap);
		اگر (uptrap.cause) {
			واپس sbi_trap_redirect(regs, &uptrap);
		}
		insn_len = INSN_LEN(insn);
	}

	/**
	 * Common for RV32/RV64:
	 *    lb, lbu, lh, lhu, lw, flw, flw
	 *    c.lbu, c.lh, c.lhu, c.lw, c.lwsp, c.fld, c.fldsp
	 */
	اگر ((insn & INSN_نقاب_LB) == INSN_مطابقت_LB) {
		len = -1;
	} ورنہ اگر ((insn & INSN_نقاب_LBU) == INSN_مطابقت_LBU) {
		len = 1;
	} ورنہ اگر ((insn & INSN_نقاب_C_LBU) == INSN_مطابقت_C_LBU) {
		/* Zcb */
		len = 1;
		imm = RVC_LB_IMM(insn);
		c_بھرو = true;
	} ورنہ اگر ((insn & INSN_نقاب_LH) == INSN_مطابقت_LH) {
		len = -2;
	} ورنہ اگر ((insn & INSN_نقاب_C_LH) == INSN_مطابقت_C_LH) {
		/* Zcb */
		len = -2;
		imm = RVC_LH_IMM(insn);
		c_بھرو = true;
	} ورنہ اگر ((insn & INSN_نقاب_LHU) == INSN_مطابقت_LHU) {
		len = 2;
	} ورنہ اگر ((insn & INSN_نقاب_C_LHU) == INSN_مطابقت_C_LHU) {
		/* Zcb */
		len = 2;
		imm = RVC_LH_IMM(insn);
		c_بھرو = true;
	} ورنہ اگر ((insn & INSN_نقاب_LW) == INSN_مطابقت_LW) {
		len = -4;
	} ورنہ اگر ((insn & INSN_نقاب_C_LW) == INSN_مطابقت_C_LW) {
		/* Zca */
		len = -4;
		imm = RVC_LW_IMM(insn);
		c_بھرو = true;
	} ورنہ اگر ((insn & INSN_نقاب_C_LWSP) == INSN_مطابقت_C_LWSP &&
		GET_RD_عدد(insn)) {
		/* Zca */
		len = -4;
		imm = RVC_LWSP_IMM(insn);
		c_ldsp = true;
#اگر_تعریف_شدہ __riscv_flen
	} ورنہ اگر ((insn & INSN_نقاب_FLW) == INSN_مطابقت_FLW) {
		len = 4;
		fp = true;
	} ورنہ اگر ((insn & INSN_نقاب_FLD) == INSN_مطابقت_FLD) {
		len = 8;
		fp = true;
	} ورنہ اگر ((insn & INSN_نقاب_C_FLD) == INSN_مطابقت_C_FLD) {
		/* Zcd */
		len = 8;
		imm = RVC_LD_IMM(insn);
		c_بھرو = true;
		fp = true;
	} ورنہ اگر ((insn & INSN_نقاب_C_FLDSP) == INSN_مطابقت_C_FLDSP) {
		/* Zcd */
		len = 8;
		imm = RVC_LDSP_IMM(insn);
		c_ldsp = true;
		fp = true;
#اگر_ختم
	} ورنہ {
		پچھلا_xlen = sbi_regs_پچھلا_xlen(regs);
	}

	/**
	 * Must distinguish between rv64 and rv32, RVC instructions have
	 * overlapping encoding:
	 *     c.ld in rv64 == c.flw in rv32
	 *     c.ldsp in rv64 == c.flwsp in rv32
	 */
	اگر (پچھلا_xlen == 64) {
		/* RV64 Only: lwu, ld, c.ld, c.ldsp  */
		اگر ((insn & INSN_نقاب_LWU) == INSN_مطابقت_LWU) {
			len = 4;
		} ورنہ اگر ((insn & INSN_نقاب_LD) == INSN_مطابقت_LD) {
			len = 8;
		} ورنہ اگر ((insn & INSN_نقاب_C_LD) == INSN_مطابقت_C_LD) {
			/* Zca */
			len = 8;
			imm = RVC_LD_IMM(insn);
			c_بھرو = true;
		} ورنہ اگر ((insn & INSN_نقاب_C_LDSP) == INSN_مطابقت_C_LDSP &&
			GET_RD_عدد(insn)) {
			/* Zca */
			len = 8;
			imm = RVC_LDSP_IMM(insn);
			c_ldsp = true;
		}
#اگر_تعریف_شدہ __riscv_flen
	} ورنہ اگر (پچھلا_xlen == 32) {
		/* RV32 Only: c.flw, c.flwsp */
		اگر ((insn & INSN_نقاب_C_FLW) == INSN_مطابقت_C_FLW) {
			/* Zcf */
			len = 4;
			imm = RVC_LW_IMM(insn);
			c_بھرو = true;
			fp = true;
		} ورنہ اگر ((insn & INSN_نقاب_C_FLWSP) == INSN_مطابقت_C_FLWSP) {
			/* Zcf */
			len = 4;
			imm = RVC_LWSP_IMM(insn);
			c_ldsp = true;
			fp = true;
		}
#اگر_ختم
	}

	اگر (len < 0) {
		len = -len;
		سرکاؤ = 8 * (حجم(ulong) - len);
	}

	اگر (!len) // Unknown instruction
		جاؤ do_emu;

#اگر !defined(OPENSBI_DEBUG)
	/**
	 * For misaligned faults. Skip offset calculation unless DEBUG
	 * builds. It helps validating OpenSBI and HW.
	 */
	اگر (orig_trap->cause == CAUSE_MISALIGNED_بھرو)
		جاؤ do_emu;
#اگر_ختم

	اگر (xform)
		/* Transformed insn */
		off = GET_RS1_عدد(insn);
	ورنہ اگر (c_بھرو)
		/* non SP-based compressed load */
		off = orig_trap->tval - GET_RS1S(insn, regs) - imm;
	ورنہ اگر (c_ldsp)
		/* SP-based compressed load */
		off = orig_trap->tval - REG_VAL(2, regs) - imm;
	ورنہ
		/* I-type non-compressed load */
		off = orig_trap->tval - GET_RS1(insn, regs) - (ulong)IMM_I(insn);
	/**
	 * Normalize offset, in case the XLEN of unpriv mode is smaller,
	 * and/or pointer masking is in effect
	 */
	off &= (len - 1);

do_emu:
	rc = emu(insn, len, orig_trap->tval - off, &val, tcntx);
	اگر (rc <= 0)
		واپس rc;
	اگر (!len)
		جاؤ epc_fixup;

	اگر (!fp) {
		ulong v = ((طویل)(val.ڈیٹا_ulong << سرکاؤ)) >> سرکاؤ;

		اگر (c_بھرو)
			سیٹ_RDS(insn, regs, v);
		ورنہ
			سیٹ_RD(insn, regs, v);
#اگر_تعریف_شدہ __riscv_flen
	} ورنہ اگر (len == 8) {
		اگر (c_بھرو)
			سیٹ_F64_RDS(insn, regs, val.ڈیٹا_u64);
		ورنہ
			سیٹ_F64_RD(insn, regs, val.ڈیٹا_u64);
	} ورنہ {
		اگر (c_بھرو)
			سیٹ_F32_RDS(insn, regs, val.ڈیٹا_ulong);
		ورنہ
			سیٹ_F32_RD(insn, regs, val.ڈیٹا_ulong);
#اگر_ختم
	}

epc_fixup:
	regs->mepc += insn_len;

	واپس 0;
}

ساکن صحیح sbi_trap_emulate_store(ساخت sbi_trap_context *tcntx,
				  sbi_trap_st_emulator emu)
{
	ثابت ساخت sbi_trap_info *orig_trap = &tcntx->trap;
	ساخت sbi_trap_regs *regs = &tcntx->regs;
	ulong insn, insn_len, imm = 0, off = 0;
	اتحاد sbi_ldst_ڈیٹا val;
	ساخت sbi_trap_info uptrap;
	bool xform = false, fp = false, c_store = false, c_stsp = false;
	صحیح rc, len = 0, پچھلا_xlen = 0;

	اگر (sbi_trap_tinst_valid(orig_trap->tinst)) {
		xform	 = true;
		insn	 = orig_trap->tinst | INSN_16_ثنائی_ہندسہ_نقاب;
		insn_len = (orig_trap->tinst & 0x2) ? INSN_LEN(insn) : 2;
	} ورنہ {
		/* trapped instruction value is zero or special value */
		insn = sbi_get_insn(regs->mepc, &uptrap);
		اگر (uptrap.cause) {
			واپس sbi_trap_redirect(regs, &uptrap);
		}
		insn_len = INSN_LEN(insn);
	}

	/**
	 * Common for RV32/RV64:
	 *    sb, sh, sw, fsw, fsd
	 *    c.sb, c.sh, c.sw, c.swsp, c.fsd, c.fsdsp
	 */
	اگر ((insn & INSN_نقاب_SB) == INSN_مطابقت_SB) {
		len = 1;
	} ورنہ اگر ((insn & INSN_نقاب_C_SB) == INSN_مطابقت_C_SB) {
		/* Zcb */
		len = 1;
		imm = RVC_SB_IMM(insn);
		c_store = true;
	} ورنہ اگر ((insn & INSN_نقاب_SH) == INSN_مطابقت_SH) {
		len = 2;
	} ورنہ اگر ((insn & INSN_نقاب_C_SH) == INSN_مطابقت_C_SH) {
		/* Zcb */
		len = 2;
		imm = RVC_SH_IMM(insn);
		c_store = true;
	} ورنہ اگر ((insn & INSN_نقاب_SW) == INSN_مطابقت_SW) {
		len = 4;
	} ورنہ اگر ((insn & INSN_نقاب_C_SW) == INSN_مطابقت_C_SW) {
		/* Zca */
		len = 4;
		imm = RVC_SW_IMM(insn);
		c_store = true;
	} ورنہ اگر ((insn & INSN_نقاب_C_SWSP) == INSN_مطابقت_C_SWSP) {
		/* Zca */
		len = 4;
		imm = RVC_SWSP_IMM(insn);
		c_stsp = true;
#اگر_تعریف_شدہ __riscv_flen
	} ورنہ اگر ((insn & INSN_نقاب_FSW) == INSN_مطابقت_FSW) {
		len = 4;
		fp = true;
	} ورنہ اگر ((insn & INSN_نقاب_FSD) == INSN_مطابقت_FSD) {
		len = 8;
		fp = true;
	} ورنہ اگر ((insn & INSN_نقاب_C_FSD) == INSN_مطابقت_C_FSD) {
		/* Zcd */
		len = 8;
		imm = RVC_SD_IMM(insn);
		c_store = true;
		fp = true;
	} ورنہ اگر ((insn & INSN_نقاب_C_FSDSP) == INSN_مطابقت_C_FSDSP) {
		/* Zcd */
		len = 8;
		imm = RVC_SDSP_IMM(insn);
		c_stsp = true;
		fp = true;
#اگر_ختم
	} ورنہ {
		پچھلا_xlen = sbi_regs_پچھلا_xlen(regs);
	}

	/**
	 * Must distinguish between rv64 and rv32, RVC instructions have
	 * overlapping encoding:
	 *     c.sd in rv64 == c.fsw in rv32
	 *     c.sdsp in rv64 == c.fswsp in rv32
	 */
	اگر (پچھلا_xlen == 64) {
		/* RV64 Only: sd, c.sd, c.sdsp */
		اگر ((insn & INSN_نقاب_SD) == INSN_مطابقت_SD) {
			len = 8;
		} ورنہ اگر ((insn & INSN_نقاب_C_SD) == INSN_مطابقت_C_SD) {
			/* Zca */
			len = 8;
			imm = RVC_SD_IMM(insn);
			c_store = true;
		} ورنہ اگر ((insn & INSN_نقاب_C_SDSP) == INSN_مطابقت_C_SDSP) {
			/* Zca */
			len = 8;
			imm = RVC_SDSP_IMM(insn);
			c_stsp = true;
		}
#اگر_تعریف_شدہ __riscv_flen
	} ورنہ اگر (پچھلا_xlen == 32) {
		/* RV32 Only: c.fsw, c.fswsp */
		اگر ((insn & INSN_نقاب_C_FSW) == INSN_مطابقت_C_FSW) {
			/* Zcf */
			len = 4;
			imm = RVC_SW_IMM(insn);
			c_store = true;
			fp = true;
		} ورنہ اگر ((insn & INSN_نقاب_C_FSWSP) == INSN_مطابقت_C_FSWSP) {
			/* Zcf */
			len = 4;
			imm = RVC_SWSP_IMM(insn);
			c_stsp = true;
			fp = true;
		}
#اگر_ختم
	}

	اگر (!fp) {
		اگر (c_store)
			val.ڈیٹا_ulong = GET_RS2S(insn, regs);
		ورنہ اگر (c_stsp)
			val.ڈیٹا_ulong = GET_RS2C(insn, regs);
		ورنہ
			val.ڈیٹا_ulong = GET_RS2(insn, regs);
#اگر_تعریف_شدہ __riscv_flen
	} ورنہ اگر (len == 8) {
		اگر (c_store)
			val.ڈیٹا_u64 = GET_F64_RS2S(insn, regs);
		ورنہ اگر (c_stsp)
			val.ڈیٹا_u64 = GET_F64_RS2C(insn, regs);
		ورنہ
			val.ڈیٹا_u64 = GET_F64_RS2(insn, regs);
	} ورنہ {
		اگر (c_store)
			val.ڈیٹا_ulong = GET_F32_RS2S(insn, regs);
		ورنہ اگر (c_stsp)
			val.ڈیٹا_ulong = GET_F32_RS2C(insn, regs);
		ورنہ
			val.ڈیٹا_ulong = GET_F32_RS2(insn, regs);
#اگر_ختم
	}

	اگر (!len) // Unknown instruction
		جاؤ do_emu;

#اگر !defined(OPENSBI_DEBUG)
	/**
	 * For misaligned faults. Skip offset calculation unless DEBUG
	 * builds. It helps validating OpenSBI and HW.
	 */
	اگر (orig_trap->cause == CAUSE_MISALIGNED_STORE)
		جاؤ do_emu;
#اگر_ختم

	اگر (xform)
		/* Transformed insn */
		off = GET_RS1_عدد(insn);
	ورنہ اگر (c_store)
		/* non SP-based compressed store */
		off = orig_trap->tval - GET_RS1S(insn, regs) - imm;
	ورنہ اگر (c_stsp)
		/* SP-based compressed store */
		off = orig_trap->tval - REG_VAL(2, regs) - imm;
	ورنہ
		/* S-type non-compressed store */
		off = orig_trap->tval - GET_RS1(insn, regs) - (ulong)IMM_S(insn);
	/**
	 * Normalize offset, in case the XLEN of unpriv mode is smaller,
	 * and/or pointer masking is in effect
	 */
	off &= (len - 1);

do_emu:
	rc = emu(insn, len, orig_trap->tval - off, val, tcntx);
	اگر (rc <= 0)
		واپس rc;

	regs->mepc += insn_len;

	واپس 0;
}

ساکن صحیح sbi_misaligned_ld_emulator(ulong insn, صحیح rlen, ulong addr,
				      اتحاد sbi_ldst_ڈیٹا *برآمد_val,
				      ساخت sbi_trap_context *tcntx)
{
	ثابت ساخت sbi_trap_info *orig_trap = &tcntx->trap;
	ساخت sbi_trap_regs *regs = &tcntx->regs;
	ساخت sbi_trap_info uptrap;

	اگر (!rlen) {
		اگر (IS_سمتیہ_بھرو_STORE(insn))
			واپس sbi_misaligned_v_ld_emulator(insn, tcntx);
		ورنہ
			/* Unrecognized instruction. Can't emulate it. */
			واپس sbi_trap_redirect(regs, orig_trap);
	}
	/* For misaligned fault, addr must be the same as orig_trap->tval */
	اگر (addr != orig_trap->tval)
		واپس SBI_EFAIL;

	sbi_بھرو_loop(برآمد_val->ڈیٹا_بائٹیں, addr, rlen, &uptrap);
	اگر (uptrap.cause) {
		sbi_misaligned_tinst_fixup(orig_trap, &uptrap);
		واپس sbi_trap_redirect(regs, &uptrap);
	}
	واپس rlen;
}

صحیح sbi_misaligned_بھرو_handler(ساخت sbi_trap_context *tcntx)
{
	واپس sbi_trap_emulate_بھرو(tcntx, sbi_misaligned_ld_emulator);
}

ساکن صحیح sbi_misaligned_st_emulator(ulong insn, صحیح wlen, ulong addr,
				      اتحاد sbi_ldst_ڈیٹا اندر_val,
				      ساخت sbi_trap_context *tcntx)
{
	ثابت ساخت sbi_trap_info *orig_trap = &tcntx->trap;
	ساخت sbi_trap_regs *regs = &tcntx->regs;
	ساخت sbi_trap_info uptrap;

	اگر (!wlen) {
		اگر (IS_سمتیہ_بھرو_STORE(insn))
			واپس sbi_misaligned_v_st_emulator(insn, tcntx);
		ورنہ
			/* Unrecognized instruction. Can't emulate it. */
			واپس sbi_trap_redirect(regs, orig_trap);
	}
	/* For misaligned fault, addr must be the same as orig_trap->tval */
	اگر (addr != orig_trap->tval)
		واپس SBI_EFAIL;

	sbi_store_loop(اندر_val.ڈیٹا_بائٹیں, addr, wlen, &uptrap);
	اگر (uptrap.cause) {
		sbi_misaligned_tinst_fixup(orig_trap, &uptrap);
		واپس sbi_trap_redirect(regs, &uptrap);
	}
	واپس wlen;
}

صحیح sbi_misaligned_store_handler(ساخت sbi_trap_context *tcntx)
{
	واپس sbi_trap_emulate_store(tcntx, sbi_misaligned_st_emulator);
}

ساکن صحیح sbi_ld_access_emulator(ulong insn, صحیح rlen, ulong addr,
				  اتحاد sbi_ldst_ڈیٹا *برآمد_val,
				  ساخت sbi_trap_context *tcntx)
{
	ثابت ساخت sbi_trap_info *orig_trap = &tcntx->trap;
	ساخت sbi_trap_regs *regs = &tcntx->regs;
	صحیح rc;

	/* If fault came from M mode, just fail */
	اگر (sbi_mstatus_پچھلا_منوال(regs->mstatus) == PRV_M)
		واپس SBI_EINVAL;

	rc = sbi_platform_emulate_بھرو(sbi_platform_thishart_ptr(),
				       insn, rlen, addr, برآمد_val, tcntx);

	/* If platform emulator failed, we redirect instead of fail */
	اگر (rc < 0)
		واپس sbi_trap_redirect(regs, orig_trap);

	واپس rc;
}

صحیح sbi_بھرو_access_handler(ساخت sbi_trap_context *tcntx)
{
	واپس sbi_trap_emulate_بھرو(tcntx, sbi_ld_access_emulator);
}

ساکن صحیح sbi_st_access_emulator(ulong insn, صحیح wlen, ulong addr,
				  اتحاد sbi_ldst_ڈیٹا اندر_val,
				  ساخت sbi_trap_context *tcntx)
{
	ثابت ساخت sbi_trap_info *orig_trap = &tcntx->trap;
	ساخت sbi_trap_regs *regs = &tcntx->regs;
	صحیح rc;

	/* If fault came from M mode, just fail */
	اگر (sbi_mstatus_پچھلا_منوال(regs->mstatus) == PRV_M)
		واپس SBI_EINVAL;

	rc = sbi_platform_emulate_store(sbi_platform_thishart_ptr(),
					insn, wlen, addr, اندر_val, tcntx);

	/* If platform emulator failed, we redirect instead of fail */
	اگر (rc < 0)
		واپس sbi_trap_redirect(regs, orig_trap);

	واپس rc;
}

صحیح sbi_store_access_handler(ساخت sbi_trap_context *tcntx)
{
	واپس sbi_trap_emulate_store(tcntx, sbi_st_access_emulator);
}
