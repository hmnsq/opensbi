/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/riscv_atomic.h>
#অন্তর্ভাব <sbi/riscv_barrier.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_cppc.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_double_trap.h>
#অন্তর্ভাব <sbi/sbi_ecall.h>
#অন্তর্ভাব <sbi/sbi_fwft.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_hartmask.h>
#অন্তর্ভাব <sbi/sbi_hart_pmp.h>
#অন্তর্ভাব <sbi/sbi_hart_protection.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi/sbi_hsm.h>
#অন্তর্ভাব <sbi/sbi_ipi.h>
#অন্তর্ভাব <sbi/sbi_irqchip.h>
#অন্তর্ভাব <sbi/sbi_platform.h>
#অন্তর্ভাব <sbi/sbi_pmu.h>
#অন্তর্ভাব <sbi/sbi_dbtr.h>
#অন্তর্ভাব <sbi/sbi_mpxy.h>
#অন্তর্ভাব <sbi/sbi_sse.h>
#অন্তর্ভাব <sbi/sbi_system.h>
#অন্তর্ভাব <sbi/sbi_string.h>
#অন্তর্ভাব <sbi/sbi_timer.h>
#অন্তর্ভাব <sbi/sbi_tlb.h>
#অন্তর্ভাব <sbi/sbi_version.h>
#অন্তর্ভাব <sbi/sbi_unit_test.h>

#define BANNER                                              \
	"   ____                    _____ ____ _____\n"     \
	"  / __ \\                  / ____|  _ \\_   _|\n"  \
	" | |  | |_ __   ___ _ __ | (___ | |_) || |\n"      \
	" | |  | | '_ \\ / _ \\ '_ \\ \\___ \\|  _ < | |\n" \
	" | |__| | |_) |  __/ | | |____) | |_) || |_\n"     \
	"  \\____/| .__/ \\___|_| |_|_____/|____/_____|\n"  \
	"        | |\n"                                     \
	"        |_|\n\n"

স্থির শূন্য sbi_boot_print_banner(কাঠামো sbi_scratch *scratch)
{
	যদি (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		ফেরত;

#ifdef OPENSBI_VERSION_GIT
	sbi_printf("\nOpenSBI %s\n", OPENSBI_VERSION_GIT);
#অন্যথা
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

স্থির শূন্য sbi_boot_print_general(কাঠামো sbi_scratch *scratch)
{
	অক্ষর str[128];
	ধ্রুবক কাঠামো sbi_pmu_device *pdev;
	ধ্রুবক কাঠামো sbi_hsm_device *hdev;
	ধ্রুবক কাঠামো sbi_ipi_device *idev;
	ধ্রুবক কাঠামো sbi_timer_device *tdev;
	ধ্রুবক কাঠামো sbi_console_device *cdev;
	ধ্রুবক কাঠামো sbi_system_reset_device *srdev;
	ধ্রুবক কাঠামো sbi_system_suspend_device *susp_dev;
	ধ্রুবক কাঠামো sbi_cppc_device *cppc_dev;
	ধ্রুবক কাঠামো sbi_platform *plat = sbi_platform_ptr(scratch);

	যদি (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		ফেরত;

	/* Platform details */
	sbi_printf("Platform Name               : %s\n",
		   sbi_platform_name(plat));
	sbi_platform_get_features_str(plat, str, আকার(str));
	sbi_printf("Platform Features           : %s\n", str);
	sbi_printf("Platform HART Count         : %u\n",
		   sbi_platform_hart_গুনতি(plat));
	sbi_hart_protection_get_str(str, আকার(str));
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
		   (u32)(scratch->fw_আকার / 1024));
	sbi_printf("Firmware RW Offset          : 0x%lx\n", scratch->fw_rw_offset);
	sbi_printf("Firmware RW Size            : %d KB\n",
		   (u32)((scratch->fw_আকার - scratch->fw_rw_offset) / 1024));
	sbi_printf("Firmware Heap Offset        : 0x%lx\n", scratch->fw_heap_offset);
	sbi_printf("Firmware Heap Size          : "
		   "%d KB (total), %d KB (reserved), %d KB (used), %d KB (free)\n",
		   (u32)(scratch->fw_heap_আকার / 1024),
		   (u32)(sbi_heap_reserved_space() / 1024),
		   (u32)(sbi_heap_used_space() / 1024),
		   (u32)(sbi_heap_free_space() / 1024));
	sbi_printf("Firmware Scratch Size       : "
		   "%d B (total), %d B (used), %d B (free)\n",
		   SBI_SCRATCH_আকার,
		   (u32)sbi_scratch_used_space(),
		   (u32)(SBI_SCRATCH_আকার - sbi_scratch_used_space()));

	/* SBI details */
	sbi_printf("Runtime SBI Version         : %d.%d\n",
		   sbi_ecall_version_major(), sbi_ecall_version_minor());
	sbi_ecall_get_extensions_str(str, আকার(str), false);
	sbi_printf("Standard SBI Extensions     : %s\n", str);
	sbi_ecall_get_extensions_str(str, আকার(str), true);
	sbi_printf("Experimental SBI Extensions : %s\n", str);
	sbi_printf("\n");
}

স্থির শূন্য sbi_boot_print_domains(কাঠামো sbi_scratch *scratch)
{
	যদি (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		ফেরত;

	/* Domain details */
	sbi_domain_dump_all("        ");
}

স্থির শূন্য sbi_boot_print_hart(কাঠামো sbi_scratch *scratch, u32 hartid)
{
	পূর্ণসংখ্যা xlen;
	অক্ষর str[256];
	ধ্রুবক কাঠামো sbi_domain *dom = sbi_domain_thishart_ptr();

	যদি (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		ফেরত;

	/* Determine MISA XLEN and MISA string */
	xlen = misa_xlen();
	যদি (xlen < 1) {
		sbi_printf("Error %d getting MISA XLEN\n", xlen);
		sbi_hart_hang();
	}

	/* Boot HART details */
	sbi_printf("Boot HART ID                : %u\n", hartid);
	sbi_printf("Boot HART Domain            : %s\n", dom->name);
	sbi_hart_get_priv_version_str(scratch, str, আকার(str));
	sbi_printf("Boot HART Priv Version      : %s\n", str);
	misa_string(xlen, str, আকার(str));
	sbi_printf("Boot HART Base ISA          : %s\n", str);
	sbi_hart_get_extensions_str(scratch, str, আকার(str));
	sbi_printf("Boot HART ISA Extensions    : %s\n", str);
	sbi_printf("Boot HART PMP Count         : %d\n",
		   sbi_hart_pmp_গুনতি(scratch));
	sbi_printf("Boot HART PMP Granularity   : %u bits\n",
		   sbi_hart_pmp_log2gran(scratch));
	sbi_printf("Boot HART PMP Address Bits  : %d\n",
		   sbi_hart_pmp_addrbits(scratch));
	sbi_printf("Boot HART MHPM Info         : %lu (0x%08x)\n",
		   sbi_popcount(sbi_hart_mhpm_mask(scratch)),
		   sbi_hart_mhpm_mask(scratch));
	sbi_printf("Boot HART Debug Triggers    : %d triggers\n",
		   sbi_dbtr_get_যোগ_triggers());
	sbi_hart_delegation_dump(scratch, "Boot HART ", "           ");
}

স্থির unsigned long coldboot_done;

স্থির শূন্য wait_for_coldboot(কাঠামো sbi_scratch *scratch)
{
	/* Wait for coldboot to finish */
	যখন (!__smp_load_acquire(&coldboot_done))
		cpu_relax();
}

স্থির শূন্য wake_coldboot_harts(কাঠামো sbi_scratch *scratch)
{
	/* Mark coldboot done */
	__smp_store_release(&coldboot_done, 1);
}

unsigned long __attribute__((weak)) __stack_chk_guard = 0x95B5FF5A;

স্থির unsigned long entry_গুনতি_offset;
স্থির unsigned long init_গুনতি_offset;

স্থির শূন্য __noreturn init_coldboot(কাঠামো sbi_scratch *scratch, u32 hartid)
{
	পূর্ণসংখ্যা rc;
	unsigned long *গুনতি;
	ধ্রুবক কাঠামো sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Note: This has to be first thing in coldboot init sequence */
	rc = sbi_scratch_init(scratch);
	যদি (rc)
		sbi_hart_hang();

	/* Note: This has to be second thing in coldboot init sequence */
	rc = sbi_heap_init(scratch);
	যদি (rc)
		sbi_hart_hang();

	/* Note: This has to be the third thing in coldboot init sequence */
	rc = sbi_domain_init(scratch, hartid);
	যদি (rc)
		sbi_hart_hang();

	entry_গুনতি_offset = sbi_scratch_alloc_offset(__SIZEOF_POINTER__);
	যদি (!entry_গুনতি_offset)
		sbi_hart_hang();

	init_গুনতি_offset = sbi_scratch_alloc_offset(__SIZEOF_POINTER__);
	যদি (!init_গুনতি_offset)
		sbi_hart_hang();

	গুনতি = sbi_scratch_offset_ptr(scratch, entry_গুনতি_offset);
	(*গুনতি)++;

	rc = sbi_hsm_init(scratch, true);
	যদি (rc)
		sbi_hart_hang();

	/*
	 * All non-coldboot HARTs do HSM initialization (i.e. enter HSM state
	 * machine) at the start of the warmboot path so it is wasteful to
	 * have these HARTs busy spin in wait_for_coldboot() until coldboot
	 * path is completed.
	 */
	wake_coldboot_harts(scratch);

	rc = sbi_hart_init(scratch, true);
	যদি (rc)
		sbi_hart_hang();

	/*
	 * Initialize stack guard via Zkr entropy source if Zkr is
	 * implemented according to device tree. Writing new seed value
	 * to __stack_chk_guard is safe here because function doesn't
	 * return and no check against value on entry will be done.
	 */
	যদি (sbi_hart_has_extension(scratch, SBI_HART_EXT_ZKR)) {
		unsigned long guard_val = 0;
		পূর্ণসংখ্যা chunks = আকার(unsigned long) / আকার(uint16_t);
#ifndef CONFIG_ZKR_POLL_BUDGET
#define CONFIG_ZKR_POLL_BUDGET		1000
#endif
		unsigned পূর্ণসংখ্যা tries = CONFIG_ZKR_POLL_BUDGET;
		bool res = false;

		যখন (chunks && tries) {
			unsigned long seed = csr_swap(CSR_SEED, 0);
			unsigned long opst = seed & SEED_OPTS_MASK;
			res = false;

			যদি (opst == SEED_OPTS_DEAD) {
				বিরতি;
			}
			যদি (opst == SEED_OPTS_ES16) {
				guard_val = (guard_val << 16) | (seed & SEED_ENTROPY_MASK);
				chunks--;
				res = true;
				/* Successful read doesn't consume a try */
				tries++;
			}

			tries--;
			চালু;
		}
		যদি (res)
			__stack_chk_guard = guard_val;
	}

	rc = sbi_trap_init(scratch, true);
	যদি (rc)
		sbi_hart_hang();

	rc = sbi_timer_init(scratch, true);
	যদি (rc)
		sbi_hart_hang();

	rc = sbi_platform_early_init(plat, true);
	যদি (rc)
		sbi_hart_hang();

	rc = sbi_pmu_init(scratch, true);
	যদি (rc) {
		sbi_printf("%s: pmu init failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_dbtr_init(scratch, true);
	যদি (rc)
		sbi_hart_hang();

	sbi_boot_print_banner(scratch);

	sbi_double_trap_init(scratch);

	rc = sbi_irqchip_init(scratch, true);
	যদি (rc) {
		sbi_printf("%s: irqchip init failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_ipi_init(scratch, true);
	যদি (rc) {
		sbi_printf("%s: ipi init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_tlb_init(scratch, true);
	যদি (rc) {
		sbi_printf("%s: tlb init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_fwft_init(scratch, true);
	যদি (rc) {
		sbi_printf("%s: fwft init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_mpxy_init(scratch);
	যদি (rc) {
		sbi_printf("%s: mpxy init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Note: Finalize domains after HSM initialization
	 * Note: Finalize domains before HART PMP configuration so
	 * that we use correct domain for configuring PMP.
	 */
	rc = sbi_domain_finalize(scratch);
	যদি (rc) {
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
	যদি (rc) {
		sbi_printf("%s: platform final init failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Note: SSE events callbacks can be registered by other drivers so
	 * sbi_sse_init() needs to be called after all drivers have been probed.
	 */
	rc = sbi_sse_init(scratch, true);
	যদি (rc) {
		sbi_printf("%s: sse init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Note: Ecall initialization should be after platform final
	 * initialization so that all available platform devices are
	 * already registered.
	 */
	rc = sbi_ecall_init();
	যদি (rc) {
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
	যদি (rc) {
		sbi_printf("%s: domain startup failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Configure hart protection at last because if SMEPMP is
	 * detected, M-mode access to the S/U space will be rescinded.
	 */
	rc = sbi_hart_protection_configure(scratch, sbi_domain_thishart_ptr());
	যদি (rc) {
		sbi_printf("%s: hart protection configure failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	গুনতি = sbi_scratch_offset_ptr(scratch, init_গুনতি_offset);
	(*গুনতি)++;

	sbi_hsm_hart_start_finish(scratch, hartid);
}

স্থির শূন্য __noreturn init_warm_startup(কাঠামো sbi_scratch *scratch,
					 u32 hartid)
{
	পূর্ণসংখ্যা rc;
	unsigned long *গুনতি;
	ধ্রুবক কাঠামো sbi_platform *plat = sbi_platform_ptr(scratch);

	যদি (!entry_গুনতি_offset || !init_গুনতি_offset)
		sbi_hart_hang();

	গুনতি = sbi_scratch_offset_ptr(scratch, entry_গুনতি_offset);
	(*গুনতি)++;

	/* Note: This has to be first thing in warmboot init sequence */
	rc = sbi_hsm_init(scratch, false);
	যদি (rc)
		sbi_hart_hang();

	rc = sbi_hart_init(scratch, false);
	যদি (rc)
		sbi_hart_hang();

	rc = sbi_trap_init(scratch, false);
	যদি (rc)
		sbi_hart_hang();

	rc = sbi_timer_init(scratch, false);
	যদি (rc)
		sbi_hart_hang();

	rc = sbi_platform_early_init(plat, false);
	যদি (rc)
		sbi_hart_hang();

	rc = sbi_pmu_init(scratch, false);
	যদি (rc)
		sbi_hart_hang();

	rc = sbi_dbtr_init(scratch, false);
	যদি (rc)
		sbi_hart_hang();

	rc = sbi_irqchip_init(scratch, false);
	যদি (rc)
		sbi_hart_hang();

	rc = sbi_ipi_init(scratch, false);
	যদি (rc)
		sbi_hart_hang();

	rc = sbi_tlb_init(scratch, false);
	যদি (rc)
		sbi_hart_hang();

	rc = sbi_fwft_init(scratch, false);
	যদি (rc)
		sbi_hart_hang();

	rc = sbi_platform_final_init(plat, false);
	যদি (rc)
		sbi_hart_hang();

	rc = sbi_sse_init(scratch, false);
	যদি (rc)
		sbi_hart_hang();

	/*
	 * Configure hart protection at last because if SMEPMP is
	 * detected, M-mode access to the S/U space will be rescinded.
	 */
	rc = sbi_hart_protection_configure(scratch, sbi_domain_thishart_ptr());
	যদি (rc)
		sbi_hart_hang();

	গুনতি = sbi_scratch_offset_ptr(scratch, init_গুনতি_offset);
	(*গুনতি)++;

	sbi_hsm_hart_start_finish(scratch, hartid);
}

স্থির শূন্য __noreturn init_warm_resume(কাঠামো sbi_scratch *scratch,
					u32 hartid)
{
	পূর্ণসংখ্যা rc;

	sbi_hsm_hart_resume_start(scratch);

	rc = sbi_hart_reinit(scratch);
	যদি (rc)
		sbi_hart_hang();

	rc = sbi_hart_protection_configure(scratch, sbi_domain_thishart_ptr());
	যদি (rc)
		sbi_hart_hang();

	sbi_hsm_hart_resume_finish(scratch, hartid);
}

স্থির শূন্য __noreturn init_warmboot(কাঠামো sbi_scratch *scratch, u32 hartid)
{
	পূর্ণসংখ্যা hstate;

	wait_for_coldboot(scratch);

	hstate = sbi_hsm_hart_get_state(sbi_domain_thishart_ptr(), hartid);
	যদি (hstate < 0)
		sbi_hart_hang();

	যদি (hstate == SBI_HSM_STATE_SUSPENDED) {
		init_warm_resume(scratch, hartid);
	} অন্যথা {
		sbi_ipi_raw_clear(true);
		init_warm_startup(scratch, hartid);
	}
}

স্থির atomic_t coldboot_lottery = ATOMIC_INITIALIZER(0);

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
শূন্য __noreturn sbi_init(কাঠামো sbi_scratch *scratch)
{
	bool পরবর্তী_mode_supported	= false;
	bool coldboot			= false;
	u32 hartid			= current_hartid();
	ধ্রুবক কাঠামো sbi_platform *plat = sbi_platform_ptr(scratch);

	নির্বাচন (scratch->পরবর্তী_mode) {
	ক্ষেত্র PRV_M:
		পরবর্তী_mode_supported = true;
		বিরতি;
	ক্ষেত্র PRV_S:
		যদি (misa_extension('S'))
			পরবর্তী_mode_supported = true;
		বিরতি;
	ক্ষেত্র PRV_U:
		যদি (misa_extension('U'))
			পরবর্তী_mode_supported = true;
		বিরতি;
	পূর্বনির্ধারিত:
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

	যদি (sbi_platform_cold_boot_allowed(plat, hartid)) {
		যদি (পরবর্তী_mode_supported &&
		    atomic_xchg(&coldboot_lottery, 1) == 0)
			coldboot = true;
	}

	/*
	 * Do platform specific nascent (very early) initialization so
	 * that platform can initialize platform specific per-HART CSRs
	 * or per-HART devices.
	 */
	যদি (sbi_platform_nascent_init(plat))
		sbi_hart_hang();

	যদি (coldboot)
		init_coldboot(scratch, hartid);
	অন্যথা
		init_warmboot(scratch, hartid);
}

শূন্য sbi_revert_entry_গুনতি(কাঠামো sbi_scratch *scratch)
{
	unsigned long *entry_গুনতি, *init_গুনতি;

	যদি (!entry_গুনতি_offset || !init_গুনতি_offset)
		sbi_hart_hang();

	entry_গুনতি = sbi_scratch_offset_ptr(scratch, entry_গুনতি_offset);
	init_গুনতি = sbi_scratch_offset_ptr(scratch, init_গুনতি_offset);

	*entry_গুনতি = *init_গুনতি;
}

unsigned long sbi_entry_গুনতি(u32 hartindex)
{
	কাঠামো sbi_scratch *scratch;
	unsigned long *entry_গুনতি;

	যদি (!entry_গুনতি_offset)
		ফেরত 0;

	scratch = sbi_hartindex_to_scratch(hartindex);
	যদি (!scratch)
		ফেরত 0;

	entry_গুনতি = sbi_scratch_offset_ptr(scratch, entry_গুনতি_offset);

	ফেরত *entry_গুনতি;
}

unsigned long sbi_init_গুনতি(u32 hartindex)
{
	কাঠামো sbi_scratch *scratch;
	unsigned long *init_গুনতি;

	যদি (!init_গুনতি_offset)
		ফেরত 0;

	scratch = sbi_hartindex_to_scratch(hartindex);
	যদি (!scratch)
		ফেরত 0;

	init_গুনতি = sbi_scratch_offset_ptr(scratch, init_গুনতি_offset);

	ফেরত *init_গুনতি;
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
শূন্য __noreturn sbi_exit(কাঠামো sbi_scratch *scratch)
{
	u32 hartid			= current_hartid();
	ধ্রুবক কাঠামো sbi_platform *plat = sbi_platform_ptr(scratch);

	যদি (!sbi_hartid_valid(hartid))
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
