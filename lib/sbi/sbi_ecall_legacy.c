/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_ecall.h>
#অন্তর্ভাব <sbi/sbi_ecall_interface.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_ipi.h>
#অন্তর্ভাব <sbi/sbi_platform.h>
#অন্তর্ভাব <sbi/sbi_system.h>
#অন্তর্ভাব <sbi/sbi_timer.h>
#অন্তর্ভাব <sbi/sbi_tlb.h>
#অন্তর্ভাব <sbi/sbi_trap.h>
#অন্তর্ভাব <sbi/sbi_unpriv.h>
#অন্তর্ভাব <sbi/sbi_hart.h>

স্থির bool sbi_load_hart_mask_unpriv(ulong *pmask, ulong *hmask, ulong *hbase,
				      সংরচনা sbi_trap_info *uptrap)
{
	ulong mask = 0;

	যদি (pmask) {
		mask = sbi_load_ulong(pmask, uptrap);
		যদি (uptrap->cause)
			প্রত্যাগম false;
		*hbase = 0;
	} অন্যথা {
		*hbase = -1UL;
	}
	*hmask = mask;

	প্রত্যাগম true;
}

স্থির পূর্ণাঙ্ক sbi_ecall_legacy_handler(unsigned long extid, unsigned long funcid,
				    সংরচনা sbi_trap_regs *regs,
				    সংরচনা sbi_ecall_return *out)
{
	পূর্ণাঙ্ক ret = 0;
	সংরচনা sbi_tlb_info tlb_info;
	u32 source_hart = current_hartid();
	সংরচনা sbi_trap_info trap = {0};
	ulong hmask, hbase;

	বিকল্প (extid) {
	পক্ষ SBI_EXT_0_1_SET_TIMER:
#যদি __riscv_xlen == 32
		sbi_timer_smode_event_start((((u64)regs->a1 << 32) | (u64)regs->a0));
#অন্যথা
		sbi_timer_smode_event_start((u64)regs->a0);
#endif
		ভঙ্গ;
	পক্ষ SBI_EXT_0_1_CONSOLE_PUTCHAR:
		sbi_putc(regs->a0);
		ভঙ্গ;
	পক্ষ SBI_EXT_0_1_CONSOLE_GETCHAR:
		ret = sbi_getc();
		ভঙ্গ;
	পক্ষ SBI_EXT_0_1_CLEAR_IPI:
		sbi_ipi_clear_smode();
		ভঙ্গ;
	পক্ষ SBI_EXT_0_1_SEND_IPI:
		যদি (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			ret = sbi_ipi_send_smode(hmask, hbase);
		} অন্যথা {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		ভঙ্গ;
	পক্ষ SBI_EXT_0_1_REMOTE_FENCE_I:
		যদি (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, 0, 0, 0, 0,
					  SBI_TLB_FENCE_I, source_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} অন্যথা {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		ভঙ্গ;
	পক্ষ SBI_EXT_0_1_REMOTE_SFENCE_VMA:
		যদি (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, regs->a1, regs->a2, 0, 0,
					  SBI_TLB_SFENCE_VMA, source_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} অন্যথা {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		ভঙ্গ;
	পক্ষ SBI_EXT_0_1_REMOTE_SFENCE_VMA_ASID:
		যদি (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, regs->a1,
					  regs->a2, regs->a3, 0,
					  SBI_TLB_SFENCE_VMA_ASID,
					  source_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} অন্যথা {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		ভঙ্গ;
	পক্ষ SBI_EXT_0_1_SHUTDOWN:
		sbi_system_reset(SBI_SRST_RESET_TYPE_SHUTDOWN,
				 SBI_SRST_RESET_REASON_NONE);
		ভঙ্গ;
	স্বভাব:
		ret = SBI_ENOTSUPP;
	}

	প্রত্যাগম ret;
}

সংরচনা sbi_ecall_extension ecall_legacy;

স্থির পূর্ণাঙ্ক sbi_ecall_legacy_register_extensions(শূন্য)
{
	প্রত্যাগম sbi_ecall_register_extension(&ecall_legacy);
}

সংরচনা sbi_ecall_extension ecall_legacy = {
	.name			= "legacy",
	.extid_start		= SBI_EXT_0_1_SET_TIMER,
	.extid_end		= SBI_EXT_0_1_SHUTDOWN,
	.register_extensions	= sbi_ecall_legacy_register_extensions,
	.handle			= sbi_ecall_legacy_handler,
};
