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
				    सङ्घ sbi_ldst_दत्त *बहिर्गमनम्_val,
				    संरचना sbi_trap_context *tcntx);

/**
 * Store emulator callback:
 *   Refer to comments of `sbi_platform_emulate_store`.
 */
प्रकारनिर्देश पूर्णाङ्क (*sbi_trap_st_emulator)(ulong insn, पूर्णाङ्क wlen, ulong waddr,
				    सङ्घ sbi_ldst_दत्त अन्तर्_val,
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
	ulong विस्थापनम् = uptrap->tval - orig_trap->tval;

	/*
	 * The function is called in code path for handling a scalar
	 * load/store misaligned fault, thus the new uptrap can't have
	 * custom value of tinst
	 */
	यदि (uptrap->tinst == INSN_PSEUDO_VS_आरोपय ||
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
	अन्यथा यदि (orig_trap->tinst == 0)
		uptrap->tinst = 0;
	अन्यथा
		uptrap->tinst = orig_trap->tinst | (विस्थापनम् << SH_RS1);
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

स्थिर पूर्णाङ्क sbi_trap_emulate_आरोपय(संरचना sbi_trap_context *tcntx,
				 sbi_trap_ld_emulator emu)
{
	ध्रुव संरचना sbi_trap_info *orig_trap = &tcntx->trap;
	संरचना sbi_trap_regs *regs = &tcntx->regs;
	ulong insn, insn_len, imm = 0, सरणम् = 0, off = 0;
	सङ्घ sbi_ldst_दत्त val = { 0 };
	संरचना sbi_trap_info uptrap;
	bool xform = false, fp = false, c_आरोपय = false, c_ldsp = false;
	पूर्णाङ्क rc, len = 0, पूर्व_xlen = 0;

	यदि (sbi_trap_tinst_valid(orig_trap->tinst)) {
		xform	 = true;
		insn	 = orig_trap->tinst | INSN_16_BIT_आवरणम्;
		insn_len = (orig_trap->tinst & 0x2) ? INSN_LEN(insn) : 2;
	} अन्यथा {
		/* trapped instruction value is zero or special value */
		insn = sbi_get_insn(regs->mepc, &uptrap);
		यदि (uptrap.cause) {
			प्रत्यागम sbi_trap_redirect(regs, &uptrap);
		}
		insn_len = INSN_LEN(insn);
	}

	/**
	 * Common for RV32/RV64:
	 *    lb, lbu, lh, lhu, lw, flw, flw
	 *    c.lbu, c.lh, c.lhu, c.lw, c.lwsp, c.fld, c.fldsp
	 */
	यदि ((insn & INSN_आवरणम्_LB) == INSN_मेलनम्_LB) {
		len = -1;
	} अन्यथा यदि ((insn & INSN_आवरणम्_LBU) == INSN_मेलनम्_LBU) {
		len = 1;
	} अन्यथा यदि ((insn & INSN_आवरणम्_C_LBU) == INSN_मेलनम्_C_LBU) {
		/* Zcb */
		len = 1;
		imm = RVC_LB_IMM(insn);
		c_आरोपय = true;
	} अन्यथा यदि ((insn & INSN_आवरणम्_LH) == INSN_मेलनम्_LH) {
		len = -2;
	} अन्यथा यदि ((insn & INSN_आवरणम्_C_LH) == INSN_मेलनम्_C_LH) {
		/* Zcb */
		len = -2;
		imm = RVC_LH_IMM(insn);
		c_आरोपय = true;
	} अन्यथा यदि ((insn & INSN_आवरणम्_LHU) == INSN_मेलनम्_LHU) {
		len = 2;
	} अन्यथा यदि ((insn & INSN_आवरणम्_C_LHU) == INSN_मेलनम्_C_LHU) {
		/* Zcb */
		len = 2;
		imm = RVC_LH_IMM(insn);
		c_आरोपय = true;
	} अन्यथा यदि ((insn & INSN_आवरणम्_LW) == INSN_मेलनम्_LW) {
		len = -4;
	} अन्यथा यदि ((insn & INSN_आवरणम्_C_LW) == INSN_मेलनम्_C_LW) {
		/* Zca */
		len = -4;
		imm = RVC_LW_IMM(insn);
		c_आरोपय = true;
	} अन्यथा यदि ((insn & INSN_आवरणम्_C_LWSP) == INSN_मेलनम्_C_LWSP &&
		GET_RD_अङ्कः(insn)) {
		/* Zca */
		len = -4;
		imm = RVC_LWSP_IMM(insn);
		c_ldsp = true;
#यदि_परिभाषितम् __riscv_flen
	} अन्यथा यदि ((insn & INSN_आवरणम्_FLW) == INSN_मेलनम्_FLW) {
		len = 4;
		fp = true;
	} अन्यथा यदि ((insn & INSN_आवरणम्_FLD) == INSN_मेलनम्_FLD) {
		len = 8;
		fp = true;
	} अन्यथा यदि ((insn & INSN_आवरणम्_C_FLD) == INSN_मेलनम्_C_FLD) {
		/* Zcd */
		len = 8;
		imm = RVC_LD_IMM(insn);
		c_आरोपय = true;
		fp = true;
	} अन्यथा यदि ((insn & INSN_आवरणम्_C_FLDSP) == INSN_मेलनम्_C_FLDSP) {
		/* Zcd */
		len = 8;
		imm = RVC_LDSP_IMM(insn);
		c_ldsp = true;
		fp = true;
#यदि_अन्तः
	} अन्यथा {
		पूर्व_xlen = sbi_regs_पूर्व_xlen(regs);
	}

	/**
	 * Must distinguish between rv64 and rv32, RVC instructions have
	 * overlapping encoding:
	 *     c.ld in rv64 == c.flw in rv32
	 *     c.ldsp in rv64 == c.flwsp in rv32
	 */
	यदि (पूर्व_xlen == 64) {
		/* RV64 Only: lwu, ld, c.ld, c.ldsp  */
		यदि ((insn & INSN_आवरणम्_LWU) == INSN_मेलनम्_LWU) {
			len = 4;
		} अन्यथा यदि ((insn & INSN_आवरणम्_LD) == INSN_मेलनम्_LD) {
			len = 8;
		} अन्यथा यदि ((insn & INSN_आवरणम्_C_LD) == INSN_मेलनम्_C_LD) {
			/* Zca */
			len = 8;
			imm = RVC_LD_IMM(insn);
			c_आरोपय = true;
		} अन्यथा यदि ((insn & INSN_आवरणम्_C_LDSP) == INSN_मेलनम्_C_LDSP &&
			GET_RD_अङ्कः(insn)) {
			/* Zca */
			len = 8;
			imm = RVC_LDSP_IMM(insn);
			c_ldsp = true;
		}
#यदि_परिभाषितम् __riscv_flen
	} अन्यथा यदि (पूर्व_xlen == 32) {
		/* RV32 Only: c.flw, c.flwsp */
		यदि ((insn & INSN_आवरणम्_C_FLW) == INSN_मेलनम्_C_FLW) {
			/* Zcf */
			len = 4;
			imm = RVC_LW_IMM(insn);
			c_आरोपय = true;
			fp = true;
		} अन्यथा यदि ((insn & INSN_आवरणम्_C_FLWSP) == INSN_मेलनम्_C_FLWSP) {
			/* Zcf */
			len = 4;
			imm = RVC_LWSP_IMM(insn);
			c_ldsp = true;
			fp = true;
		}
#यदि_अन्तः
	}

	यदि (len < 0) {
		len = -len;
		सरणम् = 8 * (परिमाण(ulong) - len);
	}

	यदि (!len) // Unknown instruction
		गच्छ do_emu;

#यदि !defined(OPENSBI_DEBUG)
	/**
	 * For misaligned faults. Skip offset calculation unless DEBUG
	 * builds. It helps validating OpenSBI and HW.
	 */
	यदि (orig_trap->cause == CAUSE_MISALIGNED_आरोपय)
		गच्छ do_emu;
#यदि_अन्तः

	यदि (xform)
		/* Transformed insn */
		off = GET_RS1_अङ्कः(insn);
	अन्यथा यदि (c_आरोपय)
		/* non SP-based compressed load */
		off = orig_trap->tval - GET_RS1S(insn, regs) - imm;
	अन्यथा यदि (c_ldsp)
		/* SP-based compressed load */
		off = orig_trap->tval - REG_VAL(2, regs) - imm;
	अन्यथा
		/* I-type non-compressed load */
		off = orig_trap->tval - GET_RS1(insn, regs) - (ulong)IMM_I(insn);
	/**
	 * Normalize offset, in case the XLEN of unpriv mode is smaller,
	 * and/or pointer masking is in effect
	 */
	off &= (len - 1);

do_emu:
	rc = emu(insn, len, orig_trap->tval - off, &val, tcntx);
	यदि (rc <= 0)
		प्रत्यागम rc;
	यदि (!len)
		गच्छ epc_fixup;

	यदि (!fp) {
		ulong v = ((दीर्घ)(val.दत्त_ulong << सरणम्)) >> सरणम्;

		यदि (c_आरोपय)
			समुच्चयः_RDS(insn, regs, v);
		अन्यथा
			समुच्चयः_RD(insn, regs, v);
#यदि_परिभाषितम् __riscv_flen
	} अन्यथा यदि (len == 8) {
		यदि (c_आरोपय)
			समुच्चयः_F64_RDS(insn, regs, val.दत्त_u64);
		अन्यथा
			समुच्चयः_F64_RD(insn, regs, val.दत्त_u64);
	} अन्यथा {
		यदि (c_आरोपय)
			समुच्चयः_F32_RDS(insn, regs, val.दत्त_ulong);
		अन्यथा
			समुच्चयः_F32_RD(insn, regs, val.दत्त_ulong);
#यदि_अन्तः
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
	सङ्घ sbi_ldst_दत्त val;
	संरचना sbi_trap_info uptrap;
	bool xform = false, fp = false, c_store = false, c_stsp = false;
	पूर्णाङ्क rc, len = 0, पूर्व_xlen = 0;

	यदि (sbi_trap_tinst_valid(orig_trap->tinst)) {
		xform	 = true;
		insn	 = orig_trap->tinst | INSN_16_BIT_आवरणम्;
		insn_len = (orig_trap->tinst & 0x2) ? INSN_LEN(insn) : 2;
	} अन्यथा {
		/* trapped instruction value is zero or special value */
		insn = sbi_get_insn(regs->mepc, &uptrap);
		यदि (uptrap.cause) {
			प्रत्यागम sbi_trap_redirect(regs, &uptrap);
		}
		insn_len = INSN_LEN(insn);
	}

	/**
	 * Common for RV32/RV64:
	 *    sb, sh, sw, fsw, fsd
	 *    c.sb, c.sh, c.sw, c.swsp, c.fsd, c.fsdsp
	 */
	यदि ((insn & INSN_आवरणम्_SB) == INSN_मेलनम्_SB) {
		len = 1;
	} अन्यथा यदि ((insn & INSN_आवरणम्_C_SB) == INSN_मेलनम्_C_SB) {
		/* Zcb */
		len = 1;
		imm = RVC_SB_IMM(insn);
		c_store = true;
	} अन्यथा यदि ((insn & INSN_आवरणम्_SH) == INSN_मेलनम्_SH) {
		len = 2;
	} अन्यथा यदि ((insn & INSN_आवरणम्_C_SH) == INSN_मेलनम्_C_SH) {
		/* Zcb */
		len = 2;
		imm = RVC_SH_IMM(insn);
		c_store = true;
	} अन्यथा यदि ((insn & INSN_आवरणम्_SW) == INSN_मेलनम्_SW) {
		len = 4;
	} अन्यथा यदि ((insn & INSN_आवरणम्_C_SW) == INSN_मेलनम्_C_SW) {
		/* Zca */
		len = 4;
		imm = RVC_SW_IMM(insn);
		c_store = true;
	} अन्यथा यदि ((insn & INSN_आवरणम्_C_SWSP) == INSN_मेलनम्_C_SWSP) {
		/* Zca */
		len = 4;
		imm = RVC_SWSP_IMM(insn);
		c_stsp = true;
#यदि_परिभाषितम् __riscv_flen
	} अन्यथा यदि ((insn & INSN_आवरणम्_FSW) == INSN_मेलनम्_FSW) {
		len = 4;
		fp = true;
	} अन्यथा यदि ((insn & INSN_आवरणम्_FSD) == INSN_मेलनम्_FSD) {
		len = 8;
		fp = true;
	} अन्यथा यदि ((insn & INSN_आवरणम्_C_FSD) == INSN_मेलनम्_C_FSD) {
		/* Zcd */
		len = 8;
		imm = RVC_SD_IMM(insn);
		c_store = true;
		fp = true;
	} अन्यथा यदि ((insn & INSN_आवरणम्_C_FSDSP) == INSN_मेलनम्_C_FSDSP) {
		/* Zcd */
		len = 8;
		imm = RVC_SDSP_IMM(insn);
		c_stsp = true;
		fp = true;
#यदि_अन्तः
	} अन्यथा {
		पूर्व_xlen = sbi_regs_पूर्व_xlen(regs);
	}

	/**
	 * Must distinguish between rv64 and rv32, RVC instructions have
	 * overlapping encoding:
	 *     c.sd in rv64 == c.fsw in rv32
	 *     c.sdsp in rv64 == c.fswsp in rv32
	 */
	यदि (पूर्व_xlen == 64) {
		/* RV64 Only: sd, c.sd, c.sdsp */
		यदि ((insn & INSN_आवरणम्_SD) == INSN_मेलनम्_SD) {
			len = 8;
		} अन्यथा यदि ((insn & INSN_आवरणम्_C_SD) == INSN_मेलनम्_C_SD) {
			/* Zca */
			len = 8;
			imm = RVC_SD_IMM(insn);
			c_store = true;
		} अन्यथा यदि ((insn & INSN_आवरणम्_C_SDSP) == INSN_मेलनम्_C_SDSP) {
			/* Zca */
			len = 8;
			imm = RVC_SDSP_IMM(insn);
			c_stsp = true;
		}
#यदि_परिभाषितम् __riscv_flen
	} अन्यथा यदि (पूर्व_xlen == 32) {
		/* RV32 Only: c.fsw, c.fswsp */
		यदि ((insn & INSN_आवरणम्_C_FSW) == INSN_मेलनम्_C_FSW) {
			/* Zcf */
			len = 4;
			imm = RVC_SW_IMM(insn);
			c_store = true;
			fp = true;
		} अन्यथा यदि ((insn & INSN_आवरणम्_C_FSWSP) == INSN_मेलनम्_C_FSWSP) {
			/* Zcf */
			len = 4;
			imm = RVC_SWSP_IMM(insn);
			c_stsp = true;
			fp = true;
		}
#यदि_अन्तः
	}

	यदि (!fp) {
		यदि (c_store)
			val.दत्त_ulong = GET_RS2S(insn, regs);
		अन्यथा यदि (c_stsp)
			val.दत्त_ulong = GET_RS2C(insn, regs);
		अन्यथा
			val.दत्त_ulong = GET_RS2(insn, regs);
#यदि_परिभाषितम् __riscv_flen
	} अन्यथा यदि (len == 8) {
		यदि (c_store)
			val.दत्त_u64 = GET_F64_RS2S(insn, regs);
		अन्यथा यदि (c_stsp)
			val.दत्त_u64 = GET_F64_RS2C(insn, regs);
		अन्यथा
			val.दत्त_u64 = GET_F64_RS2(insn, regs);
	} अन्यथा {
		यदि (c_store)
			val.दत्त_ulong = GET_F32_RS2S(insn, regs);
		अन्यथा यदि (c_stsp)
			val.दत्त_ulong = GET_F32_RS2C(insn, regs);
		अन्यथा
			val.दत्त_ulong = GET_F32_RS2(insn, regs);
#यदि_अन्तः
	}

	यदि (!len) // Unknown instruction
		गच्छ do_emu;

#यदि !defined(OPENSBI_DEBUG)
	/**
	 * For misaligned faults. Skip offset calculation unless DEBUG
	 * builds. It helps validating OpenSBI and HW.
	 */
	यदि (orig_trap->cause == CAUSE_MISALIGNED_STORE)
		गच्छ do_emu;
#यदि_अन्तः

	यदि (xform)
		/* Transformed insn */
		off = GET_RS1_अङ्कः(insn);
	अन्यथा यदि (c_store)
		/* non SP-based compressed store */
		off = orig_trap->tval - GET_RS1S(insn, regs) - imm;
	अन्यथा यदि (c_stsp)
		/* SP-based compressed store */
		off = orig_trap->tval - REG_VAL(2, regs) - imm;
	अन्यथा
		/* S-type non-compressed store */
		off = orig_trap->tval - GET_RS1(insn, regs) - (ulong)IMM_S(insn);
	/**
	 * Normalize offset, in case the XLEN of unpriv mode is smaller,
	 * and/or pointer masking is in effect
	 */
	off &= (len - 1);

do_emu:
	rc = emu(insn, len, orig_trap->tval - off, val, tcntx);
	यदि (rc <= 0)
		प्रत्यागम rc;

	regs->mepc += insn_len;

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क sbi_misaligned_ld_emulator(ulong insn, पूर्णाङ्क rlen, ulong addr,
				      सङ्घ sbi_ldst_दत्त *बहिर्गमनम्_val,
				      संरचना sbi_trap_context *tcntx)
{
	ध्रुव संरचना sbi_trap_info *orig_trap = &tcntx->trap;
	संरचना sbi_trap_regs *regs = &tcntx->regs;
	संरचना sbi_trap_info uptrap;

	यदि (!rlen) {
		यदि (IS_सदिशम्_आरोपय_STORE(insn))
			प्रत्यागम sbi_misaligned_v_ld_emulator(insn, tcntx);
		अन्यथा
			/* Unrecognized instruction. Can't emulate it. */
			प्रत्यागम sbi_trap_redirect(regs, orig_trap);
	}
	/* For misaligned fault, addr must be the same as orig_trap->tval */
	यदि (addr != orig_trap->tval)
		प्रत्यागम SBI_EFAIL;

	sbi_आरोपय_loop(बहिर्गमनम्_val->दत्त_अष्टकानि, addr, rlen, &uptrap);
	यदि (uptrap.cause) {
		sbi_misaligned_tinst_fixup(orig_trap, &uptrap);
		प्रत्यागम sbi_trap_redirect(regs, &uptrap);
	}
	प्रत्यागम rlen;
}

पूर्णाङ्क sbi_misaligned_आरोपय_handler(संरचना sbi_trap_context *tcntx)
{
	प्रत्यागम sbi_trap_emulate_आरोपय(tcntx, sbi_misaligned_ld_emulator);
}

स्थिर पूर्णाङ्क sbi_misaligned_st_emulator(ulong insn, पूर्णाङ्क wlen, ulong addr,
				      सङ्घ sbi_ldst_दत्त अन्तर्_val,
				      संरचना sbi_trap_context *tcntx)
{
	ध्रुव संरचना sbi_trap_info *orig_trap = &tcntx->trap;
	संरचना sbi_trap_regs *regs = &tcntx->regs;
	संरचना sbi_trap_info uptrap;

	यदि (!wlen) {
		यदि (IS_सदिशम्_आरोपय_STORE(insn))
			प्रत्यागम sbi_misaligned_v_st_emulator(insn, tcntx);
		अन्यथा
			/* Unrecognized instruction. Can't emulate it. */
			प्रत्यागम sbi_trap_redirect(regs, orig_trap);
	}
	/* For misaligned fault, addr must be the same as orig_trap->tval */
	यदि (addr != orig_trap->tval)
		प्रत्यागम SBI_EFAIL;

	sbi_store_loop(अन्तर्_val.दत्त_अष्टकानि, addr, wlen, &uptrap);
	यदि (uptrap.cause) {
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
				  सङ्घ sbi_ldst_दत्त *बहिर्गमनम्_val,
				  संरचना sbi_trap_context *tcntx)
{
	ध्रुव संरचना sbi_trap_info *orig_trap = &tcntx->trap;
	संरचना sbi_trap_regs *regs = &tcntx->regs;
	पूर्णाङ्क rc;

	/* If fault came from M mode, just fail */
	यदि (sbi_mstatus_पूर्व_बहुलकम्(regs->mstatus) == PRV_M)
		प्रत्यागम SBI_EINVAL;

	rc = sbi_platform_emulate_आरोपय(sbi_platform_thishart_ptr(),
				       insn, rlen, addr, बहिर्गमनम्_val, tcntx);

	/* If platform emulator failed, we redirect instead of fail */
	यदि (rc < 0)
		प्रत्यागम sbi_trap_redirect(regs, orig_trap);

	प्रत्यागम rc;
}

पूर्णाङ्क sbi_आरोपय_access_handler(संरचना sbi_trap_context *tcntx)
{
	प्रत्यागम sbi_trap_emulate_आरोपय(tcntx, sbi_ld_access_emulator);
}

स्थिर पूर्णाङ्क sbi_st_access_emulator(ulong insn, पूर्णाङ्क wlen, ulong addr,
				  सङ्घ sbi_ldst_दत्त अन्तर्_val,
				  संरचना sbi_trap_context *tcntx)
{
	ध्रुव संरचना sbi_trap_info *orig_trap = &tcntx->trap;
	संरचना sbi_trap_regs *regs = &tcntx->regs;
	पूर्णाङ्क rc;

	/* If fault came from M mode, just fail */
	यदि (sbi_mstatus_पूर्व_बहुलकम्(regs->mstatus) == PRV_M)
		प्रत्यागम SBI_EINVAL;

	rc = sbi_platform_emulate_store(sbi_platform_thishart_ptr(),
					insn, wlen, addr, अन्तर्_val, tcntx);

	/* If platform emulator failed, we redirect instead of fail */
	यदि (rc < 0)
		प्रत्यागम sbi_trap_redirect(regs, orig_trap);

	प्रत्यागम rc;
}

पूर्णाङ्क sbi_store_access_handler(संरचना sbi_trap_context *tcntx)
{
	प्रत्यागम sbi_trap_emulate_store(tcntx, sbi_st_access_emulator);
}
