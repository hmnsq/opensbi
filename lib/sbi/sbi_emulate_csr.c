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

स्थिर bool hpm_allowed(पूर्णाङ्क hpm_अंक, ulong prev_mode, bool virt)
{
	ulong cen = -1UL;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	यदि (prev_mode <= PRV_S) {
		यदि (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10) {
			cen &= csr_read(CSR_MCOUNTEREN);
			यदि (virt)
				cen &= csr_read(CSR_HCOUNTEREN);
		} अन्यथा {
			cen = 0;
		}
	}
	यदि (prev_mode == PRV_U) {
		यदि (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
			cen &= csr_read(CSR_SCOUNTEREN);
		अन्यथा
			cen = 0;
	}

	प्रत्यागम ((cen >> hpm_अंक) & 1) ? true : false;
}

पूर्णाङ्क sbi_emulate_csr_read(पूर्णाङ्क csr_अंक, संरचना sbi_trap_regs *regs,
			 ulong *csr_val)
{
	पूर्णाङ्क ret = 0;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	विकल्प (csr_अंक) {
	पक्ष CSR_HTIMEDELTA:
		यदि (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta();
		अन्यथा
			ret = SBI_ENOTSUPP;
		भङ्ग;
	पक्ष CSR_CYCLE:
		यदि (!hpm_allowed(csr_अंक - CSR_CYCLE, prev_mode, virt))
			प्रत्यागम SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLE);
		भङ्ग;
	पक्ष CSR_TIME:
		यदि (!hpm_allowed(csr_अंक - CSR_CYCLE, prev_mode, virt))
			प्रत्यागम SBI_ENOTSUPP;
		/*
		 * We emulate TIME CSR for both Host (HS/U-mode) and
		 * Guest (VS/VU-mode).
		 */
		*csr_val = (virt) ? sbi_timer_virt_मूल्य():
				    sbi_timer_मूल्य();
		भङ्ग;
	पक्ष CSR_INSTRET:
		यदि (!hpm_allowed(csr_अंक - CSR_CYCLE, prev_mode, virt))
			प्रत्यागम SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRET);
		भङ्ग;

#यदि __riscv_xlen == 32
	पक्ष CSR_HTIMEDELTAH:
		यदि (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta() >> 32;
		अन्यथा
			ret = SBI_ENOTSUPP;
		भङ्ग;
	पक्ष CSR_CYCLEH:
		यदि (!hpm_allowed(csr_अंक - CSR_CYCLEH, prev_mode, virt))
			प्रत्यागम SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLEH);
		भङ्ग;
	पक्ष CSR_TIMEH:
		यदि (!hpm_allowed(csr_अंक - CSR_CYCLEH, prev_mode, virt))
			प्रत्यागम SBI_ENOTSUPP;
		*csr_val = (virt) ? sbi_timer_virt_मूल्य() >> 32:
				    sbi_timer_मूल्य() >> 32;
		भङ्ग;
	पक्ष CSR_INSTRETH:
		यदि (!hpm_allowed(csr_अंक - CSR_CYCLEH, prev_mode, virt))
			प्रत्यागम SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRETH);
		भङ्ग;
#endif

#define switchcase_hpm(__uref, __mref, __csr)				\
	पक्ष __csr:							\
		यदि (sbi_hart_mhpm_mask(scratch) & (1 << (__csr - __uref)))\
			प्रत्यागम SBI_ENOTSUPP;				\
		यदि (!hpm_allowed(__csr - __uref, prev_mode, virt))	\
			प्रत्यागम SBI_ENOTSUPP;				\
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

#यदि __riscv_xlen == 32
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

	प्रत्यागम ret;
}

पूर्णाङ्क sbi_emulate_csr_write(पूर्णाङ्क csr_अंक, संरचना sbi_trap_regs *regs,
			  ulong csr_val)
{
	पूर्णाङ्क ret = 0;
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	विकल्प (csr_अंक) {
	पक्ष CSR_HTIMEDELTA:
		यदि (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta(csr_val);
		अन्यथा
			ret = SBI_ENOTSUPP;
		भङ्ग;
#यदि __riscv_xlen == 32
	पक्ष CSR_HTIMEDELTAH:
		यदि (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta_upper(csr_val);
		अन्यथा
			ret = SBI_ENOTSUPP;
		भङ्ग;
#endif
	स्वभाव:
		ret = SBI_ENOTSUPP;
		भङ्ग;
	}

	प्रत्यागम ret;
}
