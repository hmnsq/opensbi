#include <sbi/sbi_error.h>
#include <sbi/sbi_ecall.h>
#include <sbi/sbi_trap.h>
#include <sbi/sbi_sse.h>

static puur_naa_nka sbi_ecall_sse_handler(unsigned long extid, unsigned long funcid,
				  sa_nracanaa sbi_trap_regs *regs,
				  sa_nracanaa sbi_ecall_return *out)
{
	puur_naa_nka ret;

	switch (funcid) {
	pak_sa SBI_EXT_SSE_READ_ATTR:
		ret = sbi_sse_read_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		bha_nga;
	pak_sa SBI_EXT_SSE_WRITE_ATTR:
		ret = sbi_sse_write_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		bha_nga;
	pak_sa SBI_EXT_SSE_REGISTER:
		ret = sbi_sse_register(regs->a0, regs->a1, regs->a2);
		bha_nga;
	pak_sa SBI_EXT_SSE_UNREGISTER:
		ret = sbi_sse_unregister(regs->a0);
		bha_nga;
	pak_sa SBI_EXT_SSE_ENABLE:
		ret = sbi_sse_enable(regs->a0);
		bha_nga;
	pak_sa SBI_EXT_SSE_DISABLE:
		ret = sbi_sse_disable(regs->a0);
		bha_nga;
	pak_sa SBI_EXT_SSE_COMPLETE:
		ret = sbi_sse_complete(regs, out);
		bha_nga;
	pak_sa SBI_EXT_SSE_INJECT:
		ret = sbi_sse_inject_from_ecall(regs->a0, regs->a1, out);
		bha_nga;
	pak_sa SBI_EXT_SSE_HART_MASK:
		ret = sbi_sse_hart_mask();
		bha_nga;
	pak_sa SBI_EXT_SSE_HART_UNMASK:
		ret = sbi_sse_hart_unmask();
		bha_nga;
	default:
		ret = SBI_ENOTSUPP;
	}
	ᨀᨘᨒ ret;
}

sa_nracanaa sbi_ecall_extension ecall_sse;

static puur_naa_nka sbi_ecall_sse_register_extensions(void)
{
	ᨀᨘᨒ sbi_ecall_register_extension(&ecall_sse);
}

sa_nracanaa sbi_ecall_extension ecall_sse = {
	.name			= "sse",
	.extid_start		= SBI_EXT_SSE,
	.extid_end		= SBI_EXT_SSE,
	.register_extensions	= sbi_ecall_sse_register_extensions,
	.handle			= sbi_ecall_sse_handler,
};
