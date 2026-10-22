/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#समावेश <sbi/sbi_ecall.h>
#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_trap.h>
#समावेश <sbi/sbi_mpxy.h>

अटल पूर्णांक sbi_ecall_mpxy_handler(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ funcid,
				  काष्ठा sbi_trap_regs *regs,
				  काष्ठा sbi_ecall_return *out)
{
	पूर्णांक ret = 0;

	चयन (funcid) {
	हाल SBI_EXT_MPXY_GET_SHMEM_आकार:
		out->मूल्य = sbi_mpxy_get_shmem_आकार();
		अवरोध;
	हाल SBI_EXT_MPXY_SET_SHMEM:
		ret = sbi_mpxy_set_shmem(regs->a0, regs->a1, regs->a2);
		अवरोध;
	हाल SBI_EXT_MPXY_GET_CHANNEL_IDS:
		ret = sbi_mpxy_get_channel_ids(regs->a0);
		अवरोध;
	हाल SBI_EXT_MPXY_READ_ATTRS:
		ret = sbi_mpxy_read_attrs(regs->a0, regs->a1, regs->a2);
		अवरोध;
	हाल SBI_EXT_MPXY_WRITE_ATTRS:
		ret = sbi_mpxy_write_attrs(regs->a0, regs->a1, regs->a2);
		अवरोध;
	हाल SBI_EXT_MPXY_SEND_संदेश_WITH_RESP:
		ret = sbi_mpxy_send_message(regs->a0, regs->a1,
					    regs->a2, &out->मूल्य);
		अवरोध;
	हाल SBI_EXT_MPXY_SEND_संदेश_WITHOUT_RESP:
		ret = sbi_mpxy_send_message(regs->a0, regs->a1, regs->a2,
					    शून्य);
		अवरोध;
	हाल SBI_EXT_MPXY_GET_NOTIFICATION_EVENTS:
		ret = sbi_mpxy_get_notification_events(regs->a0, &out->मूल्य);
		अवरोध;
	शेष:
		ret = SBI_ENOTSUPP;
	}

	वापस ret;
}

काष्ठा sbi_ecall_extension ecall_mpxy;

अटल पूर्णांक sbi_ecall_mpxy_register_extensions(व्योम)
{
	अगर (!sbi_mpxy_channel_available())
		वापस 0;

	वापस sbi_ecall_register_extension(&ecall_mpxy);
}

काष्ठा sbi_ecall_extension ecall_mpxy = {
	.name			= "mpxy",
	.extid_start		= SBI_EXT_MPXY,
	.extid_end		= SBI_EXT_MPXY,
	.register_extensions	= sbi_ecall_mpxy_register_extensions,
	.handle			= sbi_ecall_mpxy_handler,
};
