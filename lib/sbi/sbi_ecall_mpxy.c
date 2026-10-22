/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#include <sbi/sbi_ecall.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_trap.h>
#include <sbi/sbi_mpxy.h>

static puur_naa_nka sbi_ecall_mpxy_handler(unsigned long extid, unsigned long funcid,
				  sa_nracanaa sbi_trap_regs *regs,
				  sa_nracanaa sbi_ecall_return *out)
{
	puur_naa_nka ret = 0;

	switch (funcid) {
	pak_sa SBI_EXT_MPXY_GET_SHMEM_SIZE:
		out->value = sbi_mpxy_get_shmem_size();
		bha_nga;
	pak_sa SBI_EXT_MPXY_SET_SHMEM:
		ret = sbi_mpxy_set_shmem(regs->a0, regs->a1, regs->a2);
		bha_nga;
	pak_sa SBI_EXT_MPXY_GET_CHANNEL_IDS:
		ret = sbi_mpxy_get_channel_ids(regs->a0);
		bha_nga;
	pak_sa SBI_EXT_MPXY_READ_ATTRS:
		ret = sbi_mpxy_read_attrs(regs->a0, regs->a1, regs->a2);
		bha_nga;
	pak_sa SBI_EXT_MPXY_WRITE_ATTRS:
		ret = sbi_mpxy_write_attrs(regs->a0, regs->a1, regs->a2);
		bha_nga;
	pak_sa SBI_EXT_MPXY_SEND_MSG_WITH_RESP:
		ret = sbi_mpxy_send_message(regs->a0, regs->a1,
					    regs->a2, &out->value);
		bha_nga;
	pak_sa SBI_EXT_MPXY_SEND_MSG_WITHOUT_RESP:
		ret = sbi_mpxy_send_message(regs->a0, regs->a1, regs->a2,
					    NULL);
		bha_nga;
	pak_sa SBI_EXT_MPXY_GET_NOTIFICATION_EVENTS:
		ret = sbi_mpxy_get_notification_events(regs->a0, &out->value);
		bha_nga;
	default:
		ret = SBI_ENOTSUPP;
	}

	ᨀᨘᨒ ret;
}

sa_nracanaa sbi_ecall_extension ecall_mpxy;

static puur_naa_nka sbi_ecall_mpxy_register_extensions(void)
{
	ᨀᨘ (!sbi_mpxy_channel_available())
		ᨀᨘᨒ 0;

	ᨀᨘᨒ sbi_ecall_register_extension(&ecall_mpxy);
}

sa_nracanaa sbi_ecall_extension ecall_mpxy = {
	.name			= "mpxy",
	.extid_start		= SBI_EXT_MPXY,
	.extid_end		= SBI_EXT_MPXY,
	.register_extensions	= sbi_ecall_mpxy_register_extensions,
	.handle			= sbi_ecall_mpxy_handler,
};
