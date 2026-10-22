/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitops.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hsm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_platform.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_system.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ipi.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_init.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_timer.h>

ਸਥਿਰ SBI_LIST_ਸਿਰ(reset_devices_list);

ਅਚਲ ਬਣਤਰ sbi_system_reset_device *sbi_system_reset_get_device(
					u32 reset_type, u32 reset_reason)
{
	ਬਣਤਰ sbi_system_reset_device *reset_dev = ਨਿਰਾ;
	ਬਣਤਰ sbi_dlist *pos;
	/** lowest priority - any non zero is our candidate */
	ਪੂਰਨ ਤਰਜੀਹ = 0;

	/* Check each reset device registered for supported reset type */
	sbi_list_for_each(pos, &(reset_devices_list)) {
		ਬਣਤਰ sbi_system_reset_device *dev =
			ਮੰਜ਼ਿਲ_system_reset_device(pos);
		ਜੇ (dev->system_reset_check) {
			ਪੂਰਨ ਸਥਿਤੀ = dev->system_reset_check(reset_type,
							     reset_reason);
			/** reset_type not supported */
			ਜੇ (ਸਥਿਤੀ == 0)
				ਜਾਰੀ;

			ਜੇ (ਸਥਿਤੀ > ਤਰਜੀਹ) {
				reset_dev = dev;
				ਤਰਜੀਹ = ਸਥਿਤੀ;
			}
		}
	}

	ਵਾਪਸ reset_dev;
}

ਖਾਲੀ sbi_system_reset_ਜੋੜੋ_device(ਬਣਤਰ sbi_system_reset_device *dev)
{
	ਜੇ (!dev || !dev->system_reset_check)
		ਵਾਪਸ;

	sbi_list_ਜੋੜੋ(&(dev->ਗੰਢ), &(reset_devices_list));
}

bool sbi_system_reset_supported(u32 reset_type, u32 reset_reason)
{
	ਵਾਪਸ !!sbi_system_reset_get_device(reset_type, reset_reason);
}

ਖਾਲੀ __noreturn sbi_system_reset(u32 reset_type, u32 reset_reason)
{
	ਬਣਤਰ sbi_domain *dom = sbi_domain_thishart_ptr();
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/* Send HALT IPI to every hart other than the current hart */
	sbi_ipi_send_halt(0, -1UL);

	/* Stop current HART */
	sbi_hsm_hart_stop(scratch, false);

	/* Platform specific reset if domain allowed system reset */
	ਜੇ (dom->system_reset_allowed) {
		ਅਚਲ ਬਣਤਰ sbi_system_reset_device *dev =
			sbi_system_reset_get_device(reset_type, reset_reason);
		ਜੇ (dev)
			dev->system_reset(reset_type, reset_reason);
	}

	/* If platform specific reset did not work then do sbi_exit() */
	sbi_exit(scratch);
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ sbi_system_suspend_device *suspend_dev = ਨਿਰਾ;
ਸਥਿਰ bool system_suspended;

ਅਚਲ ਬਣਤਰ sbi_system_suspend_device *sbi_system_suspend_get_device(ਖਾਲੀ)
{
	ਵਾਪਸ suspend_dev;
}

ਖਾਲੀ sbi_system_suspend_ਸੈੱਟ_device(ਬਣਤਰ sbi_system_suspend_device *dev)
{
	ਜੇ (!dev || suspend_dev)
		ਵਾਪਸ;

	suspend_dev = dev;
}

ਸਥਿਰ ਪੂਰਨ sbi_system_suspend_test_check(u32 sleep_type)
{
	ਵਾਪਸ sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? 0 : SBI_EINVAL;
}

ਸਥਿਰ ਪੂਰਨ sbi_system_suspend_test_suspend(u32 sleep_type,
					   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ mmode_resume_addr)
{
	ਜੇ (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		ਵਾਪਸ SBI_EINVAL;

	sbi_timer_mdelay(5000);

	/* Wait for interrupt */
	wfi();

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਬਣਤਰ sbi_system_suspend_device sbi_system_suspend_test = {
	.name = "system-suspend-test",
	.system_suspend_check = sbi_system_suspend_test_check,
	.system_suspend = sbi_system_suspend_test_suspend,
};

ਖਾਲੀ sbi_system_suspend_test_enable(ਖਾਲੀ)
{
	sbi_system_suspend_ਸੈੱਟ_device(&sbi_system_suspend_test);
}

bool sbi_system_suspend_supported(u32 sleep_type)
{
	ਵਾਪਸ suspend_dev && suspend_dev->system_suspend_check &&
	       suspend_dev->system_suspend_check(sleep_type) == 0;
}

bool sbi_system_is_suspended(ਖਾਲੀ)
{
	ਵਾਪਸ system_suspended;
}

ਖਾਲੀ sbi_system_resume(ਖਾਲੀ)
{
	ਜੇ (suspend_dev && suspend_dev->system_resume)
		suspend_dev->system_resume();

	system_suspended = false;
}

ਪੂਰਨ sbi_system_suspend(u32 sleep_type, ulong resume_addr, ulong opaque)
{
	ਬਣਤਰ sbi_domain *dom = sbi_domain_thishart_ptr();
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ਖਾਲੀ (*jump_warmboot)(ਖਾਲੀ) = (ਖਾਲੀ (*)(ਖਾਲੀ))scratch->warmboot_addr;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ hartindex = current_hartindex();
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ prev_ਬਹੁਲਕ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ i;
	ਪੂਰਨ ret;

	ਜੇ (!dom || !dom->system_suspend_allowed)
		ਵਾਪਸ SBI_EFAIL;

	ਜੇ (!suspend_dev || !suspend_dev->system_suspend ||
	    !suspend_dev->system_suspend_check)
		ਵਾਪਸ SBI_EFAIL;

	ret = suspend_dev->system_suspend_check(sleep_type);
	ਜੇ (ret != SBI_OK)
		ਵਾਪਸ ret;

	prev_ਬਹੁਲਕ = sbi_mstatus_prev_ਬਹੁਲਕ(csr_read(CSR_MSTATUS));
	ਜੇ (prev_ਬਹੁਲਕ != PRV_S && prev_ਬਹੁਲਕ != PRV_U)
		ਵਾਪਸ SBI_EFAIL;

	spin_ਤਾਲਾ(&dom->assigned_harts_ਤਾਲਾ);
	sbi_hartmask_for_each_hartindex(i, &dom->assigned_harts) {
		ਜੇ (i == hartindex)
			ਜਾਰੀ;
		ਜੇ (__sbi_hsm_hart_get_state(i) != SBI_HSM_STATE_STOPPED) {
			spin_unlock(&dom->assigned_harts_ਤਾਲਾ);
			ਵਾਪਸ SBI_ERR_DENIED;
		}
	}
	spin_unlock(&dom->assigned_harts_ਤਾਲਾ);

	ਜੇ (!sbi_domain_check_addr(dom, resume_addr, prev_ਬਹੁਲਕ,
				   SBI_DOMAIN_EXECUTE))
		ਵਾਪਸ SBI_EINVALID_ADDR;

	ਜੇ (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_STARTED,
				       SBI_HSM_STATE_SUSPENDED))
		ਵਾਪਸ SBI_EFAIL;

	/* Prepare for resume */
	scratch->ਅਗਲਾ_ਬਹੁਲਕ = prev_ਬਹੁਲਕ;
	scratch->ਅਗਲਾ_addr = resume_addr;
	scratch->ਅਗਲਾ_arg1 = opaque;

	__sbi_hsm_suspend_non_ret_save(scratch);

	/* Suspend */
	system_suspended = true;
	ret = suspend_dev->system_suspend(sleep_type, scratch->warmboot_addr);
	ਜੇ (ret != SBI_OK) {
		ਜੇ (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_SUSPENDED,
					       SBI_HSM_STATE_STARTED))
			sbi_hart_hang();

		system_suspended = false;
		ਵਾਪਸ ret;
	}

	/* Resume */
	jump_warmboot();

	__builtin_unreachable();
}
