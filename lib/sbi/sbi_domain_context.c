/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) IPADS@SJTU 2023. All rights reserved.
 */

#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/riscv_locks.h>
#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_hsm.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_hart_protection.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi/sbi_string.h>
#અન્તર્ભાવ <sbi/sbi_domain.h>
#અન્તર્ભાવ <sbi/sbi_domain_context.h>
#અન્તર્ભાવ <sbi/sbi_platform.h>
#અન્તર્ભાવ <sbi/sbi_trap.h>
#અન્તર્ભાવ <sbi/sbi_vector.h>
#અન્તર્ભાવ <sbi/sbi_fp.h>

/** Context representation for a hart within a domain */
સંરચના hart_context {
	/** Trap-related states such as GPRs, mepc, and mstatus */
	સંરચના sbi_trap_context trap_ctx;

	/** Supervisor status register */
	unsigned long sstatus;
	/** Supervisor interrupt enable register */
	unsigned long sie;
	/** Supervisor trap vector base address register */
	unsigned long stvec;
	/** Supervisor scratch register for temporary storage */
	unsigned long sscratch;
	/** Supervisor exception program counter register */
	unsigned long sepc;
	/** Supervisor cause register */
	unsigned long scause;
	/** Supervisor trap value register */
	unsigned long stval;
	/** Supervisor interrupt pending register */
	unsigned long sip;
	/** Supervisor address translation and protection register */
	unsigned long satp;
	/** Counter-enable register */
	unsigned long scounteren;
	/** Supervisor environment configuration register */
	unsigned long senvcfg;
	/** Supervisor resource management configuration register */
	unsigned long srmcfg;

	/** Float context state */
	સંરચના sbi_fp_context fp_ctx;
	/** Vector context state */
	સંરચના sbi_vector_context *vec_ctx;

	/** Reference to the owning domain */
	સંરચના sbi_domain *dom;
	/** Previous context (caller) to jump to during context exits */
	સંરચના hart_context *prev_ctx;
	/** Is context initialized and runnable */
	bool initialized;
};

સ્થિર સંરચના sbi_domain_state dcstate;

સ્થિર અંતઃરેખ સંરચના hart_context *hart_context_get(સંરચના sbi_domain *dom,
						    u32 hartindex)
{
	સંરચના hart_context **dom_hartindex_to_context_table;

	dom_hartindex_to_context_table = sbi_domain_state_ptr(dom, &dcstate);
	જો (!dom_hartindex_to_context_table || !sbi_hartindex_valid(hartindex))
		પાછા ખાલી;

	પાછા dom_hartindex_to_context_table[hartindex];
}

સ્થિર શૂન્ય hart_context_set(સંરચના sbi_domain *dom, u32 hartindex,
			     સંરચના hart_context *hc)
{
	સંરચના hart_context **dom_hartindex_to_context_table;

	dom_hartindex_to_context_table = sbi_domain_state_ptr(dom, &dcstate);
	જો (!dom_hartindex_to_context_table || !sbi_hartindex_valid(hartindex))
		પાછા;

	dom_hartindex_to_context_table[hartindex] = hc;
}

/** Macro to obtain the current hart's context pointer */
#define hart_context_thishart_get()					\
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
સ્થિર પૂર્ણાંક switch_to_next_domain_context(સંરચના hart_context *ctx,
					  સંરચના hart_context *dom_ctx)
	MUST_NOT_HOLD(&ctx->dom->assigned_harts_lock)
	MUST_NOT_HOLD(&dom_ctx->dom->assigned_harts_lock)
{
	u32 hartindex = current_hartindex();
	સંરચના sbi_trap_context *trap_ctx;
	સંરચના sbi_domain *current_dom, *target_dom;
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	જો (!ctx || !dom_ctx || ctx == dom_ctx)
		પાછા SBI_EINVAL;

	current_dom = ctx->dom;
	target_dom = dom_ctx->dom;
	/* Assign current hart to target domain */
	spin_lock(&current_dom->assigned_harts_lock);
	sbi_hartmask_clear_hartindex(hartindex, &current_dom->assigned_harts);
	spin_unlock(&current_dom->assigned_harts_lock);

	sbi_update_hartindex_to_domain(hartindex, target_dom);

	spin_lock(&target_dom->assigned_harts_lock);
	sbi_hartmask_set_hartindex(hartindex, &target_dom->assigned_harts);
	spin_unlock(&target_dom->assigned_harts_lock);

	/* Save current CSR context and restore target domain's CSR context */
	ctx->sstatus	= csr_swap(CSR_SSTATUS, dom_ctx->sstatus);
	ctx->sie	= csr_swap(CSR_SIE, dom_ctx->sie);
	ctx->stvec	= csr_swap(CSR_STVEC, dom_ctx->stvec);
	ctx->sscratch	= csr_swap(CSR_SSCRATCH, dom_ctx->sscratch);
	ctx->sepc	= csr_swap(CSR_SEPC, dom_ctx->sepc);
	ctx->scause	= csr_swap(CSR_SCAUSE, dom_ctx->scause);
	ctx->stval	= csr_swap(CSR_STVAL, dom_ctx->stval);
	ctx->sip	= csr_swap(CSR_SIP, dom_ctx->sip);
	ctx->satp	= csr_swap(CSR_SATP, dom_ctx->satp);
	જો (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
		ctx->scounteren = csr_swap(CSR_SCOUNTEREN, dom_ctx->scounteren);
	જો (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12)
		ctx->senvcfg	= csr_swap(CSR_SENVCFG, dom_ctx->senvcfg);
	જો (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSQOSID))
		ctx->srmcfg	= csr_swap(CSR_SRMCFG, dom_ctx->srmcfg);

	/* Eager context switch for float */
	જો (sbi_hart_has_extension(scratch, SBI_HART_EXT_F) ||
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_D)) {
		sbi_fp_save(&ctx->fp_ctx);
		sbi_fp_restore(&dom_ctx->fp_ctx);
	}

	/* Eager context switch for vector */
	જો (sbi_hart_has_extension(scratch, SBI_HART_EXT_V)) {
		sbi_vector_save(ctx->vec_ctx);
		sbi_vector_restore(dom_ctx->vec_ctx);
	}

	/* Save current trap state and restore target domain's trap state */
	trap_ctx = sbi_trap_get_context(scratch);
	sbi_memcpy(&ctx->trap_ctx, trap_ctx, કદ(*trap_ctx));
	sbi_memcpy(trap_ctx, &dom_ctx->trap_ctx, કદ(*trap_ctx));

	/*
	 * Re-configure PMP settings for the new domain
	 *
	 * This will internally perform full SFENCE / HFENCE which
	 * is also required for some of the above CSR updates (such
	 * as satp CSR).
	 */
	sbi_hart_protection_reconfigure(scratch, current_dom, target_dom);

	/* Mark current context structure initialized because context saved */
	ctx->initialized = true;

	/* If target domain context is not initialized or runnable */
	જો (!dom_ctx->initialized) {
		/* Startup boot HART of target domain */
		જો (current_hartid() == target_dom->boot_hartid)
			sbi_hart_switch_mode(target_dom->boot_hartid,
					     target_dom->next_arg1,
					     target_dom->next_addr,
					     target_dom->next_mode,
					     false);
		નહીંતર
			sbi_hsm_hart_stop(scratch, true);
	}
	પાછા 0;
}

સ્થિર પૂર્ણાંક hart_context_init(u32 hartindex)
{
	size_t vec_size;
	સંરચના hart_context *ctx;
	સંરચના sbi_domain *dom;

	sbi_domain_for_each(dom) {
		જો (!sbi_hartmask_test_hartindex(hartindex,
						 dom->possible_harts))
			ચાલુ;

		ctx = sbi_zalloc(કદ(સંરચના hart_context));
		જો (!ctx)
			પાછા SBI_ENOMEM;

		જો (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
					   SBI_HART_EXT_V)) {
			vec_size = sbi_vector_context_size();

			/* Allocate the vector context pointer */
			ctx->vec_ctx = sbi_zalloc(vec_size);
			જો (!ctx->vec_ctx) {
				sbi_free(ctx);
				પાછા SBI_ENOMEM;
			}
		}

		/* Bind context and domain */
		ctx->dom = dom;
		hart_context_set(dom, hartindex, ctx);
	}

	પાછા 0;
}

પૂર્ણાંક sbi_domain_context_enter(સંરચના sbi_domain *dom)
{
	પૂર્ણાંક rc;
	સંરચના hart_context *dom_ctx;
	સંરચના hart_context *ctx = hart_context_thishart_get();
	u32 hartindex = current_hartindex();

	/* Target domain must not be same as the current domain */
	જો (!dom || dom == sbi_domain_thishart_ptr())
		પાછા SBI_EINVAL;

	/* Target domain must have current hart as a possible harts */
	જો (!sbi_hartmask_test_hartindex(hartindex, dom->possible_harts))
		પાછા SBI_EINVAL;

	/*
	 * If it's first time to call `enter` on the current hart, no
	 * context allocated before. Allocate context for each valid
	 * domain on the current hart.
	 */
	જો (!ctx) {
		rc = hart_context_init(hartindex);
		જો (rc)
			પાછા rc;

		ctx = hart_context_thishart_get();
		જો (!ctx)
			પાછા SBI_EINVAL;
	}

	dom_ctx = hart_context_get(dom, hartindex);
	/* Validate the domain context existence */
	જો (!dom_ctx)
		પાછા SBI_EINVAL;

	/* Update target context's previous context to indicate the caller */
	dom_ctx->prev_ctx = ctx;

	પાછા switch_to_next_domain_context(ctx, dom_ctx);
}

પૂર્ણાંક sbi_domain_context_exit(શૂન્ય)
{
	પૂર્ણાંક rc;
	u32 hartindex = current_hartindex();
	સંરચના sbi_domain *dom;
	સંરચના hart_context *ctx = hart_context_thishart_get();
	સંરચના hart_context *dom_ctx, *tmp;

	/*
	 * If it's first time to call `exit` on the current hart, no
	 * context allocated before. Loop through each domain to allocate
	 * its context on the current hart if valid.
	 */
	જો (!ctx) {
		rc = hart_context_init(hartindex);
		જો (rc)
			પાછા rc;

		ctx = hart_context_thishart_get();
		જો (!ctx)
			પાછા SBI_EINVAL;
	}

	dom_ctx = ctx->prev_ctx;
	ctx->prev_ctx = ખાલી;

	/* If no previous caller context */
	જો (!dom_ctx) {
		/* Try to find next uninitialized domain with least initialization order */
		dom_ctx = ખાલી;
		sbi_domain_for_each(dom) {
			જો (dom == sbi_domain_thishart_ptr())
				ચાલુ;

			જો (!sbi_hartmask_test_hartindex(hartindex, dom->possible_harts))
				ચાલુ;

			tmp = hart_context_get(dom, hartindex);
			જો (tmp && tmp->initialized)
				ચાલુ;

			જો (!dom_ctx || tmp->dom->init_order < dom_ctx->dom->init_order)
				dom_ctx = tmp;
		}
	}
	જો (!dom_ctx)
		પાછા SBI_ENOENT;

	પાછા switch_to_next_domain_context(ctx, dom_ctx);
}

પૂર્ણાંક sbi_domain_context_init(શૂન્ય)
{
	/**
	 * Allocate per-domain and per-hart context data.
	 * The data type is "struct hart_context **" whose memory space will be
	 * dynamically allocated by domain_setup_state_one(). Calculate needed
	 * size of memory space here.
	 */
	dcstate.state_size = કદ(સંરચના hart_context *) * sbi_hart_ગણતરી();

	પાછા sbi_domain_register_state(&dcstate);
}

શૂન્ય sbi_domain_context_deinit(શૂન્ય)
{
	sbi_domain_unregister_state(&dcstate);
}
