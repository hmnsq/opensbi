/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <sbi/riscv_asm.h>
#include <sbi/riscv_encoding.h>
#include <sbi/sbi_bitops.h>
#include <sbi/sbi_emulate_csr.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_scratch.h>
#include <sbi/sbi_timer.h>
#include <sbi/sbi_trap.h>

static bool hpm_allowed(ຈຳນວນເຕັມ hpm_num, ulong prev_mode, bool virt)
{
	ulong cen = -1UL;
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ຖ້າ (prev_mode <= PRV_S) {
		ຖ້າ (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10) {
			cen &= csr_read(CSR_MCOUNTEREN);
			ຖ້າ (virt)
				cen &= csr_read(CSR_HCOUNTEREN);
		} ບໍ່ດັ່ງນັ້ນ {
			cen = 0;
		}
	}
	ຖ້າ (prev_mode == PRV_U) {
		ຖ້າ (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
			cen &= csr_read(CSR_SCOUNTEREN);
		ບໍ່ດັ່ງນັ້ນ
			cen = 0;
	}

	ສົ່ງຄືນ ((cen >> hpm_num) & 1) ? true : false;
}

ຈຳນວນເຕັມ sbi_emulate_csr_read(ຈຳນວນເຕັມ csr_num, sa_nracanaa sbi_trap_regs *regs,
			 ulong *csr_val)
{
	ຈຳນວນເຕັມ ret = 0;
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	switch (csr_num) {
	pak_sa CSR_HTIMEDELTA:
		ຖ້າ (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta();
		ບໍ່ດັ່ງນັ້ນ
			ret = SBI_ENOTSUPP;
		bha_nga;
	pak_sa CSR_CYCLE:
		ຖ້າ (!hpm_allowed(csr_num - CSR_CYCLE, prev_mode, virt))
			ສົ່ງຄືນ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLE);
		bha_nga;
	pak_sa CSR_TIME:
		ຖ້າ (!hpm_allowed(csr_num - CSR_CYCLE, prev_mode, virt))
			ສົ່ງຄືນ SBI_ENOTSUPP;
		/*
		 * We emulate TIME CSR for both Host (HS/U-mode) and
		 * Guest (VS/VU-mode).
		 */
		*csr_val = (virt) ? sbi_timer_virt_value():
				    sbi_timer_value();
		bha_nga;
	pak_sa CSR_INSTRET:
		ຖ້າ (!hpm_allowed(csr_num - CSR_CYCLE, prev_mode, virt))
			ສົ່ງຄືນ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRET);
		bha_nga;

#ຖ້າ __riscv_xlen == 32
	pak_sa CSR_HTIMEDELTAH:
		ຖ້າ (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta() >> 32;
		ບໍ່ດັ່ງນັ້ນ
			ret = SBI_ENOTSUPP;
		bha_nga;
	pak_sa CSR_CYCLEH:
		ຖ້າ (!hpm_allowed(csr_num - CSR_CYCLEH, prev_mode, virt))
			ສົ່ງຄືນ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLEH);
		bha_nga;
	pak_sa CSR_TIMEH:
		ຖ້າ (!hpm_allowed(csr_num - CSR_CYCLEH, prev_mode, virt))
			ສົ່ງຄືນ SBI_ENOTSUPP;
		*csr_val = (virt) ? sbi_timer_virt_value() >> 32:
				    sbi_timer_value() >> 32;
		bha_nga;
	pak_sa CSR_INSTRETH:
		ຖ້າ (!hpm_allowed(csr_num - CSR_CYCLEH, prev_mode, virt))
			ສົ່ງຄືນ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRETH);
		bha_nga;
#endif

#define switchcase_hpm(__uref, __mref, __csr)				\
	pak_sa __csr:							\
		ຖ້າ (sbi_hart_mhpm_mask(scratch) & (1 << (__csr - __uref)))\
			ສົ່ງຄືນ SBI_ENOTSUPP;				\
		ຖ້າ (!hpm_allowed(__csr - __uref, prev_mode, virt))	\
			ສົ່ງຄືນ SBI_ENOTSUPP;				\
		*csr_val = csr_read(__mref + __csr - __uref);		\
		bha_nga;
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

#ຖ້າ __riscv_xlen == 32
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

	default:
		ret = SBI_ENOTSUPP;
		bha_nga;
	}

	ສົ່ງຄືນ ret;
}

ຈຳນວນເຕັມ sbi_emulate_csr_write(ຈຳນວນເຕັມ csr_num, sa_nracanaa sbi_trap_regs *regs,
			  ulong csr_val)
{
	ຈຳນວນເຕັມ ret = 0;
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	switch (csr_num) {
	pak_sa CSR_HTIMEDELTA:
		ຖ້າ (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta(csr_val);
		ບໍ່ດັ່ງນັ້ນ
			ret = SBI_ENOTSUPP;
		bha_nga;
#ຖ້າ __riscv_xlen == 32
	pak_sa CSR_HTIMEDELTAH:
		ຖ້າ (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta_upper(csr_val);
		ບໍ່ດັ່ງນັ້ນ
			ret = SBI_ENOTSUPP;
		bha_nga;
#endif
	default:
		ret = SBI_ENOTSUPP;
		bha_nga;
	}

	ສົ່ງຄືນ ret;
}
