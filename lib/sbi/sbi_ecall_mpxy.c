/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_mpxy.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_mpxy_handler(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ extid, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ funcid,
				  ರಚನೆ sbi_trap_regs *regs,
				  ರಚನೆ sbi_ecall_return *ಹೊರಹೋಗು)
{
	ಪೂರ್ಣಾಂಕ ret = 0;

	ಆಯ್ಕೆ (funcid) {
	ಸ್ಥಿತಿ SBI_EXT_MPXY_GET_SHMEM_ಗಾತ್ರ:
		ಹೊರಹೋಗು->ಮೌಲ್ಯ = sbi_mpxy_get_shmem_ಗಾತ್ರ();
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_MPXY_ಗಣ_SHMEM:
		ret = sbi_mpxy_ಗಣ_shmem(regs->a0, regs->a1, regs->a2);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_MPXY_GET_CHANNEL_IDS:
		ret = sbi_mpxy_get_channel_ids(regs->a0);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_MPXY_READ_ATTRS:
		ret = sbi_mpxy_read_attrs(regs->a0, regs->a1, regs->a2);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_MPXY_WRITE_ATTRS:
		ret = sbi_mpxy_write_attrs(regs->a0, regs->a1, regs->a2);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_MPXY_SEND_ಸಂದೇಶ_WITH_RESP:
		ret = sbi_mpxy_send_ಸಂದೇಶ(regs->a0, regs->a1,
					    regs->a2, &ಹೊರಹೋಗು->ಮೌಲ್ಯ);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_MPXY_SEND_ಸಂದೇಶ_WITHOUT_RESP:
		ret = sbi_mpxy_send_ಸಂದೇಶ(regs->a0, regs->a1, regs->a2,
					    ಖಾಲಿ);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_MPXY_GET_NOTIFICATION_EVENTS:
		ret = sbi_mpxy_get_notification_events(regs->a0, &ಹೊರಹೋಗು->ಮೌಲ್ಯ);
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ret = SBI_ENOTSUPP;
	}

	ಹಿಂತಿರುಗಿ ret;
}

ರಚನೆ sbi_ecall_extension ecall_mpxy;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_mpxy_ರಿಜಿಸ್ಟರ್_extensions(ಶೂನ್ಯ)
{
	ಆದರೆ (!sbi_mpxy_channel_ಲಭ್ಯ())
		ಹಿಂತಿರುಗಿ 0;

	ಹಿಂತಿರುಗಿ sbi_ecall_ರಿಜಿಸ್ಟರ್_extension(&ecall_mpxy);
}

ರಚನೆ sbi_ecall_extension ecall_mpxy = {
	.name			= "mpxy",
	.extid_ಆರಂಭ		= SBI_EXT_MPXY,
	.extid_ಕೊನೆ		= SBI_EXT_MPXY,
	.ರಿಜಿಸ್ಟರ್_extensions	= sbi_ecall_mpxy_ರಿಜಿಸ್ಟರ್_extensions,
	.handle			= sbi_ecall_mpxy_handler,
};
