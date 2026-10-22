/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#അന്തര്ഭാവ <andes/andes.h>
#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_ecall_interface.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_system.h>
#അന്തര്ഭാവ <sbi/sbi_timer.h>
#അന്തര്ഭാവ <sbi_utils/cache/fdt_cmo_helper.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_driver.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#define HART_SLEEP_TIMEOUT_MS 1000

സ്ഥിതം പൂർണ്ണസംഖ്യ wait_secondary_harts_sleep(u32 hartid, bool deep_sleep)
{
	സ്ഥിരം ഘടന sbi_domain *dom = &വേര്;
	unsigned long i;
	u32 target;
	ഘടന atcsmu_sleep_arg arg;

	arg.deep_sleep = deep_sleep;

	/* Wait for the secondary harts entering the corresponding sleep state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		target = sbi_hartindex_to_hartid(i);
		എങ്കിൽ (target == hartid)
			തുടരുക;

		arg.hartid = target;
		എങ്കിൽ (!sbi_timer_waitms_until(atcsmu_hart_is_sleep, &arg,
					    HART_SLEEP_TIMEOUT_MS)) {
			sbi_printf("ATCSMU: hart%u (PCS%u): timed out waiting for %s sleep\n",
				   target, target + 3,
				   deep_sleep ? "deep" : "light");
			തിരികെ SBI_ETIMEOUT;
		}
	}

	തിരികെ SBI_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ ae350_system_suspend_check(u32 sleep_type)
{
	തിരികെ (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ||
		sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) ? SBI_OK : SBI_EINVAL;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ ae350_system_suspend(u32 sleep_type, unsigned long addr)
{
	u32 hartid = current_hartid();
	unsigned long saved_mie;
	പൂർണ്ണസംഖ്യ rc;

	/* Prevent the core leaving the WFI mode unexpectedly */
	saved_mie = csr_read(CSR_MIE);
	csr_write(CSR_MIE, 0);

	/* SMU wakes the primary hart on RTC alarm / UART2 */
	atcsmu_set_wakeup_events(PCS_WAKEUP_RTC_ALARM_MASK | PCS_WAKEUP_UART2_MASK, hartid);

	എങ്കിൽ (sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) {
		rc = wait_secondary_harts_sleep(hartid, false);
		എങ്കിൽ (rc)
			പോകുക err_restore_mie;

		/* Clock-gated only: enable SEI to resume past the WFI */
		csr_set(CSR_MIE, MIP_SEIP);
		atcsmu_set_command(LIGHT_SLEEP_CMD, hartid);
	} അല്ലെങ്കിൽ എങ്കിൽ (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		rc = wait_secondary_harts_sleep(hartid, true);
		എങ്കിൽ (rc)
			പോകുക err_restore_mie;

		rc = atcsmu_set_reset_vector((ulong)ae350_enable_coherency_warmboot, hartid);
		എങ്കിൽ (rc)
			പോകുക err_restore_mie;

		ae350_non_ret_save(sbi_scratch_thishart_ptr());

		/* No LLC is fine; only fail on real errors */
		rc = fdt_cmo_llc_enable(false);
		എങ്കിൽ (rc && rc != SBI_ENODEV)
			പോകുക err_discard_save;

		rc = fdt_cmo_llc_flush_all();
		എങ്കിൽ (rc && rc != SBI_ENODEV)
			പോകുക err_enable_llc;

		atcsmu_set_command(DEEP_SLEEP_CMD, hartid);
	}

	ae350_disable_coherency();
	wfi();

	/* Light sleep resumes here */
	ae350_enable_coherency();

	തിരികെ SBI_OK;

err_enable_llc:
	fdt_cmo_llc_enable(true);
err_discard_save:
	ae350_non_ret_discard(sbi_scratch_thishart_ptr());
err_restore_mie:
	csr_write(CSR_MIE, saved_mie);

	തിരികെ rc;
}

സ്ഥിതം ശൂന്യം ae350_system_resume(ശൂന്യം)
{
	u32 hartid = current_hartid();
	u32 sleep_type = atcsmu_get_sleep_type(hartid);

	എങ്കിൽ (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		fdt_cmo_llc_enable(true);
		ae350_non_ret_restore(sbi_scratch_thishart_ptr());
	}
}

സ്ഥിതം ഘടന sbi_system_suspend_device suspend_andes_atcsmu = {
	.name = "andes_atcsmu",
	.system_suspend_check = ae350_system_suspend_check,
	.system_suspend = ae350_system_suspend,
	.system_resume = ae350_system_resume,
};

സ്ഥിതം പൂർണ്ണസംഖ്യ suspend_andes_atcsmu_probe(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff, സ്ഥിരം ഘടന fdt_match *match)
{
	sbi_system_suspend_set_device(&suspend_andes_atcsmu);
	തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match suspend_andes_atcsmu_match[] = {
	{ .compatible = "andestech,atcsmu-sys" },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_suspend_andes_atcsmu = {
	.match_table = suspend_andes_atcsmu_match,
	.init = suspend_andes_atcsmu_probe,
};
