#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_sse.h>

ਸਥਿਰ ਪੂਰਨ sbi_ecall_sse_handler(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ extid, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ funcid,
				  ਬਣਤਰ sbi_trap_regs *regs,
				  ਬਣਤਰ sbi_ecall_return *ਬਾਹਰਲਾ)
{
	ਪੂਰਨ ret;

	ਚੋਣ (funcid) {
	ਹਾਲਤ SBI_EXT_SSE_READ_ATTR:
		ret = sbi_sse_read_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_SSE_WRITE_ATTR:
		ret = sbi_sse_write_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_SSE_ਰਜਿਸਟਰ:
		ret = sbi_sse_ਰਜਿਸਟਰ(regs->a0, regs->a1, regs->a2);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_SSE_UNREGISTER:
		ret = sbi_sse_unregister(regs->a0);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_SSE_ENABLE:
		ret = sbi_sse_enable(regs->a0);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_SSE_DISABLE:
		ret = sbi_sse_disable(regs->a0);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_SSE_COMPLETE:
		ret = sbi_sse_complete(regs, ਬਾਹਰਲਾ);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_SSE_INJECT:
		ret = sbi_sse_inject_from_ecall(regs->a0, regs->a1, ਬਾਹਰਲਾ);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_SSE_HART_ਨਕਾਬ:
		ret = sbi_sse_hart_ਨਕਾਬ();
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_SSE_HART_UNMASK:
		ret = sbi_sse_hart_unmask();
		ਰੋਕੋ;
	ਮੂਲ:
		ret = SBI_ENOTSUPP;
	}
	ਵਾਪਸ ret;
}

ਬਣਤਰ sbi_ecall_extension ecall_sse;

ਸਥਿਰ ਪੂਰਨ sbi_ecall_sse_ਰਜਿਸਟਰ_extensions(ਖਾਲੀ)
{
	ਵਾਪਸ sbi_ecall_ਰਜਿਸਟਰ_extension(&ecall_sse);
}

ਬਣਤਰ sbi_ecall_extension ecall_sse = {
	.name			= "sse",
	.extid_ਸ਼ੁਰੂ		= SBI_EXT_SSE,
	.extid_ਸਿਰਾ		= SBI_EXT_SSE,
	.ਰਜਿਸਟਰ_extensions	= sbi_ecall_sse_ਰਜਿਸਟਰ_extensions,
	.handle			= sbi_ecall_sse_handler,
};
