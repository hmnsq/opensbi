#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_ecall.h>
#அந்தர்பாவ <sbi/sbi_trap.h>
#அந்தர்பாவ <sbi/sbi_sse.h>

நிலையான முழு sbi_ecall_sse_handler(unsigned long extid, unsigned long funcid,
				  கட்டமைப்பு sbi_trap_regs *regs,
				  கட்டமைப்பு sbi_ecall_return *out)
{
	முழு ret;

	தேர்வு (funcid) {
	நிலை SBI_EXT_SSE_READ_ATTR:
		ret = sbi_sse_read_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		நிறுத்து;
	நிலை SBI_EXT_SSE_WRITE_ATTR:
		ret = sbi_sse_write_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		நிறுத்து;
	நிலை SBI_EXT_SSE_REGISTER:
		ret = sbi_sse_register(regs->a0, regs->a1, regs->a2);
		நிறுத்து;
	நிலை SBI_EXT_SSE_UNREGISTER:
		ret = sbi_sse_unregister(regs->a0);
		நிறுத்து;
	நிலை SBI_EXT_SSE_ENABLE:
		ret = sbi_sse_enable(regs->a0);
		நிறுத்து;
	நிலை SBI_EXT_SSE_DISABLE:
		ret = sbi_sse_disable(regs->a0);
		நிறுத்து;
	நிலை SBI_EXT_SSE_COMPLETE:
		ret = sbi_sse_complete(regs, out);
		நிறுத்து;
	நிலை SBI_EXT_SSE_INJECT:
		ret = sbi_sse_inject_from_ecall(regs->a0, regs->a1, out);
		நிறுத்து;
	நிலை SBI_EXT_SSE_HART_MASK:
		ret = sbi_sse_hart_mask();
		நிறுத்து;
	நிலை SBI_EXT_SSE_HART_UNMASK:
		ret = sbi_sse_hart_unmask();
		நிறுத்து;
	இயல்பு:
		ret = SBI_ENOTSUPP;
	}
	திரும்பு ret;
}

கட்டமைப்பு sbi_ecall_extension ecall_sse;

நிலையான முழு sbi_ecall_sse_register_extensions(வெறுமை)
{
	திரும்பு sbi_ecall_register_extension(&ecall_sse);
}

கட்டமைப்பு sbi_ecall_extension ecall_sse = {
	.name			= "sse",
	.extid_start		= SBI_EXT_SSE,
	.extid_end		= SBI_EXT_SSE,
	.register_extensions	= sbi_ecall_sse_register_extensions,
	.handle			= sbi_ecall_sse_handler,
};
