/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_mpxy.h>

ਸਥਿਰ ਪੂਰਨ sbi_ecall_mpxy_handler(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ extid, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ funcid,
				  ਬਣਤਰ sbi_trap_regs *regs,
				  ਬਣਤਰ sbi_ecall_return *ਬਾਹਰਲਾ)
{
	ਪੂਰਨ ret = 0;

	ਚੋਣ (funcid) {
	ਹਾਲਤ SBI_EXT_MPXY_GET_SHMEM_ਮਾਪ:
		ਬਾਹਰਲਾ->ਮੁੱਲ = sbi_mpxy_get_shmem_ਮਾਪ();
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_MPXY_ਸੈੱਟ_SHMEM:
		ret = sbi_mpxy_ਸੈੱਟ_shmem(regs->a0, regs->a1, regs->a2);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_MPXY_GET_CHANNEL_IDS:
		ret = sbi_mpxy_get_channel_ids(regs->a0);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_MPXY_READ_ATTRS:
		ret = sbi_mpxy_read_attrs(regs->a0, regs->a1, regs->a2);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_MPXY_WRITE_ATTRS:
		ret = sbi_mpxy_write_attrs(regs->a0, regs->a1, regs->a2);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_MPXY_SEND_ਸੁਨੇਹਾ_WITH_RESP:
		ret = sbi_mpxy_send_ਸੁਨੇਹਾ(regs->a0, regs->a1,
					    regs->a2, &ਬਾਹਰਲਾ->ਮੁੱਲ);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_MPXY_SEND_ਸੁਨੇਹਾ_WITHOUT_RESP:
		ret = sbi_mpxy_send_ਸੁਨੇਹਾ(regs->a0, regs->a1, regs->a2,
					    ਨਿਰਾ);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_MPXY_GET_NOTIFICATION_EVENTS:
		ret = sbi_mpxy_get_notification_events(regs->a0, &ਬਾਹਰਲਾ->ਮੁੱਲ);
		ਰੋਕੋ;
	ਮੂਲ:
		ret = SBI_ENOTSUPP;
	}

	ਵਾਪਸ ret;
}

ਬਣਤਰ sbi_ecall_extension ecall_mpxy;

ਸਥਿਰ ਪੂਰਨ sbi_ecall_mpxy_ਰਜਿਸਟਰ_extensions(ਖਾਲੀ)
{
	ਜੇ (!sbi_mpxy_channel_ਉਪਲਬਧ())
		ਵਾਪਸ 0;

	ਵਾਪਸ sbi_ecall_ਰਜਿਸਟਰ_extension(&ecall_mpxy);
}

ਬਣਤਰ sbi_ecall_extension ecall_mpxy = {
	.name			= "mpxy",
	.extid_ਸ਼ੁਰੂ		= SBI_EXT_MPXY,
	.extid_ਸਿਰਾ		= SBI_EXT_MPXY,
	.ਰਜਿਸਟਰ_extensions	= sbi_ecall_mpxy_ਰਜਿਸਟਰ_extensions,
	.handle			= sbi_ecall_mpxy_handler,
};
