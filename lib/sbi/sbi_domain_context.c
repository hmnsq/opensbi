/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) IPADS@SJTU 2023. All rights reserved.
 */

#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/riscv_locks.h>
#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_hsm.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_hart_protection.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi/sbi_string.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_domain_context.h>
#అన్తర్భావ <sbi/sbi_platform.h>
#అన్తర్భావ <sbi/sbi_trap.h>
#అన్తర్భావ <sbi/sbi_vector.h>
#అన్తర్భావ <sbi/sbi_fp.h>

/** Context representation for a hart within a domain */
నిర్మాణం hart_context {
	/** Trap-related states such as GPRs, mepc, and mstatus */
	నిర్మాణం sbi_trap_context trap_ctx;

	/** Supervisor status register */
	గుర్తులేని దీర్ఘ sstatus;
	/** Supervisor interrupt enable register */
	గుర్తులేని దీర్ఘ sie;
	/** Supervisor trap vector base address register */
	గుర్తులేని దీర్ఘ stvec;
	/** Supervisor scratch register for temporary storage */
	గుర్తులేని దీర్ఘ sscratch;
	/** Supervisor exception program counter register */
	గుర్తులేని దీర్ఘ sepc;
	/** Supervisor cause register */
	గుర్తులేని దీర్ఘ scause;
	/** Supervisor trap value register */
	గుర్తులేని దీర్ఘ stval;
	/** Supervisor interrupt pending register */
	గుర్తులేని దీర్ఘ sip;
	/** Supervisor address translation and protection register */
	గుర్తులేని దీర్ఘ satp;
	/** Counter-enable register */
	గుర్తులేని దీర్ఘ scounteren;
	/** Supervisor environment configuration register */
	గుర్తులేని దీర్ఘ senvcfg;
	/** Supervisor resource management configuration register */
	గుర్తులేని దీర్ఘ srmcfg;

	/** Float context state */
	నిర్మాణం sbi_fp_context fp_ctx;
	/** Vector context state */
	నిర్మాణం sbi_సదిశ_context *vec_ctx;

	/** Reference to the owning domain */
	నిర్మాణం sbi_domain *dom;
	/** Previous context (caller) to jump to during context exits */
	నిర్మాణం hart_context *prev_ctx;
	/** Is context initialized and runnable */
	bool initialized;
};

స్థిర నిర్మాణం sbi_domain_state dcstate;

స్థిర అంతఃవరుస నిర్మాణం hart_context *hart_context_get(నిర్మాణం sbi_domain *dom,
						    u32 hartindex)
{
	నిర్మాణం hart_context **dom_hartindex_గమ్యం_context_పట్టిక;

	dom_hartindex_గమ్యం_context_పట్టిక = sbi_domain_state_ptr(dom, &dcstate);
	ఒకవేళ (!dom_hartindex_గమ్యం_context_పట్టిక || !sbi_hartindex_valid(hartindex))
		తిరిగి ఖాళీ;

	తిరిగి dom_hartindex_గమ్యం_context_పట్టిక[hartindex];
}

స్థిర శూన్య hart_context_సమితి(నిర్మాణం sbi_domain *dom, u32 hartindex,
			     నిర్మాణం hart_context *hc)
{
	నిర్మాణం hart_context **dom_hartindex_గమ్యం_context_పట్టిక;

	dom_hartindex_గమ్యం_context_పట్టిక = sbi_domain_state_ptr(dom, &dcstate);
	ఒకవేళ (!dom_hartindex_గమ్యం_context_పట్టిక || !sbi_hartindex_valid(hartindex))
		తిరిగి;

	dom_hartindex_గమ్యం_context_పట్టిక[hartindex] = hc;
}

/** Macro to obtain the current hart's context pointer */
#నిర్వచించు hart_context_thishart_get()					\
	hart_context_get(sbi_domain_thishart_ptr(),			\
			 current_hartindex())

/**
 * Switches the HART context from the current domain to the target domain.
 * This includes changing domain assignments and reconfiguring PMP, as well
 * as saving and restoring CSRs and trap states.
 *
 * @param ctx pointer to the current HART context
 * @param dom_ctx pointer to the target domain context
 *
 * @return 0 on success and negative error code on failure
 */
స్థిర పూర్ణాంకం switch_గమ్యం_తదుపరి_domain_context(నిర్మాణం hart_context *ctx,
					  నిర్మాణం hart_context *dom_ctx)
	MUST_NOT_HOLD(&ctx->dom->assigned_harts_తాళం)
	MUST_NOT_HOLD(&dom_ctx->dom->assigned_harts_తాళం)
{
	u32 hartindex = current_hartindex();
	నిర్మాణం sbi_trap_context *trap_ctx;
	నిర్మాణం sbi_domain *current_dom, *లక్ష్యం_dom;
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ఒకవేళ (!ctx || !dom_ctx || ctx == dom_ctx)
		తిరిగి SBI_EINVAL;

	current_dom = ctx->dom;
	లక్ష్యం_dom = dom_ctx->dom;
	/* Assign current hart to target domain */
	spin_తాళం(&current_dom->assigned_harts_తాళం);
	sbi_hartmask_clear_hartindex(hartindex, &current_dom->assigned_harts);
	spin_unlock(&current_dom->assigned_harts_తాళం);

	sbi_update_hartindex_గమ్యం_domain(hartindex, లక్ష్యం_dom);

	spin_తాళం(&లక్ష్యం_dom->assigned_harts_తాళం);
	sbi_hartmask_సమితి_hartindex(hartindex, &లక్ష్యం_dom->assigned_harts);
	spin_unlock(&లక్ష్యం_dom->assigned_harts_తాళం);

	/* Save current CSR context and restore target domain's CSR context */
	ctx->sstatus	= csr_మార్పిడి(CSR_SSTATUS, dom_ctx->sstatus);
	ctx->sie	= csr_మార్పిడి(CSR_SIE, dom_ctx->sie);
	ctx->stvec	= csr_మార్పిడి(CSR_STVEC, dom_ctx->stvec);
	ctx->sscratch	= csr_మార్పిడి(CSR_SSCRATCH, dom_ctx->sscratch);
	ctx->sepc	= csr_మార్పిడి(CSR_SEPC, dom_ctx->sepc);
	ctx->scause	= csr_మార్పిడి(CSR_SCAUSE, dom_ctx->scause);
	ctx->stval	= csr_మార్పిడి(CSR_STVAL, dom_ctx->stval);
	ctx->sip	= csr_మార్పిడి(CSR_SIP, dom_ctx->sip);
	ctx->satp	= csr_మార్పిడి(CSR_SATP, dom_ctx->satp);
	ఒకవేళ (sbi_hart_priv_సంస్కరణ(scratch) >= SBI_HART_PRIV_VER_1_10)
		ctx->scounteren = csr_మార్పిడి(CSR_SCOUNTEREN, dom_ctx->scounteren);
	ఒకవేళ (sbi_hart_priv_సంస్కరణ(scratch) >= SBI_HART_PRIV_VER_1_12)
		ctx->senvcfg	= csr_మార్పిడి(CSR_SENVCFG, dom_ctx->senvcfg);
	ఒకవేళ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSQOSID))
		ctx->srmcfg	= csr_మార్పిడి(CSR_SRMCFG, dom_ctx->srmcfg);

	/* Eager context switch for float */
	ఒకవేళ (sbi_hart_has_extension(scratch, SBI_HART_EXT_F) ||
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_D)) {
		sbi_fp_save(&ctx->fp_ctx);
		sbi_fp_restore(&dom_ctx->fp_ctx);
	}

	/* Eager context switch for vector */
	ఒకవేళ (sbi_hart_has_extension(scratch, SBI_HART_EXT_V)) {
		sbi_సదిశ_save(ctx->vec_ctx);
		sbi_సదిశ_restore(dom_ctx->vec_ctx);
	}

	/* Save current trap state and restore target domain's trap state */
	trap_ctx = sbi_trap_get_context(scratch);
	sbi_memcpy(&ctx->trap_ctx, trap_ctx, పరిమాణం(*trap_ctx));
	sbi_memcpy(trap_ctx, &dom_ctx->trap_ctx, పరిమాణం(*trap_ctx));

	/*
	 * Re-configure PMP settings for the new domain
	 *
	 * This will internally perform full SFENCE / HFENCE which
	 * is also required for some of the above CSR updates (such
	 * as satp CSR).
	 */
	sbi_hart_protection_reconfigure(scratch, current_dom, లక్ష్యం_dom);

	/* Mark current context structure initialized because context saved */
	ctx->initialized = true;

	/* If target domain context is not initialized or runnable */
	ఒకవేళ (!dom_ctx->initialized) {
		/* Startup boot HART of target domain */
		ఒకవేళ (current_hartid() == లక్ష్యం_dom->boot_hartid)
			sbi_hart_switch_బహుళకం(లక్ష్యం_dom->boot_hartid,
					     లక్ష్యం_dom->తదుపరి_arg1,
					     లక్ష్యం_dom->తదుపరి_addr,
					     లక్ష్యం_dom->తదుపరి_బహుళకం,
					     false);
		లేకపోతే
			sbi_hsm_hart_stop(scratch, true);
	}
	తిరిగి 0;
}

స్థిర పూర్ణాంకం hart_context_init(u32 hartindex)
{
	size_t vec_కొలత;
	నిర్మాణం hart_context *ctx;
	నిర్మాణం sbi_domain *dom;

	sbi_domain_for_each(dom) {
		ఒకవేళ (!sbi_hartmask_test_hartindex(hartindex,
						 dom->possible_harts))
			కొనసాగు;

		ctx = sbi_zalloc(పరిమాణం(నిర్మాణం hart_context));
		ఒకవేళ (!ctx)
			తిరిగి SBI_ENOMEM;

		ఒకవేళ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
					   SBI_HART_EXT_V)) {
			vec_కొలత = sbi_సదిశ_context_కొలత();

			/* Allocate the vector context pointer */
			ctx->vec_ctx = sbi_zalloc(vec_కొలత);
			ఒకవేళ (!ctx->vec_ctx) {
				sbi_free(ctx);
				తిరిగి SBI_ENOMEM;
			}
		}

		/* Bind context and domain */
		ctx->dom = dom;
		hart_context_సమితి(dom, hartindex, ctx);
	}

	తిరిగి 0;
}

పూర్ణాంకం sbi_domain_context_enter(నిర్మాణం sbi_domain *dom)
{
	పూర్ణాంకం rc;
	నిర్మాణం hart_context *dom_ctx;
	నిర్మాణం hart_context *ctx = hart_context_thishart_get();
	u32 hartindex = current_hartindex();

	/* Target domain must not be same as the current domain */
	ఒకవేళ (!dom || dom == sbi_domain_thishart_ptr())
		తిరిగి SBI_EINVAL;

	/* Target domain must have current hart as a possible harts */
	ఒకవేళ (!sbi_hartmask_test_hartindex(hartindex, dom->possible_harts))
		తిరిగి SBI_EINVAL;

	/*
	 * If it's first time to call `enter` on the current hart, no
	 * context allocated before. Allocate context for each valid
	 * domain on the current hart.
	 */
	ఒకవేళ (!ctx) {
		rc = hart_context_init(hartindex);
		ఒకవేళ (rc)
			తిరిగి rc;

		ctx = hart_context_thishart_get();
		ఒకవేళ (!ctx)
			తిరిగి SBI_EINVAL;
	}

	dom_ctx = hart_context_get(dom, hartindex);
	/* Validate the domain context existence */
	ఒకవేళ (!dom_ctx)
		తిరిగి SBI_EINVAL;

	/* Update target context's previous context to indicate the caller */
	dom_ctx->prev_ctx = ctx;

	తిరిగి switch_గమ్యం_తదుపరి_domain_context(ctx, dom_ctx);
}

పూర్ణాంకం sbi_domain_context_exit(శూన్య)
{
	పూర్ణాంకం rc;
	u32 hartindex = current_hartindex();
	నిర్మాణం sbi_domain *dom;
	నిర్మాణం hart_context *ctx = hart_context_thishart_get();
	నిర్మాణం hart_context *dom_ctx, *tmp;

	/*
	 * If it's first time to call `exit` on the current hart, no
	 * context allocated before. Loop through each domain to allocate
	 * its context on the current hart if valid.
	 */
	ఒకవేళ (!ctx) {
		rc = hart_context_init(hartindex);
		ఒకవేళ (rc)
			తిరిగి rc;

		ctx = hart_context_thishart_get();
		ఒకవేళ (!ctx)
			తిరిగి SBI_EINVAL;
	}

	dom_ctx = ctx->prev_ctx;
	ctx->prev_ctx = ఖాళీ;

	/* If no previous caller context */
	ఒకవేళ (!dom_ctx) {
		/* Try to find next uninitialized domain with least initialization order */
		dom_ctx = ఖాళీ;
		sbi_domain_for_each(dom) {
			ఒకవేళ (dom == sbi_domain_thishart_ptr())
				కొనసాగు;

			ఒకవేళ (!sbi_hartmask_test_hartindex(hartindex, dom->possible_harts))
				కొనసాగు;

			tmp = hart_context_get(dom, hartindex);
			ఒకవేళ (tmp && tmp->initialized)
				కొనసాగు;

			ఒకవేళ (!dom_ctx || tmp->dom->init_క్రమం < dom_ctx->dom->init_క్రమం)
				dom_ctx = tmp;
		}
	}
	ఒకవేళ (!dom_ctx)
		తిరిగి SBI_ENOENT;

	తిరిగి switch_గమ్యం_తదుపరి_domain_context(ctx, dom_ctx);
}

పూర్ణాంకం sbi_domain_context_init(శూన్య)
{
	/**
	 * Allocate per-domain and per-hart context data.
	 * The data type is "struct hart_context **" whose memory space will be
	 * dynamically allocated by domain_setup_state_one(). Calculate needed
	 * size of memory space here.
	 */
	dcstate.state_కొలత = పరిమాణం(నిర్మాణం hart_context *) * sbi_hart_లెక్క();

	తిరిగి sbi_domain_రిజిస్టరు_state(&dcstate);
}

శూన్య sbi_domain_context_deinit(శూన్య)
{
	sbi_domain_unregister_state(&dcstate);
}
