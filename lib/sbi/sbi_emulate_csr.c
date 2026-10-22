/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_encoding.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitops.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_emulate_csr.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_timer.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>

ಸ್ಥಿರ bool hpm_allowed(ಪೂರ್ಣಾಂಕ hpm_ಅಂಕೆ, ulong prev_ಬಹುಲಕ, bool virt)
{
	ulong cen = -1UL;
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ಆದರೆ (prev_ಬಹುಲಕ <= PRV_S) {
		ಆದರೆ (sbi_hart_priv_ಆವೃತ್ತಿ(scratch) >= SBI_HART_PRIV_VER_1_10) {
			cen &= csr_read(CSR_MCOUNTEREN);
			ಆದರೆ (virt)
				cen &= csr_read(CSR_HCOUNTEREN);
		} ಇಲ್ಲದಿದ್ದರೆ {
			cen = 0;
		}
	}
	ಆದರೆ (prev_ಬಹುಲಕ == PRV_U) {
		ಆದರೆ (sbi_hart_priv_ಆವೃತ್ತಿ(scratch) >= SBI_HART_PRIV_VER_1_10)
			cen &= csr_read(CSR_SCOUNTEREN);
		ಇಲ್ಲದಿದ್ದರೆ
			cen = 0;
	}

	ಹಿಂತಿರುಗಿ ((cen >> hpm_ಅಂಕೆ) & 1) ? true : false;
}

ಪೂರ್ಣಾಂಕ sbi_emulate_csr_read(ಪೂರ್ಣಾಂಕ csr_ಅಂಕೆ, ರಚನೆ sbi_trap_regs *regs,
			 ulong *csr_val)
{
	ಪೂರ್ಣಾಂಕ ret = 0;
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong prev_ಬಹುಲಕ = sbi_mstatus_prev_ಬಹುಲಕ(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	ಆಯ್ಕೆ (csr_ಅಂಕೆ) {
	ಸ್ಥಿತಿ CSR_HTIMEDELTA:
		ಆದರೆ (prev_ಬಹುಲಕ == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta();
		ಇಲ್ಲದಿದ್ದರೆ
			ret = SBI_ENOTSUPP;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CSR_ಚಕ್ರ:
		ಆದರೆ (!hpm_allowed(csr_ಅಂಕೆ - CSR_ಚಕ್ರ, prev_ಬಹುಲಕ, virt))
			ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLE);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CSR_ಸಮಯ:
		ಆದರೆ (!hpm_allowed(csr_ಅಂಕೆ - CSR_ಚಕ್ರ, prev_ಬಹುಲಕ, virt))
			ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;
		/*
		 * We emulate TIME CSR for both Host (HS/U-mode) and
		 * Guest (VS/VU-mode).
		 */
		*csr_val = (virt) ? sbi_timer_virt_ಮೌಲ್ಯ():
				    sbi_timer_ಮೌಲ್ಯ();
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CSR_INSTRET:
		ಆದರೆ (!hpm_allowed(csr_ಅಂಕೆ - CSR_ಚಕ್ರ, prev_ಬಹುಲಕ, virt))
			ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRET);
		ನಿಲ್ಲಿಸು;

#ಆದರೆ __riscv_xlen == 32
	ಸ್ಥಿತಿ CSR_HTIMEDELTAH:
		ಆದರೆ (prev_ಬಹುಲಕ == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta() >> 32;
		ಇಲ್ಲದಿದ್ದರೆ
			ret = SBI_ENOTSUPP;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CSR_CYCLEH:
		ಆದರೆ (!hpm_allowed(csr_ಅಂಕೆ - CSR_CYCLEH, prev_ಬಹುಲಕ, virt))
			ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLEH);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CSR_TIMEH:
		ಆದರೆ (!hpm_allowed(csr_ಅಂಕೆ - CSR_CYCLEH, prev_ಬಹುಲಕ, virt))
			ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;
		*csr_val = (virt) ? sbi_timer_virt_ಮೌಲ್ಯ() >> 32:
				    sbi_timer_ಮೌಲ್ಯ() >> 32;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CSR_INSTRETH:
		ಆದರೆ (!hpm_allowed(csr_ಅಂಕೆ - CSR_CYCLEH, prev_ಬಹುಲಕ, virt))
			ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRETH);
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು switchcase_hpm(__uref, __mref, __csr)				\
	ಸ್ಥಿತಿ __csr:							\
		ಆದರೆ (sbi_hart_mhpm_ಮುಖವಾಡ(scratch) & (1 << (__csr - __uref)))\
			ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;				\
		ಆದರೆ (!hpm_allowed(__csr - __uref, prev_ಬಹುಲಕ, virt))	\
			ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;				\
		*csr_val = csr_read(__mref + __csr - __uref);		\
		ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿಸು switchcase_hpm_2(__uref, __mref, __csr)			\
	switchcase_hpm(__uref, __mref, __csr + 0)			\
	switchcase_hpm(__uref, __mref, __csr + 1)
#ವ್ಯಾಖ್ಯಾನಿಸು switchcase_hpm_4(__uref, __mref, __csr)			\
	switchcase_hpm_2(__uref, __mref, __csr + 0)			\
	switchcase_hpm_2(__uref, __mref, __csr + 2)
#ವ್ಯಾಖ್ಯಾನಿಸು switchcase_hpm_8(__uref, __mref, __csr)			\
	switchcase_hpm_4(__uref, __mref, __csr + 0)			\
	switchcase_hpm_4(__uref, __mref, __csr + 4)
#ವ್ಯಾಖ್ಯಾನಿಸು switchcase_hpm_16(__uref, __mref, __csr)			\
	switchcase_hpm_8(__uref, __mref, __csr + 0)			\
	switchcase_hpm_8(__uref, __mref, __csr + 8)

	switchcase_hpm(CSR_ಚಕ್ರ, CSR_MCYCLE, CSR_HPMCOUNTER3)
	switchcase_hpm_4(CSR_ಚಕ್ರ, CSR_MCYCLE, CSR_HPMCOUNTER4)
	switchcase_hpm_8(CSR_ಚಕ್ರ, CSR_MCYCLE, CSR_HPMCOUNTER8)
	switchcase_hpm_16(CSR_ಚಕ್ರ, CSR_MCYCLE, CSR_HPMCOUNTER16)

#ಆದರೆ __riscv_xlen == 32
	switchcase_hpm(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER3H)
	switchcase_hpm_4(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER4H)
	switchcase_hpm_8(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER8H)
	switchcase_hpm_16(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER16H)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು switchcase_hpm_16
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು switchcase_hpm_8
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು switchcase_hpm_4
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು switchcase_hpm_2
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು switchcase_hpm

	ಪೂರ್ವನಿಯೋಜಿತ:
		ret = SBI_ENOTSUPP;
		ನಿಲ್ಲಿಸು;
	}

	ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ sbi_emulate_csr_write(ಪೂರ್ಣಾಂಕ csr_ಅಂಕೆ, ರಚನೆ sbi_trap_regs *regs,
			  ulong csr_val)
{
	ಪೂರ್ಣಾಂಕ ret = 0;
	ulong prev_ಬಹುಲಕ = sbi_mstatus_prev_ಬಹುಲಕ(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	ಆಯ್ಕೆ (csr_ಅಂಕೆ) {
	ಸ್ಥಿತಿ CSR_HTIMEDELTA:
		ಆದರೆ (prev_ಬಹುಲಕ == PRV_S && !virt)
			sbi_timer_ಗಣ_delta(csr_val);
		ಇಲ್ಲದಿದ್ದರೆ
			ret = SBI_ENOTSUPP;
		ನಿಲ್ಲಿಸು;
#ಆದರೆ __riscv_xlen == 32
	ಸ್ಥಿತಿ CSR_HTIMEDELTAH:
		ಆದರೆ (prev_ಬಹುಲಕ == PRV_S && !virt)
			sbi_timer_ಗಣ_delta_ಊರ್ಧ್ವ(csr_val);
		ಇಲ್ಲದಿದ್ದರೆ
			ret = SBI_ENOTSUPP;
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಪೂರ್ವನಿಯೋಜಿತ:
		ret = SBI_ENOTSUPP;
		ನಿಲ್ಲಿಸು;
	}

	ಹಿಂತಿರುಗಿ ret;
}
