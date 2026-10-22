/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/riscv_barrier.h>
#അന്തര്ഭാവ <sbi/riscv_encoding.h>
#അന്തര്ഭാവ <sbi/riscv_atomic.h>
#അന്തര്ഭാവ <sbi/sbi_bitops.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_ecall_interface.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_hartmask.h>
#അന്തര്ഭാവ <sbi/sbi_hsm.h>
#അന്തര്ഭാവ <sbi/sbi_init.h>
#അന്തര്ഭാവ <sbi/sbi_ipi.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi/sbi_system.h>
#അന്തര്ഭാവ <sbi/sbi_timer.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>

#define __sbi_hsm_hart_change_state(hdata, oldstate, newstate)		\
({									\
	long state = atomic_cmpxchg(&(hdata)->state, oldstate, newstate); \
	എങ്കിൽ (state != (oldstate))					\
		sbi_printf("%s: ERR: The hart is in invalid state [%lu]\n", \
			   __func__, state);				\
	state == (oldstate);						\
})

സ്ഥിതം സ്ഥിരം ഘടന sbi_hsm_device *hsm_dev = ശൂന്യ;
സ്ഥിതം unsigned long hart_data_offset;
സ്ഥിതം bool hsm_device_has_hart_hotplug(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ hsm_device_hart_stop(ശൂന്യം);

/** Per hart specific data to manage state transition **/
ഘടന sbi_hsm_data {
	atomic_t state;
	unsigned long suspend_type;
	unsigned long saved_mie;
	unsigned long saved_mip;
	unsigned long saved_medeleg;
	unsigned long saved_mideleg;
	u64 saved_menvcfg;
	atomic_t start_ticket;
};

bool sbi_hsm_hart_change_state(ഘടന sbi_scratch *scratch, long oldstate,
			       long newstate)
{
	ഘടന sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	തിരികെ __sbi_hsm_hart_change_state(hdata, oldstate, newstate);
}

പൂർണ്ണസംഖ്യ __sbi_hsm_hart_get_state(u32 hartindex)
{
	ഘടന sbi_hsm_data *hdata;
	ഘടന sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	എങ്കിൽ (!scratch)
		തിരികെ SBI_EINVAL;

	hdata = sbi_scratch_offset_ptr(scratch, hart_data_offset);
	തിരികെ atomic_read(&hdata->state);
}

പൂർണ്ണസംഖ്യ sbi_hsm_hart_get_state(സ്ഥിരം ഘടന sbi_domain *dom, u32 hartid)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);

	എങ്കിൽ (!sbi_domain_is_assigned_hart(dom, hartindex))
		തിരികെ SBI_EINVAL;

	തിരികെ __sbi_hsm_hart_get_state(hartindex);
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
സ്ഥിതം bool hsm_start_ticket_acquire(ഘടന sbi_hsm_data *hdata)
{
	തിരികെ (atomic_cmpxchg(&hdata->start_ticket, 0, 1) == 0);
}

/*
 * Release the ticket for the given target hart.
 *
 * The function has "release" semantics: no memory operations preceding it
 * in the current hart can be seen after it by other harts.
 */
സ്ഥിതം ശൂന്യം hsm_start_ticket_release(ഘടന sbi_hsm_data *hdata)
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
പൂർണ്ണസംഖ്യ sbi_hsm_hart_interruptible_mask(സ്ഥിരം ഘടന sbi_domain *dom,
				    ഘടന sbi_hartmask *mask)
{
	പൂർണ്ണസംഖ്യ hstate, ret;
	u32 i;

	ret = sbi_domain_get_assigned_hartmask(dom, mask);
	എങ്കിൽ (ret)
		തിരികെ ret;

	sbi_hartmask_for_each_hartindex(i, mask) {
		hstate = __sbi_hsm_hart_get_state(i);
		എങ്കിൽ (hstate != SBI_HSM_STATE_STARTED &&
		    hstate != SBI_HSM_STATE_SUSPENDED &&
		    hstate != SBI_HSM_STATE_RESUME_PENDING)
			sbi_hartmask_clear_hartindex(i, mask);
	}

	തിരികെ 0;
}

ശൂന്യം __noreturn sbi_hsm_hart_start_finish(ഘടന sbi_scratch *scratch,
					  u32 hartid)
{
	unsigned long next_arg1;
	unsigned long next_addr;
	unsigned long next_mode;
	ഘടന sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	എങ്കിൽ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_START_PENDING,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	next_arg1 = scratch->next_arg1;
	next_addr = scratch->next_addr;
	next_mode = scratch->next_mode;
	hsm_start_ticket_release(hdata);

	sbi_hart_switch_mode(hartid, next_arg1, next_addr, next_mode, false);
}

സ്ഥിതം ശൂന്യം sbi_hsm_hart_wait(ഘടന sbi_scratch *scratch)
{
	unsigned long saved_mie;
	ഘടന sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	/* Save MIE CSR */
	saved_mie = csr_read(CSR_MIE);

	/* Set MSIE and MEIE bits to receive IPI */
	csr_set(CSR_MIE, MIP_MSIP | MIP_MEIP);

	/* Wait for state transition requested by sbi_hsm_hart_start() */
	ആകുമ്പോൾ (atomic_read(&hdata->state) != SBI_HSM_STATE_START_PENDING) {
		/*
		 * If the hsm_dev is ready and it support the hotplug, we can
		 * use the hsm stop for more power saving
		 */
		എങ്കിൽ (hsm_device_has_hart_hotplug()) {
			sbi_revert_entry_എണ്ണം(scratch);
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

സ്ഥിരം ഘടന sbi_hsm_device *sbi_hsm_get_device(ശൂന്യം)
{
	തിരികെ hsm_dev;
}

ശൂന്യം sbi_hsm_set_device(സ്ഥിരം ഘടന sbi_hsm_device *dev)
{
	എങ്കിൽ (!dev || hsm_dev)
		തിരികെ;

	hsm_dev = dev;
}

സ്ഥിതം bool hsm_device_has_hart_hotplug(ശൂന്യം)
{
	എങ്കിൽ (hsm_dev && hsm_dev->hart_start && hsm_dev->hart_stop)
		തിരികെ true;
	തിരികെ false;
}

സ്ഥിതം bool hsm_device_has_hart_secondary_boot(ശൂന്യം)
{
	എങ്കിൽ (hsm_dev && hsm_dev->hart_start && !hsm_dev->hart_stop)
		തിരികെ true;
	തിരികെ false;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ hsm_device_hart_start(u32 hartid, ulong saddr)
{
	എങ്കിൽ (hsm_dev && hsm_dev->hart_start)
		തിരികെ hsm_dev->hart_start(hartid, saddr);
	തിരികെ SBI_ENOTSUPP;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ hsm_device_hart_stop(ശൂന്യം)
{
	എങ്കിൽ (hsm_dev && hsm_dev->hart_stop)
		തിരികെ hsm_dev->hart_stop();
	തിരികെ SBI_ENOTSUPP;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ hsm_device_hart_suspend(u32 suspend_type, ulong mmode_resume_addr)
{
	എങ്കിൽ (hsm_dev && hsm_dev->hart_suspend)
		തിരികെ hsm_dev->hart_suspend(suspend_type, mmode_resume_addr);
	തിരികെ SBI_ENOTSUPP;
}

സ്ഥിതം ശൂന്യം hsm_device_hart_resume(ശൂന്യം)
{
	എങ്കിൽ (hsm_dev && hsm_dev->hart_resume)
		hsm_dev->hart_resume();
}

പൂർണ്ണസംഖ്യ sbi_hsm_init(ഘടന sbi_scratch *scratch, bool cold_boot)
{
	ഘടന sbi_scratch *rscratch;
	ഘടന sbi_hsm_data *hdata;

	എങ്കിൽ (cold_boot) {
		hart_data_offset = sbi_scratch_alloc_offset(വലുപ്പം(*hdata));
		എങ്കിൽ (!hart_data_offset)
			തിരികെ SBI_ENOMEM;

		/* Initialize hart state data for every hart */
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_to_scratch(i);
			എങ്കിൽ (!rscratch)
				തുടരുക;

			hdata = sbi_scratch_offset_ptr(rscratch,
						       hart_data_offset);
			ATOMIC_INIT(&hdata->state,
				    (i == current_hartindex()) ?
				    SBI_HSM_STATE_START_PENDING :
				    SBI_HSM_STATE_STOPPED);
			ATOMIC_INIT(&hdata->start_ticket, 0);
		}
	} അല്ലെങ്കിൽ {
		sbi_hsm_hart_wait(scratch);
	}

	തിരികെ 0;
}

ശൂന്യം __noreturn sbi_hsm_exit(ഘടന sbi_scratch *scratch)
{
	ഘടന sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	ശൂന്യം (*jump_warmboot)(ശൂന്യം) = (ശൂന്യം (*)(ശൂന്യം))scratch->warmboot_addr;

	എങ്കിൽ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STOP_PENDING,
					 SBI_HSM_STATE_STOPPED))
		പോകുക fail_exit;

	എങ്കിൽ (hsm_device_has_hart_hotplug()) {
		എങ്കിൽ (hsm_device_hart_stop() != SBI_ENOTSUPP)
			പോകുക fail_exit;
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

പൂർണ്ണസംഖ്യ sbi_hsm_hart_start(ഘടന sbi_scratch *scratch,
		       സ്ഥിരം ഘടന sbi_domain *dom,
		       u32 hartid, ulong saddr, ulong smode, ulong arg1)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);
	unsigned long init_എണ്ണം, entry_എണ്ണം;
	unsigned പൂർണ്ണസംഖ്യ hstate;
	ഘടന sbi_scratch *rscratch;
	ഘടന sbi_hsm_data *hdata;
	പൂർണ്ണസംഖ്യ rc;

	/* For now, we only allow start mode to be S-mode or U-mode. */
	എങ്കിൽ (smode != PRV_S && smode != PRV_U)
		തിരികെ SBI_EINVAL;
	എങ്കിൽ (dom && !sbi_domain_is_assigned_hart(dom, hartindex))
		തിരികെ SBI_EINVAL;
	എങ്കിൽ (dom && !sbi_domain_check_addr(dom, saddr, smode,
					  SBI_DOMAIN_EXECUTE))
		തിരികെ SBI_EINVALID_ADDR;

	rscratch = sbi_hartindex_to_scratch(hartindex);
	എങ്കിൽ (!rscratch)
		തിരികെ SBI_EINVAL;

	hdata = sbi_scratch_offset_ptr(rscratch, hart_data_offset);
	എങ്കിൽ (!hsm_start_ticket_acquire(hdata))
		തിരികെ SBI_EINVAL;

	init_എണ്ണം = sbi_init_എണ്ണം(hartindex);
	entry_എണ്ണം = sbi_entry_എണ്ണം(hartindex);

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
	എങ്കിൽ (hstate == SBI_HSM_STATE_STARTED) {
		rc = SBI_EALREADY;
		പോകുക err;
	}

	/**
	 * if a hart is already transition to start or stop, another start call
	 * is considered as invalid request.
	 */
	എങ്കിൽ (hstate != SBI_HSM_STATE_STOPPED) {
		rc = SBI_EINVAL;
		പോകുക err;
	}

	എങ്കിൽ ((hsm_device_has_hart_hotplug() && (entry_എണ്ണം == init_എണ്ണം)) ||
	   (hsm_device_has_hart_secondary_boot() && !init_എണ്ണം)) {
		rc = hsm_device_hart_start(hartid, scratch->warmboot_addr);
	} അല്ലെങ്കിൽ {
		rc = sbi_ipi_raw_send(hartindex, true);
	}

	എങ്കിൽ (!rc)
		തിരികെ 0;

	/* If it fails to start, change hart state back to stop */
	__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_START_PENDING,
				    SBI_HSM_STATE_STOPPED);
err:
	hsm_start_ticket_release(hdata);
	തിരികെ rc;
}

പൂർണ്ണസംഖ്യ sbi_hsm_hart_stop(ഘടന sbi_scratch *scratch, bool exitnow)
{
	സ്ഥിരം ഘടന sbi_domain *dom = sbi_domain_thishart_ptr();
	ഘടന sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	എങ്കിൽ (!dom)
		തിരികെ SBI_EFAIL;

	എങ്കിൽ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_STOP_PENDING))
		തിരികെ SBI_EFAIL;

	എങ്കിൽ (exitnow)
		sbi_exit(scratch);

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ __sbi_hsm_suspend_default(ഘടന sbi_scratch *scratch)
{
	/* Wait for interrupt */
	wfi();

	തിരികെ 0;
}

ശൂന്യം __sbi_hsm_suspend_non_ret_save(ഘടന sbi_scratch *scratch)
{
	ഘടന sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
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
	എങ്കിൽ (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12)
		hdata->saved_menvcfg = csr_read64(CSR_MENVCFG);
}

സ്ഥിതം ശൂന്യം __sbi_hsm_suspend_non_ret_restore(ഘടന sbi_scratch *scratch)
{
	ഘടന sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	എങ്കിൽ (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12)
		csr_write64(CSR_MENVCFG, hdata->saved_menvcfg);
	csr_write(CSR_MIDELEG, hdata->saved_mideleg);
	csr_write(CSR_MEDELEG, hdata->saved_medeleg);
	csr_write(CSR_MIE, hdata->saved_mie);
	csr_set(CSR_MIP, (hdata->saved_mip & (MIP_SSIP | MIP_STIP)));
}

ശൂന്യം sbi_hsm_hart_resume_start(ഘടന sbi_scratch *scratch)
{
	ഘടന sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* If current HART was SUSPENDED then set RESUME_PENDING state */
	എങ്കിൽ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_RESUME_PENDING))
		sbi_hart_hang();

	എങ്കിൽ (sbi_system_is_suspended())
		sbi_system_resume();
	അല്ലെങ്കിൽ
		hsm_device_hart_resume();
}

ശൂന്യം __noreturn sbi_hsm_hart_resume_finish(ഘടന sbi_scratch *scratch,
					   u32 hartid)
{
	ഘടന sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* If current HART was RESUME_PENDING then set STARTED state */
	എങ്കിൽ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_RESUME_PENDING,
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

പൂർണ്ണസംഖ്യ sbi_hsm_hart_suspend(ഘടന sbi_scratch *scratch, u32 suspend_type,
			 ulong raddr, ulong rmode, ulong arg1)
{
	പൂർണ്ണസംഖ്യ ret;
	സ്ഥിരം ഘടന sbi_domain *dom = sbi_domain_thishart_ptr();
	ഘടന sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* Sanity check on domain assigned to current HART */
	എങ്കിൽ (!dom)
		തിരികെ SBI_EFAIL;

	/* Sanity check on suspend type */
	എങ്കിൽ (SBI_HSM_SUSPEND_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_RET_PLATFORM)
		തിരികെ SBI_EINVAL;
	എങ്കിൽ (SBI_HSM_SUSPEND_NON_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_NON_RET_PLATFORM)
		തിരികെ SBI_EINVAL;

	/* Additional sanity check for non-retentive suspend */
	എങ്കിൽ (suspend_type & SBI_HSM_SUSP_NON_RET_BIT) {
		/*
		 * For now, we only allow non-retentive suspend from
		 * S-mode or U-mode.
		 */
		എങ്കിൽ (rmode != PRV_S && rmode != PRV_U)
			തിരികെ SBI_EFAIL;
		എങ്കിൽ (dom && !sbi_domain_check_addr(dom, raddr, rmode,
						  SBI_DOMAIN_EXECUTE))
			തിരികെ SBI_EINVALID_ADDR;
	}

	/* Save the resume address and resume mode */
	scratch->next_arg1 = arg1;
	scratch->next_addr = raddr;
	scratch->next_mode = rmode;

	/* Directly move from STARTED to SUSPENDED state */
	എങ്കിൽ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_SUSPENDED))
		തിരികെ SBI_EFAIL;

	/* Save the suspend type */
	hdata->suspend_type = suspend_type;

	/*
	 * Save context which will be restored after resuming from
	 * non-retentive suspend.
	 */
	എങ്കിൽ (suspend_type & SBI_HSM_SUSP_NON_RET_BIT)
		__sbi_hsm_suspend_non_ret_save(scratch);

	/* Try platform specific suspend */
	ret = hsm_device_hart_suspend(suspend_type, scratch->warmboot_addr);
	എങ്കിൽ (ret == SBI_ENOTSUPP) {
		/* Try generic implementation of default suspend types */
		എങ്കിൽ (suspend_type == SBI_HSM_SUSPEND_RET_DEFAULT ||
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
	എങ്കിൽ (ret == 0 && (suspend_type & SBI_HSM_SUSP_NON_RET_BIT)) {
		ശൂന്യം (*jump_warmboot)(ശൂന്യം) =
			(ശൂന്യം (*)(ശൂന്യം))scratch->warmboot_addr;

		jump_warmboot();
	}

	/*
	 * We might have successfully resumed from retentive suspend
	 * or suspend failed. In both cases, we restore state of hart.
	 */
	എങ്കിൽ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	തിരികെ ret;
}
