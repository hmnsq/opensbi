/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/sbi_bitops.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_hsm.h>
#অন্তর্ভাব <sbi/sbi_platform.h>
#অন্তর্ভাব <sbi/sbi_system.h>
#অন্তর্ভাব <sbi/sbi_ipi.h>
#অন্তর্ভাব <sbi/sbi_init.h>
#অন্তর্ভাব <sbi/sbi_timer.h>

স্থির SBI_LIST_HEAD(reset_devices_list);

ধ্রুব সংরচনা sbi_system_reset_device *sbi_system_reset_get_device(
					u32 reset_type, u32 reset_reason)
{
	সংরচনা sbi_system_reset_device *reset_dev = রিক্ত;
	সংরচনা sbi_dlist *pos;
	/** lowest priority - any non zero is our candidate */
	পূর্ণাঙ্ক priority = 0;

	/* Check each reset device registered for supported reset type */
	sbi_list_for_each(pos, &(reset_devices_list)) {
		সংরচনা sbi_system_reset_device *dev =
			to_system_reset_device(pos);
		যদি (dev->system_reset_check) {
			পূর্ণাঙ্ক status = dev->system_reset_check(reset_type,
							     reset_reason);
			/** reset_type not supported */
			যদি (status == 0)
				অনুবর্ত;

			যদি (status > priority) {
				reset_dev = dev;
				priority = status;
			}
		}
	}

	প্রত্যাগম reset_dev;
}

শূন্য sbi_system_reset_add_device(সংরচনা sbi_system_reset_device *dev)
{
	যদি (!dev || !dev->system_reset_check)
		প্রত্যাগম;

	sbi_list_add(&(dev->গ্রন্থি), &(reset_devices_list));
}

bool sbi_system_reset_supported(u32 reset_type, u32 reset_reason)
{
	প্রত্যাগম !!sbi_system_reset_get_device(reset_type, reset_reason);
}

শূন্য __noreturn sbi_system_reset(u32 reset_type, u32 reset_reason)
{
	সংরচনা sbi_domain *dom = sbi_domain_thishart_ptr();
	সংরচনা sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/* Send HALT IPI to every hart other than the current hart */
	sbi_ipi_send_halt(0, -1UL);

	/* Stop current HART */
	sbi_hsm_hart_stop(scratch, false);

	/* Platform specific reset if domain allowed system reset */
	যদি (dom->system_reset_allowed) {
		ধ্রুব সংরচনা sbi_system_reset_device *dev =
			sbi_system_reset_get_device(reset_type, reset_reason);
		যদি (dev)
			dev->system_reset(reset_type, reset_reason);
	}

	/* If platform specific reset did not work then do sbi_exit() */
	sbi_exit(scratch);
}

স্থির ধ্রুব সংরচনা sbi_system_suspend_device *suspend_dev = রিক্ত;
স্থির bool system_suspended;

ধ্রুব সংরচনা sbi_system_suspend_device *sbi_system_suspend_get_device(শূন্য)
{
	প্রত্যাগম suspend_dev;
}

শূন্য sbi_system_suspend_set_device(সংরচনা sbi_system_suspend_device *dev)
{
	যদি (!dev || suspend_dev)
		প্রত্যাগম;

	suspend_dev = dev;
}

স্থির পূর্ণাঙ্ক sbi_system_suspend_test_check(u32 sleep_type)
{
	প্রত্যাগম sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? 0 : SBI_EINVAL;
}

স্থির পূর্ণাঙ্ক sbi_system_suspend_test_suspend(u32 sleep_type,
					   unsigned long mmode_resume_addr)
{
	যদি (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		প্রত্যাগম SBI_EINVAL;

	sbi_timer_mdelay(5000);

	/* Wait for interrupt */
	wfi();

	প্রত্যাগম SBI_OK;
}

স্থির সংরচনা sbi_system_suspend_device sbi_system_suspend_test = {
	.name = "system-suspend-test",
	.system_suspend_check = sbi_system_suspend_test_check,
	.system_suspend = sbi_system_suspend_test_suspend,
};

শূন্য sbi_system_suspend_test_enable(শূন্য)
{
	sbi_system_suspend_set_device(&sbi_system_suspend_test);
}

bool sbi_system_suspend_supported(u32 sleep_type)
{
	প্রত্যাগম suspend_dev && suspend_dev->system_suspend_check &&
	       suspend_dev->system_suspend_check(sleep_type) == 0;
}

bool sbi_system_is_suspended(শূন্য)
{
	প্রত্যাগম system_suspended;
}

শূন্য sbi_system_resume(শূন্য)
{
	যদি (suspend_dev && suspend_dev->system_resume)
		suspend_dev->system_resume();

	system_suspended = false;
}

পূর্ণাঙ্ক sbi_system_suspend(u32 sleep_type, ulong resume_addr, ulong opaque)
{
	সংরচনা sbi_domain *dom = sbi_domain_thishart_ptr();
	সংরচনা sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	শূন্য (*jump_warmboot)(শূন্য) = (শূন্য (*)(শূন্য))scratch->warmboot_addr;
	unsigned পূর্ণাঙ্ক hartindex = current_hartindex();
	unsigned long prev_mode;
	unsigned long i;
	পূর্ণাঙ্ক ret;

	যদি (!dom || !dom->system_suspend_allowed)
		প্রত্যাগম SBI_EFAIL;

	যদি (!suspend_dev || !suspend_dev->system_suspend ||
	    !suspend_dev->system_suspend_check)
		প্রত্যাগম SBI_EFAIL;

	ret = suspend_dev->system_suspend_check(sleep_type);
	যদি (ret != SBI_OK)
		প্রত্যাগম ret;

	prev_mode = sbi_mstatus_prev_mode(csr_read(CSR_MSTATUS));
	যদি (prev_mode != PRV_S && prev_mode != PRV_U)
		প্রত্যাগম SBI_EFAIL;

	spin_lock(&dom->assigned_harts_lock);
	sbi_hartmask_for_each_hartindex(i, &dom->assigned_harts) {
		যদি (i == hartindex)
			অনুবর্ত;
		যদি (__sbi_hsm_hart_get_state(i) != SBI_HSM_STATE_STOPPED) {
			spin_unlock(&dom->assigned_harts_lock);
			প্রত্যাগম SBI_ERR_DENIED;
		}
	}
	spin_unlock(&dom->assigned_harts_lock);

	যদি (!sbi_domain_check_addr(dom, resume_addr, prev_mode,
				   SBI_DOMAIN_EXECUTE))
		প্রত্যাগম SBI_EINVALID_ADDR;

	যদি (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_STARTED,
				       SBI_HSM_STATE_SUSPENDED))
		প্রত্যাগম SBI_EFAIL;

	/* Prepare for resume */
	scratch->next_mode = prev_mode;
	scratch->next_addr = resume_addr;
	scratch->next_arg1 = opaque;

	__sbi_hsm_suspend_non_ret_save(scratch);

	/* Suspend */
	system_suspended = true;
	ret = suspend_dev->system_suspend(sleep_type, scratch->warmboot_addr);
	যদি (ret != SBI_OK) {
		যদি (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_SUSPENDED,
					       SBI_HSM_STATE_STARTED))
			sbi_hart_hang();

		system_suspended = false;
		প্রত্যাগম ret;
	}

	/* Resume */
	jump_warmboot();

	__builtin_unreachable();
}
