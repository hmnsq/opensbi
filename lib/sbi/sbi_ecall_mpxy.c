/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#അന്തര്ഭാവ <sbi/sbi_ecall.h>
#അന്തര്ഭാവ <sbi/sbi_ecall_interface.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_trap.h>
#അന്തര്ഭാവ <sbi/sbi_mpxy.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_mpxy_handler(unsigned long extid, unsigned long funcid,
				  ഘടന sbi_trap_regs *regs,
				  ഘടന sbi_ecall_return *out)
{
	പൂർണ്ണസംഖ്യ ret = 0;

	തിരഞ്ഞെടുപ്പ് (funcid) {
	അവസ്ഥ SBI_EXT_MPXY_GET_SHMEM_SIZE:
		out->മൂല്യം = sbi_mpxy_get_shmem_size();
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_MPXY_SET_SHMEM:
		ret = sbi_mpxy_set_shmem(regs->a0, regs->a1, regs->a2);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_MPXY_GET_CHANNEL_IDS:
		ret = sbi_mpxy_get_channel_ids(regs->a0);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_MPXY_READ_ATTRS:
		ret = sbi_mpxy_read_attrs(regs->a0, regs->a1, regs->a2);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_MPXY_WRITE_ATTRS:
		ret = sbi_mpxy_write_attrs(regs->a0, regs->a1, regs->a2);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_MPXY_SEND_സന്ദേശം_WITH_RESP:
		ret = sbi_mpxy_send_message(regs->a0, regs->a1,
					    regs->a2, &out->മൂല്യം);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_MPXY_SEND_സന്ദേശം_WITHOUT_RESP:
		ret = sbi_mpxy_send_message(regs->a0, regs->a1, regs->a2,
					    ശൂന്യ);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_MPXY_GET_NOTIFICATION_EVENTS:
		ret = sbi_mpxy_get_notification_events(regs->a0, &out->മൂല്യം);
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		ret = SBI_ENOTSUPP;
	}

	തിരികെ ret;
}

ഘടന sbi_ecall_extension ecall_mpxy;

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_mpxy_register_extensions(ശൂന്യം)
{
	എങ്കിൽ (!sbi_mpxy_channel_available())
		തിരികെ 0;

	തിരികെ sbi_ecall_register_extension(&ecall_mpxy);
}

ഘടന sbi_ecall_extension ecall_mpxy = {
	.name			= "mpxy",
	.extid_start		= SBI_EXT_MPXY,
	.extid_end		= SBI_EXT_MPXY,
	.register_extensions	= sbi_ecall_mpxy_register_extensions,
	.handle			= sbi_ecall_mpxy_handler,
};
