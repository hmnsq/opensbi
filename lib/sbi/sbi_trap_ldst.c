/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/riscv_encoding.h>
#अन्तर्भाव <sbi/riscv_fp.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_trap_ldst.h>
#अन्तर्भाव <sbi/sbi_trap.h>
#अन्तर्भाव <sbi/sbi_unpriv.h>
#अन्तर्भाव <sbi/sbi_platform.h>

/**
 * Load emulator callback:
 *   Refer to comments of `sbi_platform_emulate_load`.
 */
प्रकारनिर्देश पूर्णाङ्क (*sbi_trap_ld_emulator)(ulong insn, पूर्णाङ्क rlen, ulong raddr,
				    सङ्घ sbi_ldst_data *out_val,
				    संरचना sbi_trap_context *tcntx);

/**
 * Store emulator callback:
 *   Refer to comments of `sbi_platform_emulate_store`.
 */
प्रकारनिर्देश पूर्णाङ्क (*sbi_trap_st_emulator)(ulong insn, पूर्णाङ्क wlen, ulong waddr,
				    सङ्घ sbi_ldst_data in_val,
				    संरचना sbi_trap_context *tcntx);

/**
 * Handling of misaligned fault is done by a collection of smaller, but
 * aligned load/store(s). Another fault (load/store, page fault...) can
 * arise from any of them, then the handling gets aborted. We must fixup
 * the tinst to pretend the fault was rised from the original insn.
 * Specifically, fixup the offset field using the tval diff between the
 * new trap and the original one (if required).
 */
स्थिर अन्तःस्थ शून्य sbi_misaligned_tinst_fixup(
			ध्रुव संरचना sbi_trap_info *orig_trap,
			      संरचना sbi_trap_info *uptrap)
{
	ulong offset = uptrap->tval - orig_trap->tval;

	/*
	 * The function is called in code path for handling a scalar
	 * load/store misaligned fault, thus the new uptrap can't have
	 * custom value of tinst
	 */
	जइ (uptrap->tinst == INSN_PSEUDO_VS_LOAD ||
	    uptrap->tinst == INSN_PSEUDO_VS_STORE)
		/* Use uptrap as-is for guest-page faults */
		प्रत्यागम;
	/*
	 * Only fixup if orig tinst is valid. Otherwise, discard the
	 * new tinst to be on the safe side. Never use new tinst as-is!
	 * It's load/store width surely mismatches the original width.
	 * For vector, discard it regardless. It doesn't make sense to
	 * have a transformed tinst
	 */
	अण्णहा जइ (orig_trap->tinst == 0)
		uptrap->tinst = 0;
	अण्णहा
		uptrap->tinst = orig_trap->tinst | (offset << SH_RS1);
}

स्थिर अन्तःस्थ bool sbi_trap_tinst_valid(ulong tinst)
{
	/*
	 * Bit[0] == 1 implies trapped instruction value is
	 * transformed instruction or custom instruction.
	 * Also do proper checking per Privileged ISA 19.6.3,
	 * and make sure high 32 bits of tinst is 0
	 */
	प्रत्यागम tinst == (uint32_t)tinst && (tinst & 0x1);
}

स्थिर पूर्णाङ्क sbi_trap_emulate_load(संरचना sbi_trap_context *tcntx,
				 sbi_trap_ld_emulator emu)
{
	ध्रुव संरचना sbi_trap_info *orig_trap = &tcntx->trap;
	संरचना sbi_trap_regs *regs = &tcntx->regs;
	ulong insn, insn_len, imm = 0, shift = 0, off = 0;
	सङ्घ sbi_ldst_data val = { 0 };
	संरचना sbi_trap_info uptrap;
	bool xform = false, fp = false, c_load = false, c_ldsp = false;
	पूर्णाङ्क rc, len = 0, prev_xlen = 0;

	जइ (sbi_trap_tinst_valid(orig_trap->tinst)) {
		xform	 = true;
		insn	 = orig_trap->tinst | INSN_16BIT_MASK;
		insn_len = (orig_trap->tinst & 0x2) ? INSN_LEN(insn) : 2;
	} अण्णहा {
		/* trapped instruction value is zero or special value */
		insn = sbi_get_insn(regs->mepc, &uptrap);
		जइ (uptrap.cause) {
			प्रत्यागम sbi_trap_redirect(regs, &uptrap);
		}
		insn_len = INSN_LEN(insn);
	}

	/**
	 * Common for RV32/RV64:
	 *    lb, lbu, lh, lhu, lw, flw, flw
	 *    c.lbu, c.lh, c.lhu, c.lw, c.lwsp, c.fld, c.fldsp
	 */
	जइ ((insn & INSN_MASK_LB) == INSN_MATCH_LB) {
		len = -1;
	} अण्णहा जइ ((insn & INSN_MASK_LBU) == INSN_MATCH_LBU) {
		len = 1;
	} अण्णहा जइ ((insn & INSN_MASK_C_LBU) == INSN_MATCH_C_LBU) {
		/* Zcb */
		len = 1;
		imm = RVC_LB_IMM(insn);
		c_load = true;
	} अण्णहा जइ ((insn & INSN_MASK_LH) == INSN_MATCH_LH) {
		len = -2;
	} अण्णहा जइ ((insn & INSN_MASK_C_LH) == INSN_MATCH_C_LH) {
		/* Zcb */
		len = -2;
		imm = RVC_LH_IMM(insn);
		c_load = true;
	} अण्णहा जइ ((insn & INSN_MASK_LHU) == INSN_MATCH_LHU) {
		len = 2;
	} अण्णहा जइ ((insn & INSN_MASK_C_LHU) == INSN_MATCH_C_LHU) {
		/* Zcb */
		len = 2;
		imm = RVC_LH_IMM(insn);
		c_load = true;
	} अण्णहा जइ ((insn & INSN_MASK_LW) == INSN_MATCH_LW) {
		len = -4;
	} अण्णहा जइ ((insn & INSN_MASK_C_LW) == INSN_MATCH_C_LW) {
		/* Zca */
		len = -4;
		imm = RVC_LW_IMM(insn);
		c_load = true;
	} अण्णहा जइ ((insn & INSN_MASK_C_LWSP) == INSN_MATCH_C_LWSP &&
		GET_RD_अङ्कः(insn)) {
		/* Zca */
		len = -4;
		imm = RVC_LWSP_IMM(insn);
		c_ldsp = true;
#ifdef __riscv_flen
	} अण्णहा जइ ((insn & INSN_MASK_FLW) == INSN_MATCH_FLW) {
		len = 4;
		fp = true;
	} अण्णहा जइ ((insn & INSN_MASK_FLD) == INSN_MATCH_FLD) {
		len = 8;
		fp = true;
	} अण्णहा जइ ((insn & INSN_MASK_C_FLD) == INSN_MATCH_C_FLD) {
		/* Zcd */
		len = 8;
		imm = RVC_LD_IMM(insn);
		c_load = true;
		fp = true;
	} अण्णहा जइ ((insn & INSN_MASK_C_FLDSP) == INSN_MATCH_C_FLDSP) {
		/* Zcd */
		len = 8;
		imm = RVC_LDSP_IMM(insn);
		c_ldsp = true;
		fp = true;
#endif
	} अण्णहा {
		prev_xlen = sbi_regs_prev_xlen(regs);
	}

	/**
	 * Must distinguish between rv64 and rv32, RVC instructions have
	 * overlapping encoding:
	 *     c.ld in rv64 == c.flw in rv32
	 *     c.ldsp in rv64 == c.flwsp in rv32
	 */
	जइ (prev_xlen == 64) {
		/* RV64 Only: lwu, ld, c.ld, c.ldsp  */
		जइ ((insn & INSN_MASK_LWU) == INSN_MATCH_LWU) {
			len = 4;
		} अण्णहा जइ ((insn & INSN_MASK_LD) == INSN_MATCH_LD) {
			len = 8;
		} अण्णहा जइ ((insn & INSN_MASK_C_LD) == INSN_MATCH_C_LD) {
			/* Zca */
			len = 8;
			imm = RVC_LD_IMM(insn);
			c_load = true;
		} अण्णहा जइ ((insn & INSN_MASK_C_LDSP) == INSN_MATCH_C_LDSP &&
			GET_RD_अङ्कः(insn)) {
			/* Zca */
			len = 8;
			imm = RVC_LDSP_IMM(insn);
			c_ldsp = true;
		}
#ifdef __riscv_flen
	} अण्णहा जइ (prev_xlen == 32) {
		/* RV32 Only: c.flw, c.flwsp */
		जइ ((insn & INSN_MASK_C_FLW) == INSN_MATCH_C_FLW) {
			/* Zcf */
			len = 4;
			imm = RVC_LW_IMM(insn);
			c_load = true;
			fp = true;
		} अण्णहा जइ ((insn & INSN_MASK_C_FLWSP) == INSN_MATCH_C_FLWSP) {
			/* Zcf */
			len = 4;
			imm = RVC_LWSP_IMM(insn);
			c_ldsp = true;
			fp = true;
		}
#endif
	}

	जइ (len < 0) {
		len = -len;
		shift = 8 * (परिमाण(ulong) - len);
	}

	जइ (!len) // Unknown instruction
		गच्छ do_emu;

#जइ !defined(OPENSBI_DEBUG)
	/**
	 * For misaligned faults. Skip offset calculation unless DEBUG
	 * builds. It helps validating OpenSBI and HW.
	 */
	जइ (orig_trap->cause == CAUSE_MISALIGNED_LOAD)
		गच्छ do_emu;
#endif

	जइ (xform)
		/* Transformed insn */
		off = GET_RS1_अङ्कः(insn);
	अण्णहा जइ (c_load)
		/* non SP-based compressed load */
		off = orig_trap->tval - GET_RS1S(insn, regs) - imm;
	अण्णहा जइ (c_ldsp)
		/* SP-based compressed load */
		off = orig_trap->tval - REG_VAL(2, regs) - imm;
	अण्णहा
		/* I-type non-compressed load */
		off = orig_trap->tval - GET_RS1(insn, regs) - (ulong)IMM_I(insn);
	/**
	 * Normalize offset, in case the XLEN of unpriv mode is smaller,
	 * and/or pointer masking is in effect
	 */
	off &= (len - 1);

do_emu:
	rc = emu(insn, len, orig_trap->tval - off, &val, tcntx);
	जइ (rc <= 0)
		प्रत्यागम rc;
	जइ (!len)
		गच्छ epc_fixup;

	जइ (!fp) {
		ulong v = ((long)(val.data_ulong << shift)) >> shift;

		जइ (c_load)
			SET_RDS(insn, regs, v);
		अण्णहा
			SET_RD(insn, regs, v);
#ifdef __riscv_flen
	} अण्णहा जइ (len == 8) {
		जइ (c_load)
			SET_F64_RDS(insn, regs, val.data_u64);
		अण्णहा
			SET_F64_RD(insn, regs, val.data_u64);
	} अण्णहा {
		जइ (c_load)
			SET_F32_RDS(insn, regs, val.data_ulong);
		अण्णहा
			SET_F32_RD(insn, regs, val.data_ulong);
#endif
	}

epc_fixup:
	regs->mepc += insn_len;

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क sbi_trap_emulate_store(संरचना sbi_trap_context *tcntx,
				  sbi_trap_st_emulator emu)
{
	ध्रुव संरचना sbi_trap_info *orig_trap = &tcntx->trap;
	संरचना sbi_trap_regs *regs = &tcntx->regs;
	ulong insn, insn_len, imm = 0, off = 0;
	सङ्घ sbi_ldst_data val;
	संरचना sbi_trap_info uptrap;
	bool xform = false, fp = false, c_store = false, c_stsp = false;
	पूर्णाङ्क rc, len = 0, prev_xlen = 0;

	जइ (sbi_trap_tinst_valid(orig_trap->tinst)) {
		xform	 = true;
		insn	 = orig_trap->tinst | INSN_16BIT_MASK;
		insn_len = (orig_trap->tinst & 0x2) ? INSN_LEN(insn) : 2;
	} अण्णहा {
		/* trapped instruction value is zero or special value */
		insn = sbi_get_insn(regs->mepc, &uptrap);
		जइ (uptrap.cause) {
			प्रत्यागम sbi_trap_redirect(regs, &uptrap);
		}
		insn_len = INSN_LEN(insn);
	}

	/**
	 * Common for RV32/RV64:
	 *    sb, sh, sw, fsw, fsd
	 *    c.sb, c.sh, c.sw, c.swsp, c.fsd, c.fsdsp
	 */
	जइ ((insn & INSN_MASK_SB) == INSN_MATCH_SB) {
		len = 1;
	} अण्णहा जइ ((insn & INSN_MASK_C_SB) == INSN_MATCH_C_SB) {
		/* Zcb */
		len = 1;
		imm = RVC_SB_IMM(insn);
		c_store = true;
	} अण्णहा जइ ((insn & INSN_MASK_SH) == INSN_MATCH_SH) {
		len = 2;
	} अण्णहा जइ ((insn & INSN_MASK_C_SH) == INSN_MATCH_C_SH) {
		/* Zcb */
		len = 2;
		imm = RVC_SH_IMM(insn);
		c_store = true;
	} अण्णहा जइ ((insn & INSN_MASK_SW) == INSN_MATCH_SW) {
		len = 4;
	} अण्णहा जइ ((insn & INSN_MASK_C_SW) == INSN_MATCH_C_SW) {
		/* Zca */
		len = 4;
		imm = RVC_SW_IMM(insn);
		c_store = true;
	} अण्णहा जइ ((insn & INSN_MASK_C_SWSP) == INSN_MATCH_C_SWSP) {
		/* Zca */
		len = 4;
		imm = RVC_SWSP_IMM(insn);
		c_stsp = true;
#ifdef __riscv_flen
	} अण्णहा जइ ((insn & INSN_MASK_FSW) == INSN_MATCH_FSW) {
		len = 4;
		fp = true;
	} अण्णहा जइ ((insn & INSN_MASK_FSD) == INSN_MATCH_FSD) {
		len = 8;
		fp = true;
	} अण्णहा जइ ((insn & INSN_MASK_C_FSD) == INSN_MATCH_C_FSD) {
		/* Zcd */
		len = 8;
		imm = RVC_SD_IMM(insn);
		c_store = true;
		fp = true;
	} अण्णहा जइ ((insn & INSN_MASK_C_FSDSP) == INSN_MATCH_C_FSDSP) {
		/* Zcd */
		len = 8;
		imm = RVC_SDSP_IMM(insn);
		c_stsp = true;
		fp = true;
#endif
	} अण्णहा {
		prev_xlen = sbi_regs_prev_xlen(regs);
	}

	/**
	 * Must distinguish between rv64 and rv32, RVC instructions have
	 * overlapping encoding:
	 *     c.sd in rv64 == c.fsw in rv32
	 *     c.sdsp in rv64 == c.fswsp in rv32
	 */
	जइ (prev_xlen == 64) {
		/* RV64 Only: sd, c.sd, c.sdsp */
		जइ ((insn & INSN_MASK_SD) == INSN_MATCH_SD) {
			len = 8;
		} अण्णहा जइ ((insn & INSN_MASK_C_SD) == INSN_MATCH_C_SD) {
			/* Zca */
			len = 8;
			imm = RVC_SD_IMM(insn);
			c_store = true;
		} अण्णहा जइ ((insn & INSN_MASK_C_SDSP) == INSN_MATCH_C_SDSP) {
			/* Zca */
			len = 8;
			imm = RVC_SDSP_IMM(insn);
			c_stsp = true;
		}
#ifdef __riscv_flen
	} अण्णहा जइ (prev_xlen == 32) {
		/* RV32 Only: c.fsw, c.fswsp */
		जइ ((insn & INSN_MASK_C_FSW) == INSN_MATCH_C_FSW) {
			/* Zcf */
			len = 4;
			imm = RVC_SW_IMM(insn);
			c_store = true;
			fp = true;
		} अण्णहा जइ ((insn & INSN_MASK_C_FSWSP) == INSN_MATCH_C_FSWSP) {
			/* Zcf */
			len = 4;
			imm = RVC_SWSP_IMM(insn);
			c_stsp = true;
			fp = true;
		}
#endif
	}

	जइ (!fp) {
		जइ (c_store)
			val.data_ulong = GET_RS2S(insn, regs);
		अण्णहा जइ (c_stsp)
			val.data_ulong = GET_RS2C(insn, regs);
		अण्णहा
			val.data_ulong = GET_RS2(insn, regs);
#ifdef __riscv_flen
	} अण्णहा जइ (len == 8) {
		जइ (c_store)
			val.data_u64 = GET_F64_RS2S(insn, regs);
		अण्णहा जइ (c_stsp)
			val.data_u64 = GET_F64_RS2C(insn, regs);
		अण्णहा
			val.data_u64 = GET_F64_RS2(insn, regs);
	} अण्णहा {
		जइ (c_store)
			val.data_ulong = GET_F32_RS2S(insn, regs);
		अण्णहा जइ (c_stsp)
			val.data_ulong = GET_F32_RS2C(insn, regs);
		अण्णहा
			val.data_ulong = GET_F32_RS2(insn, regs);
#endif
	}

	जइ (!len) // Unknown instruction
		गच्छ do_emu;

#जइ !defined(OPENSBI_DEBUG)
	/**
	 * For misaligned faults. Skip offset calculation unless DEBUG
	 * builds. It helps validating OpenSBI and HW.
	 */
	जइ (orig_trap->cause == CAUSE_MISALIGNED_STORE)
		गच्छ do_emu;
#endif

	जइ (xform)
		/* Transformed insn */
		off = GET_RS1_अङ्कः(insn);
	अण्णहा जइ (c_store)
		/* non SP-based compressed store */
		off = orig_trap->tval - GET_RS1S(insn, regs) - imm;
	अण्णहा जइ (c_stsp)
		/* SP-based compressed store */
		off = orig_trap->tval - REG_VAL(2, regs) - imm;
	अण्णहा
		/* S-type non-compressed store */
		off = orig_trap->tval - GET_RS1(insn, regs) - (ulong)IMM_S(insn);
	/**
	 * Normalize offset, in case the XLEN of unpriv mode is smaller,
	 * and/or pointer masking is in effect
	 */
	off &= (len - 1);

do_emu:
	rc = emu(insn, len, orig_trap->tval - off, val, tcntx);
	जइ (rc <= 0)
		प्रत्यागम rc;

	regs->mepc += insn_len;

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क sbi_misaligned_ld_emulator(ulong insn, पूर्णाङ्क rlen, ulong addr,
				      सङ्घ sbi_ldst_data *out_val,
				      संरचना sbi_trap_context *tcntx)
{
	ध्रुव संरचना sbi_trap_info *orig_trap = &tcntx->trap;
	संरचना sbi_trap_regs *regs = &tcntx->regs;
	संरचना sbi_trap_info uptrap;

	जइ (!rlen) {
		जइ (IS_VECTOR_LOAD_STORE(insn))
			प्रत्यागम sbi_misaligned_v_ld_emulator(insn, tcntx);
		अण्णहा
			/* Unrecognized instruction. Can't emulate it. */
			प्रत्यागम sbi_trap_redirect(regs, orig_trap);
	}
	/* For misaligned fault, addr must be the same as orig_trap->tval */
	जइ (addr != orig_trap->tval)
		प्रत्यागम SBI_EFAIL;

	sbi_load_loop(out_val->data_bytes, addr, rlen, &uptrap);
	जइ (uptrap.cause) {
		sbi_misaligned_tinst_fixup(orig_trap, &uptrap);
		प्रत्यागम sbi_trap_redirect(regs, &uptrap);
	}
	प्रत्यागम rlen;
}

पूर्णाङ्क sbi_misaligned_load_handler(संरचना sbi_trap_context *tcntx)
{
	प्रत्यागम sbi_trap_emulate_load(tcntx, sbi_misaligned_ld_emulator);
}

स्थिर पूर्णाङ्क sbi_misaligned_st_emulator(ulong insn, पूर्णाङ्क wlen, ulong addr,
				      सङ्घ sbi_ldst_data in_val,
				      संरचना sbi_trap_context *tcntx)
{
	ध्रुव संरचना sbi_trap_info *orig_trap = &tcntx->trap;
	संरचना sbi_trap_regs *regs = &tcntx->regs;
	संरचना sbi_trap_info uptrap;

	जइ (!wlen) {
		जइ (IS_VECTOR_LOAD_STORE(insn))
			प्रत्यागम sbi_misaligned_v_st_emulator(insn, tcntx);
		अण्णहा
			/* Unrecognized instruction. Can't emulate it. */
			प्रत्यागम sbi_trap_redirect(regs, orig_trap);
	}
	/* For misaligned fault, addr must be the same as orig_trap->tval */
	जइ (addr != orig_trap->tval)
		प्रत्यागम SBI_EFAIL;

	sbi_store_loop(in_val.data_bytes, addr, wlen, &uptrap);
	जइ (uptrap.cause) {
		sbi_misaligned_tinst_fixup(orig_trap, &uptrap);
		प्रत्यागम sbi_trap_redirect(regs, &uptrap);
	}
	प्रत्यागम wlen;
}

पूर्णाङ्क sbi_misaligned_store_handler(संरचना sbi_trap_context *tcntx)
{
	प्रत्यागम sbi_trap_emulate_store(tcntx, sbi_misaligned_st_emulator);
}

स्थिर पूर्णाङ्क sbi_ld_access_emulator(ulong insn, पूर्णाङ्क rlen, ulong addr,
				  सङ्घ sbi_ldst_data *out_val,
				  संरचना sbi_trap_context *tcntx)
{
	ध्रुव संरचना sbi_trap_info *orig_trap = &tcntx->trap;
	संरचना sbi_trap_regs *regs = &tcntx->regs;
	पूर्णाङ्क rc;

	/* If fault came from M mode, just fail */
	जइ (sbi_mstatus_prev_mode(regs->mstatus) == PRV_M)
		प्रत्यागम SBI_EINVAL;

	rc = sbi_platform_emulate_load(sbi_platform_thishart_ptr(),
				       insn, rlen, addr, out_val, tcntx);

	/* If platform emulator failed, we redirect instead of fail */
	जइ (rc < 0)
		प्रत्यागम sbi_trap_redirect(regs, orig_trap);

	प्रत्यागम rc;
}

पूर्णाङ्क sbi_load_access_handler(संरचना sbi_trap_context *tcntx)
{
	प्रत्यागम sbi_trap_emulate_load(tcntx, sbi_ld_access_emulator);
}

स्थिर पूर्णाङ्क sbi_st_access_emulator(ulong insn, पूर्णाङ्क wlen, ulong addr,
				  सङ्घ sbi_ldst_data in_val,
				  संरचना sbi_trap_context *tcntx)
{
	ध्रुव संरचना sbi_trap_info *orig_trap = &tcntx->trap;
	संरचना sbi_trap_regs *regs = &tcntx->regs;
	पूर्णाङ्क rc;

	/* If fault came from M mode, just fail */
	जइ (sbi_mstatus_prev_mode(regs->mstatus) == PRV_M)
		प्रत्यागम SBI_EINVAL;

	rc = sbi_platform_emulate_store(sbi_platform_thishart_ptr(),
					insn, wlen, addr, in_val, tcntx);

	/* If platform emulator failed, we redirect instead of fail */
	जइ (rc < 0)
		प्रत्यागम sbi_trap_redirect(regs, orig_trap);

	प्रत्यागम rc;
}

पूर्णाङ्क sbi_store_access_handler(संरचना sbi_trap_context *tcntx)
{
	प्रत्यागम sbi_trap_emulate_store(tcntx, sbi_st_access_emulator);
}
