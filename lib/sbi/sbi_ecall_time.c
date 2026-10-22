/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_timer.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_ಸಮಯ_handler(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ extid, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ funcid,
				  ರಚನೆ sbi_trap_regs *regs,
				  ರಚನೆ sbi_ecall_return *ಹೊರಹೋಗು)
{
	ಪೂರ್ಣಾಂಕ ret = 0;

	ಆದರೆ (funcid == SBI_EXT_ಸಮಯ_ಗಣ_TIMER) {
#ಆದರೆ __riscv_xlen == 32
		sbi_timer_smode_event_ಆರಂಭ((((u64)regs->a1 << 32) | (u64)regs->a0));
#ಇಲ್ಲದಿದ್ದರೆ
		sbi_timer_smode_event_ಆರಂಭ((u64)regs->a0);
#ಆದರೆ_ಅಂತ್ಯ
	} ಇಲ್ಲದಿದ್ದರೆ
		ret = SBI_ENOTSUPP;

	ಹಿಂತಿರುಗಿ ret;
}

ರಚನೆ sbi_ecall_extension ecall_ಸಮಯ;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_ಸಮಯ_ರಿಜಿಸ್ಟರ್_extensions(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ sbi_ecall_ರಿಜಿಸ್ಟರ್_extension(&ecall_ಸಮಯ);
}

ರಚನೆ sbi_ecall_extension ecall_ಸಮಯ = {
	.name			= "time",
	.extid_ಆರಂಭ		= SBI_EXT_ಸಮಯ,
	.extid_ಕೊನೆ		= SBI_EXT_ಸಮಯ,
	.ರಿಜಿಸ್ಟರ್_extensions	= sbi_ecall_ಸಮಯ_ರಿಜಿಸ್ಟರ್_extensions,
	.handle			= sbi_ecall_ಸಮಯ_handler,
};
