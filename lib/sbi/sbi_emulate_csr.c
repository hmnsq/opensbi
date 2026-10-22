/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_encoding.h>
#ضمن <sbi/sbi_bitops.h>
#ضمن <sbi/sbi_emulate_csr.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi/sbi_timer.h>
#ضمن <sbi/sbi_trap.h>

ساكن bool hpm_allowed(صحيح hpm_num, ulong prev_mode, bool virt)
{
	ulong cen = -1UL;
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	𐡄𐡍 (prev_mode <= PRV_S) {
		𐡄𐡍 (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10) {
			cen &= csr_read(CSR_MCOUNTEREN);
			𐡄𐡍 (virt)
				cen &= csr_read(CSR_HCOUNTEREN);
		} وإلا {
			cen = 0;
		}
	}
	𐡄𐡍 (prev_mode == PRV_U) {
		𐡄𐡍 (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
			cen &= csr_read(CSR_SCOUNTEREN);
		وإلا
			cen = 0;
	}

	أرجع ((cen >> hpm_num) & 1) ? true : false;
}

صحيح sbi_emulate_csr_read(صحيح csr_num, بنية sbi_trap_regs *regs,
			 ulong *csr_val)
{
	صحيح ret = 0;
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	بدل (csr_num) {
	حالة CSR_HTIMEDELTA:
		𐡄𐡍 (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta();
		وإلا
			ret = SBI_ENOTSUPP;
		اكسر;
	حالة CSR_CYCLE:
		𐡄𐡍 (!hpm_allowed(csr_num - CSR_CYCLE, prev_mode, virt))
			أرجع SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLE);
		اكسر;
	حالة CSR_TIME:
		𐡄𐡍 (!hpm_allowed(csr_num - CSR_CYCLE, prev_mode, virt))
			أرجع SBI_ENOTSUPP;
		/*
		 * We emulate TIME CSR for both Host (HS/U-mode) and
		 * Guest (VS/VU-mode).
		 */
		*csr_val = (virt) ? sbi_timer_virt_value():
				    sbi_timer_value();
		اكسر;
	حالة CSR_INSTRET:
		𐡄𐡍 (!hpm_allowed(csr_num - CSR_CYCLE, prev_mode, virt))
			أرجع SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRET);
		اكسر;

#𐡄𐡍 __riscv_xlen == 32
	حالة CSR_HTIMEDELTAH:
		𐡄𐡍 (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta() >> 32;
		وإلا
			ret = SBI_ENOTSUPP;
		اكسر;
	حالة CSR_CYCLEH:
		𐡄𐡍 (!hpm_allowed(csr_num - CSR_CYCLEH, prev_mode, virt))
			أرجع SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLEH);
		اكسر;
	حالة CSR_TIMEH:
		𐡄𐡍 (!hpm_allowed(csr_num - CSR_CYCLEH, prev_mode, virt))
			أرجع SBI_ENOTSUPP;
		*csr_val = (virt) ? sbi_timer_virt_value() >> 32:
				    sbi_timer_value() >> 32;
		اكسر;
	حالة CSR_INSTRETH:
		𐡄𐡍 (!hpm_allowed(csr_num - CSR_CYCLEH, prev_mode, virt))
			أرجع SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRETH);
		اكسر;
#endif

#define switchcase_hpm(__uref, __mref, __csr)				\
	حالة __csr:							\
		𐡄𐡍 (sbi_hart_mhpm_mask(scratch) & (1 << (__csr - __uref)))\
			أرجع SBI_ENOTSUPP;				\
		𐡄𐡍 (!hpm_allowed(__csr - __uref, prev_mode, virt))	\
			أرجع SBI_ENOTSUPP;				\
		*csr_val = csr_read(__mref + __csr - __uref);		\
		اكسر;
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

#𐡄𐡍 __riscv_xlen == 32
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

	افتراضي:
		ret = SBI_ENOTSUPP;
		اكسر;
	}

	أرجع ret;
}

صحيح sbi_emulate_csr_write(صحيح csr_num, بنية sbi_trap_regs *regs,
			  ulong csr_val)
{
	صحيح ret = 0;
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	بدل (csr_num) {
	حالة CSR_HTIMEDELTA:
		𐡄𐡍 (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta(csr_val);
		وإلا
			ret = SBI_ENOTSUPP;
		اكسر;
#𐡄𐡍 __riscv_xlen == 32
	حالة CSR_HTIMEDELTAH:
		𐡄𐡍 (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta_upper(csr_val);
		وإلا
			ret = SBI_ENOTSUPP;
		اكسر;
#endif
	افتراضي:
		ret = SBI_ENOTSUPP;
		اكسر;
	}

	أرجع ret;
}
