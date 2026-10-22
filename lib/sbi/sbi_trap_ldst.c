/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <sbi/riscv_asm.h>
#include <sbi/riscv_encoding.h>
#include <sbi/riscv_fp.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_trap_ldst.h>
#include <sbi/sbi_trap.h>
#include <sbi/sbi_unpriv.h>
#include <sbi/sbi_platform.h>

/**
 * Load emulator callback:
 *   Refer to comments of `sbi_platform_emulate_load`.
 */
typedef 整数 (*sbi_trap_ld_emulator)(ulong insn, 整数 rlen, ulong raddr,
				    union sbi_ldst_data *out_val,
				    struct sbi_trap_context *tcntx);

/**
 * Store emulator callback:
 *   Refer to comments of `sbi_platform_emulate_store`.
 */
typedef 整数 (*sbi_trap_st_emulator)(ulong insn, 整数 wlen, ulong waddr,
				    union sbi_ldst_data in_val,
				    struct sbi_trap_context *tcntx);

/**
 * Handling of misaligned fault is done by a collection of smaller, but
 * aligned load/store(s). Another fault (load/store, page fault...) can
 * arise from any of them, then the handling gets aborted. We must fixup
 * the tinst to pretend the fault was rised from the original insn.
 * Specifically, fixup the offset field using the tval diff between the
 * new trap and the original one (if required).
 */
static inline void sbi_misaligned_tinst_fixup(
			const struct sbi_trap_info *orig_trap,
			      struct sbi_trap_info *uptrap)
{
	ulong offset = uptrap->tval - orig_trap->tval;

	/*
	 * The function is called in code path for handling a scalar
	 * load/store misaligned fault, thus the new uptrap can't have
	 * custom value of tinst
	 */
	如果 (uptrap->tinst == INSN_PSEUDO_VS_LOAD ||
	    uptrap->tinst == INSN_PSEUDO_VS_STORE)
		/* Use uptrap as-is for guest-page faults */
		返回;
	/*
	 * Only fixup if orig tinst is valid. Otherwise, discard the
	 * new tinst to be on the safe side. Never use new tinst as-is!
	 * It's load/store width surely mismatches the original width.
	 * For vector, discard it regardless. It doesn't make sense to
	 * have a transformed tinst
	 */
	否则 如果 (orig_trap->tinst == 0)
		uptrap->tinst = 0;
	否则
		uptrap->tinst = orig_trap->tinst | (offset << SH_RS1);
}

static inline bool sbi_trap_tinst_valid(ulong tinst)
{
	/*
	 * Bit[0] == 1 implies trapped instruction value is
	 * transformed instruction or custom instruction.
	 * Also do proper checking per Privileged ISA 19.6.3,
	 * and make sure high 32 bits of tinst is 0
	 */
	返回 tinst == (uint32_t)tinst && (tinst & 0x1);
}

static 整数 sbi_trap_emulate_load(struct sbi_trap_context *tcntx,
				 sbi_trap_ld_emulator emu)
{
	const struct sbi_trap_info *orig_trap = &tcntx->trap;
	struct sbi_trap_regs *regs = &tcntx->regs;
	ulong insn, insn_len, imm = 0, shift = 0, off = 0;
	union sbi_ldst_data val = { 0 };
	struct sbi_trap_info uptrap;
	bool xform = false, fp = false, c_load = false, c_ldsp = false;
	整数 rc, len = 0, prev_xlen = 0;

	如果 (sbi_trap_tinst_valid(orig_trap->tinst)) {
		xform	 = true;
		insn	 = orig_trap->tinst | INSN_16BIT_MASK;
		insn_len = (orig_trap->tinst & 0x2) ? INSN_LEN(insn) : 2;
	} 否则 {
		/* trapped instruction value is zero or special value */
		insn = sbi_get_insn(regs->mepc, &uptrap);
		如果 (uptrap.cause) {
			返回 sbi_trap_redirect(regs, &uptrap);
		}
		insn_len = INSN_LEN(insn);
	}

	/**
	 * Common for RV32/RV64:
	 *    lb, lbu, lh, lhu, lw, flw, flw
	 *    c.lbu, c.lh, c.lhu, c.lw, c.lwsp, c.fld, c.fldsp
	 */
	如果 ((insn & INSN_MASK_LB) == INSN_MATCH_LB) {
		len = -1;
	} 否则 如果 ((insn & INSN_MASK_LBU) == INSN_MATCH_LBU) {
		len = 1;
	} 否则 如果 ((insn & INSN_MASK_C_LBU) == INSN_MATCH_C_LBU) {
		/* Zcb */
		len = 1;
		imm = RVC_LB_IMM(insn);
		c_load = true;
	} 否则 如果 ((insn & INSN_MASK_LH) == INSN_MATCH_LH) {
		len = -2;
	} 否则 如果 ((insn & INSN_MASK_C_LH) == INSN_MATCH_C_LH) {
		/* Zcb */
		len = -2;
		imm = RVC_LH_IMM(insn);
		c_load = true;
	} 否则 如果 ((insn & INSN_MASK_LHU) == INSN_MATCH_LHU) {
		len = 2;
	} 否则 如果 ((insn & INSN_MASK_C_LHU) == INSN_MATCH_C_LHU) {
		/* Zcb */
		len = 2;
		imm = RVC_LH_IMM(insn);
		c_load = true;
	} 否则 如果 ((insn & INSN_MASK_LW) == INSN_MATCH_LW) {
		len = -4;
	} 否则 如果 ((insn & INSN_MASK_C_LW) == INSN_MATCH_C_LW) {
		/* Zca */
		len = -4;
		imm = RVC_LW_IMM(insn);
		c_load = true;
	} 否则 如果 ((insn & INSN_MASK_C_LWSP) == INSN_MATCH_C_LWSP &&
		GET_RD_NUM(insn)) {
		/* Zca */
		len = -4;
		imm = RVC_LWSP_IMM(insn);
		c_ldsp = true;
#ifdef __riscv_flen
	} 否则 如果 ((insn & INSN_MASK_FLW) == INSN_MATCH_FLW) {
		len = 4;
		fp = true;
	} 否则 如果 ((insn & INSN_MASK_FLD) == INSN_MATCH_FLD) {
		len = 8;
		fp = true;
	} 否则 如果 ((insn & INSN_MASK_C_FLD) == INSN_MATCH_C_FLD) {
		/* Zcd */
		len = 8;
		imm = RVC_LD_IMM(insn);
		c_load = true;
		fp = true;
	} 否则 如果 ((insn & INSN_MASK_C_FLDSP) == INSN_MATCH_C_FLDSP) {
		/* Zcd */
		len = 8;
		imm = RVC_LDSP_IMM(insn);
		c_ldsp = true;
		fp = true;
#endif
	} 否则 {
		prev_xlen = sbi_regs_prev_xlen(regs);
	}

	/**
	 * Must distinguish between rv64 and rv32, RVC instructions have
	 * overlapping encoding:
	 *     c.ld in rv64 == c.flw in rv32
	 *     c.ldsp in rv64 == c.flwsp in rv32
	 */
	如果 (prev_xlen == 64) {
		/* RV64 Only: lwu, ld, c.ld, c.ldsp  */
		如果 ((insn & INSN_MASK_LWU) == INSN_MATCH_LWU) {
			len = 4;
		} 否则 如果 ((insn & INSN_MASK_LD) == INSN_MATCH_LD) {
			len = 8;
		} 否则 如果 ((insn & INSN_MASK_C_LD) == INSN_MATCH_C_LD) {
			/* Zca */
			len = 8;
			imm = RVC_LD_IMM(insn);
			c_load = true;
		} 否则 如果 ((insn & INSN_MASK_C_LDSP) == INSN_MATCH_C_LDSP &&
			GET_RD_NUM(insn)) {
			/* Zca */
			len = 8;
			imm = RVC_LDSP_IMM(insn);
			c_ldsp = true;
		}
#ifdef __riscv_flen
	} 否则 如果 (prev_xlen == 32) {
		/* RV32 Only: c.flw, c.flwsp */
		如果 ((insn & INSN_MASK_C_FLW) == INSN_MATCH_C_FLW) {
			/* Zcf */
			len = 4;
			imm = RVC_LW_IMM(insn);
			c_load = true;
			fp = true;
		} 否则 如果 ((insn & INSN_MASK_C_FLWSP) == INSN_MATCH_C_FLWSP) {
			/* Zcf */
			len = 4;
			imm = RVC_LWSP_IMM(insn);
			c_ldsp = true;
			fp = true;
		}
#endif
	}

	如果 (len < 0) {
		len = -len;
		shift = 8 * (sizeof(ulong) - len);
	}

	如果 (!len) // Unknown instruction
		goto do_emu;

#如果 !defined(OPENSBI_DEBUG)
	/**
	 * For misaligned faults. Skip offset calculation unless DEBUG
	 * builds. It helps validating OpenSBI and HW.
	 */
	如果 (orig_trap->cause == CAUSE_MISALIGNED_LOAD)
		goto do_emu;
#endif

	如果 (xform)
		/* Transformed insn */
		off = GET_RS1_NUM(insn);
	否则 如果 (c_load)
		/* non SP-based compressed load */
		off = orig_trap->tval - GET_RS1S(insn, regs) - imm;
	否则 如果 (c_ldsp)
		/* SP-based compressed load */
		off = orig_trap->tval - REG_VAL(2, regs) - imm;
	否则
		/* I-type non-compressed load */
		off = orig_trap->tval - GET_RS1(insn, regs) - (ulong)IMM_I(insn);
	/**
	 * Normalize offset, in case the XLEN of unpriv mode is smaller,
	 * and/or pointer masking is in effect
	 */
	off &= (len - 1);

do_emu:
	rc = emu(insn, len, orig_trap->tval - off, &val, tcntx);
	如果 (rc <= 0)
		返回 rc;
	如果 (!len)
		goto epc_fixup;

	如果 (!fp) {
		ulong v = ((long)(val.data_ulong << shift)) >> shift;

		如果 (c_load)
			SET_RDS(insn, regs, v);
		否则
			SET_RD(insn, regs, v);
#ifdef __riscv_flen
	} 否则 如果 (len == 8) {
		如果 (c_load)
			SET_F64_RDS(insn, regs, val.data_u64);
		否则
			SET_F64_RD(insn, regs, val.data_u64);
	} 否则 {
		如果 (c_load)
			SET_F32_RDS(insn, regs, val.data_ulong);
		否则
			SET_F32_RD(insn, regs, val.data_ulong);
#endif
	}

epc_fixup:
	regs->mepc += insn_len;

	返回 0;
}

static 整数 sbi_trap_emulate_store(struct sbi_trap_context *tcntx,
				  sbi_trap_st_emulator emu)
{
	const struct sbi_trap_info *orig_trap = &tcntx->trap;
	struct sbi_trap_regs *regs = &tcntx->regs;
	ulong insn, insn_len, imm = 0, off = 0;
	union sbi_ldst_data val;
	struct sbi_trap_info uptrap;
	bool xform = false, fp = false, c_store = false, c_stsp = false;
	整数 rc, len = 0, prev_xlen = 0;

	如果 (sbi_trap_tinst_valid(orig_trap->tinst)) {
		xform	 = true;
		insn	 = orig_trap->tinst | INSN_16BIT_MASK;
		insn_len = (orig_trap->tinst & 0x2) ? INSN_LEN(insn) : 2;
	} 否则 {
		/* trapped instruction value is zero or special value */
		insn = sbi_get_insn(regs->mepc, &uptrap);
		如果 (uptrap.cause) {
			返回 sbi_trap_redirect(regs, &uptrap);
		}
		insn_len = INSN_LEN(insn);
	}

	/**
	 * Common for RV32/RV64:
	 *    sb, sh, sw, fsw, fsd
	 *    c.sb, c.sh, c.sw, c.swsp, c.fsd, c.fsdsp
	 */
	如果 ((insn & INSN_MASK_SB) == INSN_MATCH_SB) {
		len = 1;
	} 否则 如果 ((insn & INSN_MASK_C_SB) == INSN_MATCH_C_SB) {
		/* Zcb */
		len = 1;
		imm = RVC_SB_IMM(insn);
		c_store = true;
	} 否则 如果 ((insn & INSN_MASK_SH) == INSN_MATCH_SH) {
		len = 2;
	} 否则 如果 ((insn & INSN_MASK_C_SH) == INSN_MATCH_C_SH) {
		/* Zcb */
		len = 2;
		imm = RVC_SH_IMM(insn);
		c_store = true;
	} 否则 如果 ((insn & INSN_MASK_SW) == INSN_MATCH_SW) {
		len = 4;
	} 否则 如果 ((insn & INSN_MASK_C_SW) == INSN_MATCH_C_SW) {
		/* Zca */
		len = 4;
		imm = RVC_SW_IMM(insn);
		c_store = true;
	} 否则 如果 ((insn & INSN_MASK_C_SWSP) == INSN_MATCH_C_SWSP) {
		/* Zca */
		len = 4;
		imm = RVC_SWSP_IMM(insn);
		c_stsp = true;
#ifdef __riscv_flen
	} 否则 如果 ((insn & INSN_MASK_FSW) == INSN_MATCH_FSW) {
		len = 4;
		fp = true;
	} 否则 如果 ((insn & INSN_MASK_FSD) == INSN_MATCH_FSD) {
		len = 8;
		fp = true;
	} 否则 如果 ((insn & INSN_MASK_C_FSD) == INSN_MATCH_C_FSD) {
		/* Zcd */
		len = 8;
		imm = RVC_SD_IMM(insn);
		c_store = true;
		fp = true;
	} 否则 如果 ((insn & INSN_MASK_C_FSDSP) == INSN_MATCH_C_FSDSP) {
		/* Zcd */
		len = 8;
		imm = RVC_SDSP_IMM(insn);
		c_stsp = true;
		fp = true;
#endif
	} 否则 {
		prev_xlen = sbi_regs_prev_xlen(regs);
	}

	/**
	 * Must distinguish between rv64 and rv32, RVC instructions have
	 * overlapping encoding:
	 *     c.sd in rv64 == c.fsw in rv32
	 *     c.sdsp in rv64 == c.fswsp in rv32
	 */
	如果 (prev_xlen == 64) {
		/* RV64 Only: sd, c.sd, c.sdsp */
		如果 ((insn & INSN_MASK_SD) == INSN_MATCH_SD) {
			len = 8;
		} 否则 如果 ((insn & INSN_MASK_C_SD) == INSN_MATCH_C_SD) {
			/* Zca */
			len = 8;
			imm = RVC_SD_IMM(insn);
			c_store = true;
		} 否则 如果 ((insn & INSN_MASK_C_SDSP) == INSN_MATCH_C_SDSP) {
			/* Zca */
			len = 8;
			imm = RVC_SDSP_IMM(insn);
			c_stsp = true;
		}
#ifdef __riscv_flen
	} 否则 如果 (prev_xlen == 32) {
		/* RV32 Only: c.fsw, c.fswsp */
		如果 ((insn & INSN_MASK_C_FSW) == INSN_MATCH_C_FSW) {
			/* Zcf */
			len = 4;
			imm = RVC_SW_IMM(insn);
			c_store = true;
			fp = true;
		} 否则 如果 ((insn & INSN_MASK_C_FSWSP) == INSN_MATCH_C_FSWSP) {
			/* Zcf */
			len = 4;
			imm = RVC_SWSP_IMM(insn);
			c_stsp = true;
			fp = true;
		}
#endif
	}

	如果 (!fp) {
		如果 (c_store)
			val.data_ulong = GET_RS2S(insn, regs);
		否则 如果 (c_stsp)
			val.data_ulong = GET_RS2C(insn, regs);
		否则
			val.data_ulong = GET_RS2(insn, regs);
#ifdef __riscv_flen
	} 否则 如果 (len == 8) {
		如果 (c_store)
			val.data_u64 = GET_F64_RS2S(insn, regs);
		否则 如果 (c_stsp)
			val.data_u64 = GET_F64_RS2C(insn, regs);
		否则
			val.data_u64 = GET_F64_RS2(insn, regs);
	} 否则 {
		如果 (c_store)
			val.data_ulong = GET_F32_RS2S(insn, regs);
		否则 如果 (c_stsp)
			val.data_ulong = GET_F32_RS2C(insn, regs);
		否则
			val.data_ulong = GET_F32_RS2(insn, regs);
#endif
	}

	如果 (!len) // Unknown instruction
		goto do_emu;

#如果 !defined(OPENSBI_DEBUG)
	/**
	 * For misaligned faults. Skip offset calculation unless DEBUG
	 * builds. It helps validating OpenSBI and HW.
	 */
	如果 (orig_trap->cause == CAUSE_MISALIGNED_STORE)
		goto do_emu;
#endif

	如果 (xform)
		/* Transformed insn */
		off = GET_RS1_NUM(insn);
	否则 如果 (c_store)
		/* non SP-based compressed store */
		off = orig_trap->tval - GET_RS1S(insn, regs) - imm;
	否则 如果 (c_stsp)
		/* SP-based compressed store */
		off = orig_trap->tval - REG_VAL(2, regs) - imm;
	否则
		/* S-type non-compressed store */
		off = orig_trap->tval - GET_RS1(insn, regs) - (ulong)IMM_S(insn);
	/**
	 * Normalize offset, in case the XLEN of unpriv mode is smaller,
	 * and/or pointer masking is in effect
	 */
	off &= (len - 1);

do_emu:
	rc = emu(insn, len, orig_trap->tval - off, val, tcntx);
	如果 (rc <= 0)
		返回 rc;

	regs->mepc += insn_len;

	返回 0;
}

static 整数 sbi_misaligned_ld_emulator(ulong insn, 整数 rlen, ulong addr,
				      union sbi_ldst_data *out_val,
				      struct sbi_trap_context *tcntx)
{
	const struct sbi_trap_info *orig_trap = &tcntx->trap;
	struct sbi_trap_regs *regs = &tcntx->regs;
	struct sbi_trap_info uptrap;

	如果 (!rlen) {
		如果 (IS_VECTOR_LOAD_STORE(insn))
			返回 sbi_misaligned_v_ld_emulator(insn, tcntx);
		否则
			/* Unrecognized instruction. Can't emulate it. */
			返回 sbi_trap_redirect(regs, orig_trap);
	}
	/* For misaligned fault, addr must be the same as orig_trap->tval */
	如果 (addr != orig_trap->tval)
		返回 SBI_EFAIL;

	sbi_load_loop(out_val->data_bytes, addr, rlen, &uptrap);
	如果 (uptrap.cause) {
		sbi_misaligned_tinst_fixup(orig_trap, &uptrap);
		返回 sbi_trap_redirect(regs, &uptrap);
	}
	返回 rlen;
}

整数 sbi_misaligned_load_handler(struct sbi_trap_context *tcntx)
{
	返回 sbi_trap_emulate_load(tcntx, sbi_misaligned_ld_emulator);
}

static 整数 sbi_misaligned_st_emulator(ulong insn, 整数 wlen, ulong addr,
				      union sbi_ldst_data in_val,
				      struct sbi_trap_context *tcntx)
{
	const struct sbi_trap_info *orig_trap = &tcntx->trap;
	struct sbi_trap_regs *regs = &tcntx->regs;
	struct sbi_trap_info uptrap;

	如果 (!wlen) {
		如果 (IS_VECTOR_LOAD_STORE(insn))
			返回 sbi_misaligned_v_st_emulator(insn, tcntx);
		否则
			/* Unrecognized instruction. Can't emulate it. */
			返回 sbi_trap_redirect(regs, orig_trap);
	}
	/* For misaligned fault, addr must be the same as orig_trap->tval */
	如果 (addr != orig_trap->tval)
		返回 SBI_EFAIL;

	sbi_store_loop(in_val.data_bytes, addr, wlen, &uptrap);
	如果 (uptrap.cause) {
		sbi_misaligned_tinst_fixup(orig_trap, &uptrap);
		返回 sbi_trap_redirect(regs, &uptrap);
	}
	返回 wlen;
}

整数 sbi_misaligned_store_handler(struct sbi_trap_context *tcntx)
{
	返回 sbi_trap_emulate_store(tcntx, sbi_misaligned_st_emulator);
}

static 整数 sbi_ld_access_emulator(ulong insn, 整数 rlen, ulong addr,
				  union sbi_ldst_data *out_val,
				  struct sbi_trap_context *tcntx)
{
	const struct sbi_trap_info *orig_trap = &tcntx->trap;
	struct sbi_trap_regs *regs = &tcntx->regs;
	整数 rc;

	/* If fault came from M mode, just fail */
	如果 (sbi_mstatus_prev_mode(regs->mstatus) == PRV_M)
		返回 SBI_EINVAL;

	rc = sbi_platform_emulate_load(sbi_platform_thishart_ptr(),
				       insn, rlen, addr, out_val, tcntx);

	/* If platform emulator failed, we redirect instead of fail */
	如果 (rc < 0)
		返回 sbi_trap_redirect(regs, orig_trap);

	返回 rc;
}

整数 sbi_load_access_handler(struct sbi_trap_context *tcntx)
{
	返回 sbi_trap_emulate_load(tcntx, sbi_ld_access_emulator);
}

static 整数 sbi_st_access_emulator(ulong insn, 整数 wlen, ulong addr,
				  union sbi_ldst_data in_val,
				  struct sbi_trap_context *tcntx)
{
	const struct sbi_trap_info *orig_trap = &tcntx->trap;
	struct sbi_trap_regs *regs = &tcntx->regs;
	整数 rc;

	/* If fault came from M mode, just fail */
	如果 (sbi_mstatus_prev_mode(regs->mstatus) == PRV_M)
		返回 SBI_EINVAL;

	rc = sbi_platform_emulate_store(sbi_platform_thishart_ptr(),
					insn, wlen, addr, in_val, tcntx);

	/* If platform emulator failed, we redirect instead of fail */
	如果 (rc < 0)
		返回 sbi_trap_redirect(regs, orig_trap);

	返回 rc;
}

整数 sbi_store_access_handler(struct sbi_trap_context *tcntx)
{
	返回 sbi_trap_emulate_store(tcntx, sbi_st_access_emulator);
}
