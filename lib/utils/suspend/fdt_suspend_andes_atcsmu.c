/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#अन्तर्भाव <andes/andes.h>
#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_system.h>
#अन्तर्भाव <sbi/sbi_timer.h>
#अन्तर्भाव <sbi_utils/cache/fdt_cmo_helper.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#define HART_SLEEP_TIMEOUT_MS 1000

स्थिर पूर्णाङ्क wait_secondary_harts_sleep(u32 hartid, bool deep_sleep)
{
	ध्रुव संरचना sbi_domain *dom = &मूलम्;
	unsigned long i;
	u32 target;
	संरचना atcsmu_sleep_arg arg;

	arg.deep_sleep = deep_sleep;

	/* Wait for the secondary harts entering the corresponding sleep state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		सचे (target == hartid)
			अनुवर्त;

		arg.hartid = target;
		सचे (!sbi_timer_waitms_until(atcsmu_hart_is_sleep, &arg,
					    HART_SLEEP_TIMEOUT_MS)) {
			sbi_printf("ATCSMU: hart%u (PCS%u): timed out waiting for %s sleep\n",
				   target, target + 3,
				   deep_sleep ? "deep" : "light");
			पटिनिवत्त SBI_ETIMEOUT;
		}
	}

	पटिनिवत्त SBI_OK;
}

स्थिर पूर्णाङ्क ae350_system_suspend_check(u32 sleep_type)
{
	पटिनिवत्त (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ||
		sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) ? SBI_OK : SBI_EINVAL;
}

स्थिर पूर्णाङ्क ae350_system_suspend(u32 sleep_type, unsigned long addr)
{
	u32 hartid = current_hartid();
	unsigned long saved_mie;
	पूर्णाङ्क rc;

	/* Prevent the core leaving the WFI mode unexpectedly */
	saved_mie = csr_read(CSR_MIE);
	csr_write(CSR_MIE, 0);

	/* SMU wakes the primary hart on RTC alarm / UART2 */
	atcsmu_set_wakeup_events(PCS_WAKEUP_RTC_ALARM_MASK | PCS_WAKEUP_UART2_MASK, hartid);

	सचे (sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) {
		rc = wait_secondary_harts_sleep(hartid, false);
		सचे (rc)
			गच्छ err_restore_mie;

		/* Clock-gated only: enable SEI to resume past the WFI */
		csr_set(CSR_MIE, MIP_SEIP);
		atcsmu_set_command(LIGHT_SLEEP_CMD, hartid);
	} अञ्ञथा सचे (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		rc = wait_secondary_harts_sleep(hartid, true);
		सचे (rc)
			गच्छ err_restore_mie;

		rc = atcsmu_set_reset_vector((ulong)ae350_enable_coherency_warmboot, hartid);
		सचे (rc)
			गच्छ err_restore_mie;

		ae350_non_ret_save(sbi_scratch_thishart_ptr());

		/* No LLC is fine; only fail on real errors */
		rc = fdt_cmo_llc_enable(false);
		सचे (rc && rc != SBI_ENODEV)
			गच्छ err_discard_save;

		rc = fdt_cmo_llc_flush_all();
		सचे (rc && rc != SBI_ENODEV)
			गच्छ err_enable_llc;

		atcsmu_set_command(DEEP_SLEEP_CMD, hartid);
	}

	ae350_disable_coherency();
	wfi();

	/* Light sleep resumes here */
	ae350_enable_coherency();

	पटिनिवत्त SBI_OK;

err_enable_llc:
	fdt_cmo_llc_enable(true);
err_discard_save:
	ae350_non_ret_discard(sbi_scratch_thishart_ptr());
err_restore_mie:
	csr_write(CSR_MIE, saved_mie);

	पटिनिवत्त rc;
}

स्थिर शून्य ae350_system_resume(शून्य)
{
	u32 hartid = current_hartid();
	u32 sleep_type = atcsmu_get_sleep_type(hartid);

	सचे (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		fdt_cmo_llc_enable(true);
		ae350_non_ret_restore(sbi_scratch_thishart_ptr());
	}
}

स्थिर संरचना sbi_system_suspend_device suspend_andes_atcsmu = {
	.name = "andes_atcsmu",
	.system_suspend_check = ae350_system_suspend_check,
	.system_suspend = ae350_system_suspend,
	.system_resume = ae350_system_resume,
};

स्थिर पूर्णाङ्क suspend_andes_atcsmu_probe(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff, ध्रुव संरचना fdt_match *match)
{
	sbi_system_suspend_set_device(&suspend_andes_atcsmu);
	पटिनिवत्त 0;
}

स्थिर ध्रुव संरचना fdt_match suspend_andes_atcsmu_match[] = {
	{ .compatible = "andestech,atcsmu-sys" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_suspend_andes_atcsmu = {
	.match_table = suspend_andes_atcsmu_match,
	.init = suspend_andes_atcsmu_probe,
};
