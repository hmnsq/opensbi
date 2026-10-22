/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/riscv_encoding.h>
#समावेश <sbi/sbi_bitops.h>
#समावेश <sbi/sbi_emulate_csr.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi/sbi_timer.h>
#समावेश <sbi/sbi_trap.h>

अटल bool hpm_allowed(पूर्णांक hpm_अंक, ulong पूर्व_mode, bool virt)
{
	ulong cen = -1UL;
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	अगर (पूर्व_mode <= PRV_S) {
		अगर (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10) {
			cen &= csr_read(CSR_MCOUNTEREN);
			अगर (virt)
				cen &= csr_read(CSR_HCOUNTEREN);
		} अन्यथा {
			cen = 0;
		}
	}
	अगर (पूर्व_mode == PRV_U) {
		अगर (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
			cen &= csr_read(CSR_SCOUNTEREN);
		अन्यथा
			cen = 0;
	}

	वापस ((cen >> hpm_अंक) & 1) ? true : false;
}

पूर्णांक sbi_emulate_csr_read(पूर्णांक csr_अंक, काष्ठा sbi_trap_regs *regs,
			 ulong *csr_val)
{
	पूर्णांक ret = 0;
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong पूर्व_mode = sbi_mstatus_पूर्व_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	चयन (csr_अंक) {
	हाल CSR_HTIMEDELTA:
		अगर (पूर्व_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta();
		अन्यथा
			ret = SBI_ENOTSUPP;
		अवरोध;
	हाल CSR_CYCLE:
		अगर (!hpm_allowed(csr_अंक - CSR_CYCLE, पूर्व_mode, virt))
			वापस SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLE);
		अवरोध;
	हाल CSR_TIME:
		अगर (!hpm_allowed(csr_अंक - CSR_CYCLE, पूर्व_mode, virt))
			वापस SBI_ENOTSUPP;
		/*
		 * We emulate TIME CSR for both Host (HS/U-mode) and
		 * Guest (VS/VU-mode).
		 */
		*csr_val = (virt) ? sbi_timer_virt_मूल्य():
				    sbi_timer_मूल्य();
		अवरोध;
	हाल CSR_INSTRET:
		अगर (!hpm_allowed(csr_अंक - CSR_CYCLE, पूर्व_mode, virt))
			वापस SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRET);
		अवरोध;

#अगर __riscv_xlen == 32
	हाल CSR_HTIMEDELTAH:
		अगर (पूर्व_mode == PRV_S && !virt)
			*csr_val = sbi_timer_get_delta() >> 32;
		अन्यथा
			ret = SBI_ENOTSUPP;
		अवरोध;
	हाल CSR_CYCLEH:
		अगर (!hpm_allowed(csr_अंक - CSR_CYCLEH, पूर्व_mode, virt))
			वापस SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MCYCLEH);
		अवरोध;
	हाल CSR_TIMEH:
		अगर (!hpm_allowed(csr_अंक - CSR_CYCLEH, पूर्व_mode, virt))
			वापस SBI_ENOTSUPP;
		*csr_val = (virt) ? sbi_timer_virt_मूल्य() >> 32:
				    sbi_timer_मूल्य() >> 32;
		अवरोध;
	हाल CSR_INSTRETH:
		अगर (!hpm_allowed(csr_अंक - CSR_CYCLEH, पूर्व_mode, virt))
			वापस SBI_ENOTSUPP;
		*csr_val = csr_read(CSR_MINSTRETH);
		अवरोध;
#पूर्ण_अगर

#घोषणा switchcase_hpm(__uref, __mref, __csr)				\
	हाल __csr:							\
		अगर (sbi_hart_mhpm_mask(scratch) & (1 << (__csr - __uref)))\
			वापस SBI_ENOTSUPP;				\
		अगर (!hpm_allowed(__csr - __uref, पूर्व_mode, virt))	\
			वापस SBI_ENOTSUPP;				\
		*csr_val = csr_read(__mref + __csr - __uref);		\
		अवरोध;
#घोषणा switchcase_hpm_2(__uref, __mref, __csr)			\
	switchcase_hpm(__uref, __mref, __csr + 0)			\
	switchcase_hpm(__uref, __mref, __csr + 1)
#घोषणा switchcase_hpm_4(__uref, __mref, __csr)			\
	switchcase_hpm_2(__uref, __mref, __csr + 0)			\
	switchcase_hpm_2(__uref, __mref, __csr + 2)
#घोषणा switchcase_hpm_8(__uref, __mref, __csr)			\
	switchcase_hpm_4(__uref, __mref, __csr + 0)			\
	switchcase_hpm_4(__uref, __mref, __csr + 4)
#घोषणा switchcase_hpm_16(__uref, __mref, __csr)			\
	switchcase_hpm_8(__uref, __mref, __csr + 0)			\
	switchcase_hpm_8(__uref, __mref, __csr + 8)

	switchcase_hpm(CSR_CYCLE, CSR_MCYCLE, CSR_HPMCOUNTER3)
	switchcase_hpm_4(CSR_CYCLE, CSR_MCYCLE, CSR_HPMCOUNTER4)
	switchcase_hpm_8(CSR_CYCLE, CSR_MCYCLE, CSR_HPMCOUNTER8)
	switchcase_hpm_16(CSR_CYCLE, CSR_MCYCLE, CSR_HPMCOUNTER16)

#अगर __riscv_xlen == 32
	switchcase_hpm(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER3H)
	switchcase_hpm_4(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER4H)
	switchcase_hpm_8(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER8H)
	switchcase_hpm_16(CSR_CYCLEH, CSR_MCYCLEH, CSR_HPMCOUNTER16H)
#पूर्ण_अगर

#अघोषित switchcase_hpm_16
#अघोषित switchcase_hpm_8
#अघोषित switchcase_hpm_4
#अघोषित switchcase_hpm_2
#अघोषित switchcase_hpm

	शेष:
		ret = SBI_ENOTSUPP;
		अवरोध;
	}

	वापस ret;
}

पूर्णांक sbi_emulate_csr_write(पूर्णांक csr_अंक, काष्ठा sbi_trap_regs *regs,
			  ulong csr_val)
{
	पूर्णांक ret = 0;
	ulong पूर्व_mode = sbi_mstatus_पूर्व_mode(regs->mstatus);
	bool virt = sbi_regs_from_virt(regs);

	चयन (csr_अंक) {
	हाल CSR_HTIMEDELTA:
		अगर (पूर्व_mode == PRV_S && !virt)
			sbi_timer_set_delta(csr_val);
		अन्यथा
			ret = SBI_ENOTSUPP;
		अवरोध;
#अगर __riscv_xlen == 32
	हाल CSR_HTIMEDELTAH:
		अगर (पूर्व_mode == PRV_S && !virt)
			sbi_timer_set_delta_upper(csr_val);
		अन्यथा
			ret = SBI_ENOTSUPP;
		अवरोध;
#पूर्ण_अगर
	शेष:
		ret = SBI_ENOTSUPP;
		अवरोध;
	}

	वापस ret;
}
