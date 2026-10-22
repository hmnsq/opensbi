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

स्थिर bool hpm_allowed(पूर्णाङ्क hpm_अङ्कः, ulong पूर्व_बहुलकम्, bool virt)
{
	ulong cen = -1UL;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	यदि (पूर्व_बहुलकम् <= PRV_S) {
		यदि (sbi_hart_priv_संस्करणम्(scratch) >= SBI_HART_PRIV_VER_1_10) {
			cen &= csr_read(CSR_MCOUNTEREN);
			यदि (virt)
				cen &= csr_read(CSR_HCOUNTEREN);
		} अन्यथा {
			cen = 0;
		}
	}
	यदि (पूर्व_बहुलकम् == PRV_U) {
		यदि (sbi_hart_priv_संस्करणम्(scratch) >= SBI_HART_PRIV_VER_1_10)
			cen &= csr_read(CSR_SCOUNTEREN);
		अन्यथा
			cen = 0;
	}

	प्रत्यागम ((cen >> hpm_अङ्कः) & 1) ? true : false;
}

पूर्णाङ्क sbi_emulate_csr_read(पूर्णाङ्क csr_अङ्कः, संरचना sbi_trap_regs *regs,
			 ulong *csr_val)
{
	पूर्णाङ्क ret = 0;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong पूर्व_बहुलकम् = sbi_mstatus_पूर्व_बहुलकम्(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	विकल्प (csr_अङ्कः) {
	पक्ष CSR_HTIMEDELTA:
		यदि (पूर्व_बहुलकम् == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta();
		अन्यथा
			ret = SBI_ENOTSUPP;
		भङ्ग;
	पक्ष CSR_आवृत्तिः:
		यदि (!hpm_allowed(csr_अङ्कः - CSR_आवृत्तिः, पूर्व_बहुलकम्, virt))
			प्रत्यागम SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLE);
		भङ्ग;
	पक्ष CSR_TIME:
		यदि (!hpm_allowed(csr_अङ्कः - CSR_आवृत्तिः, पूर्व_बहुलकम्, virt))
			प्रत्यागम SBI_ENOTSUPP;
		/*
		 * We emulate TIME CSR for both Host (HS/U-mode) and
		 * Guest (VS/VU-mode).
		 */
		*csr_val = (virt) ? sbi_timer_virt_मानम्():
				    sbi_timer_मानम्();
		भङ्ग;
	पक्ष CSR_INSTRET:
		यदि (!hpm_allowed(csr_अङ्कः - CSR_आवृत्तिः, पूर्व_बहुलकम्, virt))
			प्रत्यागम SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRET);
		भङ्ग;

#यदि __riscv_xlen == 32
	पक्ष CSR_HTIMEDELTAH:
		यदि (पूर्व_बहुलकम् == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta() >> 32;
		अन्यथा
			ret = SBI_ENOTSUPP;
		भङ्ग;
	पक्ष CSR_CYCLEH:
		यदि (!hpm_allowed(csr_अङ्कः - CSR_CYCLEH, पूर्व_बहुलकम्, virt))
			प्रत्यागम SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLEH);
		भङ्ग;
	पक्ष CSR_TIMEH:
		यदि (!hpm_allowed(csr_अङ्कः - CSR_CYCLEH, पूर्व_बहुलकम्, virt))
			प्रत्यागम SBI_ENOTSUPP;
		*csr_val = (virt) ? sbi_timer_virt_मानम्() >> 32:
				    sbi_timer_मानम्() >> 32;
		भङ्ग;
	पक्ष CSR_INSTRETH:
		यदि (!hpm_allowed(csr_अङ्कः - CSR_CYCLEH, पूर्व_बहुलकम्, virt))
			प्रत्यागम SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRETH);
		भङ्ग;
#यदि_अन्तः

#परिभाषय switchcase_hpm(__uref, __mref, __csr)				\
	पक्ष __csr:							\
		यदि (sbi_hart_mhpm_आवरणम्(scratch) & (1 << (__csr - __uref)))\
			प्रत्यागम SBI_ENOTSUPP;				\
		यदि (!hpm_allowed(__csr - __uref, पूर्व_बहुलकम्, virt))	\
			प्रत्यागम SBI_ENOTSUPP;				\
		*csr_val = csr_read(__mref + __csr - __uref);		\
		भङ्ग;
#परिभाषय switchcase_hpm_2(__uref, __mref, __csr)			\
	switchcase_hpm(__uref, __mref, __csr + 0)			\
	switchcase_hpm(__uref, __mref, __csr + 1)
#परिभाषय switchcase_hpm_4(__uref, __mref, __csr)			\
	switchcase_hpm_2(__uref, __mref, __csr + 0)			\
	switchcase_hpm_2(__uref, __mref, __csr + 2)
#परिभाषय switchcase_hpm_8(__uref, __mref, __csr)			\
	switchcase_hpm_4(__uref, __mref, __csr + 0)			\
	switchcase_hpm_4(__uref, __mref, __csr + 4)
#परिभाषय switchcase_hpm_16(__uref, __mref, __csr)			\
	switchcase_hpm_8(__uref, __mref, __csr + 0)			\
	switchcase_hpm_8(__uref, __mref, __csr + 8)

	switchcase_hpm(CSR_आवृत्तिः, CSR_MCYCLE, CSR_HPMCOUNTER3)
	switchcase_hpm_4(CSR_आवृत्तिः, CSR_MCYCLE, CSR_HPMCOUNTER4)
	switchcase_hpm_8(CSR_आवृत्तिः, CSR_MCYCLE, CSR_HPMCOUNTER8)
	switchcase_hpm_16(CSR_आवृत्तिः, CSR_MCYCLE, CSR_HPMCOUNTER16)

#यदि __riscv_xlen == 32
	switchcase_hpm(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER3H)
	switchcase_hpm_4(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER4H)
	switchcase_hpm_8(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER8H)
	switchcase_hpm_16(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER16H)
#यदि_अन्तः

#अपरिभाषय switchcase_hpm_16
#अपरिभाषय switchcase_hpm_8
#अपरिभाषय switchcase_hpm_4
#अपरिभाषय switchcase_hpm_2
#अपरिभाषय switchcase_hpm

	स्वभाव:
		ret = SBI_ENOTSUPP;
		भङ्ग;
	}

	प्रत्यागम ret;
}

पूर्णाङ्क sbi_emulate_csr_write(पूर्णाङ्क csr_अङ्कः, संरचना sbi_trap_regs *regs,
			  ulong csr_val)
{
	पूर्णाङ्क ret = 0;
	ulong पूर्व_बहुलकम् = sbi_mstatus_पूर्व_बहुलकम्(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	विकल्प (csr_अङ्कः) {
	पक्ष CSR_HTIMEDELTA:
		यदि (पूर्व_बहुलकम् == PRV_S && !virt)
			sbi_timer_समुच्चयः_delta(csr_val);
		अन्यथा
			ret = SBI_ENOTSUPP;
		भङ्ग;
#यदि __riscv_xlen == 32
	पक्ष CSR_HTIMEDELTAH:
		यदि (पूर्व_बहुलकम् == PRV_S && !virt)
			sbi_timer_समुच्चयः_delta_ऊर्ध्वस्थम्(csr_val);
		अन्यथा
			ret = SBI_ENOTSUPP;
		भङ्ग;
#यदि_अन्तः
	स्वभाव:
		ret = SBI_ENOTSUPP;
		भङ्ग;
	}

	प्रत्यागम ret;
}
