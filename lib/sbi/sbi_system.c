/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/sbi_bitops.h>
#અન્તર્ભાવ <sbi/sbi_domain.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_hsm.h>
#અન્તર્ભાવ <sbi/sbi_platform.h>
#અન્તર્ભાવ <sbi/sbi_system.h>
#અન્તર્ભાવ <sbi/sbi_ipi.h>
#અન્તર્ભાવ <sbi/sbi_init.h>
#અન્તર્ભાવ <sbi/sbi_timer.h>

સ્થિર SBI_LIST_HEAD(reset_devices_list);

અચલ સંરચના sbi_system_reset_device *sbi_system_reset_get_device(
					u32 reset_type, u32 reset_reason)
{
	સંરચના sbi_system_reset_device *reset_dev = ખાલી;
	સંરચના sbi_dlist *pos;
	/** lowest priority - any non zero is our candidate */
	પૂર્ણાંક priority = 0;

	/* Check each reset device registered for supported reset type */
	sbi_list_for_each(pos, &(reset_devices_list)) {
		સંરચના sbi_system_reset_device *dev =
			to_system_reset_device(pos);
		જો (dev->system_reset_check) {
			પૂર્ણાંક status = dev->system_reset_check(reset_type,
							     reset_reason);
			/** reset_type not supported */
			જો (status == 0)
				ચાલુ;

			જો (status > priority) {
				reset_dev = dev;
				priority = status;
			}
		}
	}

	પાછા reset_dev;
}

શૂન્ય sbi_system_reset_add_device(સંરચના sbi_system_reset_device *dev)
{
	જો (!dev || !dev->system_reset_check)
		પાછા;

	sbi_list_add(&(dev->ગાંઠ), &(reset_devices_list));
}

bool sbi_system_reset_supported(u32 reset_type, u32 reset_reason)
{
	પાછા !!sbi_system_reset_get_device(reset_type, reset_reason);
}

શૂન્ય __noreturn sbi_system_reset(u32 reset_type, u32 reset_reason)
{
	સંરચના sbi_domain *dom = sbi_domain_thishart_ptr();
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/* Send HALT IPI to every hart other than the current hart */
	sbi_ipi_send_halt(0, -1UL);

	/* Stop current HART */
	sbi_hsm_hart_stop(scratch, false);

	/* Platform specific reset if domain allowed system reset */
	જો (dom->system_reset_allowed) {
		અચલ સંરચના sbi_system_reset_device *dev =
			sbi_system_reset_get_device(reset_type, reset_reason);
		જો (dev)
			dev->system_reset(reset_type, reset_reason);
	}

	/* If platform specific reset did not work then do sbi_exit() */
	sbi_exit(scratch);
}

સ્થિર અચલ સંરચના sbi_system_suspend_device *suspend_dev = ખાલી;
સ્થિર bool system_suspended;

અચલ સંરચના sbi_system_suspend_device *sbi_system_suspend_get_device(શૂન્ય)
{
	પાછા suspend_dev;
}

શૂન્ય sbi_system_suspend_set_device(સંરચના sbi_system_suspend_device *dev)
{
	જો (!dev || suspend_dev)
		પાછા;

	suspend_dev = dev;
}

સ્થિર પૂર્ણાંક sbi_system_suspend_test_check(u32 sleep_type)
{
	પાછા sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? 0 : SBI_EINVAL;
}

સ્થિર પૂર્ણાંક sbi_system_suspend_test_suspend(u32 sleep_type,
					   unsigned long mmode_resume_addr)
{
	જો (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		પાછા SBI_EINVAL;

	sbi_timer_mdelay(5000);

	/* Wait for interrupt */
	wfi();

	પાછા SBI_OK;
}

સ્થિર સંરચના sbi_system_suspend_device sbi_system_suspend_test = {
	.name = "system-suspend-test",
	.system_suspend_check = sbi_system_suspend_test_check,
	.system_suspend = sbi_system_suspend_test_suspend,
};

શૂન્ય sbi_system_suspend_test_enable(શૂન્ય)
{
	sbi_system_suspend_set_device(&sbi_system_suspend_test);
}

bool sbi_system_suspend_supported(u32 sleep_type)
{
	પાછા suspend_dev && suspend_dev->system_suspend_check &&
	       suspend_dev->system_suspend_check(sleep_type) == 0;
}

bool sbi_system_is_suspended(શૂન્ય)
{
	પાછા system_suspended;
}

શૂન્ય sbi_system_resume(શૂન્ય)
{
	જો (suspend_dev && suspend_dev->system_resume)
		suspend_dev->system_resume();

	system_suspended = false;
}

પૂર્ણાંક sbi_system_suspend(u32 sleep_type, ulong resume_addr, ulong opaque)
{
	સંરચના sbi_domain *dom = sbi_domain_thishart_ptr();
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	શૂન્ય (*jump_warmboot)(શૂન્ય) = (શૂન્ય (*)(શૂન્ય))scratch->warmboot_addr;
	unsigned પૂર્ણાંક hartindex = current_hartindex();
	unsigned long prev_mode;
	unsigned long i;
	પૂર્ણાંક ret;

	જો (!dom || !dom->system_suspend_allowed)
		પાછા SBI_EFAIL;

	જો (!suspend_dev || !suspend_dev->system_suspend ||
	    !suspend_dev->system_suspend_check)
		પાછા SBI_EFAIL;

	ret = suspend_dev->system_suspend_check(sleep_type);
	જો (ret != SBI_OK)
		પાછા ret;

	prev_mode = sbi_mstatus_prev_mode(csr_read(CSR_MSTATUS));
	જો (prev_mode != PRV_S && prev_mode != PRV_U)
		પાછા SBI_EFAIL;

	spin_lock(&dom->assigned_harts_lock);
	sbi_hartmask_for_each_hartindex(i, &dom->assigned_harts) {
		જો (i == hartindex)
			ચાલુ;
		જો (__sbi_hsm_hart_get_state(i) != SBI_HSM_STATE_STOPPED) {
			spin_unlock(&dom->assigned_harts_lock);
			પાછા SBI_ERR_DENIED;
		}
	}
	spin_unlock(&dom->assigned_harts_lock);

	જો (!sbi_domain_check_addr(dom, resume_addr, prev_mode,
				   SBI_DOMAIN_EXECUTE))
		પાછા SBI_EINVALID_ADDR;

	જો (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_STARTED,
				       SBI_HSM_STATE_SUSPENDED))
		પાછા SBI_EFAIL;

	/* Prepare for resume */
	scratch->next_mode = prev_mode;
	scratch->next_addr = resume_addr;
	scratch->next_arg1 = opaque;

	__sbi_hsm_suspend_non_ret_save(scratch);

	/* Suspend */
	system_suspended = true;
	ret = suspend_dev->system_suspend(sleep_type, scratch->warmboot_addr);
	જો (ret != SBI_OK) {
		જો (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_SUSPENDED,
					       SBI_HSM_STATE_STARTED))
			sbi_hart_hang();

		system_suspended = false;
		પાછા ret;
	}

	/* Resume */
	jump_warmboot();

	__builtin_unreachable();
}
