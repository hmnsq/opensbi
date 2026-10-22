/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_ecall.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_ipi.h>
#ضمن <sbi/sbi_platform.h>
#ضمن <sbi/sbi_system.h>
#ضمن <sbi/sbi_timer.h>
#ضمن <sbi/sbi_tlb.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/sbi_unpriv.h>
#ضمن <sbi/sbi_hart.h>

ساكن bool sbi_بارگیری_hart_ماسک_unpriv(ulong *pmask, ulong *hmask, ulong *hbase,
				      بنية sbi_trap_info *uptrap)
{
	ulong ماسک = 0;

	إذا (pmask) {
		ماسک = sbi_بارگیری_ulong(pmask, uptrap);
		إذا (uptrap->cause)
			أرجع false;
		*hbase = 0;
	} وإلا {
		*hbase = -1UL;
	}
	*hmask = ماسک;

	أرجع true;
}

ساكن صحيح sbi_ecall_legacy_handler(بیعلامت دراز extid, بیعلامت دراز funcid,
				    بنية sbi_trap_regs *regs,
				    بنية sbi_ecall_return *بیرون)
{
	صحيح ret = 0;
	بنية sbi_tlb_info tlb_info;
	u32 منبع_hart = current_hartid();
	بنية sbi_trap_info trap = {0};
	ulong hmask, hbase;

	بدل (extid) {
	حالة SBI_EXT_0_1_مجموعه_TIMER:
#إذا __riscv_xlen == 32
		sbi_timer_smode_event_شروع((((u64)regs->a1 << 32) | (u64)regs->a0));
#وإلا
		sbi_timer_smode_event_شروع((u64)regs->a0);
#پایان_اگر
		اكسر;
	حالة SBI_EXT_0_1_CONSOLE_PUTCHAR:
		sbi_putc(regs->a0);
		اكسر;
	حالة SBI_EXT_0_1_CONSOLE_GETCHAR:
		ret = sbi_getc();
		اكسر;
	حالة SBI_EXT_0_1_CLEAR_IPI:
		sbi_ipi_clear_smode();
		اكسر;
	حالة SBI_EXT_0_1_SEND_IPI:
		إذا (sbi_بارگیری_hart_ماسک_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			ret = sbi_ipi_send_smode(hmask, hbase);
		} وإلا {
			sbi_trap_redirect(regs, &trap);
			بیرون->skip_regs_update = true;
		}
		اكسر;
	حالة SBI_EXT_0_1_REMOTE_FENCE_I:
		إذا (sbi_بارگیری_hart_ماسک_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, 0, 0, 0, 0,
					  SBI_TLB_FENCE_I, منبع_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} وإلا {
			sbi_trap_redirect(regs, &trap);
			بیرون->skip_regs_update = true;
		}
		اكسر;
	حالة SBI_EXT_0_1_REMOTE_SFENCE_VMA:
		إذا (sbi_بارگیری_hart_ماسک_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, regs->a1, regs->a2, 0, 0,
					  SBI_TLB_SFENCE_VMA, منبع_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} وإلا {
			sbi_trap_redirect(regs, &trap);
			بیرون->skip_regs_update = true;
		}
		اكسر;
	حالة SBI_EXT_0_1_REMOTE_SFENCE_VMA_ASID:
		إذا (sbi_بارگیری_hart_ماسک_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, regs->a1,
					  regs->a2, regs->a3, 0,
					  SBI_TLB_SFENCE_VMA_ASID,
					  منبع_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} وإلا {
			sbi_trap_redirect(regs, &trap);
			بیرون->skip_regs_update = true;
		}
		اكسر;
	حالة SBI_EXT_0_1_SHUTDOWN:
		sbi_system_reset(SBI_SRST_RESET_TYPE_SHUTDOWN,
				 SBI_SRST_RESET_REASON_NONE);
		اكسر;
	افتراضي:
		ret = SBI_ENOTSUPP;
	}

	أرجع ret;
}

بنية sbi_ecall_extension ecall_legacy;

ساكن صحيح sbi_ecall_legacy_ثبات_extensions(فراغ)
{
	أرجع sbi_ecall_ثبات_extension(&ecall_legacy);
}

بنية sbi_ecall_extension ecall_legacy = {
	.name			= "legacy",
	.extid_شروع		= SBI_EXT_0_1_مجموعه_TIMER,
	.extid_انتها		= SBI_EXT_0_1_SHUTDOWN,
	.ثبات_extensions	= sbi_ecall_legacy_ثبات_extensions,
	.handle			= sbi_ecall_legacy_handler,
};
