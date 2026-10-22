/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/riscv_atomic.h>
#समावेश <sbi/riscv_barrier.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_cppc.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_double_trap.h>
#समावेश <sbi/sbi_ecall.h>
#समावेश <sbi/sbi_fwft.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_hartmask.h>
#समावेश <sbi/sbi_hart_pmp.h>
#समावेश <sbi/sbi_hart_protection.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi/sbi_hsm.h>
#समावेश <sbi/sbi_ipi.h>
#समावेश <sbi/sbi_irqchip.h>
#समावेश <sbi/sbi_platform.h>
#समावेश <sbi/sbi_pmu.h>
#समावेश <sbi/sbi_dbtr.h>
#समावेश <sbi/sbi_mpxy.h>
#समावेश <sbi/sbi_sse.h>
#समावेश <sbi/sbi_system.h>
#समावेश <sbi/sbi_string.h>
#समावेश <sbi/sbi_timer.h>
#समावेश <sbi/sbi_tlb.h>
#समावेश <sbi/sbi_version.h>
#समावेश <sbi/sbi_unit_test.h>

#घोषणा BANNER                                              \
	"   ____                    _____ ____ _____\n"     \
	"  / __ \\                  / ____|  _ \\_   _|\n"  \
	" | |  | |_ __   ___ _ __ | (___ | |_) || |\n"      \
	" | |  | | '_ \\ / _ \\ '_ \\ \\___ \\|  _ < | |\n" \
	" | |__| | |_) |  __/ | | |____) | |_) || |_\n"     \
	"  \\____/| .__/ \\___|_| |_|_____/|____/_____|\n"  \
	"        | |\n"                                     \
	"        |_|\n\n"

अटल व्योम sbi_boot_print_banner(काष्ठा sbi_scratch *scratch)
{
	अगर (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		वापस;

#अगर_घोषित OPENSBI_VERSION_GIT
	sbi_printf("\nOpenSBI %s\n", OPENSBI_VERSION_GIT);
#अन्यथा
	sbi_printf("\nOpenSBI v%d.%d\n", OPENSBI_VERSION_MAJOR,
		   OPENSBI_VERSION_MINOR);
#पूर्ण_अगर

#अगर_घोषित OPENSBI_BUILD_TIME_STAMP
	sbi_printf("Build time: %s\n", OPENSBI_BUILD_TIME_STAMP);
#पूर्ण_अगर

#अगर_घोषित OPENSBI_BUILD_COMPILER_VERSION
	sbi_printf("Build compiler: %s\n", OPENSBI_BUILD_COMPILER_VERSION);
#पूर्ण_अगर

	sbi_printf(BANNER);
}

अटल व्योम sbi_boot_print_general(काष्ठा sbi_scratch *scratch)
{
	अक्षर str[128];
	स्थिर काष्ठा sbi_pmu_device *pdev;
	स्थिर काष्ठा sbi_hsm_device *hdev;
	स्थिर काष्ठा sbi_ipi_device *idev;
	स्थिर काष्ठा sbi_timer_device *tdev;
	स्थिर काष्ठा sbi_console_device *cdev;
	स्थिर काष्ठा sbi_system_reset_device *srdev;
	स्थिर काष्ठा sbi_system_suspend_device *susp_dev;
	स्थिर काष्ठा sbi_cppc_device *cppc_dev;
	स्थिर काष्ठा sbi_platform *plat = sbi_platform_ptr(scratch);

	अगर (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		वापस;

	/* Platform details */
	sbi_printf("Platform Name               : %s\n",
		   sbi_platform_name(plat));
	sbi_platform_get_features_str(plat, str, माप(str));
	sbi_printf("Platform Features           : %s\n", str);
	sbi_printf("Platform HART Count         : %u\n",
		   sbi_platform_hart_गिनती(plat));
	sbi_hart_protection_get_str(str, माप(str));
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
		   (u32)(scratch->fw_आकार / 1024));
	sbi_printf("Firmware RW Offset          : 0x%lx\n", scratch->fw_rw_offset);
	sbi_printf("Firmware RW Size            : %d KB\n",
		   (u32)((scratch->fw_आकार - scratch->fw_rw_offset) / 1024));
	sbi_printf("Firmware Heap Offset        : 0x%lx\n", scratch->fw_heap_offset);
	sbi_printf("Firmware Heap Size          : "
		   "%d KB (total), %d KB (reserved), %d KB (used), %d KB (free)\n",
		   (u32)(scratch->fw_heap_आकार / 1024),
		   (u32)(sbi_heap_reserved_space() / 1024),
		   (u32)(sbi_heap_used_space() / 1024),
		   (u32)(sbi_heap_free_space() / 1024));
	sbi_printf("Firmware Scratch Size       : "
		   "%d B (total), %d B (used), %d B (free)\n",
		   SBI_SCRATCH_आकार,
		   (u32)sbi_scratch_used_space(),
		   (u32)(SBI_SCRATCH_आकार - sbi_scratch_used_space()));

	/* SBI details */
	sbi_printf("Runtime SBI Version         : %d.%d\n",
		   sbi_ecall_version_major(), sbi_ecall_version_minor());
	sbi_ecall_get_extensions_str(str, माप(str), false);
	sbi_printf("Standard SBI Extensions     : %s\n", str);
	sbi_ecall_get_extensions_str(str, माप(str), true);
	sbi_printf("Experimental SBI Extensions : %s\n", str);
	sbi_printf("\n");
}

अटल व्योम sbi_boot_print_domains(काष्ठा sbi_scratch *scratch)
{
	अगर (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		वापस;

	/* Domain details */
	sbi_domain_dump_all("        ");
}

अटल व्योम sbi_boot_print_hart(काष्ठा sbi_scratch *scratch, u32 hartid)
{
	पूर्णांक xlen;
	अक्षर str[256];
	स्थिर काष्ठा sbi_domain *dom = sbi_domain_thishart_ptr();

	अगर (scratch->options & SBI_SCRATCH_NO_BOOT_PRINTS)
		वापस;

	/* Determine MISA XLEN and MISA string */
	xlen = misa_xlen();
	अगर (xlen < 1) {
		sbi_printf("Error %d getting MISA XLEN\n", xlen);
		sbi_hart_hang();
	}

	/* Boot HART details */
	sbi_printf("Boot HART ID                : %u\n", hartid);
	sbi_printf("Boot HART Domain            : %s\n", dom->name);
	sbi_hart_get_priv_version_str(scratch, str, माप(str));
	sbi_printf("Boot HART Priv Version      : %s\n", str);
	misa_string(xlen, str, माप(str));
	sbi_printf("Boot HART Base ISA          : %s\n", str);
	sbi_hart_get_extensions_str(scratch, str, माप(str));
	sbi_printf("Boot HART ISA Extensions    : %s\n", str);
	sbi_printf("Boot HART PMP Count         : %d\n",
		   sbi_hart_pmp_गिनती(scratch));
	sbi_printf("Boot HART PMP Granularity   : %u bits\n",
		   sbi_hart_pmp_log2gran(scratch));
	sbi_printf("Boot HART PMP Address Bits  : %d\n",
		   sbi_hart_pmp_addrbits(scratch));
	sbi_printf("Boot HART MHPM Info         : %lu (0x%08x)\n",
		   sbi_popcount(sbi_hart_mhpm_mask(scratch)),
		   sbi_hart_mhpm_mask(scratch));
	sbi_printf("Boot HART Debug Triggers    : %d triggers\n",
		   sbi_dbtr_get_योग_triggers());
	sbi_hart_delegation_dump(scratch, "Boot HART ", "           ");
}

अटल अचिन्हित दीर्घ coldboot_done;

अटल व्योम wait_for_coldboot(काष्ठा sbi_scratch *scratch)
{
	/* Wait for coldboot to finish */
	जबतक (!__smp_load_acquire(&coldboot_done))
		cpu_relax();
}

अटल व्योम wake_coldboot_harts(काष्ठा sbi_scratch *scratch)
{
	/* Mark coldboot done */
	__smp_store_release(&coldboot_done, 1);
}

अचिन्हित दीर्घ __attribute__((weak)) __stack_chk_guard = 0x95B5FF5A;

अटल अचिन्हित दीर्घ entry_गिनती_offset;
अटल अचिन्हित दीर्घ init_गिनती_offset;

अटल व्योम __noreturn init_coldboot(काष्ठा sbi_scratch *scratch, u32 hartid)
{
	पूर्णांक rc;
	अचिन्हित दीर्घ *गिनती;
	स्थिर काष्ठा sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Note: This has to be first thing in coldboot init sequence */
	rc = sbi_scratch_init(scratch);
	अगर (rc)
		sbi_hart_hang();

	/* Note: This has to be second thing in coldboot init sequence */
	rc = sbi_heap_init(scratch);
	अगर (rc)
		sbi_hart_hang();

	/* Note: This has to be the third thing in coldboot init sequence */
	rc = sbi_domain_init(scratch, hartid);
	अगर (rc)
		sbi_hart_hang();

	entry_गिनती_offset = sbi_scratch_alloc_offset(__SIZEOF_POINTER__);
	अगर (!entry_गिनती_offset)
		sbi_hart_hang();

	init_गिनती_offset = sbi_scratch_alloc_offset(__SIZEOF_POINTER__);
	अगर (!init_गिनती_offset)
		sbi_hart_hang();

	गिनती = sbi_scratch_offset_ptr(scratch, entry_गिनती_offset);
	(*गिनती)++;

	rc = sbi_hsm_init(scratch, true);
	अगर (rc)
		sbi_hart_hang();

	/*
	 * All non-coldboot HARTs do HSM initialization (i.e. enter HSM state
	 * machine) at the start of the warmboot path so it is wasteful to
	 * have these HARTs busy spin in wait_for_coldboot() until coldboot
	 * path is completed.
	 */
	wake_coldboot_harts(scratch);

	rc = sbi_hart_init(scratch, true);
	अगर (rc)
		sbi_hart_hang();

	/*
	 * Initialize stack guard via Zkr entropy source if Zkr is
	 * implemented according to device tree. Writing new seed value
	 * to __stack_chk_guard is safe here because function doesn't
	 * return and no check against value on entry will be done.
	 */
	अगर (sbi_hart_has_extension(scratch, SBI_HART_EXT_ZKR)) {
		अचिन्हित दीर्घ guard_val = 0;
		पूर्णांक chunks = माप(अचिन्हित दीर्घ) / माप(uint16_t);
#अगर_अघोषित CONFIG_ZKR_POLL_BUDGET
#घोषणा CONFIG_ZKR_POLL_BUDGET		1000
#पूर्ण_अगर
		अचिन्हित पूर्णांक tries = CONFIG_ZKR_POLL_BUDGET;
		bool res = false;

		जबतक (chunks && tries) {
			अचिन्हित दीर्घ seed = csr_swap(CSR_SEED, 0);
			अचिन्हित दीर्घ opst = seed & SEED_OPTS_MASK;
			res = false;

			अगर (opst == SEED_OPTS_DEAD) {
				अवरोध;
			}
			अगर (opst == SEED_OPTS_ES16) {
				guard_val = (guard_val << 16) | (seed & SEED_ENTROPY_MASK);
				chunks--;
				res = true;
				/* Successful read doesn't consume a try */
				tries++;
			}

			tries--;
			जारी;
		}
		अगर (res)
			__stack_chk_guard = guard_val;
	}

	rc = sbi_trap_init(scratch, true);
	अगर (rc)
		sbi_hart_hang();

	rc = sbi_timer_init(scratch, true);
	अगर (rc)
		sbi_hart_hang();

	rc = sbi_platform_early_init(plat, true);
	अगर (rc)
		sbi_hart_hang();

	rc = sbi_pmu_init(scratch, true);
	अगर (rc) {
		sbi_printf("%s: pmu init failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_dbtr_init(scratch, true);
	अगर (rc)
		sbi_hart_hang();

	sbi_boot_print_banner(scratch);

	sbi_double_trap_init(scratch);

	rc = sbi_irqchip_init(scratch, true);
	अगर (rc) {
		sbi_printf("%s: irqchip init failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_ipi_init(scratch, true);
	अगर (rc) {
		sbi_printf("%s: ipi init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_tlb_init(scratch, true);
	अगर (rc) {
		sbi_printf("%s: tlb init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_fwft_init(scratch, true);
	अगर (rc) {
		sbi_printf("%s: fwft init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	rc = sbi_mpxy_init(scratch);
	अगर (rc) {
		sbi_printf("%s: mpxy init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Note: Finalize domains after HSM initialization
	 * Note: Finalize domains before HART PMP configuration so
	 * that we use correct domain for configuring PMP.
	 */
	rc = sbi_domain_finalize(scratch);
	अगर (rc) {
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
	अगर (rc) {
		sbi_printf("%s: platform final init failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Note: SSE events callbacks can be registered by other drivers so
	 * sbi_sse_init() needs to be called after all drivers have been probed.
	 */
	rc = sbi_sse_init(scratch, true);
	अगर (rc) {
		sbi_printf("%s: sse init failed (error %d)\n", __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Note: Ecall initialization should be after platform final
	 * initialization so that all available platform devices are
	 * already registered.
	 */
	rc = sbi_ecall_init();
	अगर (rc) {
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
	अगर (rc) {
		sbi_printf("%s: domain startup failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	/*
	 * Configure hart protection at last because if SMEPMP is
	 * detected, M-mode access to the S/U space will be rescinded.
	 */
	rc = sbi_hart_protection_configure(scratch, sbi_domain_thishart_ptr());
	अगर (rc) {
		sbi_printf("%s: hart protection configure failed (error %d)\n",
			   __func__, rc);
		sbi_hart_hang();
	}

	गिनती = sbi_scratch_offset_ptr(scratch, init_गिनती_offset);
	(*गिनती)++;

	sbi_hsm_hart_start_finish(scratch, hartid);
}

अटल व्योम __noreturn init_warm_startup(काष्ठा sbi_scratch *scratch,
					 u32 hartid)
{
	पूर्णांक rc;
	अचिन्हित दीर्घ *गिनती;
	स्थिर काष्ठा sbi_platform *plat = sbi_platform_ptr(scratch);

	अगर (!entry_गिनती_offset || !init_गिनती_offset)
		sbi_hart_hang();

	गिनती = sbi_scratch_offset_ptr(scratch, entry_गिनती_offset);
	(*गिनती)++;

	/* Note: This has to be first thing in warmboot init sequence */
	rc = sbi_hsm_init(scratch, false);
	अगर (rc)
		sbi_hart_hang();

	rc = sbi_hart_init(scratch, false);
	अगर (rc)
		sbi_hart_hang();

	rc = sbi_trap_init(scratch, false);
	अगर (rc)
		sbi_hart_hang();

	rc = sbi_timer_init(scratch, false);
	अगर (rc)
		sbi_hart_hang();

	rc = sbi_platform_early_init(plat, false);
	अगर (rc)
		sbi_hart_hang();

	rc = sbi_pmu_init(scratch, false);
	अगर (rc)
		sbi_hart_hang();

	rc = sbi_dbtr_init(scratch, false);
	अगर (rc)
		sbi_hart_hang();

	rc = sbi_irqchip_init(scratch, false);
	अगर (rc)
		sbi_hart_hang();

	rc = sbi_ipi_init(scratch, false);
	अगर (rc)
		sbi_hart_hang();

	rc = sbi_tlb_init(scratch, false);
	अगर (rc)
		sbi_hart_hang();

	rc = sbi_fwft_init(scratch, false);
	अगर (rc)
		sbi_hart_hang();

	rc = sbi_platform_final_init(plat, false);
	अगर (rc)
		sbi_hart_hang();

	rc = sbi_sse_init(scratch, false);
	अगर (rc)
		sbi_hart_hang();

	/*
	 * Configure hart protection at last because if SMEPMP is
	 * detected, M-mode access to the S/U space will be rescinded.
	 */
	rc = sbi_hart_protection_configure(scratch, sbi_domain_thishart_ptr());
	अगर (rc)
		sbi_hart_hang();

	गिनती = sbi_scratch_offset_ptr(scratch, init_गिनती_offset);
	(*गिनती)++;

	sbi_hsm_hart_start_finish(scratch, hartid);
}

अटल व्योम __noreturn init_warm_resume(काष्ठा sbi_scratch *scratch,
					u32 hartid)
{
	पूर्णांक rc;

	sbi_hsm_hart_resume_start(scratch);

	rc = sbi_hart_reinit(scratch);
	अगर (rc)
		sbi_hart_hang();

	rc = sbi_hart_protection_configure(scratch, sbi_domain_thishart_ptr());
	अगर (rc)
		sbi_hart_hang();

	sbi_hsm_hart_resume_finish(scratch, hartid);
}

अटल व्योम __noreturn init_warmboot(काष्ठा sbi_scratch *scratch, u32 hartid)
{
	पूर्णांक hstate;

	wait_for_coldboot(scratch);

	hstate = sbi_hsm_hart_get_state(sbi_domain_thishart_ptr(), hartid);
	अगर (hstate < 0)
		sbi_hart_hang();

	अगर (hstate == SBI_HSM_STATE_SUSPENDED) {
		init_warm_resume(scratch, hartid);
	} अन्यथा {
		sbi_ipi_raw_clear(true);
		init_warm_startup(scratch, hartid);
	}
}

अटल atomic_t coldboot_lottery = ATOMIC_INITIALIZER(0);

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
व्योम __noreturn sbi_init(काष्ठा sbi_scratch *scratch)
{
	bool अग्र_mode_supported	= false;
	bool coldboot			= false;
	u32 hartid			= current_hartid();
	स्थिर काष्ठा sbi_platform *plat = sbi_platform_ptr(scratch);

	चयन (scratch->अग्र_mode) {
	हाल PRV_M:
		अग्र_mode_supported = true;
		अवरोध;
	हाल PRV_S:
		अगर (misa_extension('S'))
			अग्र_mode_supported = true;
		अवरोध;
	हाल PRV_U:
		अगर (misa_extension('U'))
			अग्र_mode_supported = true;
		अवरोध;
	शेष:
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

	अगर (sbi_platform_cold_boot_allowed(plat, hartid)) {
		अगर (अग्र_mode_supported &&
		    atomic_xchg(&coldboot_lottery, 1) == 0)
			coldboot = true;
	}

	/*
	 * Do platform specific nascent (very early) initialization so
	 * that platform can initialize platform specific per-HART CSRs
	 * or per-HART devices.
	 */
	अगर (sbi_platform_nascent_init(plat))
		sbi_hart_hang();

	अगर (coldboot)
		init_coldboot(scratch, hartid);
	अन्यथा
		init_warmboot(scratch, hartid);
}

व्योम sbi_revert_entry_गिनती(काष्ठा sbi_scratch *scratch)
{
	अचिन्हित दीर्घ *entry_गिनती, *init_गिनती;

	अगर (!entry_गिनती_offset || !init_गिनती_offset)
		sbi_hart_hang();

	entry_गिनती = sbi_scratch_offset_ptr(scratch, entry_गिनती_offset);
	init_गिनती = sbi_scratch_offset_ptr(scratch, init_गिनती_offset);

	*entry_गिनती = *init_गिनती;
}

अचिन्हित दीर्घ sbi_entry_गिनती(u32 hartindex)
{
	काष्ठा sbi_scratch *scratch;
	अचिन्हित दीर्घ *entry_गिनती;

	अगर (!entry_गिनती_offset)
		वापस 0;

	scratch = sbi_hartindex_to_scratch(hartindex);
	अगर (!scratch)
		वापस 0;

	entry_गिनती = sbi_scratch_offset_ptr(scratch, entry_गिनती_offset);

	वापस *entry_गिनती;
}

अचिन्हित दीर्घ sbi_init_गिनती(u32 hartindex)
{
	काष्ठा sbi_scratch *scratch;
	अचिन्हित दीर्घ *init_गिनती;

	अगर (!init_गिनती_offset)
		वापस 0;

	scratch = sbi_hartindex_to_scratch(hartindex);
	अगर (!scratch)
		वापस 0;

	init_गिनती = sbi_scratch_offset_ptr(scratch, init_गिनती_offset);

	वापस *init_गिनती;
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
व्योम __noreturn sbi_exit(काष्ठा sbi_scratch *scratch)
{
	u32 hartid			= current_hartid();
	स्थिर काष्ठा sbi_platform *plat = sbi_platform_ptr(scratch);

	अगर (!sbi_hartid_valid(hartid))
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
