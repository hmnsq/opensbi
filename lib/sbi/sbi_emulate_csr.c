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
#অন্তর্ভাব <sbi/sbi_bitops.h>
#অন্তর্ভাব <sbi/sbi_emulate_csr.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>
#অন্তর্ভাব <sbi/sbi_timer.h>
#অন্তর্ভাব <sbi/sbi_trap.h>

স্থির bool hpm_allowed(পূর্ণসংখ্যা hpm_অঙ্ক, ulong পূর্ব_mode, bool virt)
{
	ulong cen = -1UL;
	কাঠামো sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	যদি (পূর্ব_mode <= PRV_S) {
		যদি (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10) {
			cen &= csr_read(CSR_MCOUNTEREN);
			যদি (virt)
				cen &= csr_read(CSR_HCOUNTEREN);
		} অন্যথা {
			cen = 0;
		}
	}
	যদি (পূর্ব_mode == PRV_U) {
		যদি (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
			cen &= csr_read(CSR_SCOUNTEREN);
		অন্যথা
			cen = 0;
	}

	ফেরত ((cen >> hpm_অঙ্ক) & 1) ? true : false;
}

পূর্ণসংখ্যা sbi_emulate_csr_read(পূর্ণসংখ্যা csr_অঙ্ক, কাঠামো sbi_trap_regs *regs,
			 ulong *csr_val)
{
	পূর্ণসংখ্যা ret = 0;
	কাঠামো sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong পূর্ব_mode = sbi_mstatus_পূর্ব_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	নির্বাচন (csr_অঙ্ক) {
	ক্ষেত্র CSR_HTIMEDELTA:
		যদি (পূর্ব_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta();
		অন্যথা
			ret = SBI_ENOTSUPP;
		বিরতি;
	ক্ষেত্র CSR_CYCLE:
		যদি (!hpm_allowed(csr_অঙ্ক - CSR_CYCLE, পূর্ব_mode, virt))
			ফেরত SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLE);
		বিরতি;
	ক্ষেত্র CSR_TIME:
		যদি (!hpm_allowed(csr_অঙ্ক - CSR_CYCLE, পূর্ব_mode, virt))
			ফেরত SBI_ENOTSUPP;
		/*
		 * We emulate TIME CSR for both Host (HS/U-mode) and
		 * Guest (VS/VU-mode).
		 */
		*csr_val = (virt) ? sbi_timer_virt_মান():
				    sbi_timer_মান();
		বিরতি;
	ক্ষেত্র CSR_INSTRET:
		যদি (!hpm_allowed(csr_অঙ্ক - CSR_CYCLE, পূর্ব_mode, virt))
			ফেরত SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRET);
		বিরতি;

#যদি __riscv_xlen == 32
	ক্ষেত্র CSR_HTIMEDELTAH:
		যদি (পূর্ব_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta() >> 32;
		অন্যথা
			ret = SBI_ENOTSUPP;
		বিরতি;
	ক্ষেত্র CSR_CYCLEH:
		যদি (!hpm_allowed(csr_অঙ্ক - CSR_CYCLEH, পূর্ব_mode, virt))
			ফেরত SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLEH);
		বিরতি;
	ক্ষেত্র CSR_TIMEH:
		যদি (!hpm_allowed(csr_অঙ্ক - CSR_CYCLEH, পূর্ব_mode, virt))
			ফেরত SBI_ENOTSUPP;
		*csr_val = (virt) ? sbi_timer_virt_মান() >> 32:
				    sbi_timer_মান() >> 32;
		বিরতি;
	ক্ষেত্র CSR_INSTRETH:
		যদি (!hpm_allowed(csr_অঙ্ক - CSR_CYCLEH, পূর্ব_mode, virt))
			ফেরত SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRETH);
		বিরতি;
#endif

#define switchcase_hpm(__uref, __mref, __csr)				\
	ক্ষেত্র __csr:							\
		যদি (sbi_hart_mhpm_mask(scratch) & (1 << (__csr - __uref)))\
			ফেরত SBI_ENOTSUPP;				\
		যদি (!hpm_allowed(__csr - __uref, পূর্ব_mode, virt))	\
			ফেরত SBI_ENOTSUPP;				\
		*csr_val = csr_read(__mref + __csr - __uref);		\
		বিরতি;
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

	পূর্বনির্ধারিত:
		ret = SBI_ENOTSUPP;
		বিরতি;
	}

	ফেরত ret;
}

পূর্ণসংখ্যা sbi_emulate_csr_write(পূর্ণসংখ্যা csr_অঙ্ক, কাঠামো sbi_trap_regs *regs,
			  ulong csr_val)
{
	পূর্ণসংখ্যা ret = 0;
	ulong পূর্ব_mode = sbi_mstatus_পূর্ব_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	নির্বাচন (csr_অঙ্ক) {
	ক্ষেত্র CSR_HTIMEDELTA:
		যদি (পূর্ব_mode == PRV_S && !virt)
			sbi_timer_set_delta(csr_val);
		অন্যথা
			ret = SBI_ENOTSUPP;
		বিরতি;
#যদি __riscv_xlen == 32
	ক্ষেত্র CSR_HTIMEDELTAH:
		যদি (পূর্ব_mode == PRV_S && !virt)
			sbi_timer_set_delta_upper(csr_val);
		অন্যথা
			ret = SBI_ENOTSUPP;
		বিরতি;
#endif
	পূর্বনির্ধারিত:
		ret = SBI_ENOTSUPP;
		বিরতি;
	}

	ফেরত ret;
}
