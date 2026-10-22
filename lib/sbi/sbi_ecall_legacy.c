/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_ecall.h>
#അന്തര്ഭാവ <sbi/sbi_ecall_interface.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_ipi.h>
#അന്തര്ഭാവ <sbi/sbi_platform.h>
#അന്തര്ഭാവ <sbi/sbi_system.h>
#അന്തര്ഭാവ <sbi/sbi_timer.h>
#അന്തര്ഭാവ <sbi/sbi_tlb.h>
#അന്തര്ഭാവ <sbi/sbi_trap.h>
#അന്തര്ഭാവ <sbi/sbi_unpriv.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>

സ്ഥിതം bool sbi_load_hart_mask_unpriv(ulong *pmask, ulong *hmask, ulong *hbase,
				      ഘടന sbi_trap_info *uptrap)
{
	ulong mask = 0;

	എങ്കിൽ (pmask) {
		mask = sbi_load_ulong(pmask, uptrap);
		എങ്കിൽ (uptrap->cause)
			തിരികെ false;
		*hbase = 0;
	} അല്ലെങ്കിൽ {
		*hbase = -1UL;
	}
	*hmask = mask;

	തിരികെ true;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_legacy_handler(unsigned long extid, unsigned long funcid,
				    ഘടന sbi_trap_regs *regs,
				    ഘടന sbi_ecall_return *out)
{
	പൂർണ്ണസംഖ്യ ret = 0;
	ഘടന sbi_tlb_info tlb_info;
	u32 source_hart = current_hartid();
	ഘടന sbi_trap_info trap = {0};
	ulong hmask, hbase;

	തിരഞ്ഞെടുപ്പ് (extid) {
	അവസ്ഥ SBI_EXT_0_1_SET_TIMER:
#എങ്കിൽ __riscv_xlen == 32
		sbi_timer_smode_event_start((((u64)regs->a1 << 32) | (u64)regs->a0));
#അല്ലെങ്കിൽ
		sbi_timer_smode_event_start((u64)regs->a0);
#endif
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_0_1_CONSOLE_PUTCHAR:
		sbi_putc(regs->a0);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_0_1_CONSOLE_GETCHAR:
		ret = sbi_getc();
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_0_1_CLEAR_IPI:
		sbi_ipi_clear_smode();
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_0_1_SEND_IPI:
		എങ്കിൽ (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			ret = sbi_ipi_send_smode(hmask, hbase);
		} അല്ലെങ്കിൽ {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_0_1_REMOTE_FENCE_I:
		എങ്കിൽ (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, 0, 0, 0, 0,
					  SBI_TLB_FENCE_I, source_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} അല്ലെങ്കിൽ {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_0_1_REMOTE_SFENCE_VMA:
		എങ്കിൽ (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, regs->a1, regs->a2, 0, 0,
					  SBI_TLB_SFENCE_VMA, source_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} അല്ലെങ്കിൽ {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_0_1_REMOTE_SFENCE_VMA_ASID:
		എങ്കിൽ (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, regs->a1,
					  regs->a2, regs->a3, 0,
					  SBI_TLB_SFENCE_VMA_ASID,
					  source_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} അല്ലെങ്കിൽ {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_0_1_SHUTDOWN:
		sbi_system_reset(SBI_SRST_RESET_TYPE_SHUTDOWN,
				 SBI_SRST_RESET_REASON_NONE);
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		ret = SBI_ENOTSUPP;
	}

	തിരികെ ret;
}

ഘടന sbi_ecall_extension ecall_legacy;

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_legacy_register_extensions(ശൂന്യം)
{
	തിരികെ sbi_ecall_register_extension(&ecall_legacy);
}

ഘടന sbi_ecall_extension ecall_legacy = {
	.name			= "legacy",
	.extid_start		= SBI_EXT_0_1_SET_TIMER,
	.extid_end		= SBI_EXT_0_1_SHUTDOWN,
	.register_extensions	= sbi_ecall_legacy_register_extensions,
	.handle			= sbi_ecall_legacy_handler,
};
