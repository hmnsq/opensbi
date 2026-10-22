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

ساکن bool hpm_allowed(صحیح hpm_عدد, ulong پچھلا_منوال, bool virt)
{
	ulong cen = -1UL;
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	اگر (پچھلا_منوال <= PRV_S) {
		اگر (sbi_hart_priv_نسخہ(scratch) >= SBI_HART_PRIV_VER_1_10) {
			cen &= csr_read(CSR_MCOUNTEREN);
			اگر (virt)
				cen &= csr_read(CSR_HCOUNTEREN);
		} ورنہ {
			cen = 0;
		}
	}
	اگر (پچھلا_منوال == PRV_U) {
		اگر (sbi_hart_priv_نسخہ(scratch) >= SBI_HART_PRIV_VER_1_10)
			cen &= csr_read(CSR_SCOUNTEREN);
		ورنہ
			cen = 0;
	}

	واپس ((cen >> hpm_عدد) & 1) ? true : false;
}

صحیح sbi_emulate_csr_read(صحیح csr_عدد, ساخت sbi_trap_regs *regs,
			 ulong *csr_val)
{
	صحیح ret = 0;
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong پچھلا_منوال = sbi_mstatus_پچھلا_منوال(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	انتخاب (csr_عدد) {
	حالت CSR_HTIMEDELTA:
		اگر (پچھلا_منوال == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta();
		ورنہ
			ret = SBI_ENOTSUPP;
		توڑو;
	حالت CSR_چکر:
		اگر (!hpm_allowed(csr_عدد - CSR_چکر, پچھلا_منوال, virt))
			واپس SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLE);
		توڑو;
	حالت CSR_وقت:
		اگر (!hpm_allowed(csr_عدد - CSR_چکر, پچھلا_منوال, virt))
			واپس SBI_ENOTSUPP;
		/*
		 * We emulate TIME CSR for both Host (HS/U-mode) and
		 * Guest (VS/VU-mode).
		 */
		*csr_val = (virt) ? sbi_timer_virt_قدر():
				    sbi_timer_قدر();
		توڑو;
	حالت CSR_INSTRET:
		اگر (!hpm_allowed(csr_عدد - CSR_چکر, پچھلا_منوال, virt))
			واپس SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRET);
		توڑو;

#اگر __riscv_xlen == 32
	حالت CSR_HTIMEDELTAH:
		اگر (پچھلا_منوال == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta() >> 32;
		ورنہ
			ret = SBI_ENOTSUPP;
		توڑو;
	حالت CSR_CYCLEH:
		اگر (!hpm_allowed(csr_عدد - CSR_CYCLEH, پچھلا_منوال, virt))
			واپس SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLEH);
		توڑو;
	حالت CSR_TIMEH:
		اگر (!hpm_allowed(csr_عدد - CSR_CYCLEH, پچھلا_منوال, virt))
			واپس SBI_ENOTSUPP;
		*csr_val = (virt) ? sbi_timer_virt_قدر() >> 32:
				    sbi_timer_قدر() >> 32;
		توڑو;
	حالت CSR_INSTRETH:
		اگر (!hpm_allowed(csr_عدد - CSR_CYCLEH, پچھلا_منوال, virt))
			واپس SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRETH);
		توڑو;
#اگر_ختم

#تعریف_کرو switchcase_hpm(__uref, __mref, __csr)				\
	حالت __csr:							\
		اگر (sbi_hart_mhpm_نقاب(scratch) & (1 << (__csr - __uref)))\
			واپس SBI_ENOTSUPP;				\
		اگر (!hpm_allowed(__csr - __uref, پچھلا_منوال, virt))	\
			واپس SBI_ENOTSUPP;				\
		*csr_val = csr_read(__mref + __csr - __uref);		\
		توڑو;
#تعریف_کرو switchcase_hpm_2(__uref, __mref, __csr)			\
	switchcase_hpm(__uref, __mref, __csr + 0)			\
	switchcase_hpm(__uref, __mref, __csr + 1)
#تعریف_کرو switchcase_hpm_4(__uref, __mref, __csr)			\
	switchcase_hpm_2(__uref, __mref, __csr + 0)			\
	switchcase_hpm_2(__uref, __mref, __csr + 2)
#تعریف_کرو switchcase_hpm_8(__uref, __mref, __csr)			\
	switchcase_hpm_4(__uref, __mref, __csr + 0)			\
	switchcase_hpm_4(__uref, __mref, __csr + 4)
#تعریف_کرو switchcase_hpm_16(__uref, __mref, __csr)			\
	switchcase_hpm_8(__uref, __mref, __csr + 0)			\
	switchcase_hpm_8(__uref, __mref, __csr + 8)

	switchcase_hpm(CSR_چکر, CSR_MCYCLE, CSR_HPMCOUNTER3)
	switchcase_hpm_4(CSR_چکر, CSR_MCYCLE, CSR_HPMCOUNTER4)
	switchcase_hpm_8(CSR_چکر, CSR_MCYCLE, CSR_HPMCOUNTER8)
	switchcase_hpm_16(CSR_چکر, CSR_MCYCLE, CSR_HPMCOUNTER16)

#اگر __riscv_xlen == 32
	switchcase_hpm(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER3H)
	switchcase_hpm_4(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER4H)
	switchcase_hpm_8(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER8H)
	switchcase_hpm_16(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER16H)
#اگر_ختم

#تعریف_منسوخ switchcase_hpm_16
#تعریف_منسوخ switchcase_hpm_8
#تعریف_منسوخ switchcase_hpm_4
#تعریف_منسوخ switchcase_hpm_2
#تعریف_منسوخ switchcase_hpm

	طےشدہ:
		ret = SBI_ENOTSUPP;
		توڑو;
	}

	واپس ret;
}

صحیح sbi_emulate_csr_write(صحیح csr_عدد, ساخت sbi_trap_regs *regs,
			  ulong csr_val)
{
	صحیح ret = 0;
	ulong پچھلا_منوال = sbi_mstatus_پچھلا_منوال(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	انتخاب (csr_عدد) {
	حالت CSR_HTIMEDELTA:
		اگر (پچھلا_منوال == PRV_S && !virt)
			sbi_timer_سیٹ_delta(csr_val);
		ورنہ
			ret = SBI_ENOTSUPP;
		توڑو;
#اگر __riscv_xlen == 32
	حالت CSR_HTIMEDELTAH:
		اگر (پچھلا_منوال == PRV_S && !virt)
			sbi_timer_سیٹ_delta_بالائی(csr_val);
		ورنہ
			ret = SBI_ENOTSUPP;
		توڑو;
#اگر_ختم
	طےشدہ:
		ret = SBI_ENOTSUPP;
		توڑو;
	}

	واپس ret;
}
