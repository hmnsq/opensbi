/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/riscv_encoding.h>
#অন্তর্ভাব <sbi/riscv_fp.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_trap_ldst.h>
#অন্তর্ভাব <sbi/sbi_trap.h>
#অন্তর্ভাব <sbi/sbi_unpriv.h>
#অন্তর্ভাব <sbi/sbi_platform.h>

/**
 * Load emulator callback:
 *   Refer to comments of `sbi_platform_emulate_load`.
 */
প্রকারনির্দেশ পূর্ণাঙ্ক (*sbi_trap_ld_emulator)(ulong insn, পূর্ণাঙ্ক rlen, ulong raddr,
				    সঙ্ঘ sbi_ldst_data *out_val,
				    সংরচনা sbi_trap_context *tcntx);

/**
 * Store emulator callback:
 *   Refer to comments of `sbi_platform_emulate_store`.
 */
প্রকারনির্দেশ পূর্ণাঙ্ক (*sbi_trap_st_emulator)(ulong insn, পূর্ণাঙ্ক wlen, ulong waddr,
				    সঙ্ঘ sbi_ldst_data in_val,
				    সংরচনা sbi_trap_context *tcntx);

/**
 * Handling of misaligned fault is done by a collection of smaller, but
 * aligned load/store(s). Another fault (load/store, page fault...) can
 * arise from any of them, then the handling gets aborted. We must fixup
 * the tinst to pretend the fault was rised from the original insn.
 * Specifically, fixup the offset field using the tval diff between the
 * new trap and the original one (if required).
 */
স্থির অন্তঃস্থ শূন্য sbi_misaligned_tinst_fixup(
			ধ্রুব সংরচনা sbi_trap_info *orig_trap,
			      সংরচনা sbi_trap_info *uptrap)
{
	ulong offset = uptrap->tval - orig_trap->tval;

	/*
	 * The function is called in code path for handling a scalar
	 * load/store misaligned fault, thus the new uptrap can't have
	 * custom value of tinst
	 */
	যদি (uptrap->tinst == INSN_PSEUDO_VS_LOAD ||
	    uptrap->tinst == INSN_PSEUDO_VS_STORE)
		/* Use uptrap as-is for guest-page faults */
		প্রত্যাগম;
	/*
	 * Only fixup if orig tinst is valid. Otherwise, discard the
	 * new tinst to be on the safe side. Never use new tinst as-is!
	 * It's load/store width surely mismatches the original width.
	 * For vector, discard it regardless. It doesn't make sense to
	 * have a transformed tinst
	 */
	অন্যথা যদি (orig_trap->tinst == 0)
		uptrap->tinst = 0;
	অন্যথা
		uptrap->tinst = orig_trap->tinst | (offset << SH_RS1);
}

স্থির অন্তঃস্থ bool sbi_trap_tinst_valid(ulong tinst)
{
	/*
	 * Bit[0] == 1 implies trapped instruction value is
	 * transformed instruction or custom instruction.
	 * Also do proper checking per Privileged ISA 19.6.3,
	 * and make sure high 32 bits of tinst is 0
	 */
	প্রত্যাগম tinst == (uint32_t)tinst && (tinst & 0x1);
}

স্থির পূর্ণাঙ্ক sbi_trap_emulate_load(সংরচনা sbi_trap_context *tcntx,
				 sbi_trap_ld_emulator emu)
{
	ধ্রুব সংরচনা sbi_trap_info *orig_trap = &tcntx->trap;
	সংরচনা sbi_trap_regs *regs = &tcntx->regs;
	ulong insn, insn_len, imm = 0, shift = 0, off = 0;
	সঙ্ঘ sbi_ldst_data val = { 0 };
	সংরচনা sbi_trap_info uptrap;
	bool xform = false, fp = false, c_load = false, c_ldsp = false;
	পূর্ণাঙ্ক rc, len = 0, prev_xlen = 0;

	যদি (sbi_trap_tinst_valid(orig_trap->tinst)) {
		xform	 = true;
		insn	 = orig_trap->tinst | INSN_16BIT_MASK;
		insn_len = (orig_trap->tinst & 0x2) ? INSN_LEN(insn) : 2;
	} অন্যথা {
		/* trapped instruction value is zero or special value */
		insn = sbi_get_insn(regs->mepc, &uptrap);
		যদি (uptrap.cause) {
			প্রত্যাগম sbi_trap_redirect(regs, &uptrap);
		}
		insn_len = INSN_LEN(insn);
	}

	/**
	 * Common for RV32/RV64:
	 *    lb, lbu, lh, lhu, lw, flw, flw
	 *    c.lbu, c.lh, c.lhu, c.lw, c.lwsp, c.fld, c.fldsp
	 */
	যদি ((insn & INSN_MASK_LB) == INSN_MATCH_LB) {
		len = -1;
	} অন্যথা যদি ((insn & INSN_MASK_LBU) == INSN_MATCH_LBU) {
		len = 1;
	} অন্যথা যদি ((insn & INSN_MASK_C_LBU) == INSN_MATCH_C_LBU) {
		/* Zcb */
		len = 1;
		imm = RVC_LB_IMM(insn);
		c_load = true;
	} অন্যথা যদি ((insn & INSN_MASK_LH) == INSN_MATCH_LH) {
		len = -2;
	} অন্যথা যদি ((insn & INSN_MASK_C_LH) == INSN_MATCH_C_LH) {
		/* Zcb */
		len = -2;
		imm = RVC_LH_IMM(insn);
		c_load = true;
	} অন্যথা যদি ((insn & INSN_MASK_LHU) == INSN_MATCH_LHU) {
		len = 2;
	} অন্যথা যদি ((insn & INSN_MASK_C_LHU) == INSN_MATCH_C_LHU) {
		/* Zcb */
		len = 2;
		imm = RVC_LH_IMM(insn);
		c_load = true;
	} অন্যথা যদি ((insn & INSN_MASK_LW) == INSN_MATCH_LW) {
		len = -4;
	} অন্যথা যদি ((insn & INSN_MASK_C_LW) == INSN_MATCH_C_LW) {
		/* Zca */
		len = -4;
		imm = RVC_LW_IMM(insn);
		c_load = true;
	} অন্যথা যদি ((insn & INSN_MASK_C_LWSP) == INSN_MATCH_C_LWSP &&
		GET_RD_অঙ্ক(insn)) {
		/* Zca */
		len = -4;
		imm = RVC_LWSP_IMM(insn);
		c_ldsp = true;
#ifdef __riscv_flen
	} অন্যথা যদি ((insn & INSN_MASK_FLW) == INSN_MATCH_FLW) {
		len = 4;
		fp = true;
	} অন্যথা যদি ((insn & INSN_MASK_FLD) == INSN_MATCH_FLD) {
		len = 8;
		fp = true;
	} অন্যথা যদি ((insn & INSN_MASK_C_FLD) == INSN_MATCH_C_FLD) {
		/* Zcd */
		len = 8;
		imm = RVC_LD_IMM(insn);
		c_load = true;
		fp = true;
	} অন্যথা যদি ((insn & INSN_MASK_C_FLDSP) == INSN_MATCH_C_FLDSP) {
		/* Zcd */
		len = 8;
		imm = RVC_LDSP_IMM(insn);
		c_ldsp = true;
		fp = true;
#endif
	} অন্যথা {
		prev_xlen = sbi_regs_prev_xlen(regs);
	}

	/**
	 * Must distinguish between rv64 and rv32, RVC instructions have
	 * overlapping encoding:
	 *     c.ld in rv64 == c.flw in rv32
	 *     c.ldsp in rv64 == c.flwsp in rv32
	 */
	যদি (prev_xlen == 64) {
		/* RV64 Only: lwu, ld, c.ld, c.ldsp  */
		যদি ((insn & INSN_MASK_LWU) == INSN_MATCH_LWU) {
			len = 4;
		} অন্যথা যদি ((insn & INSN_MASK_LD) == INSN_MATCH_LD) {
			len = 8;
		} অন্যথা যদি ((insn & INSN_MASK_C_LD) == INSN_MATCH_C_LD) {
			/* Zca */
			len = 8;
			imm = RVC_LD_IMM(insn);
			c_load = true;
		} অন্যথা যদি ((insn & INSN_MASK_C_LDSP) == INSN_MATCH_C_LDSP &&
			GET_RD_অঙ্ক(insn)) {
			/* Zca */
			len = 8;
			imm = RVC_LDSP_IMM(insn);
			c_ldsp = true;
		}
#ifdef __riscv_flen
	} অন্যথা যদি (prev_xlen == 32) {
		/* RV32 Only: c.flw, c.flwsp */
		যদি ((insn & INSN_MASK_C_FLW) == INSN_MATCH_C_FLW) {
			/* Zcf */
			len = 4;
			imm = RVC_LW_IMM(insn);
			c_load = true;
			fp = true;
		} অন্যথা যদি ((insn & INSN_MASK_C_FLWSP) == INSN_MATCH_C_FLWSP) {
			/* Zcf */
			len = 4;
			imm = RVC_LWSP_IMM(insn);
			c_ldsp = true;
			fp = true;
		}
#endif
	}

	যদি (len < 0) {
		len = -len;
		shift = 8 * (পরিমাণ(ulong) - len);
	}

	যদি (!len) // Unknown instruction
		গচ্ছ do_emu;

#যদি !defined(OPENSBI_DEBUG)
	/**
	 * For misaligned faults. Skip offset calculation unless DEBUG
	 * builds. It helps validating OpenSBI and HW.
	 */
	যদি (orig_trap->cause == CAUSE_MISALIGNED_LOAD)
		গচ্ছ do_emu;
#endif

	যদি (xform)
		/* Transformed insn */
		off = GET_RS1_অঙ্ক(insn);
	অন্যথা যদি (c_load)
		/* non SP-based compressed load */
		off = orig_trap->tval - GET_RS1S(insn, regs) - imm;
	অন্যথা যদি (c_ldsp)
		/* SP-based compressed load */
		off = orig_trap->tval - REG_VAL(2, regs) - imm;
	অন্যথা
		/* I-type non-compressed load */
		off = orig_trap->tval - GET_RS1(insn, regs) - (ulong)IMM_I(insn);
	/**
	 * Normalize offset, in case the XLEN of unpriv mode is smaller,
	 * and/or pointer masking is in effect
	 */
	off &= (len - 1);

do_emu:
	rc = emu(insn, len, orig_trap->tval - off, &val, tcntx);
	যদি (rc <= 0)
		প্রত্যাগম rc;
	যদি (!len)
		গচ্ছ epc_fixup;

	যদি (!fp) {
		ulong v = ((long)(val.data_ulong << shift)) >> shift;

		যদি (c_load)
			SET_RDS(insn, regs, v);
		অন্যথা
			SET_RD(insn, regs, v);
#ifdef __riscv_flen
	} অন্যথা যদি (len == 8) {
		যদি (c_load)
			SET_F64_RDS(insn, regs, val.data_u64);
		অন্যথা
			SET_F64_RD(insn, regs, val.data_u64);
	} অন্যথা {
		যদি (c_load)
			SET_F32_RDS(insn, regs, val.data_ulong);
		অন্যথা
			SET_F32_RD(insn, regs, val.data_ulong);
#endif
	}

epc_fixup:
	regs->mepc += insn_len;

	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক sbi_trap_emulate_store(সংরচনা sbi_trap_context *tcntx,
				  sbi_trap_st_emulator emu)
{
	ধ্রুব সংরচনা sbi_trap_info *orig_trap = &tcntx->trap;
	সংরচনা sbi_trap_regs *regs = &tcntx->regs;
	ulong insn, insn_len, imm = 0, off = 0;
	সঙ্ঘ sbi_ldst_data val;
	সংরচনা sbi_trap_info uptrap;
	bool xform = false, fp = false, c_store = false, c_stsp = false;
	পূর্ণাঙ্ক rc, len = 0, prev_xlen = 0;

	যদি (sbi_trap_tinst_valid(orig_trap->tinst)) {
		xform	 = true;
		insn	 = orig_trap->tinst | INSN_16BIT_MASK;
		insn_len = (orig_trap->tinst & 0x2) ? INSN_LEN(insn) : 2;
	} অন্যথা {
		/* trapped instruction value is zero or special value */
		insn = sbi_get_insn(regs->mepc, &uptrap);
		যদি (uptrap.cause) {
			প্রত্যাগম sbi_trap_redirect(regs, &uptrap);
		}
		insn_len = INSN_LEN(insn);
	}

	/**
	 * Common for RV32/RV64:
	 *    sb, sh, sw, fsw, fsd
	 *    c.sb, c.sh, c.sw, c.swsp, c.fsd, c.fsdsp
	 */
	যদি ((insn & INSN_MASK_SB) == INSN_MATCH_SB) {
		len = 1;
	} অন্যথা যদি ((insn & INSN_MASK_C_SB) == INSN_MATCH_C_SB) {
		/* Zcb */
		len = 1;
		imm = RVC_SB_IMM(insn);
		c_store = true;
	} অন্যথা যদি ((insn & INSN_MASK_SH) == INSN_MATCH_SH) {
		len = 2;
	} অন্যথা যদি ((insn & INSN_MASK_C_SH) == INSN_MATCH_C_SH) {
		/* Zcb */
		len = 2;
		imm = RVC_SH_IMM(insn);
		c_store = true;
	} অন্যথা যদি ((insn & INSN_MASK_SW) == INSN_MATCH_SW) {
		len = 4;
	} অন্যথা যদি ((insn & INSN_MASK_C_SW) == INSN_MATCH_C_SW) {
		/* Zca */
		len = 4;
		imm = RVC_SW_IMM(insn);
		c_store = true;
	} অন্যথা যদি ((insn & INSN_MASK_C_SWSP) == INSN_MATCH_C_SWSP) {
		/* Zca */
		len = 4;
		imm = RVC_SWSP_IMM(insn);
		c_stsp = true;
#ifdef __riscv_flen
	} অন্যথা যদি ((insn & INSN_MASK_FSW) == INSN_MATCH_FSW) {
		len = 4;
		fp = true;
	} অন্যথা যদি ((insn & INSN_MASK_FSD) == INSN_MATCH_FSD) {
		len = 8;
		fp = true;
	} অন্যথা যদি ((insn & INSN_MASK_C_FSD) == INSN_MATCH_C_FSD) {
		/* Zcd */
		len = 8;
		imm = RVC_SD_IMM(insn);
		c_store = true;
		fp = true;
	} অন্যথা যদি ((insn & INSN_MASK_C_FSDSP) == INSN_MATCH_C_FSDSP) {
		/* Zcd */
		len = 8;
		imm = RVC_SDSP_IMM(insn);
		c_stsp = true;
		fp = true;
#endif
	} অন্যথা {
		prev_xlen = sbi_regs_prev_xlen(regs);
	}

	/**
	 * Must distinguish between rv64 and rv32, RVC instructions have
	 * overlapping encoding:
	 *     c.sd in rv64 == c.fsw in rv32
	 *     c.sdsp in rv64 == c.fswsp in rv32
	 */
	যদি (prev_xlen == 64) {
		/* RV64 Only: sd, c.sd, c.sdsp */
		যদি ((insn & INSN_MASK_SD) == INSN_MATCH_SD) {
			len = 8;
		} অন্যথা যদি ((insn & INSN_MASK_C_SD) == INSN_MATCH_C_SD) {
			/* Zca */
			len = 8;
			imm = RVC_SD_IMM(insn);
			c_store = true;
		} অন্যথা যদি ((insn & INSN_MASK_C_SDSP) == INSN_MATCH_C_SDSP) {
			/* Zca */
			len = 8;
			imm = RVC_SDSP_IMM(insn);
			c_stsp = true;
		}
#ifdef __riscv_flen
	} অন্যথা যদি (prev_xlen == 32) {
		/* RV32 Only: c.fsw, c.fswsp */
		যদি ((insn & INSN_MASK_C_FSW) == INSN_MATCH_C_FSW) {
			/* Zcf */
			len = 4;
			imm = RVC_SW_IMM(insn);
			c_store = true;
			fp = true;
		} অন্যথা যদি ((insn & INSN_MASK_C_FSWSP) == INSN_MATCH_C_FSWSP) {
			/* Zcf */
			len = 4;
			imm = RVC_SWSP_IMM(insn);
			c_stsp = true;
			fp = true;
		}
#endif
	}

	যদি (!fp) {
		যদি (c_store)
			val.data_ulong = GET_RS2S(insn, regs);
		অন্যথা যদি (c_stsp)
			val.data_ulong = GET_RS2C(insn, regs);
		অন্যথা
			val.data_ulong = GET_RS2(insn, regs);
#ifdef __riscv_flen
	} অন্যথা যদি (len == 8) {
		যদি (c_store)
			val.data_u64 = GET_F64_RS2S(insn, regs);
		অন্যথা যদি (c_stsp)
			val.data_u64 = GET_F64_RS2C(insn, regs);
		অন্যথা
			val.data_u64 = GET_F64_RS2(insn, regs);
	} অন্যথা {
		যদি (c_store)
			val.data_ulong = GET_F32_RS2S(insn, regs);
		অন্যথা যদি (c_stsp)
			val.data_ulong = GET_F32_RS2C(insn, regs);
		অন্যথা
			val.data_ulong = GET_F32_RS2(insn, regs);
#endif
	}

	যদি (!len) // Unknown instruction
		গচ্ছ do_emu;

#যদি !defined(OPENSBI_DEBUG)
	/**
	 * For misaligned faults. Skip offset calculation unless DEBUG
	 * builds. It helps validating OpenSBI and HW.
	 */
	যদি (orig_trap->cause == CAUSE_MISALIGNED_STORE)
		গচ্ছ do_emu;
#endif

	যদি (xform)
		/* Transformed insn */
		off = GET_RS1_অঙ্ক(insn);
	অন্যথা যদি (c_store)
		/* non SP-based compressed store */
		off = orig_trap->tval - GET_RS1S(insn, regs) - imm;
	অন্যথা যদি (c_stsp)
		/* SP-based compressed store */
		off = orig_trap->tval - REG_VAL(2, regs) - imm;
	অন্যথা
		/* S-type non-compressed store */
		off = orig_trap->tval - GET_RS1(insn, regs) - (ulong)IMM_S(insn);
	/**
	 * Normalize offset, in case the XLEN of unpriv mode is smaller,
	 * and/or pointer masking is in effect
	 */
	off &= (len - 1);

do_emu:
	rc = emu(insn, len, orig_trap->tval - off, val, tcntx);
	যদি (rc <= 0)
		প্রত্যাগম rc;

	regs->mepc += insn_len;

	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক sbi_misaligned_ld_emulator(ulong insn, পূর্ণাঙ্ক rlen, ulong addr,
				      সঙ্ঘ sbi_ldst_data *out_val,
				      সংরচনা sbi_trap_context *tcntx)
{
	ধ্রুব সংরচনা sbi_trap_info *orig_trap = &tcntx->trap;
	সংরচনা sbi_trap_regs *regs = &tcntx->regs;
	সংরচনা sbi_trap_info uptrap;

	যদি (!rlen) {
		যদি (IS_VECTOR_LOAD_STORE(insn))
			প্রত্যাগম sbi_misaligned_v_ld_emulator(insn, tcntx);
		অন্যথা
			/* Unrecognized instruction. Can't emulate it. */
			প্রত্যাগম sbi_trap_redirect(regs, orig_trap);
	}
	/* For misaligned fault, addr must be the same as orig_trap->tval */
	যদি (addr != orig_trap->tval)
		প্রত্যাগম SBI_EFAIL;

	sbi_load_loop(out_val->data_bytes, addr, rlen, &uptrap);
	যদি (uptrap.cause) {
		sbi_misaligned_tinst_fixup(orig_trap, &uptrap);
		প্রত্যাগম sbi_trap_redirect(regs, &uptrap);
	}
	প্রত্যাগম rlen;
}

পূর্ণাঙ্ক sbi_misaligned_load_handler(সংরচনা sbi_trap_context *tcntx)
{
	প্রত্যাগম sbi_trap_emulate_load(tcntx, sbi_misaligned_ld_emulator);
}

স্থির পূর্ণাঙ্ক sbi_misaligned_st_emulator(ulong insn, পূর্ণাঙ্ক wlen, ulong addr,
				      সঙ্ঘ sbi_ldst_data in_val,
				      সংরচনা sbi_trap_context *tcntx)
{
	ধ্রুব সংরচনা sbi_trap_info *orig_trap = &tcntx->trap;
	সংরচনা sbi_trap_regs *regs = &tcntx->regs;
	সংরচনা sbi_trap_info uptrap;

	যদি (!wlen) {
		যদি (IS_VECTOR_LOAD_STORE(insn))
			প্রত্যাগম sbi_misaligned_v_st_emulator(insn, tcntx);
		অন্যথা
			/* Unrecognized instruction. Can't emulate it. */
			প্রত্যাগম sbi_trap_redirect(regs, orig_trap);
	}
	/* For misaligned fault, addr must be the same as orig_trap->tval */
	যদি (addr != orig_trap->tval)
		প্রত্যাগম SBI_EFAIL;

	sbi_store_loop(in_val.data_bytes, addr, wlen, &uptrap);
	যদি (uptrap.cause) {
		sbi_misaligned_tinst_fixup(orig_trap, &uptrap);
		প্রত্যাগম sbi_trap_redirect(regs, &uptrap);
	}
	প্রত্যাগম wlen;
}

পূর্ণাঙ্ক sbi_misaligned_store_handler(সংরচনা sbi_trap_context *tcntx)
{
	প্রত্যাগম sbi_trap_emulate_store(tcntx, sbi_misaligned_st_emulator);
}

স্থির পূর্ণাঙ্ক sbi_ld_access_emulator(ulong insn, পূর্ণাঙ্ক rlen, ulong addr,
				  সঙ্ঘ sbi_ldst_data *out_val,
				  সংরচনা sbi_trap_context *tcntx)
{
	ধ্রুব সংরচনা sbi_trap_info *orig_trap = &tcntx->trap;
	সংরচনা sbi_trap_regs *regs = &tcntx->regs;
	পূর্ণাঙ্ক rc;

	/* If fault came from M mode, just fail */
	যদি (sbi_mstatus_prev_mode(regs->mstatus) == PRV_M)
		প্রত্যাগম SBI_EINVAL;

	rc = sbi_platform_emulate_load(sbi_platform_thishart_ptr(),
				       insn, rlen, addr, out_val, tcntx);

	/* If platform emulator failed, we redirect instead of fail */
	যদি (rc < 0)
		প্রত্যাগম sbi_trap_redirect(regs, orig_trap);

	প্রত্যাগম rc;
}

পূর্ণাঙ্ক sbi_load_access_handler(সংরচনা sbi_trap_context *tcntx)
{
	প্রত্যাগম sbi_trap_emulate_load(tcntx, sbi_ld_access_emulator);
}

স্থির পূর্ণাঙ্ক sbi_st_access_emulator(ulong insn, পূর্ণাঙ্ক wlen, ulong addr,
				  সঙ্ঘ sbi_ldst_data in_val,
				  সংরচনা sbi_trap_context *tcntx)
{
	ধ্রুব সংরচনা sbi_trap_info *orig_trap = &tcntx->trap;
	সংরচনা sbi_trap_regs *regs = &tcntx->regs;
	পূর্ণাঙ্ক rc;

	/* If fault came from M mode, just fail */
	যদি (sbi_mstatus_prev_mode(regs->mstatus) == PRV_M)
		প্রত্যাগম SBI_EINVAL;

	rc = sbi_platform_emulate_store(sbi_platform_thishart_ptr(),
					insn, wlen, addr, in_val, tcntx);

	/* If platform emulator failed, we redirect instead of fail */
	যদি (rc < 0)
		প্রত্যাগম sbi_trap_redirect(regs, orig_trap);

	প্রত্যাগম rc;
}

পূর্ণাঙ্ক sbi_store_access_handler(সংরচনা sbi_trap_context *tcntx)
{
	প্রত্যাগম sbi_trap_emulate_store(tcntx, sbi_st_access_emulator);
}
