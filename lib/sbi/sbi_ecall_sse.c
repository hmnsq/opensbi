#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_ecall.h>
#అన్తర్భావ <sbi/sbi_trap.h>
#అన్తర్భావ <sbi/sbi_sse.h>

స్థిర పూర్ణాంకం sbi_ecall_sse_handler(గుర్తులేని దీర్ఘ extid, గుర్తులేని దీర్ఘ funcid,
				  నిర్మాణం sbi_trap_regs *regs,
				  నిర్మాణం sbi_ecall_return *బహిర్గతం)
{
	పూర్ణాంకం ret;

	ఎంపిక (funcid) {
	స్థితి SBI_EXT_SSE_READ_ATTR:
		ret = sbi_sse_read_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		ఆపు;
	స్థితి SBI_EXT_SSE_WRITE_ATTR:
		ret = sbi_sse_write_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		ఆపు;
	స్థితి SBI_EXT_SSE_రిజిస్టరు:
		ret = sbi_sse_రిజిస్టరు(regs->a0, regs->a1, regs->a2);
		ఆపు;
	స్థితి SBI_EXT_SSE_UNREGISTER:
		ret = sbi_sse_unregister(regs->a0);
		ఆపు;
	స్థితి SBI_EXT_SSE_ENABLE:
		ret = sbi_sse_enable(regs->a0);
		ఆపు;
	స్థితి SBI_EXT_SSE_DISABLE:
		ret = sbi_sse_disable(regs->a0);
		ఆపు;
	స్థితి SBI_EXT_SSE_COMPLETE:
		ret = sbi_sse_complete(regs, బహిర్గతం);
		ఆపు;
	స్థితి SBI_EXT_SSE_INJECT:
		ret = sbi_sse_inject_from_ecall(regs->a0, regs->a1, బహిర్గతం);
		ఆపు;
	స్థితి SBI_EXT_SSE_HART_ముసుగు:
		ret = sbi_sse_hart_ముసుగు();
		ఆపు;
	స్థితి SBI_EXT_SSE_HART_UNMASK:
		ret = sbi_sse_hart_unmask();
		ఆపు;
	అప్రమేయ:
		ret = SBI_ENOTSUPP;
	}
	తిరిగి ret;
}

నిర్మాణం sbi_ecall_extension ecall_sse;

స్థిర పూర్ణాంకం sbi_ecall_sse_రిజిస్టరు_extensions(శూన్య)
{
	తిరిగి sbi_ecall_రిజిస్టరు_extension(&ecall_sse);
}

నిర్మాణం sbi_ecall_extension ecall_sse = {
	.name			= "sse",
	.extid_ప్రారంభం		= SBI_EXT_SSE,
	.extid_చివర		= SBI_EXT_SSE,
	.రిజిస్టరు_extensions	= sbi_ecall_sse_రిజిస్టరు_extensions,
	.handle			= sbi_ecall_sse_handler,
};
