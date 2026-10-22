#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_ecall.h>
#شامل <sbi/sbi_trap.h>
#شامل <sbi/sbi_sse.h>

ساکن صحیح sbi_ecall_sse_handler(غیرعلامتی طویل extid, غیرعلامتی طویل funcid,
				  ساخت sbi_trap_regs *regs,
				  ساخت sbi_ecall_return *برآمد)
{
	صحیح ret;

	انتخاب (funcid) {
	حالت SBI_EXT_SSE_READ_ATTR:
		ret = sbi_sse_read_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		توڑو;
	حالت SBI_EXT_SSE_WRITE_ATTR:
		ret = sbi_sse_write_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		توڑو;
	حالت SBI_EXT_SSE_رجسٹر:
		ret = sbi_sse_رجسٹر(regs->a0, regs->a1, regs->a2);
		توڑو;
	حالت SBI_EXT_SSE_UNREGISTER:
		ret = sbi_sse_unregister(regs->a0);
		توڑو;
	حالت SBI_EXT_SSE_ENABLE:
		ret = sbi_sse_enable(regs->a0);
		توڑو;
	حالت SBI_EXT_SSE_DISABLE:
		ret = sbi_sse_disable(regs->a0);
		توڑو;
	حالت SBI_EXT_SSE_COMPLETE:
		ret = sbi_sse_complete(regs, برآمد);
		توڑو;
	حالت SBI_EXT_SSE_INJECT:
		ret = sbi_sse_inject_from_ecall(regs->a0, regs->a1, برآمد);
		توڑو;
	حالت SBI_EXT_SSE_HART_نقاب:
		ret = sbi_sse_hart_نقاب();
		توڑو;
	حالت SBI_EXT_SSE_HART_UNMASK:
		ret = sbi_sse_hart_unmask();
		توڑو;
	طےشدہ:
		ret = SBI_ENOTSUPP;
	}
	واپس ret;
}

ساخت sbi_ecall_extension ecall_sse;

ساکن صحیح sbi_ecall_sse_رجسٹر_extensions(خالی)
{
	واپس sbi_ecall_رجسٹر_extension(&ecall_sse);
}

ساخت sbi_ecall_extension ecall_sse = {
	.name			= "sse",
	.extid_آغاز		= SBI_EXT_SSE,
	.extid_انتہا		= SBI_EXT_SSE,
	.رجسٹر_extensions	= sbi_ecall_sse_رجسٹر_extensions,
	.handle			= sbi_ecall_sse_handler,
};
