/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <andes/andes.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_system.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/cache/fdt_cmo_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_driver.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HART_SLEEP_TIMEOUT_MS 1000

ਸਥਿਰ ਪੂਰਨ wait_secondary_harts_sleep(u32 hartid, bool deep_sleep)
{
	ਅਚਲ ਬਣਤਰ sbi_domain *dom = &ਜੜ੍ਹ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ i;
	u32 ਨਿਸ਼ਾਨਾ;
	ਬਣਤਰ atcsmu_sleep_arg arg;

	arg.deep_sleep = deep_sleep;

	/* Wait for the secondary harts entering the corresponding sleep state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ਨਿਸ਼ਾਨਾ = sbi_hartindex_ਮੰਜ਼ਿਲ_hartid(i);
		ਜੇ (ਨਿਸ਼ਾਨਾ == hartid)
			ਜਾਰੀ;

		arg.hartid = ਨਿਸ਼ਾਨਾ;
		ਜੇ (!sbi_timer_waitms_until(atcsmu_hart_is_sleep, &arg,
					    HART_SLEEP_TIMEOUT_MS)) {
			sbi_printf("ATCSMU: hart%u (PCS%u): timed out waiting for %s sleep\n",
				   ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ + 3,
				   deep_sleep ? "deep" : "light");
			ਵਾਪਸ SBI_ETIMEOUT;
		}
	}

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਪੂਰਨ ae350_system_suspend_check(u32 sleep_type)
{
	ਵਾਪਸ (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ||
		sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) ? SBI_OK : SBI_EINVAL;
}

ਸਥਿਰ ਪੂਰਨ ae350_system_suspend(u32 sleep_type, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr)
{
	u32 hartid = current_hartid();
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ saved_mie;
	ਪੂਰਨ rc;

	/* Prevent the core leaving the WFI mode unexpectedly */
	saved_mie = csr_read(CSR_MIE);
	csr_write(CSR_MIE, 0);

	/* SMU wakes the primary hart on RTC alarm / UART2 */
	atcsmu_ਸੈੱਟ_wakeup_events(PCS_WAKEUP_RTC_ALARM_ਨਕਾਬ | PCS_WAKEUP_UART2_ਨਕਾਬ, hartid);

	ਜੇ (sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) {
		rc = wait_secondary_harts_sleep(hartid, false);
		ਜੇ (rc)
			ਜਾਓ err_restore_mie;

		/* Clock-gated only: enable SEI to resume past the WFI */
		csr_ਸੈੱਟ(CSR_MIE, MIP_SEIP);
		atcsmu_ਸੈੱਟ_command(LIGHT_SLEEP_CMD, hartid);
	} ਨਹੀਂਤਾਂ ਜੇ (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		rc = wait_secondary_harts_sleep(hartid, true);
		ਜੇ (rc)
			ਜਾਓ err_restore_mie;

		rc = atcsmu_ਸੈੱਟ_reset_ਸਦਿਸ਼((ulong)ae350_enable_coherency_warmboot, hartid);
		ਜੇ (rc)
			ਜਾਓ err_restore_mie;

		ae350_non_ret_save(sbi_scratch_thishart_ptr());

		/* No LLC is fine; only fail on real errors */
		rc = fdt_cmo_llc_enable(false);
		ਜੇ (rc && rc != SBI_ENODEV)
			ਜਾਓ err_discard_save;

		rc = fdt_cmo_llc_flush_all();
		ਜੇ (rc && rc != SBI_ENODEV)
			ਜਾਓ err_enable_llc;

		atcsmu_ਸੈੱਟ_command(DEEP_SLEEP_CMD, hartid);
	}

	ae350_disable_coherency();
	wfi();

	/* Light sleep resumes here */
	ae350_enable_coherency();

	ਵਾਪਸ SBI_OK;

err_enable_llc:
	fdt_cmo_llc_enable(true);
err_discard_save:
	ae350_non_ret_discard(sbi_scratch_thishart_ptr());
err_restore_mie:
	csr_write(CSR_MIE, saved_mie);

	ਵਾਪਸ rc;
}

ਸਥਿਰ ਖਾਲੀ ae350_system_resume(ਖਾਲੀ)
{
	u32 hartid = current_hartid();
	u32 sleep_type = atcsmu_get_sleep_type(hartid);

	ਜੇ (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		fdt_cmo_llc_enable(true);
		ae350_non_ret_restore(sbi_scratch_thishart_ptr());
	}
}

ਸਥਿਰ ਬਣਤਰ sbi_system_suspend_device suspend_andes_atcsmu = {
	.name = "andes_atcsmu",
	.system_suspend_check = ae350_system_suspend_check,
	.system_suspend = ae350_system_suspend,
	.system_resume = ae350_system_resume,
};

ਸਥਿਰ ਪੂਰਨ suspend_andes_atcsmu_probe(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff, ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	sbi_system_suspend_ਸੈੱਟ_device(&suspend_andes_atcsmu);
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ suspend_andes_atcsmu_ਮੇਲ[] = {
	{ .compatible = "andestech,atcsmu-sys" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_suspend_andes_atcsmu = {
	.ਮੇਲ_ਸਾਰਣੀ = suspend_andes_atcsmu_ਮੇਲ,
	.init = suspend_andes_atcsmu_probe,
};
