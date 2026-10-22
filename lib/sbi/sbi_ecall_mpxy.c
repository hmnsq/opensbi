/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall_interface.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_trap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_mpxy.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_mpxy_handler(unsigned long extid, unsigned long funcid,
				  ସଂରଚନା sbi_trap_regs *regs,
				  ସଂରଚନା sbi_ecall_return *out)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 0;

	ବାଛ (funcid) {
	ସ୍ଥିତି SBI_EXT_MPXY_GET_SHMEM_SIZE:
		out->ମୂଲ୍ୟ = sbi_mpxy_get_shmem_size();
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_MPXY_SET_SHMEM:
		ret = sbi_mpxy_set_shmem(regs->a0, regs->a1, regs->a2);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_MPXY_GET_CHANNEL_IDS:
		ret = sbi_mpxy_get_channel_ids(regs->a0);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_MPXY_READ_ATTRS:
		ret = sbi_mpxy_read_attrs(regs->a0, regs->a1, regs->a2);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_MPXY_WRITE_ATTRS:
		ret = sbi_mpxy_write_attrs(regs->a0, regs->a1, regs->a2);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_MPXY_SEND_ସନ୍ଦେଶ_WITH_RESP:
		ret = sbi_mpxy_send_message(regs->a0, regs->a1,
					    regs->a2, &out->ମୂଲ୍ୟ);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_MPXY_SEND_ସନ୍ଦେଶ_WITHOUT_RESP:
		ret = sbi_mpxy_send_message(regs->a0, regs->a1, regs->a2,
					    ଖାଲି);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_MPXY_GET_NOTIFICATION_EVENTS:
		ret = sbi_mpxy_get_notification_events(regs->a0, &out->ମୂଲ୍ୟ);
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ret = SBI_ENOTSUPP;
	}

	ଫେରନ୍ତୁ ret;
}

ସଂରଚନା sbi_ecall_extension ecall_mpxy;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_mpxy_register_extensions(ଶୂନ୍ୟ)
{
	ଯଦି (!sbi_mpxy_channel_available())
		ଫେରନ୍ତୁ 0;

	ଫେରନ୍ତୁ sbi_ecall_register_extension(&ecall_mpxy);
}

ସଂରଚନା sbi_ecall_extension ecall_mpxy = {
	.name			= "mpxy",
	.extid_start		= SBI_EXT_MPXY,
	.extid_end		= SBI_EXT_MPXY,
	.register_extensions	= sbi_ecall_mpxy_register_extensions,
	.handle			= sbi_ecall_mpxy_handler,
};
