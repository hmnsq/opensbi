/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_ecall.h>
#అన్తర్భావ <sbi/sbi_ecall_interface.h>
#అన్తర్భావ <sbi/sbi_trap.h>
#అన్తర్భావ <sbi/sbi_tlb.h>

స్థిర పూర్ణాంకం sbi_ecall_rfence_handler(గుర్తులేని దీర్ఘ extid, గుర్తులేని దీర్ఘ funcid,
				    నిర్మాణం sbi_trap_regs *regs,
				    నిర్మాణం sbi_ecall_return *బహిర్గతం)
{
	పూర్ణాంకం ret = 0;
	గుర్తులేని దీర్ఘ vmid;
	నిర్మాణం sbi_tlb_info tlb_info;
	u32 ఆధారం_hart = current_hartid();

	ఒకవేళ (funcid >= SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA_VMID &&
	    funcid <= SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA)
		ఒకవేళ (!misa_extension('H'))
			తిరిగి SBI_ENOTSUPP;

	ఎంపిక (funcid) {
	స్థితి SBI_EXT_RFENCE_REMOTE_FENCE_I:
		SBI_TLB_INFO_INIT(&tlb_info, 0, 0, 0, 0,
				  SBI_TLB_FENCE_I, ఆధారం_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ఆపు;
	స్థితి SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, 0,
				  SBI_TLB_HFENCE_GVMA, ఆధారం_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ఆపు;
	స్థితి SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA_VMID:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, regs->a4,
				  SBI_TLB_HFENCE_GVMA_VMID, ఆధారం_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ఆపు;
	స్థితి SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA:
		vmid = (csr_read(CSR_HGATP) & HGATP_VMID_ముసుగు);
		vmid = vmid >> HGATP_VMID_జరుపు;
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, vmid,
				  SBI_TLB_HFENCE_VVMA, ఆధారం_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ఆపు;
	స్థితి SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA_ASID:
		vmid = (csr_read(CSR_HGATP) & HGATP_VMID_ముసుగు);
		vmid = vmid >> HGATP_VMID_జరుపు;
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, regs->a4,
				  vmid, SBI_TLB_HFENCE_VVMA_ASID, ఆధారం_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ఆపు;
	స్థితి SBI_EXT_RFENCE_REMOTE_SFENCE_VMA:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, 0,
				  SBI_TLB_SFENCE_VMA, ఆధారం_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ఆపు;
	స్థితి SBI_EXT_RFENCE_REMOTE_SFENCE_VMA_ASID:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, regs->a4, 0,
				  SBI_TLB_SFENCE_VMA_ASID, ఆధారం_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ఆపు;
	అప్రమేయ:
		ret = SBI_ENOTSUPP;
	}

	తిరిగి ret;
}

నిర్మాణం sbi_ecall_extension ecall_rfence;

స్థిర పూర్ణాంకం sbi_ecall_rfence_రిజిస్టరు_extensions(శూన్య)
{
	తిరిగి sbi_ecall_రిజిస్టరు_extension(&ecall_rfence);
}

నిర్మాణం sbi_ecall_extension ecall_rfence = {
	.name			= "rfnc",
	.extid_ప్రారంభం		= SBI_EXT_RFENCE,
	.extid_చివర		= SBI_EXT_RFENCE,
	.రిజిస్టరు_extensions	= sbi_ecall_rfence_రిజిస్టరు_extensions,
	.handle			= sbi_ecall_rfence_handler,
};
