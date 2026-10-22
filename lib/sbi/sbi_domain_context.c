/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) IPADS@SJTU 2023. All rights reserved.
 */

#شامل <sbi/sbi_error.h>
#شامل <sbi/riscv_locks.h>
#شامل <sbi/riscv_asm.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_hsm.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_hart_protection.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_string.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_domain_context.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_trap.h>
#شامل <sbi/sbi_vector.h>
#شامل <sbi/sbi_fp.h>

/** Context representation for a hart within a domain */
ساخت hart_context {
	/** Trap-related states such as GPRs, mepc, and mstatus */
	ساخت sbi_trap_context trap_ctx;

	/** Supervisor status register */
	غیرعلامتی طویل sstatus;
	/** Supervisor interrupt enable register */
	غیرعلامتی طویل sie;
	/** Supervisor trap vector base address register */
	غیرعلامتی طویل stvec;
	/** Supervisor scratch register for temporary storage */
	غیرعلامتی طویل sscratch;
	/** Supervisor exception program counter register */
	غیرعلامتی طویل sepc;
	/** Supervisor cause register */
	غیرعلامتی طویل scause;
	/** Supervisor trap value register */
	غیرعلامتی طویل stval;
	/** Supervisor interrupt pending register */
	غیرعلامتی طویل sip;
	/** Supervisor address translation and protection register */
	غیرعلامتی طویل satp;
	/** Counter-enable register */
	غیرعلامتی طویل scounteren;
	/** Supervisor environment configuration register */
	غیرعلامتی طویل senvcfg;
	/** Supervisor resource management configuration register */
	غیرعلامتی طویل srmcfg;

	/** Float context state */
	ساخت sbi_fp_context fp_ctx;
	/** Vector context state */
	ساخت sbi_سمتیہ_context *vec_ctx;

	/** Reference to the owning domain */
	ساخت sbi_domain *dom;
	/** Previous context (caller) to jump to during context exits */
	ساخت hart_context *پچھلا_ctx;
	/** Is context initialized and runnable */
	bool initialized;
};

ساکن ساخت sbi_domain_state dcstate;

ساکن اندرونی ساخت hart_context *hart_context_get(ساخت sbi_domain *dom,
						    u32 hartindex)
{
	ساخت hart_context **dom_hartindex_مقصد_context_جدول;

	dom_hartindex_مقصد_context_جدول = sbi_domain_state_ptr(dom, &dcstate);
	اگر (!dom_hartindex_مقصد_context_جدول || !sbi_hartindex_valid(hartindex))
		واپس تہی;

	واپس dom_hartindex_مقصد_context_جدول[hartindex];
}

ساکن خالی hart_context_سیٹ(ساخت sbi_domain *dom, u32 hartindex,
			     ساخت hart_context *hc)
{
	ساخت hart_context **dom_hartindex_مقصد_context_جدول;

	dom_hartindex_مقصد_context_جدول = sbi_domain_state_ptr(dom, &dcstate);
	اگر (!dom_hartindex_مقصد_context_جدول || !sbi_hartindex_valid(hartindex))
		واپس;

	dom_hartindex_مقصد_context_جدول[hartindex] = hc;
}

/** Macro to obtain the current hart's context pointer */
#تعریف_کرو hart_context_thishart_get()					\
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
ساکن صحیح switch_مقصد_اگلا_domain_context(ساخت hart_context *ctx,
					  ساخت hart_context *dom_ctx)
	MUST_NOT_HOLD(&ctx->dom->assigned_harts_تالا)
	MUST_NOT_HOLD(&dom_ctx->dom->assigned_harts_تالا)
{
	u32 hartindex = current_hartindex();
	ساخت sbi_trap_context *trap_ctx;
	ساخت sbi_domain *current_dom, *ہدف_dom;
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	اگر (!ctx || !dom_ctx || ctx == dom_ctx)
		واپس SBI_EINVAL;

	current_dom = ctx->dom;
	ہدف_dom = dom_ctx->dom;
	/* Assign current hart to target domain */
	spin_تالا(&current_dom->assigned_harts_تالا);
	sbi_hartmask_clear_hartindex(hartindex, &current_dom->assigned_harts);
	spin_unlock(&current_dom->assigned_harts_تالا);

	sbi_update_hartindex_مقصد_domain(hartindex, ہدف_dom);

	spin_تالا(&ہدف_dom->assigned_harts_تالا);
	sbi_hartmask_سیٹ_hartindex(hartindex, &ہدف_dom->assigned_harts);
	spin_unlock(&ہدف_dom->assigned_harts_تالا);

	/* Save current CSR context and restore target domain's CSR context */
	ctx->sstatus	= csr_تبادلہ(CSR_SSTATUS, dom_ctx->sstatus);
	ctx->sie	= csr_تبادلہ(CSR_SIE, dom_ctx->sie);
	ctx->stvec	= csr_تبادلہ(CSR_STVEC, dom_ctx->stvec);
	ctx->sscratch	= csr_تبادلہ(CSR_SSCRATCH, dom_ctx->sscratch);
	ctx->sepc	= csr_تبادلہ(CSR_SEPC, dom_ctx->sepc);
	ctx->scause	= csr_تبادلہ(CSR_SCAUSE, dom_ctx->scause);
	ctx->stval	= csr_تبادلہ(CSR_STVAL, dom_ctx->stval);
	ctx->sip	= csr_تبادلہ(CSR_SIP, dom_ctx->sip);
	ctx->satp	= csr_تبادلہ(CSR_SATP, dom_ctx->satp);
	اگر (sbi_hart_priv_نسخہ(scratch) >= SBI_HART_PRIV_VER_1_10)
		ctx->scounteren = csr_تبادلہ(CSR_SCOUNTEREN, dom_ctx->scounteren);
	اگر (sbi_hart_priv_نسخہ(scratch) >= SBI_HART_PRIV_VER_1_12)
		ctx->senvcfg	= csr_تبادلہ(CSR_SENVCFG, dom_ctx->senvcfg);
	اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSQOSID))
		ctx->srmcfg	= csr_تبادلہ(CSR_SRMCFG, dom_ctx->srmcfg);

	/* Eager context switch for float */
	اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_F) ||
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_D)) {
		sbi_fp_save(&ctx->fp_ctx);
		sbi_fp_restore(&dom_ctx->fp_ctx);
	}

	/* Eager context switch for vector */
	اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_V)) {
		sbi_سمتیہ_save(ctx->vec_ctx);
		sbi_سمتیہ_restore(dom_ctx->vec_ctx);
	}

	/* Save current trap state and restore target domain's trap state */
	trap_ctx = sbi_trap_get_context(scratch);
	sbi_memcpy(&ctx->trap_ctx, trap_ctx, حجم(*trap_ctx));
	sbi_memcpy(trap_ctx, &dom_ctx->trap_ctx, حجم(*trap_ctx));

	/*
	 * Re-configure PMP settings for the new domain
	 *
	 * This will internally perform full SFENCE / HFENCE which
	 * is also required for some of the above CSR updates (such
	 * as satp CSR).
	 */
	sbi_hart_protection_reconfigure(scratch, current_dom, ہدف_dom);

	/* Mark current context structure initialized because context saved */
	ctx->initialized = true;

	/* If target domain context is not initialized or runnable */
	اگر (!dom_ctx->initialized) {
		/* Startup boot HART of target domain */
		اگر (current_hartid() == ہدف_dom->boot_hartid)
			sbi_hart_switch_منوال(ہدف_dom->boot_hartid,
					     ہدف_dom->اگلا_arg1,
					     ہدف_dom->اگلا_addr,
					     ہدف_dom->اگلا_منوال,
					     false);
		ورنہ
			sbi_hsm_hart_stop(scratch, true);
	}
	واپس 0;
}

ساکن صحیح hart_context_init(u32 hartindex)
{
	size_t vec_سائز;
	ساخت hart_context *ctx;
	ساخت sbi_domain *dom;

	sbi_domain_for_each(dom) {
		اگر (!sbi_hartmask_test_hartindex(hartindex,
						 dom->possible_harts))
			جاری;

		ctx = sbi_zalloc(حجم(ساخت hart_context));
		اگر (!ctx)
			واپس SBI_ENOMEM;

		اگر (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
					   SBI_HART_EXT_V)) {
			vec_سائز = sbi_سمتیہ_context_سائز();

			/* Allocate the vector context pointer */
			ctx->vec_ctx = sbi_zalloc(vec_سائز);
			اگر (!ctx->vec_ctx) {
				sbi_free(ctx);
				واپس SBI_ENOMEM;
			}
		}

		/* Bind context and domain */
		ctx->dom = dom;
		hart_context_سیٹ(dom, hartindex, ctx);
	}

	واپس 0;
}

صحیح sbi_domain_context_enter(ساخت sbi_domain *dom)
{
	صحیح rc;
	ساخت hart_context *dom_ctx;
	ساخت hart_context *ctx = hart_context_thishart_get();
	u32 hartindex = current_hartindex();

	/* Target domain must not be same as the current domain */
	اگر (!dom || dom == sbi_domain_thishart_ptr())
		واپس SBI_EINVAL;

	/* Target domain must have current hart as a possible harts */
	اگر (!sbi_hartmask_test_hartindex(hartindex, dom->possible_harts))
		واپس SBI_EINVAL;

	/*
	 * If it's first time to call `enter` on the current hart, no
	 * context allocated before. Allocate context for each valid
	 * domain on the current hart.
	 */
	اگر (!ctx) {
		rc = hart_context_init(hartindex);
		اگر (rc)
			واپس rc;

		ctx = hart_context_thishart_get();
		اگر (!ctx)
			واپس SBI_EINVAL;
	}

	dom_ctx = hart_context_get(dom, hartindex);
	/* Validate the domain context existence */
	اگر (!dom_ctx)
		واپس SBI_EINVAL;

	/* Update target context's previous context to indicate the caller */
	dom_ctx->پچھلا_ctx = ctx;

	واپس switch_مقصد_اگلا_domain_context(ctx, dom_ctx);
}

صحیح sbi_domain_context_exit(خالی)
{
	صحیح rc;
	u32 hartindex = current_hartindex();
	ساخت sbi_domain *dom;
	ساخت hart_context *ctx = hart_context_thishart_get();
	ساخت hart_context *dom_ctx, *tmp;

	/*
	 * If it's first time to call `exit` on the current hart, no
	 * context allocated before. Loop through each domain to allocate
	 * its context on the current hart if valid.
	 */
	اگر (!ctx) {
		rc = hart_context_init(hartindex);
		اگر (rc)
			واپس rc;

		ctx = hart_context_thishart_get();
		اگر (!ctx)
			واپس SBI_EINVAL;
	}

	dom_ctx = ctx->پچھلا_ctx;
	ctx->پچھلا_ctx = تہی;

	/* If no previous caller context */
	اگر (!dom_ctx) {
		/* Try to find next uninitialized domain with least initialization order */
		dom_ctx = تہی;
		sbi_domain_for_each(dom) {
			اگر (dom == sbi_domain_thishart_ptr())
				جاری;

			اگر (!sbi_hartmask_test_hartindex(hartindex, dom->possible_harts))
				جاری;

			tmp = hart_context_get(dom, hartindex);
			اگر (tmp && tmp->initialized)
				جاری;

			اگر (!dom_ctx || tmp->dom->init_ترتیب < dom_ctx->dom->init_ترتیب)
				dom_ctx = tmp;
		}
	}
	اگر (!dom_ctx)
		واپس SBI_ENOENT;

	واپس switch_مقصد_اگلا_domain_context(ctx, dom_ctx);
}

صحیح sbi_domain_context_init(خالی)
{
	/**
	 * Allocate per-domain and per-hart context data.
	 * The data type is "struct hart_context **" whose memory space will be
	 * dynamically allocated by domain_setup_state_one(). Calculate needed
	 * size of memory space here.
	 */
	dcstate.state_سائز = حجم(ساخت hart_context *) * sbi_hart_گنتی();

	واپس sbi_domain_رجسٹر_state(&dcstate);
}

خالی sbi_domain_context_deinit(خالی)
{
	sbi_domain_unregister_state(&dcstate);
}
