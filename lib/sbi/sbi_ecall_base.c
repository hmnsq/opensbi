/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_version.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>

ਸਥਿਰ ਪੂਰਨ sbi_ecall_ਅਧਾਰ_probe(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ extid, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਬਾਹਰਲਾ_val)
{
	ਬਣਤਰ sbi_ecall_extension *ext;

	ext = sbi_ecall_ਲੱਭੋ_extension(extid);
	ਜੇ (!ext) {
		*ਬਾਹਰਲਾ_val = 0;
		ਵਾਪਸ 0;
	}

	ਜੇ (ext->probe)
		ਵਾਪਸ ext->probe(extid, ਬਾਹਰਲਾ_val);

	*ਬਾਹਰਲਾ_val = 1;
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ sbi_ecall_ਅਧਾਰ_handler(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ extid, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ funcid,
				  ਬਣਤਰ sbi_trap_regs *regs,
				  ਬਣਤਰ sbi_ecall_return *ਬਾਹਰਲਾ)
{
	ਪੂਰਨ ret = 0;

	ਚੋਣ (funcid) {
	ਹਾਲਤ SBI_EXT_ਅਧਾਰ_GET_SPEC_ਸੰਸਕਰਣ:
		ਬਾਹਰਲਾ->ਮੁੱਲ = (SBI_ECALL_ਸੰਸਕਰਣ_MAJOR <<
			      SBI_SPEC_ਸੰਸਕਰਣ_MAJOR_ਵਿੱਥ) &
			     (SBI_SPEC_ਸੰਸਕਰਣ_MAJOR_ਨਕਾਬ <<
			      SBI_SPEC_ਸੰਸਕਰਣ_MAJOR_ਵਿੱਥ);
		ਬਾਹਰਲਾ->ਮੁੱਲ = ਬਾਹਰਲਾ->ਮੁੱਲ | SBI_ECALL_ਸੰਸਕਰਣ_MINOR;
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_ਅਧਾਰ_GET_IMP_ID:
		ਬਾਹਰਲਾ->ਮੁੱਲ = sbi_ecall_get_impid();
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_ਅਧਾਰ_GET_IMP_ਸੰਸਕਰਣ:
		ਬਾਹਰਲਾ->ਮੁੱਲ = OPENSBI_ਸੰਸਕਰਣ;
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_ਅਧਾਰ_GET_MVENDORID:
		ਬਾਹਰਲਾ->ਮੁੱਲ = csr_read(CSR_MVENDORID);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_ਅਧਾਰ_GET_MARCHID:
		ਬਾਹਰਲਾ->ਮੁੱਲ = csr_read(CSR_MARCHID);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_ਅਧਾਰ_GET_MIMPID:
		ਬਾਹਰਲਾ->ਮੁੱਲ = csr_read(CSR_MIMPID);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_ਅਧਾਰ_PROBE_EXT:
		ret = sbi_ecall_ਅਧਾਰ_probe(regs->a0, &ਬਾਹਰਲਾ->ਮੁੱਲ);
		ਰੋਕੋ;
	ਮੂਲ:
		ret = SBI_ENOTSUPP;
	}

	ਵਾਪਸ ret;
}

ਬਣਤਰ sbi_ecall_extension ecall_ਅਧਾਰ;

ਸਥਿਰ ਪੂਰਨ sbi_ecall_ਅਧਾਰ_ਰਜਿਸਟਰ_extensions(ਖਾਲੀ)
{
	ਵਾਪਸ sbi_ecall_ਰਜਿਸਟਰ_extension(&ecall_ਅਧਾਰ);
}

ਬਣਤਰ sbi_ecall_extension ecall_ਅਧਾਰ = {
	.name			= "base",
	.extid_ਸ਼ੁਰੂ		= SBI_EXT_ਅਧਾਰ,
	.extid_ਸਿਰਾ		= SBI_EXT_ਅਧਾਰ,
	.ਰਜਿਸਟਰ_extensions	= sbi_ecall_ਅਧਾਰ_ਰਜਿਸਟਰ_extensions,
	.handle			= sbi_ecall_ਅਧਾਰ_handler,
};
