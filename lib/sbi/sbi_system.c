/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitops.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hsm.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_platform.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_system.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ipi.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_init.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_timer.h>

ಸ್ಥಿರ SBI_LIST_HEAD(reset_devices_list);

ಧ್ರುವ ಸಂರಚನಾ sbi_system_reset_device *sbi_system_reset_get_device(
					u32 reset_type, u32 reset_reason)
{
	ಸಂರಚನಾ sbi_system_reset_device *reset_dev = ರಿಕ್ತ;
	ಸಂರಚನಾ sbi_dlist *pos;
	/** lowest priority - any non zero is our candidate */
	ಪೂರ್ಣಾಙ್ಕ priority = 0;

	/* Check each reset device registered for supported reset type */
	sbi_list_for_each(pos, &(reset_devices_list)) {
		ಸಂರಚನಾ sbi_system_reset_device *dev =
			to_system_reset_device(pos);
		ಯದಿ (dev->system_reset_check) {
			ಪೂರ್ಣಾಙ್ಕ status = dev->system_reset_check(reset_type,
							     reset_reason);
			/** reset_type not supported */
			ಯದಿ (status == 0)
				ಅನುವರ್ತ;

			ಯದಿ (status > priority) {
				reset_dev = dev;
				priority = status;
			}
		}
	}

	ಪ್ರತ್ಯಾಗಮ reset_dev;
}

ಶೂನ್ಯ sbi_system_reset_add_device(ಸಂರಚನಾ sbi_system_reset_device *dev)
{
	ಯದಿ (!dev || !dev->system_reset_check)
		ಪ್ರತ್ಯಾಗಮ;

	sbi_list_add(&(dev->ಗಂಟು), &(reset_devices_list));
}

bool sbi_system_reset_supported(u32 reset_type, u32 reset_reason)
{
	ಪ್ರತ್ಯಾಗಮ !!sbi_system_reset_get_device(reset_type, reset_reason);
}

ಶೂನ್ಯ __noreturn sbi_system_reset(u32 reset_type, u32 reset_reason)
{
	ಸಂರಚನಾ sbi_domain *dom = sbi_domain_thishart_ptr();
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/* Send HALT IPI to every hart other than the current hart */
	sbi_ipi_send_halt(0, -1UL);

	/* Stop current HART */
	sbi_hsm_hart_stop(scratch, false);

	/* Platform specific reset if domain allowed system reset */
	ಯದಿ (dom->system_reset_allowed) {
		ಧ್ರುವ ಸಂರಚನಾ sbi_system_reset_device *dev =
			sbi_system_reset_get_device(reset_type, reset_reason);
		ಯದಿ (dev)
			dev->system_reset(reset_type, reset_reason);
	}

	/* If platform specific reset did not work then do sbi_exit() */
	sbi_exit(scratch);
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ sbi_system_suspend_device *suspend_dev = ರಿಕ್ತ;
ಸ್ಥಿರ bool system_suspended;

ಧ್ರುವ ಸಂರಚನಾ sbi_system_suspend_device *sbi_system_suspend_get_device(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ suspend_dev;
}

ಶೂನ್ಯ sbi_system_suspend_set_device(ಸಂರಚನಾ sbi_system_suspend_device *dev)
{
	ಯದಿ (!dev || suspend_dev)
		ಪ್ರತ್ಯಾಗಮ;

	suspend_dev = dev;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_system_suspend_test_check(u32 sleep_type)
{
	ಪ್ರತ್ಯಾಗಮ sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? 0 : SBI_EINVAL;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_system_suspend_test_suspend(u32 sleep_type,
					   unsigned long mmode_resume_addr)
{
	ಯದಿ (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	sbi_timer_mdelay(5000);

	/* Wait for interrupt */
	wfi();

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_system_suspend_device sbi_system_suspend_test = {
	.name = "system-suspend-test",
	.system_suspend_check = sbi_system_suspend_test_check,
	.system_suspend = sbi_system_suspend_test_suspend,
};

ಶೂನ್ಯ sbi_system_suspend_test_enable(ಶೂನ್ಯ)
{
	sbi_system_suspend_set_device(&sbi_system_suspend_test);
}

bool sbi_system_suspend_supported(u32 sleep_type)
{
	ಪ್ರತ್ಯಾಗಮ suspend_dev && suspend_dev->system_suspend_check &&
	       suspend_dev->system_suspend_check(sleep_type) == 0;
}

bool sbi_system_is_suspended(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ system_suspended;
}

ಶೂನ್ಯ sbi_system_resume(ಶೂನ್ಯ)
{
	ಯದಿ (suspend_dev && suspend_dev->system_resume)
		suspend_dev->system_resume();

	system_suspended = false;
}

ಪೂರ್ಣಾಙ್ಕ sbi_system_suspend(u32 sleep_type, ulong resume_addr, ulong opaque)
{
	ಸಂರಚನಾ sbi_domain *dom = sbi_domain_thishart_ptr();
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ಶೂನ್ಯ (*jump_warmboot)(ಶೂನ್ಯ) = (ಶೂನ್ಯ (*)(ಶೂನ್ಯ))scratch->warmboot_addr;
	unsigned ಪೂರ್ಣಾಙ್ಕ hartindex = current_hartindex();
	unsigned long prev_mode;
	unsigned long i;
	ಪೂರ್ಣಾಙ್ಕ ret;

	ಯದಿ (!dom || !dom->system_suspend_allowed)
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;

	ಯದಿ (!suspend_dev || !suspend_dev->system_suspend ||
	    !suspend_dev->system_suspend_check)
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;

	ret = suspend_dev->system_suspend_check(sleep_type);
	ಯದಿ (ret != SBI_OK)
		ಪ್ರತ್ಯಾಗಮ ret;

	prev_mode = sbi_mstatus_prev_mode(csr_read(CSR_MSTATUS));
	ಯದಿ (prev_mode != PRV_S && prev_mode != PRV_U)
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;

	spin_lock(&dom->assigned_harts_lock);
	sbi_hartmask_for_each_hartindex(i, &dom->assigned_harts) {
		ಯದಿ (i == hartindex)
			ಅನುವರ್ತ;
		ಯದಿ (__sbi_hsm_hart_get_state(i) != SBI_HSM_STATE_STOPPED) {
			spin_unlock(&dom->assigned_harts_lock);
			ಪ್ರತ್ಯಾಗಮ SBI_ERR_DENIED;
		}
	}
	spin_unlock(&dom->assigned_harts_lock);

	ಯದಿ (!sbi_domain_check_addr(dom, resume_addr, prev_mode,
				   SBI_DOMAIN_EXECUTE))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVALID_ADDR;

	ಯದಿ (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_STARTED,
				       SBI_HSM_STATE_SUSPENDED))
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;

	/* Prepare for resume */
	scratch->next_mode = prev_mode;
	scratch->next_addr = resume_addr;
	scratch->next_arg1 = opaque;

	__sbi_hsm_suspend_non_ret_save(scratch);

	/* Suspend */
	system_suspended = true;
	ret = suspend_dev->system_suspend(sleep_type, scratch->warmboot_addr);
	ಯದಿ (ret != SBI_OK) {
		ಯದಿ (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_SUSPENDED,
					       SBI_HSM_STATE_STARTED))
			sbi_hart_hang();

		system_suspended = false;
		ಪ್ರತ್ಯಾಗಮ ret;
	}

	/* Resume */
	jump_warmboot();

	__builtin_unreachable();
}
