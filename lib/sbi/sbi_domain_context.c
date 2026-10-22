/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) IPADS@SJTU 2023. All rights reserved.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_locks.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hsm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart_protection.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain_context.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_platform.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_vector.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_fp.h>

/** Context representation for a hart within a domain */
ਬਣਤਰ hart_context {
	/** Trap-related states such as GPRs, mepc, and mstatus */
	ਬਣਤਰ sbi_trap_context trap_ctx;

	/** Supervisor status register */
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sstatus;
	/** Supervisor interrupt enable register */
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sie;
	/** Supervisor trap vector base address register */
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ stvec;
	/** Supervisor scratch register for temporary storage */
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sscratch;
	/** Supervisor exception program counter register */
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sepc;
	/** Supervisor cause register */
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ scause;
	/** Supervisor trap value register */
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ stval;
	/** Supervisor interrupt pending register */
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sip;
	/** Supervisor address translation and protection register */
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ satp;
	/** Counter-enable register */
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ scounteren;
	/** Supervisor environment configuration register */
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ senvcfg;
	/** Supervisor resource management configuration register */
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ srmcfg;

	/** Float context state */
	ਬਣਤਰ sbi_fp_context fp_ctx;
	/** Vector context state */
	ਬਣਤਰ sbi_ਸਦਿਸ਼_context *vec_ctx;

	/** Reference to the owning domain */
	ਬਣਤਰ sbi_domain *dom;
	/** Previous context (caller) to jump to during context exits */
	ਬਣਤਰ hart_context *prev_ctx;
	/** Is context initialized and runnable */
	bool initialized;
};

ਸਥਿਰ ਬਣਤਰ sbi_domain_state dcstate;

ਸਥਿਰ ਸਤਰਅੰਦਰ ਬਣਤਰ hart_context *hart_context_get(ਬਣਤਰ sbi_domain *dom,
						    u32 hartindex)
{
	ਬਣਤਰ hart_context **dom_hartindex_ਮੰਜ਼ਿਲ_context_ਸਾਰਣੀ;

	dom_hartindex_ਮੰਜ਼ਿਲ_context_ਸਾਰਣੀ = sbi_domain_state_ptr(dom, &dcstate);
	ਜੇ (!dom_hartindex_ਮੰਜ਼ਿਲ_context_ਸਾਰਣੀ || !sbi_hartindex_valid(hartindex))
		ਵਾਪਸ ਨਿਰਾ;

	ਵਾਪਸ dom_hartindex_ਮੰਜ਼ਿਲ_context_ਸਾਰਣੀ[hartindex];
}

ਸਥਿਰ ਖਾਲੀ hart_context_ਸੈੱਟ(ਬਣਤਰ sbi_domain *dom, u32 hartindex,
			     ਬਣਤਰ hart_context *hc)
{
	ਬਣਤਰ hart_context **dom_hartindex_ਮੰਜ਼ਿਲ_context_ਸਾਰਣੀ;

	dom_hartindex_ਮੰਜ਼ਿਲ_context_ਸਾਰਣੀ = sbi_domain_state_ptr(dom, &dcstate);
	ਜੇ (!dom_hartindex_ਮੰਜ਼ਿਲ_context_ਸਾਰਣੀ || !sbi_hartindex_valid(hartindex))
		ਵਾਪਸ;

	dom_hartindex_ਮੰਜ਼ਿਲ_context_ਸਾਰਣੀ[hartindex] = hc;
}

/** Macro to obtain the current hart's context pointer */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ hart_context_thishart_get()					\
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
ਸਥਿਰ ਪੂਰਨ switch_ਮੰਜ਼ਿਲ_ਅਗਲਾ_domain_context(ਬਣਤਰ hart_context *ctx,
					  ਬਣਤਰ hart_context *dom_ctx)
	MUST_NOT_HOLD(&ctx->dom->assigned_harts_ਤਾਲਾ)
	MUST_NOT_HOLD(&dom_ctx->dom->assigned_harts_ਤਾਲਾ)
{
	u32 hartindex = current_hartindex();
	ਬਣਤਰ sbi_trap_context *trap_ctx;
	ਬਣਤਰ sbi_domain *current_dom, *ਨਿਸ਼ਾਨਾ_dom;
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ਜੇ (!ctx || !dom_ctx || ctx == dom_ctx)
		ਵਾਪਸ SBI_EINVAL;

	current_dom = ctx->dom;
	ਨਿਸ਼ਾਨਾ_dom = dom_ctx->dom;
	/* Assign current hart to target domain */
	spin_ਤਾਲਾ(&current_dom->assigned_harts_ਤਾਲਾ);
	sbi_hartmask_clear_hartindex(hartindex, &current_dom->assigned_harts);
	spin_unlock(&current_dom->assigned_harts_ਤਾਲਾ);

	sbi_update_hartindex_ਮੰਜ਼ਿਲ_domain(hartindex, ਨਿਸ਼ਾਨਾ_dom);

	spin_ਤਾਲਾ(&ਨਿਸ਼ਾਨਾ_dom->assigned_harts_ਤਾਲਾ);
	sbi_hartmask_ਸੈੱਟ_hartindex(hartindex, &ਨਿਸ਼ਾਨਾ_dom->assigned_harts);
	spin_unlock(&ਨਿਸ਼ਾਨਾ_dom->assigned_harts_ਤਾਲਾ);

	/* Save current CSR context and restore target domain's CSR context */
	ctx->sstatus	= csr_ਅਦਲਾ_ਬਦਲੀ(CSR_SSTATUS, dom_ctx->sstatus);
	ctx->sie	= csr_ਅਦਲਾ_ਬਦਲੀ(CSR_SIE, dom_ctx->sie);
	ctx->stvec	= csr_ਅਦਲਾ_ਬਦਲੀ(CSR_STVEC, dom_ctx->stvec);
	ctx->sscratch	= csr_ਅਦਲਾ_ਬਦਲੀ(CSR_SSCRATCH, dom_ctx->sscratch);
	ctx->sepc	= csr_ਅਦਲਾ_ਬਦਲੀ(CSR_SEPC, dom_ctx->sepc);
	ctx->scause	= csr_ਅਦਲਾ_ਬਦਲੀ(CSR_SCAUSE, dom_ctx->scause);
	ctx->stval	= csr_ਅਦਲਾ_ਬਦਲੀ(CSR_STVAL, dom_ctx->stval);
	ctx->sip	= csr_ਅਦਲਾ_ਬਦਲੀ(CSR_SIP, dom_ctx->sip);
	ctx->satp	= csr_ਅਦਲਾ_ਬਦਲੀ(CSR_SATP, dom_ctx->satp);
	ਜੇ (sbi_hart_priv_ਸੰਸਕਰਣ(scratch) >= SBI_HART_PRIV_VER_1_10)
		ctx->scounteren = csr_ਅਦਲਾ_ਬਦਲੀ(CSR_SCOUNTEREN, dom_ctx->scounteren);
	ਜੇ (sbi_hart_priv_ਸੰਸਕਰਣ(scratch) >= SBI_HART_PRIV_VER_1_12)
		ctx->senvcfg	= csr_ਅਦਲਾ_ਬਦਲੀ(CSR_SENVCFG, dom_ctx->senvcfg);
	ਜੇ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSQOSID))
		ctx->srmcfg	= csr_ਅਦਲਾ_ਬਦਲੀ(CSR_SRMCFG, dom_ctx->srmcfg);

	/* Eager context switch for float */
	ਜੇ (sbi_hart_has_extension(scratch, SBI_HART_EXT_F) ||
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_D)) {
		sbi_fp_save(&ctx->fp_ctx);
		sbi_fp_restore(&dom_ctx->fp_ctx);
	}

	/* Eager context switch for vector */
	ਜੇ (sbi_hart_has_extension(scratch, SBI_HART_EXT_V)) {
		sbi_ਸਦਿਸ਼_save(ctx->vec_ctx);
		sbi_ਸਦਿਸ਼_restore(dom_ctx->vec_ctx);
	}

	/* Save current trap state and restore target domain's trap state */
	trap_ctx = sbi_trap_get_context(scratch);
	sbi_memcpy(&ctx->trap_ctx, trap_ctx, ਆਕਾਰ(*trap_ctx));
	sbi_memcpy(trap_ctx, &dom_ctx->trap_ctx, ਆਕਾਰ(*trap_ctx));

	/*
	 * Re-configure PMP settings for the new domain
	 *
	 * This will internally perform full SFENCE / HFENCE which
	 * is also required for some of the above CSR updates (such
	 * as satp CSR).
	 */
	sbi_hart_protection_reconfigure(scratch, current_dom, ਨਿਸ਼ਾਨਾ_dom);

	/* Mark current context structure initialized because context saved */
	ctx->initialized = true;

	/* If target domain context is not initialized or runnable */
	ਜੇ (!dom_ctx->initialized) {
		/* Startup boot HART of target domain */
		ਜੇ (current_hartid() == ਨਿਸ਼ਾਨਾ_dom->boot_hartid)
			sbi_hart_switch_ਬਹੁਲਕ(ਨਿਸ਼ਾਨਾ_dom->boot_hartid,
					     ਨਿਸ਼ਾਨਾ_dom->ਅਗਲਾ_arg1,
					     ਨਿਸ਼ਾਨਾ_dom->ਅਗਲਾ_addr,
					     ਨਿਸ਼ਾਨਾ_dom->ਅਗਲਾ_ਬਹੁਲਕ,
					     false);
		ਨਹੀਂਤਾਂ
			sbi_hsm_hart_stop(scratch, true);
	}
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ hart_context_init(u32 hartindex)
{
	size_t vec_ਮਾਪ;
	ਬਣਤਰ hart_context *ctx;
	ਬਣਤਰ sbi_domain *dom;

	sbi_domain_for_each(dom) {
		ਜੇ (!sbi_hartmask_test_hartindex(hartindex,
						 dom->possible_harts))
			ਜਾਰੀ;

		ctx = sbi_zalloc(ਆਕਾਰ(ਬਣਤਰ hart_context));
		ਜੇ (!ctx)
			ਵਾਪਸ SBI_ENOMEM;

		ਜੇ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
					   SBI_HART_EXT_V)) {
			vec_ਮਾਪ = sbi_ਸਦਿਸ਼_context_ਮਾਪ();

			/* Allocate the vector context pointer */
			ctx->vec_ctx = sbi_zalloc(vec_ਮਾਪ);
			ਜੇ (!ctx->vec_ctx) {
				sbi_free(ctx);
				ਵਾਪਸ SBI_ENOMEM;
			}
		}

		/* Bind context and domain */
		ctx->dom = dom;
		hart_context_ਸੈੱਟ(dom, hartindex, ctx);
	}

	ਵਾਪਸ 0;
}

ਪੂਰਨ sbi_domain_context_enter(ਬਣਤਰ sbi_domain *dom)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ hart_context *dom_ctx;
	ਬਣਤਰ hart_context *ctx = hart_context_thishart_get();
	u32 hartindex = current_hartindex();

	/* Target domain must not be same as the current domain */
	ਜੇ (!dom || dom == sbi_domain_thishart_ptr())
		ਵਾਪਸ SBI_EINVAL;

	/* Target domain must have current hart as a possible harts */
	ਜੇ (!sbi_hartmask_test_hartindex(hartindex, dom->possible_harts))
		ਵਾਪਸ SBI_EINVAL;

	/*
	 * If it's first time to call `enter` on the current hart, no
	 * context allocated before. Allocate context for each valid
	 * domain on the current hart.
	 */
	ਜੇ (!ctx) {
		rc = hart_context_init(hartindex);
		ਜੇ (rc)
			ਵਾਪਸ rc;

		ctx = hart_context_thishart_get();
		ਜੇ (!ctx)
			ਵਾਪਸ SBI_EINVAL;
	}

	dom_ctx = hart_context_get(dom, hartindex);
	/* Validate the domain context existence */
	ਜੇ (!dom_ctx)
		ਵਾਪਸ SBI_EINVAL;

	/* Update target context's previous context to indicate the caller */
	dom_ctx->prev_ctx = ctx;

	ਵਾਪਸ switch_ਮੰਜ਼ਿਲ_ਅਗਲਾ_domain_context(ctx, dom_ctx);
}

ਪੂਰਨ sbi_domain_context_exit(ਖਾਲੀ)
{
	ਪੂਰਨ rc;
	u32 hartindex = current_hartindex();
	ਬਣਤਰ sbi_domain *dom;
	ਬਣਤਰ hart_context *ctx = hart_context_thishart_get();
	ਬਣਤਰ hart_context *dom_ctx, *tmp;

	/*
	 * If it's first time to call `exit` on the current hart, no
	 * context allocated before. Loop through each domain to allocate
	 * its context on the current hart if valid.
	 */
	ਜੇ (!ctx) {
		rc = hart_context_init(hartindex);
		ਜੇ (rc)
			ਵਾਪਸ rc;

		ctx = hart_context_thishart_get();
		ਜੇ (!ctx)
			ਵਾਪਸ SBI_EINVAL;
	}

	dom_ctx = ctx->prev_ctx;
	ctx->prev_ctx = ਨਿਰਾ;

	/* If no previous caller context */
	ਜੇ (!dom_ctx) {
		/* Try to find next uninitialized domain with least initialization order */
		dom_ctx = ਨਿਰਾ;
		sbi_domain_for_each(dom) {
			ਜੇ (dom == sbi_domain_thishart_ptr())
				ਜਾਰੀ;

			ਜੇ (!sbi_hartmask_test_hartindex(hartindex, dom->possible_harts))
				ਜਾਰੀ;

			tmp = hart_context_get(dom, hartindex);
			ਜੇ (tmp && tmp->initialized)
				ਜਾਰੀ;

			ਜੇ (!dom_ctx || tmp->dom->init_ਕ੍ਰਮ < dom_ctx->dom->init_ਕ੍ਰਮ)
				dom_ctx = tmp;
		}
	}
	ਜੇ (!dom_ctx)
		ਵਾਪਸ SBI_ENOENT;

	ਵਾਪਸ switch_ਮੰਜ਼ਿਲ_ਅਗਲਾ_domain_context(ctx, dom_ctx);
}

ਪੂਰਨ sbi_domain_context_init(ਖਾਲੀ)
{
	/**
	 * Allocate per-domain and per-hart context data.
	 * The data type is "struct hart_context **" whose memory space will be
	 * dynamically allocated by domain_setup_state_one(). Calculate needed
	 * size of memory space here.
	 */
	dcstate.state_ਮਾਪ = ਆਕਾਰ(ਬਣਤਰ hart_context *) * sbi_hart_ਗਿਣਤੀ();

	ਵਾਪਸ sbi_domain_ਰਜਿਸਟਰ_state(&dcstate);
}

ਖਾਲੀ sbi_domain_context_deinit(ਖਾਲੀ)
{
	sbi_domain_unregister_state(&dcstate);
}
