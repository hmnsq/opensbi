/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#شامل <andes/andes.h>
#شامل <sbi/riscv_asm.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_system.h>
#شامل <sbi/sbi_timer.h>
#شامل <sbi_utils/cache/fdt_cmo_helper.h>
#شامل <sbi_utils/fdt/fdt_driver.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#تعریف_کرو HART_SLEEP_TIMEOUT_MS 1000

ساکن صحیح wait_secondary_harts_sleep(u32 hartid, bool deep_sleep)
{
	ثابت ساخت sbi_domain *dom = &جڑ;
	غیرعلامتی طویل i;
	u32 ہدف;
	ساخت atcsmu_sleep_arg arg;

	arg.deep_sleep = deep_sleep;

	/* Wait for the secondary harts entering the corresponding sleep state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ہدف = sbi_hartindex_مقصد_hartid(i);
		اگر (ہدف == hartid)
			جاری;

		arg.hartid = ہدف;
		اگر (!sbi_timer_waitms_until(atcsmu_hart_is_sleep, &arg,
					    HART_SLEEP_TIMEOUT_MS)) {
			sbi_printf("ATCSMU: hart%u (PCS%u): timed out waiting for %s sleep\n",
				   ہدف, ہدف + 3,
				   deep_sleep ? "deep" : "light");
			واپس SBI_ETIMEOUT;
		}
	}

	واپس SBI_OK;
}

ساکن صحیح ae350_system_suspend_check(u32 sleep_type)
{
	واپس (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ||
		sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) ? SBI_OK : SBI_EINVAL;
}

ساکن صحیح ae350_system_suspend(u32 sleep_type, غیرعلامتی طویل addr)
{
	u32 hartid = current_hartid();
	غیرعلامتی طویل saved_mie;
	صحیح rc;

	/* Prevent the core leaving the WFI mode unexpectedly */
	saved_mie = csr_read(CSR_MIE);
	csr_write(CSR_MIE, 0);

	/* SMU wakes the primary hart on RTC alarm / UART2 */
	atcsmu_سیٹ_wakeup_events(PCS_WAKEUP_RTC_ALARM_نقاب | PCS_WAKEUP_UART2_نقاب, hartid);

	اگر (sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) {
		rc = wait_secondary_harts_sleep(hartid, false);
		اگر (rc)
			جاؤ err_restore_mie;

		/* Clock-gated only: enable SEI to resume past the WFI */
		csr_سیٹ(CSR_MIE, MIP_SEIP);
		atcsmu_سیٹ_command(LIGHT_SLEEP_CMD, hartid);
	} ورنہ اگر (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		rc = wait_secondary_harts_sleep(hartid, true);
		اگر (rc)
			جاؤ err_restore_mie;

		rc = atcsmu_سیٹ_reset_سمتیہ((ulong)ae350_enable_coherency_warmboot, hartid);
		اگر (rc)
			جاؤ err_restore_mie;

		ae350_non_ret_save(sbi_scratch_thishart_ptr());

		/* No LLC is fine; only fail on real errors */
		rc = fdt_cmo_llc_enable(false);
		اگر (rc && rc != SBI_ENODEV)
			جاؤ err_discard_save;

		rc = fdt_cmo_llc_flush_all();
		اگر (rc && rc != SBI_ENODEV)
			جاؤ err_enable_llc;

		atcsmu_سیٹ_command(DEEP_SLEEP_CMD, hartid);
	}

	ae350_disable_coherency();
	wfi();

	/* Light sleep resumes here */
	ae350_enable_coherency();

	واپس SBI_OK;

err_enable_llc:
	fdt_cmo_llc_enable(true);
err_discard_save:
	ae350_non_ret_discard(sbi_scratch_thishart_ptr());
err_restore_mie:
	csr_write(CSR_MIE, saved_mie);

	واپس rc;
}

ساکن خالی ae350_system_resume(خالی)
{
	u32 hartid = current_hartid();
	u32 sleep_type = atcsmu_get_sleep_type(hartid);

	اگر (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		fdt_cmo_llc_enable(true);
		ae350_non_ret_restore(sbi_scratch_thishart_ptr());
	}
}

ساکن ساخت sbi_system_suspend_device suspend_andes_atcsmu = {
	.name = "andes_atcsmu",
	.system_suspend_check = ae350_system_suspend_check,
	.system_suspend = ae350_system_suspend,
	.system_resume = ae350_system_resume,
};

ساکن صحیح suspend_andes_atcsmu_probe(ثابت خالی *fdt, صحیح nodeoff, ثابت ساخت fdt_مطابقت *مطابقت)
{
	sbi_system_suspend_سیٹ_device(&suspend_andes_atcsmu);
	واپس 0;
}

ساکن ثابت ساخت fdt_مطابقت suspend_andes_atcsmu_مطابقت[] = {
	{ .compatible = "andestech,atcsmu-sys" },
	{ },
};

ثابت ساخت fdt_driver fdt_suspend_andes_atcsmu = {
	.مطابقت_جدول = suspend_andes_atcsmu_مطابقت,
	.init = suspend_andes_atcsmu_probe,
};
