/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#अन्तर्भाव <sbi/sbi_ecall.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_trap.h>
#अन्तर्भाव <sbi/sbi_mpxy.h>

स्थिर पूर्णाङ्क sbi_ecall_mpxy_handler(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ funcid,
				  संरचना sbi_trap_regs *regs,
				  संरचना sbi_ecall_return *out)
{
	पूर्णाङ्क ret = 0;

	विकल्प (funcid) {
	पक्ष SBI_EXT_MPXY_GET_SHMEM_SIZE:
		out->मूल्य = sbi_mpxy_get_shmem_size();
		भङ्ग;
	पक्ष SBI_EXT_MPXY_SET_SHMEM:
		ret = sbi_mpxy_set_shmem(regs->a0, regs->a1, regs->a2);
		भङ्ग;
	पक्ष SBI_EXT_MPXY_GET_CHANNEL_IDS:
		ret = sbi_mpxy_get_channel_ids(regs->a0);
		भङ्ग;
	पक्ष SBI_EXT_MPXY_READ_ATTRS:
		ret = sbi_mpxy_read_attrs(regs->a0, regs->a1, regs->a2);
		भङ्ग;
	पक्ष SBI_EXT_MPXY_WRITE_ATTRS:
		ret = sbi_mpxy_write_attrs(regs->a0, regs->a1, regs->a2);
		भङ्ग;
	पक्ष SBI_EXT_MPXY_SEND_संदेश_WITH_RESP:
		ret = sbi_mpxy_send_message(regs->a0, regs->a1,
					    regs->a2, &out->मूल्य);
		भङ्ग;
	पक्ष SBI_EXT_MPXY_SEND_संदेश_WITHOUT_RESP:
		ret = sbi_mpxy_send_message(regs->a0, regs->a1, regs->a2,
					    रिक्त);
		भङ्ग;
	पक्ष SBI_EXT_MPXY_GET_NOTIFICATION_EVENTS:
		ret = sbi_mpxy_get_notification_events(regs->a0, &out->मूल्य);
		भङ्ग;
	स्वभाव:
		ret = SBI_ENOTSUPP;
	}

	प्रत्यागम ret;
}

संरचना sbi_ecall_extension ecall_mpxy;

स्थिर पूर्णाङ्क sbi_ecall_mpxy_register_extensions(शून्य)
{
	यदि (!sbi_mpxy_channel_available())
		प्रत्यागम 0;

	प्रत्यागम sbi_ecall_register_extension(&ecall_mpxy);
}

संरचना sbi_ecall_extension ecall_mpxy = {
	.name			= "mpxy",
	.extid_start		= SBI_EXT_MPXY,
	.extid_end		= SBI_EXT_MPXY,
	.register_extensions	= sbi_ecall_mpxy_register_extensions,
	.handle			= sbi_ecall_mpxy_handler,
};
