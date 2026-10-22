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

ساكن SBI_LIST_سر(reset_devices_list);

ثابت بنية sbi_system_reset_device *sbi_system_reset_get_device(
					u32 reset_type, u32 reset_reason)
{
	بنية sbi_system_reset_device *reset_dev = عدم;
	بنية sbi_dlist *pos;
	/** lowest priority - any non zero is our candidate */
	صحيح ترجیح = 0;

	/* Check each reset device registered for supported reset type */
	sbi_list_for_each(pos, &(reset_devices_list)) {
		بنية sbi_system_reset_device *dev =
			منزل_system_reset_device(pos);
		إذا (dev->system_reset_check) {
			صحيح حالت = dev->system_reset_check(reset_type,
							     reset_reason);
			/** reset_type not supported */
			إذا (حالت == 0)
				استمر;

			إذا (حالت > ترجیح) {
				reset_dev = dev;
				ترجیح = حالت;
			}
		}
	}

	أرجع reset_dev;
}

فراغ sbi_system_reset_جوڑو_device(بنية sbi_system_reset_device *dev)
{
	إذا (!dev || !dev->system_reset_check)
		أرجع;

	sbi_list_جوڑو(&(dev->گرہ), &(reset_devices_list));
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

فراغ sbi_system_suspend_سیٹ_device(بنية sbi_system_suspend_device *dev)
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
					   بنانشانی لما mmode_resume_addr)
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
	sbi_system_suspend_سیٹ_device(&sbi_system_suspend_test);
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
	بنانشانی صحيح hartindex = current_hartindex();
	بنانشانی لما prev_منوال;
	بنانشانی لما i;
	صحيح ret;

	إذا (!dom || !dom->system_suspend_allowed)
		أرجع SBI_EFAIL;

	إذا (!suspend_dev || !suspend_dev->system_suspend ||
	    !suspend_dev->system_suspend_check)
		أرجع SBI_EFAIL;

	ret = suspend_dev->system_suspend_check(sleep_type);
	إذا (ret != SBI_OK)
		أرجع ret;

	prev_منوال = sbi_mstatus_prev_منوال(csr_read(CSR_MSTATUS));
	إذا (prev_منوال != PRV_S && prev_منوال != PRV_U)
		أرجع SBI_EFAIL;

	spin_جندرا(&dom->assigned_harts_جندرا);
	sbi_hartmask_for_each_hartindex(i, &dom->assigned_harts) {
		إذا (i == hartindex)
			استمر;
		إذا (__sbi_hsm_hart_get_state(i) != SBI_HSM_STATE_STOPPED) {
			spin_unlock(&dom->assigned_harts_جندرا);
			أرجع SBI_ERR_DENIED;
		}
	}
	spin_unlock(&dom->assigned_harts_جندرا);

	إذا (!sbi_domain_check_addr(dom, resume_addr, prev_منوال,
				   SBI_DOMAIN_EXECUTE))
		أرجع SBI_EINVALID_ADDR;

	إذا (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_STARTED,
				       SBI_HSM_STATE_SUSPENDED))
		أرجع SBI_EFAIL;

	/* Prepare for resume */
	scratch->اگلا_منوال = prev_منوال;
	scratch->اگلا_addr = resume_addr;
	scratch->اگلا_arg1 = opaque;

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
