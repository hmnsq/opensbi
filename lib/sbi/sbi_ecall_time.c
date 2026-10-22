/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_timer.h>

ਸਥਿਰ ਪੂਰਨ sbi_ecall_ਸਮਾਂ_handler(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ extid, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ funcid,
				  ਬਣਤਰ sbi_trap_regs *regs,
				  ਬਣਤਰ sbi_ecall_return *ਬਾਹਰਲਾ)
{
	ਪੂਰਨ ret = 0;

	ਜੇ (funcid == SBI_EXT_ਸਮਾਂ_ਸੈੱਟ_TIMER) {
#ਜੇ __riscv_xlen == 32
		sbi_timer_smode_event_ਸ਼ੁਰੂ((((u64)regs->a1 << 32) | (u64)regs->a0));
#ਨਹੀਂਤਾਂ
		sbi_timer_smode_event_ਸ਼ੁਰੂ((u64)regs->a0);
#ਜੇ_ਅੰਤ
	} ਨਹੀਂਤਾਂ
		ret = SBI_ENOTSUPP;

	ਵਾਪਸ ret;
}

ਬਣਤਰ sbi_ecall_extension ecall_ਸਮਾਂ;

ਸਥਿਰ ਪੂਰਨ sbi_ecall_ਸਮਾਂ_ਰਜਿਸਟਰ_extensions(ਖਾਲੀ)
{
	ਵਾਪਸ sbi_ecall_ਰਜਿਸਟਰ_extension(&ecall_ਸਮਾਂ);
}

ਬਣਤਰ sbi_ecall_extension ecall_ਸਮਾਂ = {
	.name			= "time",
	.extid_ਸ਼ੁਰੂ		= SBI_EXT_ਸਮਾਂ,
	.extid_ਸਿਰਾ		= SBI_EXT_ਸਮਾਂ,
	.ਰਜਿਸਟਰ_extensions	= sbi_ecall_ਸਮਾਂ_ਰਜਿਸਟਰ_extensions,
	.handle			= sbi_ecall_ਸਮਾਂ_handler,
};
