/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#ಅನ್ತರ್ಭಾವ <andes/andes.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_system.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_timer.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/cache/fdt_cmo_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#ವ್ಯಾಖ್ಯಾನಿಸು HART_SLEEP_TIMEOUT_MS 1000

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ wait_secondary_harts_sleep(u32 hartid, bool deep_sleep)
{
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain *dom = &ಬೇರು;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ i;
	u32 ಗುರಿ;
	ರಚನೆ atcsmu_sleep_arg arg;

	arg.deep_sleep = deep_sleep;

	/* Wait for the secondary harts entering the corresponding sleep state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		ಗುರಿ = sbi_hartindex_ಗಮ್ಯ_hartid(i);
		ಆದರೆ (ಗುರಿ == hartid)
			ಮುಂದುವರಿ;

		arg.hartid = ಗುರಿ;
		ಆದರೆ (!sbi_timer_waitms_until(atcsmu_hart_is_sleep, &arg,
					    HART_SLEEP_TIMEOUT_MS)) {
			sbi_printf("ATCSMU: hart%u (PCS%u): timed out waiting for %s sleep\n",
				   ಗುರಿ, ಗುರಿ + 3,
				   deep_sleep ? "deep" : "light");
			ಹಿಂತಿರುಗಿ SBI_ETIMEOUT;
		}
	}

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ae350_system_suspend_check(u32 sleep_type)
{
	ಹಿಂತಿರುಗಿ (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ||
		sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) ? SBI_OK : SBI_EINVAL;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ae350_system_suspend(u32 sleep_type, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr)
{
	u32 hartid = current_hartid();
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ saved_mie;
	ಪೂರ್ಣಾಂಕ rc;

	/* Prevent the core leaving the WFI mode unexpectedly */
	saved_mie = csr_read(CSR_MIE);
	csr_write(CSR_MIE, 0);

	/* SMU wakes the primary hart on RTC alarm / UART2 */
	atcsmu_ಗಣ_wakeup_events(PCS_WAKEUP_RTC_ALARM_ಮುಖವಾಡ | PCS_WAKEUP_UART2_ಮುಖವಾಡ, hartid);

	ಆದರೆ (sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) {
		rc = wait_secondary_harts_sleep(hartid, false);
		ಆದರೆ (rc)
			ಹೋಗು err_restore_mie;

		/* Clock-gated only: enable SEI to resume past the WFI */
		csr_ಗಣ(CSR_MIE, MIP_SEIP);
		atcsmu_ಗಣ_command(LIGHT_SLEEP_CMD, hartid);
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		rc = wait_secondary_harts_sleep(hartid, true);
		ಆದರೆ (rc)
			ಹೋಗು err_restore_mie;

		rc = atcsmu_ಗಣ_reset_ಸದಿಶ((ulong)ae350_enable_coherency_warmboot, hartid);
		ಆದರೆ (rc)
			ಹೋಗು err_restore_mie;

		ae350_non_ret_save(sbi_scratch_thishart_ptr());

		/* No LLC is fine; only fail on real errors */
		rc = fdt_cmo_llc_enable(false);
		ಆದರೆ (rc && rc != SBI_ENODEV)
			ಹೋಗು err_discard_save;

		rc = fdt_cmo_llc_flush_all();
		ಆದರೆ (rc && rc != SBI_ENODEV)
			ಹೋಗು err_enable_llc;

		atcsmu_ಗಣ_command(DEEP_SLEEP_CMD, hartid);
	}

	ae350_disable_coherency();
	wfi();

	/* Light sleep resumes here */
	ae350_enable_coherency();

	ಹಿಂತಿರುಗಿ SBI_OK;

err_enable_llc:
	fdt_cmo_llc_enable(true);
err_discard_save:
	ae350_non_ret_discard(sbi_scratch_thishart_ptr());
err_restore_mie:
	csr_write(CSR_MIE, saved_mie);

	ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಶೂನ್ಯ ae350_system_resume(ಶೂನ್ಯ)
{
	u32 hartid = current_hartid();
	u32 sleep_type = atcsmu_get_sleep_type(hartid);

	ಆದರೆ (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		fdt_cmo_llc_enable(true);
		ae350_non_ret_restore(sbi_scratch_thishart_ptr());
	}
}

ಸ್ಥಿರ ರಚನೆ sbi_system_suspend_device suspend_andes_atcsmu = {
	.name = "andes_atcsmu",
	.system_suspend_check = ae350_system_suspend_check,
	.system_suspend = ae350_system_suspend,
	.system_resume = ae350_system_resume,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ suspend_andes_atcsmu_probe(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff, ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	sbi_system_suspend_ಗಣ_device(&suspend_andes_atcsmu);
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ suspend_andes_atcsmu_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "andestech,atcsmu-sys" },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_suspend_andes_atcsmu = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = suspend_andes_atcsmu_ಹೊಂದಿಕೆ,
	.init = suspend_andes_atcsmu_probe,
};
