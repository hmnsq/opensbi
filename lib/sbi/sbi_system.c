/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_bitops.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hsm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_platform.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_system.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ipi.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_init.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_timer.h>

ସ୍ଥିର SBI_LIST_HEAD(reset_devices_list);

ଅଚଳ ସଂରଚନା sbi_system_reset_device *sbi_system_reset_get_device(
					u32 reset_type, u32 reset_reason)
{
	ସଂରଚନା sbi_system_reset_device *reset_dev = ଖାଲି;
	ସଂରଚନା sbi_dlist *pos;
	/** lowest priority - any non zero is our candidate */
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା priority = 0;

	/* Check each reset device registered for supported reset type */
	sbi_list_for_each(pos, &(reset_devices_list)) {
		ସଂରଚନା sbi_system_reset_device *dev =
			to_system_reset_device(pos);
		ଯଦି (dev->system_reset_check) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା status = dev->system_reset_check(reset_type,
							     reset_reason);
			/** reset_type not supported */
			ଯଦି (status == 0)
				ଜାରି;

			ଯଦି (status > priority) {
				reset_dev = dev;
				priority = status;
			}
		}
	}

	ଫେରନ୍ତୁ reset_dev;
}

ଶୂନ୍ୟ sbi_system_reset_add_device(ସଂରଚନା sbi_system_reset_device *dev)
{
	ଯଦି (!dev || !dev->system_reset_check)
		ଫେରନ୍ତୁ;

	sbi_list_add(&(dev->ଗଣ୍ଠି), &(reset_devices_list));
}

bool sbi_system_reset_supported(u32 reset_type, u32 reset_reason)
{
	ଫେରନ୍ତୁ !!sbi_system_reset_get_device(reset_type, reset_reason);
}

ଶୂନ୍ୟ __noreturn sbi_system_reset(u32 reset_type, u32 reset_reason)
{
	ସଂରଚନା sbi_domain *dom = sbi_domain_thishart_ptr();
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/* Send HALT IPI to every hart other than the current hart */
	sbi_ipi_send_halt(0, -1UL);

	/* Stop current HART */
	sbi_hsm_hart_stop(scratch, false);

	/* Platform specific reset if domain allowed system reset */
	ଯଦି (dom->system_reset_allowed) {
		ଅଚଳ ସଂରଚନା sbi_system_reset_device *dev =
			sbi_system_reset_get_device(reset_type, reset_reason);
		ଯଦି (dev)
			dev->system_reset(reset_type, reset_reason);
	}

	/* If platform specific reset did not work then do sbi_exit() */
	sbi_exit(scratch);
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା sbi_system_suspend_device *suspend_dev = ଖାଲି;
ସ୍ଥିର bool system_suspended;

ଅଚଳ ସଂରଚନା sbi_system_suspend_device *sbi_system_suspend_get_device(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ suspend_dev;
}

ଶୂନ୍ୟ sbi_system_suspend_set_device(ସଂରଚନା sbi_system_suspend_device *dev)
{
	ଯଦି (!dev || suspend_dev)
		ଫେରନ୍ତୁ;

	suspend_dev = dev;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_system_suspend_test_check(u32 sleep_type)
{
	ଫେରନ୍ତୁ sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? 0 : SBI_EINVAL;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_system_suspend_test_suspend(u32 sleep_type,
					   unsigned long mmode_resume_addr)
{
	ଯଦି (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		ଫେରନ୍ତୁ SBI_EINVAL;

	sbi_timer_mdelay(5000);

	/* Wait for interrupt */
	wfi();

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ସଂରଚନା sbi_system_suspend_device sbi_system_suspend_test = {
	.name = "system-suspend-test",
	.system_suspend_check = sbi_system_suspend_test_check,
	.system_suspend = sbi_system_suspend_test_suspend,
};

ଶୂନ୍ୟ sbi_system_suspend_test_enable(ଶୂନ୍ୟ)
{
	sbi_system_suspend_set_device(&sbi_system_suspend_test);
}

bool sbi_system_suspend_supported(u32 sleep_type)
{
	ଫେରନ୍ତୁ suspend_dev && suspend_dev->system_suspend_check &&
	       suspend_dev->system_suspend_check(sleep_type) == 0;
}

bool sbi_system_is_suspended(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ system_suspended;
}

ଶୂନ୍ୟ sbi_system_resume(ଶୂନ୍ୟ)
{
	ଯଦି (suspend_dev && suspend_dev->system_resume)
		suspend_dev->system_resume();

	system_suspended = false;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_system_suspend(u32 sleep_type, ulong resume_addr, ulong opaque)
{
	ସଂରଚନା sbi_domain *dom = sbi_domain_thishart_ptr();
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ଶୂନ୍ୟ (*jump_warmboot)(ଶୂନ୍ୟ) = (ଶୂନ୍ୟ (*)(ଶୂନ୍ୟ))scratch->warmboot_addr;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା hartindex = current_hartindex();
	unsigned long prev_mode;
	unsigned long i;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

	ଯଦି (!dom || !dom->system_suspend_allowed)
		ଫେରନ୍ତୁ SBI_EFAIL;

	ଯଦି (!suspend_dev || !suspend_dev->system_suspend ||
	    !suspend_dev->system_suspend_check)
		ଫେରନ୍ତୁ SBI_EFAIL;

	ret = suspend_dev->system_suspend_check(sleep_type);
	ଯଦି (ret != SBI_OK)
		ଫେରନ୍ତୁ ret;

	prev_mode = sbi_mstatus_prev_mode(csr_read(CSR_MSTATUS));
	ଯଦି (prev_mode != PRV_S && prev_mode != PRV_U)
		ଫେରନ୍ତୁ SBI_EFAIL;

	spin_lock(&dom->assigned_harts_lock);
	sbi_hartmask_for_each_hartindex(i, &dom->assigned_harts) {
		ଯଦି (i == hartindex)
			ଜାରି;
		ଯଦି (__sbi_hsm_hart_get_state(i) != SBI_HSM_STATE_STOPPED) {
			spin_unlock(&dom->assigned_harts_lock);
			ଫେରନ୍ତୁ SBI_ERR_DENIED;
		}
	}
	spin_unlock(&dom->assigned_harts_lock);

	ଯଦି (!sbi_domain_check_addr(dom, resume_addr, prev_mode,
				   SBI_DOMAIN_EXECUTE))
		ଫେରନ୍ତୁ SBI_EINVALID_ADDR;

	ଯଦି (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_STARTED,
				       SBI_HSM_STATE_SUSPENDED))
		ଫେରନ୍ତୁ SBI_EFAIL;

	/* Prepare for resume */
	scratch->next_mode = prev_mode;
	scratch->next_addr = resume_addr;
	scratch->next_arg1 = opaque;

	__sbi_hsm_suspend_non_ret_save(scratch);

	/* Suspend */
	system_suspended = true;
	ret = suspend_dev->system_suspend(sleep_type, scratch->warmboot_addr);
	ଯଦି (ret != SBI_OK) {
		ଯଦି (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_SUSPENDED,
					       SBI_HSM_STATE_STARTED))
			sbi_hart_hang();

		system_suspended = false;
		ଫେରନ୍ତୁ ret;
	}

	/* Resume */
	jump_warmboot();

	__builtin_unreachable();
}
