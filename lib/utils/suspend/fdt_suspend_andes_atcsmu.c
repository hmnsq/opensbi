/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#ضمن <andes/andes.h>
#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_system.h>
#ضمن <sbi/sbi_timer.h>
#ضمن <sbi_utils/cache/fdt_cmo_helper.h>
#ضمن <sbi_utils/fdt/fdt_driver.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#تعریف_کړه HART_SLEEP_TIMEOUT_MS 1000

ساكن صحيح wait_secondary_harts_sleep(u32 hartid, bool deep_sleep)
{
	ثابت بنية sbi_domain *dom = &ریښه;
	بینښه اوږد i;
	u32 موخه;
	بنية atcsmu_sleep_arg arg;

	arg.deep_sleep = deep_sleep;

	/* Wait for the secondary harts entering the corresponding sleep state */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		موخه = sbi_hartindex_منزل_hartid(i);
		إذا (موخه == hartid)
			استمر;

		arg.hartid = موخه;
		إذا (!sbi_timer_waitms_until(atcsmu_hart_is_sleep, &arg,
					    HART_SLEEP_TIMEOUT_MS)) {
			sbi_printf("ATCSMU: hart%u (PCS%u): timed out waiting for %s sleep\n",
				   موخه, موخه + 3,
				   deep_sleep ? "deep" : "light");
			أرجع SBI_ETIMEOUT;
		}
	}

	أرجع SBI_OK;
}

ساكن صحيح ae350_system_suspend_check(u32 sleep_type)
{
	أرجع (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND ||
		sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) ? SBI_OK : SBI_EINVAL;
}

ساكن صحيح ae350_system_suspend(u32 sleep_type, بینښه اوږد addr)
{
	u32 hartid = current_hartid();
	بینښه اوږد saved_mie;
	صحيح rc;

	/* Prevent the core leaving the WFI mode unexpectedly */
	saved_mie = csr_read(CSR_MIE);
	csr_write(CSR_MIE, 0);

	/* SMU wakes the primary hart on RTC alarm / UART2 */
	atcsmu_سېټ_wakeup_events(PCS_WAKEUP_RTC_ALARM_ماسک | PCS_WAKEUP_UART2_ماسک, hartid);

	إذا (sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) {
		rc = wait_secondary_harts_sleep(hartid, false);
		إذا (rc)
			اذهب err_restore_mie;

		/* Clock-gated only: enable SEI to resume past the WFI */
		csr_سېټ(CSR_MIE, MIP_SEIP);
		atcsmu_سېټ_command(LIGHT_SLEEP_CMD, hartid);
	} وإلا إذا (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		rc = wait_secondary_harts_sleep(hartid, true);
		إذا (rc)
			اذهب err_restore_mie;

		rc = atcsmu_سېټ_reset_ویکټور((ulong)ae350_enable_coherency_warmboot, hartid);
		إذا (rc)
			اذهب err_restore_mie;

		ae350_non_ret_save(sbi_scratch_thishart_ptr());

		/* No LLC is fine; only fail on real errors */
		rc = fdt_cmo_llc_enable(false);
		إذا (rc && rc != SBI_ENODEV)
			اذهب err_discard_save;

		rc = fdt_cmo_llc_flush_all();
		إذا (rc && rc != SBI_ENODEV)
			اذهب err_enable_llc;

		atcsmu_سېټ_command(DEEP_SLEEP_CMD, hartid);
	}

	ae350_disable_coherency();
	wfi();

	/* Light sleep resumes here */
	ae350_enable_coherency();

	أرجع SBI_OK;

err_enable_llc:
	fdt_cmo_llc_enable(true);
err_discard_save:
	ae350_non_ret_discard(sbi_scratch_thishart_ptr());
err_restore_mie:
	csr_write(CSR_MIE, saved_mie);

	أرجع rc;
}

ساكن فراغ ae350_system_resume(فراغ)
{
	u32 hartid = current_hartid();
	u32 sleep_type = atcsmu_get_sleep_type(hartid);

	إذا (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		fdt_cmo_llc_enable(true);
		ae350_non_ret_restore(sbi_scratch_thishart_ptr());
	}
}

ساكن بنية sbi_system_suspend_device suspend_andes_atcsmu = {
	.name = "andes_atcsmu",
	.system_suspend_check = ae350_system_suspend_check,
	.system_suspend = ae350_system_suspend,
	.system_resume = ae350_system_resume,
};

ساكن صحيح suspend_andes_atcsmu_probe(ثابت فراغ *fdt, صحيح nodeoff, ثابت بنية fdt_سمون *سمون)
{
	sbi_system_suspend_سېټ_device(&suspend_andes_atcsmu);
	أرجع 0;
}

ساكن ثابت بنية fdt_سمون suspend_andes_atcsmu_سمون[] = {
	{ .compatible = "andestech,atcsmu-sys" },
	{ },
};

ثابت بنية fdt_driver fdt_suspend_andes_atcsmu = {
	.سمون_جدول = suspend_andes_atcsmu_سمون,
	.init = suspend_andes_atcsmu_probe,
};
