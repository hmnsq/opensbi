/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_tlb.h>

ਸਥਿਰ ਪੂਰਨ sbi_ecall_rfence_handler(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ extid, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ funcid,
				    ਬਣਤਰ sbi_trap_regs *regs,
				    ਬਣਤਰ sbi_ecall_return *ਬਾਹਰਲਾ)
{
	ਪੂਰਨ ret = 0;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ vmid;
	ਬਣਤਰ sbi_tlb_info tlb_info;
	u32 ਸਰੋਤ_hart = current_hartid();

	ਜੇ (funcid >= SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA_VMID &&
	    funcid <= SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA)
		ਜੇ (!misa_extension('H'))
			ਵਾਪਸ SBI_ENOTSUPP;

	ਚੋਣ (funcid) {
	ਹਾਲਤ SBI_EXT_RFENCE_REMOTE_FENCE_I:
		SBI_TLB_INFO_INIT(&tlb_info, 0, 0, 0, 0,
				  SBI_TLB_FENCE_I, ਸਰੋਤ_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, 0,
				  SBI_TLB_HFENCE_GVMA, ਸਰੋਤ_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA_VMID:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, regs->a4,
				  SBI_TLB_HFENCE_GVMA_VMID, ਸਰੋਤ_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA:
		vmid = (csr_read(CSR_HGATP) & HGATP_VMID_ਨਕਾਬ);
		vmid = vmid >> HGATP_VMID_ਖਿਸਕਾਓ;
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, vmid,
				  SBI_TLB_HFENCE_VVMA, ਸਰੋਤ_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA_ASID:
		vmid = (csr_read(CSR_HGATP) & HGATP_VMID_ਨਕਾਬ);
		vmid = vmid >> HGATP_VMID_ਖਿਸਕਾਓ;
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, regs->a4,
				  vmid, SBI_TLB_HFENCE_VVMA_ASID, ਸਰੋਤ_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_RFENCE_REMOTE_SFENCE_VMA:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, 0,
				  SBI_TLB_SFENCE_VMA, ਸਰੋਤ_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_RFENCE_REMOTE_SFENCE_VMA_ASID:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, regs->a4, 0,
				  SBI_TLB_SFENCE_VMA_ASID, ਸਰੋਤ_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		ਰੋਕੋ;
	ਮੂਲ:
		ret = SBI_ENOTSUPP;
	}

	ਵਾਪਸ ret;
}

ਬਣਤਰ sbi_ecall_extension ecall_rfence;

ਸਥਿਰ ਪੂਰਨ sbi_ecall_rfence_ਰਜਿਸਟਰ_extensions(ਖਾਲੀ)
{
	ਵਾਪਸ sbi_ecall_ਰਜਿਸਟਰ_extension(&ecall_rfence);
}

ਬਣਤਰ sbi_ecall_extension ecall_rfence = {
	.name			= "rfnc",
	.extid_ਸ਼ੁਰੂ		= SBI_EXT_RFENCE,
	.extid_ਸਿਰਾ		= SBI_EXT_RFENCE,
	.ਰਜਿਸਟਰ_extensions	= sbi_ecall_rfence_ਰਜਿਸਟਰ_extensions,
	.handle			= sbi_ecall_rfence_handler,
};
