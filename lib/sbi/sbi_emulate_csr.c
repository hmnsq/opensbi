/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/riscv_encoding.h>
#அந்தர்பாவ <sbi/sbi_bitops.h>
#அந்தர்பாவ <sbi/sbi_emulate_csr.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi/sbi_timer.h>
#அந்தர்பாவ <sbi/sbi_trap.h>

நிலையான bool hpm_allowed(முழு hpm_இலக்கம், ulong prev_mode, bool virt)
{
	ulong cen = -1UL;
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	எனில் (prev_mode <= PRV_S) {
		எனில் (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10) {
			cen &= csr_read(CSR_MCOUNTEREN);
			எனில் (virt)
				cen &= csr_read(CSR_HCOUNTEREN);
		} இல்லையெனில் {
			cen = 0;
		}
	}
	எனில் (prev_mode == PRV_U) {
		எனில் (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
			cen &= csr_read(CSR_SCOUNTEREN);
		இல்லையெனில்
			cen = 0;
	}

	திரும்பு ((cen >> hpm_இலக்கம்) & 1) ? true : false;
}

முழு sbi_emulate_csr_read(முழு csr_இலக்கம், கட்டமைப்பு sbi_trap_regs *regs,
			 ulong *csr_val)
{
	முழு ret = 0;
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	தேர்வு (csr_இலக்கம்) {
	நிலை CSR_HTIMEDELTA:
		எனில் (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta();
		இல்லையெனில்
			ret = SBI_ENOTSUPP;
		நிறுத்து;
	நிலை CSR_CYCLE:
		எனில் (!hpm_allowed(csr_இலக்கம் - CSR_CYCLE, prev_mode, virt))
			திரும்பு SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLE);
		நிறுத்து;
	நிலை CSR_TIME:
		எனில் (!hpm_allowed(csr_இலக்கம் - CSR_CYCLE, prev_mode, virt))
			திரும்பு SBI_ENOTSUPP;
		/*
		 * We emulate TIME CSR for both Host (HS/U-mode) and
		 * Guest (VS/VU-mode).
		 */
		*csr_val = (virt) ? sbi_timer_virt_மதிப்பு():
				    sbi_timer_மதிப்பு();
		நிறுத்து;
	நிலை CSR_INSTRET:
		எனில் (!hpm_allowed(csr_இலக்கம் - CSR_CYCLE, prev_mode, virt))
			திரும்பு SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRET);
		நிறுத்து;

#எனில் __riscv_xlen == 32
	நிலை CSR_HTIMEDELTAH:
		எனில் (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta() >> 32;
		இல்லையெனில்
			ret = SBI_ENOTSUPP;
		நிறுத்து;
	நிலை CSR_CYCLEH:
		எனில் (!hpm_allowed(csr_இலக்கம் - CSR_CYCLEH, prev_mode, virt))
			திரும்பு SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLEH);
		நிறுத்து;
	நிலை CSR_TIMEH:
		எனில் (!hpm_allowed(csr_இலக்கம் - CSR_CYCLEH, prev_mode, virt))
			திரும்பு SBI_ENOTSUPP;
		*csr_val = (virt) ? sbi_timer_virt_மதிப்பு() >> 32:
				    sbi_timer_மதிப்பு() >> 32;
		நிறுத்து;
	நிலை CSR_INSTRETH:
		எனில் (!hpm_allowed(csr_இலக்கம் - CSR_CYCLEH, prev_mode, virt))
			திரும்பு SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRETH);
		நிறுத்து;
#endif

#define switchcase_hpm(__uref, __mref, __csr)				\
	நிலை __csr:							\
		எனில் (sbi_hart_mhpm_mask(scratch) & (1 << (__csr - __uref)))\
			திரும்பு SBI_ENOTSUPP;				\
		எனில் (!hpm_allowed(__csr - __uref, prev_mode, virt))	\
			திரும்பு SBI_ENOTSUPP;				\
		*csr_val = csr_read(__mref + __csr - __uref);		\
		நிறுத்து;
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

#எனில் __riscv_xlen == 32
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

	இயல்பு:
		ret = SBI_ENOTSUPP;
		நிறுத்து;
	}

	திரும்பு ret;
}

முழு sbi_emulate_csr_write(முழு csr_இலக்கம், கட்டமைப்பு sbi_trap_regs *regs,
			  ulong csr_val)
{
	முழு ret = 0;
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	தேர்வு (csr_இலக்கம்) {
	நிலை CSR_HTIMEDELTA:
		எனில் (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta(csr_val);
		இல்லையெனில்
			ret = SBI_ENOTSUPP;
		நிறுத்து;
#எனில் __riscv_xlen == 32
	நிலை CSR_HTIMEDELTAH:
		எனில் (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta_upper(csr_val);
		இல்லையெனில்
			ret = SBI_ENOTSUPP;
		நிறுத்து;
#endif
	இயல்பு:
		ret = SBI_ENOTSUPP;
		நிறுத்து;
	}

	திரும்பு ret;
}
