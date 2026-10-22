/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/sbi_bitops.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_hsm.h>
#ضمن <sbi/sbi_platform.h>
#ضمن <sbi/sbi_system.h>
#ضمن <sbi/sbi_ipi.h>
#ضمن <sbi/sbi_init.h>
#ضمن <sbi/sbi_timer.h>

ساكن SBI_LIST_HEAD(reset_devices_list);

ثابت بنية sbi_system_reset_device *sbi_system_reset_get_device(
					u32 reset_type, u32 reset_reason)
{
	بنية sbi_system_reset_device *reset_dev = عدم;
	بنية sbi_dlist *pos;
	/** lowest priority - any non zero is our candidate */
	صحيح priority = 0;

	/* Check each reset device registered for supported reset type */
	sbi_list_for_each(pos, &(reset_devices_list)) {
		بنية sbi_system_reset_device *dev =
			to_system_reset_device(pos);
		إذا (dev->system_reset_check) {
			صحيح status = dev->system_reset_check(reset_type,
							     reset_reason);
			/** reset_type not supported */
			إذا (status == 0)
				استمر;

			إذا (status > priority) {
				reset_dev = dev;
				priority = status;
			}
		}
	}

	أرجع reset_dev;
}

فراغ sbi_system_reset_add_device(بنية sbi_system_reset_device *dev)
{
	إذا (!dev || !dev->system_reset_check)
		أرجع;

	sbi_list_add(&(dev->كلي), &(reset_devices_list));
}

bool sbi_system_reset_supported(u32 reset_type, u32 reset_reason)
{
	أرجع !!sbi_system_reset_get_device(reset_type, reset_reason);
}

فراغ __noreturn sbi_system_reset(u32 reset_type, u32 reset_reason)
{
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/* Send HALT IPI to every hart other than the current hart */
	sbi_ipi_send_halt(0, -1UL);

	/* Stop current HART */
	sbi_hsm_hart_stop(scratch, false);

	/* Platform specific reset if domain allowed system reset */
	إذا (dom->system_reset_allowed) {
		ثابت بنية sbi_system_reset_device *dev =
			sbi_system_reset_get_device(reset_type, reset_reason);
		إذا (dev)
			dev->system_reset(reset_type, reset_reason);
	}

	/* If platform specific reset did not work then do sbi_exit() */
	sbi_exit(scratch);
}

ساكن ثابت بنية sbi_system_suspend_device *suspend_dev = عدم;
ساكن bool system_suspended;

ثابت بنية sbi_system_suspend_device *sbi_system_suspend_get_device(فراغ)
{
	أرجع suspend_dev;
}

فراغ sbi_system_suspend_set_device(بنية sbi_system_suspend_device *dev)
{
	إذا (!dev || suspend_dev)
		أرجع;

	suspend_dev = dev;
}

ساكن صحيح sbi_system_suspend_test_check(u32 sleep_type)
{
	أرجع sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? 0 : SBI_EINVAL;
}

ساكن صحيح sbi_system_suspend_test_suspend(u32 sleep_type,
					   unsigned long mmode_resume_addr)
{
	إذا (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		أرجع SBI_EINVAL;

	sbi_timer_mdelay(5000);

	/* Wait for interrupt */
	wfi();

	أرجع SBI_OK;
}

ساكن بنية sbi_system_suspend_device sbi_system_suspend_test = {
	.name = "system-suspend-test",
	.system_suspend_check = sbi_system_suspend_test_check,
	.system_suspend = sbi_system_suspend_test_suspend,
};

فراغ sbi_system_suspend_test_enable(فراغ)
{
	sbi_system_suspend_set_device(&sbi_system_suspend_test);
}

bool sbi_system_suspend_supported(u32 sleep_type)
{
	أرجع suspend_dev && suspend_dev->system_suspend_check &&
	       suspend_dev->system_suspend_check(sleep_type) == 0;
}

bool sbi_system_is_suspended(فراغ)
{
	أرجع system_suspended;
}

فراغ sbi_system_resume(فراغ)
{
	إذا (suspend_dev && suspend_dev->system_resume)
		suspend_dev->system_resume();

	system_suspended = false;
}

صحيح sbi_system_suspend(u32 sleep_type, ulong resume_addr, ulong opaque)
{
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	فراغ (*jump_warmboot)(فراغ) = (فراغ (*)(فراغ))scratch->warmboot_addr;
	unsigned صحيح hartindex = current_hartindex();
	unsigned long prev_mode;
	unsigned long i;
	صحيح ret;

	إذا (!dom || !dom->system_suspend_allowed)
		أرجع SBI_EFAIL;

	إذا (!suspend_dev || !suspend_dev->system_suspend ||
	    !suspend_dev->system_suspend_check)
		أرجع SBI_EFAIL;

	ret = suspend_dev->system_suspend_check(sleep_type);
	إذا (ret != SBI_OK)
		أرجع ret;

	prev_mode = sbi_mstatus_prev_mode(csr_read(CSR_MSTATUS));
	إذا (prev_mode != PRV_S && prev_mode != PRV_U)
		أرجع SBI_EFAIL;

	spin_lock(&dom->assigned_harts_lock);
	sbi_hartmask_for_each_hartindex(i, &dom->assigned_harts) {
		إذا (i == hartindex)
			استمر;
		إذا (__sbi_hsm_hart_get_state(i) != SBI_HSM_STATE_STOPPED) {
			spin_unlock(&dom->assigned_harts_lock);
			أرجع SBI_ERR_DENIED;
		}
	}
	spin_unlock(&dom->assigned_harts_lock);

	إذا (!sbi_domain_check_addr(dom, resume_addr, prev_mode,
				   SBI_DOMAIN_EXECUTE))
		أرجع SBI_EINVALID_ADDR;

	إذا (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_STARTED,
				       SBI_HSM_STATE_SUSPENDED))
		أرجع SBI_EFAIL;

	/* Prepare for resume */
	scratch->next_mode = prev_mode;
	scratch->next_addr = resume_addr;
	scratch->next_arg1 = opaque;

	__sbi_hsm_suspend_non_ret_save(scratch);

	/* Suspend */
	system_suspended = true;
	ret = suspend_dev->system_suspend(sleep_type, scratch->warmboot_addr);
	إذا (ret != SBI_OK) {
		إذا (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_SUSPENDED,
					       SBI_HSM_STATE_STARTED))
			sbi_hart_hang();

		system_suspended = false;
		أرجع ret;
	}

	/* Resume */
	jump_warmboot();

	__builtin_unreachable();
}
