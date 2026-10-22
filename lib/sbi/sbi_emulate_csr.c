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
#include <sbi/sbi_bitops.h>
#include <sbi/sbi_emulate_csr.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_scratch.h>
#include <sbi/sbi_timer.h>
#include <sbi/sbi_trap.h>

static bool hpm_allowed(цео hpm_num, ulong prev_mode, bool virt)
{
	ulong cen = -1UL;
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ако (prev_mode <= PRV_S) {
		ако (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10) {
			cen &= csr_read(CSR_MCOUNTEREN);
			ако (virt)
				cen &= csr_read(CSR_HCOUNTEREN);
		} инако {
			cen = 0;
		}
	}
	ако (prev_mode == PRV_U) {
		ако (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
			cen &= csr_read(CSR_SCOUNTEREN);
		инако
			cen = 0;
	}

	врати ((cen >> hpm_num) & 1) ? true : false;
}

цео sbi_emulate_csr_read(цео csr_num, struct sbi_trap_regs *regs,
			 ulong *csr_val)
{
	цео ret = 0;
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	switch (csr_num) {
	case CSR_HTIMEDELTA:
		ако (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta();
		инако
			ret = SBI_ENOTSUPP;
		break;
	case CSR_CYCLE:
		ако (!hpm_allowed(csr_num - CSR_CYCLE, prev_mode, virt))
			врати SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLE);
		break;
	case CSR_TIME:
		ако (!hpm_allowed(csr_num - CSR_CYCLE, prev_mode, virt))
			врати SBI_ENOTSUPP;
		/*
		 * We emulate TIME CSR for both Host (HS/U-mode) and
		 * Guest (VS/VU-mode).
		 */
		*csr_val = (virt) ? sbi_timer_virt_value():
				    sbi_timer_value();
		break;
	case CSR_INSTRET:
		ако (!hpm_allowed(csr_num - CSR_CYCLE, prev_mode, virt))
			врати SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRET);
		break;

#ако __riscv_xlen == 32
	case CSR_HTIMEDELTAH:
		ако (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta() >> 32;
		инако
			ret = SBI_ENOTSUPP;
		break;
	case CSR_CYCLEH:
		ако (!hpm_allowed(csr_num - CSR_CYCLEH, prev_mode, virt))
			врати SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLEH);
		break;
	case CSR_TIMEH:
		ако (!hpm_allowed(csr_num - CSR_CYCLEH, prev_mode, virt))
			врати SBI_ENOTSUPP;
		*csr_val = (virt) ? sbi_timer_virt_value() >> 32:
				    sbi_timer_value() >> 32;
		break;
	case CSR_INSTRETH:
		ако (!hpm_allowed(csr_num - CSR_CYCLEH, prev_mode, virt))
			врати SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRETH);
		break;
#endif

#define switchcase_hpm(__uref, __mref, __csr)				\
	case __csr:							\
		ако (sbi_hart_mhpm_mask(scratch) & (1 << (__csr - __uref)))\
			врати SBI_ENOTSUPP;				\
		ако (!hpm_allowed(__csr - __uref, prev_mode, virt))	\
			врати SBI_ENOTSUPP;				\
		*csr_val = csr_read(__mref + __csr - __uref);		\
		break;
#define switchcase_hpm_2(__uref, __mref, __csr)			\
	switchcase_hpm(__uref, __mref, __csr + 0)			\
	switchcase_hpm(__uref, __mref, __csr + 1)
#define switchcase_hpm_4(__uref, __mref, __csr)			\
	switchcase_hpm_2(__uref, __mref, __csr + 0)			\
	switchcase_hpm_2(__uref, __mref, __csr + 2)
#define switchcase_hpm_8(__uref, __mref, __csr)			\
	switchcase_hpm_4(__uref, __mref, __csr + 0)			\
	switchcase_hpm_4(__uref, __mref, __csr + 4)
#define switchcase_hpm_16(__uref, __mref, __csr)			\
	switchcase_hpm_8(__uref, __mref, __csr + 0)			\
	switchcase_hpm_8(__uref, __mref, __csr + 8)

	switchcase_hpm(CSR_CYCLE, CSR_MCYCLE, CSR_HPMCOUNTER3)
	switchcase_hpm_4(CSR_CYCLE, CSR_MCYCLE, CSR_HPMCOUNTER4)
	switchcase_hpm_8(CSR_CYCLE, CSR_MCYCLE, CSR_HPMCOUNTER8)
	switchcase_hpm_16(CSR_CYCLE, CSR_MCYCLE, CSR_HPMCOUNTER16)

#ако __riscv_xlen == 32
	switchcase_hpm(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER3H)
	switchcase_hpm_4(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER4H)
	switchcase_hpm_8(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER8H)
	switchcase_hpm_16(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER16H)
#endif

#undef switchcase_hpm_16
#undef switchcase_hpm_8
#undef switchcase_hpm_4
#undef switchcase_hpm_2
#undef switchcase_hpm

	default:
		ret = SBI_ENOTSUPP;
		break;
	}

	врати ret;
}

цео sbi_emulate_csr_write(цео csr_num, struct sbi_trap_regs *regs,
			  ulong csr_val)
{
	цео ret = 0;
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	switch (csr_num) {
	case CSR_HTIMEDELTA:
		ако (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta(csr_val);
		инако
			ret = SBI_ENOTSUPP;
		break;
#ако __riscv_xlen == 32
	case CSR_HTIMEDELTAH:
		ако (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta_upper(csr_val);
		инако
			ret = SBI_ENOTSUPP;
		break;
#endif
	default:
		ret = SBI_ENOTSUPP;
		break;
	}

	врати ret;
}
