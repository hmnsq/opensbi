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
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_ecall.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/sbi_tlb.h>

ساكن صحيح sbi_ecall_rfence_handler(unsigned long extid, unsigned long funcid,
				    بنية sbi_trap_regs *regs,
				    بنية sbi_ecall_return *out)
{
	صحيح ret = 0;
	unsigned long vmid;
	بنية sbi_tlb_info tlb_info;
	u32 source_hart = current_hartid();

	𐡄𐡍 (funcid >= SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA_VMID &&
	    funcid <= SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA)
		𐡄𐡍 (!misa_extension('H'))
			أرجع SBI_ENOTSUPP;

	بدل (funcid) {
	حالة SBI_EXT_RFENCE_REMOTE_FENCE_I:
		SBI_TLB_INFO_INIT(&tlb_info, 0, 0, 0, 0,
				  SBI_TLB_FENCE_I, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		اكسر;
	حالة SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, 0,
				  SBI_TLB_HFENCE_GVMA, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		اكسر;
	حالة SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA_VMID:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, regs->a4,
				  SBI_TLB_HFENCE_GVMA_VMID, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		اكسر;
	حالة SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA:
		vmid = (csr_read(CSR_HGATP) & HGATP_VMID_MASK);
		vmid = vmid >> HGATP_VMID_SHIFT;
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, vmid,
				  SBI_TLB_HFENCE_VVMA, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		اكسر;
	حالة SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA_ASID:
		vmid = (csr_read(CSR_HGATP) & HGATP_VMID_MASK);
		vmid = vmid >> HGATP_VMID_SHIFT;
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, regs->a4,
				  vmid, SBI_TLB_HFENCE_VVMA_ASID, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		اكسر;
	حالة SBI_EXT_RFENCE_REMOTE_SFENCE_VMA:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, 0,
				  SBI_TLB_SFENCE_VMA, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		اكسر;
	حالة SBI_EXT_RFENCE_REMOTE_SFENCE_VMA_ASID:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, regs->a4, 0,
				  SBI_TLB_SFENCE_VMA_ASID, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		اكسر;
	افتراضي:
		ret = SBI_ENOTSUPP;
	}

	أرجع ret;
}

بنية sbi_ecall_extension ecall_rfence;

ساكن صحيح sbi_ecall_rfence_register_extensions(فراغ)
{
	أرجع sbi_ecall_register_extension(&ecall_rfence);
}

بنية sbi_ecall_extension ecall_rfence = {
	.name			= "rfnc",
	.extid_start		= SBI_EXT_RFENCE,
	.extid_end		= SBI_EXT_RFENCE,
	.register_extensions	= sbi_ecall_rfence_register_extensions,
	.handle			= sbi_ecall_rfence_handler,
};
