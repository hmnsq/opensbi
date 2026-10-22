#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_trap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_sse.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_sse_handler(unsigned long extid, unsigned long funcid,
				  ସଂରଚନା sbi_trap_regs *regs,
				  ସଂରଚନା sbi_ecall_return *out)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

	ବାଛ (funcid) {
	ସ୍ଥିତି SBI_EXT_SSE_READ_ATTR:
		ret = sbi_sse_read_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_SSE_WRITE_ATTR:
		ret = sbi_sse_write_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_SSE_REGISTER:
		ret = sbi_sse_register(regs->a0, regs->a1, regs->a2);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_SSE_UNREGISTER:
		ret = sbi_sse_unregister(regs->a0);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_SSE_ENABLE:
		ret = sbi_sse_enable(regs->a0);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_SSE_DISABLE:
		ret = sbi_sse_disable(regs->a0);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_SSE_COMPLETE:
		ret = sbi_sse_complete(regs, out);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_SSE_INJECT:
		ret = sbi_sse_inject_from_ecall(regs->a0, regs->a1, out);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_SSE_HART_MASK:
		ret = sbi_sse_hart_mask();
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_SSE_HART_UNMASK:
		ret = sbi_sse_hart_unmask();
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ret = SBI_ENOTSUPP;
	}
	ଫେରନ୍ତୁ ret;
}

ସଂରଚନା sbi_ecall_extension ecall_sse;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_sse_register_extensions(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ sbi_ecall_register_extension(&ecall_sse);
}

ସଂରଚନା sbi_ecall_extension ecall_sse = {
	.name			= "sse",
	.extid_start		= SBI_EXT_SSE,
	.extid_end		= SBI_EXT_SSE,
	.register_extensions	= sbi_ecall_sse_register_extensions,
	.handle			= sbi_ecall_sse_handler,
};
