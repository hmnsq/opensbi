/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) IPADS@SJTU 2023. All rights reserved.
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_locks.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hsm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart_protection.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_string.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain_context.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_platform.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_trap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_vector.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_fp.h>

/** Context representation for a hart within a domain */
ସଂରଚନା hart_context {
	/** Trap-related states such as GPRs, mepc, and mstatus */
	ସଂରଚନା sbi_trap_context trap_ctx;

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
	ସଂରଚନା sbi_fp_context fp_ctx;
	/** Vector context state */
	ସଂରଚନା sbi_vector_context *vec_ctx;

	/** Reference to the owning domain */
	ସଂରଚନା sbi_domain *dom;
	/** Previous context (caller) to jump to during context exits */
	ସଂରଚନା hart_context *prev_ctx;
	/** Is context initialized and runnable */
	bool initialized;
};

ସ୍ଥିର ସଂରଚନା sbi_domain_state dcstate;

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ସଂରଚନା hart_context *hart_context_get(ସଂରଚନା sbi_domain *dom,
						    u32 hartindex)
{
	ସଂରଚନା hart_context **dom_hartindex_to_context_table;

	dom_hartindex_to_context_table = sbi_domain_state_ptr(dom, &dcstate);
	ଯଦି (!dom_hartindex_to_context_table || !sbi_hartindex_valid(hartindex))
		ଫେରନ୍ତୁ ଖାଲି;

	ଫେରନ୍ତୁ dom_hartindex_to_context_table[hartindex];
}

ସ୍ଥିର ଶୂନ୍ୟ hart_context_set(ସଂରଚନା sbi_domain *dom, u32 hartindex,
			     ସଂରଚନା hart_context *hc)
{
	ସଂରଚନା hart_context **dom_hartindex_to_context_table;

	dom_hartindex_to_context_table = sbi_domain_state_ptr(dom, &dcstate);
	ଯଦି (!dom_hartindex_to_context_table || !sbi_hartindex_valid(hartindex))
		ଫେରନ୍ତୁ;

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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା switch_to_next_domain_context(ସଂରଚନା hart_context *ctx,
					  ସଂରଚନା hart_context *dom_ctx)
	MUST_NOT_HOLD(&ctx->dom->assigned_harts_lock)
	MUST_NOT_HOLD(&dom_ctx->dom->assigned_harts_lock)
{
	u32 hartindex = current_hartindex();
	ସଂରଚନା sbi_trap_context *trap_ctx;
	ସଂରଚନା sbi_domain *current_dom, *target_dom;
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ଯଦି (!ctx || !dom_ctx || ctx == dom_ctx)
		ଫେରନ୍ତୁ SBI_EINVAL;

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
	ଯଦି (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
		ctx->scounteren = csr_swap(CSR_SCOUNTEREN, dom_ctx->scounteren);
	ଯଦି (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12)
		ctx->senvcfg	= csr_swap(CSR_SENVCFG, dom_ctx->senvcfg);
	ଯଦି (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSQOSID))
		ctx->srmcfg	= csr_swap(CSR_SRMCFG, dom_ctx->srmcfg);

	/* Eager context switch for float */
	ଯଦି (sbi_hart_has_extension(scratch, SBI_HART_EXT_F) ||
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_D)) {
		sbi_fp_save(&ctx->fp_ctx);
		sbi_fp_restore(&dom_ctx->fp_ctx);
	}

	/* Eager context switch for vector */
	ଯଦି (sbi_hart_has_extension(scratch, SBI_HART_EXT_V)) {
		sbi_vector_save(ctx->vec_ctx);
		sbi_vector_restore(dom_ctx->vec_ctx);
	}

	/* Save current trap state and restore target domain's trap state */
	trap_ctx = sbi_trap_get_context(scratch);
	sbi_memcpy(&ctx->trap_ctx, trap_ctx, ଆକାର(*trap_ctx));
	sbi_memcpy(trap_ctx, &dom_ctx->trap_ctx, ଆକାର(*trap_ctx));

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
	ଯଦି (!dom_ctx->initialized) {
		/* Startup boot HART of target domain */
		ଯଦି (current_hartid() == target_dom->boot_hartid)
			sbi_hart_switch_mode(target_dom->boot_hartid,
					     target_dom->next_arg1,
					     target_dom->next_addr,
					     target_dom->next_mode,
					     false);
		ଅନ୍ୟଥା
			sbi_hsm_hart_stop(scratch, true);
	}
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା hart_context_init(u32 hartindex)
{
	size_t vec_size;
	ସଂରଚନା hart_context *ctx;
	ସଂରଚନା sbi_domain *dom;

	sbi_domain_for_each(dom) {
		ଯଦି (!sbi_hartmask_test_hartindex(hartindex,
						 dom->possible_harts))
			ଜାରି;

		ctx = sbi_zalloc(ଆକାର(ସଂରଚନା hart_context));
		ଯଦି (!ctx)
			ଫେରନ୍ତୁ SBI_ENOMEM;

		ଯଦି (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
					   SBI_HART_EXT_V)) {
			vec_size = sbi_vector_context_size();

			/* Allocate the vector context pointer */
			ctx->vec_ctx = sbi_zalloc(vec_size);
			ଯଦି (!ctx->vec_ctx) {
				sbi_free(ctx);
				ଫେରନ୍ତୁ SBI_ENOMEM;
			}
		}

		/* Bind context and domain */
		ctx->dom = dom;
		hart_context_set(dom, hartindex, ctx);
	}

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_domain_context_enter(ସଂରଚନା sbi_domain *dom)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା hart_context *dom_ctx;
	ସଂରଚନା hart_context *ctx = hart_context_thishart_get();
	u32 hartindex = current_hartindex();

	/* Target domain must not be same as the current domain */
	ଯଦି (!dom || dom == sbi_domain_thishart_ptr())
		ଫେରନ୍ତୁ SBI_EINVAL;

	/* Target domain must have current hart as a possible harts */
	ଯଦି (!sbi_hartmask_test_hartindex(hartindex, dom->possible_harts))
		ଫେରନ୍ତୁ SBI_EINVAL;

	/*
	 * If it's first time to call `enter` on the current hart, no
	 * context allocated before. Allocate context for each valid
	 * domain on the current hart.
	 */
	ଯଦି (!ctx) {
		rc = hart_context_init(hartindex);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;

		ctx = hart_context_thishart_get();
		ଯଦି (!ctx)
			ଫେରନ୍ତୁ SBI_EINVAL;
	}

	dom_ctx = hart_context_get(dom, hartindex);
	/* Validate the domain context existence */
	ଯଦି (!dom_ctx)
		ଫେରନ୍ତୁ SBI_EINVAL;

	/* Update target context's previous context to indicate the caller */
	dom_ctx->prev_ctx = ctx;

	ଫେରନ୍ତୁ switch_to_next_domain_context(ctx, dom_ctx);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_domain_context_exit(ଶୂନ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	u32 hartindex = current_hartindex();
	ସଂରଚନା sbi_domain *dom;
	ସଂରଚନା hart_context *ctx = hart_context_thishart_get();
	ସଂରଚନା hart_context *dom_ctx, *tmp;

	/*
	 * If it's first time to call `exit` on the current hart, no
	 * context allocated before. Loop through each domain to allocate
	 * its context on the current hart if valid.
	 */
	ଯଦି (!ctx) {
		rc = hart_context_init(hartindex);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;

		ctx = hart_context_thishart_get();
		ଯଦି (!ctx)
			ଫେରନ୍ତୁ SBI_EINVAL;
	}

	dom_ctx = ctx->prev_ctx;
	ctx->prev_ctx = ଖାଲି;

	/* If no previous caller context */
	ଯଦି (!dom_ctx) {
		/* Try to find next uninitialized domain with least initialization order */
		dom_ctx = ଖାଲି;
		sbi_domain_for_each(dom) {
			ଯଦି (dom == sbi_domain_thishart_ptr())
				ଜାରି;

			ଯଦି (!sbi_hartmask_test_hartindex(hartindex, dom->possible_harts))
				ଜାରି;

			tmp = hart_context_get(dom, hartindex);
			ଯଦି (tmp && tmp->initialized)
				ଜାରି;

			ଯଦି (!dom_ctx || tmp->dom->init_order < dom_ctx->dom->init_order)
				dom_ctx = tmp;
		}
	}
	ଯଦି (!dom_ctx)
		ଫେରନ୍ତୁ SBI_ENOENT;

	ଫେରନ୍ତୁ switch_to_next_domain_context(ctx, dom_ctx);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_domain_context_init(ଶୂନ୍ୟ)
{
	/**
	 * Allocate per-domain and per-hart context data.
	 * The data type is "struct hart_context **" whose memory space will be
	 * dynamically allocated by domain_setup_state_one(). Calculate needed
	 * size of memory space here.
	 */
	dcstate.state_size = ଆକାର(ସଂରଚନା hart_context *) * sbi_hart_ଗଣନା();

	ଫେରନ୍ତୁ sbi_domain_register_state(&dcstate);
}

ଶୂନ୍ୟ sbi_domain_context_deinit(ଶୂନ୍ୟ)
{
	sbi_domain_unregister_state(&dcstate);
}
