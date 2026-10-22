/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_ecall.h>
#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_ipi.h>
#समावेश <sbi/sbi_platform.h>
#समावेश <sbi/sbi_system.h>
#समावेश <sbi/sbi_timer.h>
#समावेश <sbi/sbi_tlb.h>
#समावेश <sbi/sbi_trap.h>
#समावेश <sbi/sbi_unpriv.h>
#समावेश <sbi/sbi_hart.h>

अटल bool sbi_load_hart_mask_unpriv(ulong *pmask, ulong *hmask, ulong *hbase,
				      काष्ठा sbi_trap_info *uptrap)
{
	ulong mask = 0;

	अगर (pmask) {
		mask = sbi_load_ulong(pmask, uptrap);
		अगर (uptrap->cause)
			वापस false;
		*hbase = 0;
	} अन्यथा {
		*hbase = -1UL;
	}
	*hmask = mask;

	वापस true;
}

अटल पूर्णांक sbi_ecall_legacy_handler(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ funcid,
				    काष्ठा sbi_trap_regs *regs,
				    काष्ठा sbi_ecall_return *out)
{
	पूर्णांक ret = 0;
	काष्ठा sbi_tlb_info tlb_info;
	u32 source_hart = current_hartid();
	काष्ठा sbi_trap_info trap = {0};
	ulong hmask, hbase;

	चयन (extid) {
	हाल SBI_EXT_0_1_SET_TIMER:
#अगर __riscv_xlen == 32
		sbi_timer_smode_event_start((((u64)regs->a1 << 32) | (u64)regs->a0));
#अन्यथा
		sbi_timer_smode_event_start((u64)regs->a0);
#पूर्ण_अगर
		अवरोध;
	हाल SBI_EXT_0_1_CONSOLE_PUTCHAR:
		sbi_putc(regs->a0);
		अवरोध;
	हाल SBI_EXT_0_1_CONSOLE_GETCHAR:
		ret = sbi_getc();
		अवरोध;
	हाल SBI_EXT_0_1_CLEAR_IPI:
		sbi_ipi_clear_smode();
		अवरोध;
	हाल SBI_EXT_0_1_SEND_IPI:
		अगर (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			ret = sbi_ipi_send_smode(hmask, hbase);
		} अन्यथा {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		अवरोध;
	हाल SBI_EXT_0_1_REMOTE_FENCE_I:
		अगर (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, 0, 0, 0, 0,
					  SBI_TLB_FENCE_I, source_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} अन्यथा {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		अवरोध;
	हाल SBI_EXT_0_1_REMOTE_SFENCE_VMA:
		अगर (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, regs->a1, regs->a2, 0, 0,
					  SBI_TLB_SFENCE_VMA, source_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} अन्यथा {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		अवरोध;
	हाल SBI_EXT_0_1_REMOTE_SFENCE_VMA_ASID:
		अगर (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, regs->a1,
					  regs->a2, regs->a3, 0,
					  SBI_TLB_SFENCE_VMA_ASID,
					  source_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} अन्यथा {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		अवरोध;
	हाल SBI_EXT_0_1_SHUTDOWN:
		sbi_system_reset(SBI_SRST_RESET_TYPE_SHUTDOWN,
				 SBI_SRST_RESET_REASON_NONE);
		अवरोध;
	शेष:
		ret = SBI_ENOTSUPP;
	}

	वापस ret;
}

काष्ठा sbi_ecall_extension ecall_legacy;

अटल पूर्णांक sbi_ecall_legacy_register_extensions(व्योम)
{
	वापस sbi_ecall_register_extension(&ecall_legacy);
}

काष्ठा sbi_ecall_extension ecall_legacy = {
	.name			= "legacy",
	.extid_start		= SBI_EXT_0_1_SET_TIMER,
	.extid_end		= SBI_EXT_0_1_SHUTDOWN,
	.register_extensions	= sbi_ecall_legacy_register_extensions,
	.handle			= sbi_ecall_legacy_handler,
};
