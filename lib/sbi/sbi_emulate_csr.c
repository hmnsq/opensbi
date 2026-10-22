/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_encoding.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitops.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_emulate_csr.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>

ਸਥਿਰ bool hpm_allowed(ਪੂਰਨ hpm_ਅੰਕ, ulong prev_ਬਹੁਲਕ, bool virt)
{
	ulong cen = -1UL;
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ਜੇ (prev_ਬਹੁਲਕ <= PRV_S) {
		ਜੇ (sbi_hart_priv_ਸੰਸਕਰਣ(scratch) >= SBI_HART_PRIV_VER_1_10) {
			cen &= csr_read(CSR_MCOUNTEREN);
			ਜੇ (virt)
				cen &= csr_read(CSR_HCOUNTEREN);
		} ਨਹੀਂਤਾਂ {
			cen = 0;
		}
	}
	ਜੇ (prev_ਬਹੁਲਕ == PRV_U) {
		ਜੇ (sbi_hart_priv_ਸੰਸਕਰਣ(scratch) >= SBI_HART_PRIV_VER_1_10)
			cen &= csr_read(CSR_SCOUNTEREN);
		ਨਹੀਂਤਾਂ
			cen = 0;
	}

	ਵਾਪਸ ((cen >> hpm_ਅੰਕ) & 1) ? true : false;
}

ਪੂਰਨ sbi_emulate_csr_read(ਪੂਰਨ csr_ਅੰਕ, ਬਣਤਰ sbi_trap_regs *regs,
			 ulong *csr_val)
{
	ਪੂਰਨ ret = 0;
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong prev_ਬਹੁਲਕ = sbi_mstatus_prev_ਬਹੁਲਕ(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	ਚੋਣ (csr_ਅੰਕ) {
	ਹਾਲਤ CSR_HTIMEDELTA:
		ਜੇ (prev_ਬਹੁਲਕ == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta();
		ਨਹੀਂਤਾਂ
			ret = SBI_ENOTSUPP;
		ਰੋਕੋ;
	ਹਾਲਤ CSR_ਚੱਕਰ:
		ਜੇ (!hpm_allowed(csr_ਅੰਕ - CSR_ਚੱਕਰ, prev_ਬਹੁਲਕ, virt))
			ਵਾਪਸ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLE);
		ਰੋਕੋ;
	ਹਾਲਤ CSR_ਸਮਾਂ:
		ਜੇ (!hpm_allowed(csr_ਅੰਕ - CSR_ਚੱਕਰ, prev_ਬਹੁਲਕ, virt))
			ਵਾਪਸ SBI_ENOTSUPP;
		/*
		 * We emulate TIME CSR for both Host (HS/U-mode) and
		 * Guest (VS/VU-mode).
		 */
		*csr_val = (virt) ? sbi_timer_virt_ਮੁੱਲ():
				    sbi_timer_ਮੁੱਲ();
		ਰੋਕੋ;
	ਹਾਲਤ CSR_INSTRET:
		ਜੇ (!hpm_allowed(csr_ਅੰਕ - CSR_ਚੱਕਰ, prev_ਬਹੁਲਕ, virt))
			ਵਾਪਸ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRET);
		ਰੋਕੋ;

#ਜੇ __riscv_xlen == 32
	ਹਾਲਤ CSR_HTIMEDELTAH:
		ਜੇ (prev_ਬਹੁਲਕ == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta() >> 32;
		ਨਹੀਂਤਾਂ
			ret = SBI_ENOTSUPP;
		ਰੋਕੋ;
	ਹਾਲਤ CSR_CYCLEH:
		ਜੇ (!hpm_allowed(csr_ਅੰਕ - CSR_CYCLEH, prev_ਬਹੁਲਕ, virt))
			ਵਾਪਸ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLEH);
		ਰੋਕੋ;
	ਹਾਲਤ CSR_TIMEH:
		ਜੇ (!hpm_allowed(csr_ਅੰਕ - CSR_CYCLEH, prev_ਬਹੁਲਕ, virt))
			ਵਾਪਸ SBI_ENOTSUPP;
		*csr_val = (virt) ? sbi_timer_virt_ਮੁੱਲ() >> 32:
				    sbi_timer_ਮੁੱਲ() >> 32;
		ਰੋਕੋ;
	ਹਾਲਤ CSR_INSTRETH:
		ਜੇ (!hpm_allowed(csr_ਅੰਕ - CSR_CYCLEH, prev_ਬਹੁਲਕ, virt))
			ਵਾਪਸ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRETH);
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ switchcase_hpm(__uref, __mref, __csr)				\
	ਹਾਲਤ __csr:							\
		ਜੇ (sbi_hart_mhpm_ਨਕਾਬ(scratch) & (1 << (__csr - __uref)))\
			ਵਾਪਸ SBI_ENOTSUPP;				\
		ਜੇ (!hpm_allowed(__csr - __uref, prev_ਬਹੁਲਕ, virt))	\
			ਵਾਪਸ SBI_ENOTSUPP;				\
		*csr_val = csr_read(__mref + __csr - __uref);		\
		ਰੋਕੋ;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ switchcase_hpm_2(__uref, __mref, __csr)			\
	switchcase_hpm(__uref, __mref, __csr + 0)			\
	switchcase_hpm(__uref, __mref, __csr + 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ switchcase_hpm_4(__uref, __mref, __csr)			\
	switchcase_hpm_2(__uref, __mref, __csr + 0)			\
	switchcase_hpm_2(__uref, __mref, __csr + 2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ switchcase_hpm_8(__uref, __mref, __csr)			\
	switchcase_hpm_4(__uref, __mref, __csr + 0)			\
	switchcase_hpm_4(__uref, __mref, __csr + 4)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ switchcase_hpm_16(__uref, __mref, __csr)			\
	switchcase_hpm_8(__uref, __mref, __csr + 0)			\
	switchcase_hpm_8(__uref, __mref, __csr + 8)

	switchcase_hpm(CSR_ਚੱਕਰ, CSR_MCYCLE, CSR_HPMCOUNTER3)
	switchcase_hpm_4(CSR_ਚੱਕਰ, CSR_MCYCLE, CSR_HPMCOUNTER4)
	switchcase_hpm_8(CSR_ਚੱਕਰ, CSR_MCYCLE, CSR_HPMCOUNTER8)
	switchcase_hpm_16(CSR_ਚੱਕਰ, CSR_MCYCLE, CSR_HPMCOUNTER16)

#ਜੇ __riscv_xlen == 32
	switchcase_hpm(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER3H)
	switchcase_hpm_4(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER4H)
	switchcase_hpm_8(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER8H)
	switchcase_hpm_16(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER16H)
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ switchcase_hpm_16
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ switchcase_hpm_8
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ switchcase_hpm_4
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ switchcase_hpm_2
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ switchcase_hpm

	ਮੂਲ:
		ret = SBI_ENOTSUPP;
		ਰੋਕੋ;
	}

	ਵਾਪਸ ret;
}

ਪੂਰਨ sbi_emulate_csr_write(ਪੂਰਨ csr_ਅੰਕ, ਬਣਤਰ sbi_trap_regs *regs,
			  ulong csr_val)
{
	ਪੂਰਨ ret = 0;
	ulong prev_ਬਹੁਲਕ = sbi_mstatus_prev_ਬਹੁਲਕ(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	ਚੋਣ (csr_ਅੰਕ) {
	ਹਾਲਤ CSR_HTIMEDELTA:
		ਜੇ (prev_ਬਹੁਲਕ == PRV_S && !virt)
			sbi_timer_ਸੈੱਟ_delta(csr_val);
		ਨਹੀਂਤਾਂ
			ret = SBI_ENOTSUPP;
		ਰੋਕੋ;
#ਜੇ __riscv_xlen == 32
	ਹਾਲਤ CSR_HTIMEDELTAH:
		ਜੇ (prev_ਬਹੁਲਕ == PRV_S && !virt)
			sbi_timer_ਸੈੱਟ_delta_ਉੱਤਲਾ(csr_val);
		ਨਹੀਂਤਾਂ
			ret = SBI_ENOTSUPP;
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਮੂਲ:
		ret = SBI_ENOTSUPP;
		ਰੋਕੋ;
	}

	ਵਾਪਸ ret;
}
