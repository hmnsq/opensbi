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
#ضمن <sbi/sbi_bitops.h>
#ضمن <sbi/sbi_emulate_csr.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi/sbi_timer.h>
#ضمن <sbi/sbi_trap.h>

ساكن bool hpm_allowed(صحيح hpm_انگ, ulong prev_گهڻائي, bool virt)
{
	ulong cen = -1UL;
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	إذا (prev_گهڻائي <= PRV_S) {
		إذا (sbi_hart_priv_نسخو(scratch) >= SBI_HART_PRIV_VER_1_10) {
			cen &= csr_read(CSR_MCOUNTEREN);
			إذا (virt)
				cen &= csr_read(CSR_HCOUNTEREN);
		} وإلا {
			cen = 0;
		}
	}
	إذا (prev_گهڻائي == PRV_U) {
		إذا (sbi_hart_priv_نسخو(scratch) >= SBI_HART_PRIV_VER_1_10)
			cen &= csr_read(CSR_SCOUNTEREN);
		وإلا
			cen = 0;
	}

	أرجع ((cen >> hpm_انگ) & 1) ? true : false;
}

صحيح sbi_emulate_csr_read(صحيح csr_انگ, بنية sbi_trap_regs *regs,
			 ulong *csr_val)
{
	صحيح ret = 0;
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong prev_گهڻائي = sbi_mstatus_prev_گهڻائي(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	بدل (csr_انگ) {
	حالة CSR_HTIMEDELTA:
		إذا (prev_گهڻائي == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta();
		وإلا
			ret = SBI_ENOTSUPP;
		اكسر;
	حالة CSR_چڪر:
		إذا (!hpm_allowed(csr_انگ - CSR_چڪر, prev_گهڻائي, virt))
			أرجع SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLE);
		اكسر;
	حالة CSR_وقت:
		إذا (!hpm_allowed(csr_انگ - CSR_چڪر, prev_گهڻائي, virt))
			أرجع SBI_ENOTSUPP;
		/*
		 * We emulate TIME CSR for both Host (HS/U-mode) and
		 * Guest (VS/VU-mode).
		 */
		*csr_val = (virt) ? sbi_timer_virt_قدر():
				    sbi_timer_قدر();
		اكسر;
	حالة CSR_INSTRET:
		إذا (!hpm_allowed(csr_انگ - CSR_چڪر, prev_گهڻائي, virt))
			أرجع SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRET);
		اكسر;

#إذا __riscv_xlen == 32
	حالة CSR_HTIMEDELTAH:
		إذا (prev_گهڻائي == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta() >> 32;
		وإلا
			ret = SBI_ENOTSUPP;
		اكسر;
	حالة CSR_CYCLEH:
		إذا (!hpm_allowed(csr_انگ - CSR_CYCLEH, prev_گهڻائي, virt))
			أرجع SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLEH);
		اكسر;
	حالة CSR_TIMEH:
		إذا (!hpm_allowed(csr_انگ - CSR_CYCLEH, prev_گهڻائي, virt))
			أرجع SBI_ENOTSUPP;
		*csr_val = (virt) ? sbi_timer_virt_قدر() >> 32:
				    sbi_timer_قدر() >> 32;
		اكسر;
	حالة CSR_INSTRETH:
		إذا (!hpm_allowed(csr_انگ - CSR_CYCLEH, prev_گهڻائي, virt))
			أرجع SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRETH);
		اكسر;
#جيڪڏهن_پورو

#وصف_ڪريو switchcase_hpm(__uref, __mref, __csr)				\
	حالة __csr:							\
		إذا (sbi_hart_mhpm_نقاب(scratch) & (1 << (__csr - __uref)))\
			أرجع SBI_ENOTSUPP;				\
		إذا (!hpm_allowed(__csr - __uref, prev_گهڻائي, virt))	\
			أرجع SBI_ENOTSUPP;				\
		*csr_val = csr_read(__mref + __csr - __uref);		\
		اكسر;
#وصف_ڪريو switchcase_hpm_2(__uref, __mref, __csr)			\
	switchcase_hpm(__uref, __mref, __csr + 0)			\
	switchcase_hpm(__uref, __mref, __csr + 1)
#وصف_ڪريو switchcase_hpm_4(__uref, __mref, __csr)			\
	switchcase_hpm_2(__uref, __mref, __csr + 0)			\
	switchcase_hpm_2(__uref, __mref, __csr + 2)
#وصف_ڪريو switchcase_hpm_8(__uref, __mref, __csr)			\
	switchcase_hpm_4(__uref, __mref, __csr + 0)			\
	switchcase_hpm_4(__uref, __mref, __csr + 4)
#وصف_ڪريو switchcase_hpm_16(__uref, __mref, __csr)			\
	switchcase_hpm_8(__uref, __mref, __csr + 0)			\
	switchcase_hpm_8(__uref, __mref, __csr + 8)

	switchcase_hpm(CSR_چڪر, CSR_MCYCLE, CSR_HPMCOUNTER3)
	switchcase_hpm_4(CSR_چڪر, CSR_MCYCLE, CSR_HPMCOUNTER4)
	switchcase_hpm_8(CSR_چڪر, CSR_MCYCLE, CSR_HPMCOUNTER8)
	switchcase_hpm_16(CSR_چڪر, CSR_MCYCLE, CSR_HPMCOUNTER16)

#إذا __riscv_xlen == 32
	switchcase_hpm(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER3H)
	switchcase_hpm_4(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER4H)
	switchcase_hpm_8(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER8H)
	switchcase_hpm_16(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER16H)
#جيڪڏهن_پورو

#وصف_رد switchcase_hpm_16
#وصف_رد switchcase_hpm_8
#وصف_رد switchcase_hpm_4
#وصف_رد switchcase_hpm_2
#وصف_رد switchcase_hpm

	افتراضي:
		ret = SBI_ENOTSUPP;
		اكسر;
	}

	أرجع ret;
}

صحيح sbi_emulate_csr_write(صحيح csr_انگ, بنية sbi_trap_regs *regs,
			  ulong csr_val)
{
	صحيح ret = 0;
	ulong prev_گهڻائي = sbi_mstatus_prev_گهڻائي(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	بدل (csr_انگ) {
	حالة CSR_HTIMEDELTA:
		إذا (prev_گهڻائي == PRV_S && !virt)
			sbi_timer_سيٽ_delta(csr_val);
		وإلا
			ret = SBI_ENOTSUPP;
		اكسر;
#إذا __riscv_xlen == 32
	حالة CSR_HTIMEDELTAH:
		إذا (prev_گهڻائي == PRV_S && !virt)
			sbi_timer_سيٽ_delta_مٿانهون(csr_val);
		وإلا
			ret = SBI_ENOTSUPP;
		اكسر;
#جيڪڏهن_پورو
	افتراضي:
		ret = SBI_ENOTSUPP;
		اكسر;
	}

	أرجع ret;
}
