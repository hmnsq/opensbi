/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/riscv_atomic.h>
#അന്തര്ഭാവ <sbi/riscv_barrier.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_cppc.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_double_trap.h>
#അന്തര്ഭാവ <sbi/sbi_ecall.h>
#അന്തര്ഭാവ <sbi/sbi_fwft.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_hartmask.h>
#അന്തര്ഭാവ <sbi/sbi_hart_pmp.h>
#അന്തര്ഭാവ <sbi/sbi_hart_protection.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi/sbi_hsm.h>
#അന്തര്ഭാവ <sbi/sbi_ipi.h>
#അന്തര്ഭാവ <sbi/sbi_irqchip.h>
#അന്തര്ഭാവ <sbi/sbi_platform.h>
#അന്തര്ഭാവ <sbi/sbi_pmu.h>
#അന്തര്ഭാവ <sbi/sbi_dbtr.h>
#അന്തര്ഭാവ <sbi/sbi_mpxy.h>
#അന്തര്ഭാവ <sbi/sbi_sse.h>
#അന്തര്ഭാവ <sbi/sbi_system.h>
#അന്തര്ഭാവ <sbi/sbi_string.h>
#അന്തര്ഭാവ <sbi/sbi_timer.h>
#അന്തര്ഭാവ <sbi/sbi_tlb.h>
#അന്തര്ഭാവ <sbi/sbi_version.h>
#അന്തര്ഭാവ <sbi/sbi_unit_test.h>

#define BANNER                                              \
	"   ____                    _____ ____ _____\n"     \
	"  / __ \\                  / ____|  _ \\_   _|\n"  \
	" | |  | |_ __   ___ _ __ | (___ | |_) || |\n"      \
	" | |  | | '_ \\ / _ \\ '_ \\ \\___ \\|  _ < | |\n" \
	" | |__| | |_) |  __/ | | |____) | |_) || |_\n"     \
	"  \\____/| .__/ \\___|_| |_|_____/|____/_____|\n"  \
	"        | |\n"                                     \
	"        |_|\n\n"

സ്ഥിതം ശൂന്യം sbi_boot_print_banner(ഘടന sbi_scratch *scratch)
{
	എങ്കിൽ (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		തിരികെ;

#ifdef OPENSBI_VERSION_GIT
	sbi_printf("\nOpenSBI %s\n", OPENSBI_VERSION_GIT);
#അല്ലെങ്കിൽ
	sbi_printf("\nOpenSBI v%d.%d\n", OPENSBI_VERSION_MAJOR,
		   OPENSBI_VERSION_MINOR);
#endif

#ifdef OPENSBI_BUILD_TIME_STAMP
	sbi_printf("Build time: %s\n", OPENSBI_BUILD_TIME_STAMP);
#endif

#ifdef OPENSBI_BUILD_COMPILER_VERSION
	sbi_printf("Build compiler: %s\n", OPENSBI_BUILD_COMPILER_VERSION);
#endif

	sbi_printf(BANNER);
}

സ്ഥിതം ശൂന്യം sbi_boot_print_general(ഘടന sbi_scratch *scratch)
{
	അക്ഷരം str[128];
	സ്ഥിരം ഘടന sbi_pmu_device *pdev;
	സ്ഥിരം ഘടന sbi_hsm_device *hdev;
	സ്ഥിരം ഘടന sbi_ipi_device *idev;
	സ്ഥിരം ഘടന sbi_timer_device *tdev;
	സ്ഥിരം ഘടന sbi_console_device *cdev;
	സ്ഥിരം ഘടന sbi_system_reset_device *srdev;
	സ്ഥിരം ഘടന sbi_system_suspend_device *susp_dev;
	സ്ഥിരം ഘടന sbi_cppc_device *cppc_dev;
	സ്ഥിരം ഘടന sbi_platform *plat = sbi_platform_ptr(scratch);

	എങ്കിൽ (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		തിരികെ;

	/* Platform details */
	sbi_printf("Platform Name               : %s\n",
		   sbi_platform_name(plat));
	sbi_platform_get_features_str(plat, str, വലുപ്പം(str));
	sbi_printf("Platform Features           : %s\n", str);
	sbi_printf("Platform HART Count         : %u\n",
		   sbi_platform_hart_എണ്ണം(plat));
	sbi_hart_protection_get_str(str, വലുപ്പം(str));
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
	sbi_printf("Firmware Base               : 0x%lx\n", scratch->fw_start);
	sbi_printf("Firmware Size               : %d KB\n",
		   (u32)(scratch->fw_size / 1024));
	sbi_printf("Firmware RW Offset          : 0x%lx\n", scratch->fw_rw_offset);
	sbi_printf("Firmware RW Size            : %d KB\n",
		   (u32)((scratch->fw_size - scratch->fw_rw_offset) / 1024));
	sbi_printf("Firmware Heap Offset        : 0x%lx\n", scratch->fw_heap_offset);
	sbi_printf("Firmware Heap Size          : "
		   "%d KB (total), %d KB (reserved), %d KB (used), %d KB (free)\n",
		   (u32)(scratch->fw_heap_size / 1024),
		   (u32)(sbi_heap_reserved_space() / 1024),
		   (u32)(sbi_heap_used_space() / 1024),
		   (u32)(sbi_heap_free_space() / 1024));
	sbi_printf("Firmware Scratch Size       : "
		   "%d B (total), %d B (used), %d B (free)\n",
		   SBI_SCRATCH_SIZE,
		   (u32)sbi_scratch_used_space(),
		   (u32)(SBI_SCRATCH_SIZE - sbi_scratch_used_space()));

	/* SBI details */
	sbi_printf("Runtime SBI Version         : %d.%d\n",
		   sbi_ecall_version_major(), sbi_ecall_version_minor());
	sbi_ecall_get_extensions_str(str, വലുപ്പം(str), false);
	sbi_printf("Standard SBI Extensions     : %s\n", str);
	sbi_ecall_get_extensions_str(str, വലുപ്പം(str), true);
	sbi_printf("Experimental SBI Extensions : %s\n", str);
	sbi_printf("\n");
}

സ്ഥിതം ശൂന്യം sbi_boot_print_domains(ഘടന sbi_scratch *scratch)
{
	എങ്കിൽ (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		തിരികെ;

	/* Domain details */
	sbi_domain_dump_all("        ");
}

സ്ഥിതം ശൂന്യം sbi_boot_print_hart(ഘടന sbi_scratch *scratch, u32 hartid)
{
	പൂർണ്ണസംഖ്യ xlen;
	അക്ഷരം str[256];
	സ്ഥിരം ഘടന sbi_domain *dom = sbi_domain_thishart_ptr();

	എങ്കിൽ (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		തിരികെ;

	/* Determine MISA XLEN and MISA string */
	xlen = misa_xlen();
	എങ്കിൽ (xlen < 1) {
		sbi_printf("Error %d getting MISA XLEN\n", xlen);
		sbi_hart_hang();
	}

	/* Boot HART details */
	sbi_printf("Boot HART ID                : %u\n", hartid);
	sbi_printf("Boot HART Domain            : %s\n", dom->name);
	sbi_hart_get_priv_version_str(scratch, str, വലുപ്പം(str));
	sbi_printf("Boot HART Priv Version      : %s\n", str);
	misa_string(xlen, str, വലുപ്പം(str));
	sbi_printf("Boot HART Base ISA          : %s\n", str);
	sbi_hart_get_extensions_str(scratch, str, വലുപ്പം(str));
	sbi_printf("Boot HART ISA Extensions    : %s\n", str);
	sbi_printf("Boot HART PMP Count         : %d\n",
		   sbi_hart_pmp_എണ്ണം(scratch));
	sbi_printf("Boot HART PMP Granularity   : %u bits\n",
		   sbi_hart_pmp_log2gran(scratch));
	sbi_printf("Boot HART PMP Address Bits  : %d\n",
		   sbi_hart_pmp_addrbits(scratch));
	sbi_printf("Boot HART MHPM Info         : %lu (0x%08x)\n",
		   sbi_popcount(sbi_hart_mhpm_mask(scratch)),
		   sbi_hart_mhpm_mask(scratch));
	sbi_printf("Boot HART Debug Triggers    : %d triggers\n",
		   sbi_dbtr_get_total_triggers());
	sbi_hart_delegation_dump(scratch, "Boot HART ", "           ");
}

സ്ഥിതം unsigned long coldboot_done;

സ്ഥിതം ശൂന്യം wait_for_coldboot(ഘടന sbi_scratch *scratch)
{
	/* Wait for coldboot to finish */
	ആകുമ്പോൾ (!__smp_load_acquire(&coldboot_done))
		cpu_relax();
}

സ്ഥിതം ശൂന്യം wake_coldboot_harts(ഘടന sbi_scratch *scratch)
{
	/* Mark coldboot done */
	__smp_store_release(&coldboot_done, 1);
}

unsigned long __attribute__((weak)) __stack_chk_guard = 0x95B5FF5A;

സ്ഥിതം unsigned long entry_എണ്ണം_offset;
സ്ഥിതം unsigned long init_എണ്ണം_offset;

സ്ഥിതം ശൂന്യം __noreturn init_coldboot(ഘടന sbi_scratch *scratch, u32 hartid)
{
	പൂർണ്ണസംഖ്യ rc;
	unsigned long *എണ്ണം;
	സ്ഥിരം ഘടന sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Note: This has to be first thing in coldboot init sequence */
	rc = sbi_scratch_init(scratch);
	എങ്കിൽ (rc)
		sbi_hart_hang();

	/* Note: This has to be second thing in coldboot init sequence */
	rc = sbi_heap_init(scratch);
	എങ്കിൽ (rc)
		sbi_hart_hang();

	/* Note: This has to be the third thing in coldboot init sequence */
	rc = sbi_domain_init(scratch, hartid);
	എങ്കിൽ (rc)
		sbi_hart_hang();

	entry_എണ്ണം_offset = sbi_scratch_alloc_offset(__SIZEOF_POINTER__);
	എങ്കിൽ (!entry_എണ്ണം_offset)
		sbi_hart_hang();

	init_എണ്ണം_offset = sbi_scratch_alloc_offset(__SIZEOF_POINTER__);
	എങ്കിൽ (!init_എണ്ണം_offset)
		sbi_hart_hang();

	എണ്ണം = sbi_scratch_offset_ptr(scratch, entry_എണ്ണം_offset);
	(*എണ്ണം)++;

	rc = sbi_hsm_init(scratch, true);
	എങ്കിൽ (rc)
		sbi_hart_hang();

	/*
	 * All non-coldboot HARTs do HSM initialization (i.e. enter HSM state
	 * machine) at the start of the warmboot path so it is wasteful to
	 * have these HARTs busy spin in wait_for_coldboot() until coldboot
	 * path is completed.
	 */
	wake_coldboot_harts(scratch);

	rc = sbi_hart_init(scratch, true);
	എങ്കിൽ (rc)
		sbi_hart_hang();

	/*
	 * Initialize stack guard via Zkr entropy source if Zkr is
	 * implemented according to device tree. Writing new seed value
	 * to __stack_chk_guard is safe here because function doesn't
	 * return and no check against value on entry will be done.
	 */
	എങ്കിൽ (sbi_hart_has_extension(scratch, SBI_HART_EXT_ZKR)) {
		unsigned long guard_val = 0;
		പൂർണ്ണസംഖ്യ chunks = വലുപ്പം(unsigned long) / വലുപ്പം(uint16_t);
#ifndef CONFIG_ZKR_POLL_BUDGET
#define CONFIG_ZKR_POLL_BUDGET		1000
#endif
		unsigned പൂർണ്ണസംഖ്യ tries = CONFIG_ZKR_POLL_BUDGET;
		bool res = false;

		ആകുമ്പോൾ (chunks && tries) {
			unsigned long seed = csr_swap(CSR_SEED, 0);
			unsigned long opst = seed & SEED_OPTS_MASK;
			res = false;

			എങ്കിൽ (opst == SEED_OPTS_DEAD) {
				നിർത്തുക;
			}
			എങ്കിൽ (opst == SEED_OPTS_ES16) {
				guard_val = (guard_val << 16) | (seed & SEED_ENTROPY_MASK);
				chunks--;
				res = true;
				/* Successful read doesn't consume a try */
				tries++;
			}

			tries--;
			തുടരുക;
		}
		എങ്കിൽ (res)
			__stack_chk_guard = guard_val;
	}

	rc = sbi_trap_init(scratch, true);
	എങ്കിൽ (rc)
		sbi_hart_hang();

	rc = sbi_timer_init(scratch, true);
	എങ്കിൽ (rc)
		sbi_hart_hang();

	rc = sbi_platform_early_init(plat, true);
	എങ്കിൽ (rc)
		sbi_hart_hang();

	rc = sbi_pmu_init(scratch, true);
	എങ്കിൽ (rc) {
		sbi_printf("%s: pmu init failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_dbtr_init(scratch, true);
	എങ്കിൽ (rc)
		sbi_hart_hang();

	sbi_boot_print_banner(scratch);

	sbi_double_trap_init(scratch);

	rc = sbi_irqchip_init(scratch, true);
	എങ്കിൽ (rc) {
		sbi_printf("%s: irqchip init failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_ipi_init(scratch, true);
	എങ്കിൽ (rc) {
		sbi_printf("%s: ipi init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_tlb_init(scratch, true);
	എങ്കിൽ (rc) {
		sbi_printf("%s: tlb init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_fwft_init(scratch, true);
	എങ്കിൽ (rc) {
		sbi_printf("%s: fwft init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_mpxy_init(scratch);
	എങ്കിൽ (rc) {
		sbi_printf("%s: mpxy init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Note: Finalize domains after HSM initialization
	 * Note: Finalize domains before HART PMP configuration so
	 * that we use correct domain for configuring PMP.
	 */
	rc = sbi_domain_finalize(scratch);
	എങ്കിൽ (rc) {
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
	എങ്കിൽ (rc) {
		sbi_printf("%s: platform final init failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Note: SSE events callbacks can be registered by other drivers so
	 * sbi_sse_init() needs to be called after all drivers have been probed.
	 */
	rc = sbi_sse_init(scratch, true);
	എങ്കിൽ (rc) {
		sbi_printf("%s: sse init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Note: Ecall initialization should be after platform final
	 * initialization so that all available platform devices are
	 * already registered.
	 */
	rc = sbi_ecall_init();
	എങ്കിൽ (rc) {
		sbi_printf("%s: ecall init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	sbi_boot_print_general(scratch);

	sbi_boot_print_domains(scratch);

	sbi_boot_print_hart(scratch, hartid);

	run_all_tests();

	/*
	 * Note: Startup domains after all initialization are done
	 * otherwise boot HART of non-root domain can crash.
	 */
	rc = sbi_domain_startup(scratch, hartid);
	എങ്കിൽ (rc) {
		sbi_printf("%s: domain startup failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Configure hart protection at last because if SMEPMP is
	 * detected, M-mode access to the S/U space will be rescinded.
	 */
	rc = sbi_hart_protection_configure(scratch, sbi_domain_thishart_ptr());
	എങ്കിൽ (rc) {
		sbi_printf("%s: hart protection configure failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	എണ്ണം = sbi_scratch_offset_ptr(scratch, init_എണ്ണം_offset);
	(*എണ്ണം)++;

	sbi_hsm_hart_start_finish(scratch, hartid);
}

സ്ഥിതം ശൂന്യം __noreturn init_warm_startup(ഘടന sbi_scratch *scratch,
					 u32 hartid)
{
	പൂർണ്ണസംഖ്യ rc;
	unsigned long *എണ്ണം;
	സ്ഥിരം ഘടന sbi_platform *plat = sbi_platform_ptr(scratch);

	എങ്കിൽ (!entry_എണ്ണം_offset || !init_എണ്ണം_offset)
		sbi_hart_hang();

	എണ്ണം = sbi_scratch_offset_ptr(scratch, entry_എണ്ണം_offset);
	(*എണ്ണം)++;

	/* Note: This has to be first thing in warmboot init sequence */
	rc = sbi_hsm_init(scratch, false);
	എങ്കിൽ (rc)
		sbi_hart_hang();

	rc = sbi_hart_init(scratch, false);
	എങ്കിൽ (rc)
		sbi_hart_hang();

	rc = sbi_trap_init(scratch, false);
	എങ്കിൽ (rc)
		sbi_hart_hang();

	rc = sbi_timer_init(scratch, false);
	എങ്കിൽ (rc)
		sbi_hart_hang();

	rc = sbi_platform_early_init(plat, false);
	എങ്കിൽ (rc)
		sbi_hart_hang();

	rc = sbi_pmu_init(scratch, false);
	എങ്കിൽ (rc)
		sbi_hart_hang();

	rc = sbi_dbtr_init(scratch, false);
	എങ്കിൽ (rc)
		sbi_hart_hang();

	rc = sbi_irqchip_init(scratch, false);
	എങ്കിൽ (rc)
		sbi_hart_hang();

	rc = sbi_ipi_init(scratch, false);
	എങ്കിൽ (rc)
		sbi_hart_hang();

	rc = sbi_tlb_init(scratch, false);
	എങ്കിൽ (rc)
		sbi_hart_hang();

	rc = sbi_fwft_init(scratch, false);
	എങ്കിൽ (rc)
		sbi_hart_hang();

	rc = sbi_platform_final_init(plat, false);
	എങ്കിൽ (rc)
		sbi_hart_hang();

	rc = sbi_sse_init(scratch, false);
	എങ്കിൽ (rc)
		sbi_hart_hang();

	/*
	 * Configure hart protection at last because if SMEPMP is
	 * detected, M-mode access to the S/U space will be rescinded.
	 */
	rc = sbi_hart_protection_configure(scratch, sbi_domain_thishart_ptr());
	എങ്കിൽ (rc)
		sbi_hart_hang();

	എണ്ണം = sbi_scratch_offset_ptr(scratch, init_എണ്ണം_offset);
	(*എണ്ണം)++;

	sbi_hsm_hart_start_finish(scratch, hartid);
}

സ്ഥിതം ശൂന്യം __noreturn init_warm_resume(ഘടന sbi_scratch *scratch,
					u32 hartid)
{
	പൂർണ്ണസംഖ്യ rc;

	sbi_hsm_hart_resume_start(scratch);

	rc = sbi_hart_reinit(scratch);
	എങ്കിൽ (rc)
		sbi_hart_hang();

	rc = sbi_hart_protection_configure(scratch, sbi_domain_thishart_ptr());
	എങ്കിൽ (rc)
		sbi_hart_hang();

	sbi_hsm_hart_resume_finish(scratch, hartid);
}

സ്ഥിതം ശൂന്യം __noreturn init_warmboot(ഘടന sbi_scratch *scratch, u32 hartid)
{
	പൂർണ്ണസംഖ്യ hstate;

	wait_for_coldboot(scratch);

	hstate = sbi_hsm_hart_get_state(sbi_domain_thishart_ptr(), hartid);
	എങ്കിൽ (hstate < 0)
		sbi_hart_hang();

	എങ്കിൽ (hstate == SBI_HSM_STATE_SUSPENDED) {
		init_warm_resume(scratch, hartid);
	} അല്ലെങ്കിൽ {
		sbi_ipi_raw_clear(true);
		init_warm_startup(scratch, hartid);
	}
}

സ്ഥിതം atomic_t coldboot_lottery = ATOMIC_INITIALIZER(0);

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
ശൂന്യം __noreturn sbi_init(ഘടന sbi_scratch *scratch)
{
	bool next_mode_supported	= false;
	bool coldboot			= false;
	u32 hartid			= current_hartid();
	സ്ഥിരം ഘടന sbi_platform *plat = sbi_platform_ptr(scratch);

	തിരഞ്ഞെടുപ്പ് (scratch->next_mode) {
	അവസ്ഥ PRV_M:
		next_mode_supported = true;
		നിർത്തുക;
	അവസ്ഥ PRV_S:
		എങ്കിൽ (misa_extension('S'))
			next_mode_supported = true;
		നിർത്തുക;
	അവസ്ഥ PRV_U:
		എങ്കിൽ (misa_extension('U'))
			next_mode_supported = true;
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
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

	എങ്കിൽ (sbi_platform_cold_boot_allowed(plat, hartid)) {
		എങ്കിൽ (next_mode_supported &&
		    atomic_xchg(&coldboot_lottery, 1) == 0)
			coldboot = true;
	}

	/*
	 * Do platform specific nascent (very early) initialization so
	 * that platform can initialize platform specific per-HART CSRs
	 * or per-HART devices.
	 */
	എങ്കിൽ (sbi_platform_nascent_init(plat))
		sbi_hart_hang();

	എങ്കിൽ (coldboot)
		init_coldboot(scratch, hartid);
	അല്ലെങ്കിൽ
		init_warmboot(scratch, hartid);
}

ശൂന്യം sbi_revert_entry_എണ്ണം(ഘടന sbi_scratch *scratch)
{
	unsigned long *entry_എണ്ണം, *init_എണ്ണം;

	എങ്കിൽ (!entry_എണ്ണം_offset || !init_എണ്ണം_offset)
		sbi_hart_hang();

	entry_എണ്ണം = sbi_scratch_offset_ptr(scratch, entry_എണ്ണം_offset);
	init_എണ്ണം = sbi_scratch_offset_ptr(scratch, init_എണ്ണം_offset);

	*entry_എണ്ണം = *init_എണ്ണം;
}

unsigned long sbi_entry_എണ്ണം(u32 hartindex)
{
	ഘടന sbi_scratch *scratch;
	unsigned long *entry_എണ്ണം;

	എങ്കിൽ (!entry_എണ്ണം_offset)
		തിരികെ 0;

	scratch = sbi_hartindex_to_scratch(hartindex);
	എങ്കിൽ (!scratch)
		തിരികെ 0;

	entry_എണ്ണം = sbi_scratch_offset_ptr(scratch, entry_എണ്ണം_offset);

	തിരികെ *entry_എണ്ണം;
}

unsigned long sbi_init_എണ്ണം(u32 hartindex)
{
	ഘടന sbi_scratch *scratch;
	unsigned long *init_എണ്ണം;

	എങ്കിൽ (!init_എണ്ണം_offset)
		തിരികെ 0;

	scratch = sbi_hartindex_to_scratch(hartindex);
	എങ്കിൽ (!scratch)
		തിരികെ 0;

	init_എണ്ണം = sbi_scratch_offset_ptr(scratch, init_എണ്ണം_offset);

	തിരികെ *init_എണ്ണം;
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
ശൂന്യം __noreturn sbi_exit(ഘടന sbi_scratch *scratch)
{
	u32 hartid			= current_hartid();
	സ്ഥിരം ഘടന sbi_platform *plat = sbi_platform_ptr(scratch);

	എങ്കിൽ (!sbi_hartid_valid(hartid))
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
