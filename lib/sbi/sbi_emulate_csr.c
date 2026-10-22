/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_encoding.h>
#అన్తర్భావ <sbi/sbi_bitops.h>
#అన్తర్భావ <sbi/sbi_emulate_csr.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi/sbi_timer.h>
#అన్తర్భావ <sbi/sbi_trap.h>

స్థిర bool hpm_allowed(పూర్ణాంకం hpm_అంకె, ulong prev_బహుళకం, bool virt)
{
	ulong cen = -1UL;
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ఒకవేళ (prev_బహుళకం <= PRV_S) {
		ఒకవేళ (sbi_hart_priv_సంస్కరణ(scratch) >= SBI_HART_PRIV_VER_1_10) {
			cen &= csr_read(CSR_MCOUNTEREN);
			ఒకవేళ (virt)
				cen &= csr_read(CSR_HCOUNTEREN);
		} లేకపోతే {
			cen = 0;
		}
	}
	ఒకవేళ (prev_బహుళకం == PRV_U) {
		ఒకవేళ (sbi_hart_priv_సంస్కరణ(scratch) >= SBI_HART_PRIV_VER_1_10)
			cen &= csr_read(CSR_SCOUNTEREN);
		లేకపోతే
			cen = 0;
	}

	తిరిగి ((cen >> hpm_అంకె) & 1) ? true : false;
}

పూర్ణాంకం sbi_emulate_csr_read(పూర్ణాంకం csr_అంకె, నిర్మాణం sbi_trap_regs *regs,
			 ulong *csr_val)
{
	పూర్ణాంకం ret = 0;
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong prev_బహుళకం = sbi_mstatus_prev_బహుళకం(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	ఎంపిక (csr_అంకె) {
	స్థితి CSR_HTIMEDELTA:
		ఒకవేళ (prev_బహుళకం == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta();
		లేకపోతే
			ret = SBI_ENOTSUPP;
		ఆపు;
	స్థితి CSR_చక్రం:
		ఒకవేళ (!hpm_allowed(csr_అంకె - CSR_చక్రం, prev_బహుళకం, virt))
			తిరిగి SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLE);
		ఆపు;
	స్థితి CSR_సమయం:
		ఒకవేళ (!hpm_allowed(csr_అంకె - CSR_చక్రం, prev_బహుళకం, virt))
			తిరిగి SBI_ENOTSUPP;
		/*
		 * We emulate TIME CSR for both Host (HS/U-mode) and
		 * Guest (VS/VU-mode).
		 */
		*csr_val = (virt) ? sbi_timer_virt_విలువ():
				    sbi_timer_విలువ();
		ఆపు;
	స్థితి CSR_INSTRET:
		ఒకవేళ (!hpm_allowed(csr_అంకె - CSR_చక్రం, prev_బహుళకం, virt))
			తిరిగి SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRET);
		ఆపు;

#ఒకవేళ __riscv_xlen == 32
	స్థితి CSR_HTIMEDELTAH:
		ఒకవేళ (prev_బహుళకం == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta() >> 32;
		లేకపోతే
			ret = SBI_ENOTSUPP;
		ఆపు;
	స్థితి CSR_CYCLEH:
		ఒకవేళ (!hpm_allowed(csr_అంకె - CSR_CYCLEH, prev_బహుళకం, virt))
			తిరిగి SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLEH);
		ఆపు;
	స్థితి CSR_TIMEH:
		ఒకవేళ (!hpm_allowed(csr_అంకె - CSR_CYCLEH, prev_బహుళకం, virt))
			తిరిగి SBI_ENOTSUPP;
		*csr_val = (virt) ? sbi_timer_virt_విలువ() >> 32:
				    sbi_timer_విలువ() >> 32;
		ఆపు;
	స్థితి CSR_INSTRETH:
		ఒకవేళ (!hpm_allowed(csr_అంకె - CSR_CYCLEH, prev_బహుళకం, virt))
			తిరిగి SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRETH);
		ఆపు;
#అయితే_ముగింపు

#నిర్వచించు switchcase_hpm(__uref, __mref, __csr)				\
	స్థితి __csr:							\
		ఒకవేళ (sbi_hart_mhpm_ముసుగు(scratch) & (1 << (__csr - __uref)))\
			తిరిగి SBI_ENOTSUPP;				\
		ఒకవేళ (!hpm_allowed(__csr - __uref, prev_బహుళకం, virt))	\
			తిరిగి SBI_ENOTSUPP;				\
		*csr_val = csr_read(__mref + __csr - __uref);		\
		ఆపు;
#నిర్వచించు switchcase_hpm_2(__uref, __mref, __csr)			\
	switchcase_hpm(__uref, __mref, __csr + 0)			\
	switchcase_hpm(__uref, __mref, __csr + 1)
#నిర్వచించు switchcase_hpm_4(__uref, __mref, __csr)			\
	switchcase_hpm_2(__uref, __mref, __csr + 0)			\
	switchcase_hpm_2(__uref, __mref, __csr + 2)
#నిర్వచించు switchcase_hpm_8(__uref, __mref, __csr)			\
	switchcase_hpm_4(__uref, __mref, __csr + 0)			\
	switchcase_hpm_4(__uref, __mref, __csr + 4)
#నిర్వచించు switchcase_hpm_16(__uref, __mref, __csr)			\
	switchcase_hpm_8(__uref, __mref, __csr + 0)			\
	switchcase_hpm_8(__uref, __mref, __csr + 8)

	switchcase_hpm(CSR_చక్రం, CSR_MCYCLE, CSR_HPMCOUNTER3)
	switchcase_hpm_4(CSR_చక్రం, CSR_MCYCLE, CSR_HPMCOUNTER4)
	switchcase_hpm_8(CSR_చక్రం, CSR_MCYCLE, CSR_HPMCOUNTER8)
	switchcase_hpm_16(CSR_చక్రం, CSR_MCYCLE, CSR_HPMCOUNTER16)

#ఒకవేళ __riscv_xlen == 32
	switchcase_hpm(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER3H)
	switchcase_hpm_4(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER4H)
	switchcase_hpm_8(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER8H)
	switchcase_hpm_16(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER16H)
#అయితే_ముగింపు

#నిర్వచనం_రద్దు switchcase_hpm_16
#నిర్వచనం_రద్దు switchcase_hpm_8
#నిర్వచనం_రద్దు switchcase_hpm_4
#నిర్వచనం_రద్దు switchcase_hpm_2
#నిర్వచనం_రద్దు switchcase_hpm

	అప్రమేయ:
		ret = SBI_ENOTSUPP;
		ఆపు;
	}

	తిరిగి ret;
}

పూర్ణాంకం sbi_emulate_csr_write(పూర్ణాంకం csr_అంకె, నిర్మాణం sbi_trap_regs *regs,
			  ulong csr_val)
{
	పూర్ణాంకం ret = 0;
	ulong prev_బహుళకం = sbi_mstatus_prev_బహుళకం(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	ఎంపిక (csr_అంకె) {
	స్థితి CSR_HTIMEDELTA:
		ఒకవేళ (prev_బహుళకం == PRV_S && !virt)
			sbi_timer_సమితి_delta(csr_val);
		లేకపోతే
			ret = SBI_ENOTSUPP;
		ఆపు;
#ఒకవేళ __riscv_xlen == 32
	స్థితి CSR_HTIMEDELTAH:
		ఒకవేళ (prev_బహుళకం == PRV_S && !virt)
			sbi_timer_సమితి_delta_పై(csr_val);
		లేకపోతే
			ret = SBI_ENOTSUPP;
		ఆపు;
#అయితే_ముగింపు
	అప్రమేయ:
		ret = SBI_ENOTSUPP;
		ఆపు;
	}

	తిరిగి ret;
}
