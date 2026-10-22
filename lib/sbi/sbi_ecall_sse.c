#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_ecall.h>
#অন্তর্ভাব <sbi/sbi_trap.h>
#অন্তর্ভাব <sbi/sbi_sse.h>

স্থির পূর্ণাঙ্ক sbi_ecall_sse_handler(unsigned long extid, unsigned long funcid,
				  সংরচনা sbi_trap_regs *regs,
				  সংরচনা sbi_ecall_return *out)
{
	পূর্ণাঙ্ক ret;

	বিকল্প (funcid) {
	পক্ষ SBI_EXT_SSE_READ_ATTR:
		ret = sbi_sse_read_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		ভঙ্গ;
	পক্ষ SBI_EXT_SSE_WRITE_ATTR:
		ret = sbi_sse_write_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		ভঙ্গ;
	পক্ষ SBI_EXT_SSE_REGISTER:
		ret = sbi_sse_register(regs->a0, regs->a1, regs->a2);
		ভঙ্গ;
	পক্ষ SBI_EXT_SSE_UNREGISTER:
		ret = sbi_sse_unregister(regs->a0);
		ভঙ্গ;
	পক্ষ SBI_EXT_SSE_ENABLE:
		ret = sbi_sse_enable(regs->a0);
		ভঙ্গ;
	পক্ষ SBI_EXT_SSE_DISABLE:
		ret = sbi_sse_disable(regs->a0);
		ভঙ্গ;
	পক্ষ SBI_EXT_SSE_COMPLETE:
		ret = sbi_sse_complete(regs, out);
		ভঙ্গ;
	পক্ষ SBI_EXT_SSE_INJECT:
		ret = sbi_sse_inject_from_ecall(regs->a0, regs->a1, out);
		ভঙ্গ;
	পক্ষ SBI_EXT_SSE_HART_MASK:
		ret = sbi_sse_hart_mask();
		ভঙ্গ;
	পক্ষ SBI_EXT_SSE_HART_UNMASK:
		ret = sbi_sse_hart_unmask();
		ভঙ্গ;
	স্বভাব:
		ret = SBI_ENOTSUPP;
	}
	প্রত্যাগম ret;
}

সংরচনা sbi_ecall_extension ecall_sse;

স্থির পূর্ণাঙ্ক sbi_ecall_sse_register_extensions(শূন্য)
{
	প্রত্যাগম sbi_ecall_register_extension(&ecall_sse);
}

সংরচনা sbi_ecall_extension ecall_sse = {
	.name			= "sse",
	.extid_start		= SBI_EXT_SSE,
	.extid_end		= SBI_EXT_SSE,
	.register_extensions	= sbi_ecall_sse_register_extensions,
	.handle			= sbi_ecall_sse_handler,
};
