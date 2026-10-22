#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_ecall.h>
#अन्तर्भाव <sbi/sbi_trap.h>
#अन्तर्भाव <sbi/sbi_sse.h>

स्थिर पूर्णांक sbi_ecall_sse_handler(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ funcid,
				  संरचना sbi_trap_regs *regs,
				  संरचना sbi_ecall_return *out)
{
	पूर्णांक ret;

	निवड (funcid) {
	स्थिती SBI_EXT_SSE_READ_ATTR:
		ret = sbi_sse_read_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		थांबा;
	स्थिती SBI_EXT_SSE_WRITE_ATTR:
		ret = sbi_sse_write_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		थांबा;
	स्थिती SBI_EXT_SSE_REGISTER:
		ret = sbi_sse_register(regs->a0, regs->a1, regs->a2);
		थांबा;
	स्थिती SBI_EXT_SSE_UNREGISTER:
		ret = sbi_sse_unregister(regs->a0);
		थांबा;
	स्थिती SBI_EXT_SSE_ENABLE:
		ret = sbi_sse_enable(regs->a0);
		थांबा;
	स्थिती SBI_EXT_SSE_DISABLE:
		ret = sbi_sse_disable(regs->a0);
		थांबा;
	स्थिती SBI_EXT_SSE_COMPLETE:
		ret = sbi_sse_complete(regs, out);
		थांबा;
	स्थिती SBI_EXT_SSE_INJECT:
		ret = sbi_sse_inject_from_ecall(regs->a0, regs->a1, out);
		थांबा;
	स्थिती SBI_EXT_SSE_HART_MASK:
		ret = sbi_sse_hart_mask();
		थांबा;
	स्थिती SBI_EXT_SSE_HART_UNMASK:
		ret = sbi_sse_hart_unmask();
		थांबा;
	पूर्वनिर्धारित:
		ret = SBI_ENOTSUPP;
	}
	परत ret;
}

संरचना sbi_ecall_extension ecall_sse;

स्थिर पूर्णांक sbi_ecall_sse_register_extensions(शून्य)
{
	परत sbi_ecall_register_extension(&ecall_sse);
}

संरचना sbi_ecall_extension ecall_sse = {
	.name			= "sse",
	.extid_start		= SBI_EXT_SSE,
	.extid_end		= SBI_EXT_SSE,
	.register_extensions	= sbi_ecall_sse_register_extensions,
	.handle			= sbi_ecall_sse_handler,
};
