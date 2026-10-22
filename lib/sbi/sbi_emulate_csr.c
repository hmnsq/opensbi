/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/riscv_encoding.h>
#അന്തര്ഭാവ <sbi/sbi_bitops.h>
#അന്തര്ഭാവ <sbi/sbi_emulate_csr.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi/sbi_timer.h>
#അന്തര്ഭാവ <sbi/sbi_trap.h>

സ്ഥിതം bool hpm_allowed(പൂർണ്ണസംഖ്യ hpm_അക്കം, ulong prev_mode, bool virt)
{
	ulong cen = -1UL;
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	എങ്കിൽ (prev_mode <= PRV_S) {
		എങ്കിൽ (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10) {
			cen &= csr_read(CSR_MCOUNTEREN);
			എങ്കിൽ (virt)
				cen &= csr_read(CSR_HCOUNTEREN);
		} അല്ലെങ്കിൽ {
			cen = 0;
		}
	}
	എങ്കിൽ (prev_mode == PRV_U) {
		എങ്കിൽ (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
			cen &= csr_read(CSR_SCOUNTEREN);
		അല്ലെങ്കിൽ
			cen = 0;
	}

	തിരികെ ((cen >> hpm_അക്കം) & 1) ? true : false;
}

പൂർണ്ണസംഖ്യ sbi_emulate_csr_read(പൂർണ്ണസംഖ്യ csr_അക്കം, ഘടന sbi_trap_regs *regs,
			 ulong *csr_val)
{
	പൂർണ്ണസംഖ്യ ret = 0;
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	തിരഞ്ഞെടുപ്പ് (csr_അക്കം) {
	അവസ്ഥ CSR_HTIMEDELTA:
		എങ്കിൽ (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta();
		അല്ലെങ്കിൽ
			ret = SBI_ENOTSUPP;
		നിർത്തുക;
	അവസ്ഥ CSR_CYCLE:
		എങ്കിൽ (!hpm_allowed(csr_അക്കം - CSR_CYCLE, prev_mode, virt))
			തിരികെ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLE);
		നിർത്തുക;
	അവസ്ഥ CSR_TIME:
		എങ്കിൽ (!hpm_allowed(csr_അക്കം - CSR_CYCLE, prev_mode, virt))
			തിരികെ SBI_ENOTSUPP;
		/*
		 * We emulate TIME CSR for both Host (HS/U-mode) and
		 * Guest (VS/VU-mode).
		 */
		*csr_val = (virt) ? sbi_timer_virt_മൂല്യം():
				    sbi_timer_മൂല്യം();
		നിർത്തുക;
	അവസ്ഥ CSR_INSTRET:
		എങ്കിൽ (!hpm_allowed(csr_അക്കം - CSR_CYCLE, prev_mode, virt))
			തിരികെ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRET);
		നിർത്തുക;

#എങ്കിൽ __riscv_xlen == 32
	അവസ്ഥ CSR_HTIMEDELTAH:
		എങ്കിൽ (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta() >> 32;
		അല്ലെങ്കിൽ
			ret = SBI_ENOTSUPP;
		നിർത്തുക;
	അവസ്ഥ CSR_CYCLEH:
		എങ്കിൽ (!hpm_allowed(csr_അക്കം - CSR_CYCLEH, prev_mode, virt))
			തിരികെ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLEH);
		നിർത്തുക;
	അവസ്ഥ CSR_TIMEH:
		എങ്കിൽ (!hpm_allowed(csr_അക്കം - CSR_CYCLEH, prev_mode, virt))
			തിരികെ SBI_ENOTSUPP;
		*csr_val = (virt) ? sbi_timer_virt_മൂല്യം() >> 32:
				    sbi_timer_മൂല്യം() >> 32;
		നിർത്തുക;
	അവസ്ഥ CSR_INSTRETH:
		എങ്കിൽ (!hpm_allowed(csr_അക്കം - CSR_CYCLEH, prev_mode, virt))
			തിരികെ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRETH);
		നിർത്തുക;
#endif

#define switchcase_hpm(__uref, __mref, __csr)				\
	അവസ്ഥ __csr:							\
		എങ്കിൽ (sbi_hart_mhpm_mask(scratch) & (1 << (__csr - __uref)))\
			തിരികെ SBI_ENOTSUPP;				\
		എങ്കിൽ (!hpm_allowed(__csr - __uref, prev_mode, virt))	\
			തിരികെ SBI_ENOTSUPP;				\
		*csr_val = csr_read(__mref + __csr - __uref);		\
		നിർത്തുക;
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

#എങ്കിൽ __riscv_xlen == 32
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

	സ്ഥിരസ്ഥിതി:
		ret = SBI_ENOTSUPP;
		നിർത്തുക;
	}

	തിരികെ ret;
}

പൂർണ്ണസംഖ്യ sbi_emulate_csr_write(പൂർണ്ണസംഖ്യ csr_അക്കം, ഘടന sbi_trap_regs *regs,
			  ulong csr_val)
{
	പൂർണ്ണസംഖ്യ ret = 0;
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	തിരഞ്ഞെടുപ്പ് (csr_അക്കം) {
	അവസ്ഥ CSR_HTIMEDELTA:
		എങ്കിൽ (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta(csr_val);
		അല്ലെങ്കിൽ
			ret = SBI_ENOTSUPP;
		നിർത്തുക;
#എങ്കിൽ __riscv_xlen == 32
	അവസ്ഥ CSR_HTIMEDELTAH:
		എങ്കിൽ (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta_upper(csr_val);
		അല്ലെങ്കിൽ
			ret = SBI_ENOTSUPP;
		നിർത്തുക;
#endif
	സ്ഥിരസ്ഥിതി:
		ret = SBI_ENOTSUPP;
		നിർത്തുക;
	}

	തിരികെ ret;
}
