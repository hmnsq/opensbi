/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_atomic.h>
#شامل <sbi/riscv_barrier.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_cppc.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_double_trap.h>
#شامل <sbi/sbi_ecall.h>
#شامل <sbi/sbi_fwft.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_hartmask.h>
#شامل <sbi/sbi_hart_pmp.h>
#شامل <sbi/sbi_hart_protection.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_hsm.h>
#شامل <sbi/sbi_ipi.h>
#شامل <sbi/sbi_irqchip.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_pmu.h>
#شامل <sbi/sbi_dbtr.h>
#شامل <sbi/sbi_mpxy.h>
#شامل <sbi/sbi_sse.h>
#شامل <sbi/sbi_system.h>
#شامل <sbi/sbi_string.h>
#شامل <sbi/sbi_timer.h>
#شامل <sbi/sbi_tlb.h>
#شامل <sbi/sbi_version.h>
#شامل <sbi/sbi_unit_test.h>

#تعریف_کرو BANNER                                              \
	"   ____                    _____ ____ _____\n"     \
	"  / __ \\                  / ____|  _ \\_   _|\n"  \
	" | |  | |_ __   ___ _ __ | (___ | |_) || |\n"      \
	" | |  | | '_ \\ / _ \\ '_ \\ \\___ \\|  _ < | |\n" \
	" | |__| | |_) |  __/ | | |____) | |_) || |_\n"     \
	"  \\____/| .__/ \\___|_| |_|_____/|____/_____|\n"  \
	"        | |\n"                                     \
	"        |_|\n\n"

ساکن خالی sbi_boot_print_banner(ساخت sbi_scratch *scratch)
{
	اگر (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		واپس;

#اگر_تعریف_شدہ OPENSBI_نسخہ_GIT
	sbi_printf("\nOpenSBI %s\n", OPENSBI_نسخہ_GIT);
#ورنہ
	sbi_printf("\nOpenSBI v%d.%d\n", OPENSBI_نسخہ_MAJOR,
		   OPENSBI_نسخہ_MINOR);
#اگر_ختم

#اگر_تعریف_شدہ OPENSBI_BUILD_وقت_STAMP
	sbi_printf("Build time: %s\n", OPENSBI_BUILD_وقت_STAMP);
#اگر_ختم

#اگر_تعریف_شدہ OPENSBI_BUILD_COMPILER_نسخہ
	sbi_printf("Build compiler: %s\n", OPENSBI_BUILD_COMPILER_نسخہ);
#اگر_ختم

	sbi_printf(BANNER);
}

ساکن خالی sbi_boot_print_general(ساخت sbi_scratch *scratch)
{
	حرف str[128];
	ثابت ساخت sbi_pmu_device *pdev;
	ثابت ساخت sbi_hsm_device *hdev;
	ثابت ساخت sbi_ipi_device *idev;
	ثابت ساخت sbi_timer_device *tdev;
	ثابت ساخت sbi_console_device *cdev;
	ثابت ساخت sbi_system_reset_device *srdev;
	ثابت ساخت sbi_system_suspend_device *susp_dev;
	ثابت ساخت sbi_cppc_device *cppc_dev;
	ثابت ساخت sbi_platform *plat = sbi_platform_ptr(scratch);

	اگر (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		واپس;

	/* Platform details */
	sbi_printf("Platform Name               : %s\n",
		   sbi_platform_name(plat));
	sbi_platform_get_features_str(plat, str, حجم(str));
	sbi_printf("Platform Features           : %s\n", str);
	sbi_printf("Platform HART Count         : %u\n",
		   sbi_platform_hart_گنتی(plat));
	sbi_hart_protection_get_str(str, حجم(str));
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
	sbi_printf("Firmware Base               : 0x%lx\n", scratch->fw_آغاز);
	sbi_printf("Firmware Size               : %d KB\n",
		   (u32)(scratch->fw_سائز / 1024));
	sbi_printf("Firmware RW Offset          : 0x%lx\n", scratch->fw_rw_انحراف);
	sbi_printf("Firmware RW Size            : %d KB\n",
		   (u32)((scratch->fw_سائز - scratch->fw_rw_انحراف) / 1024));
	sbi_printf("Firmware Heap Offset        : 0x%lx\n", scratch->fw_ڈھیر_انحراف);
	sbi_printf("Firmware Heap Size          : "
		   "%d KB (total), %d KB (reserved), %d KB (used), %d KB (free)\n",
		   (u32)(scratch->fw_ڈھیر_سائز / 1024),
		   (u32)(sbi_ڈھیر_reserved_space() / 1024),
		   (u32)(sbi_ڈھیر_مستعمل_space() / 1024),
		   (u32)(sbi_ڈھیر_free_space() / 1024));
	sbi_printf("Firmware Scratch Size       : "
		   "%d B (total), %d B (used), %d B (free)\n",
		   SBI_SCRATCH_سائز,
		   (u32)sbi_scratch_مستعمل_space(),
		   (u32)(SBI_SCRATCH_سائز - sbi_scratch_مستعمل_space()));

	/* SBI details */
	sbi_printf("Runtime SBI Version         : %d.%d\n",
		   sbi_ecall_نسخہ_major(), sbi_ecall_نسخہ_minor());
	sbi_ecall_get_extensions_str(str, حجم(str), false);
	sbi_printf("Standard SBI Extensions     : %s\n", str);
	sbi_ecall_get_extensions_str(str, حجم(str), true);
	sbi_printf("Experimental SBI Extensions : %s\n", str);
	sbi_printf("\n");
}

ساکن خالی sbi_boot_print_domains(ساخت sbi_scratch *scratch)
{
	اگر (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		واپس;

	/* Domain details */
	sbi_domain_dump_all("        ");
}

ساکن خالی sbi_boot_print_hart(ساخت sbi_scratch *scratch, u32 hartid)
{
	صحیح xlen;
	حرف str[256];
	ثابت ساخت sbi_domain *dom = sbi_domain_thishart_ptr();

	اگر (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		واپس;

	/* Determine MISA XLEN and MISA string */
	xlen = misa_xlen();
	اگر (xlen < 1) {
		sbi_printf("Error %d getting MISA XLEN\n", xlen);
		sbi_hart_hang();
	}

	/* Boot HART details */
	sbi_printf("Boot HART ID                : %u\n", hartid);
	sbi_printf("Boot HART Domain            : %s\n", dom->name);
	sbi_hart_get_priv_نسخہ_str(scratch, str, حجم(str));
	sbi_printf("Boot HART Priv Version      : %s\n", str);
	misa_string(xlen, str, حجم(str));
	sbi_printf("Boot HART Base ISA          : %s\n", str);
	sbi_hart_get_extensions_str(scratch, str, حجم(str));
	sbi_printf("Boot HART ISA Extensions    : %s\n", str);
	sbi_printf("Boot HART PMP Count         : %d\n",
		   sbi_hart_pmp_گنتی(scratch));
	sbi_printf("Boot HART PMP Granularity   : %u bits\n",
		   sbi_hart_pmp_log2gran(scratch));
	sbi_printf("Boot HART PMP Address Bits  : %d\n",
		   sbi_hart_pmp_addrbits(scratch));
	sbi_printf("Boot HART MHPM Info         : %lu (0x%08x)\n",
		   sbi_popcount(sbi_hart_mhpm_نقاب(scratch)),
		   sbi_hart_mhpm_نقاب(scratch));
	sbi_printf("Boot HART Debug Triggers    : %d triggers\n",
		   sbi_dbtr_get_کل_triggers());
	sbi_hart_delegation_dump(scratch, "Boot HART ", "           ");
}

ساکن غیرعلامتی طویل coldboot_مکمل;

ساکن خالی wait_for_coldboot(ساخت sbi_scratch *scratch)
{
	/* Wait for coldboot to finish */
	جب (!__smp_load_acquire(&coldboot_مکمل))
		cpu_relax();
}

ساکن خالی wake_coldboot_harts(ساخت sbi_scratch *scratch)
{
	/* Mark coldboot done */
	__smp_store_release(&coldboot_مکمل, 1);
}

غیرعلامتی طویل __attribute__((weak)) __stack_chk_guard = 0x95B5FF5A;

ساکن غیرعلامتی طویل اندراج_گنتی_انحراف;
ساکن غیرعلامتی طویل init_گنتی_انحراف;

ساکن خالی __noreturn init_coldboot(ساخت sbi_scratch *scratch, u32 hartid)
{
	صحیح rc;
	غیرعلامتی طویل *گنتی;
	ثابت ساخت sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Note: This has to be first thing in coldboot init sequence */
	rc = sbi_scratch_init(scratch);
	اگر (rc)
		sbi_hart_hang();

	/* Note: This has to be second thing in coldboot init sequence */
	rc = sbi_ڈھیر_init(scratch);
	اگر (rc)
		sbi_hart_hang();

	/* Note: This has to be the third thing in coldboot init sequence */
	rc = sbi_domain_init(scratch, hartid);
	اگر (rc)
		sbi_hart_hang();

	اندراج_گنتی_انحراف = sbi_scratch_alloc_انحراف(__SIZEOF_POINTER__);
	اگر (!اندراج_گنتی_انحراف)
		sbi_hart_hang();

	init_گنتی_انحراف = sbi_scratch_alloc_انحراف(__SIZEOF_POINTER__);
	اگر (!init_گنتی_انحراف)
		sbi_hart_hang();

	گنتی = sbi_scratch_انحراف_ptr(scratch, اندراج_گنتی_انحراف);
	(*گنتی)++;

	rc = sbi_hsm_init(scratch, true);
	اگر (rc)
		sbi_hart_hang();

	/*
	 * All non-coldboot HARTs do HSM initialization (i.e. enter HSM state
	 * machine) at the start of the warmboot path so it is wasteful to
	 * have these HARTs busy spin in wait_for_coldboot() until coldboot
	 * path is completed.
	 */
	wake_coldboot_harts(scratch);

	rc = sbi_hart_init(scratch, true);
	اگر (rc)
		sbi_hart_hang();

	/*
	 * Initialize stack guard via Zkr entropy source if Zkr is
	 * implemented according to device tree. Writing new seed value
	 * to __stack_chk_guard is safe here because function doesn't
	 * return and no check against value on entry will be done.
	 */
	اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_ZKR)) {
		غیرعلامتی طویل guard_val = 0;
		صحیح chunks = حجم(غیرعلامتی طویل) / حجم(uint16_t);
#اگر_غیر_تعریف_شدہ CONFIG_ZKR_POLL_BUDGET
#تعریف_کرو CONFIG_ZKR_POLL_BUDGET		1000
#اگر_ختم
		غیرعلامتی صحیح tries = CONFIG_ZKR_POLL_BUDGET;
		bool res = false;

		جب (chunks && tries) {
			غیرعلامتی طویل بیج = csr_تبادلہ(CSR_بیج, 0);
			غیرعلامتی طویل opst = بیج & بیج_OPTS_نقاب;
			res = false;

			اگر (opst == بیج_OPTS_DEAD) {
				توڑو;
			}
			اگر (opst == بیج_OPTS_ES16) {
				guard_val = (guard_val << 16) | (بیج & بیج_ENTROPY_نقاب);
				chunks--;
				res = true;
				/* Successful read doesn't consume a try */
				tries++;
			}

			tries--;
			جاری;
		}
		اگر (res)
			__stack_chk_guard = guard_val;
	}

	rc = sbi_trap_init(scratch, true);
	اگر (rc)
		sbi_hart_hang();

	rc = sbi_timer_init(scratch, true);
	اگر (rc)
		sbi_hart_hang();

	rc = sbi_platform_early_init(plat, true);
	اگر (rc)
		sbi_hart_hang();

	rc = sbi_pmu_init(scratch, true);
	اگر (rc) {
		sbi_printf("%s: pmu init failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_dbtr_init(scratch, true);
	اگر (rc)
		sbi_hart_hang();

	sbi_boot_print_banner(scratch);

	sbi_double_trap_init(scratch);

	rc = sbi_irqchip_init(scratch, true);
	اگر (rc) {
		sbi_printf("%s: irqchip init failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_ipi_init(scratch, true);
	اگر (rc) {
		sbi_printf("%s: ipi init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_tlb_init(scratch, true);
	اگر (rc) {
		sbi_printf("%s: tlb init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_fwft_init(scratch, true);
	اگر (rc) {
		sbi_printf("%s: fwft init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_mpxy_init(scratch);
	اگر (rc) {
		sbi_printf("%s: mpxy init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Note: Finalize domains after HSM initialization
	 * Note: Finalize domains before HART PMP configuration so
	 * that we use correct domain for configuring PMP.
	 */
	rc = sbi_domain_finalize(scratch);
	اگر (rc) {
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
	اگر (rc) {
		sbi_printf("%s: platform final init failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Note: SSE events callbacks can be registered by other drivers so
	 * sbi_sse_init() needs to be called after all drivers have been probed.
	 */
	rc = sbi_sse_init(scratch, true);
	اگر (rc) {
		sbi_printf("%s: sse init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Note: Ecall initialization should be after platform final
	 * initialization so that all available platform devices are
	 * already registered.
	 */
	rc = sbi_ecall_init();
	اگر (rc) {
		sbi_printf("%s: ecall init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	sbi_boot_print_general(scratch);

	sbi_boot_print_domains(scratch);

	sbi_boot_print_hart(scratch, hartid);

	تسلسل_all_tests();

	/*
	 * Note: Startup domains after all initialization are done
	 * otherwise boot HART of non-root domain can crash.
	 */
	rc = sbi_domain_startup(scratch, hartid);
	اگر (rc) {
		sbi_printf("%s: domain startup failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Configure hart protection at last because if SMEPMP is
	 * detected, M-mode access to the S/U space will be rescinded.
	 */
	rc = sbi_hart_protection_configure(scratch, sbi_domain_thishart_ptr());
	اگر (rc) {
		sbi_printf("%s: hart protection configure failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	گنتی = sbi_scratch_انحراف_ptr(scratch, init_گنتی_انحراف);
	(*گنتی)++;

	sbi_hsm_hart_آغاز_اختتام(scratch, hartid);
}

ساکن خالی __noreturn init_warm_startup(ساخت sbi_scratch *scratch,
					 u32 hartid)
{
	صحیح rc;
	غیرعلامتی طویل *گنتی;
	ثابت ساخت sbi_platform *plat = sbi_platform_ptr(scratch);

	اگر (!اندراج_گنتی_انحراف || !init_گنتی_انحراف)
		sbi_hart_hang();

	گنتی = sbi_scratch_انحراف_ptr(scratch, اندراج_گنتی_انحراف);
	(*گنتی)++;

	/* Note: This has to be first thing in warmboot init sequence */
	rc = sbi_hsm_init(scratch, false);
	اگر (rc)
		sbi_hart_hang();

	rc = sbi_hart_init(scratch, false);
	اگر (rc)
		sbi_hart_hang();

	rc = sbi_trap_init(scratch, false);
	اگر (rc)
		sbi_hart_hang();

	rc = sbi_timer_init(scratch, false);
	اگر (rc)
		sbi_hart_hang();

	rc = sbi_platform_early_init(plat, false);
	اگر (rc)
		sbi_hart_hang();

	rc = sbi_pmu_init(scratch, false);
	اگر (rc)
		sbi_hart_hang();

	rc = sbi_dbtr_init(scratch, false);
	اگر (rc)
		sbi_hart_hang();

	rc = sbi_irqchip_init(scratch, false);
	اگر (rc)
		sbi_hart_hang();

	rc = sbi_ipi_init(scratch, false);
	اگر (rc)
		sbi_hart_hang();

	rc = sbi_tlb_init(scratch, false);
	اگر (rc)
		sbi_hart_hang();

	rc = sbi_fwft_init(scratch, false);
	اگر (rc)
		sbi_hart_hang();

	rc = sbi_platform_final_init(plat, false);
	اگر (rc)
		sbi_hart_hang();

	rc = sbi_sse_init(scratch, false);
	اگر (rc)
		sbi_hart_hang();

	/*
	 * Configure hart protection at last because if SMEPMP is
	 * detected, M-mode access to the S/U space will be rescinded.
	 */
	rc = sbi_hart_protection_configure(scratch, sbi_domain_thishart_ptr());
	اگر (rc)
		sbi_hart_hang();

	گنتی = sbi_scratch_انحراف_ptr(scratch, init_گنتی_انحراف);
	(*گنتی)++;

	sbi_hsm_hart_آغاز_اختتام(scratch, hartid);
}

ساکن خالی __noreturn init_warm_resume(ساخت sbi_scratch *scratch,
					u32 hartid)
{
	صحیح rc;

	sbi_hsm_hart_resume_آغاز(scratch);

	rc = sbi_hart_reinit(scratch);
	اگر (rc)
		sbi_hart_hang();

	rc = sbi_hart_protection_configure(scratch, sbi_domain_thishart_ptr());
	اگر (rc)
		sbi_hart_hang();

	sbi_hsm_hart_resume_اختتام(scratch, hartid);
}

ساکن خالی __noreturn init_warmboot(ساخت sbi_scratch *scratch, u32 hartid)
{
	صحیح hstate;

	wait_for_coldboot(scratch);

	hstate = sbi_hsm_hart_get_state(sbi_domain_thishart_ptr(), hartid);
	اگر (hstate < 0)
		sbi_hart_hang();

	اگر (hstate == SBI_HSM_STATE_SUSPENDED) {
		init_warm_resume(scratch, hartid);
	} ورنہ {
		sbi_ipi_raw_clear(true);
		init_warm_startup(scratch, hartid);
	}
}

ساکن atomic_t coldboot_lottery = ATOMIC_INITIALIZER(0);

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
خالی __noreturn sbi_init(ساخت sbi_scratch *scratch)
{
	bool اگلا_منوال_supported	= false;
	bool coldboot			= false;
	u32 hartid			= current_hartid();
	ثابت ساخت sbi_platform *plat = sbi_platform_ptr(scratch);

	انتخاب (scratch->اگلا_منوال) {
	حالت PRV_M:
		اگلا_منوال_supported = true;
		توڑو;
	حالت PRV_S:
		اگر (misa_extension('S'))
			اگلا_منوال_supported = true;
		توڑو;
	حالت PRV_U:
		اگر (misa_extension('U'))
			اگلا_منوال_supported = true;
		توڑو;
	طےشدہ:
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

	اگر (sbi_platform_cold_boot_allowed(plat, hartid)) {
		اگر (اگلا_منوال_supported &&
		    atomic_xchg(&coldboot_lottery, 1) == 0)
			coldboot = true;
	}

	/*
	 * Do platform specific nascent (very early) initialization so
	 * that platform can initialize platform specific per-HART CSRs
	 * or per-HART devices.
	 */
	اگر (sbi_platform_nascent_init(plat))
		sbi_hart_hang();

	اگر (coldboot)
		init_coldboot(scratch, hartid);
	ورنہ
		init_warmboot(scratch, hartid);
}

خالی sbi_revert_اندراج_گنتی(ساخت sbi_scratch *scratch)
{
	غیرعلامتی طویل *اندراج_گنتی, *init_گنتی;

	اگر (!اندراج_گنتی_انحراف || !init_گنتی_انحراف)
		sbi_hart_hang();

	اندراج_گنتی = sbi_scratch_انحراف_ptr(scratch, اندراج_گنتی_انحراف);
	init_گنتی = sbi_scratch_انحراف_ptr(scratch, init_گنتی_انحراف);

	*اندراج_گنتی = *init_گنتی;
}

غیرعلامتی طویل sbi_اندراج_گنتی(u32 hartindex)
{
	ساخت sbi_scratch *scratch;
	غیرعلامتی طویل *اندراج_گنتی;

	اگر (!اندراج_گنتی_انحراف)
		واپس 0;

	scratch = sbi_hartindex_مقصد_scratch(hartindex);
	اگر (!scratch)
		واپس 0;

	اندراج_گنتی = sbi_scratch_انحراف_ptr(scratch, اندراج_گنتی_انحراف);

	واپس *اندراج_گنتی;
}

غیرعلامتی طویل sbi_init_گنتی(u32 hartindex)
{
	ساخت sbi_scratch *scratch;
	غیرعلامتی طویل *init_گنتی;

	اگر (!init_گنتی_انحراف)
		واپس 0;

	scratch = sbi_hartindex_مقصد_scratch(hartindex);
	اگر (!scratch)
		واپس 0;

	init_گنتی = sbi_scratch_انحراف_ptr(scratch, init_گنتی_انحراف);

	واپس *init_گنتی;
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
خالی __noreturn sbi_exit(ساخت sbi_scratch *scratch)
{
	u32 hartid			= current_hartid();
	ثابت ساخت sbi_platform *plat = sbi_platform_ptr(scratch);

	اگر (!sbi_hartid_valid(hartid))
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
