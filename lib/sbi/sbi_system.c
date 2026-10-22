/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/sbi_bitops.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_hsm.h>
#अन्तर्भाव <sbi/sbi_platform.h>
#अन्तर्भाव <sbi/sbi_system.h>
#अन्तर्भाव <sbi/sbi_ipi.h>
#अन्तर्भाव <sbi/sbi_init.h>
#अन्तर्भाव <sbi/sbi_timer.h>

स्थिर SBI_LIST_HEAD(reset_devices_list);

ध्रुव संरचना sbi_system_reset_device *sbi_system_reset_get_device(
					u32 reset_type, u32 reset_reason)
{
	संरचना sbi_system_reset_device *reset_dev = रिक्त;
	संरचना sbi_dlist *pos;
	/** lowest priority - any non zero is our candidate */
	पूर्णाङ्क priority = 0;

	/* Check each reset device registered for supported reset type */
	sbi_list_for_each(pos, &(reset_devices_list)) {
		संरचना sbi_system_reset_device *dev =
			to_system_reset_device(pos);
		यदि (dev->system_reset_check) {
			पूर्णाङ्क status = dev->system_reset_check(reset_type,
							     reset_reason);
			/** reset_type not supported */
			यदि (status == 0)
				अनुवर्त;

			यदि (status > priority) {
				reset_dev = dev;
				priority = status;
			}
		}
	}

	प्रत्यागम reset_dev;
}

शून्य sbi_system_reset_add_device(संरचना sbi_system_reset_device *dev)
{
	यदि (!dev || !dev->system_reset_check)
		प्रत्यागम;

	sbi_list_add(&(dev->गाँठ), &(reset_devices_list));
}

bool sbi_system_reset_supported(u32 reset_type, u32 reset_reason)
{
	प्रत्यागम !!sbi_system_reset_get_device(reset_type, reset_reason);
}

शून्य __noreturn sbi_system_reset(u32 reset_type, u32 reset_reason)
{
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/* Send HALT IPI to every hart other than the current hart */
	sbi_ipi_send_halt(0, -1UL);

	/* Stop current HART */
	sbi_hsm_hart_stop(scratch, false);

	/* Platform specific reset if domain allowed system reset */
	यदि (dom->system_reset_allowed) {
		ध्रुव संरचना sbi_system_reset_device *dev =
			sbi_system_reset_get_device(reset_type, reset_reason);
		यदि (dev)
			dev->system_reset(reset_type, reset_reason);
	}

	/* If platform specific reset did not work then do sbi_exit() */
	sbi_exit(scratch);
}

स्थिर ध्रुव संरचना sbi_system_suspend_device *suspend_dev = रिक्त;
स्थिर bool system_suspended;

ध्रुव संरचना sbi_system_suspend_device *sbi_system_suspend_get_device(शून्य)
{
	प्रत्यागम suspend_dev;
}

शून्य sbi_system_suspend_set_device(संरचना sbi_system_suspend_device *dev)
{
	यदि (!dev || suspend_dev)
		प्रत्यागम;

	suspend_dev = dev;
}

स्थिर पूर्णाङ्क sbi_system_suspend_test_check(u32 sleep_type)
{
	प्रत्यागम sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? 0 : SBI_EINVAL;
}

स्थिर पूर्णाङ्क sbi_system_suspend_test_suspend(u32 sleep_type,
					   unsigned long mmode_resume_addr)
{
	यदि (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		प्रत्यागम SBI_EINVAL;

	sbi_timer_mdelay(5000);

	/* Wait for interrupt */
	wfi();

	प्रत्यागम SBI_OK;
}

स्थिर संरचना sbi_system_suspend_device sbi_system_suspend_test = {
	.name = "system-suspend-test",
	.system_suspend_check = sbi_system_suspend_test_check,
	.system_suspend = sbi_system_suspend_test_suspend,
};

शून्य sbi_system_suspend_test_enable(शून्य)
{
	sbi_system_suspend_set_device(&sbi_system_suspend_test);
}

bool sbi_system_suspend_supported(u32 sleep_type)
{
	प्रत्यागम suspend_dev && suspend_dev->system_suspend_check &&
	       suspend_dev->system_suspend_check(sleep_type) == 0;
}

bool sbi_system_is_suspended(शून्य)
{
	प्रत्यागम system_suspended;
}

शून्य sbi_system_resume(शून्य)
{
	यदि (suspend_dev && suspend_dev->system_resume)
		suspend_dev->system_resume();

	system_suspended = false;
}

पूर्णाङ्क sbi_system_suspend(u32 sleep_type, ulong resume_addr, ulong opaque)
{
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	शून्य (*jump_warmboot)(शून्य) = (शून्य (*)(शून्य))scratch->warmboot_addr;
	unsigned पूर्णाङ्क hartindex = current_hartindex();
	unsigned long prev_mode;
	unsigned long i;
	पूर्णाङ्क ret;

	यदि (!dom || !dom->system_suspend_allowed)
		प्रत्यागम SBI_EFAIL;

	यदि (!suspend_dev || !suspend_dev->system_suspend ||
	    !suspend_dev->system_suspend_check)
		प्रत्यागम SBI_EFAIL;

	ret = suspend_dev->system_suspend_check(sleep_type);
	यदि (ret != SBI_OK)
		प्रत्यागम ret;

	prev_mode = sbi_mstatus_prev_mode(csr_read(CSR_MSTATUS));
	यदि (prev_mode != PRV_S && prev_mode != PRV_U)
		प्रत्यागम SBI_EFAIL;

	spin_lock(&dom->assigned_harts_lock);
	sbi_hartmask_for_each_hartindex(i, &dom->assigned_harts) {
		यदि (i == hartindex)
			अनुवर्त;
		यदि (__sbi_hsm_hart_get_state(i) != SBI_HSM_STATE_STOPPED) {
			spin_unlock(&dom->assigned_harts_lock);
			प्रत्यागम SBI_ERR_DENIED;
		}
	}
	spin_unlock(&dom->assigned_harts_lock);

	यदि (!sbi_domain_check_addr(dom, resume_addr, prev_mode,
				   SBI_DOMAIN_EXECUTE))
		प्रत्यागम SBI_EINVALID_ADDR;

	यदि (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_STARTED,
				       SBI_HSM_STATE_SUSPENDED))
		प्रत्यागम SBI_EFAIL;

	/* Prepare for resume */
	scratch->next_mode = prev_mode;
	scratch->next_addr = resume_addr;
	scratch->next_arg1 = opaque;

	__sbi_hsm_suspend_non_ret_save(scratch);

	/* Suspend */
	system_suspended = true;
	ret = suspend_dev->system_suspend(sleep_type, scratch->warmboot_addr);
	यदि (ret != SBI_OK) {
		यदि (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_SUSPENDED,
					       SBI_HSM_STATE_STARTED))
			sbi_hart_hang();

		system_suspended = false;
		प्रत्यागम ret;
	}

	/* Resume */
	jump_warmboot();

	__builtin_unreachable();
}
