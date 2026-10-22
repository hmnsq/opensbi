/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall_interface.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_trap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_tlb.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_rfence_handler(unsigned long extid, unsigned long funcid,
				    ସଂରଚନା sbi_trap_regs *regs,
				    ସଂରଚନା sbi_ecall_return *out)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 0;
	unsigned long vmid;
	ସଂରଚନା sbi_tlb_info tlb_info;
	u32 source_hart = current_hartid();

	ଯଦି (funcid >= SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA_VMID &&
	    funcid <= SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA)
		ଯଦି (!misa_extension('H'))
			ଫେରନ୍ତୁ SBI_ENOTSUPP;

	ବାଛ (funcid) {
	ସ୍ଥିତି SBI_EXT_RFENCE_REMOTE_FENCE_I:
		SBI_TLB_INFO_INIT(&tlb_info, 0, 0, 0, 0,
				  SBI_TLB_FENCE_I, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, 0,
				  SBI_TLB_HFENCE_GVMA, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA_VMID:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, regs->a4,
				  SBI_TLB_HFENCE_GVMA_VMID, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA:
		vmid = (csr_read(CSR_HGATP) & HGATP_VMID_MASK);
		vmid = vmid >> HGATP_VMID_SHIFT;
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, vmid,
				  SBI_TLB_HFENCE_VVMA, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA_ASID:
		vmid = (csr_read(CSR_HGATP) & HGATP_VMID_MASK);
		vmid = vmid >> HGATP_VMID_SHIFT;
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, regs->a4,
				  vmid, SBI_TLB_HFENCE_VVMA_ASID, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_RFENCE_REMOTE_SFENCE_VMA:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, 0,
				  SBI_TLB_SFENCE_VMA, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_RFENCE_REMOTE_SFENCE_VMA_ASID:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, regs->a4, 0,
				  SBI_TLB_SFENCE_VMA_ASID, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ret = SBI_ENOTSUPP;
	}

	ଫେରନ୍ତୁ ret;
}

ସଂରଚନା sbi_ecall_extension ecall_rfence;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_rfence_register_extensions(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ sbi_ecall_register_extension(&ecall_rfence);
}

ସଂରଚନା sbi_ecall_extension ecall_rfence = {
	.name			= "rfnc",
	.extid_start		= SBI_EXT_RFENCE,
	.extid_end		= SBI_EXT_RFENCE,
	.register_extensions	= sbi_ecall_rfence_register_extensions,
	.handle			= sbi_ecall_rfence_handler,
};
