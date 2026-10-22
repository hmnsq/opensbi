/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_ecall.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_ipi.h>
#अन्तर्भाव <sbi/sbi_platform.h>
#अन्तर्भाव <sbi/sbi_system.h>
#अन्तर्भाव <sbi/sbi_timer.h>
#अन्तर्भाव <sbi/sbi_tlb.h>
#अन्तर्भाव <sbi/sbi_trap.h>
#अन्तर्भाव <sbi/sbi_unpriv.h>
#अन्तर्भाव <sbi/sbi_hart.h>

स्थिर bool sbi_load_hart_mask_unpriv(ulong *pmask, ulong *hmask, ulong *hbase,
				      संरचना sbi_trap_info *uptrap)
{
	ulong mask = 0;

	जर (pmask) {
		mask = sbi_load_ulong(pmask, uptrap);
		जर (uptrap->cause)
			परत false;
		*hbase = 0;
	} अन्यथा {
		*hbase = -1UL;
	}
	*hmask = mask;

	परत true;
}

स्थिर पूर्णांक sbi_ecall_legacy_handler(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ funcid,
				    संरचना sbi_trap_regs *regs,
				    संरचना sbi_ecall_return *out)
{
	पूर्णांक ret = 0;
	संरचना sbi_tlb_info tlb_info;
	u32 source_hart = current_hartid();
	संरचना sbi_trap_info trap = {0};
	ulong hmask, hbase;

	निवड (extid) {
	स्थिती SBI_EXT_0_1_SET_TIMER:
#जर __riscv_xlen == 32
		sbi_timer_smode_event_start((((u64)regs->a1 << 32) | (u64)regs->a0));
#अन्यथा
		sbi_timer_smode_event_start((u64)regs->a0);
#endif
		थांबा;
	स्थिती SBI_EXT_0_1_CONSOLE_PUTCHAR:
		sbi_putc(regs->a0);
		थांबा;
	स्थिती SBI_EXT_0_1_CONSOLE_GETCHAR:
		ret = sbi_getc();
		थांबा;
	स्थिती SBI_EXT_0_1_CLEAR_IPI:
		sbi_ipi_clear_smode();
		थांबा;
	स्थिती SBI_EXT_0_1_SEND_IPI:
		जर (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			ret = sbi_ipi_send_smode(hmask, hbase);
		} अन्यथा {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		थांबा;
	स्थिती SBI_EXT_0_1_REMOTE_FENCE_I:
		जर (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, 0, 0, 0, 0,
					  SBI_TLB_FENCE_I, source_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} अन्यथा {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		थांबा;
	स्थिती SBI_EXT_0_1_REMOTE_SFENCE_VMA:
		जर (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, regs->a1, regs->a2, 0, 0,
					  SBI_TLB_SFENCE_VMA, source_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} अन्यथा {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		थांबा;
	स्थिती SBI_EXT_0_1_REMOTE_SFENCE_VMA_ASID:
		जर (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
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
		थांबा;
	स्थिती SBI_EXT_0_1_SHUTDOWN:
		sbi_system_reset(SBI_SRST_RESET_TYPE_SHUTDOWN,
				 SBI_SRST_RESET_REASON_NONE);
		थांबा;
	पूर्वनिर्धारित:
		ret = SBI_ENOTSUPP;
	}

	परत ret;
}

संरचना sbi_ecall_extension ecall_legacy;

स्थिर पूर्णांक sbi_ecall_legacy_register_extensions(शून्य)
{
	परत sbi_ecall_register_extension(&ecall_legacy);
}

संरचना sbi_ecall_extension ecall_legacy = {
	.name			= "legacy",
	.extid_start		= SBI_EXT_0_1_SET_TIMER,
	.extid_end		= SBI_EXT_0_1_SHUTDOWN,
	.register_extensions	= sbi_ecall_legacy_register_extensions,
	.handle			= sbi_ecall_legacy_handler,
};
