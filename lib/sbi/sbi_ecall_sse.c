#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_sse.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_sse_handler(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ extid, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ funcid,
				  ರಚನೆ sbi_trap_regs *regs,
				  ರಚನೆ sbi_ecall_return *ಹೊರಹೋಗು)
{
	ಪೂರ್ಣಾಂಕ ret;

	ಆಯ್ಕೆ (funcid) {
	ಸ್ಥಿತಿ SBI_EXT_SSE_READ_ATTR:
		ret = sbi_sse_read_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_SSE_WRITE_ATTR:
		ret = sbi_sse_write_attrs(regs->a0, regs->a1, regs->a2,
					 regs->a3, regs->a4);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_SSE_ರಿಜಿಸ್ಟರ್:
		ret = sbi_sse_ರಿಜಿಸ್ಟರ್(regs->a0, regs->a1, regs->a2);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_SSE_UNREGISTER:
		ret = sbi_sse_unregister(regs->a0);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_SSE_ENABLE:
		ret = sbi_sse_enable(regs->a0);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_SSE_DISABLE:
		ret = sbi_sse_disable(regs->a0);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_SSE_COMPLETE:
		ret = sbi_sse_complete(regs, ಹೊರಹೋಗು);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_SSE_INJECT:
		ret = sbi_sse_inject_from_ecall(regs->a0, regs->a1, ಹೊರಹೋಗು);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_SSE_HART_ಮುಖವಾಡ:
		ret = sbi_sse_hart_ಮುಖವಾಡ();
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_SSE_HART_UNMASK:
		ret = sbi_sse_hart_unmask();
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ret = SBI_ENOTSUPP;
	}
	ಹಿಂತಿರುಗಿ ret;
}

ರಚನೆ sbi_ecall_extension ecall_sse;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_sse_ರಿಜಿಸ್ಟರ್_extensions(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ sbi_ecall_ರಿಜಿಸ್ಟರ್_extension(&ecall_sse);
}

ರಚನೆ sbi_ecall_extension ecall_sse = {
	.name			= "sse",
	.extid_ಆರಂಭ		= SBI_EXT_SSE,
	.extid_ಕೊನೆ		= SBI_EXT_SSE,
	.ರಿಜಿಸ್ಟರ್_extensions	= sbi_ecall_sse_ರಿಜಿಸ್ಟರ್_extensions,
	.handle			= sbi_ecall_sse_handler,
};
