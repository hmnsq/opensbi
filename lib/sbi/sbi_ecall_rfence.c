/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#অন্তর্ভাৱ <sbi/riscv_asm.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_ecall.h>
#অন্তর্ভাৱ <sbi/sbi_ecall_interface.h>
#অন্তর্ভাৱ <sbi/sbi_trap.h>
#অন্তর্ভাৱ <sbi/sbi_tlb.h>

স্থির পূর্ণাঙ্ক sbi_ecall_rfence_handler(unsigned long extid, unsigned long funcid,
				    সংরচনা sbi_trap_regs *regs,
				    সংরচনা sbi_ecall_return *out)
{
	পূর্ণাঙ্ক ret = 0;
	unsigned long vmid;
	সংরচনা sbi_tlb_info tlb_info;
	u32 source_hart = current_hartid();

	যদি (funcid >= SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA_VMID &&
	    funcid <= SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA)
		যদি (!misa_extension('H'))
			ঘূৰাই SBI_ENOTSUPP;

	ৱিকল্প (funcid) {
	পক্ষ SBI_EXT_RFENCE_REMOTE_FENCE_I:
		SBI_TLB_INFO_INIT(&tlb_info, 0, 0, 0, 0,
				  SBI_TLB_FENCE_I, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ভঙ্গ;
	পক্ষ SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, 0,
				  SBI_TLB_HFENCE_GVMA, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ভঙ্গ;
	পক্ষ SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA_VMID:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, regs->a4,
				  SBI_TLB_HFENCE_GVMA_VMID, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ভঙ্গ;
	পক্ষ SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA:
		vmid = (csr_read(CSR_HGATP) & HGATP_VMID_MASK);
		vmid = vmid >> HGATP_VMID_SHIFT;
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, vmid,
				  SBI_TLB_HFENCE_VVMA, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ভঙ্গ;
	পক্ষ SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA_ASID:
		vmid = (csr_read(CSR_HGATP) & HGATP_VMID_MASK);
		vmid = vmid >> HGATP_VMID_SHIFT;
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, regs->a4,
				  vmid, SBI_TLB_HFENCE_VVMA_ASID, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ভঙ্গ;
	পক্ষ SBI_EXT_RFENCE_REMOTE_SFENCE_VMA:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, 0,
				  SBI_TLB_SFENCE_VMA, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ভঙ্গ;
	পক্ষ SBI_EXT_RFENCE_REMOTE_SFENCE_VMA_ASID:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, regs->a4, 0,
				  SBI_TLB_SFENCE_VMA_ASID, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ভঙ্গ;
	স্ৱভাৱ:
		ret = SBI_ENOTSUPP;
	}

	ঘূৰাই ret;
}

সংরচনা sbi_ecall_extension ecall_rfence;

স্থির পূর্ণাঙ্ক sbi_ecall_rfence_register_extensions(শূন্য)
{
	ঘূৰাই sbi_ecall_register_extension(&ecall_rfence);
}

সংরচনা sbi_ecall_extension ecall_rfence = {
	.name			= "rfnc",
	.extid_start		= SBI_EXT_RFENCE,
	.extid_end		= SBI_EXT_RFENCE,
	.register_extensions	= sbi_ecall_rfence_register_extensions,
	.handle			= sbi_ecall_rfence_handler,
};
