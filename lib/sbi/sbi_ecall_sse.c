#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_ecall.h>
#समावेश <sbi/sbi_trap.h>
#समावेश <sbi/sbi_sse.h>

अटल पूर्णांक sbi_ecall_sse_handler(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ funcid,
				  काष्ठा sbi_trap_regs *regs,
				  काष्ठा sbi_ecall_return *out)
{
	पूर्णांक ret;

	चयन (funcid) {
	हाल SBI_EXT_SSE_READ_ATTR:
		ret = sbi_sse_read_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		अवरोध;
	हाल SBI_EXT_SSE_WRITE_ATTR:
		ret = sbi_sse_write_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		अवरोध;
	हाल SBI_EXT_SSE_REGISTER:
		ret = sbi_sse_register(regs->a0, regs->a1, regs->a2);
		अवरोध;
	हाल SBI_EXT_SSE_UNREGISTER:
		ret = sbi_sse_unregister(regs->a0);
		अवरोध;
	हाल SBI_EXT_SSE_ENABLE:
		ret = sbi_sse_enable(regs->a0);
		अवरोध;
	हाल SBI_EXT_SSE_DISABLE:
		ret = sbi_sse_disable(regs->a0);
		अवरोध;
	हाल SBI_EXT_SSE_COMPLETE:
		ret = sbi_sse_complete(regs, out);
		अवरोध;
	हाल SBI_EXT_SSE_INJECT:
		ret = sbi_sse_inject_from_ecall(regs->a0, regs->a1, out);
		अवरोध;
	हाल SBI_EXT_SSE_HART_MASK:
		ret = sbi_sse_hart_mask();
		अवरोध;
	हाल SBI_EXT_SSE_HART_UNMASK:
		ret = sbi_sse_hart_unmask();
		अवरोध;
	शेष:
		ret = SBI_ENOTSUPP;
	}
	वापस ret;
}

काष्ठा sbi_ecall_extension ecall_sse;

अटल पूर्णांक sbi_ecall_sse_register_extensions(व्योम)
{
	वापस sbi_ecall_register_extension(&ecall_sse);
}

काष्ठा sbi_ecall_extension ecall_sse = {
	.name			= "sse",
	.extid_start		= SBI_EXT_SSE,
	.extid_end		= SBI_EXT_SSE,
	.register_extensions	= sbi_ecall_sse_register_extensions,
	.handle			= sbi_ecall_sse_handler,
};
