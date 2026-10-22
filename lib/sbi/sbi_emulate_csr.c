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

static bool hpm_allowed(puur_naa_nka hpm_num, ulong prev_mode, bool virt)
{
	ulong cen = -1UL;
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ᬬᬾᬦ᭄ (prev_mode <= PRV_S) {
		ᬬᬾᬦ᭄ (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10) {
			cen &= csr_read(CSR_MCOUNTEREN);
			ᬬᬾᬦ᭄ (virt)
				cen &= csr_read(CSR_HCOUNTEREN);
		} ᬮᬶᬬ {
			cen = 0;
		}
	}
	ᬬᬾᬦ᭄ (prev_mode == PRV_U) {
		ᬬᬾᬦ᭄ (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
			cen &= csr_read(CSR_SCOUNTEREN);
		ᬮᬶᬬ
			cen = 0;
	}

	ᬯᬮᬶ ((cen >> hpm_num) & 1) ? true : false;
}

puur_naa_nka sbi_emulate_csr_read(puur_naa_nka csr_num, sa_nracanaa sbi_trap_regs *regs,
			 ulong *csr_val)
{
	puur_naa_nka ret = 0;
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	switch (csr_num) {
	pak_sa CSR_HTIMEDELTA:
		ᬬᬾᬦ᭄ (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta();
		ᬮᬶᬬ
			ret = SBI_ENOTSUPP;
		bha_nga;
	pak_sa CSR_CYCLE:
		ᬬᬾᬦ᭄ (!hpm_allowed(csr_num - CSR_CYCLE, prev_mode, virt))
			ᬯᬮᬶ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLE);
		bha_nga;
	pak_sa CSR_TIME:
		ᬬᬾᬦ᭄ (!hpm_allowed(csr_num - CSR_CYCLE, prev_mode, virt))
			ᬯᬮᬶ SBI_ENOTSUPP;
		/*
		 * We emulate TIME CSR for both Host (HS/U-mode) and
		 * Guest (VS/VU-mode).
		 */
		*csr_val = (virt) ? sbi_timer_virt_value():
				    sbi_timer_value();
		bha_nga;
	pak_sa CSR_INSTRET:
		ᬬᬾᬦ᭄ (!hpm_allowed(csr_num - CSR_CYCLE, prev_mode, virt))
			ᬯᬮᬶ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRET);
		bha_nga;

#ᬬᬾᬦ᭄ __riscv_xlen == 32
	pak_sa CSR_HTIMEDELTAH:
		ᬬᬾᬦ᭄ (prev_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta() >> 32;
		ᬮᬶᬬ
			ret = SBI_ENOTSUPP;
		bha_nga;
	pak_sa CSR_CYCLEH:
		ᬬᬾᬦ᭄ (!hpm_allowed(csr_num - CSR_CYCLEH, prev_mode, virt))
			ᬯᬮᬶ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLEH);
		bha_nga;
	pak_sa CSR_TIMEH:
		ᬬᬾᬦ᭄ (!hpm_allowed(csr_num - CSR_CYCLEH, prev_mode, virt))
			ᬯᬮᬶ SBI_ENOTSUPP;
		*csr_val = (virt) ? sbi_timer_virt_value() >> 32:
				    sbi_timer_value() >> 32;
		bha_nga;
	pak_sa CSR_INSTRETH:
		ᬬᬾᬦ᭄ (!hpm_allowed(csr_num - CSR_CYCLEH, prev_mode, virt))
			ᬯᬮᬶ SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRETH);
		bha_nga;
#endif

#define switchcase_hpm(__uref, __mref, __csr)				\
	pak_sa __csr:							\
		ᬬᬾᬦ᭄ (sbi_hart_mhpm_mask(scratch) & (1 << (__csr - __uref)))\
			ᬯᬮᬶ SBI_ENOTSUPP;				\
		ᬬᬾᬦ᭄ (!hpm_allowed(__csr - __uref, prev_mode, virt))	\
			ᬯᬮᬶ SBI_ENOTSUPP;				\
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

#ᬬᬾᬦ᭄ __riscv_xlen == 32
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

	ᬯᬮᬶ ret;
}

puur_naa_nka sbi_emulate_csr_write(puur_naa_nka csr_num, sa_nracanaa sbi_trap_regs *regs,
			  ulong csr_val)
{
	puur_naa_nka ret = 0;
	ulong prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	switch (csr_num) {
	pak_sa CSR_HTIMEDELTA:
		ᬬᬾᬦ᭄ (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta(csr_val);
		ᬮᬶᬬ
			ret = SBI_ENOTSUPP;
		bha_nga;
#ᬬᬾᬦ᭄ __riscv_xlen == 32
	pak_sa CSR_HTIMEDELTAH:
		ᬬᬾᬦ᭄ (prev_mode == PRV_S && !virt)
			sbi_timer_set_delta_upper(csr_val);
		ᬮᬶᬬ
			ret = SBI_ENOTSUPP;
		bha_nga;
#endif
	default:
		ret = SBI_ENOTSUPP;
		bha_nga;
	}

	ᬯᬮᬶ ret;
}
