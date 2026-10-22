/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_ecall.h>
#అన్తర్భావ <sbi/sbi_ecall_interface.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_ipi.h>
#అన్తర్భావ <sbi/sbi_platform.h>
#అన్తర్భావ <sbi/sbi_system.h>
#అన్తర్భావ <sbi/sbi_timer.h>
#అన్తర్భావ <sbi/sbi_tlb.h>
#అన్తర్భావ <sbi/sbi_trap.h>
#అన్తర్భావ <sbi/sbi_unpriv.h>
#అన్తర్భావ <sbi/sbi_hart.h>

స్థిర bool sbi_ఎక్కించు_hart_ముసుగు_unpriv(ulong *pmask, ulong *hmask, ulong *hbase,
				      నిర్మాణం sbi_trap_info *uptrap)
{
	ulong ముసుగు = 0;

	ఒకవేళ (pmask) {
		ముసుగు = sbi_ఎక్కించు_ulong(pmask, uptrap);
		ఒకవేళ (uptrap->cause)
			తిరిగి false;
		*hbase = 0;
	} లేకపోతే {
		*hbase = -1UL;
	}
	*hmask = ముసుగు;

	తిరిగి true;
}

స్థిర పూర్ణాంకం sbi_ecall_legacy_handler(గుర్తులేని దీర్ఘ extid, గుర్తులేని దీర్ఘ funcid,
				    నిర్మాణం sbi_trap_regs *regs,
				    నిర్మాణం sbi_ecall_return *బహిర్గతం)
{
	పూర్ణాంకం ret = 0;
	నిర్మాణం sbi_tlb_info tlb_info;
	u32 ఆధారం_hart = current_hartid();
	నిర్మాణం sbi_trap_info trap = {0};
	ulong hmask, hbase;

	ఎంపిక (extid) {
	స్థితి SBI_EXT_0_1_సమితి_TIMER:
#ఒకవేళ __riscv_xlen == 32
		sbi_timer_smode_event_ప్రారంభం((((u64)regs->a1 << 32) | (u64)regs->a0));
#లేకపోతే
		sbi_timer_smode_event_ప్రారంభం((u64)regs->a0);
#అయితే_ముగింపు
		ఆపు;
	స్థితి SBI_EXT_0_1_CONSOLE_PUTCHAR:
		sbi_putc(regs->a0);
		ఆపు;
	స్థితి SBI_EXT_0_1_CONSOLE_GETCHAR:
		ret = sbi_getc();
		ఆపు;
	స్థితి SBI_EXT_0_1_CLEAR_IPI:
		sbi_ipi_clear_smode();
		ఆపు;
	స్థితి SBI_EXT_0_1_SEND_IPI:
		ఒకవేళ (sbi_ఎక్కించు_hart_ముసుగు_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			ret = sbi_ipi_send_smode(hmask, hbase);
		} లేకపోతే {
			sbi_trap_redirect(regs, &trap);
			బహిర్గతం->skip_regs_update = true;
		}
		ఆపు;
	స్థితి SBI_EXT_0_1_REMOTE_FENCE_I:
		ఒకవేళ (sbi_ఎక్కించు_hart_ముసుగు_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, 0, 0, 0, 0,
					  SBI_TLB_FENCE_I, ఆధారం_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} లేకపోతే {
			sbi_trap_redirect(regs, &trap);
			బహిర్గతం->skip_regs_update = true;
		}
		ఆపు;
	స్థితి SBI_EXT_0_1_REMOTE_SFENCE_VMA:
		ఒకవేళ (sbi_ఎక్కించు_hart_ముసుగు_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, regs->a1, regs->a2, 0, 0,
					  SBI_TLB_SFENCE_VMA, ఆధారం_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} లేకపోతే {
			sbi_trap_redirect(regs, &trap);
			బహిర్గతం->skip_regs_update = true;
		}
		ఆపు;
	స్థితి SBI_EXT_0_1_REMOTE_SFENCE_VMA_ASID:
		ఒకవేళ (sbi_ఎక్కించు_hart_ముసుగు_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, regs->a1,
					  regs->a2, regs->a3, 0,
					  SBI_TLB_SFENCE_VMA_ASID,
					  ఆధారం_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} లేకపోతే {
			sbi_trap_redirect(regs, &trap);
			బహిర్గతం->skip_regs_update = true;
		}
		ఆపు;
	స్థితి SBI_EXT_0_1_SHUTDOWN:
		sbi_system_reset(SBI_SRST_RESET_TYPE_SHUTDOWN,
				 SBI_SRST_RESET_REASON_NONE);
		ఆపు;
	అప్రమేయ:
		ret = SBI_ENOTSUPP;
	}

	తిరిగి ret;
}

నిర్మాణం sbi_ecall_extension ecall_legacy;

స్థిర పూర్ణాంకం sbi_ecall_legacy_రిజిస్టరు_extensions(శూన్య)
{
	తిరిగి sbi_ecall_రిజిస్టరు_extension(&ecall_legacy);
}

నిర్మాణం sbi_ecall_extension ecall_legacy = {
	.name			= "legacy",
	.extid_ప్రారంభం		= SBI_EXT_0_1_సమితి_TIMER,
	.extid_చివర		= SBI_EXT_0_1_SHUTDOWN,
	.రిజిస్టరు_extensions	= sbi_ecall_legacy_రిజిస్టరు_extensions,
	.handle			= sbi_ecall_legacy_handler,
};
