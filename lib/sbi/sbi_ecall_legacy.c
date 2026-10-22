/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ipi.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_platform.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_system.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_timer.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_tlb.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_unpriv.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>

ಸ್ಥಿರ bool sbi_ತುಂಬು_hart_ಮುಖವಾಡ_unpriv(ulong *pmask, ulong *hmask, ulong *hbase,
				      ರಚನೆ sbi_trap_info *uptrap)
{
	ulong ಮುಖವಾಡ = 0;

	ಆದರೆ (pmask) {
		ಮುಖವಾಡ = sbi_ತುಂಬು_ulong(pmask, uptrap);
		ಆದರೆ (uptrap->cause)
			ಹಿಂತಿರುಗಿ false;
		*hbase = 0;
	} ಇಲ್ಲದಿದ್ದರೆ {
		*hbase = -1UL;
	}
	*hmask = ಮುಖವಾಡ;

	ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_legacy_handler(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ extid, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ funcid,
				    ರಚನೆ sbi_trap_regs *regs,
				    ರಚನೆ sbi_ecall_return *ಹೊರಹೋಗು)
{
	ಪೂರ್ಣಾಂಕ ret = 0;
	ರಚನೆ sbi_tlb_info tlb_info;
	u32 ಮೂಲ_hart = current_hartid();
	ರಚನೆ sbi_trap_info trap = {0};
	ulong hmask, hbase;

	ಆಯ್ಕೆ (extid) {
	ಸ್ಥಿತಿ SBI_EXT_0_1_ಗಣ_TIMER:
#ಆದರೆ __riscv_xlen == 32
		sbi_timer_smode_event_ಆರಂಭ((((u64)regs->a1 << 32) | (u64)regs->a0));
#ಇಲ್ಲದಿದ್ದರೆ
		sbi_timer_smode_event_ಆರಂಭ((u64)regs->a0);
#ಆದರೆ_ಅಂತ್ಯ
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_0_1_CONSOLE_PUTCHAR:
		sbi_putc(regs->a0);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_0_1_CONSOLE_GETCHAR:
		ret = sbi_getc();
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_0_1_CLEAR_IPI:
		sbi_ipi_clear_smode();
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_0_1_SEND_IPI:
		ಆದರೆ (sbi_ತುಂಬು_hart_ಮುಖವಾಡ_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			ret = sbi_ipi_send_smode(hmask, hbase);
		} ಇಲ್ಲದಿದ್ದರೆ {
			sbi_trap_redirect(regs, &trap);
			ಹೊರಹೋಗು->skip_regs_update = true;
		}
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_0_1_REMOTE_FENCE_I:
		ಆದರೆ (sbi_ತುಂಬು_hart_ಮುಖವಾಡ_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, 0, 0, 0, 0,
					  SBI_TLB_FENCE_I, ಮೂಲ_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} ಇಲ್ಲದಿದ್ದರೆ {
			sbi_trap_redirect(regs, &trap);
			ಹೊರಹೋಗು->skip_regs_update = true;
		}
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_0_1_REMOTE_SFENCE_VMA:
		ಆದರೆ (sbi_ತುಂಬು_hart_ಮುಖವಾಡ_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, regs->a1, regs->a2, 0, 0,
					  SBI_TLB_SFENCE_VMA, ಮೂಲ_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} ಇಲ್ಲದಿದ್ದರೆ {
			sbi_trap_redirect(regs, &trap);
			ಹೊರಹೋಗು->skip_regs_update = true;
		}
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_0_1_REMOTE_SFENCE_VMA_ASID:
		ಆದರೆ (sbi_ತುಂಬು_hart_ಮುಖವಾಡ_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, regs->a1,
					  regs->a2, regs->a3, 0,
					  SBI_TLB_SFENCE_VMA_ASID,
					  ಮೂಲ_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} ಇಲ್ಲದಿದ್ದರೆ {
			sbi_trap_redirect(regs, &trap);
			ಹೊರಹೋಗು->skip_regs_update = true;
		}
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_0_1_SHUTDOWN:
		sbi_system_reset(SBI_SRST_RESET_TYPE_SHUTDOWN,
				 SBI_SRST_RESET_REASON_NONE);
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ret = SBI_ENOTSUPP;
	}

	ಹಿಂತಿರುಗಿ ret;
}

ರಚನೆ sbi_ecall_extension ecall_legacy;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_legacy_ರಿಜಿಸ್ಟರ್_extensions(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ sbi_ecall_ರಿಜಿಸ್ಟರ್_extension(&ecall_legacy);
}

ರಚನೆ sbi_ecall_extension ecall_legacy = {
	.name			= "legacy",
	.extid_ಆರಂಭ		= SBI_EXT_0_1_ಗಣ_TIMER,
	.extid_ಕೊನೆ		= SBI_EXT_0_1_SHUTDOWN,
	.ರಿಜಿಸ್ಟರ್_extensions	= sbi_ecall_legacy_ರಿಜಿಸ್ಟರ್_extensions,
	.handle			= sbi_ecall_legacy_handler,
};
