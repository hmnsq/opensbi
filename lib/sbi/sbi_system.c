/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/sbi_bitops.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_hsm.h>
#അന്തര്ഭാവ <sbi/sbi_platform.h>
#അന്തര്ഭാവ <sbi/sbi_system.h>
#അന്തര്ഭാവ <sbi/sbi_ipi.h>
#അന്തര്ഭാവ <sbi/sbi_init.h>
#അന്തര്ഭാവ <sbi/sbi_timer.h>

സ്ഥിതം SBI_LIST_HEAD(reset_devices_list);

സ്ഥിരം ഘടന sbi_system_reset_device *sbi_system_reset_get_device(
					u32 reset_type, u32 reset_reason)
{
	ഘടന sbi_system_reset_device *reset_dev = ശൂന്യ;
	ഘടന sbi_dlist *pos;
	/** lowest priority - any non zero is our candidate */
	പൂർണ്ണസംഖ്യ priority = 0;

	/* Check each reset device registered for supported reset type */
	sbi_list_for_each(pos, &(reset_devices_list)) {
		ഘടന sbi_system_reset_device *dev =
			to_system_reset_device(pos);
		എങ്കിൽ (dev->system_reset_check) {
			പൂർണ്ണസംഖ്യ status = dev->system_reset_check(reset_type,
							     reset_reason);
			/** reset_type not supported */
			എങ്കിൽ (status == 0)
				തുടരുക;

			എങ്കിൽ (status > priority) {
				reset_dev = dev;
				priority = status;
			}
		}
	}

	തിരികെ reset_dev;
}

ശൂന്യം sbi_system_reset_add_device(ഘടന sbi_system_reset_device *dev)
{
	എങ്കിൽ (!dev || !dev->system_reset_check)
		തിരികെ;

	sbi_list_add(&(dev->കണ്ണി), &(reset_devices_list));
}

bool sbi_system_reset_supported(u32 reset_type, u32 reset_reason)
{
	തിരികെ !!sbi_system_reset_get_device(reset_type, reset_reason);
}

ശൂന്യം __noreturn sbi_system_reset(u32 reset_type, u32 reset_reason)
{
	ഘടന sbi_domain *dom = sbi_domain_thishart_ptr();
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/* Send HALT IPI to every hart other than the current hart */
	sbi_ipi_send_halt(0, -1UL);

	/* Stop current HART */
	sbi_hsm_hart_stop(scratch, false);

	/* Platform specific reset if domain allowed system reset */
	എങ്കിൽ (dom->system_reset_allowed) {
		സ്ഥിരം ഘടന sbi_system_reset_device *dev =
			sbi_system_reset_get_device(reset_type, reset_reason);
		എങ്കിൽ (dev)
			dev->system_reset(reset_type, reset_reason);
	}

	/* If platform specific reset did not work then do sbi_exit() */
	sbi_exit(scratch);
}

സ്ഥിതം സ്ഥിരം ഘടന sbi_system_suspend_device *suspend_dev = ശൂന്യ;
സ്ഥിതം bool system_suspended;

സ്ഥിരം ഘടന sbi_system_suspend_device *sbi_system_suspend_get_device(ശൂന്യം)
{
	തിരികെ suspend_dev;
}

ശൂന്യം sbi_system_suspend_set_device(ഘടന sbi_system_suspend_device *dev)
{
	എങ്കിൽ (!dev || suspend_dev)
		തിരികെ;

	suspend_dev = dev;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_system_suspend_test_check(u32 sleep_type)
{
	തിരികെ sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? 0 : SBI_EINVAL;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_system_suspend_test_suspend(u32 sleep_type,
					   unsigned long mmode_resume_addr)
{
	എങ്കിൽ (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		തിരികെ SBI_EINVAL;

	sbi_timer_mdelay(5000);

	/* Wait for interrupt */
	wfi();

	തിരികെ SBI_OK;
}

സ്ഥിതം ഘടന sbi_system_suspend_device sbi_system_suspend_test = {
	.name = "system-suspend-test",
	.system_suspend_check = sbi_system_suspend_test_check,
	.system_suspend = sbi_system_suspend_test_suspend,
};

ശൂന്യം sbi_system_suspend_test_enable(ശൂന്യം)
{
	sbi_system_suspend_set_device(&sbi_system_suspend_test);
}

bool sbi_system_suspend_supported(u32 sleep_type)
{
	തിരികെ suspend_dev && suspend_dev->system_suspend_check &&
	       suspend_dev->system_suspend_check(sleep_type) == 0;
}

bool sbi_system_is_suspended(ശൂന്യം)
{
	തിരികെ system_suspended;
}

ശൂന്യം sbi_system_resume(ശൂന്യം)
{
	എങ്കിൽ (suspend_dev && suspend_dev->system_resume)
		suspend_dev->system_resume();

	system_suspended = false;
}

പൂർണ്ണസംഖ്യ sbi_system_suspend(u32 sleep_type, ulong resume_addr, ulong opaque)
{
	ഘടന sbi_domain *dom = sbi_domain_thishart_ptr();
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ശൂന്യം (*jump_warmboot)(ശൂന്യം) = (ശൂന്യം (*)(ശൂന്യം))scratch->warmboot_addr;
	unsigned പൂർണ്ണസംഖ്യ hartindex = current_hartindex();
	unsigned long prev_mode;
	unsigned long i;
	പൂർണ്ണസംഖ്യ ret;

	എങ്കിൽ (!dom || !dom->system_suspend_allowed)
		തിരികെ SBI_EFAIL;

	എങ്കിൽ (!suspend_dev || !suspend_dev->system_suspend ||
	    !suspend_dev->system_suspend_check)
		തിരികെ SBI_EFAIL;

	ret = suspend_dev->system_suspend_check(sleep_type);
	എങ്കിൽ (ret != SBI_OK)
		തിരികെ ret;

	prev_mode = sbi_mstatus_prev_mode(csr_read(CSR_MSTATUS));
	എങ്കിൽ (prev_mode != PRV_S && prev_mode != PRV_U)
		തിരികെ SBI_EFAIL;

	spin_lock(&dom->assigned_harts_lock);
	sbi_hartmask_for_each_hartindex(i, &dom->assigned_harts) {
		എങ്കിൽ (i == hartindex)
			തുടരുക;
		എങ്കിൽ (__sbi_hsm_hart_get_state(i) != SBI_HSM_STATE_STOPPED) {
			spin_unlock(&dom->assigned_harts_lock);
			തിരികെ SBI_ERR_DENIED;
		}
	}
	spin_unlock(&dom->assigned_harts_lock);

	എങ്കിൽ (!sbi_domain_check_addr(dom, resume_addr, prev_mode,
				   SBI_DOMAIN_EXECUTE))
		തിരികെ SBI_EINVALID_ADDR;

	എങ്കിൽ (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_STARTED,
				       SBI_HSM_STATE_SUSPENDED))
		തിരികെ SBI_EFAIL;

	/* Prepare for resume */
	scratch->next_mode = prev_mode;
	scratch->next_addr = resume_addr;
	scratch->next_arg1 = opaque;

	__sbi_hsm_suspend_non_ret_save(scratch);

	/* Suspend */
	system_suspended = true;
	ret = suspend_dev->system_suspend(sleep_type, scratch->warmboot_addr);
	എങ്കിൽ (ret != SBI_OK) {
		എങ്കിൽ (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_SUSPENDED,
					       SBI_HSM_STATE_STARTED))
			sbi_hart_hang();

		system_suspended = false;
		തിരികെ ret;
	}

	/* Resume */
	jump_warmboot();

	__builtin_unreachable();
}
