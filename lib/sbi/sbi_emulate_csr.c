/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_encoding.h>
#شامل <sbi/sbi_bitops.h>
#شامل <sbi/sbi_emulate_csr.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_timer.h>
#شامل <sbi/sbi_trap.h>

ایستا bool hpm_allowed(صحیح hpm_رقم, ulong prev_mode, bool virt)
{
	ulong cen = -1UL;
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	اگر (prev_mode <= PRV_S) {
		اگر (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10) {
			cen &= csr_read(CSR_MCOUNTEREN);
			اگر (virt)
				cen &= csr_read(CSR_HCOUNTEREN);
		} وگرنه {
			cen = 0;
		}
	}
	اگر (prev_mode == PRV_U) {
		اگر (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
			cen &= csr_read(CSR_SCOUNTEREN);
		وگرنه
			cen = 0;
	}

	بازگشت ((cen >> hpm_رقم) & 1) ? true : false;
}

صحیح sbi_emulate_csr_read(صحیح csr_رقم, ساختار sbi_trap_regs *regs,
			 ulong *csr_val)
{
	صحیح ret = 0;
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	انتخاب (csr_رقم) {
	حالت CSR_HTIMEDELTA:
		اگر (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta();
		وگرنه
			ret = SBI_ENOTSUPP;
		بشکن;
	حالت CSR_CYCLE:
		اگر (!hpm_allowed(csr_رقم - CSR_CYCLE, prev_mode, virt))
			بازگشت SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLE);
		بشکن;
	حالت CSR_TIME:
		اگر (!hpm_allowed(csr_رقم - CSR_CYCLE, prev_mode, virt))
			بازگشت SBI_ENOTSUPP;
		/*
		 * We emulate TIME CSR for both Host (HS/U-mode) and
		 * Guest (VS/VU-mode).
		 */
		*csr_val = (virt) ? sbi_timer_virt_مقدار():
				    sbi_timer_مقدار();
		بشکن;
	حالت CSR_INSTRET:
		اگر (!hpm_allowed(csr_رقم - CSR_CYCLE, prev_mode, virt))
			بازگشت SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRET);
		بشکن;

#اگر __riscv_xlen == 32
	حالت CSR_HTIMEDELTAH:
		اگر (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta() >> 32;
		وگرنه
			ret = SBI_ENOTSUPP;
		بشکن;
	حالت CSR_CYCLEH:
		اگر (!hpm_allowed(csr_رقم - CSR_CYCLEH, prev_mode, virt))
			بازگشت SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLEH);
		بشکن;
	حالت CSR_TIMEH:
		اگر (!hpm_allowed(csr_رقم - CSR_CYCLEH, prev_mode, virt))
			بازگشت SBI_ENOTSUPP;
		*csr_val = (virt) ? sbi_timer_virt_مقدار() >> 32:
				    sbi_timer_مقدار() >> 32;
		بشکن;
	حالت CSR_INSTRETH:
		اگر (!hpm_allowed(csr_رقم - CSR_CYCLEH, prev_mode, virt))
			بازگشت SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRETH);
		بشکن;
#endif

#define switchcase_hpm(__uref, __mref, __csr)				\
	حالت __csr:							\
		اگر (sbi_hart_mhpm_mask(scratch) & (1 << (__csr - __uref)))\
			بازگشت SBI_ENOTSUPP;				\
		اگر (!hpm_allowed(__csr - __uref, prev_mode, virt))	\
			بازگشت SBI_ENOTSUPP;				\
		*csr_val = csr_read(__mref + __csr - __uref);		\
		بشکن;
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

#اگر __riscv_xlen == 32
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

	پیشفرض:
		ret = SBI_ENOTSUPP;
		بشکن;
	}

	بازگشت ret;
}

صحیح sbi_emulate_csr_write(صحیح csr_رقم, ساختار sbi_trap_regs *regs,
			  ulong csr_val)
{
	صحیح ret = 0;
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	انتخاب (csr_رقم) {
	حالت CSR_HTIMEDELTA:
		اگر (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta(csr_val);
		وگرنه
			ret = SBI_ENOTSUPP;
		بشکن;
#اگر __riscv_xlen == 32
	حالت CSR_HTIMEDELTAH:
		اگر (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta_upper(csr_val);
		وگرنه
			ret = SBI_ENOTSUPP;
		بشکن;
#endif
	پیشفرض:
		ret = SBI_ENOTSUPP;
		بشکن;
	}

	بازگشت ret;
}
