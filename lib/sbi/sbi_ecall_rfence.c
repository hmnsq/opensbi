/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_ecall.h>
#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_trap.h>
#समावेश <sbi/sbi_tlb.h>

अटल पूर्णांक sbi_ecall_rfence_handler(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ funcid,
				    काष्ठा sbi_trap_regs *regs,
				    काष्ठा sbi_ecall_return *out)
{
	पूर्णांक ret = 0;
	अचिन्हित दीर्घ vmid;
	काष्ठा sbi_tlb_info tlb_info;
	u32 source_hart = current_hartid();

	अगर (funcid >= SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA_VMID &&
	    funcid <= SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA)
		अगर (!misa_extension('H'))
			वापस SBI_ENOTSUPP;

	चयन (funcid) {
	हाल SBI_EXT_RFENCE_REMOTE_FENCE_I:
		SBI_TLB_INFO_INIT(&tlb_info, 0, 0, 0, 0,
				  SBI_TLB_FENCE_I, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		अवरोध;
	हाल SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, 0,
				  SBI_TLB_HFENCE_GVMA, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		अवरोध;
	हाल SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA_VMID:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, regs->a4,
				  SBI_TLB_HFENCE_GVMA_VMID, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		अवरोध;
	हाल SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA:
		vmid = (csr_read(CSR_HGATP) & HGATP_VMID_MASK);
		vmid = vmid >> HGATP_VMID_SHIFT;
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, vmid,
				  SBI_TLB_HFENCE_VVMA, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		अवरोध;
	हाल SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA_ASID:
		vmid = (csr_read(CSR_HGATP) & HGATP_VMID_MASK);
		vmid = vmid >> HGATP_VMID_SHIFT;
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, regs->a4,
				  vmid, SBI_TLB_HFENCE_VVMA_ASID, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		अवरोध;
	हाल SBI_EXT_RFENCE_REMOTE_SFENCE_VMA:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, 0,
				  SBI_TLB_SFENCE_VMA, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		अवरोध;
	हाल SBI_EXT_RFENCE_REMOTE_SFENCE_VMA_ASID:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, regs->a4, 0,
				  SBI_TLB_SFENCE_VMA_ASID, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		अवरोध;
	शेष:
		ret = SBI_ENOTSUPP;
	}

	वापस ret;
}

काष्ठा sbi_ecall_extension ecall_rfence;

अटल पूर्णांक sbi_ecall_rfence_register_extensions(व्योम)
{
	वापस sbi_ecall_register_extension(&ecall_rfence);
}

काष्ठा sbi_ecall_extension ecall_rfence = {
	.name			= "rfnc",
	.extid_start		= SBI_EXT_RFENCE,
	.extid_end		= SBI_EXT_RFENCE,
	.register_extensions	= sbi_ecall_rfence_register_extensions,
	.handle			= sbi_ecall_rfence_handler,
};
