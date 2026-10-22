/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#include <sbi/riscv_asm.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_domain.h>
#include <sbi/sbi_ecall.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_ipi.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_system.h>
#include <sbi/sbi_timer.h>
#include <sbi/sbi_tlb.h>
#include <sbi/sbi_trap.h>
#include <sbi/sbi_unpriv.h>
#include <sbi/sbi_hart.h>

static bool sbi_load_hart_mask_unpriv(ulong *pmask, ulong *hmask, ulong *hbase,
				      sa_nracanaa sbi_trap_info *uptrap)
{
	ulong mask = 0;

	ပေႃး (pmask) {
		mask = sbi_load_ulong(pmask, uptrap);
		ပေႃး (uptrap->cause)
			ပွၵ်ႈ false;
		*hbase = 0;
	} ပေႃးဢမ်ႇ {
		*hbase = -1UL;
	}
	*hmask = mask;

	ပွၵ်ႈ true;
}

static puur_naa_nka sbi_ecall_legacy_handler(unsigned long extid, unsigned long funcid,
				    sa_nracanaa sbi_trap_regs *regs,
				    sa_nracanaa sbi_ecall_return *out)
{
	puur_naa_nka ret = 0;
	sa_nracanaa sbi_tlb_info tlb_info;
	u32 source_hart = current_hartid();
	sa_nracanaa sbi_trap_info trap = {0};
	ulong hmask, hbase;

	switch (extid) {
	pak_sa SBI_EXT_0_1_SET_TIMER:
#ပေႃး __riscv_xlen == 32
		sbi_timer_smode_event_start((((u64)regs->a1 << 32) | (u64)regs->a0));
#ပေႃးဢမ်ႇ
		sbi_timer_smode_event_start((u64)regs->a0);
#endif
		bha_nga;
	pak_sa SBI_EXT_0_1_CONSOLE_PUTCHAR:
		sbi_putc(regs->a0);
		bha_nga;
	pak_sa SBI_EXT_0_1_CONSOLE_GETCHAR:
		ret = sbi_getc();
		bha_nga;
	pak_sa SBI_EXT_0_1_CLEAR_IPI:
		sbi_ipi_clear_smode();
		bha_nga;
	pak_sa SBI_EXT_0_1_SEND_IPI:
		ပေႃး (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			ret = sbi_ipi_send_smode(hmask, hbase);
		} ပေႃးဢမ်ႇ {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		bha_nga;
	pak_sa SBI_EXT_0_1_REMOTE_FENCE_I:
		ပေႃး (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, 0, 0, 0, 0,
					  SBI_TLB_FENCE_I, source_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} ပေႃးဢမ်ႇ {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		bha_nga;
	pak_sa SBI_EXT_0_1_REMOTE_SFENCE_VMA:
		ပေႃး (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, regs->a1, regs->a2, 0, 0,
					  SBI_TLB_SFENCE_VMA, source_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} ပေႃးဢမ်ႇ {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		bha_nga;
	pak_sa SBI_EXT_0_1_REMOTE_SFENCE_VMA_ASID:
		ပေႃး (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, regs->a1,
					  regs->a2, regs->a3, 0,
					  SBI_TLB_SFENCE_VMA_ASID,
					  source_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} ပေႃးဢမ်ႇ {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		bha_nga;
	pak_sa SBI_EXT_0_1_SHUTDOWN:
		sbi_system_reset(SBI_SRST_RESET_TYPE_SHUTDOWN,
				 SBI_SRST_RESET_REASON_NONE);
		bha_nga;
	default:
		ret = SBI_ENOTSUPP;
	}

	ပွၵ်ႈ ret;
}

sa_nracanaa sbi_ecall_extension ecall_legacy;

static puur_naa_nka sbi_ecall_legacy_register_extensions(void)
{
	ပွၵ်ႈ sbi_ecall_register_extension(&ecall_legacy);
}

sa_nracanaa sbi_ecall_extension ecall_legacy = {
	.name			= "legacy",
	.extid_start		= SBI_EXT_0_1_SET_TIMER,
	.extid_end		= SBI_EXT_0_1_SHUTDOWN,
	.register_extensions	= sbi_ecall_legacy_register_extensions,
	.handle			= sbi_ecall_legacy_handler,
};
