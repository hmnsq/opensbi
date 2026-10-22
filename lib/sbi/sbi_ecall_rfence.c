/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_ecall.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_trap.h>
#شامل <sbi/sbi_tlb.h>

ساکن صحیح sbi_ecall_rfence_handler(غیرعلامتی طویل extid, غیرعلامتی طویل funcid,
				    ساخت sbi_trap_regs *regs,
				    ساخت sbi_ecall_return *برآمد)
{
	صحیح ret = 0;
	غیرعلامتی طویل vmid;
	ساخت sbi_tlb_info tlb_info;
	u32 ماخذ_hart = current_hartid();

	اگر (funcid >= SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA_VMID &&
	    funcid <= SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA)
		اگر (!misa_extension('H'))
			واپس SBI_ENOTSUPP;

	انتخاب (funcid) {
	حالت SBI_EXT_RFENCE_REMOTE_FENCE_I:
		SBI_TLB_INFO_INIT(&tlb_info, 0, 0, 0, 0,
				  SBI_TLB_FENCE_I, ماخذ_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		توڑو;
	حالت SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, 0,
				  SBI_TLB_HFENCE_GVMA, ماخذ_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		توڑو;
	حالت SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA_VMID:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, regs->a4,
				  SBI_TLB_HFENCE_GVMA_VMID, ماخذ_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		توڑو;
	حالت SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA:
		vmid = (csr_read(CSR_HGATP) & HGATP_VMID_نقاب);
		vmid = vmid >> HGATP_VMID_سرکاؤ;
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, vmid,
				  SBI_TLB_HFENCE_VVMA, ماخذ_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		توڑو;
	حالت SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA_ASID:
		vmid = (csr_read(CSR_HGATP) & HGATP_VMID_نقاب);
		vmid = vmid >> HGATP_VMID_سرکاؤ;
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, regs->a4,
				  vmid, SBI_TLB_HFENCE_VVMA_ASID, ماخذ_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		توڑو;
	حالت SBI_EXT_RFENCE_REMOTE_SFENCE_VMA:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, 0,
				  SBI_TLB_SFENCE_VMA, ماخذ_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		توڑو;
	حالت SBI_EXT_RFENCE_REMOTE_SFENCE_VMA_ASID:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, regs->a4, 0,
				  SBI_TLB_SFENCE_VMA_ASID, ماخذ_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		توڑو;
	طےشدہ:
		ret = SBI_ENOTSUPP;
	}

	واپس ret;
}

ساخت sbi_ecall_extension ecall_rfence;

ساکن صحیح sbi_ecall_rfence_رجسٹر_extensions(خالی)
{
	واپس sbi_ecall_رجسٹر_extension(&ecall_rfence);
}

ساخت sbi_ecall_extension ecall_rfence = {
	.name			= "rfnc",
	.extid_آغاز		= SBI_EXT_RFENCE,
	.extid_انتہا		= SBI_EXT_RFENCE,
	.رجسٹر_extensions	= sbi_ecall_rfence_رجسٹر_extensions,
	.handle			= sbi_ecall_rfence_handler,
};
