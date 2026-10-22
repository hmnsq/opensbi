/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_atomic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_barrier.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_cppc.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_double_trap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_fwft.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hartmask.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart_pmp.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart_protection.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hsm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ipi.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_irqchip.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_platform.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_pmu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_dbtr.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_mpxy.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_sse.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_system.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_tlb.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_version.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_unit_test.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BANNER                                              \
	"   ____                    _____ ____ _____\n"     \
	"  / __ \\                  / ____|  _ \\_   _|\n"  \
	" | |  | |_ __   ___ _ __ | (___ | |_) || |\n"      \
	" | |  | | '_ \\ / _ \\ '_ \\ \\___ \\|  _ < | |\n" \
	" | |__| | |_) |  __/ | | |____) | |_) || |_\n"     \
	"  \\____/| .__/ \\___|_| |_|_____/|____/_____|\n"  \
	"        | |\n"                                     \
	"        |_|\n\n"

ਸਥਿਰ ਖਾਲੀ sbi_boot_print_banner(ਬਣਤਰ sbi_scratch *scratch)
{
	ਜੇ (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ OPENSBI_ਸੰਸਕਰਣ_GIT
	sbi_printf("\nOpenSBI %s\n", OPENSBI_ਸੰਸਕਰਣ_GIT);
#ਨਹੀਂਤਾਂ
	sbi_printf("\nOpenSBI v%d.%d\n", OPENSBI_ਸੰਸਕਰਣ_MAJOR,
		   OPENSBI_ਸੰਸਕਰਣ_MINOR);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ OPENSBI_BUILD_ਸਮਾਂ_STAMP
	sbi_printf("Build time: %s\n", OPENSBI_BUILD_ਸਮਾਂ_STAMP);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ OPENSBI_BUILD_COMPILER_ਸੰਸਕਰਣ
	sbi_printf("Build compiler: %s\n", OPENSBI_BUILD_COMPILER_ਸੰਸਕਰਣ);
#ਜੇ_ਅੰਤ

	sbi_printf(BANNER);
}

ਸਥਿਰ ਖਾਲੀ sbi_boot_print_general(ਬਣਤਰ sbi_scratch *scratch)
{
	ਅੱਖਰ str[128];
	ਅਚਲ ਬਣਤਰ sbi_pmu_device *pdev;
	ਅਚਲ ਬਣਤਰ sbi_hsm_device *hdev;
	ਅਚਲ ਬਣਤਰ sbi_ipi_device *idev;
	ਅਚਲ ਬਣਤਰ sbi_timer_device *tdev;
	ਅਚਲ ਬਣਤਰ sbi_console_device *cdev;
	ਅਚਲ ਬਣਤਰ sbi_system_reset_device *srdev;
	ਅਚਲ ਬਣਤਰ sbi_system_suspend_device *susp_dev;
	ਅਚਲ ਬਣਤਰ sbi_cppc_device *cppc_dev;
	ਅਚਲ ਬਣਤਰ sbi_platform *plat = sbi_platform_ptr(scratch);

	ਜੇ (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		ਵਾਪਸ;

	/* Platform details */
	sbi_printf("Platform Name               : %s\n",
		   sbi_platform_name(plat));
	sbi_platform_get_features_str(plat, str, ਆਕਾਰ(str));
	sbi_printf("Platform Features           : %s\n", str);
	sbi_printf("Platform HART Count         : %u\n",
		   sbi_platform_hart_ਗਿਣਤੀ(plat));
	sbi_hart_protection_get_str(str, ਆਕਾਰ(str));
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
	sbi_printf("Firmware Base               : 0x%lx\n", scratch->fw_ਸ਼ੁਰੂ);
	sbi_printf("Firmware Size               : %d KB\n",
		   (u32)(scratch->fw_ਮਾਪ / 1024));
	sbi_printf("Firmware RW Offset          : 0x%lx\n", scratch->fw_rw_ਵਿੱਥ);
	sbi_printf("Firmware RW Size            : %d KB\n",
		   (u32)((scratch->fw_ਮਾਪ - scratch->fw_rw_ਵਿੱਥ) / 1024));
	sbi_printf("Firmware Heap Offset        : 0x%lx\n", scratch->fw_ਢੇਰ_ਵਿੱਥ);
	sbi_printf("Firmware Heap Size          : "
		   "%d KB (total), %d KB (reserved), %d KB (used), %d KB (free)\n",
		   (u32)(scratch->fw_ਢੇਰ_ਮਾਪ / 1024),
		   (u32)(sbi_ਢੇਰ_reserved_space() / 1024),
		   (u32)(sbi_ਢੇਰ_ਵਰਤਿਆ_space() / 1024),
		   (u32)(sbi_ਢੇਰ_free_space() / 1024));
	sbi_printf("Firmware Scratch Size       : "
		   "%d B (total), %d B (used), %d B (free)\n",
		   SBI_SCRATCH_ਮਾਪ,
		   (u32)sbi_scratch_ਵਰਤਿਆ_space(),
		   (u32)(SBI_SCRATCH_ਮਾਪ - sbi_scratch_ਵਰਤਿਆ_space()));

	/* SBI details */
	sbi_printf("Runtime SBI Version         : %d.%d\n",
		   sbi_ecall_ਸੰਸਕਰਣ_major(), sbi_ecall_ਸੰਸਕਰਣ_minor());
	sbi_ecall_get_extensions_str(str, ਆਕਾਰ(str), false);
	sbi_printf("Standard SBI Extensions     : %s\n", str);
	sbi_ecall_get_extensions_str(str, ਆਕਾਰ(str), true);
	sbi_printf("Experimental SBI Extensions : %s\n", str);
	sbi_printf("\n");
}

ਸਥਿਰ ਖਾਲੀ sbi_boot_print_domains(ਬਣਤਰ sbi_scratch *scratch)
{
	ਜੇ (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		ਵਾਪਸ;

	/* Domain details */
	sbi_domain_dump_all("        ");
}

ਸਥਿਰ ਖਾਲੀ sbi_boot_print_hart(ਬਣਤਰ sbi_scratch *scratch, u32 hartid)
{
	ਪੂਰਨ xlen;
	ਅੱਖਰ str[256];
	ਅਚਲ ਬਣਤਰ sbi_domain *dom = sbi_domain_thishart_ptr();

	ਜੇ (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		ਵਾਪਸ;

	/* Determine MISA XLEN and MISA string */
	xlen = misa_xlen();
	ਜੇ (xlen < 1) {
		sbi_printf("Error %d getting MISA XLEN\n", xlen);
		sbi_hart_hang();
	}

	/* Boot HART details */
	sbi_printf("Boot HART ID                : %u\n", hartid);
	sbi_printf("Boot HART Domain            : %s\n", dom->name);
	sbi_hart_get_priv_ਸੰਸਕਰਣ_str(scratch, str, ਆਕਾਰ(str));
	sbi_printf("Boot HART Priv Version      : %s\n", str);
	misa_string(xlen, str, ਆਕਾਰ(str));
	sbi_printf("Boot HART Base ISA          : %s\n", str);
	sbi_hart_get_extensions_str(scratch, str, ਆਕਾਰ(str));
	sbi_printf("Boot HART ISA Extensions    : %s\n", str);
	sbi_printf("Boot HART PMP Count         : %d\n",
		   sbi_hart_pmp_ਗਿਣਤੀ(scratch));
	sbi_printf("Boot HART PMP Granularity   : %u bits\n",
		   sbi_hart_pmp_log2gran(scratch));
	sbi_printf("Boot HART PMP Address Bits  : %d\n",
		   sbi_hart_pmp_addrbits(scratch));
	sbi_printf("Boot HART MHPM Info         : %lu (0x%08x)\n",
		   sbi_popcount(sbi_hart_mhpm_ਨਕਾਬ(scratch)),
		   sbi_hart_mhpm_ਨਕਾਬ(scratch));
	sbi_printf("Boot HART Debug Triggers    : %d triggers\n",
		   sbi_dbtr_get_ਕੁੱਲ_triggers());
	sbi_hart_delegation_dump(scratch, "Boot HART ", "           ");
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ coldboot_ਪੂਰਾ;

ਸਥਿਰ ਖਾਲੀ wait_for_coldboot(ਬਣਤਰ sbi_scratch *scratch)
{
	/* Wait for coldboot to finish */
	ਜਦੋਂ (!__smp_load_acquire(&coldboot_ਪੂਰਾ))
		cpu_relax();
}

ਸਥਿਰ ਖਾਲੀ wake_coldboot_harts(ਬਣਤਰ sbi_scratch *scratch)
{
	/* Mark coldboot done */
	__smp_store_release(&coldboot_ਪੂਰਾ, 1);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ __attribute__((weak)) __stack_chk_guard = 0x95B5FF5A;

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਇੰਦਰਾਜ_ਗਿਣਤੀ_ਵਿੱਥ;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ init_ਗਿਣਤੀ_ਵਿੱਥ;

ਸਥਿਰ ਖਾਲੀ __noreturn init_coldboot(ਬਣਤਰ sbi_scratch *scratch, u32 hartid)
{
	ਪੂਰਨ rc;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਗਿਣਤੀ_2;
	ਅਚਲ ਬਣਤਰ sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Note: This has to be first thing in coldboot init sequence */
	rc = sbi_scratch_init(scratch);
	ਜੇ (rc)
		sbi_hart_hang();

	/* Note: This has to be second thing in coldboot init sequence */
	rc = sbi_ਢੇਰ_init(scratch);
	ਜੇ (rc)
		sbi_hart_hang();

	/* Note: This has to be the third thing in coldboot init sequence */
	rc = sbi_domain_init(scratch, hartid);
	ਜੇ (rc)
		sbi_hart_hang();

	ਇੰਦਰਾਜ_ਗਿਣਤੀ_ਵਿੱਥ = sbi_scratch_alloc_ਵਿੱਥ(__SIZEOF_POINTER__);
	ਜੇ (!ਇੰਦਰਾਜ_ਗਿਣਤੀ_ਵਿੱਥ)
		sbi_hart_hang();

	init_ਗਿਣਤੀ_ਵਿੱਥ = sbi_scratch_alloc_ਵਿੱਥ(__SIZEOF_POINTER__);
	ਜੇ (!init_ਗਿਣਤੀ_ਵਿੱਥ)
		sbi_hart_hang();

	ਗਿਣਤੀ_2 = sbi_scratch_ਵਿੱਥ_ptr(scratch, ਇੰਦਰਾਜ_ਗਿਣਤੀ_ਵਿੱਥ);
	(*ਗਿਣਤੀ_2)++;

	rc = sbi_hsm_init(scratch, true);
	ਜੇ (rc)
		sbi_hart_hang();

	/*
	 * All non-coldboot HARTs do HSM initialization (i.e. enter HSM state
	 * machine) at the start of the warmboot path so it is wasteful to
	 * have these HARTs busy spin in wait_for_coldboot() until coldboot
	 * path is completed.
	 */
	wake_coldboot_harts(scratch);

	rc = sbi_hart_init(scratch, true);
	ਜੇ (rc)
		sbi_hart_hang();

	/*
	 * Initialize stack guard via Zkr entropy source if Zkr is
	 * implemented according to device tree. Writing new seed value
	 * to __stack_chk_guard is safe here because function doesn't
	 * return and no check against value on entry will be done.
	 */
	ਜੇ (sbi_hart_has_extension(scratch, SBI_HART_EXT_ZKR)) {
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ guard_val = 0;
		ਪੂਰਨ chunks = ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) / ਆਕਾਰ(uint16_t);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ZKR_POLL_BUDGET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONFIG_ZKR_POLL_BUDGET		1000
#ਜੇ_ਅੰਤ
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ tries = CONFIG_ZKR_POLL_BUDGET;
		bool res = false;

		ਜਦੋਂ (chunks && tries) {
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਬੀਜ = csr_ਅਦਲਾ_ਬਦਲੀ(CSR_ਬੀਜ, 0);
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ opst = ਬੀਜ & ਬੀਜ_OPTS_ਨਕਾਬ;
			res = false;

			ਜੇ (opst == ਬੀਜ_OPTS_DEAD) {
				ਰੋਕੋ;
			}
			ਜੇ (opst == ਬੀਜ_OPTS_ES16) {
				guard_val = (guard_val << 16) | (ਬੀਜ & ਬੀਜ_ENTROPY_ਨਕਾਬ);
				chunks--;
				res = true;
				/* Successful read doesn't consume a try */
				tries++;
			}

			tries--;
			ਜਾਰੀ;
		}
		ਜੇ (res)
			__stack_chk_guard = guard_val;
	}

	rc = sbi_trap_init(scratch, true);
	ਜੇ (rc)
		sbi_hart_hang();

	rc = sbi_timer_init(scratch, true);
	ਜੇ (rc)
		sbi_hart_hang();

	rc = sbi_platform_early_init(plat, true);
	ਜੇ (rc)
		sbi_hart_hang();

	rc = sbi_pmu_init(scratch, true);
	ਜੇ (rc) {
		sbi_printf("%s: pmu init failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_dbtr_init(scratch, true);
	ਜੇ (rc)
		sbi_hart_hang();

	sbi_boot_print_banner(scratch);

	sbi_double_trap_init(scratch);

	rc = sbi_irqchip_init(scratch, true);
	ਜੇ (rc) {
		sbi_printf("%s: irqchip init failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_ipi_init(scratch, true);
	ਜੇ (rc) {
		sbi_printf("%s: ipi init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_tlb_init(scratch, true);
	ਜੇ (rc) {
		sbi_printf("%s: tlb init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_fwft_init(scratch, true);
	ਜੇ (rc) {
		sbi_printf("%s: fwft init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_mpxy_init(scratch);
	ਜੇ (rc) {
		sbi_printf("%s: mpxy init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Note: Finalize domains after HSM initialization
	 * Note: Finalize domains before HART PMP configuration so
	 * that we use correct domain for configuring PMP.
	 */
	rc = sbi_domain_finalize(scratch);
	ਜੇ (rc) {
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
	ਜੇ (rc) {
		sbi_printf("%s: platform final init failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Note: SSE events callbacks can be registered by other drivers so
	 * sbi_sse_init() needs to be called after all drivers have been probed.
	 */
	rc = sbi_sse_init(scratch, true);
	ਜੇ (rc) {
		sbi_printf("%s: sse init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Note: Ecall initialization should be after platform final
	 * initialization so that all available platform devices are
	 * already registered.
	 */
	rc = sbi_ecall_init();
	ਜੇ (rc) {
		sbi_printf("%s: ecall init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	sbi_boot_print_general(scratch);

	sbi_boot_print_domains(scratch);

	sbi_boot_print_hart(scratch, hartid);

	ਲੜੀਵਾਰ_all_tests();

	/*
	 * Note: Startup domains after all initialization are done
	 * otherwise boot HART of non-root domain can crash.
	 */
	rc = sbi_domain_startup(scratch, hartid);
	ਜੇ (rc) {
		sbi_printf("%s: domain startup failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Configure hart protection at last because if SMEPMP is
	 * detected, M-mode access to the S/U space will be rescinded.
	 */
	rc = sbi_hart_protection_configure(scratch, sbi_domain_thishart_ptr());
	ਜੇ (rc) {
		sbi_printf("%s: hart protection configure failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	ਗਿਣਤੀ_2 = sbi_scratch_ਵਿੱਥ_ptr(scratch, init_ਗਿਣਤੀ_ਵਿੱਥ);
	(*ਗਿਣਤੀ_2)++;

	sbi_hsm_hart_ਸ਼ੁਰੂ_ਅੰਤ(scratch, hartid);
}

ਸਥਿਰ ਖਾਲੀ __noreturn init_warm_startup(ਬਣਤਰ sbi_scratch *scratch,
					 u32 hartid)
{
	ਪੂਰਨ rc;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਗਿਣਤੀ_2;
	ਅਚਲ ਬਣਤਰ sbi_platform *plat = sbi_platform_ptr(scratch);

	ਜੇ (!ਇੰਦਰਾਜ_ਗਿਣਤੀ_ਵਿੱਥ || !init_ਗਿਣਤੀ_ਵਿੱਥ)
		sbi_hart_hang();

	ਗਿਣਤੀ_2 = sbi_scratch_ਵਿੱਥ_ptr(scratch, ਇੰਦਰਾਜ_ਗਿਣਤੀ_ਵਿੱਥ);
	(*ਗਿਣਤੀ_2)++;

	/* Note: This has to be first thing in warmboot init sequence */
	rc = sbi_hsm_init(scratch, false);
	ਜੇ (rc)
		sbi_hart_hang();

	rc = sbi_hart_init(scratch, false);
	ਜੇ (rc)
		sbi_hart_hang();

	rc = sbi_trap_init(scratch, false);
	ਜੇ (rc)
		sbi_hart_hang();

	rc = sbi_timer_init(scratch, false);
	ਜੇ (rc)
		sbi_hart_hang();

	rc = sbi_platform_early_init(plat, false);
	ਜੇ (rc)
		sbi_hart_hang();

	rc = sbi_pmu_init(scratch, false);
	ਜੇ (rc)
		sbi_hart_hang();

	rc = sbi_dbtr_init(scratch, false);
	ਜੇ (rc)
		sbi_hart_hang();

	rc = sbi_irqchip_init(scratch, false);
	ਜੇ (rc)
		sbi_hart_hang();

	rc = sbi_ipi_init(scratch, false);
	ਜੇ (rc)
		sbi_hart_hang();

	rc = sbi_tlb_init(scratch, false);
	ਜੇ (rc)
		sbi_hart_hang();

	rc = sbi_fwft_init(scratch, false);
	ਜੇ (rc)
		sbi_hart_hang();

	rc = sbi_platform_final_init(plat, false);
	ਜੇ (rc)
		sbi_hart_hang();

	rc = sbi_sse_init(scratch, false);
	ਜੇ (rc)
		sbi_hart_hang();

	/*
	 * Configure hart protection at last because if SMEPMP is
	 * detected, M-mode access to the S/U space will be rescinded.
	 */
	rc = sbi_hart_protection_configure(scratch, sbi_domain_thishart_ptr());
	ਜੇ (rc)
		sbi_hart_hang();

	ਗਿਣਤੀ_2 = sbi_scratch_ਵਿੱਥ_ptr(scratch, init_ਗਿਣਤੀ_ਵਿੱਥ);
	(*ਗਿਣਤੀ_2)++;

	sbi_hsm_hart_ਸ਼ੁਰੂ_ਅੰਤ(scratch, hartid);
}

ਸਥਿਰ ਖਾਲੀ __noreturn init_warm_resume(ਬਣਤਰ sbi_scratch *scratch,
					u32 hartid)
{
	ਪੂਰਨ rc;

	sbi_hsm_hart_resume_ਸ਼ੁਰੂ(scratch);

	rc = sbi_hart_reinit(scratch);
	ਜੇ (rc)
		sbi_hart_hang();

	rc = sbi_hart_protection_configure(scratch, sbi_domain_thishart_ptr());
	ਜੇ (rc)
		sbi_hart_hang();

	sbi_hsm_hart_resume_ਅੰਤ(scratch, hartid);
}

ਸਥਿਰ ਖਾਲੀ __noreturn init_warmboot(ਬਣਤਰ sbi_scratch *scratch, u32 hartid)
{
	ਪੂਰਨ hstate;

	wait_for_coldboot(scratch);

	hstate = sbi_hsm_hart_get_state(sbi_domain_thishart_ptr(), hartid);
	ਜੇ (hstate < 0)
		sbi_hart_hang();

	ਜੇ (hstate == SBI_HSM_STATE_SUSPENDED) {
		init_warm_resume(scratch, hartid);
	} ਨਹੀਂਤਾਂ {
		sbi_ipi_raw_clear(true);
		init_warm_startup(scratch, hartid);
	}
}

ਸਥਿਰ atomic_t coldboot_lottery = ATOMIC_INITIALIZER(0);

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
ਖਾਲੀ __noreturn sbi_init(ਬਣਤਰ sbi_scratch *scratch)
{
	bool ਅਗਲਾ_ਬਹੁਲਕ_supported	= false;
	bool coldboot			= false;
	u32 hartid			= current_hartid();
	ਅਚਲ ਬਣਤਰ sbi_platform *plat = sbi_platform_ptr(scratch);

	ਚੋਣ (scratch->ਅਗਲਾ_ਬਹੁਲਕ) {
	ਹਾਲਤ PRV_M:
		ਅਗਲਾ_ਬਹੁਲਕ_supported = true;
		ਰੋਕੋ;
	ਹਾਲਤ PRV_S:
		ਜੇ (misa_extension('S'))
			ਅਗਲਾ_ਬਹੁਲਕ_supported = true;
		ਰੋਕੋ;
	ਹਾਲਤ PRV_U:
		ਜੇ (misa_extension('U'))
			ਅਗਲਾ_ਬਹੁਲਕ_supported = true;
		ਰੋਕੋ;
	ਮੂਲ:
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

	ਜੇ (sbi_platform_cold_boot_allowed(plat, hartid)) {
		ਜੇ (ਅਗਲਾ_ਬਹੁਲਕ_supported &&
		    atomic_xchg(&coldboot_lottery, 1) == 0)
			coldboot = true;
	}

	/*
	 * Do platform specific nascent (very early) initialization so
	 * that platform can initialize platform specific per-HART CSRs
	 * or per-HART devices.
	 */
	ਜੇ (sbi_platform_nascent_init(plat))
		sbi_hart_hang();

	ਜੇ (coldboot)
		init_coldboot(scratch, hartid);
	ਨਹੀਂਤਾਂ
		init_warmboot(scratch, hartid);
}

ਖਾਲੀ sbi_revert_ਇੰਦਰਾਜ_ਗਿਣਤੀ(ਬਣਤਰ sbi_scratch *scratch)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਇੰਦਰਾਜ_ਗਿਣਤੀ, *init_ਗਿਣਤੀ;

	ਜੇ (!ਇੰਦਰਾਜ_ਗਿਣਤੀ_ਵਿੱਥ || !init_ਗਿਣਤੀ_ਵਿੱਥ)
		sbi_hart_hang();

	ਇੰਦਰਾਜ_ਗਿਣਤੀ = sbi_scratch_ਵਿੱਥ_ptr(scratch, ਇੰਦਰਾਜ_ਗਿਣਤੀ_ਵਿੱਥ);
	init_ਗਿਣਤੀ = sbi_scratch_ਵਿੱਥ_ptr(scratch, init_ਗਿਣਤੀ_ਵਿੱਥ);

	*ਇੰਦਰਾਜ_ਗਿਣਤੀ = *init_ਗਿਣਤੀ;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sbi_ਇੰਦਰਾਜ_ਗਿਣਤੀ(u32 hartindex)
{
	ਬਣਤਰ sbi_scratch *scratch;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਇੰਦਰਾਜ_ਗਿਣਤੀ;

	ਜੇ (!ਇੰਦਰਾਜ_ਗਿਣਤੀ_ਵਿੱਥ)
		ਵਾਪਸ 0;

	scratch = sbi_hartindex_ਮੰਜ਼ਿਲ_scratch(hartindex);
	ਜੇ (!scratch)
		ਵਾਪਸ 0;

	ਇੰਦਰਾਜ_ਗਿਣਤੀ = sbi_scratch_ਵਿੱਥ_ptr(scratch, ਇੰਦਰਾਜ_ਗਿਣਤੀ_ਵਿੱਥ);

	ਵਾਪਸ *ਇੰਦਰਾਜ_ਗਿਣਤੀ;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sbi_init_ਗਿਣਤੀ(u32 hartindex)
{
	ਬਣਤਰ sbi_scratch *scratch;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *init_ਗਿਣਤੀ;

	ਜੇ (!init_ਗਿਣਤੀ_ਵਿੱਥ)
		ਵਾਪਸ 0;

	scratch = sbi_hartindex_ਮੰਜ਼ਿਲ_scratch(hartindex);
	ਜੇ (!scratch)
		ਵਾਪਸ 0;

	init_ਗਿਣਤੀ = sbi_scratch_ਵਿੱਥ_ptr(scratch, init_ਗਿਣਤੀ_ਵਿੱਥ);

	ਵਾਪਸ *init_ਗਿਣਤੀ;
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
ਖਾਲੀ __noreturn sbi_exit(ਬਣਤਰ sbi_scratch *scratch)
{
	u32 hartid			= current_hartid();
	ਅਚਲ ਬਣਤਰ sbi_platform *plat = sbi_platform_ptr(scratch);

	ਜੇ (!sbi_hartid_valid(hartid))
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
