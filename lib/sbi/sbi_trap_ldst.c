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
				    اتحاد sbi_ldst_data *out_val,
				    بنية sbi_trap_context *tcntx);

/**
 * Store emulator callback:
 *   Refer to comments of `sbi_platform_emulate_store`.
 */
تعريفنوع صحيح (*sbi_trap_st_emulator)(ulong insn, صحيح wlen, ulong waddr,
				    اتحاد sbi_ldst_data in_val,
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
	ulong offset = uptrap->tval - orig_trap->tval;

	/*
	 * The function is called in code path for handling a scalar
	 * load/store misaligned fault, thus the new uptrap can't have
	 * custom value of tinst
	 */
	אם (uptrap->tinst == INSN_PSEUDO_VS_LOAD ||
	    uptrap->tinst == INSN_PSEUDO_VS_STORE)
		/* Use uptrap as-is for guest-page faults */
		החזר;
	/*
	 * Only fixup if orig tinst is valid. Otherwise, discard the
	 * new tinst to be on the safe side. Never use new tinst as-is!
	 * It's load/store width surely mismatches the original width.
	 * For vector, discard it regardless. It doesn't make sense to
	 * have a transformed tinst
	 */
	אחרת אם (orig_trap->tinst == 0)
		uptrap->tinst = 0;
	אחרת
		uptrap->tinst = orig_trap->tinst | (offset << SH_RS1);
}

ساكن مضمن bool sbi_trap_tinst_valid(ulong tinst)
{
	/*
	 * Bit[0] == 1 implies trapped instruction value is
	 * transformed instruction or custom instruction.
	 * Also do proper checking per Privileged ISA 19.6.3,
	 * and make sure high 32 bits of tinst is 0
	 */
	החזר tinst == (uint32_t)tinst && (tinst & 0x1);
}

ساكن صحيح sbi_trap_emulate_load(بنية sbi_trap_context *tcntx,
				 sbi_trap_ld_emulator emu)
{
	ثابت بنية sbi_trap_info *orig_trap = &tcntx->trap;
	بنية sbi_trap_regs *regs = &tcntx->regs;
	ulong insn, insn_len, imm = 0, shift = 0, off = 0;
	اتحاد sbi_ldst_data val = { 0 };
	بنية sbi_trap_info uptrap;
	bool xform = false, fp = false, c_load = false, c_ldsp = false;
	صحيح rc, len = 0, prev_xlen = 0;

	אם (sbi_trap_tinst_valid(orig_trap->tinst)) {
		xform	 = true;
		insn	 = orig_trap->tinst | INSN_16BIT_MASK;
		insn_len = (orig_trap->tinst & 0x2) ? INSN_LEN(insn) : 2;
	} אחרת {
		/* trapped instruction value is zero or special value */
		insn = sbi_get_insn(regs->mepc, &uptrap);
		אם (uptrap.cause) {
			החזר sbi_trap_redirect(regs, &uptrap);
		}
		insn_len = INSN_LEN(insn);
	}

	/**
	 * Common for RV32/RV64:
	 *    lb, lbu, lh, lhu, lw, flw, flw
	 *    c.lbu, c.lh, c.lhu, c.lw, c.lwsp, c.fld, c.fldsp
	 */
	אם ((insn & INSN_MASK_LB) == INSN_MATCH_LB) {
		len = -1;
	} אחרת אם ((insn & INSN_MASK_LBU) == INSN_MATCH_LBU) {
		len = 1;
	} אחרת אם ((insn & INSN_MASK_C_LBU) == INSN_MATCH_C_LBU) {
		/* Zcb */
		len = 1;
		imm = RVC_LB_IMM(insn);
		c_load = true;
	} אחרת אם ((insn & INSN_MASK_LH) == INSN_MATCH_LH) {
		len = -2;
	} אחרת אם ((insn & INSN_MASK_C_LH) == INSN_MATCH_C_LH) {
		/* Zcb */
		len = -2;
		imm = RVC_LH_IMM(insn);
		c_load = true;
	} אחרת אם ((insn & INSN_MASK_LHU) == INSN_MATCH_LHU) {
		len = 2;
	} אחרת אם ((insn & INSN_MASK_C_LHU) == INSN_MATCH_C_LHU) {
		/* Zcb */
		len = 2;
		imm = RVC_LH_IMM(insn);
		c_load = true;
	} אחרת אם ((insn & INSN_MASK_LW) == INSN_MATCH_LW) {
		len = -4;
	} אחרת אם ((insn & INSN_MASK_C_LW) == INSN_MATCH_C_LW) {
		/* Zca */
		len = -4;
		imm = RVC_LW_IMM(insn);
		c_load = true;
	} אחרת אם ((insn & INSN_MASK_C_LWSP) == INSN_MATCH_C_LWSP &&
		GET_RD_NUM(insn)) {
		/* Zca */
		len = -4;
		imm = RVC_LWSP_IMM(insn);
		c_ldsp = true;
#ifdef __riscv_flen
	} אחרת אם ((insn & INSN_MASK_FLW) == INSN_MATCH_FLW) {
		len = 4;
		fp = true;
	} אחרת אם ((insn & INSN_MASK_FLD) == INSN_MATCH_FLD) {
		len = 8;
		fp = true;
	} אחרת אם ((insn & INSN_MASK_C_FLD) == INSN_MATCH_C_FLD) {
		/* Zcd */
		len = 8;
		imm = RVC_LD_IMM(insn);
		c_load = true;
		fp = true;
	} אחרת אם ((insn & INSN_MASK_C_FLDSP) == INSN_MATCH_C_FLDSP) {
		/* Zcd */
		len = 8;
		imm = RVC_LDSP_IMM(insn);
		c_ldsp = true;
		fp = true;
#endif
	} אחרת {
		prev_xlen = sbi_regs_prev_xlen(regs);
	}

	/**
	 * Must distinguish between rv64 and rv32, RVC instructions have
	 * overlapping encoding:
	 *     c.ld in rv64 == c.flw in rv32
	 *     c.ldsp in rv64 == c.flwsp in rv32
	 */
	אם (prev_xlen == 64) {
		/* RV64 Only: lwu, ld, c.ld, c.ldsp  */
		אם ((insn & INSN_MASK_LWU) == INSN_MATCH_LWU) {
			len = 4;
		} אחרת אם ((insn & INSN_MASK_LD) == INSN_MATCH_LD) {
			len = 8;
		} אחרת אם ((insn & INSN_MASK_C_LD) == INSN_MATCH_C_LD) {
			/* Zca */
			len = 8;
			imm = RVC_LD_IMM(insn);
			c_load = true;
		} אחרת אם ((insn & INSN_MASK_C_LDSP) == INSN_MATCH_C_LDSP &&
			GET_RD_NUM(insn)) {
			/* Zca */
			len = 8;
			imm = RVC_LDSP_IMM(insn);
			c_ldsp = true;
		}
#ifdef __riscv_flen
	} אחרת אם (prev_xlen == 32) {
		/* RV32 Only: c.flw, c.flwsp */
		אם ((insn & INSN_MASK_C_FLW) == INSN_MATCH_C_FLW) {
			/* Zcf */
			len = 4;
			imm = RVC_LW_IMM(insn);
			c_load = true;
			fp = true;
		} אחרת אם ((insn & INSN_MASK_C_FLWSP) == INSN_MATCH_C_FLWSP) {
			/* Zcf */
			len = 4;
			imm = RVC_LWSP_IMM(insn);
			c_ldsp = true;
			fp = true;
		}
#endif
	}

	אם (len < 0) {
		len = -len;
		shift = 8 * (حجم(ulong) - len);
	}

	אם (!len) // Unknown instruction
		اذهب do_emu;

#אם !defined(OPENSBI_DEBUG)
	/**
	 * For misaligned faults. Skip offset calculation unless DEBUG
	 * builds. It helps validating OpenSBI and HW.
	 */
	אם (orig_trap->cause == CAUSE_MISALIGNED_LOAD)
		اذهب do_emu;
#endif

	אם (xform)
		/* Transformed insn */
		off = GET_RS1_NUM(insn);
	אחרת אם (c_load)
		/* non SP-based compressed load */
		off = orig_trap->tval - GET_RS1S(insn, regs) - imm;
	אחרת אם (c_ldsp)
		/* SP-based compressed load */
		off = orig_trap->tval - REG_VAL(2, regs) - imm;
	אחרת
		/* I-type non-compressed load */
		off = orig_trap->tval - GET_RS1(insn, regs) - (ulong)IMM_I(insn);
	/**
	 * Normalize offset, in case the XLEN of unpriv mode is smaller,
	 * and/or pointer masking is in effect
	 */
	off &= (len - 1);

do_emu:
	rc = emu(insn, len, orig_trap->tval - off, &val, tcntx);
	אם (rc <= 0)
		החזר rc;
	אם (!len)
		اذهب epc_fixup;

	אם (!fp) {
		ulong v = ((long)(val.data_ulong << shift)) >> shift;

		אם (c_load)
			SET_RDS(insn, regs, v);
		אחרת
			SET_RD(insn, regs, v);
#ifdef __riscv_flen
	} אחרת אם (len == 8) {
		אם (c_load)
			SET_F64_RDS(insn, regs, val.data_u64);
		אחרת
			SET_F64_RD(insn, regs, val.data_u64);
	} אחרת {
		אם (c_load)
			SET_F32_RDS(insn, regs, val.data_ulong);
		אחרת
			SET_F32_RD(insn, regs, val.data_ulong);
#endif
	}

epc_fixup:
	regs->mepc += insn_len;

	החזר 0;
}

ساكن صحيح sbi_trap_emulate_store(بنية sbi_trap_context *tcntx,
				  sbi_trap_st_emulator emu)
{
	ثابت بنية sbi_trap_info *orig_trap = &tcntx->trap;
	بنية sbi_trap_regs *regs = &tcntx->regs;
	ulong insn, insn_len, imm = 0, off = 0;
	اتحاد sbi_ldst_data val;
	بنية sbi_trap_info uptrap;
	bool xform = false, fp = false, c_store = false, c_stsp = false;
	صحيح rc, len = 0, prev_xlen = 0;

	אם (sbi_trap_tinst_valid(orig_trap->tinst)) {
		xform	 = true;
		insn	 = orig_trap->tinst | INSN_16BIT_MASK;
		insn_len = (orig_trap->tinst & 0x2) ? INSN_LEN(insn) : 2;
	} אחרת {
		/* trapped instruction value is zero or special value */
		insn = sbi_get_insn(regs->mepc, &uptrap);
		אם (uptrap.cause) {
			החזר sbi_trap_redirect(regs, &uptrap);
		}
		insn_len = INSN_LEN(insn);
	}

	/**
	 * Common for RV32/RV64:
	 *    sb, sh, sw, fsw, fsd
	 *    c.sb, c.sh, c.sw, c.swsp, c.fsd, c.fsdsp
	 */
	אם ((insn & INSN_MASK_SB) == INSN_MATCH_SB) {
		len = 1;
	} אחרת אם ((insn & INSN_MASK_C_SB) == INSN_MATCH_C_SB) {
		/* Zcb */
		len = 1;
		imm = RVC_SB_IMM(insn);
		c_store = true;
	} אחרת אם ((insn & INSN_MASK_SH) == INSN_MATCH_SH) {
		len = 2;
	} אחרת אם ((insn & INSN_MASK_C_SH) == INSN_MATCH_C_SH) {
		/* Zcb */
		len = 2;
		imm = RVC_SH_IMM(insn);
		c_store = true;
	} אחרת אם ((insn & INSN_MASK_SW) == INSN_MATCH_SW) {
		len = 4;
	} אחרת אם ((insn & INSN_MASK_C_SW) == INSN_MATCH_C_SW) {
		/* Zca */
		len = 4;
		imm = RVC_SW_IMM(insn);
		c_store = true;
	} אחרת אם ((insn & INSN_MASK_C_SWSP) == INSN_MATCH_C_SWSP) {
		/* Zca */
		len = 4;
		imm = RVC_SWSP_IMM(insn);
		c_stsp = true;
#ifdef __riscv_flen
	} אחרת אם ((insn & INSN_MASK_FSW) == INSN_MATCH_FSW) {
		len = 4;
		fp = true;
	} אחרת אם ((insn & INSN_MASK_FSD) == INSN_MATCH_FSD) {
		len = 8;
		fp = true;
	} אחרת אם ((insn & INSN_MASK_C_FSD) == INSN_MATCH_C_FSD) {
		/* Zcd */
		len = 8;
		imm = RVC_SD_IMM(insn);
		c_store = true;
		fp = true;
	} אחרת אם ((insn & INSN_MASK_C_FSDSP) == INSN_MATCH_C_FSDSP) {
		/* Zcd */
		len = 8;
		imm = RVC_SDSP_IMM(insn);
		c_stsp = true;
		fp = true;
#endif
	} אחרת {
		prev_xlen = sbi_regs_prev_xlen(regs);
	}

	/**
	 * Must distinguish between rv64 and rv32, RVC instructions have
	 * overlapping encoding:
	 *     c.sd in rv64 == c.fsw in rv32
	 *     c.sdsp in rv64 == c.fswsp in rv32
	 */
	אם (prev_xlen == 64) {
		/* RV64 Only: sd, c.sd, c.sdsp */
		אם ((insn & INSN_MASK_SD) == INSN_MATCH_SD) {
			len = 8;
		} אחרת אם ((insn & INSN_MASK_C_SD) == INSN_MATCH_C_SD) {
			/* Zca */
			len = 8;
			imm = RVC_SD_IMM(insn);
			c_store = true;
		} אחרת אם ((insn & INSN_MASK_C_SDSP) == INSN_MATCH_C_SDSP) {
			/* Zca */
			len = 8;
			imm = RVC_SDSP_IMM(insn);
			c_stsp = true;
		}
#ifdef __riscv_flen
	} אחרת אם (prev_xlen == 32) {
		/* RV32 Only: c.fsw, c.fswsp */
		אם ((insn & INSN_MASK_C_FSW) == INSN_MATCH_C_FSW) {
			/* Zcf */
			len = 4;
			imm = RVC_SW_IMM(insn);
			c_store = true;
			fp = true;
		} אחרת אם ((insn & INSN_MASK_C_FSWSP) == INSN_MATCH_C_FSWSP) {
			/* Zcf */
			len = 4;
			imm = RVC_SWSP_IMM(insn);
			c_stsp = true;
			fp = true;
		}
#endif
	}

	אם (!fp) {
		אם (c_store)
			val.data_ulong = GET_RS2S(insn, regs);
		אחרת אם (c_stsp)
			val.data_ulong = GET_RS2C(insn, regs);
		אחרת
			val.data_ulong = GET_RS2(insn, regs);
#ifdef __riscv_flen
	} אחרת אם (len == 8) {
		אם (c_store)
			val.data_u64 = GET_F64_RS2S(insn, regs);
		אחרת אם (c_stsp)
			val.data_u64 = GET_F64_RS2C(insn, regs);
		אחרת
			val.data_u64 = GET_F64_RS2(insn, regs);
	} אחרת {
		אם (c_store)
			val.data_ulong = GET_F32_RS2S(insn, regs);
		אחרת אם (c_stsp)
			val.data_ulong = GET_F32_RS2C(insn, regs);
		אחרת
			val.data_ulong = GET_F32_RS2(insn, regs);
#endif
	}

	אם (!len) // Unknown instruction
		اذهب do_emu;

#אם !defined(OPENSBI_DEBUG)
	/**
	 * For misaligned faults. Skip offset calculation unless DEBUG
	 * builds. It helps validating OpenSBI and HW.
	 */
	אם (orig_trap->cause == CAUSE_MISALIGNED_STORE)
		اذهب do_emu;
#endif

	אם (xform)
		/* Transformed insn */
		off = GET_RS1_NUM(insn);
	אחרת אם (c_store)
		/* non SP-based compressed store */
		off = orig_trap->tval - GET_RS1S(insn, regs) - imm;
	אחרת אם (c_stsp)
		/* SP-based compressed store */
		off = orig_trap->tval - REG_VAL(2, regs) - imm;
	אחרת
		/* S-type non-compressed store */
		off = orig_trap->tval - GET_RS1(insn, regs) - (ulong)IMM_S(insn);
	/**
	 * Normalize offset, in case the XLEN of unpriv mode is smaller,
	 * and/or pointer masking is in effect
	 */
	off &= (len - 1);

do_emu:
	rc = emu(insn, len, orig_trap->tval - off, val, tcntx);
	אם (rc <= 0)
		החזר rc;

	regs->mepc += insn_len;

	החזר 0;
}

ساكن صحيح sbi_misaligned_ld_emulator(ulong insn, صحيح rlen, ulong addr,
				      اتحاد sbi_ldst_data *out_val,
				      بنية sbi_trap_context *tcntx)
{
	ثابت بنية sbi_trap_info *orig_trap = &tcntx->trap;
	بنية sbi_trap_regs *regs = &tcntx->regs;
	بنية sbi_trap_info uptrap;

	אם (!rlen) {
		אם (IS_VECTOR_LOAD_STORE(insn))
			החזר sbi_misaligned_v_ld_emulator(insn, tcntx);
		אחרת
			/* Unrecognized instruction. Can't emulate it. */
			החזר sbi_trap_redirect(regs, orig_trap);
	}
	/* For misaligned fault, addr must be the same as orig_trap->tval */
	אם (addr != orig_trap->tval)
		החזר SBI_EFAIL;

	sbi_load_loop(out_val->data_bytes, addr, rlen, &uptrap);
	אם (uptrap.cause) {
		sbi_misaligned_tinst_fixup(orig_trap, &uptrap);
		החזר sbi_trap_redirect(regs, &uptrap);
	}
	החזר rlen;
}

صحيح sbi_misaligned_load_handler(بنية sbi_trap_context *tcntx)
{
	החזר sbi_trap_emulate_load(tcntx, sbi_misaligned_ld_emulator);
}

ساكن صحيح sbi_misaligned_st_emulator(ulong insn, صحيح wlen, ulong addr,
				      اتحاد sbi_ldst_data in_val,
				      بنية sbi_trap_context *tcntx)
{
	ثابت بنية sbi_trap_info *orig_trap = &tcntx->trap;
	بنية sbi_trap_regs *regs = &tcntx->regs;
	بنية sbi_trap_info uptrap;

	אם (!wlen) {
		אם (IS_VECTOR_LOAD_STORE(insn))
			החזר sbi_misaligned_v_st_emulator(insn, tcntx);
		אחרת
			/* Unrecognized instruction. Can't emulate it. */
			החזר sbi_trap_redirect(regs, orig_trap);
	}
	/* For misaligned fault, addr must be the same as orig_trap->tval */
	אם (addr != orig_trap->tval)
		החזר SBI_EFAIL;

	sbi_store_loop(in_val.data_bytes, addr, wlen, &uptrap);
	אם (uptrap.cause) {
		sbi_misaligned_tinst_fixup(orig_trap, &uptrap);
		החזר sbi_trap_redirect(regs, &uptrap);
	}
	החזר wlen;
}

صحيح sbi_misaligned_store_handler(بنية sbi_trap_context *tcntx)
{
	החזר sbi_trap_emulate_store(tcntx, sbi_misaligned_st_emulator);
}

ساكن صحيح sbi_ld_access_emulator(ulong insn, صحيح rlen, ulong addr,
				  اتحاد sbi_ldst_data *out_val,
				  بنية sbi_trap_context *tcntx)
{
	ثابت بنية sbi_trap_info *orig_trap = &tcntx->trap;
	بنية sbi_trap_regs *regs = &tcntx->regs;
	صحيح rc;

	/* If fault came from M mode, just fail */
	אם (sbi_mstatus_prev_mode(regs->mstatus) == PRV_M)
		החזר SBI_EINVAL;

	rc = sbi_platform_emulate_load(sbi_platform_thishart_ptr(),
				       insn, rlen, addr, out_val, tcntx);

	/* If platform emulator failed, we redirect instead of fail */
	אם (rc < 0)
		החזר sbi_trap_redirect(regs, orig_trap);

	החזר rc;
}

صحيح sbi_load_access_handler(بنية sbi_trap_context *tcntx)
{
	החזר sbi_trap_emulate_load(tcntx, sbi_ld_access_emulator);
}

ساكن صحيح sbi_st_access_emulator(ulong insn, صحيح wlen, ulong addr,
				  اتحاد sbi_ldst_data in_val,
				  بنية sbi_trap_context *tcntx)
{
	ثابت بنية sbi_trap_info *orig_trap = &tcntx->trap;
	بنية sbi_trap_regs *regs = &tcntx->regs;
	صحيح rc;

	/* If fault came from M mode, just fail */
	אם (sbi_mstatus_prev_mode(regs->mstatus) == PRV_M)
		החזר SBI_EINVAL;

	rc = sbi_platform_emulate_store(sbi_platform_thishart_ptr(),
					insn, wlen, addr, in_val, tcntx);

	/* If platform emulator failed, we redirect instead of fail */
	אם (rc < 0)
		החזר sbi_trap_redirect(regs, orig_trap);

	החזר rc;
}

صحيح sbi_store_access_handler(بنية sbi_trap_context *tcntx)
{
	החזר sbi_trap_emulate_store(tcntx, sbi_st_access_emulator);
}
