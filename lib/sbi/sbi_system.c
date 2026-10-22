/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/sbi_bitops.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_hsm.h>
#అన్తర్భావ <sbi/sbi_platform.h>
#అన్తర్భావ <sbi/sbi_system.h>
#అన్తర్భావ <sbi/sbi_ipi.h>
#అన్తర్భావ <sbi/sbi_init.h>
#అన్తర్భావ <sbi/sbi_timer.h>

స్థిర SBI_LIST_తల(reset_devices_list);

స్థిరరాశి నిర్మాణం sbi_system_reset_device *sbi_system_reset_get_device(
					u32 reset_type, u32 reset_reason)
{
	నిర్మాణం sbi_system_reset_device *reset_dev = ఖాళీ;
	నిర్మాణం sbi_dlist *pos;
	/** lowest priority - any non zero is our candidate */
	పూర్ణాంకం ప్రాధాన్యత = 0;

	/* Check each reset device registered for supported reset type */
	sbi_list_for_each(pos, &(reset_devices_list)) {
		నిర్మాణం sbi_system_reset_device *dev =
			గమ్యం_system_reset_device(pos);
		ఒకవేళ (dev->system_reset_check) {
			పూర్ణాంకం స్థితి_2 = dev->system_reset_check(reset_type,
							     reset_reason);
			/** reset_type not supported */
			ఒకవేళ (స్థితి_2 == 0)
				కొనసాగు;

			ఒకవేళ (స్థితి_2 > ప్రాధాన్యత) {
				reset_dev = dev;
				ప్రాధాన్యత = స్థితి_2;
			}
		}
	}

	తిరిగి reset_dev;
}

శూన్య sbi_system_reset_కూడు_device(నిర్మాణం sbi_system_reset_device *dev)
{
	ఒకవేళ (!dev || !dev->system_reset_check)
		తిరిగి;

	sbi_list_కూడు(&(dev->కణుపు), &(reset_devices_list));
}

bool sbi_system_reset_supported(u32 reset_type, u32 reset_reason)
{
	తిరిగి !!sbi_system_reset_get_device(reset_type, reset_reason);
}

శూన్య __noreturn sbi_system_reset(u32 reset_type, u32 reset_reason)
{
	నిర్మాణం sbi_domain *dom = sbi_domain_thishart_ptr();
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/* Send HALT IPI to every hart other than the current hart */
	sbi_ipi_send_halt(0, -1UL);

	/* Stop current HART */
	sbi_hsm_hart_stop(scratch, false);

	/* Platform specific reset if domain allowed system reset */
	ఒకవేళ (dom->system_reset_allowed) {
		స్థిరరాశి నిర్మాణం sbi_system_reset_device *dev =
			sbi_system_reset_get_device(reset_type, reset_reason);
		ఒకవేళ (dev)
			dev->system_reset(reset_type, reset_reason);
	}

	/* If platform specific reset did not work then do sbi_exit() */
	sbi_exit(scratch);
}

స్థిర స్థిరరాశి నిర్మాణం sbi_system_suspend_device *suspend_dev = ఖాళీ;
స్థిర bool system_suspended;

స్థిరరాశి నిర్మాణం sbi_system_suspend_device *sbi_system_suspend_get_device(శూన్య)
{
	తిరిగి suspend_dev;
}

శూన్య sbi_system_suspend_సమితి_device(నిర్మాణం sbi_system_suspend_device *dev)
{
	ఒకవేళ (!dev || suspend_dev)
		తిరిగి;

	suspend_dev = dev;
}

స్థిర పూర్ణాంకం sbi_system_suspend_test_check(u32 sleep_type)
{
	తిరిగి sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? 0 : SBI_EINVAL;
}

స్థిర పూర్ణాంకం sbi_system_suspend_test_suspend(u32 sleep_type,
					   గుర్తులేని దీర్ఘ mmode_resume_addr)
{
	ఒకవేళ (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		తిరిగి SBI_EINVAL;

	sbi_timer_mdelay(5000);

	/* Wait for interrupt */
	wfi();

	తిరిగి SBI_OK;
}

స్థిర నిర్మాణం sbi_system_suspend_device sbi_system_suspend_test = {
	.name = "system-suspend-test",
	.system_suspend_check = sbi_system_suspend_test_check,
	.system_suspend = sbi_system_suspend_test_suspend,
};

శూన్య sbi_system_suspend_test_enable(శూన్య)
{
	sbi_system_suspend_సమితి_device(&sbi_system_suspend_test);
}

bool sbi_system_suspend_supported(u32 sleep_type)
{
	తిరిగి suspend_dev && suspend_dev->system_suspend_check &&
	       suspend_dev->system_suspend_check(sleep_type) == 0;
}

bool sbi_system_is_suspended(శూన్య)
{
	తిరిగి system_suspended;
}

శూన్య sbi_system_resume(శూన్య)
{
	ఒకవేళ (suspend_dev && suspend_dev->system_resume)
		suspend_dev->system_resume();

	system_suspended = false;
}

పూర్ణాంకం sbi_system_suspend(u32 sleep_type, ulong resume_addr, ulong opaque)
{
	నిర్మాణం sbi_domain *dom = sbi_domain_thishart_ptr();
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	శూన్య (*jump_warmboot)(శూన్య) = (శూన్య (*)(శూన్య))scratch->warmboot_addr;
	గుర్తులేని పూర్ణాంకం hartindex = current_hartindex();
	గుర్తులేని దీర్ఘ prev_బహుళకం;
	గుర్తులేని దీర్ఘ i;
	పూర్ణాంకం ret;

	ఒకవేళ (!dom || !dom->system_suspend_allowed)
		తిరిగి SBI_EFAIL;

	ఒకవేళ (!suspend_dev || !suspend_dev->system_suspend ||
	    !suspend_dev->system_suspend_check)
		తిరిగి SBI_EFAIL;

	ret = suspend_dev->system_suspend_check(sleep_type);
	ఒకవేళ (ret != SBI_OK)
		తిరిగి ret;

	prev_బహుళకం = sbi_mstatus_prev_బహుళకం(csr_read(CSR_MSTATUS));
	ఒకవేళ (prev_బహుళకం != PRV_S && prev_బహుళకం != PRV_U)
		తిరిగి SBI_EFAIL;

	spin_తాళం(&dom->assigned_harts_తాళం);
	sbi_hartmask_for_each_hartindex(i, &dom->assigned_harts) {
		ఒకవేళ (i == hartindex)
			కొనసాగు;
		ఒకవేళ (__sbi_hsm_hart_get_state(i) != SBI_HSM_STATE_STOPPED) {
			spin_unlock(&dom->assigned_harts_తాళం);
			తిరిగి SBI_ERR_DENIED;
		}
	}
	spin_unlock(&dom->assigned_harts_తాళం);

	ఒకవేళ (!sbi_domain_check_addr(dom, resume_addr, prev_బహుళకం,
				   SBI_DOMAIN_EXECUTE))
		తిరిగి SBI_EINVALID_ADDR;

	ఒకవేళ (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_STARTED,
				       SBI_HSM_STATE_SUSPENDED))
		తిరిగి SBI_EFAIL;

	/* Prepare for resume */
	scratch->తదుపరి_బహుళకం = prev_బహుళకం;
	scratch->తదుపరి_addr = resume_addr;
	scratch->తదుపరి_arg1 = opaque;

	__sbi_hsm_suspend_non_ret_save(scratch);

	/* Suspend */
	system_suspended = true;
	ret = suspend_dev->system_suspend(sleep_type, scratch->warmboot_addr);
	ఒకవేళ (ret != SBI_OK) {
		ఒకవేళ (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_SUSPENDED,
					       SBI_HSM_STATE_STARTED))
			sbi_hart_hang();

		system_suspended = false;
		తిరిగి ret;
	}

	/* Resume */
	jump_warmboot();

	__builtin_unreachable();
}
