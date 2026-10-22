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
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_system.h>

ਸਥਿਰ ਪੂਰਨ sbi_ecall_srst_handler(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ extid, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ funcid,
				  ਬਣਤਰ sbi_trap_regs *regs,
				  ਬਣਤਰ sbi_ecall_return *ਬਾਹਰਲਾ)
{
	ਜੇ (funcid == SBI_EXT_SRST_RESET) {
		ਜੇ ((((u32)-1U) <= ((u64)regs->a0)) ||
		    (((u32)-1U) <= ((u64)regs->a1)))
			ਵਾਪਸ SBI_EINVAL;

		ਚੋਣ (regs->a0) {
		ਹਾਲਤ SBI_SRST_RESET_TYPE_SHUTDOWN:
		ਹਾਲਤ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		ਹਾਲਤ SBI_SRST_RESET_TYPE_WARM_REBOOT:
			ਰੋਕੋ;
		ਮੂਲ:
			ਵਾਪਸ SBI_EINVAL;
		}

		ਚੋਣ (regs->a1) {
		ਹਾਲਤ SBI_SRST_RESET_REASON_NONE:
		ਹਾਲਤ SBI_SRST_RESET_REASON_SYSFAIL:
			ਰੋਕੋ;
		ਮੂਲ:
			ਵਾਪਸ SBI_EINVAL;
		}

		ਜੇ (sbi_system_reset_supported(regs->a0, regs->a1))
			sbi_system_reset(regs->a0, regs->a1);
	}

	ਵਾਪਸ SBI_ENOTSUPP;
}

ਸਥਿਰ bool srst_ਉਪਲਬਧ(ਖਾਲੀ)
{
	u32 type;

	/*
	 * At least one standard reset types should be supported by
	 * the platform for SBI SRST extension to be usable.
	 */
	ਲਈ (type = 0; type <= SBI_SRST_RESET_TYPE_ਆਖਰੀ; type++) {
		ਜੇ (sbi_system_reset_supported(type,
					SBI_SRST_RESET_REASON_NONE))
			ਵਾਪਸ true;
	}

	ਵਾਪਸ false;
}

ਬਣਤਰ sbi_ecall_extension ecall_srst;

ਸਥਿਰ ਪੂਰਨ sbi_ecall_srst_ਰਜਿਸਟਰ_extensions(ਖਾਲੀ)
{
	ਜੇ (!srst_ਉਪਲਬਧ())
		ਵਾਪਸ 0;

	ਵਾਪਸ sbi_ecall_ਰਜਿਸਟਰ_extension(&ecall_srst);
}

ਬਣਤਰ sbi_ecall_extension ecall_srst = {
	.name			= "srst",
	.extid_ਸ਼ੁਰੂ		= SBI_EXT_SRST,
	.extid_ਸਿਰਾ		= SBI_EXT_SRST,
	.ਰਜਿਸਟਰ_extensions	= sbi_ecall_srst_ਰਜਿਸਟਰ_extensions,
	.handle			= sbi_ecall_srst_handler,
};
