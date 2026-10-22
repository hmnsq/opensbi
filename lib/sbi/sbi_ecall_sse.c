#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_ecall.h>
#अन्तर्भाव <sbi/sbi_trap.h>
#अन्तर्भाव <sbi/sbi_sse.h>

स्थिर पूर्णाङ्क sbi_ecall_sse_handler(अचिह्नित दीर्घ extid, अचिह्नित दीर्घ funcid,
				  संरचना sbi_trap_regs *regs,
				  संरचना sbi_ecall_return *बहिर्गमनम्)
{
	पूर्णाङ्क ret;

	विकल्प (funcid) {
	पक्ष SBI_EXT_SSE_READ_ATTR:
		ret = sbi_sse_read_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		भङ्ग;
	पक्ष SBI_EXT_SSE_WRITE_ATTR:
		ret = sbi_sse_write_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		भङ्ग;
	पक्ष SBI_EXT_SSE_पञ्जिकाकोशः:
		ret = sbi_sse_पञ्जिकाकोशः(regs->a0, regs->a1, regs->a2);
		भङ्ग;
	पक्ष SBI_EXT_SSE_UNREGISTER:
		ret = sbi_sse_unregister(regs->a0);
		भङ्ग;
	पक्ष SBI_EXT_SSE_ENABLE:
		ret = sbi_sse_enable(regs->a0);
		भङ्ग;
	पक्ष SBI_EXT_SSE_DISABLE:
		ret = sbi_sse_disable(regs->a0);
		भङ्ग;
	पक्ष SBI_EXT_SSE_COMPLETE:
		ret = sbi_sse_complete(regs, बहिर्गमनम्);
		भङ्ग;
	पक्ष SBI_EXT_SSE_INJECT:
		ret = sbi_sse_inject_from_ecall(regs->a0, regs->a1, बहिर्गमनम्);
		भङ्ग;
	पक्ष SBI_EXT_SSE_HART_आवरणम्:
		ret = sbi_sse_hart_आवरणम्();
		भङ्ग;
	पक्ष SBI_EXT_SSE_HART_UNMASK:
		ret = sbi_sse_hart_unmask();
		भङ्ग;
	स्वभाव:
		ret = SBI_ENOTSUPP;
	}
	प्रत्यागम ret;
}

संरचना sbi_ecall_extension ecall_sse;

स्थिर पूर्णाङ्क sbi_ecall_sse_पञ्जिकाकोशः_extensions(शून्य)
{
	प्रत्यागम sbi_ecall_पञ्जिकाकोशः_extension(&ecall_sse);
}

संरचना sbi_ecall_extension ecall_sse = {
	.name			= "sse",
	.extid_start		= SBI_EXT_SSE,
	.extid_अन्तः		= SBI_EXT_SSE,
	.पञ्जिकाकोशः_extensions	= sbi_ecall_sse_पञ्जिकाकोशः_extensions,
	.handle			= sbi_ecall_sse_handler,
};
