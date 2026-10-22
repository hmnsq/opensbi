#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_ecall.h>
#അന്തര്ഭാവ <sbi/sbi_trap.h>
#അന്തര്ഭാവ <sbi/sbi_sse.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_sse_handler(unsigned long extid, unsigned long funcid,
				  ഘടന sbi_trap_regs *regs,
				  ഘടന sbi_ecall_return *out)
{
	പൂർണ്ണസംഖ്യ ret;

	തിരഞ്ഞെടുപ്പ് (funcid) {
	അവസ്ഥ SBI_EXT_SSE_READ_ATTR:
		ret = sbi_sse_read_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_SSE_WRITE_ATTR:
		ret = sbi_sse_write_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_SSE_REGISTER:
		ret = sbi_sse_register(regs->a0, regs->a1, regs->a2);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_SSE_UNREGISTER:
		ret = sbi_sse_unregister(regs->a0);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_SSE_ENABLE:
		ret = sbi_sse_enable(regs->a0);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_SSE_DISABLE:
		ret = sbi_sse_disable(regs->a0);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_SSE_COMPLETE:
		ret = sbi_sse_complete(regs, out);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_SSE_INJECT:
		ret = sbi_sse_inject_from_ecall(regs->a0, regs->a1, out);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_SSE_HART_MASK:
		ret = sbi_sse_hart_mask();
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_SSE_HART_UNMASK:
		ret = sbi_sse_hart_unmask();
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		ret = SBI_ENOTSUPP;
	}
	തിരികെ ret;
}

ഘടന sbi_ecall_extension ecall_sse;

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_sse_register_extensions(ശൂന്യം)
{
	തിരികെ sbi_ecall_register_extension(&ecall_sse);
}

ഘടന sbi_ecall_extension ecall_sse = {
	.name			= "sse",
	.extid_start		= SBI_EXT_SSE,
	.extid_end		= SBI_EXT_SSE,
	.register_extensions	= sbi_ecall_sse_register_extensions,
	.handle			= sbi_ecall_sse_handler,
};
