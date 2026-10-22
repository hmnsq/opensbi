/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/sbi_bitops.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_hsm.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_system.h>
#شامل <sbi/sbi_ipi.h>
#شامل <sbi/sbi_init.h>
#شامل <sbi/sbi_timer.h>

ساکن SBI_LIST_سر(reset_devices_list);

ثابت ساخت sbi_system_reset_device *sbi_system_reset_get_device(
					u32 reset_type, u32 reset_reason)
{
	ساخت sbi_system_reset_device *reset_dev = تہی;
	ساخت sbi_dlist *pos;
	/** lowest priority - any non zero is our candidate */
	صحیح ترجیح = 0;

	/* Check each reset device registered for supported reset type */
	sbi_list_for_each(pos, &(reset_devices_list)) {
		ساخت sbi_system_reset_device *dev =
			مقصد_system_reset_device(pos);
		اگر (dev->system_reset_check) {
			صحیح حالت_2 = dev->system_reset_check(reset_type,
							     reset_reason);
			/** reset_type not supported */
			اگر (حالت_2 == 0)
				جاری;

			اگر (حالت_2 > ترجیح) {
				reset_dev = dev;
				ترجیح = حالت_2;
			}
		}
	}

	واپس reset_dev;
}

خالی sbi_system_reset_جوڑو_device(ساخت sbi_system_reset_device *dev)
{
	اگر (!dev || !dev->system_reset_check)
		واپس;

	sbi_list_جوڑو(&(dev->گرہ), &(reset_devices_list));
}

bool sbi_system_reset_supported(u32 reset_type, u32 reset_reason)
{
	واپس !!sbi_system_reset_get_device(reset_type, reset_reason);
}

خالی __noreturn sbi_system_reset(u32 reset_type, u32 reset_reason)
{
	ساخت sbi_domain *dom = sbi_domain_thishart_ptr();
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/* Send HALT IPI to every hart other than the current hart */
	sbi_ipi_send_halt(0, -1UL);

	/* Stop current HART */
	sbi_hsm_hart_stop(scratch, false);

	/* Platform specific reset if domain allowed system reset */
	اگر (dom->system_reset_allowed) {
		ثابت ساخت sbi_system_reset_device *dev =
			sbi_system_reset_get_device(reset_type, reset_reason);
		اگر (dev)
			dev->system_reset(reset_type, reset_reason);
	}

	/* If platform specific reset did not work then do sbi_exit() */
	sbi_exit(scratch);
}

ساکن ثابت ساخت sbi_system_suspend_device *suspend_dev = تہی;
ساکن bool system_suspended;

ثابت ساخت sbi_system_suspend_device *sbi_system_suspend_get_device(خالی)
{
	واپس suspend_dev;
}

خالی sbi_system_suspend_سیٹ_device(ساخت sbi_system_suspend_device *dev)
{
	اگر (!dev || suspend_dev)
		واپس;

	suspend_dev = dev;
}

ساکن صحیح sbi_system_suspend_test_check(u32 sleep_type)
{
	واپس sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? 0 : SBI_EINVAL;
}

ساکن صحیح sbi_system_suspend_test_suspend(u32 sleep_type,
					   غیرعلامتی طویل mmode_resume_addr)
{
	اگر (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		واپس SBI_EINVAL;

	sbi_timer_mdelay(5000);

	/* Wait for interrupt */
	wfi();

	واپس SBI_OK;
}

ساکن ساخت sbi_system_suspend_device sbi_system_suspend_test = {
	.name = "system-suspend-test",
	.system_suspend_check = sbi_system_suspend_test_check,
	.system_suspend = sbi_system_suspend_test_suspend,
};

خالی sbi_system_suspend_test_enable(خالی)
{
	sbi_system_suspend_سیٹ_device(&sbi_system_suspend_test);
}

bool sbi_system_suspend_supported(u32 sleep_type)
{
	واپس suspend_dev && suspend_dev->system_suspend_check &&
	       suspend_dev->system_suspend_check(sleep_type) == 0;
}

bool sbi_system_is_suspended(خالی)
{
	واپس system_suspended;
}

خالی sbi_system_resume(خالی)
{
	اگر (suspend_dev && suspend_dev->system_resume)
		suspend_dev->system_resume();

	system_suspended = false;
}

صحیح sbi_system_suspend(u32 sleep_type, ulong resume_addr, ulong opaque)
{
	ساخت sbi_domain *dom = sbi_domain_thishart_ptr();
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	خالی (*jump_warmboot)(خالی) = (خالی (*)(خالی))scratch->warmboot_addr;
	غیرعلامتی صحیح hartindex = current_hartindex();
	غیرعلامتی طویل پچھلا_منوال;
	غیرعلامتی طویل i;
	صحیح ret;

	اگر (!dom || !dom->system_suspend_allowed)
		واپس SBI_EFAIL;

	اگر (!suspend_dev || !suspend_dev->system_suspend ||
	    !suspend_dev->system_suspend_check)
		واپس SBI_EFAIL;

	ret = suspend_dev->system_suspend_check(sleep_type);
	اگر (ret != SBI_OK)
		واپس ret;

	پچھلا_منوال = sbi_mstatus_پچھلا_منوال(csr_read(CSR_MSTATUS));
	اگر (پچھلا_منوال != PRV_S && پچھلا_منوال != PRV_U)
		واپس SBI_EFAIL;

	spin_تالا(&dom->assigned_harts_تالا);
	sbi_hartmask_for_each_hartindex(i, &dom->assigned_harts) {
		اگر (i == hartindex)
			جاری;
		اگر (__sbi_hsm_hart_get_state(i) != SBI_HSM_STATE_STOPPED) {
			spin_unlock(&dom->assigned_harts_تالا);
			واپس SBI_ERR_DENIED;
		}
	}
	spin_unlock(&dom->assigned_harts_تالا);

	اگر (!sbi_domain_check_addr(dom, resume_addr, پچھلا_منوال,
				   SBI_DOMAIN_EXECUTE))
		واپس SBI_EINVALID_ADDR;

	اگر (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_STARTED,
				       SBI_HSM_STATE_SUSPENDED))
		واپس SBI_EFAIL;

	/* Prepare for resume */
	scratch->اگلا_منوال = پچھلا_منوال;
	scratch->اگلا_addr = resume_addr;
	scratch->اگلا_arg1 = opaque;

	__sbi_hsm_suspend_non_ret_save(scratch);

	/* Suspend */
	system_suspended = true;
	ret = suspend_dev->system_suspend(sleep_type, scratch->warmboot_addr);
	اگر (ret != SBI_OK) {
		اگر (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_SUSPENDED,
					       SBI_HSM_STATE_STARTED))
			sbi_hart_hang();

		system_suspended = false;
		واپس ret;
	}

	/* Resume */
	jump_warmboot();

	__builtin_unreachable();
}
