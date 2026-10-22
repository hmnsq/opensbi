/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_atomic.h>
#అన్తర్భావ <sbi/riscv_barrier.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_cppc.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_double_trap.h>
#అన్తర్భావ <sbi/sbi_ecall.h>
#అన్తర్భావ <sbi/sbi_fwft.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_hartmask.h>
#అన్తర్భావ <sbi/sbi_hart_pmp.h>
#అన్తర్భావ <sbi/sbi_hart_protection.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi/sbi_hsm.h>
#అన్తర్భావ <sbi/sbi_ipi.h>
#అన్తర్భావ <sbi/sbi_irqchip.h>
#అన్తర్భావ <sbi/sbi_platform.h>
#అన్తర్భావ <sbi/sbi_pmu.h>
#అన్తర్భావ <sbi/sbi_dbtr.h>
#అన్తర్భావ <sbi/sbi_mpxy.h>
#అన్తర్భావ <sbi/sbi_sse.h>
#అన్తర్భావ <sbi/sbi_system.h>
#అన్తర్భావ <sbi/sbi_string.h>
#అన్తర్భావ <sbi/sbi_timer.h>
#అన్తర్భావ <sbi/sbi_tlb.h>
#అన్తర్భావ <sbi/sbi_version.h>
#అన్తర్భావ <sbi/sbi_unit_test.h>

#నిర్వచించు BANNER                                              \
	"   ____                    _____ ____ _____\n"     \
	"  / __ \\                  / ____|  _ \\_   _|\n"  \
	" | |  | |_ __   ___ _ __ | (___ | |_) || |\n"      \
	" | |  | | '_ \\ / _ \\ '_ \\ \\___ \\|  _ < | |\n" \
	" | |__| | |_) |  __/ | | |____) | |_) || |_\n"     \
	"  \\____/| .__/ \\___|_| |_|_____/|____/_____|\n"  \
	"        | |\n"                                     \
	"        |_|\n\n"

స్థిర శూన్య sbi_boot_print_banner(నిర్మాణం sbi_scratch *scratch)
{
	ఒకవేళ (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		తిరిగి;

#నిర్వచితమైతే OPENSBI_సంస్కరణ_GIT
	sbi_printf("\nOpenSBI %s\n", OPENSBI_సంస్కరణ_GIT);
#లేకపోతే
	sbi_printf("\nOpenSBI v%d.%d\n", OPENSBI_సంస్కరణ_MAJOR,
		   OPENSBI_సంస్కరణ_MINOR);
#అయితే_ముగింపు

#నిర్వచితమైతే OPENSBI_BUILD_సమయం_STAMP
	sbi_printf("Build time: %s\n", OPENSBI_BUILD_సమయం_STAMP);
#అయితే_ముగింపు

#నిర్వచితమైతే OPENSBI_BUILD_COMPILER_సంస్కరణ
	sbi_printf("Build compiler: %s\n", OPENSBI_BUILD_COMPILER_సంస్కరణ);
#అయితే_ముగింపు

	sbi_printf(BANNER);
}

స్థిర శూన్య sbi_boot_print_general(నిర్మాణం sbi_scratch *scratch)
{
	అక్షరం str[128];
	స్థిరరాశి నిర్మాణం sbi_pmu_device *pdev;
	స్థిరరాశి నిర్మాణం sbi_hsm_device *hdev;
	స్థిరరాశి నిర్మాణం sbi_ipi_device *idev;
	స్థిరరాశి నిర్మాణం sbi_timer_device *tdev;
	స్థిరరాశి నిర్మాణం sbi_console_device *cdev;
	స్థిరరాశి నిర్మాణం sbi_system_reset_device *srdev;
	స్థిరరాశి నిర్మాణం sbi_system_suspend_device *susp_dev;
	స్థిరరాశి నిర్మాణం sbi_cppc_device *cppc_dev;
	స్థిరరాశి నిర్మాణం sbi_platform *plat = sbi_platform_ptr(scratch);

	ఒకవేళ (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		తిరిగి;

	/* Platform details */
	sbi_printf("Platform Name               : %s\n",
		   sbi_platform_name(plat));
	sbi_platform_get_features_str(plat, str, పరిమాణం(str));
	sbi_printf("Platform Features           : %s\n", str);
	sbi_printf("Platform HART Count         : %u\n",
		   sbi_platform_hart_లెక్క(plat));
	sbi_hart_protection_get_str(str, పరిమాణం(str));
	sbi_printf("Platform HART Protection    : %s\n", str);
	idev = sbi_ipi_get_device();
	sbi_printf("Platform IPI Device         : %s\n",
		   (idev) ? idev->name : "---");
	tdev = sbi_timer_get_device();
	sbi_printf("Platform Timer Device       : %s @ %luHz\n",
		   (tdev) ? tdev->name : "---",
		   (tdev) ? tdev->timer_freq : 0);
	cdev = sbi_console_get_device();
	sbi_printf("Platform Console Device     : %s\n",
		   (cdev) ? cdev->name : "---");
	hdev = sbi_hsm_get_device();
	sbi_printf("Platform HSM Device         : %s\n",
		   (hdev) ? hdev->name : "---");
	pdev = sbi_pmu_get_device();
	sbi_printf("Platform PMU Device         : %s\n",
		   (pdev) ? pdev->name : "---");
	srdev = sbi_system_reset_get_device(SBI_SRST_RESET_TYPE_COLD_REBOOT, 0);
	sbi_printf("Platform Reboot Device      : %s\n",
		   (srdev) ? srdev->name : "---");
	srdev = sbi_system_reset_get_device(SBI_SRST_RESET_TYPE_SHUTDOWN, 0);
	sbi_printf("Platform Shutdown Device    : %s\n",
		   (srdev) ? srdev->name : "---");
	susp_dev = sbi_system_suspend_get_device();
	sbi_printf("Platform Suspend Device     : %s\n",
		   (susp_dev) ? susp_dev->name : "---");
	cppc_dev = sbi_cppc_get_device();
	sbi_printf("Platform CPPC Device        : %s\n",
		   (cppc_dev) ? cppc_dev->name : "---");

	/* Firmware details */
	sbi_printf("Firmware Base               : 0x%lx\n", scratch->fw_ప్రారంభం);
	sbi_printf("Firmware Size               : %d KB\n",
		   (u32)(scratch->fw_కొలత / 1024));
	sbi_printf("Firmware RW Offset          : 0x%lx\n", scratch->fw_rw_స్థానభ్రంశం);
	sbi_printf("Firmware RW Size            : %d KB\n",
		   (u32)((scratch->fw_కొలత - scratch->fw_rw_స్థానభ్రంశం) / 1024));
	sbi_printf("Firmware Heap Offset        : 0x%lx\n", scratch->fw_కుప్ప_స్థానభ్రంశం);
	sbi_printf("Firmware Heap Size          : "
		   "%d KB (total), %d KB (reserved), %d KB (used), %d KB (free)\n",
		   (u32)(scratch->fw_కుప్ప_కొలత / 1024),
		   (u32)(sbi_కుప్ప_reserved_space() / 1024),
		   (u32)(sbi_కుప్ప_వాడిన_space() / 1024),
		   (u32)(sbi_కుప్ప_free_space() / 1024));
	sbi_printf("Firmware Scratch Size       : "
		   "%d B (total), %d B (used), %d B (free)\n",
		   SBI_SCRATCH_కొలత,
		   (u32)sbi_scratch_వాడిన_space(),
		   (u32)(SBI_SCRATCH_కొలత - sbi_scratch_వాడిన_space()));

	/* SBI details */
	sbi_printf("Runtime SBI Version         : %d.%d\n",
		   sbi_ecall_సంస్కరణ_major(), sbi_ecall_సంస్కరణ_minor());
	sbi_ecall_get_extensions_str(str, పరిమాణం(str), false);
	sbi_printf("Standard SBI Extensions     : %s\n", str);
	sbi_ecall_get_extensions_str(str, పరిమాణం(str), true);
	sbi_printf("Experimental SBI Extensions : %s\n", str);
	sbi_printf("\n");
}

స్థిర శూన్య sbi_boot_print_domains(నిర్మాణం sbi_scratch *scratch)
{
	ఒకవేళ (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		తిరిగి;

	/* Domain details */
	sbi_domain_dump_all("        ");
}

స్థిర శూన్య sbi_boot_print_hart(నిర్మాణం sbi_scratch *scratch, u32 hartid)
{
	పూర్ణాంకం xlen;
	అక్షరం str[256];
	స్థిరరాశి నిర్మాణం sbi_domain *dom = sbi_domain_thishart_ptr();

	ఒకవేళ (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		తిరిగి;

	/* Determine MISA XLEN and MISA string */
	xlen = misa_xlen();
	ఒకవేళ (xlen < 1) {
		sbi_printf("Error %d getting MISA XLEN\n", xlen);
		sbi_hart_hang();
	}

	/* Boot HART details */
	sbi_printf("Boot HART ID                : %u\n", hartid);
	sbi_printf("Boot HART Domain            : %s\n", dom->name);
	sbi_hart_get_priv_సంస్కరణ_str(scratch, str, పరిమాణం(str));
	sbi_printf("Boot HART Priv Version      : %s\n", str);
	misa_string(xlen, str, పరిమాణం(str));
	sbi_printf("Boot HART Base ISA          : %s\n", str);
	sbi_hart_get_extensions_str(scratch, str, పరిమాణం(str));
	sbi_printf("Boot HART ISA Extensions    : %s\n", str);
	sbi_printf("Boot HART PMP Count         : %d\n",
		   sbi_hart_pmp_లెక్క(scratch));
	sbi_printf("Boot HART PMP Granularity   : %u bits\n",
		   sbi_hart_pmp_log2gran(scratch));
	sbi_printf("Boot HART PMP Address Bits  : %d\n",
		   sbi_hart_pmp_addrbits(scratch));
	sbi_printf("Boot HART MHPM Info         : %lu (0x%08x)\n",
		   sbi_popcount(sbi_hart_mhpm_ముసుగు(scratch)),
		   sbi_hart_mhpm_ముసుగు(scratch));
	sbi_printf("Boot HART Debug Triggers    : %d triggers\n",
		   sbi_dbtr_get_మొత్తం_triggers());
	sbi_hart_delegation_dump(scratch, "Boot HART ", "           ");
}

స్థిర గుర్తులేని దీర్ఘ coldboot_పూర్తి;

స్థిర శూన్య wait_for_coldboot(నిర్మాణం sbi_scratch *scratch)
{
	/* Wait for coldboot to finish */
	అయితే (!__smp_load_acquire(&coldboot_పూర్తి))
		cpu_relax();
}

స్థిర శూన్య wake_coldboot_harts(నిర్మాణం sbi_scratch *scratch)
{
	/* Mark coldboot done */
	__smp_store_release(&coldboot_పూర్తి, 1);
}

గుర్తులేని దీర్ఘ __attribute__((weak)) __stack_chk_guard = 0x95B5FF5A;

స్థిర గుర్తులేని దీర్ఘ నమోదు_లెక్క_స్థానభ్రంశం;
స్థిర గుర్తులేని దీర్ఘ init_లెక్క_స్థానభ్రంశం;

స్థిర శూన్య __noreturn init_coldboot(నిర్మాణం sbi_scratch *scratch, u32 hartid)
{
	పూర్ణాంకం rc;
	గుర్తులేని దీర్ఘ *లెక్క;
	స్థిరరాశి నిర్మాణం sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Note: This has to be first thing in coldboot init sequence */
	rc = sbi_scratch_init(scratch);
	ఒకవేళ (rc)
		sbi_hart_hang();

	/* Note: This has to be second thing in coldboot init sequence */
	rc = sbi_కుప్ప_init(scratch);
	ఒకవేళ (rc)
		sbi_hart_hang();

	/* Note: This has to be the third thing in coldboot init sequence */
	rc = sbi_domain_init(scratch, hartid);
	ఒకవేళ (rc)
		sbi_hart_hang();

	నమోదు_లెక్క_స్థానభ్రంశం = sbi_scratch_alloc_స్థానభ్రంశం(__SIZEOF_POINTER__);
	ఒకవేళ (!నమోదు_లెక్క_స్థానభ్రంశం)
		sbi_hart_hang();

	init_లెక్క_స్థానభ్రంశం = sbi_scratch_alloc_స్థానభ్రంశం(__SIZEOF_POINTER__);
	ఒకవేళ (!init_లెక్క_స్థానభ్రంశం)
		sbi_hart_hang();

	లెక్క = sbi_scratch_స్థానభ్రంశం_ptr(scratch, నమోదు_లెక్క_స్థానభ్రంశం);
	(*లెక్క)++;

	rc = sbi_hsm_init(scratch, true);
	ఒకవేళ (rc)
		sbi_hart_hang();

	/*
	 * All non-coldboot HARTs do HSM initialization (i.e. enter HSM state
	 * machine) at the start of the warmboot path so it is wasteful to
	 * have these HARTs busy spin in wait_for_coldboot() until coldboot
	 * path is completed.
	 */
	wake_coldboot_harts(scratch);

	rc = sbi_hart_init(scratch, true);
	ఒకవేళ (rc)
		sbi_hart_hang();

	/*
	 * Initialize stack guard via Zkr entropy source if Zkr is
	 * implemented according to device tree. Writing new seed value
	 * to __stack_chk_guard is safe here because function doesn't
	 * return and no check against value on entry will be done.
	 */
	ఒకవేళ (sbi_hart_has_extension(scratch, SBI_HART_EXT_ZKR)) {
		గుర్తులేని దీర్ఘ guard_val = 0;
		పూర్ణాంకం chunks = పరిమాణం(గుర్తులేని దీర్ఘ) / పరిమాణం(uint16_t);
#నిర్వచితం_కాకపోతే CONFIG_ZKR_POLL_BUDGET
#నిర్వచించు CONFIG_ZKR_POLL_BUDGET		1000
#అయితే_ముగింపు
		గుర్తులేని పూర్ణాంకం tries = CONFIG_ZKR_POLL_BUDGET;
		bool res = false;

		అయితే (chunks && tries) {
			గుర్తులేని దీర్ఘ విత్తనం = csr_మార్పిడి(CSR_విత్తనం, 0);
			గుర్తులేని దీర్ఘ opst = విత్తనం & విత్తనం_OPTS_ముసుగు;
			res = false;

			ఒకవేళ (opst == విత్తనం_OPTS_DEAD) {
				ఆపు;
			}
			ఒకవేళ (opst == విత్తనం_OPTS_ES16) {
				guard_val = (guard_val << 16) | (విత్తనం & విత్తనం_ENTROPY_ముసుగు);
				chunks--;
				res = true;
				/* Successful read doesn't consume a try */
				tries++;
			}

			tries--;
			కొనసాగు;
		}
		ఒకవేళ (res)
			__stack_chk_guard = guard_val;
	}

	rc = sbi_trap_init(scratch, true);
	ఒకవేళ (rc)
		sbi_hart_hang();

	rc = sbi_timer_init(scratch, true);
	ఒకవేళ (rc)
		sbi_hart_hang();

	rc = sbi_platform_early_init(plat, true);
	ఒకవేళ (rc)
		sbi_hart_hang();

	rc = sbi_pmu_init(scratch, true);
	ఒకవేళ (rc) {
		sbi_printf("%s: pmu init failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_dbtr_init(scratch, true);
	ఒకవేళ (rc)
		sbi_hart_hang();

	sbi_boot_print_banner(scratch);

	sbi_double_trap_init(scratch);

	rc = sbi_irqchip_init(scratch, true);
	ఒకవేళ (rc) {
		sbi_printf("%s: irqchip init failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_ipi_init(scratch, true);
	ఒకవేళ (rc) {
		sbi_printf("%s: ipi init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_tlb_init(scratch, true);
	ఒకవేళ (rc) {
		sbi_printf("%s: tlb init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_fwft_init(scratch, true);
	ఒకవేళ (rc) {
		sbi_printf("%s: fwft init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_mpxy_init(scratch);
	ఒకవేళ (rc) {
		sbi_printf("%s: mpxy init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Note: Finalize domains after HSM initialization
	 * Note: Finalize domains before HART PMP configuration so
	 * that we use correct domain for configuring PMP.
	 */
	rc = sbi_domain_finalize(scratch);
	ఒకవేళ (rc) {
		sbi_printf("%s: domain finalize failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Note: Platform final initialization should be after finalizing
	 * domains so that it sees correct domain assignment and PMP
	 * configuration for FDT fixups.
	 */
	rc = sbi_platform_final_init(plat, true);
	ఒకవేళ (rc) {
		sbi_printf("%s: platform final init failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Note: SSE events callbacks can be registered by other drivers so
	 * sbi_sse_init() needs to be called after all drivers have been probed.
	 */
	rc = sbi_sse_init(scratch, true);
	ఒకవేళ (rc) {
		sbi_printf("%s: sse init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Note: Ecall initialization should be after platform final
	 * initialization so that all available platform devices are
	 * already registered.
	 */
	rc = sbi_ecall_init();
	ఒకవేళ (rc) {
		sbi_printf("%s: ecall init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	sbi_boot_print_general(scratch);

	sbi_boot_print_domains(scratch);

	sbi_boot_print_hart(scratch, hartid);

	నిరంతరం_all_tests();

	/*
	 * Note: Startup domains after all initialization are done
	 * otherwise boot HART of non-root domain can crash.
	 */
	rc = sbi_domain_startup(scratch, hartid);
	ఒకవేళ (rc) {
		sbi_printf("%s: domain startup failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Configure hart protection at last because if SMEPMP is
	 * detected, M-mode access to the S/U space will be rescinded.
	 */
	rc = sbi_hart_protection_configure(scratch, sbi_domain_thishart_ptr());
	ఒకవేళ (rc) {
		sbi_printf("%s: hart protection configure failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	లెక్క = sbi_scratch_స్థానభ్రంశం_ptr(scratch, init_లెక్క_స్థానభ్రంశం);
	(*లెక్క)++;

	sbi_hsm_hart_ప్రారంభం_ముగింపు(scratch, hartid);
}

స్థిర శూన్య __noreturn init_warm_startup(నిర్మాణం sbi_scratch *scratch,
					 u32 hartid)
{
	పూర్ణాంకం rc;
	గుర్తులేని దీర్ఘ *లెక్క;
	స్థిరరాశి నిర్మాణం sbi_platform *plat = sbi_platform_ptr(scratch);

	ఒకవేళ (!నమోదు_లెక్క_స్థానభ్రంశం || !init_లెక్క_స్థానభ్రంశం)
		sbi_hart_hang();

	లెక్క = sbi_scratch_స్థానభ్రంశం_ptr(scratch, నమోదు_లెక్క_స్థానభ్రంశం);
	(*లెక్క)++;

	/* Note: This has to be first thing in warmboot init sequence */
	rc = sbi_hsm_init(scratch, false);
	ఒకవేళ (rc)
		sbi_hart_hang();

	rc = sbi_hart_init(scratch, false);
	ఒకవేళ (rc)
		sbi_hart_hang();

	rc = sbi_trap_init(scratch, false);
	ఒకవేళ (rc)
		sbi_hart_hang();

	rc = sbi_timer_init(scratch, false);
	ఒకవేళ (rc)
		sbi_hart_hang();

	rc = sbi_platform_early_init(plat, false);
	ఒకవేళ (rc)
		sbi_hart_hang();

	rc = sbi_pmu_init(scratch, false);
	ఒకవేళ (rc)
		sbi_hart_hang();

	rc = sbi_dbtr_init(scratch, false);
	ఒకవేళ (rc)
		sbi_hart_hang();

	rc = sbi_irqchip_init(scratch, false);
	ఒకవేళ (rc)
		sbi_hart_hang();

	rc = sbi_ipi_init(scratch, false);
	ఒకవేళ (rc)
		sbi_hart_hang();

	rc = sbi_tlb_init(scratch, false);
	ఒకవేళ (rc)
		sbi_hart_hang();

	rc = sbi_fwft_init(scratch, false);
	ఒకవేళ (rc)
		sbi_hart_hang();

	rc = sbi_platform_final_init(plat, false);
	ఒకవేళ (rc)
		sbi_hart_hang();

	rc = sbi_sse_init(scratch, false);
	ఒకవేళ (rc)
		sbi_hart_hang();

	/*
	 * Configure hart protection at last because if SMEPMP is
	 * detected, M-mode access to the S/U space will be rescinded.
	 */
	rc = sbi_hart_protection_configure(scratch, sbi_domain_thishart_ptr());
	ఒకవేళ (rc)
		sbi_hart_hang();

	లెక్క = sbi_scratch_స్థానభ్రంశం_ptr(scratch, init_లెక్క_స్థానభ్రంశం);
	(*లెక్క)++;

	sbi_hsm_hart_ప్రారంభం_ముగింపు(scratch, hartid);
}

స్థిర శూన్య __noreturn init_warm_resume(నిర్మాణం sbi_scratch *scratch,
					u32 hartid)
{
	పూర్ణాంకం rc;

	sbi_hsm_hart_resume_ప్రారంభం(scratch);

	rc = sbi_hart_reinit(scratch);
	ఒకవేళ (rc)
		sbi_hart_hang();

	rc = sbi_hart_protection_configure(scratch, sbi_domain_thishart_ptr());
	ఒకవేళ (rc)
		sbi_hart_hang();

	sbi_hsm_hart_resume_ముగింపు(scratch, hartid);
}

స్థిర శూన్య __noreturn init_warmboot(నిర్మాణం sbi_scratch *scratch, u32 hartid)
{
	పూర్ణాంకం hstate;

	wait_for_coldboot(scratch);

	hstate = sbi_hsm_hart_get_state(sbi_domain_thishart_ptr(), hartid);
	ఒకవేళ (hstate < 0)
		sbi_hart_hang();

	ఒకవేళ (hstate == SBI_HSM_STATE_SUSPENDED) {
		init_warm_resume(scratch, hartid);
	} లేకపోతే {
		sbi_ipi_raw_clear(true);
		init_warm_startup(scratch, hartid);
	}
}

స్థిర atomic_t coldboot_lottery = ATOMIC_INITIALIZER(0);

/**
 * Initialize OpenSBI library for current HART and jump to next
 * booting stage.
 *
 * The function expects following:
 * 1. The 'mscratch' CSR is pointing to sbi_scratch of current HART
 * 2. Stack pointer (SP) is setup for current HART
 * 3. Interrupts are disabled in MSTATUS CSR
 * 4. All interrupts are disabled in MIE CSR
 *
 * @param scratch pointer to sbi_scratch of current HART
 */
శూన్య __noreturn sbi_init(నిర్మాణం sbi_scratch *scratch)
{
	bool తదుపరి_బహుళకం_supported	= false;
	bool coldboot			= false;
	u32 hartid			= current_hartid();
	స్థిరరాశి నిర్మాణం sbi_platform *plat = sbi_platform_ptr(scratch);

	ఎంపిక (scratch->తదుపరి_బహుళకం) {
	స్థితి PRV_M:
		తదుపరి_బహుళకం_supported = true;
		ఆపు;
	స్థితి PRV_S:
		ఒకవేళ (misa_extension('S'))
			తదుపరి_బహుళకం_supported = true;
		ఆపు;
	స్థితి PRV_U:
		ఒకవేళ (misa_extension('U'))
			తదుపరి_బహుళకం_supported = true;
		ఆపు;
	అప్రమేయ:
		sbi_hart_hang();
	}

	/*
	 * Only the HART supporting privilege mode specified in the
	 * scratch->next_mode should be allowed to become the coldboot
	 * HART because the coldboot HART will be directly jumping to
	 * the next booting stage.
	 *
	 * We use a lottery mechanism to select coldboot HART among
	 * HARTs which satisfy above condition.
	 */

	ఒకవేళ (sbi_platform_cold_boot_allowed(plat, hartid)) {
		ఒకవేళ (తదుపరి_బహుళకం_supported &&
		    atomic_xchg(&coldboot_lottery, 1) == 0)
			coldboot = true;
	}

	/*
	 * Do platform specific nascent (very early) initialization so
	 * that platform can initialize platform specific per-HART CSRs
	 * or per-HART devices.
	 */
	ఒకవేళ (sbi_platform_nascent_init(plat))
		sbi_hart_hang();

	ఒకవేళ (coldboot)
		init_coldboot(scratch, hartid);
	లేకపోతే
		init_warmboot(scratch, hartid);
}

శూన్య sbi_revert_నమోదు_లెక్క(నిర్మాణం sbi_scratch *scratch)
{
	గుర్తులేని దీర్ఘ *నమోదు_లెక్క, *init_లెక్క;

	ఒకవేళ (!నమోదు_లెక్క_స్థానభ్రంశం || !init_లెక్క_స్థానభ్రంశం)
		sbi_hart_hang();

	నమోదు_లెక్క = sbi_scratch_స్థానభ్రంశం_ptr(scratch, నమోదు_లెక్క_స్థానభ్రంశం);
	init_లెక్క = sbi_scratch_స్థానభ్రంశం_ptr(scratch, init_లెక్క_స్థానభ్రంశం);

	*నమోదు_లెక్క = *init_లెక్క;
}

గుర్తులేని దీర్ఘ sbi_నమోదు_లెక్క(u32 hartindex)
{
	నిర్మాణం sbi_scratch *scratch;
	గుర్తులేని దీర్ఘ *నమోదు_లెక్క;

	ఒకవేళ (!నమోదు_లెక్క_స్థానభ్రంశం)
		తిరిగి 0;

	scratch = sbi_hartindex_గమ్యం_scratch(hartindex);
	ఒకవేళ (!scratch)
		తిరిగి 0;

	నమోదు_లెక్క = sbi_scratch_స్థానభ్రంశం_ptr(scratch, నమోదు_లెక్క_స్థానభ్రంశం);

	తిరిగి *నమోదు_లెక్క;
}

గుర్తులేని దీర్ఘ sbi_init_లెక్క(u32 hartindex)
{
	నిర్మాణం sbi_scratch *scratch;
	గుర్తులేని దీర్ఘ *init_లెక్క;

	ఒకవేళ (!init_లెక్క_స్థానభ్రంశం)
		తిరిగి 0;

	scratch = sbi_hartindex_గమ్యం_scratch(hartindex);
	ఒకవేళ (!scratch)
		తిరిగి 0;

	init_లెక్క = sbi_scratch_స్థానభ్రంశం_ptr(scratch, init_లెక్క_స్థానభ్రంశం);

	తిరిగి *init_లెక్క;
}

/**
 * Exit OpenSBI library for current HART and stop HART
 *
 * The function expects following:
 * 1. The 'mscratch' CSR is pointing to sbi_scratch of current HART
 * 2. Stack pointer (SP) is setup for current HART
 *
 * @param scratch pointer to sbi_scratch of current HART
 */
శూన్య __noreturn sbi_exit(నిర్మాణం sbi_scratch *scratch)
{
	u32 hartid			= current_hartid();
	స్థిరరాశి నిర్మాణం sbi_platform *plat = sbi_platform_ptr(scratch);

	ఒకవేళ (!sbi_hartid_valid(hartid))
		sbi_hart_hang();

	sbi_platform_early_exit(plat);

	sbi_sse_exit(scratch);

	sbi_pmu_exit(scratch);

	sbi_timer_exit(scratch);

	sbi_ipi_exit(scratch);

	sbi_irqchip_exit(scratch);

	sbi_platform_final_exit(plat);

	sbi_hsm_exit(scratch);
}
