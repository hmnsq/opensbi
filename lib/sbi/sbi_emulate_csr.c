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

static bool hpm_allowed(số_nguyên hpm_num, ulong prev_mode, bool virt)
{
	ulong cen = -1UL;
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	nếu (prev_mode <= PRV_S) {
		nếu (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10) {
			cen &= csr_read(CSR_MCOUNTEREN);
			nếu (virt)
				cen &= csr_read(CSR_HCOUNTEREN);
		} không_thì {
			cen = 0;
		}
	}
	nếu (prev_mode == PRV_U) {
		nếu (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
			cen &= csr_read(CSR_SCOUNTEREN);
		không_thì
			cen = 0;
	}

	trả_về ((cen >> hpm_num) & 1) ? true : false;
}

số_nguyên sbi_emulate_csr_read(số_nguyên csr_num, struct sbi_trap_regs *regs,
			 ulong *csr_val)
{
	số_nguyên ret = 0;
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	switch (csr_num) {
	case CSR_HTIMEDELTA:
		nếu (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta();
		không_thì
			ret = SBI_ENOTSUPP;
		break;
	case CSR_CYCLE:
		nếu (!hpm_allowed(csr_num - CSR_CYCLE, prev_mode, virt))
			trả_về SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLE);
		break;
	case CSR_TIME:
		nếu (!hpm_allowed(csr_num - CSR_CYCLE, prev_mode, virt))
			trả_về SBI_ENOTSUPP;
		/*
		 * We emulate TIME CSR for both Host (HS/U-mode) and
		 * Guest (VS/VU-mode).
		 */
		*csr_val = (virt) ? sbi_timer_virt_value():
				    sbi_timer_value();
		break;
	case CSR_INSTRET:
		nếu (!hpm_allowed(csr_num - CSR_CYCLE, prev_mode, virt))
			trả_về SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRET);
		break;

#nếu __riscv_xlen == 32
	case CSR_HTIMEDELTAH:
		nếu (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta() >> 32;
		không_thì
			ret = SBI_ENOTSUPP;
		break;
	case CSR_CYCLEH:
		nếu (!hpm_allowed(csr_num - CSR_CYCLEH, prev_mode, virt))
			trả_về SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLEH);
		break;
	case CSR_TIMEH:
		nếu (!hpm_allowed(csr_num - CSR_CYCLEH, prev_mode, virt))
			trả_về SBI_ENOTSUPP;
		*csr_val = (virt) ? sbi_timer_virt_value() >> 32:
				    sbi_timer_value() >> 32;
		break;
	case CSR_INSTRETH:
		nếu (!hpm_allowed(csr_num - CSR_CYCLEH, prev_mode, virt))
			trả_về SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRETH);
		break;
#endif

#define switchcase_hpm(__uref, __mref, __csr)				\
	case __csr:							\
		nếu (sbi_hart_mhpm_mask(scratch) & (1 << (__csr - __uref)))\
			trả_về SBI_ENOTSUPP;				\
		nếu (!hpm_allowed(__csr - __uref, prev_mode, virt))	\
			trả_về SBI_ENOTSUPP;				\
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

#nếu __riscv_xlen == 32
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

	trả_về ret;
}

số_nguyên sbi_emulate_csr_write(số_nguyên csr_num, struct sbi_trap_regs *regs,
			  ulong csr_val)
{
	số_nguyên ret = 0;
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	switch (csr_num) {
	case CSR_HTIMEDELTA:
		nếu (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta(csr_val);
		không_thì
			ret = SBI_ENOTSUPP;
		break;
#nếu __riscv_xlen == 32
	case CSR_HTIMEDELTAH:
		nếu (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta_upper(csr_val);
		không_thì
			ret = SBI_ENOTSUPP;
		break;
#endif
	default:
		ret = SBI_ENOTSUPP;
		break;
	}

	trả_về ret;
}
