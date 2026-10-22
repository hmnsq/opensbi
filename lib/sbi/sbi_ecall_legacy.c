/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ipi.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_platform.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_system.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_tlb.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_unpriv.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>

ਸਥਿਰ bool sbi_ਲੱਦੋ_hart_ਨਕਾਬ_unpriv(ulong *pmask, ulong *hmask, ulong *hbase,
				      ਬਣਤਰ sbi_trap_info *uptrap)
{
	ulong ਨਕਾਬ = 0;

	ਜੇ (pmask) {
		ਨਕਾਬ = sbi_ਲੱਦੋ_ulong(pmask, uptrap);
		ਜੇ (uptrap->cause)
			ਵਾਪਸ false;
		*hbase = 0;
	} ਨਹੀਂਤਾਂ {
		*hbase = -1UL;
	}
	*hmask = ਨਕਾਬ;

	ਵਾਪਸ true;
}

ਸਥਿਰ ਪੂਰਨ sbi_ecall_legacy_handler(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ extid, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ funcid,
				    ਬਣਤਰ sbi_trap_regs *regs,
				    ਬਣਤਰ sbi_ecall_return *ਬਾਹਰਲਾ)
{
	ਪੂਰਨ ret = 0;
	ਬਣਤਰ sbi_tlb_info tlb_info;
	u32 ਸਰੋਤ_hart = current_hartid();
	ਬਣਤਰ sbi_trap_info trap = {0};
	ulong hmask, hbase;

	ਚੋਣ (extid) {
	ਹਾਲਤ SBI_EXT_0_1_ਸੈੱਟ_TIMER:
#ਜੇ __riscv_xlen == 32
		sbi_timer_smode_event_ਸ਼ੁਰੂ((((u64)regs->a1 << 32) | (u64)regs->a0));
#ਨਹੀਂਤਾਂ
		sbi_timer_smode_event_ਸ਼ੁਰੂ((u64)regs->a0);
#ਜੇ_ਅੰਤ
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_0_1_CONSOLE_PUTCHAR:
		sbi_putc(regs->a0);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_0_1_CONSOLE_GETCHAR:
		ret = sbi_getc();
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_0_1_CLEAR_IPI:
		sbi_ipi_clear_smode();
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_0_1_SEND_IPI:
		ਜੇ (sbi_ਲੱਦੋ_hart_ਨਕਾਬ_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			ret = sbi_ipi_send_smode(hmask, hbase);
		} ਨਹੀਂਤਾਂ {
			sbi_trap_redirect(regs, &trap);
			ਬਾਹਰਲਾ->skip_regs_update = true;
		}
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_0_1_REMOTE_FENCE_I:
		ਜੇ (sbi_ਲੱਦੋ_hart_ਨਕਾਬ_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, 0, 0, 0, 0,
					  SBI_TLB_FENCE_I, ਸਰੋਤ_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} ਨਹੀਂਤਾਂ {
			sbi_trap_redirect(regs, &trap);
			ਬਾਹਰਲਾ->skip_regs_update = true;
		}
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_0_1_REMOTE_SFENCE_VMA:
		ਜੇ (sbi_ਲੱਦੋ_hart_ਨਕਾਬ_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, regs->a1, regs->a2, 0, 0,
					  SBI_TLB_SFENCE_VMA, ਸਰੋਤ_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} ਨਹੀਂਤਾਂ {
			sbi_trap_redirect(regs, &trap);
			ਬਾਹਰਲਾ->skip_regs_update = true;
		}
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_0_1_REMOTE_SFENCE_VMA_ASID:
		ਜੇ (sbi_ਲੱਦੋ_hart_ਨਕਾਬ_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, regs->a1,
					  regs->a2, regs->a3, 0,
					  SBI_TLB_SFENCE_VMA_ASID,
					  ਸਰੋਤ_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} ਨਹੀਂਤਾਂ {
			sbi_trap_redirect(regs, &trap);
			ਬਾਹਰਲਾ->skip_regs_update = true;
		}
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_0_1_SHUTDOWN:
		sbi_system_reset(SBI_SRST_RESET_TYPE_SHUTDOWN,
				 SBI_SRST_RESET_REASON_NONE);
		ਰੋਕੋ;
	ਮੂਲ:
		ret = SBI_ENOTSUPP;
	}

	ਵਾਪਸ ret;
}

ਬਣਤਰ sbi_ecall_extension ecall_legacy;

ਸਥਿਰ ਪੂਰਨ sbi_ecall_legacy_ਰਜਿਸਟਰ_extensions(ਖਾਲੀ)
{
	ਵਾਪਸ sbi_ecall_ਰਜਿਸਟਰ_extension(&ecall_legacy);
}

ਬਣਤਰ sbi_ecall_extension ecall_legacy = {
	.name			= "legacy",
	.extid_ਸ਼ੁਰੂ		= SBI_EXT_0_1_ਸੈੱਟ_TIMER,
	.extid_ਸਿਰਾ		= SBI_EXT_0_1_SHUTDOWN,
	.ਰਜਿਸਟਰ_extensions	= sbi_ecall_legacy_ਰਜਿਸਟਰ_extensions,
	.handle			= sbi_ecall_legacy_handler,
};
