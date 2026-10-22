/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall_interface.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_trap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_system.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_srst_handler(unsigned long extid, unsigned long funcid,
				  ସଂରଚନା sbi_trap_regs *regs,
				  ସଂରଚନା sbi_ecall_return *out)
{
	ଯଦି (funcid == SBI_EXT_SRST_RESET) {
		ଯଦି ((((u32)-1U) <= ((u64)regs->a0)) ||
		    (((u32)-1U) <= ((u64)regs->a1)))
			ଫେରନ୍ତୁ SBI_EINVAL;

		ବାଛ (regs->a0) {
		ସ୍ଥିତି SBI_SRST_RESET_TYPE_SHUTDOWN:
		ସ୍ଥିତି SBI_SRST_RESET_TYPE_COLD_REBOOT:
		ସ୍ଥିତି SBI_SRST_RESET_TYPE_WARM_REBOOT:
			ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ଫେରନ୍ତୁ SBI_EINVAL;
		}

		ବାଛ (regs->a1) {
		ସ୍ଥିତି SBI_SRST_RESET_REASON_NONE:
		ସ୍ଥିତି SBI_SRST_RESET_REASON_SYSFAIL:
			ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ଫେରନ୍ତୁ SBI_EINVAL;
		}

		ଯଦି (sbi_system_reset_supported(regs->a0, regs->a1))
			sbi_system_reset(regs->a0, regs->a1);
	}

	ଫେରନ୍ତୁ SBI_ENOTSUPP;
}

ସ୍ଥିର bool srst_available(ଶୂନ୍ୟ)
{
	u32 type;

	/*
	 * At least one standard reset types should be supported by
	 * the platform for SBI SRST extension to be usable.
	 */
	ପାଇଁ (type = 0; type <= SBI_SRST_RESET_TYPE_LAST; type++) {
		ଯଦି (sbi_system_reset_supported(type,
					SBI_SRST_RESET_REASON_NONE))
			ଫେରନ୍ତୁ true;
	}

	ଫେରନ୍ତୁ false;
}

ସଂରଚନା sbi_ecall_extension ecall_srst;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_srst_register_extensions(ଶୂନ୍ୟ)
{
	ଯଦି (!srst_available())
		ଫେରନ୍ତୁ 0;

	ଫେରନ୍ତୁ sbi_ecall_register_extension(&ecall_srst);
}

ସଂରଚନା sbi_ecall_extension ecall_srst = {
	.name			= "srst",
	.extid_start		= SBI_EXT_SRST,
	.extid_end		= SBI_EXT_SRST,
	.register_extensions	= sbi_ecall_srst_register_extensions,
	.handle			= sbi_ecall_srst_handler,
};
