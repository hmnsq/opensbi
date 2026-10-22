#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_ecall.h>
#অন্তর্ভাব <sbi/sbi_trap.h>
#অন্তর্ভাব <sbi/sbi_sse.h>

স্থির পূর্ণসংখ্যা sbi_ecall_sse_handler(unsigned long extid, unsigned long funcid,
				  কাঠামো sbi_trap_regs *regs,
				  কাঠামো sbi_ecall_return *out)
{
	পূর্ণসংখ্যা ret;

	নির্বাচন (funcid) {
	ক্ষেত্র SBI_EXT_SSE_READ_ATTR:
		ret = sbi_sse_read_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		বিরতি;
	ক্ষেত্র SBI_EXT_SSE_WRITE_ATTR:
		ret = sbi_sse_write_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		বিরতি;
	ক্ষেত্র SBI_EXT_SSE_REGISTER:
		ret = sbi_sse_register(regs->a0, regs->a1, regs->a2);
		বিরতি;
	ক্ষেত্র SBI_EXT_SSE_UNREGISTER:
		ret = sbi_sse_unregister(regs->a0);
		বিরতি;
	ক্ষেত্র SBI_EXT_SSE_ENABLE:
		ret = sbi_sse_enable(regs->a0);
		বিরতি;
	ক্ষেত্র SBI_EXT_SSE_DISABLE:
		ret = sbi_sse_disable(regs->a0);
		বিরতি;
	ক্ষেত্র SBI_EXT_SSE_COMPLETE:
		ret = sbi_sse_complete(regs, out);
		বিরতি;
	ক্ষেত্র SBI_EXT_SSE_INJECT:
		ret = sbi_sse_inject_from_ecall(regs->a0, regs->a1, out);
		বিরতি;
	ক্ষেত্র SBI_EXT_SSE_HART_MASK:
		ret = sbi_sse_hart_mask();
		বিরতি;
	ক্ষেত্র SBI_EXT_SSE_HART_UNMASK:
		ret = sbi_sse_hart_unmask();
		বিরতি;
	পূর্বনির্ধারিত:
		ret = SBI_ENOTSUPP;
	}
	ফেরত ret;
}

কাঠামো sbi_ecall_extension ecall_sse;

স্থির পূর্ণসংখ্যা sbi_ecall_sse_register_extensions(শূন্য)
{
	ফেরত sbi_ecall_register_extension(&ecall_sse);
}

কাঠামো sbi_ecall_extension ecall_sse = {
	.name			= "sse",
	.extid_start		= SBI_EXT_SSE,
	.extid_end		= SBI_EXT_SSE,
	.register_extensions	= sbi_ecall_sse_register_extensions,
	.handle			= sbi_ecall_sse_handler,
};
