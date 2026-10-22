/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#அந்தர்பாவ <andes/andes.h>
#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_domain.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_ecall_interface.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_system.h>
#அந்தர்பாவ <sbi/sbi_timer.h>
#அந்தர்பாவ <sbi_utils/cache/fdt_cmo_helper.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_driver.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#define HART_SLEEP_TIMEOUT_MS 1000

நிலையான முழு wait_secondary_harts_sleep(u32 hartid, bool deep_sleep)
{
	மாறிலி கட்டமைப்பு sbi_domain *dom = &வேர்;
	unsigned long i;
	u32 target;
	கட்டமைப்பு atcsmu_sleep_arg arg;

	arg.deep_sleep = deep_sleep;

	/* Wait for the secondary harts entering the corresponding sleep state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		எனில் (target == hartid)
			தொடர்;

		arg.hartid = target;
		எனில் (!sbi_timer_waitms_until(atcsmu_hart_is_sleep, &arg,
					    HART_SLEEP_TIMEOUT_MS)) {
			sbi_printf("ATCSMU: hart%u (PCS%u): timed out waiting for %s sleep\n",
				   target, target + 3,
				   deep_sleep ? "deep" : "light");
			திரும்பு SBI_ETIMEOUT;
		}
	}

	திரும்பு SBI_OK;
}

நிலையான முழு ae350_system_suspend_check(u32 sleep_type)
{
	திரும்பு (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ||
		sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) ? SBI_OK : SBI_EINVAL;
}

நிலையான முழு ae350_system_suspend(u32 sleep_type, unsigned long addr)
{
	u32 hartid = current_hartid();
	unsigned long saved_mie;
	முழு rc;

	/* Prevent the core leaving the WFI mode unexpectedly */
	saved_mie = csr_read(CSR_MIE);
	csr_write(CSR_MIE, 0);

	/* SMU wakes the primary hart on RTC alarm / UART2 */
	atcsmu_set_wakeup_events(PCS_WAKEUP_RTC_ALARM_MASK | PCS_WAKEUP_UART2_MASK, hartid);

	எனில் (sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) {
		rc = wait_secondary_harts_sleep(hartid, false);
		எனில் (rc)
			செல் err_restore_mie;

		/* Clock-gated only: enable SEI to resume past the WFI */
		csr_set(CSR_MIE, MIP_SEIP);
		atcsmu_set_command(LIGHT_SLEEP_CMD, hartid);
	} இல்லையெனில் எனில் (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		rc = wait_secondary_harts_sleep(hartid, true);
		எனில் (rc)
			செல் err_restore_mie;

		rc = atcsmu_set_reset_vector((ulong)ae350_enable_coherency_warmboot, hartid);
		எனில் (rc)
			செல் err_restore_mie;

		ae350_non_ret_save(sbi_scratch_thishart_ptr());

		/* No LLC is fine; only fail on real errors */
		rc = fdt_cmo_llc_enable(false);
		எனில் (rc && rc != SBI_ENODEV)
			செல் err_discard_save;

		rc = fdt_cmo_llc_flush_all();
		எனில் (rc && rc != SBI_ENODEV)
			செல் err_enable_llc;

		atcsmu_set_command(DEEP_SLEEP_CMD, hartid);
	}

	ae350_disable_coherency();
	wfi();

	/* Light sleep resumes here */
	ae350_enable_coherency();

	திரும்பு SBI_OK;

err_enable_llc:
	fdt_cmo_llc_enable(true);
err_discard_save:
	ae350_non_ret_discard(sbi_scratch_thishart_ptr());
err_restore_mie:
	csr_write(CSR_MIE, saved_mie);

	திரும்பு rc;
}

நிலையான வெறுமை ae350_system_resume(வெறுமை)
{
	u32 hartid = current_hartid();
	u32 sleep_type = atcsmu_get_sleep_type(hartid);

	எனில் (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		fdt_cmo_llc_enable(true);
		ae350_non_ret_restore(sbi_scratch_thishart_ptr());
	}
}

நிலையான கட்டமைப்பு sbi_system_suspend_device suspend_andes_atcsmu = {
	.name = "andes_atcsmu",
	.system_suspend_check = ae350_system_suspend_check,
	.system_suspend = ae350_system_suspend,
	.system_resume = ae350_system_resume,
};

நிலையான முழு suspend_andes_atcsmu_probe(மாறிலி வெறுமை *fdt, முழு nodeoff, மாறிலி கட்டமைப்பு fdt_match *match)
{
	sbi_system_suspend_set_device(&suspend_andes_atcsmu);
	திரும்பு 0;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match suspend_andes_atcsmu_match[] = {
	{ .compatible = "andestech,atcsmu-sys" },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_suspend_andes_atcsmu = {
	.match_table = suspend_andes_atcsmu_match,
	.init = suspend_andes_atcsmu_probe,
};
