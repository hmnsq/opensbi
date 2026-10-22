/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#అన్తర్భావ <andes/andes.h>
#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_ecall_interface.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_system.h>
#అన్తర్భావ <sbi/sbi_timer.h>
#అన్తర్భావ <sbi_utils/cache/fdt_cmo_helper.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_driver.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#నిర్వచించు HART_SLEEP_TIMEOUT_MS 1000

స్థిర పూర్ణాంకం wait_secondary_harts_sleep(u32 hartid, bool deep_sleep)
{
	స్థిరరాశి నిర్మాణం sbi_domain *dom = &మూలం;
	గుర్తులేని దీర్ఘ i;
	u32 లక్ష్యం;
	నిర్మాణం atcsmu_sleep_arg arg;

	arg.deep_sleep = deep_sleep;

	/* Wait for the secondary harts entering the corresponding sleep state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		లక్ష్యం = sbi_hartindex_గమ్యం_hartid(i);
		ఒకవేళ (లక్ష్యం == hartid)
			కొనసాగు;

		arg.hartid = లక్ష్యం;
		ఒకవేళ (!sbi_timer_waitms_until(atcsmu_hart_is_sleep, &arg,
					    HART_SLEEP_TIMEOUT_MS)) {
			sbi_printf("ATCSMU: hart%u (PCS%u): timed out waiting for %s sleep\n",
				   లక్ష్యం, లక్ష్యం + 3,
				   deep_sleep ? "deep" : "light");
			తిరిగి SBI_ETIMEOUT;
		}
	}

	తిరిగి SBI_OK;
}

స్థిర పూర్ణాంకం ae350_system_suspend_check(u32 sleep_type)
{
	తిరిగి (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ||
		sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) ? SBI_OK : SBI_EINVAL;
}

స్థిర పూర్ణాంకం ae350_system_suspend(u32 sleep_type, గుర్తులేని దీర్ఘ addr)
{
	u32 hartid = current_hartid();
	గుర్తులేని దీర్ఘ saved_mie;
	పూర్ణాంకం rc;

	/* Prevent the core leaving the WFI mode unexpectedly */
	saved_mie = csr_read(CSR_MIE);
	csr_write(CSR_MIE, 0);

	/* SMU wakes the primary hart on RTC alarm / UART2 */
	atcsmu_సమితి_wakeup_events(PCS_WAKEUP_RTC_ALARM_ముసుగు | PCS_WAKEUP_UART2_ముసుగు, hartid);

	ఒకవేళ (sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) {
		rc = wait_secondary_harts_sleep(hartid, false);
		ఒకవేళ (rc)
			వెళ్ళు err_restore_mie;

		/* Clock-gated only: enable SEI to resume past the WFI */
		csr_సమితి(CSR_MIE, MIP_SEIP);
		atcsmu_సమితి_command(LIGHT_SLEEP_CMD, hartid);
	} లేకపోతే ఒకవేళ (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		rc = wait_secondary_harts_sleep(hartid, true);
		ఒకవేళ (rc)
			వెళ్ళు err_restore_mie;

		rc = atcsmu_సమితి_reset_సదిశ((ulong)ae350_enable_coherency_warmboot, hartid);
		ఒకవేళ (rc)
			వెళ్ళు err_restore_mie;

		ae350_non_ret_save(sbi_scratch_thishart_ptr());

		/* No LLC is fine; only fail on real errors */
		rc = fdt_cmo_llc_enable(false);
		ఒకవేళ (rc && rc != SBI_ENODEV)
			వెళ్ళు err_discard_save;

		rc = fdt_cmo_llc_flush_all();
		ఒకవేళ (rc && rc != SBI_ENODEV)
			వెళ్ళు err_enable_llc;

		atcsmu_సమితి_command(DEEP_SLEEP_CMD, hartid);
	}

	ae350_disable_coherency();
	wfi();

	/* Light sleep resumes here */
	ae350_enable_coherency();

	తిరిగి SBI_OK;

err_enable_llc:
	fdt_cmo_llc_enable(true);
err_discard_save:
	ae350_non_ret_discard(sbi_scratch_thishart_ptr());
err_restore_mie:
	csr_write(CSR_MIE, saved_mie);

	తిరిగి rc;
}

స్థిర శూన్య ae350_system_resume(శూన్య)
{
	u32 hartid = current_hartid();
	u32 sleep_type = atcsmu_get_sleep_type(hartid);

	ఒకవేళ (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		fdt_cmo_llc_enable(true);
		ae350_non_ret_restore(sbi_scratch_thishart_ptr());
	}
}

స్థిర నిర్మాణం sbi_system_suspend_device suspend_andes_atcsmu = {
	.name = "andes_atcsmu",
	.system_suspend_check = ae350_system_suspend_check,
	.system_suspend = ae350_system_suspend,
	.system_resume = ae350_system_resume,
};

స్థిర పూర్ణాంకం suspend_andes_atcsmu_probe(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff, స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	sbi_system_suspend_సమితి_device(&suspend_andes_atcsmu);
	తిరిగి 0;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక suspend_andes_atcsmu_సరిపోలిక[] = {
	{ .compatible = "andestech,atcsmu-sys" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_suspend_andes_atcsmu = {
	.సరిపోలిక_పట్టిక = suspend_andes_atcsmu_సరిపోలిక,
	.init = suspend_andes_atcsmu_probe,
};
