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

ಸ್ಥಿರ bool hpm_allowed(ಪೂರ್ಣಾಙ್ಕ hpm_ಅಂಕೆ, ulong prev_mode, bool virt)
{
	ulong cen = -1UL;
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ಯದಿ (prev_mode <= PRV_S) {
		ಯದಿ (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10) {
			cen &= csr_read(CSR_MCOUNTEREN);
			ಯದಿ (virt)
				cen &= csr_read(CSR_HCOUNTEREN);
		} ಅನ್ಯಥಾ {
			cen = 0;
		}
	}
	ಯದಿ (prev_mode == PRV_U) {
		ಯದಿ (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
			cen &= csr_read(CSR_SCOUNTEREN);
		ಅನ್ಯಥಾ
			cen = 0;
	}

	ಪ್ರತ್ಯಾಗಮ ((cen >> hpm_ಅಂಕೆ) & 1) ? true : false;
}

ಪೂರ್ಣಾಙ್ಕ sbi_emulate_csr_read(ಪೂರ್ಣಾಙ್ಕ csr_ಅಂಕೆ, ಸಂರಚನಾ sbi_trap_regs *regs,
			 ulong *csr_val)
{
	ಪೂರ್ಣಾಙ್ಕ ret = 0;
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	ವಿಕಲ್ಪ (csr_ಅಂಕೆ) {
	ಪಕ್ಷ CSR_HTIMEDELTA:
		ಯದಿ (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta();
		ಅನ್ಯಥಾ
			ret = SBI_ENOTSUPP;
		ಭಙ್ಗ;
	ಪಕ್ಷ CSR_CYCLE:
		ಯದಿ (!hpm_allowed(csr_ಅಂಕೆ - CSR_CYCLE, prev_mode, virt))
			ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLE);
		ಭಙ್ಗ;
	ಪಕ್ಷ CSR_TIME:
		ಯದಿ (!hpm_allowed(csr_ಅಂಕೆ - CSR_CYCLE, prev_mode, virt))
			ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;
		/*
		 * We emulate TIME CSR for both Host (HS/U-mode) and
		 * Guest (VS/VU-mode).
		 */
		*csr_val = (virt) ? sbi_timer_virt_ಮೌಲ್ಯ():
				    sbi_timer_ಮೌಲ್ಯ();
		ಭಙ್ಗ;
	ಪಕ್ಷ CSR_INSTRET:
		ಯದಿ (!hpm_allowed(csr_ಅಂಕೆ - CSR_CYCLE, prev_mode, virt))
			ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRET);
		ಭಙ್ಗ;

#ಯದಿ __riscv_xlen == 32
	ಪಕ್ಷ CSR_HTIMEDELTAH:
		ಯದಿ (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta() >> 32;
		ಅನ್ಯಥಾ
			ret = SBI_ENOTSUPP;
		ಭಙ್ಗ;
	ಪಕ್ಷ CSR_CYCLEH:
		ಯದಿ (!hpm_allowed(csr_ಅಂಕೆ - CSR_CYCLEH, prev_mode, virt))
			ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLEH);
		ಭಙ್ಗ;
	ಪಕ್ಷ CSR_TIMEH:
		ಯದಿ (!hpm_allowed(csr_ಅಂಕೆ - CSR_CYCLEH, prev_mode, virt))
			ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;
		*csr_val = (virt) ? sbi_timer_virt_ಮೌಲ್ಯ() >> 32:
				    sbi_timer_ಮೌಲ್ಯ() >> 32;
		ಭಙ್ಗ;
	ಪಕ್ಷ CSR_INSTRETH:
		ಯದಿ (!hpm_allowed(csr_ಅಂಕೆ - CSR_CYCLEH, prev_mode, virt))
			ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRETH);
		ಭಙ್ಗ;
#endif

#define switchcase_hpm(__uref, __mref, __csr)				\
	ಪಕ್ಷ __csr:							\
		ಯದಿ (sbi_hart_mhpm_mask(scratch) & (1 << (__csr - __uref)))\
			ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;				\
		ಯದಿ (!hpm_allowed(__csr - __uref, prev_mode, virt))	\
			ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;				\
		*csr_val = csr_read(__mref + __csr - __uref);		\
		ಭಙ್ಗ;
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

#ಯದಿ __riscv_xlen == 32
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

	ಸ್ವಭಾವ:
		ret = SBI_ENOTSUPP;
		ಭಙ್ಗ;
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ sbi_emulate_csr_write(ಪೂರ್ಣಾಙ್ಕ csr_ಅಂಕೆ, ಸಂರಚನಾ sbi_trap_regs *regs,
			  ulong csr_val)
{
	ಪೂರ್ಣಾಙ್ಕ ret = 0;
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	ವಿಕಲ್ಪ (csr_ಅಂಕೆ) {
	ಪಕ್ಷ CSR_HTIMEDELTA:
		ಯದಿ (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta(csr_val);
		ಅನ್ಯಥಾ
			ret = SBI_ENOTSUPP;
		ಭಙ್ಗ;
#ಯದಿ __riscv_xlen == 32
	ಪಕ್ಷ CSR_HTIMEDELTAH:
		ಯದಿ (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta_upper(csr_val);
		ಅನ್ಯಥಾ
			ret = SBI_ENOTSUPP;
		ಭಙ್ಗ;
#endif
	ಸ್ವಭಾವ:
		ret = SBI_ENOTSUPP;
		ಭಙ್ಗ;
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}
