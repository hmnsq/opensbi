#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_ecall.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/sbi_sse.h>

ساكن صحيح sbi_ecall_sse_handler(بینښه اوږد extid, بینښه اوږد funcid,
				  بنية sbi_trap_regs *regs,
				  بنية sbi_ecall_return *بهر)
{
	صحيح ret;

	بدل (funcid) {
	حالة SBI_EXT_SSE_READ_ATTR:
		ret = sbi_sse_read_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		اكسر;
	حالة SBI_EXT_SSE_WRITE_ATTR:
		ret = sbi_sse_write_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		اكسر;
	حالة SBI_EXT_SSE_ثبتوونکی:
		ret = sbi_sse_ثبتوونکی(regs->a0, regs->a1, regs->a2);
		اكسر;
	حالة SBI_EXT_SSE_UNREGISTER:
		ret = sbi_sse_unregister(regs->a0);
		اكسر;
	حالة SBI_EXT_SSE_ENABLE:
		ret = sbi_sse_enable(regs->a0);
		اكسر;
	حالة SBI_EXT_SSE_DISABLE:
		ret = sbi_sse_disable(regs->a0);
		اكسر;
	حالة SBI_EXT_SSE_COMPLETE:
		ret = sbi_sse_complete(regs, بهر);
		اكسر;
	حالة SBI_EXT_SSE_INJECT:
		ret = sbi_sse_inject_from_ecall(regs->a0, regs->a1, بهر);
		اكسر;
	حالة SBI_EXT_SSE_HART_ماسک:
		ret = sbi_sse_hart_ماسک();
		اكسر;
	حالة SBI_EXT_SSE_HART_UNMASK:
		ret = sbi_sse_hart_unmask();
		اكسر;
	افتراضي:
		ret = SBI_ENOTSUPP;
	}
	أرجع ret;
}

بنية sbi_ecall_extension ecall_sse;

ساكن صحيح sbi_ecall_sse_ثبتوونکی_extensions(فراغ)
{
	أرجع sbi_ecall_ثبتوونکی_extension(&ecall_sse);
}

بنية sbi_ecall_extension ecall_sse = {
	.name			= "sse",
	.extid_پیل		= SBI_EXT_SSE,
	.extid_پای_ټکی		= SBI_EXT_SSE,
	.ثبتوونکی_extensions	= sbi_ecall_sse_ثبتوونکی_extensions,
	.handle			= sbi_ecall_sse_handler,
};
