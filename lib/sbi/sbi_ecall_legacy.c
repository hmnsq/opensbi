/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_domain.h>
#અન્તર્ભાવ <sbi/sbi_ecall.h>
#અન્તર્ભાવ <sbi/sbi_ecall_interface.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_ipi.h>
#અન્તર્ભાવ <sbi/sbi_platform.h>
#અન્તર્ભાવ <sbi/sbi_system.h>
#અન્તર્ભાવ <sbi/sbi_timer.h>
#અન્તર્ભાવ <sbi/sbi_tlb.h>
#અન્તર્ભાવ <sbi/sbi_trap.h>
#અન્તર્ભાવ <sbi/sbi_unpriv.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>

સ્થિર bool sbi_load_hart_mask_unpriv(ulong *pmask, ulong *hmask, ulong *hbase,
				      સંરચના sbi_trap_info *uptrap)
{
	ulong mask = 0;

	જો (pmask) {
		mask = sbi_load_ulong(pmask, uptrap);
		જો (uptrap->cause)
			પાછા false;
		*hbase = 0;
	} નહીંતર {
		*hbase = -1UL;
	}
	*hmask = mask;

	પાછા true;
}

સ્થિર પૂર્ણાંક sbi_ecall_legacy_handler(unsigned long extid, unsigned long funcid,
				    સંરચના sbi_trap_regs *regs,
				    સંરચના sbi_ecall_return *out)
{
	પૂર્ણાંક ret = 0;
	સંરચના sbi_tlb_info tlb_info;
	u32 source_hart = current_hartid();
	સંરચના sbi_trap_info trap = {0};
	ulong hmask, hbase;

	પસંદગી (extid) {
	સ્થિતિ SBI_EXT_0_1_SET_TIMER:
#જો __riscv_xlen == 32
		sbi_timer_smode_event_start((((u64)regs->a1 << 32) | (u64)regs->a0));
#નહીંતર
		sbi_timer_smode_event_start((u64)regs->a0);
#endif
		તોડો;
	સ્થિતિ SBI_EXT_0_1_CONSOLE_PUTCHAR:
		sbi_putc(regs->a0);
		તોડો;
	સ્થિતિ SBI_EXT_0_1_CONSOLE_GETCHAR:
		ret = sbi_getc();
		તોડો;
	સ્થિતિ SBI_EXT_0_1_CLEAR_IPI:
		sbi_ipi_clear_smode();
		તોડો;
	સ્થિતિ SBI_EXT_0_1_SEND_IPI:
		જો (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			ret = sbi_ipi_send_smode(hmask, hbase);
		} નહીંતર {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		તોડો;
	સ્થિતિ SBI_EXT_0_1_REMOTE_FENCE_I:
		જો (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, 0, 0, 0, 0,
					  SBI_TLB_FENCE_I, source_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} નહીંતર {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		તોડો;
	સ્થિતિ SBI_EXT_0_1_REMOTE_SFENCE_VMA:
		જો (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, regs->a1, regs->a2, 0, 0,
					  SBI_TLB_SFENCE_VMA, source_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} નહીંતર {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		તોડો;
	સ્થિતિ SBI_EXT_0_1_REMOTE_SFENCE_VMA_ASID:
		જો (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, regs->a1,
					  regs->a2, regs->a3, 0,
					  SBI_TLB_SFENCE_VMA_ASID,
					  source_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} નહીંતર {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		તોડો;
	સ્થિતિ SBI_EXT_0_1_SHUTDOWN:
		sbi_system_reset(SBI_SRST_RESET_TYPE_SHUTDOWN,
				 SBI_SRST_RESET_REASON_NONE);
		તોડો;
	મૂળભૂત:
		ret = SBI_ENOTSUPP;
	}

	પાછા ret;
}

સંરચના sbi_ecall_extension ecall_legacy;

સ્થિર પૂર્ણાંક sbi_ecall_legacy_register_extensions(શૂન્ય)
{
	પાછા sbi_ecall_register_extension(&ecall_legacy);
}

સંરચના sbi_ecall_extension ecall_legacy = {
	.name			= "legacy",
	.extid_start		= SBI_EXT_0_1_SET_TIMER,
	.extid_end		= SBI_EXT_0_1_SHUTDOWN,
	.register_extensions	= sbi_ecall_legacy_register_extensions,
	.handle			= sbi_ecall_legacy_handler,
};
