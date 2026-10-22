/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#include <sbi/riscv_asm.h>
#include <sbi/sbi_bitops.h>
#include <sbi/sbi_domain.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_hsm.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_system.h>
#include <sbi/sbi_ipi.h>
#include <sbi/sbi_init.h>
#include <sbi/sbi_timer.h>

static SBI_LIST_HEAD(reset_devices_list);

const sa_nracanaa sbi_system_reset_device *sbi_system_reset_get_device(
					u32 reset_type, u32 reset_reason)
{
	sa_nracanaa sbi_system_reset_device *reset_dev = NULL;
	sa_nracanaa sbi_dlist *pos;
	/** lowest priority - any non zero is our candidate */
	පූර්ණ_සංඛ්‍යා priority = 0;

	/* Check each reset device registered for supported reset type */
	sbi_list_for_each(pos, &(reset_devices_list)) {
		sa_nracanaa sbi_system_reset_device *dev =
			to_system_reset_device(pos);
		නම් (dev->system_reset_check) {
			පූර්ණ_සංඛ්‍යා status = dev->system_reset_check(reset_type,
							     reset_reason);
			/** reset_type not supported */
			නම් (status == 0)
				continue;

			නම් (status > priority) {
				reset_dev = dev;
				priority = status;
			}
		}
	}

	ආපසු reset_dev;
}

void sbi_system_reset_add_device(sa_nracanaa sbi_system_reset_device *dev)
{
	නම් (!dev || !dev->system_reset_check)
		ආපසු;

	sbi_list_add(&(dev->node), &(reset_devices_list));
}

bool sbi_system_reset_supported(u32 reset_type, u32 reset_reason)
{
	ආපසු !!sbi_system_reset_get_device(reset_type, reset_reason);
}

void __noreturn sbi_system_reset(u32 reset_type, u32 reset_reason)
{
	sa_nracanaa sbi_domain *dom = sbi_domain_thishart_ptr();
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/* Send HALT IPI to every hart other than the current hart */
	sbi_ipi_send_halt(0, -1UL);

	/* Stop current HART */
	sbi_hsm_hart_stop(scratch, false);

	/* Platform specific reset if domain allowed system reset */
	නම් (dom->system_reset_allowed) {
		const sa_nracanaa sbi_system_reset_device *dev =
			sbi_system_reset_get_device(reset_type, reset_reason);
		නම් (dev)
			dev->system_reset(reset_type, reset_reason);
	}

	/* If platform specific reset did not work then do sbi_exit() */
	sbi_exit(scratch);
}

static const sa_nracanaa sbi_system_suspend_device *suspend_dev = NULL;
static bool system_suspended;

const sa_nracanaa sbi_system_suspend_device *sbi_system_suspend_get_device(void)
{
	ආපසු suspend_dev;
}

void sbi_system_suspend_set_device(sa_nracanaa sbi_system_suspend_device *dev)
{
	නම් (!dev || suspend_dev)
		ආපසු;

	suspend_dev = dev;
}

static පූර්ණ_සංඛ්‍යා sbi_system_suspend_test_check(u32 sleep_type)
{
	ආපසු sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? 0 : SBI_EINVAL;
}

static පූර්ණ_සංඛ්‍යා sbi_system_suspend_test_suspend(u32 sleep_type,
					   unsigned long mmode_resume_addr)
{
	නම් (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		ආපසු SBI_EINVAL;

	sbi_timer_mdelay(5000);

	/* Wait for interrupt */
	wfi();

	ආපසු SBI_OK;
}

static sa_nracanaa sbi_system_suspend_device sbi_system_suspend_test = {
	.name = "system-suspend-test",
	.system_suspend_check = sbi_system_suspend_test_check,
	.system_suspend = sbi_system_suspend_test_suspend,
};

void sbi_system_suspend_test_enable(void)
{
	sbi_system_suspend_set_device(&sbi_system_suspend_test);
}

bool sbi_system_suspend_supported(u32 sleep_type)
{
	ආපසු suspend_dev && suspend_dev->system_suspend_check &&
	       suspend_dev->system_suspend_check(sleep_type) == 0;
}

bool sbi_system_is_suspended(void)
{
	ආපසු system_suspended;
}

void sbi_system_resume(void)
{
	නම් (suspend_dev && suspend_dev->system_resume)
		suspend_dev->system_resume();

	system_suspended = false;
}

පූර්ණ_සංඛ්‍යා sbi_system_suspend(u32 sleep_type, ulong resume_addr, ulong opaque)
{
	sa_nracanaa sbi_domain *dom = sbi_domain_thishart_ptr();
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	void (*jump_warmboot)(void) = (void (*)(void))scratch->warmboot_addr;
	unsigned පූර්ණ_සංඛ්‍යා hartindex = current_hartindex();
	unsigned long prev_mode;
	unsigned long i;
	පූර්ණ_සංඛ්‍යා ret;

	නම් (!dom || !dom->system_suspend_allowed)
		ආපසු SBI_EFAIL;

	නම් (!suspend_dev || !suspend_dev->system_suspend ||
	    !suspend_dev->system_suspend_check)
		ආපසු SBI_EFAIL;

	ret = suspend_dev->system_suspend_check(sleep_type);
	නම් (ret != SBI_OK)
		ආපසු ret;

	prev_mode = sbi_mstatus_prev_mode(csr_read(CSR_MSTATUS));
	නම් (prev_mode != PRV_S && prev_mode != PRV_U)
		ආපසු SBI_EFAIL;

	spin_lock(&dom->assigned_harts_lock);
	sbi_hartmask_for_each_hartindex(i, &dom->assigned_harts) {
		නම් (i == hartindex)
			continue;
		නම් (__sbi_hsm_hart_get_state(i) != SBI_HSM_STATE_STOPPED) {
			spin_unlock(&dom->assigned_harts_lock);
			ආපසු SBI_ERR_DENIED;
		}
	}
	spin_unlock(&dom->assigned_harts_lock);

	නම් (!sbi_domain_check_addr(dom, resume_addr, prev_mode,
				   SBI_DOMAIN_EXECUTE))
		ආපසු SBI_EINVALID_ADDR;

	නම් (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_STARTED,
				       SBI_HSM_STATE_SUSPENDED))
		ආපසු SBI_EFAIL;

	/* Prepare for resume */
	scratch->next_mode = prev_mode;
	scratch->next_addr = resume_addr;
	scratch->next_arg1 = opaque;

	__sbi_hsm_suspend_non_ret_save(scratch);

	/* Suspend */
	system_suspended = true;
	ret = suspend_dev->system_suspend(sleep_type, scratch->warmboot_addr);
	නම් (ret != SBI_OK) {
		නම් (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_SUSPENDED,
					       SBI_HSM_STATE_STARTED))
			sbi_hart_hang();

		system_suspended = false;
		ආපසු ret;
	}

	/* Resume */
	jump_warmboot();

	__builtin_unreachable();
}
