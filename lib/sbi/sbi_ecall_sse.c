#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_ecall.h>
#અન્તર્ભાવ <sbi/sbi_trap.h>
#અન્તર્ભાવ <sbi/sbi_sse.h>

સ્થિર પૂર્ણાંક sbi_ecall_sse_handler(unsigned long extid, unsigned long funcid,
				  સંરચના sbi_trap_regs *regs,
				  સંરચના sbi_ecall_return *out)
{
	પૂર્ણાંક ret;

	પસંદગી (funcid) {
	સ્થિતિ SBI_EXT_SSE_READ_ATTR:
		ret = sbi_sse_read_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		તોડો;
	સ્થિતિ SBI_EXT_SSE_WRITE_ATTR:
		ret = sbi_sse_write_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		તોડો;
	સ્થિતિ SBI_EXT_SSE_REGISTER:
		ret = sbi_sse_register(regs->a0, regs->a1, regs->a2);
		તોડો;
	સ્થિતિ SBI_EXT_SSE_UNREGISTER:
		ret = sbi_sse_unregister(regs->a0);
		તોડો;
	સ્થિતિ SBI_EXT_SSE_ENABLE:
		ret = sbi_sse_enable(regs->a0);
		તોડો;
	સ્થિતિ SBI_EXT_SSE_DISABLE:
		ret = sbi_sse_disable(regs->a0);
		તોડો;
	સ્થિતિ SBI_EXT_SSE_COMPLETE:
		ret = sbi_sse_complete(regs, out);
		તોડો;
	સ્થિતિ SBI_EXT_SSE_INJECT:
		ret = sbi_sse_inject_from_ecall(regs->a0, regs->a1, out);
		તોડો;
	સ્થિતિ SBI_EXT_SSE_HART_MASK:
		ret = sbi_sse_hart_mask();
		તોડો;
	સ્થિતિ SBI_EXT_SSE_HART_UNMASK:
		ret = sbi_sse_hart_unmask();
		તોડો;
	મૂળભૂત:
		ret = SBI_ENOTSUPP;
	}
	પાછા ret;
}

સંરચના sbi_ecall_extension ecall_sse;

સ્થિર પૂર્ણાંક sbi_ecall_sse_register_extensions(શૂન્ય)
{
	પાછા sbi_ecall_register_extension(&ecall_sse);
}

સંરચના sbi_ecall_extension ecall_sse = {
	.name			= "sse",
	.extid_start		= SBI_EXT_SSE,
	.extid_end		= SBI_EXT_SSE,
	.register_extensions	= sbi_ecall_sse_register_extensions,
	.handle			= sbi_ecall_sse_handler,
};
