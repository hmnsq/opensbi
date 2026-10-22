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
#अन्तर्भाव <sbi/sbi_bitops.h>
#अन्तर्भाव <sbi/sbi_emulate_csr.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi/sbi_timer.h>
#अन्तर्भाव <sbi/sbi_trap.h>

स्थिर bool hpm_allowed(पूर्णाङ्क hpm_अङ्कः, ulong prev_mode, bool virt)
{
	ulong cen = -1UL;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	सचे (prev_mode <= PRV_S) {
		सचे (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10) {
			cen &= csr_read(CSR_MCOUNTEREN);
			सचे (virt)
				cen &= csr_read(CSR_HCOUNTEREN);
		} अञ्ञथा {
			cen = 0;
		}
	}
	सचे (prev_mode == PRV_U) {
		सचे (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
			cen &= csr_read(CSR_SCOUNTEREN);
		अञ्ञथा
			cen = 0;
	}

	पटिनिवत्त ((cen >> hpm_अङ्कः) & 1) ? true : false;
}

पूर्णाङ्क sbi_emulate_csr_read(पूर्णाङ्क csr_अङ्कः, संरचना sbi_trap_regs *regs,
			 ulong *csr_val)
{
	पूर्णाङ्क ret = 0;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	विकल्प (csr_अङ्कः) {
	पक्ष CSR_HTIMEDELTA:
		सचे (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta();
		अञ्ञथा
			ret = SBI_ENOTSUPP;
		भङ्ग;
	पक्ष CSR_CYCLE:
		सचे (!hpm_allowed(csr_अङ्कः - CSR_CYCLE, prev_mode, virt))
			पटिनिवत्त SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLE);
		भङ्ग;
	पक्ष CSR_TIME:
		सचे (!hpm_allowed(csr_अङ्कः - CSR_CYCLE, prev_mode, virt))
			पटिनिवत्त SBI_ENOTSUPP;
		/*
		 * We emulate TIME CSR for both Host (HS/U-mode) and
		 * Guest (VS/VU-mode).
		 */
		*csr_val = (virt) ? sbi_timer_virt_मानम्():
				    sbi_timer_मानम्();
		भङ्ग;
	पक्ष CSR_INSTRET:
		सचे (!hpm_allowed(csr_अङ्कः - CSR_CYCLE, prev_mode, virt))
			पटिनिवत्त SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRET);
		भङ्ग;

#सचे __riscv_xlen == 32
	पक्ष CSR_HTIMEDELTAH:
		सचे (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta() >> 32;
		अञ्ञथा
			ret = SBI_ENOTSUPP;
		भङ्ग;
	पक्ष CSR_CYCLEH:
		सचे (!hpm_allowed(csr_अङ्कः - CSR_CYCLEH, prev_mode, virt))
			पटिनिवत्त SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLEH);
		भङ्ग;
	पक्ष CSR_TIMEH:
		सचे (!hpm_allowed(csr_अङ्कः - CSR_CYCLEH, prev_mode, virt))
			पटिनिवत्त SBI_ENOTSUPP;
		*csr_val = (virt) ? sbi_timer_virt_मानम्() >> 32:
				    sbi_timer_मानम्() >> 32;
		भङ्ग;
	पक्ष CSR_INSTRETH:
		सचे (!hpm_allowed(csr_अङ्कः - CSR_CYCLEH, prev_mode, virt))
			पटिनिवत्त SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRETH);
		भङ्ग;
#endif

#define switchcase_hpm(__uref, __mref, __csr)				\
	पक्ष __csr:							\
		सचे (sbi_hart_mhpm_mask(scratch) & (1 << (__csr - __uref)))\
			पटिनिवत्त SBI_ENOTSUPP;				\
		सचे (!hpm_allowed(__csr - __uref, prev_mode, virt))	\
			पटिनिवत्त SBI_ENOTSUPP;				\
		*csr_val = csr_read(__mref + __csr - __uref);		\
		भङ्ग;
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

#सचे __riscv_xlen == 32
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

	स्वभाव:
		ret = SBI_ENOTSUPP;
		भङ्ग;
	}

	पटिनिवत्त ret;
}

पूर्णाङ्क sbi_emulate_csr_write(पूर्णाङ्क csr_अङ्कः, संरचना sbi_trap_regs *regs,
			  ulong csr_val)
{
	पूर्णाङ्क ret = 0;
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	विकल्प (csr_अङ्कः) {
	पक्ष CSR_HTIMEDELTA:
		सचे (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta(csr_val);
		अञ्ञथा
			ret = SBI_ENOTSUPP;
		भङ्ग;
#सचे __riscv_xlen == 32
	पक्ष CSR_HTIMEDELTAH:
		सचे (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta_upper(csr_val);
		अञ्ञथा
			ret = SBI_ENOTSUPP;
		भङ्ग;
#endif
	स्वभाव:
		ret = SBI_ENOTSUPP;
		भङ्ग;
	}

	पटिनिवत्त ret;
}
