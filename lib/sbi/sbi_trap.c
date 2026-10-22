/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_encoding.h>
#شامل <sbi/sbi_bitops.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_double_trap.h>
#شامل <sbi/sbi_ecall.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_illegal_insn.h>
#شامل <sbi/sbi_ipi.h>
#شامل <sbi/sbi_irqchip.h>
#شامل <sbi/sbi_trap_ldst.h>
#شامل <sbi/sbi_pmu.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_sse.h>
#شامل <sbi/sbi_timer.h>
#شامل <sbi/sbi_trap.h>

ساکن خالی sbi_trap_error_one(ثابت ساخت sbi_trap_context *tcntx,
			       ثابت حرف *سابقہ, u32 hartid, u32 گہرائی)
{
	ثابت ساخت sbi_trap_info *trap = &tcntx->trap;
	ثابت ساخت sbi_trap_regs *regs = &tcntx->regs;

	sbi_printf("\n");
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", سابقہ,
		   hartid, گہرائی, "mcause", trap->cause, "mtval", trap->tval);
	اگر (misa_extension('H')) {
		sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", سابقہ,
			   hartid, گہرائی, "mtval2", trap->tval2, "mtinst", trap->tinst);
	}
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", سابقہ,
		   hartid, گہرائی, "mepc", regs->mepc, "mstatus", regs->mstatus);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", سابقہ,
		   hartid, گہرائی, "ra", regs->ra, "sp", regs->sp);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", سابقہ,
		   hartid, گہرائی, "gp", regs->gp, "tp", regs->tp);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", سابقہ,
		   hartid, گہرائی, "s0", regs->s0, "s1", regs->s1);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", سابقہ,
		   hartid, گہرائی, "a0", regs->a0, "a1", regs->a1);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", سابقہ,
		   hartid, گہرائی, "a2", regs->a2, "a3", regs->a3);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", سابقہ,
		   hartid, گہرائی, "a4", regs->a4, "a5", regs->a5);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", سابقہ,
		   hartid, گہرائی, "a6", regs->a6, "a7", regs->a7);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", سابقہ,
		   hartid, گہرائی, "s2", regs->s2, "s3", regs->s3);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", سابقہ,
		   hartid, گہرائی, "s4", regs->s4, "s5", regs->s5);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", سابقہ,
		   hartid, گہرائی, "s6", regs->s6, "s7", regs->s7);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", سابقہ,
		   hartid, گہرائی, "s8", regs->s8, "s9", regs->s9);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", سابقہ,
		   hartid, گہرائی, "s10", regs->s10, "s11", regs->s11);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", سابقہ,
		   hartid, گہرائی, "t0", regs->t0, "t1", regs->t1);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", سابقہ,
		   hartid, گہرائی, "t2", regs->t2, "t3", regs->t3);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", سابقہ,
		   hartid, گہرائی, "t4", regs->t4, "t5", regs->t5);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX "\n", سابقہ,
		   hartid, گہرائی, "t6", regs->t6);
}

ساکن خالی __noreturn sbi_trap_error(ثابت حرف *پیغام, صحیح rc,
				      ثابت ساخت sbi_trap_context *tcntx)
{
	u32 گہرائی = 0, hartid = current_hartid();
	ثابت ساخت sbi_trap_context *tc;

	برائے (tc = tcntx; tc; tc = tc->پچھلا_context)
		گہرائی++;

	sbi_printf("\n");
	sbi_printf("%s: hart%d: trap%d: %s (error %d)\n", __func__,
		   hartid, گہرائی - 1, پیغام, rc);
	برائے (tc = tcntx; tc; tc = tc->پچھلا_context)
		sbi_trap_error_one(tc, __func__, hartid, --گہرائی);

	sbi_hart_hang();
}

/**
 * Redirect trap to lower privledge mode (S-mode or U-mode)
 *
 * @param regs pointer to register state
 * @param trap pointer to trap details
 *
 * @return 0 on success and negative error code on failure
 */
صحیح sbi_trap_redirect(ساخت sbi_trap_regs *regs,
		      ثابت ساخت sbi_trap_info *trap)
{
	ulong hstatus, vsstatus, پچھلا_منوال;
	bool elp = false;
	bool پچھلا_virt = sbi_regs_from_virt(regs);
	/* By default, we redirect to HS-mode */
	bool اگلا_virt = false;

	/* Sanity check on previous mode */
	پچھلا_منوال = sbi_mstatus_پچھلا_منوال(regs->mstatus);
	اگر (پچھلا_منوال != PRV_S && پچھلا_منوال != PRV_U)
		واپس SBI_ENOTSUPP;

	/* If hart support for zicfilp, clear MPELP because redirecting to VS or (H)S */
	اگر (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_ZICFILP)) {
#اگر __riscv_xlen == 32
		elp = regs->mstatusH & MSTATUSH_MPELP;
		regs->mstatusH &= ~MSTATUSH_MPELP;
#ورنہ
		elp = regs->mstatus & MSTATUS_MPELP;
		regs->mstatus &= ~MSTATUS_MPELP;
#اگر_ختم
	}

	/* If exceptions came from VS/VU-mode, redirect to VS-mode if
	 * delegated in hedeleg
	 */
	اگر (misa_extension('H') && پچھلا_virt) {
		اگر ((trap->cause < __riscv_xlen) &&
		    (csr_read(CSR_HEDELEG) & ثنائی_ہندسہ(trap->cause))) {
			اگلا_virt = true;
		}
	}

	/* Update MSTATUS MPV bits */
#اگر __riscv_xlen == 32
	regs->mstatusH &= ~MSTATUSH_MPV;
	regs->mstatusH |= (اگلا_virt) ? MSTATUSH_MPV : 0UL;
#ورنہ
	regs->mstatus &= ~MSTATUS_MPV;
	regs->mstatus |= (اگلا_virt) ? MSTATUS_MPV : 0UL;
#اگر_ختم

	/* Update hypervisor CSRs if going to HS-mode */
	اگر (misa_extension('H') && !اگلا_virt) {
		hstatus = csr_read(CSR_HSTATUS);
		اگر (پچھلا_virt) {
			/* hstatus.SPVP is only updated if coming from VS/VU-mode */
			hstatus &= ~HSTATUS_SPVP;
			hstatus |= (پچھلا_منوال == PRV_S) ? HSTATUS_SPVP : 0;
		}
		hstatus &= ~HSTATUS_SPV;
		hstatus |= (پچھلا_virt) ? HSTATUS_SPV : 0;
		hstatus &= ~HSTATUS_GVA;
		hstatus |= (trap->gva) ? HSTATUS_GVA : 0;
		csr_write(CSR_HSTATUS, hstatus);
		csr_write(CSR_HTVAL, trap->tval2);
		csr_write(CSR_HTINST, trap->tinst);
	}

	/* Update exception related CSRs */
	اگر (اگلا_virt) {
		/* Update VS-mode exception info */
		csr_write(CSR_VSTVAL, trap->tval);
		csr_write(CSR_VSEPC, regs->mepc);
		csr_write(CSR_VSCAUSE, trap->cause);

		/* Set MEPC to VS-mode exception vector base */
		regs->mepc = csr_read(CSR_VSTVEC) & ~MTVEC_منوال;

		/* Set MPP to VS-mode */
		regs->mstatus &= ~MSTATUS_MPP;
		regs->mstatus |= (PRV_S << MSTATUS_MPP_سرکاؤ);

		/* Get VS-mode SSTATUS CSR */
		vsstatus = csr_read(CSR_VSSTATUS);

		/* If elp was set, set it back in vsstatus */
		اگر (elp)
			vsstatus |= MSTATUS_SPELP;

		/* Set SPP for VS-mode */
		vsstatus &= ~SSTATUS_SPP;
		اگر (پچھلا_منوال == PRV_S)
			vsstatus |= (1UL << SSTATUS_SPP_سرکاؤ);

		/* Set SPIE for VS-mode */
		vsstatus &= ~SSTATUS_SPIE;
		اگر (vsstatus & SSTATUS_SIE)
			vsstatus |= (1UL << SSTATUS_SPIE_سرکاؤ);

		/* Clear SIE for VS-mode */
		vsstatus &= ~SSTATUS_SIE;

		/* Update VS-mode SSTATUS CSR */
		csr_write(CSR_VSSTATUS, vsstatus);
	} ورنہ {
		/* Update S-mode exception info */
		csr_write(CSR_STVAL, trap->tval);
		csr_write(CSR_SEPC, regs->mepc);
		csr_write(CSR_SCAUSE, trap->cause);

		/* Set MEPC to S-mode exception vector base */
		regs->mepc = csr_read(CSR_STVEC) & ~MTVEC_منوال;

		/* Set MPP to S-mode */
		regs->mstatus &= ~MSTATUS_MPP;
		regs->mstatus |= (PRV_S << MSTATUS_MPP_سرکاؤ);

		/* Set SPP for S-mode */
		regs->mstatus &= ~MSTATUS_SPP;
		اگر (پچھلا_منوال == PRV_S)
			regs->mstatus |= (1UL << MSTATUS_SPP_سرکاؤ);

		/* Set SPIE for S-mode */
		regs->mstatus &= ~MSTATUS_SPIE;
		اگر (regs->mstatus & MSTATUS_SIE)
			regs->mstatus |= (1UL << MSTATUS_SPIE_سرکاؤ);

		/* Clear SIE for S-mode */
		regs->mstatus &= ~MSTATUS_SIE;

		/* If elp was set, set it back in mstatus */
		اگر (elp)
			regs->mstatus |= MSTATUS_SPELP;
	}

	واپس 0;
}

ساکن صحیح sbi_trap_nonaia_irq(غیرعلامتی طویل irq)
{
	انتخاب (irq) {
	حالت IRQ_M_TIMER:
		sbi_timer_process();
		توڑو;
	حالت IRQ_M_SOFT:
		sbi_ipi_process();
		توڑو;
	حالت IRQ_M_EXT:
		واپس sbi_irqchip_process();
	طےشدہ:
		اگر (irq == sbi_pmu_irq_ثنائی_ہندسہ()) {
			sbi_pmu_ovf_irq();
			واپس 0;
		}
		واپس SBI_ENOENT;
	}

	واپس 0;
}

ساکن صحیح sbi_trap_aia_irq(خالی)
{
	صحیح rc;
	غیرعلامتی طویل mtopi;

	جب ((mtopi = csr_read(CSR_MTOPI))) {
		mtopi = mtopi >> TOPI_IID_سرکاؤ;
		انتخاب (mtopi) {
		حالت IRQ_M_TIMER:
			sbi_timer_process();
			توڑو;
		حالت IRQ_M_SOFT:
			sbi_ipi_process();
			توڑو;
		حالت IRQ_M_EXT:
			rc = sbi_irqchip_process();
			اگر (rc)
				واپس rc;
			توڑو;
		طےشدہ:
			اگر (mtopi == sbi_pmu_irq_ثنائی_ہندسہ()) {
				sbi_pmu_ovf_irq();
				توڑو;
			}

			واپس SBI_ENOENT;
		}
	}

	واپس 0;
}

/**
 * Handle trap/interrupt
 *
 * This function is called by firmware linked to OpenSBI
 * library for handling trap/interrupt. It expects the
 * following:
 * 1. The 'mscratch' CSR is pointing to sbi_scratch of current HART
 * 2. The 'mcause' CSR is having exception/interrupt cause
 * 3. The 'mtval' CSR is having additional trap information
 * 4. The 'mtval2' CSR is having additional trap information
 * 5. The 'mtinst' CSR is having decoded trap instruction
 * 6. Stack pointer (SP) is setup for current HART
 * 7. Interrupts are disabled in MSTATUS CSR
 *
 * @param tcntx pointer to trap context
 */
ساخت sbi_trap_context *sbi_trap_handler(ساخت sbi_trap_context *tcntx)
{
	صحیح rc = SBI_ENOTSUPP;
	ثابت حرف *پیغام = "trap handler failed";
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ثابت ساخت sbi_trap_info *trap = &tcntx->trap;
	ساخت sbi_trap_regs *regs = &tcntx->regs;
	ulong mcause = tcntx->trap.cause;

	/* Update trap context pointer */
	tcntx->پچھلا_context = sbi_trap_get_context(scratch);
	sbi_trap_سیٹ_context(scratch, tcntx);

	اگر (mcause & MCAUSE_IRQ_نقاب) {
		اگر (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
					   SBI_HART_EXT_SMAIA))
			rc = sbi_trap_aia_irq();
		ورنہ
			rc = sbi_trap_nonaia_irq(mcause & ~MCAUSE_IRQ_نقاب);
		پیغام = "unhandled local interrupt";
		جاؤ trap_مکمل;
	}

	انتخاب (mcause) {
	حالت CAUSE_ILLEGAL_INSTRUCTION:
		rc  = sbi_illegal_insn_handler(tcntx);
		پیغام = "illegal instruction handler failed";
		توڑو;
	حالت CAUSE_MISALIGNED_بھرو:
		sbi_pmu_ctr_incr_fw(SBI_PMU_FW_MISALIGNED_بھرو);
		rc  = sbi_misaligned_بھرو_handler(tcntx);
		پیغام = "misaligned load handler failed";
		توڑو;
	حالت CAUSE_MISALIGNED_STORE:
		sbi_pmu_ctr_incr_fw(SBI_PMU_FW_MISALIGNED_STORE);
		rc  = sbi_misaligned_store_handler(tcntx);
		پیغام = "misaligned store handler failed";
		توڑو;
	حالت CAUSE_SUPERVISOR_ECALL:
	حالت CAUSE_MACHINE_ECALL:
		rc  = sbi_ecall_handler(tcntx);
		پیغام = "ecall handler failed";
		توڑو;
	حالت CAUSE_بھرو_ACCESS:
		sbi_pmu_ctr_incr_fw(SBI_PMU_FW_ACCESS_بھرو);
		rc  = sbi_بھرو_access_handler(tcntx);
		پیغام = "load fault handler failed";
		توڑو;
	حالت CAUSE_STORE_ACCESS:
		sbi_pmu_ctr_incr_fw(SBI_PMU_FW_ACCESS_STORE);
		rc  = sbi_store_access_handler(tcntx);
		پیغام = "store fault handler failed";
		توڑو;
	حالت CAUSE_DOUBLE_TRAP:
		rc  = sbi_double_trap_handler(tcntx);
		پیغام = "double trap handler failed";
		توڑو;
	طےشدہ:
		/* If the trap came from S or U mode, redirect it there */
		پیغام = "trap redirect failed";
		rc  = sbi_trap_redirect(regs, trap);
		توڑو;
	}

trap_مکمل:
	اگر (rc)
		sbi_trap_error(پیغام, rc, tcntx);

	اگر (sbi_mstatus_پچھلا_منوال(regs->mstatus) != PRV_M)
		sbi_sse_process_pending_events(regs);

	sbi_trap_سیٹ_context(scratch, tcntx->پچھلا_context);
	واپس tcntx;
}

/**
 * Default Resumable NMI (RNMI) handler
 *
 * This function is called from the _trap_rnmi_handler assembly code.
 * It provides a simple wrapper that calls the platform-specific
 * NMI handler if registered. If no handler is registered, it prints
 * diagnostic information and hangs, similar to unhandled traps.
 *
 * Note: The trap context stores NMI CSR values (MNCAUSE, MNEPC, MNSTATUS)
 * in the generic trap context fields (cause, mepc, mstatus).
 *
 * @param tcntx Pointer to trap context (saved on stack)
 * @return Same trap context pointer (needed for restore macros)
 */
ساخت sbi_trap_context *sbi_trap_rnmi_handler(ساخت sbi_trap_context *tcntx)
{
	صحیح rc;
	ثابت ساخت sbi_platform *plat = sbi_platform_thishart_ptr();
	ثابت ساخت sbi_platform_operations *ops = sbi_platform_ops(plat);

	/* Call platform-specific NMI handler if registered */
	اگر (ops && ops->rnmi_handler) {
		rc = ops->rnmi_handler(tcntx);
		اگر (rc) {
			/* Platform handler failed to handle NMI */
			sbi_trap_error("platform NMI handler failed", rc, tcntx);
		}
		واپس tcntx;
	}

	/* No platform handler - treat as unhandled NMI */
	sbi_trap_error("unhandled NMI (no platform rnmi_handler)",
		       SBI_ENOTSUPP, tcntx);

	/* Never returns */
	واپس tcntx;
}

ساکن صحیح delegate_traps(ساخت sbi_scratch *scratch)
{
	ثابت ساخت sbi_platform *plat = sbi_platform_ptr(scratch);
	غیرعلامتی طویل interrupts, exceptions;

	اگر (!misa_extension('S'))
		/* No delegation possible as mideleg does not exist */
		واپس 0;

	/* Send M-mode interrupts and most exceptions to S-mode */
	interrupts = MIP_SSIP | MIP_STIP | MIP_SEIP;
	interrupts |= sbi_pmu_irq_نقاب();

	exceptions = (1U << CAUSE_MISALIGNED_FETCH) | (1U << CAUSE_BREAKPOINT) |
		     (1U << CAUSE_USER_ECALL);
	اگر (sbi_platform_has_mfaults_delegation(plat))
		exceptions |= (1U << CAUSE_FETCH_صفحہ_FAULT) |
			      (1U << CAUSE_بھرو_صفحہ_FAULT) |
			      (1U << CAUSE_STORE_صفحہ_FAULT) |
			      (1U << CAUSE_SW_CHECK_EXCP);

	/*
	 * If hypervisor extension available then we only handle hypervisor
	 * calls (i.e. ecalls from HS-mode) in M-mode.
	 *
	 * The HS-mode will additionally handle supervisor calls (i.e. ecalls
	 * from VS-mode), Guest page faults and Virtual interrupts.
	 */
	اگر (misa_extension('H')) {
		exceptions |= (1U << CAUSE_VIRTUAL_SUPERVISOR_ECALL);
		exceptions |= (1U << CAUSE_FETCH_GUEST_صفحہ_FAULT);
		exceptions |= (1U << CAUSE_بھرو_GUEST_صفحہ_FAULT);
		exceptions |= (1U << CAUSE_VIRTUAL_INST_FAULT);
		exceptions |= (1U << CAUSE_STORE_GUEST_صفحہ_FAULT);
	}

	csr_write(CSR_MIDELEG, interrupts);
	csr_write(CSR_MEDELEG, exceptions);

	واپس 0;
}

صحیح sbi_trap_init(ساخت sbi_scratch *scratch, bool cold_boot)
{
	واپس delegate_traps(scratch);
}
