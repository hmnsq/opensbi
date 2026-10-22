/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/sbi_bitops.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_hsm.h>
#समावेश <sbi/sbi_platform.h>
#समावेश <sbi/sbi_system.h>
#समावेश <sbi/sbi_ipi.h>
#समावेश <sbi/sbi_init.h>
#समावेश <sbi/sbi_timer.h>

अटल SBI_LIST_शीर्ष(reset_devices_list);

स्थिर काष्ठा sbi_system_reset_device *sbi_system_reset_get_device(
					u32 reset_type, u32 reset_reason)
{
	काष्ठा sbi_system_reset_device *reset_dev = शून्य;
	काष्ठा sbi_dlist *pos;
	/** lowest priority - any non zero is our candidate */
	पूर्णांक priority = 0;

	/* Check each reset device registered for supported reset type */
	sbi_list_for_each(pos, &(reset_devices_list)) {
		काष्ठा sbi_system_reset_device *dev =
			to_system_reset_device(pos);
		अगर (dev->system_reset_check) {
			पूर्णांक status = dev->system_reset_check(reset_type,
							     reset_reason);
			/** reset_type not supported */
			अगर (status == 0)
				जारी;

			अगर (status > priority) {
				reset_dev = dev;
				priority = status;
			}
		}
	}

	वापस reset_dev;
}

व्योम sbi_system_reset_add_device(काष्ठा sbi_system_reset_device *dev)
{
	अगर (!dev || !dev->system_reset_check)
		वापस;

	sbi_list_add(&(dev->गाँठ), &(reset_devices_list));
}

bool sbi_system_reset_supported(u32 reset_type, u32 reset_reason)
{
	वापस !!sbi_system_reset_get_device(reset_type, reset_reason);
}

व्योम __noreturn sbi_system_reset(u32 reset_type, u32 reset_reason)
{
	काष्ठा sbi_domain *dom = sbi_domain_thishart_ptr();
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/* Send HALT IPI to every hart other than the current hart */
	sbi_ipi_send_halt(0, -1UL);

	/* Stop current HART */
	sbi_hsm_hart_stop(scratch, false);

	/* Platform specific reset if domain allowed system reset */
	अगर (dom->system_reset_allowed) {
		स्थिर काष्ठा sbi_system_reset_device *dev =
			sbi_system_reset_get_device(reset_type, reset_reason);
		अगर (dev)
			dev->system_reset(reset_type, reset_reason);
	}

	/* If platform specific reset did not work then do sbi_exit() */
	sbi_exit(scratch);
}

अटल स्थिर काष्ठा sbi_system_suspend_device *suspend_dev = शून्य;
अटल bool system_suspended;

स्थिर काष्ठा sbi_system_suspend_device *sbi_system_suspend_get_device(व्योम)
{
	वापस suspend_dev;
}

व्योम sbi_system_suspend_set_device(काष्ठा sbi_system_suspend_device *dev)
{
	अगर (!dev || suspend_dev)
		वापस;

	suspend_dev = dev;
}

अटल पूर्णांक sbi_system_suspend_test_check(u32 sleep_type)
{
	वापस sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ? 0 : SBI_EINVAL;
}

अटल पूर्णांक sbi_system_suspend_test_suspend(u32 sleep_type,
					   अचिन्हित दीर्घ mmode_resume_addr)
{
	अगर (sleep_type != SBI_SUSP_SLEEP_TYPE_SUSPEND)
		वापस SBI_EINVAL;

	sbi_timer_mdelay(5000);

	/* Wait for interrupt */
	wfi();

	वापस SBI_OK;
}

अटल काष्ठा sbi_system_suspend_device sbi_system_suspend_test = {
	.name = "system-suspend-test",
	.system_suspend_check = sbi_system_suspend_test_check,
	.system_suspend = sbi_system_suspend_test_suspend,
};

व्योम sbi_system_suspend_test_enable(व्योम)
{
	sbi_system_suspend_set_device(&sbi_system_suspend_test);
}

bool sbi_system_suspend_supported(u32 sleep_type)
{
	वापस suspend_dev && suspend_dev->system_suspend_check &&
	       suspend_dev->system_suspend_check(sleep_type) == 0;
}

bool sbi_system_is_suspended(व्योम)
{
	वापस system_suspended;
}

व्योम sbi_system_resume(व्योम)
{
	अगर (suspend_dev && suspend_dev->system_resume)
		suspend_dev->system_resume();

	system_suspended = false;
}

पूर्णांक sbi_system_suspend(u32 sleep_type, ulong resume_addr, ulong opaque)
{
	काष्ठा sbi_domain *dom = sbi_domain_thishart_ptr();
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	व्योम (*jump_warmboot)(व्योम) = (व्योम (*)(व्योम))scratch->warmboot_addr;
	अचिन्हित पूर्णांक hartindex = current_hartindex();
	अचिन्हित दीर्घ पूर्व_mode;
	अचिन्हित दीर्घ i;
	पूर्णांक ret;

	अगर (!dom || !dom->system_suspend_allowed)
		वापस SBI_EFAIL;

	अगर (!suspend_dev || !suspend_dev->system_suspend ||
	    !suspend_dev->system_suspend_check)
		वापस SBI_EFAIL;

	ret = suspend_dev->system_suspend_check(sleep_type);
	अगर (ret != SBI_OK)
		वापस ret;

	पूर्व_mode = sbi_mstatus_पूर्व_mode(csr_read(CSR_MSTATUS));
	अगर (पूर्व_mode != PRV_S && पूर्व_mode != PRV_U)
		वापस SBI_EFAIL;

	spin_lock(&dom->assigned_harts_lock);
	sbi_hartmask_for_each_hartindex(i, &dom->assigned_harts) {
		अगर (i == hartindex)
			जारी;
		अगर (__sbi_hsm_hart_get_state(i) != SBI_HSM_STATE_STOPPED) {
			spin_unlock(&dom->assigned_harts_lock);
			वापस SBI_ERR_DENIED;
		}
	}
	spin_unlock(&dom->assigned_harts_lock);

	अगर (!sbi_domain_check_addr(dom, resume_addr, पूर्व_mode,
				   SBI_DOMAIN_EXECUTE))
		वापस SBI_EINVALID_ADDR;

	अगर (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_STARTED,
				       SBI_HSM_STATE_SUSPENDED))
		वापस SBI_EFAIL;

	/* Prepare for resume */
	scratch->अग्र_mode = पूर्व_mode;
	scratch->अग्र_addr = resume_addr;
	scratch->अग्र_arg1 = opaque;

	__sbi_hsm_suspend_non_ret_save(scratch);

	/* Suspend */
	system_suspended = true;
	ret = suspend_dev->system_suspend(sleep_type, scratch->warmboot_addr);
	अगर (ret != SBI_OK) {
		अगर (!sbi_hsm_hart_change_state(scratch, SBI_HSM_STATE_SUSPENDED,
					       SBI_HSM_STATE_STARTED))
			sbi_hart_hang();

		system_suspended = false;
		वापस ret;
	}

	/* Resume */
	jump_warmboot();

	__builtin_unreachable();
}
