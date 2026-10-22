/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_ecall.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_ipi.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_system.h>
#شامل <sbi/sbi_timer.h>
#شامل <sbi/sbi_tlb.h>
#شامل <sbi/sbi_trap.h>
#شامل <sbi/sbi_unpriv.h>
#شامل <sbi/sbi_hart.h>

ایستا bool sbi_load_hart_mask_unpriv(ulong *pmask, ulong *hmask, ulong *hbase,
				      ساختار sbi_trap_info *uptrap)
{
	ulong mask = 0;

	اگر (pmask) {
		mask = sbi_load_ulong(pmask, uptrap);
		اگر (uptrap->cause)
			بازگشت false;
		*hbase = 0;
	} وگرنه {
		*hbase = -1UL;
	}
	*hmask = mask;

	بازگشت true;
}

ایستا صحیح sbi_ecall_legacy_handler(unsigned long extid, unsigned long funcid,
				    ساختار sbi_trap_regs *regs,
				    ساختار sbi_ecall_return *out)
{
	صحیح ret = 0;
	ساختار sbi_tlb_info tlb_info;
	u32 source_hart = current_hartid();
	ساختار sbi_trap_info trap = {0};
	ulong hmask, hbase;

	انتخاب (extid) {
	حالت SBI_EXT_0_1_SET_TIMER:
#اگر __riscv_xlen == 32
		sbi_timer_smode_event_start((((u64)regs->a1 << 32) | (u64)regs->a0));
#وگرنه
		sbi_timer_smode_event_start((u64)regs->a0);
#endif
		بشکن;
	حالت SBI_EXT_0_1_CONSOLE_PUTCHAR:
		sbi_putc(regs->a0);
		بشکن;
	حالت SBI_EXT_0_1_CONSOLE_GETCHAR:
		ret = sbi_getc();
		بشکن;
	حالت SBI_EXT_0_1_CLEAR_IPI:
		sbi_ipi_clear_smode();
		بشکن;
	حالت SBI_EXT_0_1_SEND_IPI:
		اگر (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			ret = sbi_ipi_send_smode(hmask, hbase);
		} وگرنه {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		بشکن;
	حالت SBI_EXT_0_1_REMOTE_FENCE_I:
		اگر (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, 0, 0, 0, 0,
					  SBI_TLB_FENCE_I, source_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} وگرنه {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		بشکن;
	حالت SBI_EXT_0_1_REMOTE_SFENCE_VMA:
		اگر (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, regs->a1, regs->a2, 0, 0,
					  SBI_TLB_SFENCE_VMA, source_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} وگرنه {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		بشکن;
	حالت SBI_EXT_0_1_REMOTE_SFENCE_VMA_ASID:
		اگر (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, regs->a1,
					  regs->a2, regs->a3, 0,
					  SBI_TLB_SFENCE_VMA_ASID,
					  source_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} وگرنه {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		بشکن;
	حالت SBI_EXT_0_1_SHUTDOWN:
		sbi_system_reset(SBI_SRST_RESET_TYPE_SHUTDOWN,
				 SBI_SRST_RESET_REASON_NONE);
		بشکن;
	پیشفرض:
		ret = SBI_ENOTSUPP;
	}

	بازگشت ret;
}

ساختار sbi_ecall_extension ecall_legacy;

ایستا صحیح sbi_ecall_legacy_register_extensions(تهی)
{
	بازگشت sbi_ecall_register_extension(&ecall_legacy);
}

ساختار sbi_ecall_extension ecall_legacy = {
	.name			= "legacy",
	.extid_start		= SBI_EXT_0_1_SET_TIMER,
	.extid_end		= SBI_EXT_0_1_SHUTDOWN,
	.register_extensions	= sbi_ecall_legacy_register_extensions,
	.handle			= sbi_ecall_legacy_handler,
};
