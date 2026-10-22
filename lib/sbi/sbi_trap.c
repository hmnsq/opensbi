/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_encoding.h>
#అన్తర్భావ <sbi/sbi_bitops.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_double_trap.h>
#అన్తర్భావ <sbi/sbi_ecall.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_illegal_insn.h>
#అన్తర్భావ <sbi/sbi_ipi.h>
#అన్తర్భావ <sbi/sbi_irqchip.h>
#అన్తర్భావ <sbi/sbi_trap_ldst.h>
#అన్తర్భావ <sbi/sbi_pmu.h>
#అన్తర్భావ <sbi/sbi_platform.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi/sbi_sse.h>
#అన్తర్భావ <sbi/sbi_timer.h>
#అన్తర్భావ <sbi/sbi_trap.h>

స్థిర శూన్య sbi_trap_error_one(స్థిరరాశి నిర్మాణం sbi_trap_context *tcntx,
			       స్థిరరాశి అక్షరం *ఉపసర్గ, u32 hartid, u32 లోతు)
{
	స్థిరరాశి నిర్మాణం sbi_trap_info *trap = &tcntx->trap;
	స్థిరరాశి నిర్మాణం sbi_trap_regs *regs = &tcntx->regs;

	sbi_printf("\n");
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ఉపసర్గ,
		   hartid, లోతు, "mcause", trap->cause, "mtval", trap->tval);
	ఒకవేళ (misa_extension('H')) {
		sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ఉపసర్గ,
			   hartid, లోతు, "mtval2", trap->tval2, "mtinst", trap->tinst);
	}
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ఉపసర్గ,
		   hartid, లోతు, "mepc", regs->mepc, "mstatus", regs->mstatus);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ఉపసర్గ,
		   hartid, లోతు, "ra", regs->ra, "sp", regs->sp);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ఉపసర్గ,
		   hartid, లోతు, "gp", regs->gp, "tp", regs->tp);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ఉపసర్గ,
		   hartid, లోతు, "s0", regs->s0, "s1", regs->s1);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ఉపసర్గ,
		   hartid, లోతు, "a0", regs->a0, "a1", regs->a1);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ఉపసర్గ,
		   hartid, లోతు, "a2", regs->a2, "a3", regs->a3);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ఉపసర్గ,
		   hartid, లోతు, "a4", regs->a4, "a5", regs->a5);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ఉపసర్గ,
		   hartid, లోతు, "a6", regs->a6, "a7", regs->a7);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ఉపసర్గ,
		   hartid, లోతు, "s2", regs->s2, "s3", regs->s3);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ఉపసర్గ,
		   hartid, లోతు, "s4", regs->s4, "s5", regs->s5);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ఉపసర్గ,
		   hartid, లోతు, "s6", regs->s6, "s7", regs->s7);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ఉపసర్గ,
		   hartid, లోతు, "s8", regs->s8, "s9", regs->s9);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ఉపసర్గ,
		   hartid, లోతు, "s10", regs->s10, "s11", regs->s11);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ఉపసర్గ,
		   hartid, లోతు, "t0", regs->t0, "t1", regs->t1);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ఉపసర్గ,
		   hartid, లోతు, "t2", regs->t2, "t3", regs->t3);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ఉపసర్గ,
		   hartid, లోతు, "t4", regs->t4, "t5", regs->t5);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX "\n", ఉపసర్గ,
		   hartid, లోతు, "t6", regs->t6);
}

స్థిర శూన్య __noreturn sbi_trap_error(స్థిరరాశి అక్షరం *సందేశం, పూర్ణాంకం rc,
				      స్థిరరాశి నిర్మాణం sbi_trap_context *tcntx)
{
	u32 లోతు = 0, hartid = current_hartid();
	స్థిరరాశి నిర్మాణం sbi_trap_context *tc;

	కొరకు (tc = tcntx; tc; tc = tc->prev_context)
		లోతు++;

	sbi_printf("\n");
	sbi_printf("%s: hart%d: trap%d: %s (error %d)\n", __func__,
		   hartid, లోతు - 1, సందేశం, rc);
	కొరకు (tc = tcntx; tc; tc = tc->prev_context)
		sbi_trap_error_one(tc, __func__, hartid, --లోతు);

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
పూర్ణాంకం sbi_trap_redirect(నిర్మాణం sbi_trap_regs *regs,
		      స్థిరరాశి నిర్మాణం sbi_trap_info *trap)
{
	ulong hstatus, vsstatus, prev_బహుళకం;
	bool elp = false;
	bool prev_virt = sbi_regs_from_virt(regs);
	/* By default, we redirect to HS-mode */
	bool తదుపరి_virt = false;

	/* Sanity check on previous mode */
	prev_బహుళకం = sbi_mstatus_prev_బహుళకం(regs->mstatus);
	ఒకవేళ (prev_బహుళకం != PRV_S && prev_బహుళకం != PRV_U)
		తిరిగి SBI_ENOTSUPP;

	/* If hart support for zicfilp, clear MPELP because redirecting to VS or (H)S */
	ఒకవేళ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_ZICFILP)) {
#ఒకవేళ __riscv_xlen == 32
		elp = regs->mstatusH & MSTATUSH_MPELP;
		regs->mstatusH &= ~MSTATUSH_MPELP;
#లేకపోతే
		elp = regs->mstatus & MSTATUS_MPELP;
		regs->mstatus &= ~MSTATUS_MPELP;
#అయితే_ముగింపు
	}

	/* If exceptions came from VS/VU-mode, redirect to VS-mode if
	 * delegated in hedeleg
	 */
	ఒకవేళ (misa_extension('H') && prev_virt) {
		ఒకవేళ ((trap->cause < __riscv_xlen) &&
		    (csr_read(CSR_HEDELEG) & బిట్టు(trap->cause))) {
			తదుపరి_virt = true;
		}
	}

	/* Update MSTATUS MPV bits */
#ఒకవేళ __riscv_xlen == 32
	regs->mstatusH &= ~MSTATUSH_MPV;
	regs->mstatusH |= (తదుపరి_virt) ? MSTATUSH_MPV : 0UL;
#లేకపోతే
	regs->mstatus &= ~MSTATUS_MPV;
	regs->mstatus |= (తదుపరి_virt) ? MSTATUS_MPV : 0UL;
#అయితే_ముగింపు

	/* Update hypervisor CSRs if going to HS-mode */
	ఒకవేళ (misa_extension('H') && !తదుపరి_virt) {
		hstatus = csr_read(CSR_HSTATUS);
		ఒకవేళ (prev_virt) {
			/* hstatus.SPVP is only updated if coming from VS/VU-mode */
			hstatus &= ~HSTATUS_SPVP;
			hstatus |= (prev_బహుళకం == PRV_S) ? HSTATUS_SPVP : 0;
		}
		hstatus &= ~HSTATUS_SPV;
		hstatus |= (prev_virt) ? HSTATUS_SPV : 0;
		hstatus &= ~HSTATUS_GVA;
		hstatus |= (trap->gva) ? HSTATUS_GVA : 0;
		csr_write(CSR_HSTATUS, hstatus);
		csr_write(CSR_HTVAL, trap->tval2);
		csr_write(CSR_HTINST, trap->tinst);
	}

	/* Update exception related CSRs */
	ఒకవేళ (తదుపరి_virt) {
		/* Update VS-mode exception info */
		csr_write(CSR_VSTVAL, trap->tval);
		csr_write(CSR_VSEPC, regs->mepc);
		csr_write(CSR_VSCAUSE, trap->cause);

		/* Set MEPC to VS-mode exception vector base */
		regs->mepc = csr_read(CSR_VSTVEC) & ~MTVEC_బహుళకం;

		/* Set MPP to VS-mode */
		regs->mstatus &= ~MSTATUS_MPP;
		regs->mstatus |= (PRV_S << MSTATUS_MPP_జరుపు);

		/* Get VS-mode SSTATUS CSR */
		vsstatus = csr_read(CSR_VSSTATUS);

		/* If elp was set, set it back in vsstatus */
		ఒకవేళ (elp)
			vsstatus |= MSTATUS_SPELP;

		/* Set SPP for VS-mode */
		vsstatus &= ~SSTATUS_SPP;
		ఒకవేళ (prev_బహుళకం == PRV_S)
			vsstatus |= (1UL << SSTATUS_SPP_జరుపు);

		/* Set SPIE for VS-mode */
		vsstatus &= ~SSTATUS_SPIE;
		ఒకవేళ (vsstatus & SSTATUS_SIE)
			vsstatus |= (1UL << SSTATUS_SPIE_జరుపు);

		/* Clear SIE for VS-mode */
		vsstatus &= ~SSTATUS_SIE;

		/* Update VS-mode SSTATUS CSR */
		csr_write(CSR_VSSTATUS, vsstatus);
	} లేకపోతే {
		/* Update S-mode exception info */
		csr_write(CSR_STVAL, trap->tval);
		csr_write(CSR_SEPC, regs->mepc);
		csr_write(CSR_SCAUSE, trap->cause);

		/* Set MEPC to S-mode exception vector base */
		regs->mepc = csr_read(CSR_STVEC) & ~MTVEC_బహుళకం;

		/* Set MPP to S-mode */
		regs->mstatus &= ~MSTATUS_MPP;
		regs->mstatus |= (PRV_S << MSTATUS_MPP_జరుపు);

		/* Set SPP for S-mode */
		regs->mstatus &= ~MSTATUS_SPP;
		ఒకవేళ (prev_బహుళకం == PRV_S)
			regs->mstatus |= (1UL << MSTATUS_SPP_జరుపు);

		/* Set SPIE for S-mode */
		regs->mstatus &= ~MSTATUS_SPIE;
		ఒకవేళ (regs->mstatus & MSTATUS_SIE)
			regs->mstatus |= (1UL << MSTATUS_SPIE_జరుపు);

		/* Clear SIE for S-mode */
		regs->mstatus &= ~MSTATUS_SIE;

		/* If elp was set, set it back in mstatus */
		ఒకవేళ (elp)
			regs->mstatus |= MSTATUS_SPELP;
	}

	తిరిగి 0;
}

స్థిర పూర్ణాంకం sbi_trap_nonaia_irq(గుర్తులేని దీర్ఘ irq)
{
	ఎంపిక (irq) {
	స్థితి IRQ_M_TIMER:
		sbi_timer_process();
		ఆపు;
	స్థితి IRQ_M_SOFT:
		sbi_ipi_process();
		ఆపు;
	స్థితి IRQ_M_EXT:
		తిరిగి sbi_irqchip_process();
	అప్రమేయ:
		ఒకవేళ (irq == sbi_pmu_irq_బిట్టు()) {
			sbi_pmu_ovf_irq();
			తిరిగి 0;
		}
		తిరిగి SBI_ENOENT;
	}

	తిరిగి 0;
}

స్థిర పూర్ణాంకం sbi_trap_aia_irq(శూన్య)
{
	పూర్ణాంకం rc;
	గుర్తులేని దీర్ఘ mtopi;

	అయితే ((mtopi = csr_read(CSR_MTOPI))) {
		mtopi = mtopi >> TOPI_IID_జరుపు;
		ఎంపిక (mtopi) {
		స్థితి IRQ_M_TIMER:
			sbi_timer_process();
			ఆపు;
		స్థితి IRQ_M_SOFT:
			sbi_ipi_process();
			ఆపు;
		స్థితి IRQ_M_EXT:
			rc = sbi_irqchip_process();
			ఒకవేళ (rc)
				తిరిగి rc;
			ఆపు;
		అప్రమేయ:
			ఒకవేళ (mtopi == sbi_pmu_irq_బిట్టు()) {
				sbi_pmu_ovf_irq();
				ఆపు;
			}

			తిరిగి SBI_ENOENT;
		}
	}

	తిరిగి 0;
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
నిర్మాణం sbi_trap_context *sbi_trap_handler(నిర్మాణం sbi_trap_context *tcntx)
{
	పూర్ణాంకం rc = SBI_ENOTSUPP;
	స్థిరరాశి అక్షరం *సందేశం = "trap handler failed";
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	స్థిరరాశి నిర్మాణం sbi_trap_info *trap = &tcntx->trap;
	నిర్మాణం sbi_trap_regs *regs = &tcntx->regs;
	ulong mcause = tcntx->trap.cause;

	/* Update trap context pointer */
	tcntx->prev_context = sbi_trap_get_context(scratch);
	sbi_trap_సమితి_context(scratch, tcntx);

	ఒకవేళ (mcause & MCAUSE_IRQ_ముసుగు) {
		ఒకవేళ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
					   SBI_HART_EXT_SMAIA))
			rc = sbi_trap_aia_irq();
		లేకపోతే
			rc = sbi_trap_nonaia_irq(mcause & ~MCAUSE_IRQ_ముసుగు);
		సందేశం = "unhandled local interrupt";
		వెళ్ళు trap_పూర్తి;
	}

	ఎంపిక (mcause) {
	స్థితి CAUSE_ILLEGAL_INSTRUCTION:
		rc  = sbi_illegal_insn_handler(tcntx);
		సందేశం = "illegal instruction handler failed";
		ఆపు;
	స్థితి CAUSE_MISALIGNED_ఎక్కించు:
		sbi_pmu_ctr_incr_fw(SBI_PMU_FW_MISALIGNED_ఎక్కించు);
		rc  = sbi_misaligned_ఎక్కించు_handler(tcntx);
		సందేశం = "misaligned load handler failed";
		ఆపు;
	స్థితి CAUSE_MISALIGNED_STORE:
		sbi_pmu_ctr_incr_fw(SBI_PMU_FW_MISALIGNED_STORE);
		rc  = sbi_misaligned_store_handler(tcntx);
		సందేశం = "misaligned store handler failed";
		ఆపు;
	స్థితి CAUSE_SUPERVISOR_ECALL:
	స్థితి CAUSE_MACHINE_ECALL:
		rc  = sbi_ecall_handler(tcntx);
		సందేశం = "ecall handler failed";
		ఆపు;
	స్థితి CAUSE_ఎక్కించు_ACCESS:
		sbi_pmu_ctr_incr_fw(SBI_PMU_FW_ACCESS_ఎక్కించు);
		rc  = sbi_ఎక్కించు_access_handler(tcntx);
		సందేశం = "load fault handler failed";
		ఆపు;
	స్థితి CAUSE_STORE_ACCESS:
		sbi_pmu_ctr_incr_fw(SBI_PMU_FW_ACCESS_STORE);
		rc  = sbi_store_access_handler(tcntx);
		సందేశం = "store fault handler failed";
		ఆపు;
	స్థితి CAUSE_DOUBLE_TRAP:
		rc  = sbi_double_trap_handler(tcntx);
		సందేశం = "double trap handler failed";
		ఆపు;
	అప్రమేయ:
		/* If the trap came from S or U mode, redirect it there */
		సందేశం = "trap redirect failed";
		rc  = sbi_trap_redirect(regs, trap);
		ఆపు;
	}

trap_పూర్తి:
	ఒకవేళ (rc)
		sbi_trap_error(సందేశం, rc, tcntx);

	ఒకవేళ (sbi_mstatus_prev_బహుళకం(regs->mstatus) != PRV_M)
		sbi_sse_process_pending_events(regs);

	sbi_trap_సమితి_context(scratch, tcntx->prev_context);
	తిరిగి tcntx;
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
నిర్మాణం sbi_trap_context *sbi_trap_rnmi_handler(నిర్మాణం sbi_trap_context *tcntx)
{
	పూర్ణాంకం rc;
	స్థిరరాశి నిర్మాణం sbi_platform *plat = sbi_platform_thishart_ptr();
	స్థిరరాశి నిర్మాణం sbi_platform_operations *ops = sbi_platform_ops(plat);

	/* Call platform-specific NMI handler if registered */
	ఒకవేళ (ops && ops->rnmi_handler) {
		rc = ops->rnmi_handler(tcntx);
		ఒకవేళ (rc) {
			/* Platform handler failed to handle NMI */
			sbi_trap_error("platform NMI handler failed", rc, tcntx);
		}
		తిరిగి tcntx;
	}

	/* No platform handler - treat as unhandled NMI */
	sbi_trap_error("unhandled NMI (no platform rnmi_handler)",
		       SBI_ENOTSUPP, tcntx);

	/* Never returns */
	తిరిగి tcntx;
}

స్థిర పూర్ణాంకం delegate_traps(నిర్మాణం sbi_scratch *scratch)
{
	స్థిరరాశి నిర్మాణం sbi_platform *plat = sbi_platform_ptr(scratch);
	గుర్తులేని దీర్ఘ interrupts, exceptions;

	ఒకవేళ (!misa_extension('S'))
		/* No delegation possible as mideleg does not exist */
		తిరిగి 0;

	/* Send M-mode interrupts and most exceptions to S-mode */
	interrupts = MIP_SSIP | MIP_STIP | MIP_SEIP;
	interrupts |= sbi_pmu_irq_ముసుగు();

	exceptions = (1U << CAUSE_MISALIGNED_FETCH) | (1U << CAUSE_BREAKPOINT) |
		     (1U << CAUSE_USER_ECALL);
	ఒకవేళ (sbi_platform_has_mfaults_delegation(plat))
		exceptions |= (1U << CAUSE_FETCH_పుట_FAULT) |
			      (1U << CAUSE_ఎక్కించు_పుట_FAULT) |
			      (1U << CAUSE_STORE_పుట_FAULT) |
			      (1U << CAUSE_SW_CHECK_EXCP);

	/*
	 * If hypervisor extension available then we only handle hypervisor
	 * calls (i.e. ecalls from HS-mode) in M-mode.
	 *
	 * The HS-mode will additionally handle supervisor calls (i.e. ecalls
	 * from VS-mode), Guest page faults and Virtual interrupts.
	 */
	ఒకవేళ (misa_extension('H')) {
		exceptions |= (1U << CAUSE_VIRTUAL_SUPERVISOR_ECALL);
		exceptions |= (1U << CAUSE_FETCH_GUEST_పుట_FAULT);
		exceptions |= (1U << CAUSE_ఎక్కించు_GUEST_పుట_FAULT);
		exceptions |= (1U << CAUSE_VIRTUAL_INST_FAULT);
		exceptions |= (1U << CAUSE_STORE_GUEST_పుట_FAULT);
	}

	csr_write(CSR_MIDELEG, interrupts);
	csr_write(CSR_MEDELEG, exceptions);

	తిరిగి 0;
}

పూర్ణాంకం sbi_trap_init(నిర్మాణం sbi_scratch *scratch, bool cold_boot)
{
	తిరిగి delegate_traps(scratch);
}
