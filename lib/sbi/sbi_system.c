/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/sbi_bitops.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_hsm.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_system.h>
#شامل <sbi/sbi_ipi.h>
#شامل <sbi/sbi_init.h>
#شامل <sbi/sbi_timer.h>

ایستا SBI_LIST_HEAD(reset_devices_list);

ثابت ساختار sbi_system_reset_device *sbi_system_reset_get_device(
					u32 reset_type, u32 reset_reason)
{
	ساختار sbi_system_reset_device *reset_dev = پوچ;
	ساختار sbi_dlist *pos;
	/** lowest priority - any non zero is our candidate */
	صحیح priority = 0;

	/* Check each reset device registered for supported reset type */
	sbi_list_for_each(pos, &(reset_devices_list)) {
		ساختار sbi_system_reset_device *dev =
			to_system_reset_device(pos);
		اگر (dev->system_reset_check) {
			صحیح status = dev->system_reset_check(reset_type,
							     reset_reason);
			/** reset_type not supported */
			اگر (status == 0)
				ادامه;

			اگر (status > priority) {
				reset_dev = dev;
				priority = status;
			}
		}
	}

	بازگشت reset_dev;
}

تهی sbi_system_reset_add_device(ساختار sbi_system_reset_device *dev)
{
	اگر (!dev || !dev->system_reset_check)
		بازگشت;

	sbi_list_add(&(dev->گره), &(reset_devices_list));
}

bool sbi_system_reset_supported(u32 reset_type, u32 reset_reason)
{
	بازگشت !!sbi_system_reset_get_device(reset_type, reset_reason);
}

تهی __noreturn sbi_system_reset(u32 reset_type, u32 reset_reason)
{
	ساختار sbi_domain *dom = sbi_domain_thishart_ptr();
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/* Send HALT IPI to every hart other than the current hart */
	sbi_ipi_send_halt(0, -1UL);

	/* Stop current HART */
	sbi_hsm_hart_stop(scratch, false);

	/* Platform specific reset if domain allowed system reset */
	اگر (dom->system_reset_allowed) {
		ثابت ساختار sbi_system_reset_device *dev =
			sbi_system_reset_get_device(reset_type, reset_reason);
		اگر (dev)
			dev->system_reset(reset_type, reset_reason);
	}

	/* If platform specific reset did not work then do sbi_exit() */
	sbi_exit(scratch);
}

ایستا ثابت ساختار sbi_system_suspend_device *suspend_dev = پوچ;
ایستا bool system_suspended;

ثابت ساختار sbi_system_suspend_device *sbi_system_suspend_get_device(تهی)
{
	بازگشت suspend_dev;
}

تهی sbi_system_suspend_set_device(ساختار sbi_system_suspend_device *dev)
{
	اگر (!dev || suspend_dev)
		بازگشت;

	suspend_dev = dev;
}

ایستا صحیح sbi_system_suspend_test_check(u32 sleep_type)
{
	بازگشت sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? 0 : SBI_EINVAL;
}

ایستا صحیح sbi_system_suspend_test_suspend(u32 sleep_type,
					   unsigned long mmode_resume_addr)
{
	اگر (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		بازگشت SBI_EINVAL;

	sbi_timer_mdelay(5000);

	/* Wait for interrupt */
	wfi();

	بازگشت SBI_OK;
}

ایستا ساختار sbi_system_suspend_device sbi_system_suspend_test = {
	.name = "system-suspend-test",
	.system_suspend_check = sbi_system_suspend_test_check,
	.system_suspend = sbi_system_suspend_test_suspend,
};

تهی sbi_system_suspend_test_enable(تهی)
{
	sbi_system_suspend_set_device(&sbi_system_suspend_test);
}

bool sbi_system_suspend_supported(u32 sleep_type)
{
	بازگشت suspend_dev && suspend_dev->system_suspend_check &&
	       suspend_dev->system_suspend_check(sleep_type) == 0;
}

bool sbi_system_is_suspended(تهی)
{
	بازگشت system_suspended;
}

تهی sbi_system_resume(تهی)
{
	اگر (suspend_dev && suspend_dev->system_resume)
		suspend_dev->system_resume();

	system_suspended = false;
}

صحیح sbi_system_suspend(u32 sleep_type, ulong resume_addr, ulong opaque)
{
	ساختار sbi_domain *dom = sbi_domain_thishart_ptr();
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	تهی (*jump_warmboot)(تهی) = (تهی (*)(تهی))scratch->warmboot_addr;
	unsigned صحیح hartindex = current_hartindex();
	unsigned long prev_mode;
	unsigned long i;
	صحیح ret;

	اگر (!dom || !dom->system_suspend_allowed)
		بازگشت SBI_EFAIL;

	اگر (!suspend_dev || !suspend_dev->system_suspend ||
	    !suspend_dev->system_suspend_check)
		بازگشت SBI_EFAIL;

	ret = suspend_dev->system_suspend_check(sleep_type);
	اگر (ret != SBI_OK)
		بازگشت ret;

	prev_mode = sbi_mstatus_prev_mode(csr_read(CSR_MSTATUS));
	اگر (prev_mode != PRV_S && prev_mode != PRV_U)
		بازگشت SBI_EFAIL;

	spin_lock(&dom->assigned_harts_lock);
	sbi_hartmask_for_each_hartindex(i, &dom->assigned_harts) {
		اگر (i == hartindex)
			ادامه;
		اگر (__sbi_hsm_hart_get_state(i) != SBI_HSM_STATE_STOPPED) {
			spin_unlock(&dom->assigned_harts_lock);
			بازگشت SBI_ERR_DENIED;
		}
	}
	spin_unlock(&dom->assigned_harts_lock);

	اگر (!sbi_domain_check_addr(dom, resume_addr, prev_mode,
				   SBI_DOMAIN_EXECUTE))
		بازگشت SBI_EINVALID_ADDR;

	اگر (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_STARTED,
				       SBI_HSM_STATE_SUSPENDED))
		بازگشت SBI_EFAIL;

	/* Prepare for resume */
	scratch->next_mode = prev_mode;
	scratch->next_addr = resume_addr;
	scratch->next_arg1 = opaque;

	__sbi_hsm_suspend_non_ret_save(scratch);

	/* Suspend */
	system_suspended = true;
	ret = suspend_dev->system_suspend(sleep_type, scratch->warmboot_addr);
	اگر (ret != SBI_OK) {
		اگر (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_SUSPENDED,
					       SBI_HSM_STATE_STARTED))
			sbi_hart_hang();

		system_suspended = false;
		بازگشت ret;
	}

	/* Resume */
	jump_warmboot();

	__builtin_unreachable();
}
