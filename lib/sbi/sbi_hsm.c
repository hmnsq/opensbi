/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#অন্তর্ভাৱ <sbi/riscv_asm.h>
#অন্তর্ভাৱ <sbi/riscv_barrier.h>
#অন্তর্ভাৱ <sbi/riscv_encoding.h>
#অন্তর্ভাৱ <sbi/riscv_atomic.h>
#অন্তর্ভাৱ <sbi/sbi_bitops.h>
#অন্তর্ভাৱ <sbi/sbi_console.h>
#অন্তর্ভাৱ <sbi/sbi_domain.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_ecall_interface.h>
#অন্তর্ভাৱ <sbi/sbi_hart.h>
#অন্তর্ভাৱ <sbi/sbi_hartmask.h>
#অন্তর্ভাৱ <sbi/sbi_hsm.h>
#অন্তর্ভাৱ <sbi/sbi_init.h>
#অন্তর্ভাৱ <sbi/sbi_ipi.h>
#অন্তর্ভাৱ <sbi/sbi_scratch.h>
#অন্তর্ভাৱ <sbi/sbi_system.h>
#অন্তর্ভাৱ <sbi/sbi_timer.h>
#অন্তর্ভাৱ <sbi/sbi_console.h>

#define __sbi_hsm_hart_change_state(hdata, oldstate, newstate)		\
({									\
	long state = atomic_cmpxchg(&(hdata)->state, oldstate, newstate); \
	যদি (state != (oldstate))					\
		sbi_printf("%s: ERR: The hart is in invalid state [%lu]\n", \
			   __func__, state);				\
	state == (oldstate);						\
})

স্থির ধ্রুৱ সংরচনা sbi_hsm_device *hsm_dev = রিক্ত;
স্থির unsigned long hart_data_offset;
স্থির bool hsm_device_has_hart_hotplug(শূন্য);
স্থির পূর্ণাঙ্ক hsm_device_hart_stop(শূন্য);

/** Per hart specific data to manage state transition **/
সংরচনা sbi_hsm_data {
	atomic_t state;
	unsigned long suspend_type;
	unsigned long saved_mie;
	unsigned long saved_mip;
	unsigned long saved_medeleg;
	unsigned long saved_mideleg;
	u64 saved_menvcfg;
	atomic_t start_ticket;
};

bool sbi_hsm_hart_change_state(সংরচনা sbi_scratch *scratch, long oldstate,
			       long newstate)
{
	সংরচনা sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	ঘূৰাই __sbi_hsm_hart_change_state(hdata, oldstate, newstate);
}

পূর্ণাঙ্ক __sbi_hsm_hart_get_state(u32 hartindex)
{
	সংরচনা sbi_hsm_data *hdata;
	সংরচনা sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	যদি (!scratch)
		ঘূৰাই SBI_EINVAL;

	hdata = sbi_scratch_offset_ptr(scratch, hart_data_offset);
	ঘূৰাই atomic_read(&hdata->state);
}

পূর্ণাঙ্ক sbi_hsm_hart_get_state(ধ্রুৱ সংরচনা sbi_domain *dom, u32 hartid)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);

	যদি (!sbi_domain_is_assigned_hart(dom, hartindex))
		ঘূৰাই SBI_EINVAL;

	ঘূৰাই __sbi_hsm_hart_get_state(hartindex);
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
স্থির bool hsm_start_ticket_acquire(সংরচনা sbi_hsm_data *hdata)
{
	ঘূৰাই (atomic_cmpxchg(&hdata->start_ticket, 0, 1) == 0);
}

/*
 * Release the ticket for the given target hart.
 *
 * The function has "release" semantics: no memory operations preceding it
 * in the current hart can be seen after it by other harts.
 */
স্থির শূন্য hsm_start_ticket_release(সংরচনা sbi_hsm_data *hdata)
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
পূর্ণাঙ্ক sbi_hsm_hart_interruptible_mask(ধ্রুৱ সংরচনা sbi_domain *dom,
				    সংরচনা sbi_hartmask *mask)
{
	পূর্ণাঙ্ক hstate, ret;
	u32 i;

	ret = sbi_domain_get_assigned_hartmask(dom, mask);
	যদি (ret)
		ঘূৰাই ret;

	sbi_hartmask_for_each_hartindex(i, mask) {
		hstate = __sbi_hsm_hart_get_state(i);
		যদি (hstate != SBI_HSM_STATE_STARTED &&
		    hstate != SBI_HSM_STATE_SUSPENDED &&
		    hstate != SBI_HSM_STATE_RESUME_PENDING)
			sbi_hartmask_clear_hartindex(i, mask);
	}

	ঘূৰাই 0;
}

শূন্য __noreturn sbi_hsm_hart_start_finish(সংরচনা sbi_scratch *scratch,
					  u32 hartid)
{
	unsigned long next_arg1;
	unsigned long next_addr;
	unsigned long next_mode;
	সংরচনা sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	যদি (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_START_PENDING,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	next_arg1 = scratch->next_arg1;
	next_addr = scratch->next_addr;
	next_mode = scratch->next_mode;
	hsm_start_ticket_release(hdata);

	sbi_hart_switch_mode(hartid, next_arg1, next_addr, next_mode, false);
}

স্থির শূন্য sbi_hsm_hart_wait(সংরচনা sbi_scratch *scratch)
{
	unsigned long saved_mie;
	সংরচনা sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	/* Save MIE CSR */
	saved_mie = csr_read(CSR_MIE);

	/* Set MSIE and MEIE bits to receive IPI */
	csr_set(CSR_MIE, MIP_MSIP | MIP_MEIP);

	/* Wait for state transition requested by sbi_hsm_hart_start() */
	যেতিয়ালৈ (atomic_read(&hdata->state) != SBI_HSM_STATE_START_PENDING) {
		/*
		 * If the hsm_dev is ready and it support the hotplug, we can
		 * use the hsm stop for more power saving
		 */
		যদি (hsm_device_has_hart_hotplug()) {
			sbi_revert_entry_গুনতি(scratch);
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

ধ্রুৱ সংরচনা sbi_hsm_device *sbi_hsm_get_device(শূন্য)
{
	ঘূৰাই hsm_dev;
}

শূন্য sbi_hsm_set_device(ধ্রুৱ সংরচনা sbi_hsm_device *dev)
{
	যদি (!dev || hsm_dev)
		ঘূৰাই;

	hsm_dev = dev;
}

স্থির bool hsm_device_has_hart_hotplug(শূন্য)
{
	যদি (hsm_dev && hsm_dev->hart_start && hsm_dev->hart_stop)
		ঘূৰাই true;
	ঘূৰাই false;
}

স্থির bool hsm_device_has_hart_secondary_boot(শূন্য)
{
	যদি (hsm_dev && hsm_dev->hart_start && !hsm_dev->hart_stop)
		ঘূৰাই true;
	ঘূৰাই false;
}

স্থির পূর্ণাঙ্ক hsm_device_hart_start(u32 hartid, ulong saddr)
{
	যদি (hsm_dev && hsm_dev->hart_start)
		ঘূৰাই hsm_dev->hart_start(hartid, saddr);
	ঘূৰাই SBI_ENOTSUPP;
}

স্থির পূর্ণাঙ্ক hsm_device_hart_stop(শূন্য)
{
	যদি (hsm_dev && hsm_dev->hart_stop)
		ঘূৰাই hsm_dev->hart_stop();
	ঘূৰাই SBI_ENOTSUPP;
}

স্থির পূর্ণাঙ্ক hsm_device_hart_suspend(u32 suspend_type, ulong mmode_resume_addr)
{
	যদি (hsm_dev && hsm_dev->hart_suspend)
		ঘূৰাই hsm_dev->hart_suspend(suspend_type, mmode_resume_addr);
	ঘূৰাই SBI_ENOTSUPP;
}

স্থির শূন্য hsm_device_hart_resume(শূন্য)
{
	যদি (hsm_dev && hsm_dev->hart_resume)
		hsm_dev->hart_resume();
}

পূর্ণাঙ্ক sbi_hsm_init(সংরচনা sbi_scratch *scratch, bool cold_boot)
{
	সংরচনা sbi_scratch *rscratch;
	সংরচনা sbi_hsm_data *hdata;

	যদি (cold_boot) {
		hart_data_offset = sbi_scratch_alloc_offset(পরিমাণ(*hdata));
		যদি (!hart_data_offset)
			ঘূৰাই SBI_ENOMEM;

		/* Initialize hart state data for every hart */
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_to_scratch(i);
			যদি (!rscratch)
				অনুৱর্ত;

			hdata = sbi_scratch_offset_ptr(rscratch,
						       hart_data_offset);
			ATOMIC_INIT(&hdata->state,
				    (i == current_hartindex()) ?
				    SBI_HSM_STATE_START_PENDING :
				    SBI_HSM_STATE_STOPPED);
			ATOMIC_INIT(&hdata->start_ticket, 0);
		}
	} অন্যথা {
		sbi_hsm_hart_wait(scratch);
	}

	ঘূৰাই 0;
}

শূন্য __noreturn sbi_hsm_exit(সংরচনা sbi_scratch *scratch)
{
	সংরচনা sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	শূন্য (*jump_warmboot)(শূন্য) = (শূন্য (*)(শূন্য))scratch->warmboot_addr;

	যদি (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STOP_PENDING,
					 SBI_HSM_STATE_STOPPED))
		গচ্ছ fail_exit;

	যদি (hsm_device_has_hart_hotplug()) {
		যদি (hsm_device_hart_stop() != SBI_ENOTSUPP)
			গচ্ছ fail_exit;
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

পূর্ণাঙ্ক sbi_hsm_hart_start(সংরচনা sbi_scratch *scratch,
		       ধ্রুৱ সংরচনা sbi_domain *dom,
		       u32 hartid, ulong saddr, ulong smode, ulong arg1)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);
	unsigned long init_গুনতি, entry_গুনতি;
	unsigned পূর্ণাঙ্ক hstate;
	সংরচনা sbi_scratch *rscratch;
	সংরচনা sbi_hsm_data *hdata;
	পূর্ণাঙ্ক rc;

	/* For now, we only allow start mode to be S-mode or U-mode. */
	যদি (smode != PRV_S && smode != PRV_U)
		ঘূৰাই SBI_EINVAL;
	যদি (dom && !sbi_domain_is_assigned_hart(dom, hartindex))
		ঘূৰাই SBI_EINVAL;
	যদি (dom && !sbi_domain_check_addr(dom, saddr, smode,
					  SBI_DOMAIN_EXECUTE))
		ঘূৰাই SBI_EINVALID_ADDR;

	rscratch = sbi_hartindex_to_scratch(hartindex);
	যদি (!rscratch)
		ঘূৰাই SBI_EINVAL;

	hdata = sbi_scratch_offset_ptr(rscratch, hart_data_offset);
	যদি (!hsm_start_ticket_acquire(hdata))
		ঘূৰাই SBI_EINVAL;

	init_গুনতি = sbi_init_গুনতি(hartindex);
	entry_গুনতি = sbi_entry_গুনতি(hartindex);

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
	যদি (hstate == SBI_HSM_STATE_STARTED) {
		rc = SBI_EALREADY;
		গচ্ছ err;
	}

	/**
	 * if a hart is already transition to start or stop, another start call
	 * is considered as invalid request.
	 */
	যদি (hstate != SBI_HSM_STATE_STOPPED) {
		rc = SBI_EINVAL;
		গচ্ছ err;
	}

	যদি ((hsm_device_has_hart_hotplug() && (entry_গুনতি == init_গুনতি)) ||
	   (hsm_device_has_hart_secondary_boot() && !init_গুনতি)) {
		rc = hsm_device_hart_start(hartid, scratch->warmboot_addr);
	} অন্যথা {
		rc = sbi_ipi_raw_send(hartindex, true);
	}

	যদি (!rc)
		ঘূৰাই 0;

	/* If it fails to start, change hart state back to stop */
	__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_START_PENDING,
				    SBI_HSM_STATE_STOPPED);
err:
	hsm_start_ticket_release(hdata);
	ঘূৰাই rc;
}

পূর্ণাঙ্ক sbi_hsm_hart_stop(সংরচনা sbi_scratch *scratch, bool exitnow)
{
	ধ্রুৱ সংরচনা sbi_domain *dom = sbi_domain_thishart_ptr();
	সংরচনা sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	যদি (!dom)
		ঘূৰাই SBI_EFAIL;

	যদি (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_STOP_PENDING))
		ঘূৰাই SBI_EFAIL;

	যদি (exitnow)
		sbi_exit(scratch);

	ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক __sbi_hsm_suspend_default(সংরচনা sbi_scratch *scratch)
{
	/* Wait for interrupt */
	wfi();

	ঘূৰাই 0;
}

শূন্য __sbi_hsm_suspend_non_ret_save(সংরচনা sbi_scratch *scratch)
{
	সংরচনা sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
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
	যদি (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12)
		hdata->saved_menvcfg = csr_read64(CSR_MENVCFG);
}

স্থির শূন্য __sbi_hsm_suspend_non_ret_restore(সংরচনা sbi_scratch *scratch)
{
	সংরচনা sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	যদি (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12)
		csr_write64(CSR_MENVCFG, hdata->saved_menvcfg);
	csr_write(CSR_MIDELEG, hdata->saved_mideleg);
	csr_write(CSR_MEDELEG, hdata->saved_medeleg);
	csr_write(CSR_MIE, hdata->saved_mie);
	csr_set(CSR_MIP, (hdata->saved_mip & (MIP_SSIP | MIP_STIP)));
}

শূন্য sbi_hsm_hart_resume_start(সংরচনা sbi_scratch *scratch)
{
	সংরচনা sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* If current HART was SUSPENDED then set RESUME_PENDING state */
	যদি (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_RESUME_PENDING))
		sbi_hart_hang();

	যদি (sbi_system_is_suspended())
		sbi_system_resume();
	অন্যথা
		hsm_device_hart_resume();
}

শূন্য __noreturn sbi_hsm_hart_resume_finish(সংরচনা sbi_scratch *scratch,
					   u32 hartid)
{
	সংরচনা sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* If current HART was RESUME_PENDING then set STARTED state */
	যদি (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_RESUME_PENDING,
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

পূর্ণাঙ্ক sbi_hsm_hart_suspend(সংরচনা sbi_scratch *scratch, u32 suspend_type,
			 ulong raddr, ulong rmode, ulong arg1)
{
	পূর্ণাঙ্ক ret;
	ধ্রুৱ সংরচনা sbi_domain *dom = sbi_domain_thishart_ptr();
	সংরচনা sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* Sanity check on domain assigned to current HART */
	যদি (!dom)
		ঘূৰাই SBI_EFAIL;

	/* Sanity check on suspend type */
	যদি (SBI_HSM_SUSPEND_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_RET_PLATFORM)
		ঘূৰাই SBI_EINVAL;
	যদি (SBI_HSM_SUSPEND_NON_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_NON_RET_PLATFORM)
		ঘূৰাই SBI_EINVAL;

	/* Additional sanity check for non-retentive suspend */
	যদি (suspend_type & SBI_HSM_SUSP_NON_RET_BIT) {
		/*
		 * For now, we only allow non-retentive suspend from
		 * S-mode or U-mode.
		 */
		যদি (rmode != PRV_S && rmode != PRV_U)
			ঘূৰাই SBI_EFAIL;
		যদি (dom && !sbi_domain_check_addr(dom, raddr, rmode,
						  SBI_DOMAIN_EXECUTE))
			ঘূৰাই SBI_EINVALID_ADDR;
	}

	/* Save the resume address and resume mode */
	scratch->next_arg1 = arg1;
	scratch->next_addr = raddr;
	scratch->next_mode = rmode;

	/* Directly move from STARTED to SUSPENDED state */
	যদি (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_SUSPENDED))
		ঘূৰাই SBI_EFAIL;

	/* Save the suspend type */
	hdata->suspend_type = suspend_type;

	/*
	 * Save context which will be restored after resuming from
	 * non-retentive suspend.
	 */
	যদি (suspend_type & SBI_HSM_SUSP_NON_RET_BIT)
		__sbi_hsm_suspend_non_ret_save(scratch);

	/* Try platform specific suspend */
	ret = hsm_device_hart_suspend(suspend_type, scratch->warmboot_addr);
	যদি (ret == SBI_ENOTSUPP) {
		/* Try generic implementation of default suspend types */
		যদি (suspend_type == SBI_HSM_SUSPEND_RET_DEFAULT ||
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
	যদি (ret == 0 && (suspend_type & SBI_HSM_SUSP_NON_RET_BIT)) {
		শূন্য (*jump_warmboot)(শূন্য) =
			(শূন্য (*)(শূন্য))scratch->warmboot_addr;

		jump_warmboot();
	}

	/*
	 * We might have successfully resumed from retentive suspend
	 * or suspend failed. In both cases, we restore state of hart.
	 */
	যদি (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	ঘূৰাই ret;
}
