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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_mpxy_handler(unsigned long extid, unsigned long funcid,
				  ಸಂರಚನಾ sbi_trap_regs *regs,
				  ಸಂರಚನಾ sbi_ecall_return *out)
{
	ಪೂರ್ಣಾಙ್ಕ ret = 0;

	ವಿಕಲ್ಪ (funcid) {
	ಪಕ್ಷ SBI_EXT_MPXY_GET_SHMEM_SIZE:
		out->ಮೌಲ್ಯ = sbi_mpxy_get_shmem_size();
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_MPXY_SET_SHMEM:
		ret = sbi_mpxy_set_shmem(regs->a0, regs->a1, regs->a2);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_MPXY_GET_CHANNEL_IDS:
		ret = sbi_mpxy_get_channel_ids(regs->a0);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_MPXY_READ_ATTRS:
		ret = sbi_mpxy_read_attrs(regs->a0, regs->a1, regs->a2);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_MPXY_WRITE_ATTRS:
		ret = sbi_mpxy_write_attrs(regs->a0, regs->a1, regs->a2);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_MPXY_SEND_ಸಂದೇಶ_WITH_RESP:
		ret = sbi_mpxy_send_message(regs->a0, regs->a1,
					    regs->a2, &out->ಮೌಲ್ಯ);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_MPXY_SEND_ಸಂದೇಶ_WITHOUT_RESP:
		ret = sbi_mpxy_send_message(regs->a0, regs->a1, regs->a2,
					    ರಿಕ್ತ);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_MPXY_GET_NOTIFICATION_EVENTS:
		ret = sbi_mpxy_get_notification_events(regs->a0, &out->ಮೌಲ್ಯ);
		ಭಙ್ಗ;
	ಸ್ವಭಾವ:
		ret = SBI_ENOTSUPP;
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಸಂರಚನಾ sbi_ecall_extension ecall_mpxy;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_mpxy_register_extensions(ಶೂನ್ಯ)
{
	ಯದಿ (!sbi_mpxy_channel_available())
		ಪ್ರತ್ಯಾಗಮ 0;

	ಪ್ರತ್ಯಾಗಮ sbi_ecall_register_extension(&ecall_mpxy);
}

ಸಂರಚನಾ sbi_ecall_extension ecall_mpxy = {
	.name			= "mpxy",
	.extid_start		= SBI_EXT_MPXY,
	.extid_end		= SBI_EXT_MPXY,
	.register_extensions	= sbi_ecall_mpxy_register_extensions,
	.handle			= sbi_ecall_mpxy_handler,
};
