/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#অন্তর্ভাব <andes/andes.h>
#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_ecall_interface.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_system.h>
#অন্তর্ভাব <sbi/sbi_timer.h>
#অন্তর্ভাব <sbi_utils/cache/fdt_cmo_helper.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#define HART_SLEEP_TIMEOUT_MS 1000

স্থির পূর্ণসংখ্যা wait_secondary_harts_sleep(u32 hartid, bool deep_sleep)
{
	ধ্রুবক কাঠামো sbi_domain *dom = &মূল;
	unsigned long i;
	u32 target;
	কাঠামো atcsmu_sleep_arg arg;

	arg.deep_sleep = deep_sleep;

	/* Wait for the secondary harts entering the corresponding sleep state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		যদি (target == hartid)
			চালু;

		arg.hartid = target;
		যদি (!sbi_timer_waitms_until(atcsmu_hart_is_sleep, &arg,
					    HART_SLEEP_TIMEOUT_MS)) {
			sbi_printf("ATCSMU: hart%u (PCS%u): timed out waiting for %s sleep\n",
				   target, target + 3,
				   deep_sleep ? "deep" : "light");
			ফেরত SBI_ETIMEOUT;
		}
	}

	ফেরত SBI_OK;
}

স্থির পূর্ণসংখ্যা ae350_system_suspend_check(u32 sleep_type)
{
	ফেরত (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ||
		sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) ? SBI_OK : SBI_EINVAL;
}

স্থির পূর্ণসংখ্যা ae350_system_suspend(u32 sleep_type, unsigned long addr)
{
	u32 hartid = current_hartid();
	unsigned long saved_mie;
	পূর্ণসংখ্যা rc;

	/* Prevent the core leaving the WFI mode unexpectedly */
	saved_mie = csr_read(CSR_MIE);
	csr_write(CSR_MIE, 0);

	/* SMU wakes the primary hart on RTC alarm / UART2 */
	atcsmu_set_wakeup_events(PCS_WAKEUP_RTC_ALARM_MASK | PCS_WAKEUP_UART2_MASK, hartid);

	যদি (sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) {
		rc = wait_secondary_harts_sleep(hartid, false);
		যদি (rc)
			যাও err_restore_mie;

		/* Clock-gated only: enable SEI to resume past the WFI */
		csr_set(CSR_MIE, MIP_SEIP);
		atcsmu_set_command(LIGHT_SLEEP_CMD, hartid);
	} অন্যথা যদি (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		rc = wait_secondary_harts_sleep(hartid, true);
		যদি (rc)
			যাও err_restore_mie;

		rc = atcsmu_set_reset_vector((ulong)ae350_enable_coherency_warmboot, hartid);
		যদি (rc)
			যাও err_restore_mie;

		ae350_non_ret_save(sbi_scratch_thishart_ptr());

		/* No LLC is fine; only fail on real errors */
		rc = fdt_cmo_llc_enable(false);
		যদি (rc && rc != SBI_ENODEV)
			যাও err_discard_save;

		rc = fdt_cmo_llc_flush_all();
		যদি (rc && rc != SBI_ENODEV)
			যাও err_enable_llc;

		atcsmu_set_command(DEEP_SLEEP_CMD, hartid);
	}

	ae350_disable_coherency();
	wfi();

	/* Light sleep resumes here */
	ae350_enable_coherency();

	ফেরত SBI_OK;

err_enable_llc:
	fdt_cmo_llc_enable(true);
err_discard_save:
	ae350_non_ret_discard(sbi_scratch_thishart_ptr());
err_restore_mie:
	csr_write(CSR_MIE, saved_mie);

	ফেরত rc;
}

স্থির শূন্য ae350_system_resume(শূন্য)
{
	u32 hartid = current_hartid();
	u32 sleep_type = atcsmu_get_sleep_type(hartid);

	যদি (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		fdt_cmo_llc_enable(true);
		ae350_non_ret_restore(sbi_scratch_thishart_ptr());
	}
}

স্থির কাঠামো sbi_system_suspend_device suspend_andes_atcsmu = {
	.name = "andes_atcsmu",
	.system_suspend_check = ae350_system_suspend_check,
	.system_suspend = ae350_system_suspend,
	.system_resume = ae350_system_resume,
};

স্থির পূর্ণসংখ্যা suspend_andes_atcsmu_probe(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff, ধ্রুবক কাঠামো fdt_match *match)
{
	sbi_system_suspend_set_device(&suspend_andes_atcsmu);
	ফেরত 0;
}

স্থির ধ্রুবক কাঠামো fdt_match suspend_andes_atcsmu_match[] = {
	{ .compatible = "andestech,atcsmu-sys" },
	{ },
};

ধ্রুবক কাঠামো fdt_driver fdt_suspend_andes_atcsmu = {
	.match_table = suspend_andes_atcsmu_match,
	.init = suspend_andes_atcsmu_probe,
};
