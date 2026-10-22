/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/riscv_encoding.h>
#અન્તર્ભાવ <sbi/sbi_bitops.h>
#અન્તર્ભાવ <sbi/sbi_emulate_csr.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi/sbi_timer.h>
#અન્તર્ભાવ <sbi/sbi_trap.h>

સ્થિર bool hpm_allowed(પૂર્ણાંક hpm_અંક, ulong prev_mode, bool virt)
{
	ulong cen = -1UL;
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	જો (prev_mode <= PRV_S) {
		જો (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10) {
			cen &= csr_read(CSR_MCOUNTEREN);
			જો (virt)
				cen &= csr_read(CSR_HCOUNTEREN);
		} નહીંતર {
			cen = 0;
		}
	}
	જો (prev_mode == PRV_U) {
		જો (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
			cen &= csr_read(CSR_SCOUNTEREN);
		નહીંતર
			cen = 0;
	}

	પાછા ((cen >> hpm_અંક) & 1) ? true : false;
}

પૂર્ણાંક sbi_emulate_csr_read(પૂર્ણાંક csr_અંક, સંરચના sbi_trap_regs *regs,
			 ulong *csr_val)
{
	પૂર્ણાંક ret = 0;
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	પસંદગી (csr_અંક) {
	સ્થિતિ CSR_HTIMEDELTA:
		જો (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta();
		નહીંતર
			ret = SBI_ENOTSUPP;
		તોડો;
	સ્થિતિ CSR_CYCLE:
		જો (!hpm_allowed(csr_અંક - CSR_CYCLE, prev_mode, virt))
			પાછા SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLE);
		તોડો;
	સ્થિતિ CSR_TIME:
		જો (!hpm_allowed(csr_અંક - CSR_CYCLE, prev_mode, virt))
			પાછા SBI_ENOTSUPP;
		/*
		 * We emulate TIME CSR for both Host (HS/U-mode) and
		 * Guest (VS/VU-mode).
		 */
		*csr_val = (virt) ? sbi_timer_virt_કિંમત():
				    sbi_timer_કિંમત();
		તોડો;
	સ્થિતિ CSR_INSTRET:
		જો (!hpm_allowed(csr_અંક - CSR_CYCLE, prev_mode, virt))
			પાછા SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRET);
		તોડો;

#જો __riscv_xlen == 32
	સ્થિતિ CSR_HTIMEDELTAH:
		જો (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta() >> 32;
		નહીંતર
			ret = SBI_ENOTSUPP;
		તોડો;
	સ્થિતિ CSR_CYCLEH:
		જો (!hpm_allowed(csr_અંક - CSR_CYCLEH, prev_mode, virt))
			પાછા SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLEH);
		તોડો;
	સ્થિતિ CSR_TIMEH:
		જો (!hpm_allowed(csr_અંક - CSR_CYCLEH, prev_mode, virt))
			પાછા SBI_ENOTSUPP;
		*csr_val = (virt) ? sbi_timer_virt_કિંમત() >> 32:
				    sbi_timer_કિંમત() >> 32;
		તોડો;
	સ્થિતિ CSR_INSTRETH:
		જો (!hpm_allowed(csr_અંક - CSR_CYCLEH, prev_mode, virt))
			પાછા SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRETH);
		તોડો;
#endif

#define switchcase_hpm(__uref, __mref, __csr)				\
	સ્થિતિ __csr:							\
		જો (sbi_hart_mhpm_mask(scratch) & (1 << (__csr - __uref)))\
			પાછા SBI_ENOTSUPP;				\
		જો (!hpm_allowed(__csr - __uref, prev_mode, virt))	\
			પાછા SBI_ENOTSUPP;				\
		*csr_val = csr_read(__mref + __csr - __uref);		\
		તોડો;
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

#જો __riscv_xlen == 32
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

	મૂળભૂત:
		ret = SBI_ENOTSUPP;
		તોડો;
	}

	પાછા ret;
}

પૂર્ણાંક sbi_emulate_csr_write(પૂર્ણાંક csr_અંક, સંરચના sbi_trap_regs *regs,
			  ulong csr_val)
{
	પૂર્ણાંક ret = 0;
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	પસંદગી (csr_અંક) {
	સ્થિતિ CSR_HTIMEDELTA:
		જો (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta(csr_val);
		નહીંતર
			ret = SBI_ENOTSUPP;
		તોડો;
#જો __riscv_xlen == 32
	સ્થિતિ CSR_HTIMEDELTAH:
		જો (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta_upper(csr_val);
		નહીંતર
			ret = SBI_ENOTSUPP;
		તોડો;
#endif
	મૂળભૂત:
		ret = SBI_ENOTSUPP;
		તોડો;
	}

	પાછા ret;
}
