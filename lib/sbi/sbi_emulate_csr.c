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

स्थिर bool hpm_allowed(पूर्णांक hpm_अंक, ulong prev_mode, bool virt)
{
	ulong cen = -1UL;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	जर (prev_mode <= PRV_S) {
		जर (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10) {
			cen &= csr_read(CSR_MCOUNTEREN);
			जर (virt)
				cen &= csr_read(CSR_HCOUNTEREN);
		} अन्यथा {
			cen = 0;
		}
	}
	जर (prev_mode == PRV_U) {
		जर (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
			cen &= csr_read(CSR_SCOUNTEREN);
		अन्यथा
			cen = 0;
	}

	परत ((cen >> hpm_अंक) & 1) ? true : false;
}

पूर्णांक sbi_emulate_csr_read(पूर्णांक csr_अंक, संरचना sbi_trap_regs *regs,
			 ulong *csr_val)
{
	पूर्णांक ret = 0;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	निवड (csr_अंक) {
	स्थिती CSR_HTIMEDELTA:
		जर (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta();
		अन्यथा
			ret = SBI_ENOTSUPP;
		थांबा;
	स्थिती CSR_CYCLE:
		जर (!hpm_allowed(csr_अंक - CSR_CYCLE, prev_mode, virt))
			परत SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLE);
		थांबा;
	स्थिती CSR_TIME:
		जर (!hpm_allowed(csr_अंक - CSR_CYCLE, prev_mode, virt))
			परत SBI_ENOTSUPP;
		/*
		 * We emulate TIME CSR for both Host (HS/U-mode) and
		 * Guest (VS/VU-mode).
		 */
		*csr_val = (virt) ? sbi_timer_virt_मूल्य():
				    sbi_timer_मूल्य();
		थांबा;
	स्थिती CSR_INSTRET:
		जर (!hpm_allowed(csr_अंक - CSR_CYCLE, prev_mode, virt))
			परत SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRET);
		थांबा;

#जर __riscv_xlen == 32
	स्थिती CSR_HTIMEDELTAH:
		जर (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta() >> 32;
		अन्यथा
			ret = SBI_ENOTSUPP;
		थांबा;
	स्थिती CSR_CYCLEH:
		जर (!hpm_allowed(csr_अंक - CSR_CYCLEH, prev_mode, virt))
			परत SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLEH);
		थांबा;
	स्थिती CSR_TIMEH:
		जर (!hpm_allowed(csr_अंक - CSR_CYCLEH, prev_mode, virt))
			परत SBI_ENOTSUPP;
		*csr_val = (virt) ? sbi_timer_virt_मूल्य() >> 32:
				    sbi_timer_मूल्य() >> 32;
		थांबा;
	स्थिती CSR_INSTRETH:
		जर (!hpm_allowed(csr_अंक - CSR_CYCLEH, prev_mode, virt))
			परत SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRETH);
		थांबा;
#endif

#define switchcase_hpm(__uref, __mref, __csr)				\
	स्थिती __csr:							\
		जर (sbi_hart_mhpm_mask(scratch) & (1 << (__csr - __uref)))\
			परत SBI_ENOTSUPP;				\
		जर (!hpm_allowed(__csr - __uref, prev_mode, virt))	\
			परत SBI_ENOTSUPP;				\
		*csr_val = csr_read(__mref + __csr - __uref);		\
		थांबा;
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

#जर __riscv_xlen == 32
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

	पूर्वनिर्धारित:
		ret = SBI_ENOTSUPP;
		थांबा;
	}

	परत ret;
}

पूर्णांक sbi_emulate_csr_write(पूर्णांक csr_अंक, संरचना sbi_trap_regs *regs,
			  ulong csr_val)
{
	पूर्णांक ret = 0;
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	निवड (csr_अंक) {
	स्थिती CSR_HTIMEDELTA:
		जर (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta(csr_val);
		अन्यथा
			ret = SBI_ENOTSUPP;
		थांबा;
#जर __riscv_xlen == 32
	स्थिती CSR_HTIMEDELTAH:
		जर (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta_upper(csr_val);
		अन्यथा
			ret = SBI_ENOTSUPP;
		थांबा;
#endif
	पूर्वनिर्धारित:
		ret = SBI_ENOTSUPP;
		थांबा;
	}

	परत ret;
}
