/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_atomic.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_barrier.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_cppc.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_double_trap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_fwft.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hartmask.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart_pmp.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart_protection.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hsm.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ipi.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_irqchip.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_platform.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_pmu.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_dbtr.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_mpxy.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_sse.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_system.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_timer.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_tlb.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_version.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_unit_test.h>

#ವ್ಯಾಖ್ಯಾನಿಸು BANNER                                              \
	"   ____                    _____ ____ _____\n"     \
	"  / __ \\                  / ____|  _ \\_   _|\n"  \
	" | |  | |_ __   ___ _ __ | (___ | |_) || |\n"      \
	" | |  | | '_ \\ / _ \\ '_ \\ \\___ \\|  _ < | |\n" \
	" | |__| | |_) |  __/ | | |____) | |_) || |_\n"     \
	"  \\____/| .__/ \\___|_| |_|_____/|____/_____|\n"  \
	"        | |\n"                                     \
	"        |_|\n\n"

ಸ್ಥಿರ ಶೂನ್ಯ sbi_boot_print_banner(ರಚನೆ sbi_scratch *scratch)
{
	ಆದರೆ (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ OPENSBI_ಆವೃತ್ತಿ_GIT
	sbi_printf("\nOpenSBI %s\n", OPENSBI_ಆವೃತ್ತಿ_GIT);
#ಇಲ್ಲದಿದ್ದರೆ
	sbi_printf("\nOpenSBI v%d.%d\n", OPENSBI_ಆವೃತ್ತಿ_MAJOR,
		   OPENSBI_ಆವೃತ್ತಿ_MINOR);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ OPENSBI_BUILD_ಸಮಯ_STAMP
	sbi_printf("Build time: %s\n", OPENSBI_BUILD_ಸಮಯ_STAMP);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ OPENSBI_BUILD_COMPILER_ಆವೃತ್ತಿ
	sbi_printf("Build compiler: %s\n", OPENSBI_BUILD_COMPILER_ಆವೃತ್ತಿ);
#ಆದರೆ_ಅಂತ್ಯ

	sbi_printf(BANNER);
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_boot_print_general(ರಚನೆ sbi_scratch *scratch)
{
	ಅಕ್ಷರ str[128];
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_pmu_device *pdev;
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_hsm_device *hdev;
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_ipi_device *idev;
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_timer_device *tdev;
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_console_device *cdev;
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_system_reset_device *srdev;
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_system_suspend_device *susp_dev;
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_cppc_device *cppc_dev;
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_platform *plat = sbi_platform_ptr(scratch);

	ಆದರೆ (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		ಹಿಂತಿರುಗಿ;

	/* Platform details */
	sbi_printf("Platform Name               : %s\n",
		   sbi_platform_name(plat));
	sbi_platform_get_features_str(plat, str, ಗಾತ್ರ(str));
	sbi_printf("Platform Features           : %s\n", str);
	sbi_printf("Platform HART Count         : %u\n",
		   sbi_platform_hart_ಎಣಿಕೆ(plat));
	sbi_hart_protection_get_str(str, ಗಾತ್ರ(str));
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
	sbi_printf("Firmware Base               : 0x%lx\n", scratch->fw_ಆರಂಭ);
	sbi_printf("Firmware Size               : %d KB\n",
		   (u32)(scratch->fw_ಗಾತ್ರ / 1024));
	sbi_printf("Firmware RW Offset          : 0x%lx\n", scratch->fw_rw_ಸ್ಥಾನಾಂತರ);
	sbi_printf("Firmware RW Size            : %d KB\n",
		   (u32)((scratch->fw_ಗಾತ್ರ - scratch->fw_rw_ಸ್ಥಾನಾಂತರ) / 1024));
	sbi_printf("Firmware Heap Offset        : 0x%lx\n", scratch->fw_ರಾಶಿ_ಸ್ಥಾನಾಂತರ);
	sbi_printf("Firmware Heap Size          : "
		   "%d KB (total), %d KB (reserved), %d KB (used), %d KB (free)\n",
		   (u32)(scratch->fw_ರಾಶಿ_ಗಾತ್ರ / 1024),
		   (u32)(sbi_ರಾಶಿ_reserved_space() / 1024),
		   (u32)(sbi_ರಾಶಿ_ಬಳಸಿದ_space() / 1024),
		   (u32)(sbi_ರಾಶಿ_free_space() / 1024));
	sbi_printf("Firmware Scratch Size       : "
		   "%d B (total), %d B (used), %d B (free)\n",
		   SBI_SCRATCH_ಗಾತ್ರ,
		   (u32)sbi_scratch_ಬಳಸಿದ_space(),
		   (u32)(SBI_SCRATCH_ಗಾತ್ರ - sbi_scratch_ಬಳಸಿದ_space()));

	/* SBI details */
	sbi_printf("Runtime SBI Version         : %d.%d\n",
		   sbi_ecall_ಆವೃತ್ತಿ_major(), sbi_ecall_ಆವೃತ್ತಿ_minor());
	sbi_ecall_get_extensions_str(str, ಗಾತ್ರ(str), false);
	sbi_printf("Standard SBI Extensions     : %s\n", str);
	sbi_ecall_get_extensions_str(str, ಗಾತ್ರ(str), true);
	sbi_printf("Experimental SBI Extensions : %s\n", str);
	sbi_printf("\n");
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_boot_print_domains(ರಚನೆ sbi_scratch *scratch)
{
	ಆದರೆ (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		ಹಿಂತಿರುಗಿ;

	/* Domain details */
	sbi_domain_dump_all("        ");
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_boot_print_hart(ರಚನೆ sbi_scratch *scratch, u32 hartid)
{
	ಪೂರ್ಣಾಂಕ xlen;
	ಅಕ್ಷರ str[256];
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain *dom = sbi_domain_thishart_ptr();

	ಆದರೆ (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		ಹಿಂತಿರುಗಿ;

	/* Determine MISA XLEN and MISA string */
	xlen = misa_xlen();
	ಆದರೆ (xlen < 1) {
		sbi_printf("Error %d getting MISA XLEN\n", xlen);
		sbi_hart_hang();
	}

	/* Boot HART details */
	sbi_printf("Boot HART ID                : %u\n", hartid);
	sbi_printf("Boot HART Domain            : %s\n", dom->name);
	sbi_hart_get_priv_ಆವೃತ್ತಿ_str(scratch, str, ಗಾತ್ರ(str));
	sbi_printf("Boot HART Priv Version      : %s\n", str);
	misa_string(xlen, str, ಗಾತ್ರ(str));
	sbi_printf("Boot HART Base ISA          : %s\n", str);
	sbi_hart_get_extensions_str(scratch, str, ಗಾತ್ರ(str));
	sbi_printf("Boot HART ISA Extensions    : %s\n", str);
	sbi_printf("Boot HART PMP Count         : %d\n",
		   sbi_hart_pmp_ಎಣಿಕೆ(scratch));
	sbi_printf("Boot HART PMP Granularity   : %u bits\n",
		   sbi_hart_pmp_log2gran(scratch));
	sbi_printf("Boot HART PMP Address Bits  : %d\n",
		   sbi_hart_pmp_addrbits(scratch));
	sbi_printf("Boot HART MHPM Info         : %lu (0x%08x)\n",
		   sbi_popcount(sbi_hart_mhpm_ಮುಖವಾಡ(scratch)),
		   sbi_hart_mhpm_ಮುಖವಾಡ(scratch));
	sbi_printf("Boot HART Debug Triggers    : %d triggers\n",
		   sbi_dbtr_get_ಒಟ್ಟು_triggers());
	sbi_hart_delegation_dump(scratch, "Boot HART ", "           ");
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ coldboot_ಮುಗಿದ;

ಸ್ಥಿರ ಶೂನ್ಯ wait_for_coldboot(ರಚನೆ sbi_scratch *scratch)
{
	/* Wait for coldboot to finish */
	ಆಗ (!__smp_load_acquire(&coldboot_ಮುಗಿದ))
		cpu_relax();
}

ಸ್ಥಿರ ಶೂನ್ಯ wake_coldboot_harts(ರಚನೆ sbi_scratch *scratch)
{
	/* Mark coldboot done */
	__smp_store_release(&coldboot_ಮುಗಿದ, 1);
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ __attribute__((weak)) __stack_chk_guard = 0x95B5FF5A;

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ನಮೂದು_ಎಣಿಕೆ_ಸ್ಥಾನಾಂತರ;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ init_ಎಣಿಕೆ_ಸ್ಥಾನಾಂತರ;

ಸ್ಥಿರ ಶೂನ್ಯ __noreturn init_coldboot(ರಚನೆ sbi_scratch *scratch, u32 hartid)
{
	ಪೂರ್ಣಾಂಕ rc;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಎಣಿಕೆ_2;
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Note: This has to be first thing in coldboot init sequence */
	rc = sbi_scratch_init(scratch);
	ಆದರೆ (rc)
		sbi_hart_hang();

	/* Note: This has to be second thing in coldboot init sequence */
	rc = sbi_ರಾಶಿ_init(scratch);
	ಆದರೆ (rc)
		sbi_hart_hang();

	/* Note: This has to be the third thing in coldboot init sequence */
	rc = sbi_domain_init(scratch, hartid);
	ಆದರೆ (rc)
		sbi_hart_hang();

	ನಮೂದು_ಎಣಿಕೆ_ಸ್ಥಾನಾಂತರ = sbi_scratch_alloc_ಸ್ಥಾನಾಂತರ(__SIZEOF_POINTER__);
	ಆದರೆ (!ನಮೂದು_ಎಣಿಕೆ_ಸ್ಥಾನಾಂತರ)
		sbi_hart_hang();

	init_ಎಣಿಕೆ_ಸ್ಥಾನಾಂತರ = sbi_scratch_alloc_ಸ್ಥಾನಾಂತರ(__SIZEOF_POINTER__);
	ಆದರೆ (!init_ಎಣಿಕೆ_ಸ್ಥಾನಾಂತರ)
		sbi_hart_hang();

	ಎಣಿಕೆ_2 = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, ನಮೂದು_ಎಣಿಕೆ_ಸ್ಥಾನಾಂತರ);
	(*ಎಣಿಕೆ_2)++;

	rc = sbi_hsm_init(scratch, true);
	ಆದರೆ (rc)
		sbi_hart_hang();

	/*
	 * All non-coldboot HARTs do HSM initialization (i.e. enter HSM state
	 * machine) at the start of the warmboot path so it is wasteful to
	 * have these HARTs busy spin in wait_for_coldboot() until coldboot
	 * path is completed.
	 */
	wake_coldboot_harts(scratch);

	rc = sbi_hart_init(scratch, true);
	ಆದರೆ (rc)
		sbi_hart_hang();

	/*
	 * Initialize stack guard via Zkr entropy source if Zkr is
	 * implemented according to device tree. Writing new seed value
	 * to __stack_chk_guard is safe here because function doesn't
	 * return and no check against value on entry will be done.
	 */
	ಆದರೆ (sbi_hart_has_extension(scratch, SBI_HART_EXT_ZKR)) {
		ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ guard_val = 0;
		ಪೂರ್ಣಾಂಕ chunks = ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) / ಗಾತ್ರ(uint16_t);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ZKR_POLL_BUDGET
#ವ್ಯಾಖ್ಯಾನಿಸು CONFIG_ZKR_POLL_BUDGET		1000
#ಆದರೆ_ಅಂತ್ಯ
		ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ tries = CONFIG_ZKR_POLL_BUDGET;
		bool res = false;

		ಆಗ (chunks && tries) {
			ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಬೀಜ = csr_ಅದಲುಬದಲು(CSR_ಬೀಜ, 0);
			ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ opst = ಬೀಜ & ಬೀಜ_OPTS_ಮುಖವಾಡ;
			res = false;

			ಆದರೆ (opst == ಬೀಜ_OPTS_DEAD) {
				ನಿಲ್ಲಿಸು;
			}
			ಆದರೆ (opst == ಬೀಜ_OPTS_ES16) {
				guard_val = (guard_val << 16) | (ಬೀಜ & ಬೀಜ_ENTROPY_ಮುಖವಾಡ);
				chunks--;
				res = true;
				/* Successful read doesn't consume a try */
				tries++;
			}

			tries--;
			ಮುಂದುವರಿ;
		}
		ಆದರೆ (res)
			__stack_chk_guard = guard_val;
	}

	rc = sbi_trap_init(scratch, true);
	ಆದರೆ (rc)
		sbi_hart_hang();

	rc = sbi_timer_init(scratch, true);
	ಆದರೆ (rc)
		sbi_hart_hang();

	rc = sbi_platform_early_init(plat, true);
	ಆದರೆ (rc)
		sbi_hart_hang();

	rc = sbi_pmu_init(scratch, true);
	ಆದರೆ (rc) {
		sbi_printf("%s: pmu init failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_dbtr_init(scratch, true);
	ಆದರೆ (rc)
		sbi_hart_hang();

	sbi_boot_print_banner(scratch);

	sbi_double_trap_init(scratch);

	rc = sbi_irqchip_init(scratch, true);
	ಆದರೆ (rc) {
		sbi_printf("%s: irqchip init failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_ipi_init(scratch, true);
	ಆದರೆ (rc) {
		sbi_printf("%s: ipi init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_tlb_init(scratch, true);
	ಆದರೆ (rc) {
		sbi_printf("%s: tlb init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_fwft_init(scratch, true);
	ಆದರೆ (rc) {
		sbi_printf("%s: fwft init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_mpxy_init(scratch);
	ಆದರೆ (rc) {
		sbi_printf("%s: mpxy init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Note: Finalize domains after HSM initialization
	 * Note: Finalize domains before HART PMP configuration so
	 * that we use correct domain for configuring PMP.
	 */
	rc = sbi_domain_finalize(scratch);
	ಆದರೆ (rc) {
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
	ಆದರೆ (rc) {
		sbi_printf("%s: platform final init failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Note: SSE events callbacks can be registered by other drivers so
	 * sbi_sse_init() needs to be called after all drivers have been probed.
	 */
	rc = sbi_sse_init(scratch, true);
	ಆದರೆ (rc) {
		sbi_printf("%s: sse init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Note: Ecall initialization should be after platform final
	 * initialization so that all available platform devices are
	 * already registered.
	 */
	rc = sbi_ecall_init();
	ಆದರೆ (rc) {
		sbi_printf("%s: ecall init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	sbi_boot_print_general(scratch);

	sbi_boot_print_domains(scratch);

	sbi_boot_print_hart(scratch, hartid);

	ಸತತ_all_tests();

	/*
	 * Note: Startup domains after all initialization are done
	 * otherwise boot HART of non-root domain can crash.
	 */
	rc = sbi_domain_startup(scratch, hartid);
	ಆದರೆ (rc) {
		sbi_printf("%s: domain startup failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Configure hart protection at last because if SMEPMP is
	 * detected, M-mode access to the S/U space will be rescinded.
	 */
	rc = sbi_hart_protection_configure(scratch, sbi_domain_thishart_ptr());
	ಆದರೆ (rc) {
		sbi_printf("%s: hart protection configure failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	ಎಣಿಕೆ_2 = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, init_ಎಣಿಕೆ_ಸ್ಥಾನಾಂತರ);
	(*ಎಣಿಕೆ_2)++;

	sbi_hsm_hart_ಆರಂಭ_ಮುಕ್ತಾಯ(scratch, hartid);
}

ಸ್ಥಿರ ಶೂನ್ಯ __noreturn init_warm_startup(ರಚನೆ sbi_scratch *scratch,
					 u32 hartid)
{
	ಪೂರ್ಣಾಂಕ rc;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಎಣಿಕೆ_2;
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_platform *plat = sbi_platform_ptr(scratch);

	ಆದರೆ (!ನಮೂದು_ಎಣಿಕೆ_ಸ್ಥಾನಾಂತರ || !init_ಎಣಿಕೆ_ಸ್ಥಾನಾಂತರ)
		sbi_hart_hang();

	ಎಣಿಕೆ_2 = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, ನಮೂದು_ಎಣಿಕೆ_ಸ್ಥಾನಾಂತರ);
	(*ಎಣಿಕೆ_2)++;

	/* Note: This has to be first thing in warmboot init sequence */
	rc = sbi_hsm_init(scratch, false);
	ಆದರೆ (rc)
		sbi_hart_hang();

	rc = sbi_hart_init(scratch, false);
	ಆದರೆ (rc)
		sbi_hart_hang();

	rc = sbi_trap_init(scratch, false);
	ಆದರೆ (rc)
		sbi_hart_hang();

	rc = sbi_timer_init(scratch, false);
	ಆದರೆ (rc)
		sbi_hart_hang();

	rc = sbi_platform_early_init(plat, false);
	ಆದರೆ (rc)
		sbi_hart_hang();

	rc = sbi_pmu_init(scratch, false);
	ಆದರೆ (rc)
		sbi_hart_hang();

	rc = sbi_dbtr_init(scratch, false);
	ಆದರೆ (rc)
		sbi_hart_hang();

	rc = sbi_irqchip_init(scratch, false);
	ಆದರೆ (rc)
		sbi_hart_hang();

	rc = sbi_ipi_init(scratch, false);
	ಆದರೆ (rc)
		sbi_hart_hang();

	rc = sbi_tlb_init(scratch, false);
	ಆದರೆ (rc)
		sbi_hart_hang();

	rc = sbi_fwft_init(scratch, false);
	ಆದರೆ (rc)
		sbi_hart_hang();

	rc = sbi_platform_final_init(plat, false);
	ಆದರೆ (rc)
		sbi_hart_hang();

	rc = sbi_sse_init(scratch, false);
	ಆದರೆ (rc)
		sbi_hart_hang();

	/*
	 * Configure hart protection at last because if SMEPMP is
	 * detected, M-mode access to the S/U space will be rescinded.
	 */
	rc = sbi_hart_protection_configure(scratch, sbi_domain_thishart_ptr());
	ಆದರೆ (rc)
		sbi_hart_hang();

	ಎಣಿಕೆ_2 = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, init_ಎಣಿಕೆ_ಸ್ಥಾನಾಂತರ);
	(*ಎಣಿಕೆ_2)++;

	sbi_hsm_hart_ಆರಂಭ_ಮುಕ್ತಾಯ(scratch, hartid);
}

ಸ್ಥಿರ ಶೂನ್ಯ __noreturn init_warm_resume(ರಚನೆ sbi_scratch *scratch,
					u32 hartid)
{
	ಪೂರ್ಣಾಂಕ rc;

	sbi_hsm_hart_resume_ಆರಂಭ(scratch);

	rc = sbi_hart_reinit(scratch);
	ಆದರೆ (rc)
		sbi_hart_hang();

	rc = sbi_hart_protection_configure(scratch, sbi_domain_thishart_ptr());
	ಆದರೆ (rc)
		sbi_hart_hang();

	sbi_hsm_hart_resume_ಮುಕ್ತಾಯ(scratch, hartid);
}

ಸ್ಥಿರ ಶೂನ್ಯ __noreturn init_warmboot(ರಚನೆ sbi_scratch *scratch, u32 hartid)
{
	ಪೂರ್ಣಾಂಕ hstate;

	wait_for_coldboot(scratch);

	hstate = sbi_hsm_hart_get_state(sbi_domain_thishart_ptr(), hartid);
	ಆದರೆ (hstate < 0)
		sbi_hart_hang();

	ಆದರೆ (hstate == SBI_HSM_STATE_SUSPENDED) {
		init_warm_resume(scratch, hartid);
	} ಇಲ್ಲದಿದ್ದರೆ {
		sbi_ipi_raw_clear(true);
		init_warm_startup(scratch, hartid);
	}
}

ಸ್ಥಿರ atomic_t coldboot_lottery = ATOMIC_INITIALIZER(0);

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
ಶೂನ್ಯ __noreturn sbi_init(ರಚನೆ sbi_scratch *scratch)
{
	bool ಮುಂದಿನ_ಬಹುಲಕ_supported	= false;
	bool coldboot			= false;
	u32 hartid			= current_hartid();
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_platform *plat = sbi_platform_ptr(scratch);

	ಆಯ್ಕೆ (scratch->ಮುಂದಿನ_ಬಹುಲಕ) {
	ಸ್ಥಿತಿ PRV_M:
		ಮುಂದಿನ_ಬಹುಲಕ_supported = true;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PRV_S:
		ಆದರೆ (misa_extension('S'))
			ಮುಂದಿನ_ಬಹುಲಕ_supported = true;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PRV_U:
		ಆದರೆ (misa_extension('U'))
			ಮುಂದಿನ_ಬಹುಲಕ_supported = true;
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
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

	ಆದರೆ (sbi_platform_cold_boot_allowed(plat, hartid)) {
		ಆದರೆ (ಮುಂದಿನ_ಬಹುಲಕ_supported &&
		    atomic_xchg(&coldboot_lottery, 1) == 0)
			coldboot = true;
	}

	/*
	 * Do platform specific nascent (very early) initialization so
	 * that platform can initialize platform specific per-HART CSRs
	 * or per-HART devices.
	 */
	ಆದರೆ (sbi_platform_nascent_init(plat))
		sbi_hart_hang();

	ಆದರೆ (coldboot)
		init_coldboot(scratch, hartid);
	ಇಲ್ಲದಿದ್ದರೆ
		init_warmboot(scratch, hartid);
}

ಶೂನ್ಯ sbi_revert_ನಮೂದು_ಎಣಿಕೆ(ರಚನೆ sbi_scratch *scratch)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ನಮೂದು_ಎಣಿಕೆ, *init_ಎಣಿಕೆ;

	ಆದರೆ (!ನಮೂದು_ಎಣಿಕೆ_ಸ್ಥಾನಾಂತರ || !init_ಎಣಿಕೆ_ಸ್ಥಾನಾಂತರ)
		sbi_hart_hang();

	ನಮೂದು_ಎಣಿಕೆ = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, ನಮೂದು_ಎಣಿಕೆ_ಸ್ಥಾನಾಂತರ);
	init_ಎಣಿಕೆ = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, init_ಎಣಿಕೆ_ಸ್ಥಾನಾಂತರ);

	*ನಮೂದು_ಎಣಿಕೆ = *init_ಎಣಿಕೆ;
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sbi_ನಮೂದು_ಎಣಿಕೆ(u32 hartindex)
{
	ರಚನೆ sbi_scratch *scratch;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ನಮೂದು_ಎಣಿಕೆ;

	ಆದರೆ (!ನಮೂದು_ಎಣಿಕೆ_ಸ್ಥಾನಾಂತರ)
		ಹಿಂತಿರುಗಿ 0;

	scratch = sbi_hartindex_ಗಮ್ಯ_scratch(hartindex);
	ಆದರೆ (!scratch)
		ಹಿಂತಿರುಗಿ 0;

	ನಮೂದು_ಎಣಿಕೆ = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, ನಮೂದು_ಎಣಿಕೆ_ಸ್ಥಾನಾಂತರ);

	ಹಿಂತಿರುಗಿ *ನಮೂದು_ಎಣಿಕೆ;
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sbi_init_ಎಣಿಕೆ(u32 hartindex)
{
	ರಚನೆ sbi_scratch *scratch;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *init_ಎಣಿಕೆ;

	ಆದರೆ (!init_ಎಣಿಕೆ_ಸ್ಥಾನಾಂತರ)
		ಹಿಂತಿರುಗಿ 0;

	scratch = sbi_hartindex_ಗಮ್ಯ_scratch(hartindex);
	ಆದರೆ (!scratch)
		ಹಿಂತಿರುಗಿ 0;

	init_ಎಣಿಕೆ = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, init_ಎಣಿಕೆ_ಸ್ಥಾನಾಂತರ);

	ಹಿಂತಿರುಗಿ *init_ಎಣಿಕೆ;
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
ಶೂನ್ಯ __noreturn sbi_exit(ರಚನೆ sbi_scratch *scratch)
{
	u32 hartid			= current_hartid();
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_platform *plat = sbi_platform_ptr(scratch);

	ಆದರೆ (!sbi_hartid_valid(hartid))
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
