/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাৱ <sbi/riscv_asm.h>
#অন্তর্ভাৱ <sbi/riscv_encoding.h>
#অন্তর্ভাৱ <sbi/sbi_bitops.h>
#অন্তর্ভাৱ <sbi/sbi_emulate_csr.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_hart.h>
#অন্তর্ভাৱ <sbi/sbi_scratch.h>
#অন্তর্ভাৱ <sbi/sbi_timer.h>
#অন্তর্ভাৱ <sbi/sbi_trap.h>

স্থির bool hpm_allowed(পূর্ণাঙ্ক hpm_অঙ্ক, ulong prev_mode, bool virt)
{
	ulong cen = -1UL;
	সংরচনা sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	যদি (prev_mode <= PRV_S) {
		যদি (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10) {
			cen &= csr_read(CSR_MCOUNTEREN);
			যদি (virt)
				cen &= csr_read(CSR_HCOUNTEREN);
		} অন্যথা {
			cen = 0;
		}
	}
	যদি (prev_mode == PRV_U) {
		যদি (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
			cen &= csr_read(CSR_SCOUNTEREN);
		অন্যথা
			cen = 0;
	}

	ঘূৰাই ((cen >> hpm_অঙ্ক) & 1) ? true : false;
}

পূর্ণাঙ্ক sbi_emulate_csr_read(পূর্ণাঙ্ক csr_অঙ্ক, সংরচনা sbi_trap_regs *regs,
			 ulong *csr_val)
{
	পূর্ণাঙ্ক ret = 0;
	সংরচনা sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	ৱিকল্প (csr_অঙ্ক) {
	পক্ষ CSR_HTIMEDELTA:
		যদি (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta();
		অন্যথা
			ret = SBI_ENOTSUPP;
		ভঙ্গ;
	পক্ষ CSR_CYCLE:
		যদি (!hpm_allowed(csr_অঙ্ক - CSR_CYCLE, prev_mode, virt))
			ঘূৰাই SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLE);
		ভঙ্গ;
	পক্ষ CSR_TIME:
		যদি (!hpm_allowed(csr_অঙ্ক - CSR_CYCLE, prev_mode, virt))
			ঘূৰাই SBI_ENOTSUPP;
		/*
		 * We emulate TIME CSR for both Host (HS/U-mode) and
		 * Guest (VS/VU-mode).
		 */
		*csr_val = (virt) ? sbi_timer_virt_মান():
				    sbi_timer_মান();
		ভঙ্গ;
	পক্ষ CSR_INSTRET:
		যদি (!hpm_allowed(csr_অঙ্ক - CSR_CYCLE, prev_mode, virt))
			ঘূৰাই SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRET);
		ভঙ্গ;

#যদি __riscv_xlen == 32
	পক্ষ CSR_HTIMEDELTAH:
		যদি (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta() >> 32;
		অন্যথা
			ret = SBI_ENOTSUPP;
		ভঙ্গ;
	পক্ষ CSR_CYCLEH:
		যদি (!hpm_allowed(csr_অঙ্ক - CSR_CYCLEH, prev_mode, virt))
			ঘূৰাই SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLEH);
		ভঙ্গ;
	পক্ষ CSR_TIMEH:
		যদি (!hpm_allowed(csr_অঙ্ক - CSR_CYCLEH, prev_mode, virt))
			ঘূৰাই SBI_ENOTSUPP;
		*csr_val = (virt) ? sbi_timer_virt_মান() >> 32:
				    sbi_timer_মান() >> 32;
		ভঙ্গ;
	পক্ষ CSR_INSTRETH:
		যদি (!hpm_allowed(csr_অঙ্ক - CSR_CYCLEH, prev_mode, virt))
			ঘূৰাই SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRETH);
		ভঙ্গ;
#endif

#define switchcase_hpm(__uref, __mref, __csr)				\
	পক্ষ __csr:							\
		যদি (sbi_hart_mhpm_mask(scratch) & (1 << (__csr - __uref)))\
			ঘূৰাই SBI_ENOTSUPP;				\
		যদি (!hpm_allowed(__csr - __uref, prev_mode, virt))	\
			ঘূৰাই SBI_ENOTSUPP;				\
		*csr_val = csr_read(__mref + __csr - __uref);		\
		ভঙ্গ;
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

#যদি __riscv_xlen == 32
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

	স্ৱভাৱ:
		ret = SBI_ENOTSUPP;
		ভঙ্গ;
	}

	ঘূৰাই ret;
}

পূর্ণাঙ্ক sbi_emulate_csr_write(পূর্ণাঙ্ক csr_অঙ্ক, সংরচনা sbi_trap_regs *regs,
			  ulong csr_val)
{
	পূর্ণাঙ্ক ret = 0;
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	ৱিকল্প (csr_অঙ্ক) {
	পক্ষ CSR_HTIMEDELTA:
		যদি (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta(csr_val);
		অন্যথা
			ret = SBI_ENOTSUPP;
		ভঙ্গ;
#যদি __riscv_xlen == 32
	পক্ষ CSR_HTIMEDELTAH:
		যদি (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta_upper(csr_val);
		অন্যথা
			ret = SBI_ENOTSUPP;
		ভঙ্গ;
#endif
	স্ৱভাৱ:
		ret = SBI_ENOTSUPP;
		ভঙ্গ;
	}

	ঘূৰাই ret;
}
