/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#அந்தர்பாவ <sbi/sbi_ecall.h>
#அந்தர்பாவ <sbi/sbi_ecall_interface.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_trap.h>
#அந்தர்பாவ <sbi/sbi_mpxy.h>

நிலையான முழு sbi_ecall_mpxy_handler(unsigned long extid, unsigned long funcid,
				  கட்டமைப்பு sbi_trap_regs *regs,
				  கட்டமைப்பு sbi_ecall_return *out)
{
	முழு ret = 0;

	தேர்வு (funcid) {
	நிலை SBI_EXT_MPXY_GET_SHMEM_SIZE:
		out->மதிப்பு = sbi_mpxy_get_shmem_size();
		நிறுத்து;
	நிலை SBI_EXT_MPXY_SET_SHMEM:
		ret = sbi_mpxy_set_shmem(regs->a0, regs->a1, regs->a2);
		நிறுத்து;
	நிலை SBI_EXT_MPXY_GET_CHANNEL_IDS:
		ret = sbi_mpxy_get_channel_ids(regs->a0);
		நிறுத்து;
	நிலை SBI_EXT_MPXY_READ_ATTRS:
		ret = sbi_mpxy_read_attrs(regs->a0, regs->a1, regs->a2);
		நிறுத்து;
	நிலை SBI_EXT_MPXY_WRITE_ATTRS:
		ret = sbi_mpxy_write_attrs(regs->a0, regs->a1, regs->a2);
		நிறுத்து;
	நிலை SBI_EXT_MPXY_SEND_செய்தி_WITH_RESP:
		ret = sbi_mpxy_send_message(regs->a0, regs->a1,
					    regs->a2, &out->மதிப்பு);
		நிறுத்து;
	நிலை SBI_EXT_MPXY_SEND_செய்தி_WITHOUT_RESP:
		ret = sbi_mpxy_send_message(regs->a0, regs->a1, regs->a2,
					    வெற்று);
		நிறுத்து;
	நிலை SBI_EXT_MPXY_GET_NOTIFICATION_EVENTS:
		ret = sbi_mpxy_get_notification_events(regs->a0, &out->மதிப்பு);
		நிறுத்து;
	இயல்பு:
		ret = SBI_ENOTSUPP;
	}

	திரும்பு ret;
}

கட்டமைப்பு sbi_ecall_extension ecall_mpxy;

நிலையான முழு sbi_ecall_mpxy_register_extensions(வெறுமை)
{
	எனில் (!sbi_mpxy_channel_available())
		திரும்பு 0;

	திரும்பு sbi_ecall_register_extension(&ecall_mpxy);
}

கட்டமைப்பு sbi_ecall_extension ecall_mpxy = {
	.name			= "mpxy",
	.extid_start		= SBI_EXT_MPXY,
	.extid_end		= SBI_EXT_MPXY,
	.register_extensions	= sbi_ecall_mpxy_register_extensions,
	.handle			= sbi_ecall_mpxy_handler,
};
