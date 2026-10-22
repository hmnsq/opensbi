#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_sse.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_sse_handler(unsigned long extid, unsigned long funcid,
				  ಸಂರಚನಾ sbi_trap_regs *regs,
				  ಸಂರಚನಾ sbi_ecall_return *out)
{
	ಪೂರ್ಣಾಙ್ಕ ret;

	ವಿಕಲ್ಪ (funcid) {
	ಪಕ್ಷ SBI_EXT_SSE_READ_ATTR:
		ret = sbi_sse_read_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_SSE_WRITE_ATTR:
		ret = sbi_sse_write_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_SSE_REGISTER:
		ret = sbi_sse_register(regs->a0, regs->a1, regs->a2);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_SSE_UNREGISTER:
		ret = sbi_sse_unregister(regs->a0);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_SSE_ENABLE:
		ret = sbi_sse_enable(regs->a0);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_SSE_DISABLE:
		ret = sbi_sse_disable(regs->a0);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_SSE_COMPLETE:
		ret = sbi_sse_complete(regs, out);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_SSE_INJECT:
		ret = sbi_sse_inject_from_ecall(regs->a0, regs->a1, out);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_SSE_HART_MASK:
		ret = sbi_sse_hart_mask();
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_SSE_HART_UNMASK:
		ret = sbi_sse_hart_unmask();
		ಭಙ್ಗ;
	ಸ್ವಭಾವ:
		ret = SBI_ENOTSUPP;
	}
	ಪ್ರತ್ಯಾಗಮ ret;
}

ಸಂರಚನಾ sbi_ecall_extension ecall_sse;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_sse_register_extensions(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ sbi_ecall_register_extension(&ecall_sse);
}

ಸಂರಚನಾ sbi_ecall_extension ecall_sse = {
	.name			= "sse",
	.extid_start		= SBI_EXT_SSE,
	.extid_end		= SBI_EXT_SSE,
	.register_extensions	= sbi_ecall_sse_register_extensions,
	.handle			= sbi_ecall_sse_handler,
};
