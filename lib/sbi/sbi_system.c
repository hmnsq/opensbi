/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/sbi_bitops.h>
#அந்தர்பாவ <sbi/sbi_domain.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_hsm.h>
#அந்தர்பாவ <sbi/sbi_platform.h>
#அந்தர்பாவ <sbi/sbi_system.h>
#அந்தர்பாவ <sbi/sbi_ipi.h>
#அந்தர்பாவ <sbi/sbi_init.h>
#அந்தர்பாவ <sbi/sbi_timer.h>

நிலையான SBI_LIST_HEAD(reset_devices_list);

மாறிலி கட்டமைப்பு sbi_system_reset_device *sbi_system_reset_get_device(
					u32 reset_type, u32 reset_reason)
{
	கட்டமைப்பு sbi_system_reset_device *reset_dev = வெற்று;
	கட்டமைப்பு sbi_dlist *pos;
	/** lowest priority - any non zero is our candidate */
	முழு priority = 0;

	/* Check each reset device registered for supported reset type */
	sbi_list_for_each(pos, &(reset_devices_list)) {
		கட்டமைப்பு sbi_system_reset_device *dev =
			to_system_reset_device(pos);
		எனில் (dev->system_reset_check) {
			முழு status = dev->system_reset_check(reset_type,
							     reset_reason);
			/** reset_type not supported */
			எனில் (status == 0)
				தொடர்;

			எனில் (status > priority) {
				reset_dev = dev;
				priority = status;
			}
		}
	}

	திரும்பு reset_dev;
}

வெறுமை sbi_system_reset_add_device(கட்டமைப்பு sbi_system_reset_device *dev)
{
	எனில் (!dev || !dev->system_reset_check)
		திரும்பு;

	sbi_list_add(&(dev->முனை), &(reset_devices_list));
}

bool sbi_system_reset_supported(u32 reset_type, u32 reset_reason)
{
	திரும்பு !!sbi_system_reset_get_device(reset_type, reset_reason);
}

வெறுமை __noreturn sbi_system_reset(u32 reset_type, u32 reset_reason)
{
	கட்டமைப்பு sbi_domain *dom = sbi_domain_thishart_ptr();
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/* Send HALT IPI to every hart other than the current hart */
	sbi_ipi_send_halt(0, -1UL);

	/* Stop current HART */
	sbi_hsm_hart_stop(scratch, false);

	/* Platform specific reset if domain allowed system reset */
	எனில் (dom->system_reset_allowed) {
		மாறிலி கட்டமைப்பு sbi_system_reset_device *dev =
			sbi_system_reset_get_device(reset_type, reset_reason);
		எனில் (dev)
			dev->system_reset(reset_type, reset_reason);
	}

	/* If platform specific reset did not work then do sbi_exit() */
	sbi_exit(scratch);
}

நிலையான மாறிலி கட்டமைப்பு sbi_system_suspend_device *suspend_dev = வெற்று;
நிலையான bool system_suspended;

மாறிலி கட்டமைப்பு sbi_system_suspend_device *sbi_system_suspend_get_device(வெறுமை)
{
	திரும்பு suspend_dev;
}

வெறுமை sbi_system_suspend_set_device(கட்டமைப்பு sbi_system_suspend_device *dev)
{
	எனில் (!dev || suspend_dev)
		திரும்பு;

	suspend_dev = dev;
}

நிலையான முழு sbi_system_suspend_test_check(u32 sleep_type)
{
	திரும்பு sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? 0 : SBI_EINVAL;
}

நிலையான முழு sbi_system_suspend_test_suspend(u32 sleep_type,
					   unsigned long mmode_resume_addr)
{
	எனில் (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		திரும்பு SBI_EINVAL;

	sbi_timer_mdelay(5000);

	/* Wait for interrupt */
	wfi();

	திரும்பு SBI_OK;
}

நிலையான கட்டமைப்பு sbi_system_suspend_device sbi_system_suspend_test = {
	.name = "system-suspend-test",
	.system_suspend_check = sbi_system_suspend_test_check,
	.system_suspend = sbi_system_suspend_test_suspend,
};

வெறுமை sbi_system_suspend_test_enable(வெறுமை)
{
	sbi_system_suspend_set_device(&sbi_system_suspend_test);
}

bool sbi_system_suspend_supported(u32 sleep_type)
{
	திரும்பு suspend_dev && suspend_dev->system_suspend_check &&
	       suspend_dev->system_suspend_check(sleep_type) == 0;
}

bool sbi_system_is_suspended(வெறுமை)
{
	திரும்பு system_suspended;
}

வெறுமை sbi_system_resume(வெறுமை)
{
	எனில் (suspend_dev && suspend_dev->system_resume)
		suspend_dev->system_resume();

	system_suspended = false;
}

முழு sbi_system_suspend(u32 sleep_type, ulong resume_addr, ulong opaque)
{
	கட்டமைப்பு sbi_domain *dom = sbi_domain_thishart_ptr();
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	வெறுமை (*jump_warmboot)(வெறுமை) = (வெறுமை (*)(வெறுமை))scratch->warmboot_addr;
	unsigned முழு hartindex = current_hartindex();
	unsigned long prev_mode;
	unsigned long i;
	முழு ret;

	எனில் (!dom || !dom->system_suspend_allowed)
		திரும்பு SBI_EFAIL;

	எனில் (!suspend_dev || !suspend_dev->system_suspend ||
	    !suspend_dev->system_suspend_check)
		திரும்பு SBI_EFAIL;

	ret = suspend_dev->system_suspend_check(sleep_type);
	எனில் (ret != SBI_OK)
		திரும்பு ret;

	prev_mode = sbi_mstatus_prev_mode(csr_read(CSR_MSTATUS));
	எனில் (prev_mode != PRV_S && prev_mode != PRV_U)
		திரும்பு SBI_EFAIL;

	spin_lock(&dom->assigned_harts_lock);
	sbi_hartmask_for_each_hartindex(i, &dom->assigned_harts) {
		எனில் (i == hartindex)
			தொடர்;
		எனில் (__sbi_hsm_hart_get_state(i) != SBI_HSM_STATE_STOPPED) {
			spin_unlock(&dom->assigned_harts_lock);
			திரும்பு SBI_ERR_DENIED;
		}
	}
	spin_unlock(&dom->assigned_harts_lock);

	எனில் (!sbi_domain_check_addr(dom, resume_addr, prev_mode,
				   SBI_DOMAIN_EXECUTE))
		திரும்பு SBI_EINVALID_ADDR;

	எனில் (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_STARTED,
				       SBI_HSM_STATE_SUSPENDED))
		திரும்பு SBI_EFAIL;

	/* Prepare for resume */
	scratch->next_mode = prev_mode;
	scratch->next_addr = resume_addr;
	scratch->next_arg1 = opaque;

	__sbi_hsm_suspend_non_ret_save(scratch);

	/* Suspend */
	system_suspended = true;
	ret = suspend_dev->system_suspend(sleep_type, scratch->warmboot_addr);
	எனில் (ret != SBI_OK) {
		எனில் (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_SUSPENDED,
					       SBI_HSM_STATE_STARTED))
			sbi_hart_hang();

		system_suspended = false;
		திரும்பு ret;
	}

	/* Resume */
	jump_warmboot();

	__builtin_unreachable();
}
