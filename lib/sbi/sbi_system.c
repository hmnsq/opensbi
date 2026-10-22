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

ಸ್ಥಿರ SBI_LIST_ತಲೆ(reset_devices_list);

ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_system_reset_device *sbi_system_reset_get_device(
					u32 reset_type, u32 reset_reason)
{
	ರಚನೆ sbi_system_reset_device *reset_dev = ಖಾಲಿ;
	ರಚನೆ sbi_dlist *pos;
	/** lowest priority - any non zero is our candidate */
	ಪೂರ್ಣಾಂಕ ಆದ್ಯತೆ = 0;

	/* Check each reset device registered for supported reset type */
	sbi_list_for_each(pos, &(reset_devices_list)) {
		ರಚನೆ sbi_system_reset_device *dev =
			ಗಮ್ಯ_system_reset_device(pos);
		ಆದರೆ (dev->system_reset_check) {
			ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2 = dev->system_reset_check(reset_type,
							     reset_reason);
			/** reset_type not supported */
			ಆದರೆ (ಸ್ಥಿತಿ_2 == 0)
				ಮುಂದುವರಿ;

			ಆದರೆ (ಸ್ಥಿತಿ_2 > ಆದ್ಯತೆ) {
				reset_dev = dev;
				ಆದ್ಯತೆ = ಸ್ಥಿತಿ_2;
			}
		}
	}

	ಹಿಂತಿರುಗಿ reset_dev;
}

ಶೂನ್ಯ sbi_system_reset_ಕೂಡಿಸು_device(ರಚನೆ sbi_system_reset_device *dev)
{
	ಆದರೆ (!dev || !dev->system_reset_check)
		ಹಿಂತಿರುಗಿ;

	sbi_list_ಕೂಡಿಸು(&(dev->ಗಂಟು), &(reset_devices_list));
}

bool sbi_system_reset_supported(u32 reset_type, u32 reset_reason)
{
	ಹಿಂತಿರುಗಿ !!sbi_system_reset_get_device(reset_type, reset_reason);
}

ಶೂನ್ಯ __noreturn sbi_system_reset(u32 reset_type, u32 reset_reason)
{
	ರಚನೆ sbi_domain *dom = sbi_domain_thishart_ptr();
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/* Send HALT IPI to every hart other than the current hart */
	sbi_ipi_send_halt(0, -1UL);

	/* Stop current HART */
	sbi_hsm_hart_stop(scratch, false);

	/* Platform specific reset if domain allowed system reset */
	ಆದರೆ (dom->system_reset_allowed) {
		ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_system_reset_device *dev =
			sbi_system_reset_get_device(reset_type, reset_reason);
		ಆದರೆ (dev)
			dev->system_reset(reset_type, reset_reason);
	}

	/* If platform specific reset did not work then do sbi_exit() */
	sbi_exit(scratch);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_system_suspend_device *suspend_dev = ಖಾಲಿ;
ಸ್ಥಿರ bool system_suspended;

ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_system_suspend_device *sbi_system_suspend_get_device(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ suspend_dev;
}

ಶೂನ್ಯ sbi_system_suspend_ಗಣ_device(ರಚನೆ sbi_system_suspend_device *dev)
{
	ಆದರೆ (!dev || suspend_dev)
		ಹಿಂತಿರುಗಿ;

	suspend_dev = dev;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_system_suspend_test_check(u32 sleep_type)
{
	ಹಿಂತಿರುಗಿ sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? 0 : SBI_EINVAL;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_system_suspend_test_suspend(u32 sleep_type,
					   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ mmode_resume_addr)
{
	ಆದರೆ (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	sbi_timer_mdelay(5000);

	/* Wait for interrupt */
	wfi();

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ರಚನೆ sbi_system_suspend_device sbi_system_suspend_test = {
	.name = "system-suspend-test",
	.system_suspend_check = sbi_system_suspend_test_check,
	.system_suspend = sbi_system_suspend_test_suspend,
};

ಶೂನ್ಯ sbi_system_suspend_test_enable(ಶೂನ್ಯ)
{
	sbi_system_suspend_ಗಣ_device(&sbi_system_suspend_test);
}

bool sbi_system_suspend_supported(u32 sleep_type)
{
	ಹಿಂತಿರುಗಿ suspend_dev && suspend_dev->system_suspend_check &&
	       suspend_dev->system_suspend_check(sleep_type) == 0;
}

bool sbi_system_is_suspended(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ system_suspended;
}

ಶೂನ್ಯ sbi_system_resume(ಶೂನ್ಯ)
{
	ಆದರೆ (suspend_dev && suspend_dev->system_resume)
		suspend_dev->system_resume();

	system_suspended = false;
}

ಪೂರ್ಣಾಂಕ sbi_system_suspend(u32 sleep_type, ulong resume_addr, ulong opaque)
{
	ರಚನೆ sbi_domain *dom = sbi_domain_thishart_ptr();
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ಶೂನ್ಯ (*jump_warmboot)(ಶೂನ್ಯ) = (ಶೂನ್ಯ (*)(ಶೂನ್ಯ))scratch->warmboot_addr;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ hartindex = current_hartindex();
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ prev_ಬಹುಲಕ;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ i;
	ಪೂರ್ಣಾಂಕ ret;

	ಆದರೆ (!dom || !dom->system_suspend_allowed)
		ಹಿಂತಿರುಗಿ SBI_EFAIL;

	ಆದರೆ (!suspend_dev || !suspend_dev->system_suspend ||
	    !suspend_dev->system_suspend_check)
		ಹಿಂತಿರುಗಿ SBI_EFAIL;

	ret = suspend_dev->system_suspend_check(sleep_type);
	ಆದರೆ (ret != SBI_OK)
		ಹಿಂತಿರುಗಿ ret;

	prev_ಬಹುಲಕ = sbi_mstatus_prev_ಬಹುಲಕ(csr_read(CSR_MSTATUS));
	ಆದರೆ (prev_ಬಹುಲಕ != PRV_S && prev_ಬಹುಲಕ != PRV_U)
		ಹಿಂತಿರುಗಿ SBI_EFAIL;

	spin_ಬೀಗ(&dom->assigned_harts_ಬೀಗ);
	sbi_hartmask_for_each_hartindex(i, &dom->assigned_harts) {
		ಆದರೆ (i == hartindex)
			ಮುಂದುವರಿ;
		ಆದರೆ (__sbi_hsm_hart_get_state(i) != SBI_HSM_STATE_STOPPED) {
			spin_unlock(&dom->assigned_harts_ಬೀಗ);
			ಹಿಂತಿರುಗಿ SBI_ERR_DENIED;
		}
	}
	spin_unlock(&dom->assigned_harts_ಬೀಗ);

	ಆದರೆ (!sbi_domain_check_addr(dom, resume_addr, prev_ಬಹುಲಕ,
				   SBI_DOMAIN_EXECUTE))
		ಹಿಂತಿರುಗಿ SBI_EINVALID_ADDR;

	ಆದರೆ (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_STARTED,
				       SBI_HSM_STATE_SUSPENDED))
		ಹಿಂತಿರುಗಿ SBI_EFAIL;

	/* Prepare for resume */
	scratch->ಮುಂದಿನ_ಬಹುಲಕ = prev_ಬಹುಲಕ;
	scratch->ಮುಂದಿನ_addr = resume_addr;
	scratch->ಮುಂದಿನ_arg1 = opaque;

	__sbi_hsm_suspend_non_ret_save(scratch);

	/* Suspend */
	system_suspended = true;
	ret = suspend_dev->system_suspend(sleep_type, scratch->warmboot_addr);
	ಆದರೆ (ret != SBI_OK) {
		ಆದರೆ (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_SUSPENDED,
					       SBI_HSM_STATE_STARTED))
			sbi_hart_hang();

		system_suspended = false;
		ಹಿಂತಿರುಗಿ ret;
	}

	/* Resume */
	jump_warmboot();

	__builtin_unreachable();
}
