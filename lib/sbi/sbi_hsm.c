/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/riscv_barrier.h>
#અન્તર્ભાવ <sbi/riscv_encoding.h>
#અન્તર્ભાવ <sbi/riscv_atomic.h>
#અન્તર્ભાવ <sbi/sbi_bitops.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_domain.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_ecall_interface.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_hartmask.h>
#અન્તર્ભાવ <sbi/sbi_hsm.h>
#અન્તર્ભાવ <sbi/sbi_init.h>
#અન્તર્ભાવ <sbi/sbi_ipi.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi/sbi_system.h>
#અન્તર્ભાવ <sbi/sbi_timer.h>
#અન્તર્ભાવ <sbi/sbi_console.h>

#define __sbi_hsm_hart_change_state(hdata, oldstate, newstate)		\
({									\
	long state = atomic_cmpxchg(&(hdata)->state, oldstate, newstate); \
	જો (state != (oldstate))					\
		sbi_printf("%s: ERR: The hart is in invalid state [%lu]\n", \
			   __func__, state);				\
	state == (oldstate);						\
})

સ્થિર અચલ સંરચના sbi_hsm_device *hsm_dev = ખાલી;
સ્થિર unsigned long hart_data_offset;
સ્થિર bool hsm_device_has_hart_hotplug(શૂન્ય);
સ્થિર પૂર્ણાંક hsm_device_hart_stop(શૂન્ય);

/** Per hart specific data to manage state transition **/
સંરચના sbi_hsm_data {
	atomic_t state;
	unsigned long suspend_type;
	unsigned long saved_mie;
	unsigned long saved_mip;
	unsigned long saved_medeleg;
	unsigned long saved_mideleg;
	u64 saved_menvcfg;
	atomic_t start_ticket;
};

bool sbi_hsm_hart_change_state(સંરચના sbi_scratch *scratch, long oldstate,
			       long newstate)
{
	સંરચના sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	પાછા __sbi_hsm_hart_change_state(hdata, oldstate, newstate);
}

પૂર્ણાંક __sbi_hsm_hart_get_state(u32 hartindex)
{
	સંરચના sbi_hsm_data *hdata;
	સંરચના sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	જો (!scratch)
		પાછા SBI_EINVAL;

	hdata = sbi_scratch_offset_ptr(scratch, hart_data_offset);
	પાછા atomic_read(&hdata->state);
}

પૂર્ણાંક sbi_hsm_hart_get_state(અચલ સંરચના sbi_domain *dom, u32 hartid)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);

	જો (!sbi_domain_is_assigned_hart(dom, hartindex))
		પાછા SBI_EINVAL;

	પાછા __sbi_hsm_hart_get_state(hartindex);
}

/*
 * Try to acquire the ticket for the given target hart to make sure only
 * one hart prepares the start of the target hart.
 * Returns true if the ticket has been acquired, false otherwise.
 *
 * The function has "acquire" semantics: no memory operations following it
 * in the current hart can be seen before it by other harts.
 * atomic_cmpxchg() provides the memory barriers needed for that.
 */
સ્થિર bool hsm_start_ticket_acquire(સંરચના sbi_hsm_data *hdata)
{
	પાછા (atomic_cmpxchg(&hdata->start_ticket, 0, 1) == 0);
}

/*
 * Release the ticket for the given target hart.
 *
 * The function has "release" semantics: no memory operations preceding it
 * in the current hart can be seen after it by other harts.
 */
સ્થિર શૂન્ય hsm_start_ticket_release(સંરચના sbi_hsm_data *hdata)
{
	RISCV_FENCE(rw, w);
	atomic_write(&hdata->start_ticket, 0);
}

/**
 * Get the mask of harts which are valid IPI targets
 * @param dom the domain to be used for output HART mask
 * @param mask the output hartmask to fill
 * @return 0 on success and SBI_Exxx (< 0) on failure
 */
પૂર્ણાંક sbi_hsm_hart_interruptible_mask(અચલ સંરચના sbi_domain *dom,
				    સંરચના sbi_hartmask *mask)
{
	પૂર્ણાંક hstate, ret;
	u32 i;

	ret = sbi_domain_get_assigned_hartmask(dom, mask);
	જો (ret)
		પાછા ret;

	sbi_hartmask_for_each_hartindex(i, mask) {
		hstate = __sbi_hsm_hart_get_state(i);
		જો (hstate != SBI_HSM_STATE_STARTED &&
		    hstate != SBI_HSM_STATE_SUSPENDED &&
		    hstate != SBI_HSM_STATE_RESUME_PENDING)
			sbi_hartmask_clear_hartindex(i, mask);
	}

	પાછા 0;
}

શૂન્ય __noreturn sbi_hsm_hart_start_finish(સંરચના sbi_scratch *scratch,
					  u32 hartid)
{
	unsigned long next_arg1;
	unsigned long next_addr;
	unsigned long next_mode;
	સંરચના sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	જો (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_START_PENDING,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	next_arg1 = scratch->next_arg1;
	next_addr = scratch->next_addr;
	next_mode = scratch->next_mode;
	hsm_start_ticket_release(hdata);

	sbi_hart_switch_mode(hartid, next_arg1, next_addr, next_mode, false);
}

સ્થિર શૂન્ય sbi_hsm_hart_wait(સંરચના sbi_scratch *scratch)
{
	unsigned long saved_mie;
	સંરચના sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	/* Save MIE CSR */
	saved_mie = csr_read(CSR_MIE);

	/* Set MSIE and MEIE bits to receive IPI */
	csr_set(CSR_MIE, MIP_MSIP | MIP_MEIP);

	/* Wait for state transition requested by sbi_hsm_hart_start() */
	જ્યારે (atomic_read(&hdata->state) != SBI_HSM_STATE_START_PENDING) {
		/*
		 * If the hsm_dev is ready and it support the hotplug, we can
		 * use the hsm stop for more power saving
		 */
		જો (hsm_device_has_hart_hotplug()) {
			sbi_revert_entry_ગણતરી(scratch);
			hsm_device_hart_stop();
		}

		wfi();
	}

	/* Restore MIE CSR */
	csr_write(CSR_MIE, saved_mie);

	/*
	 * No need to clear IPI here because the sbi_ipi_init() will
	 * clear it for current HART.
	 */
}

અચલ સંરચના sbi_hsm_device *sbi_hsm_get_device(શૂન્ય)
{
	પાછા hsm_dev;
}

શૂન્ય sbi_hsm_set_device(અચલ સંરચના sbi_hsm_device *dev)
{
	જો (!dev || hsm_dev)
		પાછા;

	hsm_dev = dev;
}

સ્થિર bool hsm_device_has_hart_hotplug(શૂન્ય)
{
	જો (hsm_dev && hsm_dev->hart_start && hsm_dev->hart_stop)
		પાછા true;
	પાછા false;
}

સ્થિર bool hsm_device_has_hart_secondary_boot(શૂન્ય)
{
	જો (hsm_dev && hsm_dev->hart_start && !hsm_dev->hart_stop)
		પાછા true;
	પાછા false;
}

સ્થિર પૂર્ણાંક hsm_device_hart_start(u32 hartid, ulong saddr)
{
	જો (hsm_dev && hsm_dev->hart_start)
		પાછા hsm_dev->hart_start(hartid, saddr);
	પાછા SBI_ENOTSUPP;
}

સ્થિર પૂર્ણાંક hsm_device_hart_stop(શૂન્ય)
{
	જો (hsm_dev && hsm_dev->hart_stop)
		પાછા hsm_dev->hart_stop();
	પાછા SBI_ENOTSUPP;
}

સ્થિર પૂર્ણાંક hsm_device_hart_suspend(u32 suspend_type, ulong mmode_resume_addr)
{
	જો (hsm_dev && hsm_dev->hart_suspend)
		પાછા hsm_dev->hart_suspend(suspend_type, mmode_resume_addr);
	પાછા SBI_ENOTSUPP;
}

સ્થિર શૂન્ય hsm_device_hart_resume(શૂન્ય)
{
	જો (hsm_dev && hsm_dev->hart_resume)
		hsm_dev->hart_resume();
}

પૂર્ણાંક sbi_hsm_init(સંરચના sbi_scratch *scratch, bool cold_boot)
{
	સંરચના sbi_scratch *rscratch;
	સંરચના sbi_hsm_data *hdata;

	જો (cold_boot) {
		hart_data_offset = sbi_scratch_alloc_offset(કદ(*hdata));
		જો (!hart_data_offset)
			પાછા SBI_ENOMEM;

		/* Initialize hart state data for every hart */
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_to_scratch(i);
			જો (!rscratch)
				ચાલુ;

			hdata = sbi_scratch_offset_ptr(rscratch,
						       hart_data_offset);
			ATOMIC_INIT(&hdata->state,
				    (i == current_hartindex()) ?
				    SBI_HSM_STATE_START_PENDING :
				    SBI_HSM_STATE_STOPPED);
			ATOMIC_INIT(&hdata->start_ticket, 0);
		}
	} નહીંતર {
		sbi_hsm_hart_wait(scratch);
	}

	પાછા 0;
}

શૂન્ય __noreturn sbi_hsm_exit(સંરચના sbi_scratch *scratch)
{
	સંરચના sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	શૂન્ય (*jump_warmboot)(શૂન્ય) = (શૂન્ય (*)(શૂન્ય))scratch->warmboot_addr;

	જો (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STOP_PENDING,
					 SBI_HSM_STATE_STOPPED))
		જાઓ fail_exit;

	જો (hsm_device_has_hart_hotplug()) {
		જો (hsm_device_hart_stop() != SBI_ENOTSUPP)
			જાઓ fail_exit;
	}

	/**
	 * As platform is lacking support for hotplug, directly jump to warmboot
	 * and wait for interrupts in warmboot. We do it preemptively in order
	 * preserve the hart states and reuse the code path for hotplug.
	 */
	jump_warmboot();

fail_exit:
	/* It should never reach here */
	sbi_printf("ERR: Failed stop hart [%u]\n", current_hartid());
	sbi_hart_hang();
}

પૂર્ણાંક sbi_hsm_hart_start(સંરચના sbi_scratch *scratch,
		       અચલ સંરચના sbi_domain *dom,
		       u32 hartid, ulong saddr, ulong smode, ulong arg1)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);
	unsigned long init_ગણતરી, entry_ગણતરી;
	unsigned પૂર્ણાંક hstate;
	સંરચના sbi_scratch *rscratch;
	સંરચના sbi_hsm_data *hdata;
	પૂર્ણાંક rc;

	/* For now, we only allow start mode to be S-mode or U-mode. */
	જો (smode != PRV_S && smode != PRV_U)
		પાછા SBI_EINVAL;
	જો (dom && !sbi_domain_is_assigned_hart(dom, hartindex))
		પાછા SBI_EINVAL;
	જો (dom && !sbi_domain_check_addr(dom, saddr, smode,
					  SBI_DOMAIN_EXECUTE))
		પાછા SBI_EINVALID_ADDR;

	rscratch = sbi_hartindex_to_scratch(hartindex);
	જો (!rscratch)
		પાછા SBI_EINVAL;

	hdata = sbi_scratch_offset_ptr(rscratch, hart_data_offset);
	જો (!hsm_start_ticket_acquire(hdata))
		પાછા SBI_EINVAL;

	init_ગણતરી = sbi_init_ગણતરી(hartindex);
	entry_ગણતરી = sbi_entry_ગણતરી(hartindex);

	rscratch->next_arg1 = arg1;
	rscratch->next_addr = saddr;
	rscratch->next_mode = smode;

	/*
	 * atomic_cmpxchg() is an implicit barrier. It makes sure that
	 * other harts see reading of init_count and writing to *rscratch
	 * before hdata->state is set to SBI_HSM_STATE_START_PENDING.
	 */
	hstate = atomic_cmpxchg(&hdata->state, SBI_HSM_STATE_STOPPED,
				SBI_HSM_STATE_START_PENDING);
	જો (hstate == SBI_HSM_STATE_STARTED) {
		rc = SBI_EALREADY;
		જાઓ err;
	}

	/**
	 * if a hart is already transition to start or stop, another start call
	 * is considered as invalid request.
	 */
	જો (hstate != SBI_HSM_STATE_STOPPED) {
		rc = SBI_EINVAL;
		જાઓ err;
	}

	જો ((hsm_device_has_hart_hotplug() && (entry_ગણતરી == init_ગણતરી)) ||
	   (hsm_device_has_hart_secondary_boot() && !init_ગણતરી)) {
		rc = hsm_device_hart_start(hartid, scratch->warmboot_addr);
	} નહીંતર {
		rc = sbi_ipi_raw_send(hartindex, true);
	}

	જો (!rc)
		પાછા 0;

	/* If it fails to start, change hart state back to stop */
	__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_START_PENDING,
				    SBI_HSM_STATE_STOPPED);
err:
	hsm_start_ticket_release(hdata);
	પાછા rc;
}

પૂર્ણાંક sbi_hsm_hart_stop(સંરચના sbi_scratch *scratch, bool exitnow)
{
	અચલ સંરચના sbi_domain *dom = sbi_domain_thishart_ptr();
	સંરચના sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	જો (!dom)
		પાછા SBI_EFAIL;

	જો (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_STOP_PENDING))
		પાછા SBI_EFAIL;

	જો (exitnow)
		sbi_exit(scratch);

	પાછા 0;
}

સ્થિર પૂર્ણાંક __sbi_hsm_suspend_default(સંરચના sbi_scratch *scratch)
{
	/* Wait for interrupt */
	wfi();

	પાછા 0;
}

શૂન્ય __sbi_hsm_suspend_non_ret_save(સંરચના sbi_scratch *scratch)
{
	સંરચના sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/*
	 * We will be resuming in warm-boot path so the MIE and MIP CSRs
	 * will be back to initial state. It is possible that HART has
	 * configured timer event before going to suspend state so we
	 * should save MIE and MIP CSRs and restore it after resuming.
	 *
	 * Further, the M-mode bits in MIP CSR are read-only and set by
	 * external devices (such as interrupt controller) whereas all
	 * VS-mode bits in MIP are read-only alias of bits in HVIP CSR.
	 *
	 * This means we should only save/restore S-mode bits of MIP CSR
	 * such as MIP.SSIP and MIP.STIP.
	 */

	hdata->saved_mie = csr_read(CSR_MIE);
	hdata->saved_mip = csr_read(CSR_MIP) & (MIP_SSIP | MIP_STIP);
	hdata->saved_medeleg = csr_read(CSR_MEDELEG);
	hdata->saved_mideleg = csr_read(CSR_MIDELEG);
	જો (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12)
		hdata->saved_menvcfg = csr_read64(CSR_MENVCFG);
}

સ્થિર શૂન્ય __sbi_hsm_suspend_non_ret_restore(સંરચના sbi_scratch *scratch)
{
	સંરચના sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	જો (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12)
		csr_write64(CSR_MENVCFG, hdata->saved_menvcfg);
	csr_write(CSR_MIDELEG, hdata->saved_mideleg);
	csr_write(CSR_MEDELEG, hdata->saved_medeleg);
	csr_write(CSR_MIE, hdata->saved_mie);
	csr_set(CSR_MIP, (hdata->saved_mip & (MIP_SSIP | MIP_STIP)));
}

શૂન્ય sbi_hsm_hart_resume_start(સંરચના sbi_scratch *scratch)
{
	સંરચના sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* If current HART was SUSPENDED then set RESUME_PENDING state */
	જો (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_RESUME_PENDING))
		sbi_hart_hang();

	જો (sbi_system_is_suspended())
		sbi_system_resume();
	નહીંતર
		hsm_device_hart_resume();
}

શૂન્ય __noreturn sbi_hsm_hart_resume_finish(સંરચના sbi_scratch *scratch,
					   u32 hartid)
{
	સંરચના sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* If current HART was RESUME_PENDING then set STARTED state */
	જો (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_RESUME_PENDING,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	/*
	 * Restore some of the M-mode CSRs which we are re-configured by
	 * the warm-boot sequence.
	 */
	__sbi_hsm_suspend_non_ret_restore(scratch);

	sbi_hart_switch_mode(hartid, scratch->next_arg1,
			     scratch->next_addr,
			     scratch->next_mode, false);
}

પૂર્ણાંક sbi_hsm_hart_suspend(સંરચના sbi_scratch *scratch, u32 suspend_type,
			 ulong raddr, ulong rmode, ulong arg1)
{
	પૂર્ણાંક ret;
	અચલ સંરચના sbi_domain *dom = sbi_domain_thishart_ptr();
	સંરચના sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* Sanity check on domain assigned to current HART */
	જો (!dom)
		પાછા SBI_EFAIL;

	/* Sanity check on suspend type */
	જો (SBI_HSM_SUSPEND_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_RET_PLATFORM)
		પાછા SBI_EINVAL;
	જો (SBI_HSM_SUSPEND_NON_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_NON_RET_PLATFORM)
		પાછા SBI_EINVAL;

	/* Additional sanity check for non-retentive suspend */
	જો (suspend_type & SBI_HSM_SUSP_NON_RET_BIT) {
		/*
		 * For now, we only allow non-retentive suspend from
		 * S-mode or U-mode.
		 */
		જો (rmode != PRV_S && rmode != PRV_U)
			પાછા SBI_EFAIL;
		જો (dom && !sbi_domain_check_addr(dom, raddr, rmode,
						  SBI_DOMAIN_EXECUTE))
			પાછા SBI_EINVALID_ADDR;
	}

	/* Save the resume address and resume mode */
	scratch->next_arg1 = arg1;
	scratch->next_addr = raddr;
	scratch->next_mode = rmode;

	/* Directly move from STARTED to SUSPENDED state */
	જો (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_SUSPENDED))
		પાછા SBI_EFAIL;

	/* Save the suspend type */
	hdata->suspend_type = suspend_type;

	/*
	 * Save context which will be restored after resuming from
	 * non-retentive suspend.
	 */
	જો (suspend_type & SBI_HSM_SUSP_NON_RET_BIT)
		__sbi_hsm_suspend_non_ret_save(scratch);

	/* Try platform specific suspend */
	ret = hsm_device_hart_suspend(suspend_type, scratch->warmboot_addr);
	જો (ret == SBI_ENOTSUPP) {
		/* Try generic implementation of default suspend types */
		જો (suspend_type == SBI_HSM_SUSPEND_RET_DEFAULT ||
		    suspend_type == SBI_HSM_SUSPEND_NON_RET_DEFAULT) {
			ret = __sbi_hsm_suspend_default(scratch);
		}
	}

	/*
	 * The platform may have coordinated a retentive suspend, or it may
	 * have exited early from a non-retentive suspend. Either way, the
	 * caller is not expecting a successful return, so jump to the warm
	 * boot entry point to simulate resume from a non-retentive suspend.
	 */
	જો (ret == 0 && (suspend_type & SBI_HSM_SUSP_NON_RET_BIT)) {
		શૂન્ય (*jump_warmboot)(શૂન્ય) =
			(શૂન્ય (*)(શૂન્ય))scratch->warmboot_addr;

		jump_warmboot();
	}

	/*
	 * We might have successfully resumed from retentive suspend
	 * or suspend failed. In both cases, we restore state of hart.
	 */
	જો (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	પાછા ret;
}
