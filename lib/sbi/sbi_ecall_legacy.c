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

ಸ್ಥಿರ bool sbi_load_hart_mask_unpriv(ulong *pmask, ulong *hmask, ulong *hbase,
				      ಸಂರಚನಾ sbi_trap_info *uptrap)
{
	ulong mask = 0;

	ಯದಿ (pmask) {
		mask = sbi_load_ulong(pmask, uptrap);
		ಯದಿ (uptrap->cause)
			ಪ್ರತ್ಯಾಗಮ false;
		*hbase = 0;
	} ಅನ್ಯಥಾ {
		*hbase = -1UL;
	}
	*hmask = mask;

	ಪ್ರತ್ಯಾಗಮ true;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_legacy_handler(unsigned long extid, unsigned long funcid,
				    ಸಂರಚನಾ sbi_trap_regs *regs,
				    ಸಂರಚನಾ sbi_ecall_return *out)
{
	ಪೂರ್ಣಾಙ್ಕ ret = 0;
	ಸಂರಚನಾ sbi_tlb_info tlb_info;
	u32 source_hart = current_hartid();
	ಸಂರಚನಾ sbi_trap_info trap = {0};
	ulong hmask, hbase;

	ವಿಕಲ್ಪ (extid) {
	ಪಕ್ಷ SBI_EXT_0_1_SET_TIMER:
#ಯದಿ __riscv_xlen == 32
		sbi_timer_smode_event_start((((u64)regs->a1 << 32) | (u64)regs->a0));
#ಅನ್ಯಥಾ
		sbi_timer_smode_event_start((u64)regs->a0);
#endif
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_0_1_CONSOLE_PUTCHAR:
		sbi_putc(regs->a0);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_0_1_CONSOLE_GETCHAR:
		ret = sbi_getc();
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_0_1_CLEAR_IPI:
		sbi_ipi_clear_smode();
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_0_1_SEND_IPI:
		ಯದಿ (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			ret = sbi_ipi_send_smode(hmask, hbase);
		} ಅನ್ಯಥಾ {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_0_1_REMOTE_FENCE_I:
		ಯದಿ (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, 0, 0, 0, 0,
					  SBI_TLB_FENCE_I, source_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} ಅನ್ಯಥಾ {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_0_1_REMOTE_SFENCE_VMA:
		ಯದಿ (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, regs->a1, regs->a2, 0, 0,
					  SBI_TLB_SFENCE_VMA, source_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} ಅನ್ಯಥಾ {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_0_1_REMOTE_SFENCE_VMA_ASID:
		ಯದಿ (sbi_load_hart_mask_unpriv((ulong *)regs->a0,
					      &hmask, &hbase, &trap)) {
			SBI_TLB_INFO_INIT(&tlb_info, regs->a1,
					  regs->a2, regs->a3, 0,
					  SBI_TLB_SFENCE_VMA_ASID,
					  source_hart);
			ret = sbi_tlb_request(hmask, hbase, &tlb_info);
		} ಅನ್ಯಥಾ {
			sbi_trap_redirect(regs, &trap);
			out->skip_regs_update = true;
		}
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_0_1_SHUTDOWN:
		sbi_system_reset(SBI_SRST_RESET_TYPE_SHUTDOWN,
				 SBI_SRST_RESET_REASON_NONE);
		ಭಙ್ಗ;
	ಸ್ವಭಾವ:
		ret = SBI_ENOTSUPP;
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಸಂರಚನಾ sbi_ecall_extension ecall_legacy;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_legacy_register_extensions(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ sbi_ecall_register_extension(&ecall_legacy);
}

ಸಂರಚನಾ sbi_ecall_extension ecall_legacy = {
	.name			= "legacy",
	.extid_start		= SBI_EXT_0_1_SET_TIMER,
	.extid_end		= SBI_EXT_0_1_SHUTDOWN,
	.register_extensions	= sbi_ecall_legacy_register_extensions,
	.handle			= sbi_ecall_legacy_handler,
};
