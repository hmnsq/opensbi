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
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_tlb.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_rfence_handler(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ extid, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ funcid,
				    ರಚನೆ sbi_trap_regs *regs,
				    ರಚನೆ sbi_ecall_return *ಹೊರಹೋಗು)
{
	ಪೂರ್ಣಾಂಕ ret = 0;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ vmid;
	ರಚನೆ sbi_tlb_info tlb_info;
	u32 ಮೂಲ_hart = current_hartid();

	ಆದರೆ (funcid >= SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA_VMID &&
	    funcid <= SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA)
		ಆದರೆ (!misa_extension('H'))
			ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;

	ಆಯ್ಕೆ (funcid) {
	ಸ್ಥಿತಿ SBI_EXT_RFENCE_REMOTE_FENCE_I:
		SBI_TLB_INFO_INIT(&tlb_info, 0, 0, 0, 0,
				  SBI_TLB_FENCE_I, ಮೂಲ_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, 0,
				  SBI_TLB_HFENCE_GVMA, ಮೂಲ_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA_VMID:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, regs->a4,
				  SBI_TLB_HFENCE_GVMA_VMID, ಮೂಲ_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA:
		vmid = (csr_read(CSR_HGATP) & HGATP_VMID_ಮುಖವಾಡ);
		vmid = vmid >> HGATP_VMID_ಸರಿಸು;
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, vmid,
				  SBI_TLB_HFENCE_VVMA, ಮೂಲ_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA_ASID:
		vmid = (csr_read(CSR_HGATP) & HGATP_VMID_ಮುಖವಾಡ);
		vmid = vmid >> HGATP_VMID_ಸರಿಸು;
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, regs->a4,
				  vmid, SBI_TLB_HFENCE_VVMA_ASID, ಮೂಲ_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_RFENCE_REMOTE_SFENCE_VMA:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, 0,
				  SBI_TLB_SFENCE_VMA, ಮೂಲ_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_RFENCE_REMOTE_SFENCE_VMA_ASID:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, regs->a4, 0,
				  SBI_TLB_SFENCE_VMA_ASID, ಮೂಲ_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ret = SBI_ENOTSUPP;
	}

	ಹಿಂತಿರುಗಿ ret;
}

ರಚನೆ sbi_ecall_extension ecall_rfence;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_rfence_ರಿಜಿಸ್ಟರ್_extensions(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ sbi_ecall_ರಿಜಿಸ್ಟರ್_extension(&ecall_rfence);
}

ರಚನೆ sbi_ecall_extension ecall_rfence = {
	.name			= "rfnc",
	.extid_ಆರಂಭ		= SBI_EXT_RFENCE,
	.extid_ಕೊನೆ		= SBI_EXT_RFENCE,
	.ರಿಜಿಸ್ಟರ್_extensions	= sbi_ecall_rfence_ರಿಜಿಸ್ಟರ್_extensions,
	.handle			= sbi_ecall_rfence_handler,
};
