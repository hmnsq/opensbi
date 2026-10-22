/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_ecall.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_trap.h>
#अन्तर्भाव <sbi/sbi_tlb.h>

स्थिर पूर्णाङ्क sbi_ecall_rfence_handler(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ funcid,
				    संरचना sbi_trap_regs *regs,
				    संरचना sbi_ecall_return *out)
{
	पूर्णाङ्क ret = 0;
	अचिन्हित दीर्घ vmid;
	संरचना sbi_tlb_info tlb_info;
	u32 source_hart = current_hartid();

	यदि (funcid >= SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA_VMID &&
	    funcid <= SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA)
		यदि (!misa_extension('H'))
			प्रत्यागम SBI_ENOTSUPP;

	विकल्प (funcid) {
	पक्ष SBI_EXT_RFENCE_REMOTE_FENCE_I:
		SBI_TLB_INFO_INIT(&tlb_info, 0, 0, 0, 0,
				  SBI_TLB_FENCE_I, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		भङ्ग;
	पक्ष SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, 0,
				  SBI_TLB_HFENCE_GVMA, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		भङ्ग;
	पक्ष SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA_VMID:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, regs->a4,
				  SBI_TLB_HFENCE_GVMA_VMID, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		भङ्ग;
	पक्ष SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA:
		vmid = (csr_read(CSR_HGATP) & HGATP_VMID_MASK);
		vmid = vmid >> HGATP_VMID_SHIFT;
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, vmid,
				  SBI_TLB_HFENCE_VVMA, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		भङ्ग;
	पक्ष SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA_ASID:
		vmid = (csr_read(CSR_HGATP) & HGATP_VMID_MASK);
		vmid = vmid >> HGATP_VMID_SHIFT;
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, regs->a4,
				  vmid, SBI_TLB_HFENCE_VVMA_ASID, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		भङ्ग;
	पक्ष SBI_EXT_RFENCE_REMOTE_SFENCE_VMA:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, 0, 0,
				  SBI_TLB_SFENCE_VMA, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		भङ्ग;
	पक्ष SBI_EXT_RFENCE_REMOTE_SFENCE_VMA_ASID:
		SBI_TLB_INFO_INIT(&tlb_info, regs->a2, regs->a3, regs->a4, 0,
				  SBI_TLB_SFENCE_VMA_ASID, source_hart);
		ret = sbi_tlb_request(regs->a0, regs->a1, &tlb_info);
		भङ्ग;
	स्वभाव:
		ret = SBI_ENOTSUPP;
	}

	प्रत्यागम ret;
}

संरचना sbi_ecall_extension ecall_rfence;

स्थिर पूर्णाङ्क sbi_ecall_rfence_register_extensions(शून्य)
{
	प्रत्यागम sbi_ecall_register_extension(&ecall_rfence);
}

संरचना sbi_ecall_extension ecall_rfence = {
	.name			= "rfnc",
	.extid_start		= SBI_EXT_RFENCE,
	.extid_end		= SBI_EXT_RFENCE,
	.register_extensions	= sbi_ecall_rfence_register_extensions,
	.handle			= sbi_ecall_rfence_handler,
};
