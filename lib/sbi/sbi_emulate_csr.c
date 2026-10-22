/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_encoding.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_bitops.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_emulate_csr.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_timer.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_trap.h>

ସ୍ଥିର bool hpm_allowed(ପୂର୍ଣ୍ଣସଂଖ୍ୟା hpm_ଅଙ୍କ, ulong prev_mode, bool virt)
{
	ulong cen = -1UL;
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ଯଦି (prev_mode <= PRV_S) {
		ଯଦି (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10) {
			cen &= csr_read(CSR_MCOUNTEREN);
			ଯଦି (virt)
				cen &= csr_read(CSR_HCOUNTEREN);
		} ଅନ୍ୟଥା {
			cen = 0;
		}
	}
	ଯଦି (prev_mode == PRV_U) {
		ଯଦି (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
			cen &= csr_read(CSR_SCOUNTEREN);
		ଅନ୍ୟଥା
			cen = 0;
	}

	ଫେରନ୍ତୁ ((cen >> hpm_ଅଙ୍କ) & 1) ? true : false;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_emulate_csr_read(ପୂର୍ଣ୍ଣସଂଖ୍ୟା csr_ଅଙ୍କ, ସଂରଚନା sbi_trap_regs *regs,
			 ulong *csr_val)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 0;
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	ବାଛ (csr_ଅଙ୍କ) {
	ସ୍ଥିତି CSR_HTIMEDELTA:
		ଯଦି (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta();
		ଅନ୍ୟଥା
			ret = SBI_ENOTSUPP;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି CSR_CYCLE:
		ଯଦି (!hpm_allowed(csr_ଅଙ୍କ - CSR_CYCLE, prev_mode, virt))
			ଫେରନ୍ତୁ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLE);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି CSR_TIME:
		ଯଦି (!hpm_allowed(csr_ଅଙ୍କ - CSR_CYCLE, prev_mode, virt))
			ଫେରନ୍ତୁ SBI_ENOTSUPP;
		/*
		 * We emulate TIME CSR for both Host (HS/U-mode) and
		 * Guest (VS/VU-mode).
		 */
		*csr_val = (virt) ? sbi_timer_virt_ମୂଲ୍ୟ():
				    sbi_timer_ମୂଲ୍ୟ();
		ଭାଙ୍ଗ;
	ସ୍ଥିତି CSR_INSTRET:
		ଯଦି (!hpm_allowed(csr_ଅଙ୍କ - CSR_CYCLE, prev_mode, virt))
			ଫେରନ୍ତୁ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRET);
		ଭାଙ୍ଗ;

#ଯଦି __riscv_xlen == 32
	ସ୍ଥିତି CSR_HTIMEDELTAH:
		ଯଦି (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta() >> 32;
		ଅନ୍ୟଥା
			ret = SBI_ENOTSUPP;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି CSR_CYCLEH:
		ଯଦି (!hpm_allowed(csr_ଅଙ୍କ - CSR_CYCLEH, prev_mode, virt))
			ଫେରନ୍ତୁ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLEH);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି CSR_TIMEH:
		ଯଦି (!hpm_allowed(csr_ଅଙ୍କ - CSR_CYCLEH, prev_mode, virt))
			ଫେରନ୍ତୁ SBI_ENOTSUPP;
		*csr_val = (virt) ? sbi_timer_virt_ମୂଲ୍ୟ() >> 32:
				    sbi_timer_ମୂଲ୍ୟ() >> 32;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି CSR_INSTRETH:
		ଯଦି (!hpm_allowed(csr_ଅଙ୍କ - CSR_CYCLEH, prev_mode, virt))
			ଫେରନ୍ତୁ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRETH);
		ଭାଙ୍ଗ;
#endif

#define switchcase_hpm(__uref, __mref, __csr)				\
	ସ୍ଥିତି __csr:							\
		ଯଦି (sbi_hart_mhpm_mask(scratch) & (1 << (__csr - __uref)))\
			ଫେରନ୍ତୁ SBI_ENOTSUPP;				\
		ଯଦି (!hpm_allowed(__csr - __uref, prev_mode, virt))	\
			ଫେରନ୍ତୁ SBI_ENOTSUPP;				\
		*csr_val = csr_read(__mref + __csr - __uref);		\
		ଭାଙ୍ଗ;
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

#ଯଦି __riscv_xlen == 32
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

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ret = SBI_ENOTSUPP;
		ଭାଙ୍ଗ;
	}

	ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_emulate_csr_write(ପୂର୍ଣ୍ଣସଂଖ୍ୟା csr_ଅଙ୍କ, ସଂରଚନା sbi_trap_regs *regs,
			  ulong csr_val)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 0;
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	ବାଛ (csr_ଅଙ୍କ) {
	ସ୍ଥିତି CSR_HTIMEDELTA:
		ଯଦି (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta(csr_val);
		ଅନ୍ୟଥା
			ret = SBI_ENOTSUPP;
		ଭାଙ୍ଗ;
#ଯଦି __riscv_xlen == 32
	ସ୍ଥିତି CSR_HTIMEDELTAH:
		ଯଦି (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta_upper(csr_val);
		ଅନ୍ୟଥା
			ret = SBI_ENOTSUPP;
		ଭାଙ୍ଗ;
#endif
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ret = SBI_ENOTSUPP;
		ଭାଙ୍ଗ;
	}

	ଫେରନ୍ତୁ ret;
}
