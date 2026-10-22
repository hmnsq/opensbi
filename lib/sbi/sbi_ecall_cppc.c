/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_cppc.h>

ਸਥਿਰ ਪੂਰਨ sbi_ecall_cppc_handler(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ extid, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ funcid,
				  ਬਣਤਰ sbi_trap_regs *regs,
				  ਬਣਤਰ sbi_ecall_return *ਬਾਹਰਲਾ)
{
	ਪੂਰਨ ret = 0;
	uint64_t ਆਰਜ਼ੀ;

	ਚੋਣ (funcid) {
	ਹਾਲਤ SBI_EXT_CPPC_READ:
		ret = sbi_cppc_read(regs->a0, &ਆਰਜ਼ੀ);
		ਬਾਹਰਲਾ->ਮੁੱਲ = ਆਰਜ਼ੀ;
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_CPPC_READ_HI:
#ਜੇ __riscv_xlen == 32
		ret = sbi_cppc_read(regs->a0, &ਆਰਜ਼ੀ);
		ਬਾਹਰਲਾ->ਮੁੱਲ = ਆਰਜ਼ੀ >> 32;
#ਨਹੀਂਤਾਂ
		ਬਾਹਰਲਾ->ਮੁੱਲ = 0;
#ਜੇ_ਅੰਤ
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_CPPC_WRITE:
#ਜੇ __riscv_xlen == 32
		ਆਰਜ਼ੀ = regs->a2;
		ਆਰਜ਼ੀ = (ਆਰਜ਼ੀ << 32) | regs->a1;
#ਨਹੀਂਤਾਂ
		ਆਰਜ਼ੀ = regs->a1;
#ਜੇ_ਅੰਤ
		ret = sbi_cppc_write(regs->a0, ਆਰਜ਼ੀ);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_CPPC_PROBE:
		ret = sbi_cppc_probe(regs->a0);
		ਜੇ (ret >= 0) {
			ਬਾਹਰਲਾ->ਮੁੱਲ = ret;
			ret = 0;
		}
		ਰੋਕੋ;
	ਮੂਲ:
		ret = SBI_ENOTSUPP;
	}

	ਵਾਪਸ ret;
}

ਬਣਤਰ sbi_ecall_extension ecall_cppc;

ਸਥਿਰ ਪੂਰਨ sbi_ecall_cppc_ਰਜਿਸਟਰ_extensions(ਖਾਲੀ)
{
	ਜੇ (!sbi_cppc_get_device())
		ਵਾਪਸ 0;

	ਵਾਪਸ sbi_ecall_ਰਜਿਸਟਰ_extension(&ecall_cppc);
}

ਬਣਤਰ sbi_ecall_extension ecall_cppc = {
	.name			= "cppc",
	.extid_ਸ਼ੁਰੂ		= SBI_EXT_CPPC,
	.extid_ਸਿਰਾ		= SBI_EXT_CPPC,
	.ਰਜਿਸਟਰ_extensions	= sbi_ecall_cppc_ਰਜਿਸਟਰ_extensions,
	.handle			= sbi_ecall_cppc_handler,
};
