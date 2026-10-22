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

ساکن bool sbi_بھرو_hart_نقاب_unpriv(ulong *pmask, ulong *hmask, ulong *hbase,
				      ساخت sbi_trap_info *uptrap)
{
	ulong نقاب = 0;

	اگر (pmask) {
		نقاب = sbi_بھرو_ulong(pmask, uptrap);
		اگر (uptrap->cause)
			واپس false;
		*hbase = 0;
	} ورنہ {
		*hbase = -1UL;
	}
	*hmask = نقاب;

	واپس true;
}

ساکن صحیح sbi_ecall_legacy_handler(غیرعلامتی طویل extid, غیرعلامتی طویل funcid,
				    ساخت sbi_trap_regs *regs,
				    ساخت sbi_ecall_return *برآمد)
{
	صحیح ret = 0;
	ساخت sbi_tlb_info tlb_info;
	u32 ماخذ_hart = current_hartid();
	ساخت sbi_trap_info trap = {0};
	ulong hmask, hbase;

	انتخاب (extid) {
	حالت SBI_EXT_0_1_سیٹ_TIMER:
#اگر __riscv_xlen == 32
		sbi_timer_smode_event_آغاز((((u64)regs->a1 << 32) | (u64)regs->a0));
#ورنہ
		sbi_timer_smode_event_آغاز((u64)regs->a0);
#اگر_ختم
		توڑو;
	حالت SBI_EXT_0_1_CONSOLE_PUTCHAR:
		sbi_putc(regs->a0);
		توڑو;
	حالت SBI_EXT_0_1_CONSOLE_GETCHAR:
		ret = sbi_getc();
		توڑو;
	حالت SBI_EXT_0_1_CLEAR_IPI:
		sbi_ipi_clear_smode();
		توڑو;
	حالت SBI_EXT_0_1_SEND_IPI:
		اگر (sbi_بھرو_hart_نقاب_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			ret = sbi_ipi_send_smode(hmask, hbase);
		} ورنہ {
			sbi_trap_redirect(regs, &trap);
			برآمد->skip_regs_update = true;
		}
		توڑو;
	حالت SBI_EXT_0_1_REMOTE_FENCE_I:
		اگر (sbi_بھرو_hart_نقاب_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, 0, 0, 0, 0,
					  SBI_TLB_FENCE_I, ماخذ_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} ورنہ {
			sbi_trap_redirect(regs, &trap);
			برآمد->skip_regs_update = true;
		}
		توڑو;
	حالت SBI_EXT_0_1_REMOTE_SFENCE_VMA:
		اگر (sbi_بھرو_hart_نقاب_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, regs->a1, regs->a2, 0, 0,
					  SBI_TLB_SFENCE_VMA, ماخذ_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} ورنہ {
			sbi_trap_redirect(regs, &trap);
			برآمد->skip_regs_update = true;
		}
		توڑو;
	حالت SBI_EXT_0_1_REMOTE_SFENCE_VMA_ASID:
		اگر (sbi_بھرو_hart_نقاب_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, regs->a1,
					  regs->a2, regs->a3, 0,
					  SBI_TLB_SFENCE_VMA_ASID,
					  ماخذ_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} ورنہ {
			sbi_trap_redirect(regs, &trap);
			برآمد->skip_regs_update = true;
		}
		توڑو;
	حالت SBI_EXT_0_1_SHUTDOWN:
		sbi_system_reset(SBI_SRST_RESET_TYPE_SHUTDOWN,
				 SBI_SRST_RESET_REASON_NONE);
		توڑو;
	طےشدہ:
		ret = SBI_ENOTSUPP;
	}

	واپس ret;
}

ساخت sbi_ecall_extension ecall_legacy;

ساکن صحیح sbi_ecall_legacy_رجسٹر_extensions(خالی)
{
	واپس sbi_ecall_رجسٹر_extension(&ecall_legacy);
}

ساخت sbi_ecall_extension ecall_legacy = {
	.name			= "legacy",
	.extid_آغاز		= SBI_EXT_0_1_سیٹ_TIMER,
	.extid_انتہا		= SBI_EXT_0_1_SHUTDOWN,
	.رجسٹر_extensions	= sbi_ecall_legacy_رجسٹر_extensions,
	.handle			= sbi_ecall_legacy_handler,
};
