/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/riscv_barrier.h>
#অন্তর্ভাব <sbi/riscv_encoding.h>
#অন্তর্ভাব <sbi/riscv_atomic.h>
#অন্তর্ভাব <sbi/sbi_bitops.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_ecall_interface.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_hartmask.h>
#অন্তর্ভাব <sbi/sbi_hsm.h>
#অন্তর্ভাব <sbi/sbi_init.h>
#অন্তর্ভাব <sbi/sbi_ipi.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>
#অন্তর্ভাব <sbi/sbi_system.h>
#অন্তর্ভাব <sbi/sbi_timer.h>
#অন্তর্ভাব <sbi/sbi_console.h>

#define __sbi_hsm_hart_change_state(hdata, oldstate, newstate)		\
({									\
	long state = atomic_cmpxchg(&(hdata)->state, oldstate, newstate); \
	যদি (state != (oldstate))					\
		sbi_printf("%s: ERR: The hart is in invalid state [%lu]\n", \
			   __func__, state);				\
	state == (oldstate);						\
})

স্থির ধ্রুবক কাঠামো sbi_hsm_device *hsm_dev = ফাঁকা;
স্থির unsigned long hart_তথ্য_offset;
স্থির bool hsm_device_has_hart_hotplug(শূন্য);
স্থির পূর্ণসংখ্যা hsm_device_hart_stop(শূন্য);

/** Per hart specific data to manage state transition **/
কাঠামো sbi_hsm_তথ্য {
	atomic_t state;
	unsigned long suspend_type;
	unsigned long saved_mie;
	unsigned long saved_mip;
	unsigned long saved_medeleg;
	unsigned long saved_mideleg;
	u64 saved_menvcfg;
	atomic_t start_ticket;
};

bool sbi_hsm_hart_change_state(কাঠামো sbi_scratch *scratch, long oldstate,
			       long newstate)
{
	কাঠামো sbi_hsm_তথ্য *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_তথ্য_offset);
	ফেরত __sbi_hsm_hart_change_state(hdata, oldstate, newstate);
}

পূর্ণসংখ্যা __sbi_hsm_hart_get_state(u32 hartindex)
{
	কাঠামো sbi_hsm_তথ্য *hdata;
	কাঠামো sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	যদি (!scratch)
		ফেরত SBI_EINVAL;

	hdata = sbi_scratch_offset_ptr(scratch, hart_তথ্য_offset);
	ফেরত atomic_read(&hdata->state);
}

পূর্ণসংখ্যা sbi_hsm_hart_get_state(ধ্রুবক কাঠামো sbi_domain *dom, u32 hartid)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);

	যদি (!sbi_domain_is_assigned_hart(dom, hartindex))
		ফেরত SBI_EINVAL;

	ফেরত __sbi_hsm_hart_get_state(hartindex);
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
স্থির bool hsm_start_ticket_acquire(কাঠামো sbi_hsm_তথ্য *hdata)
{
	ফেরত (atomic_cmpxchg(&hdata->start_ticket, 0, 1) == 0);
}

/*
 * Release the ticket for the given target hart.
 *
 * The function has "release" semantics: no memory operations preceding it
 * in the current hart can be seen after it by other harts.
 */
স্থির শূন্য hsm_start_ticket_release(কাঠামো sbi_hsm_তথ্য *hdata)
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
পূর্ণসংখ্যা sbi_hsm_hart_interruptible_mask(ধ্রুবক কাঠামো sbi_domain *dom,
				    কাঠামো sbi_hartmask *mask)
{
	পূর্ণসংখ্যা hstate, ret;
	u32 i;

	ret = sbi_domain_get_assigned_hartmask(dom, mask);
	যদি (ret)
		ফেরত ret;

	sbi_hartmask_for_each_hartindex(i, mask) {
		hstate = __sbi_hsm_hart_get_state(i);
		যদি (hstate != SBI_HSM_STATE_STARTED &&
		    hstate != SBI_HSM_STATE_SUSPENDED &&
		    hstate != SBI_HSM_STATE_RESUME_PENDING)
			sbi_hartmask_clear_hartindex(i, mask);
	}

	ফেরত 0;
}

শূন্য __noreturn sbi_hsm_hart_start_finish(কাঠামো sbi_scratch *scratch,
					  u32 hartid)
{
	unsigned long পরবর্তী_arg1;
	unsigned long পরবর্তী_addr;
	unsigned long পরবর্তী_mode;
	কাঠামো sbi_hsm_তথ্য *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_তথ্য_offset);

	যদি (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_START_PENDING,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	পরবর্তী_arg1 = scratch->পরবর্তী_arg1;
	পরবর্তী_addr = scratch->পরবর্তী_addr;
	পরবর্তী_mode = scratch->পরবর্তী_mode;
	hsm_start_ticket_release(hdata);

	sbi_hart_switch_mode(hartid, পরবর্তী_arg1, পরবর্তী_addr, পরবর্তী_mode, false);
}

স্থির শূন্য sbi_hsm_hart_wait(কাঠামো sbi_scratch *scratch)
{
	unsigned long saved_mie;
	কাঠামো sbi_hsm_তথ্য *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_তথ্য_offset);
	/* Save MIE CSR */
	saved_mie = csr_read(CSR_MIE);

	/* Set MSIE and MEIE bits to receive IPI */
	csr_set(CSR_MIE, MIP_MSIP | MIP_MEIP);

	/* Wait for state transition requested by sbi_hsm_hart_start() */
	যখন (atomic_read(&hdata->state) != SBI_HSM_STATE_START_PENDING) {
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

ধ্রুবক কাঠামো sbi_hsm_device *sbi_hsm_get_device(শূন্য)
{
	ফেরত hsm_dev;
}

শূন্য sbi_hsm_set_device(ধ্রুবক কাঠামো sbi_hsm_device *dev)
{
	যদি (!dev || hsm_dev)
		ফেরত;

	hsm_dev = dev;
}

স্থির bool hsm_device_has_hart_hotplug(শূন্য)
{
	যদি (hsm_dev && hsm_dev->hart_start && hsm_dev->hart_stop)
		ফেরত true;
	ফেরত false;
}

স্থির bool hsm_device_has_hart_secondary_boot(শূন্য)
{
	যদি (hsm_dev && hsm_dev->hart_start && !hsm_dev->hart_stop)
		ফেরত true;
	ফেরত false;
}

স্থির পূর্ণসংখ্যা hsm_device_hart_start(u32 hartid, ulong saddr)
{
	যদি (hsm_dev && hsm_dev->hart_start)
		ফেরত hsm_dev->hart_start(hartid, saddr);
	ফেরত SBI_ENOTSUPP;
}

স্থির পূর্ণসংখ্যা hsm_device_hart_stop(শূন্য)
{
	যদি (hsm_dev && hsm_dev->hart_stop)
		ফেরত hsm_dev->hart_stop();
	ফেরত SBI_ENOTSUPP;
}

স্থির পূর্ণসংখ্যা hsm_device_hart_suspend(u32 suspend_type, ulong mmode_resume_addr)
{
	যদি (hsm_dev && hsm_dev->hart_suspend)
		ফেরত hsm_dev->hart_suspend(suspend_type, mmode_resume_addr);
	ফেরত SBI_ENOTSUPP;
}

স্থির শূন্য hsm_device_hart_resume(শূন্য)
{
	যদি (hsm_dev && hsm_dev->hart_resume)
		hsm_dev->hart_resume();
}

পূর্ণসংখ্যা sbi_hsm_init(কাঠামো sbi_scratch *scratch, bool cold_boot)
{
	কাঠামো sbi_scratch *rscratch;
	কাঠামো sbi_hsm_তথ্য *hdata;

	যদি (cold_boot) {
		hart_তথ্য_offset = sbi_scratch_alloc_offset(আকার(*hdata));
		যদি (!hart_তথ্য_offset)
			ফেরত SBI_ENOMEM;

		/* Initialize hart state data for every hart */
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_to_scratch(i);
			যদি (!rscratch)
				চালু;

			hdata = sbi_scratch_offset_ptr(rscratch,
						       hart_তথ্য_offset);
			ATOMIC_INIT(&hdata->state,
				    (i == current_hartindex()) ?
				    SBI_HSM_STATE_START_PENDING :
				    SBI_HSM_STATE_STOPPED);
			ATOMIC_INIT(&hdata->start_ticket, 0);
		}
	} অন্যথা {
		sbi_hsm_hart_wait(scratch);
	}

	ফেরত 0;
}

শূন্য __noreturn sbi_hsm_exit(কাঠামো sbi_scratch *scratch)
{
	কাঠামো sbi_hsm_তথ্য *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_তথ্য_offset);
	শূন্য (*jump_warmboot)(শূন্য) = (শূন্য (*)(শূন্য))scratch->warmboot_addr;

	যদি (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STOP_PENDING,
					 SBI_HSM_STATE_STOPPED))
		যাও fail_exit;

	যদি (hsm_device_has_hart_hotplug()) {
		যদি (hsm_device_hart_stop() != SBI_ENOTSUPP)
			যাও fail_exit;
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

পূর্ণসংখ্যা sbi_hsm_hart_start(কাঠামো sbi_scratch *scratch,
		       ধ্রুবক কাঠামো sbi_domain *dom,
		       u32 hartid, ulong saddr, ulong smode, ulong arg1)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);
	unsigned long init_গুনতি, entry_গুনতি;
	unsigned পূর্ণসংখ্যা hstate;
	কাঠামো sbi_scratch *rscratch;
	কাঠামো sbi_hsm_তথ্য *hdata;
	পূর্ণসংখ্যা rc;

	/* For now, we only allow start mode to be S-mode or U-mode. */
	যদি (smode != PRV_S && smode != PRV_U)
		ফেরত SBI_EINVAL;
	যদি (dom && !sbi_domain_is_assigned_hart(dom, hartindex))
		ফেরত SBI_EINVAL;
	যদি (dom && !sbi_domain_check_addr(dom, saddr, smode,
					  SBI_DOMAIN_EXECUTE))
		ফেরত SBI_EINVALID_ADDR;

	rscratch = sbi_hartindex_to_scratch(hartindex);
	যদি (!rscratch)
		ফেরত SBI_EINVAL;

	hdata = sbi_scratch_offset_ptr(rscratch, hart_তথ্য_offset);
	যদি (!hsm_start_ticket_acquire(hdata))
		ফেরত SBI_EINVAL;

	init_গুনতি = sbi_init_গুনতি(hartindex);
	entry_গুনতি = sbi_entry_গুনতি(hartindex);

	rscratch->পরবর্তী_arg1 = arg1;
	rscratch->পরবর্তী_addr = saddr;
	rscratch->পরবর্তী_mode = smode;

	/*
	 * atomic_cmpxchg() is an implicit barrier. It makes sure that
	 * other harts see reading of init_count and writing to *rscratch
	 * before hdata->state is set to SBI_HSM_STATE_START_PENDING.
	 */
	hstate = atomic_cmpxchg(&hdata->state, SBI_HSM_STATE_STOPPED,
				SBI_HSM_STATE_START_PENDING);
	যদি (hstate == SBI_HSM_STATE_STARTED) {
		rc = SBI_EALREADY;
		যাও err;
	}

	/**
	 * if a hart is already transition to start or stop, another start call
	 * is considered as invalid request.
	 */
	যদি (hstate != SBI_HSM_STATE_STOPPED) {
		rc = SBI_EINVAL;
		যাও err;
	}

	যদি ((hsm_device_has_hart_hotplug() && (entry_গুনতি == init_গুনতি)) ||
	   (hsm_device_has_hart_secondary_boot() && !init_গুনতি)) {
		rc = hsm_device_hart_start(hartid, scratch->warmboot_addr);
	} অন্যথা {
		rc = sbi_ipi_raw_send(hartindex, true);
	}

	যদি (!rc)
		ফেরত 0;

	/* If it fails to start, change hart state back to stop */
	__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_START_PENDING,
				    SBI_HSM_STATE_STOPPED);
err:
	hsm_start_ticket_release(hdata);
	ফেরত rc;
}

পূর্ণসংখ্যা sbi_hsm_hart_stop(কাঠামো sbi_scratch *scratch, bool exitnow)
{
	ধ্রুবক কাঠামো sbi_domain *dom = sbi_domain_thishart_ptr();
	কাঠামো sbi_hsm_তথ্য *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_তথ্য_offset);

	যদি (!dom)
		ফেরত SBI_EFAIL;

	যদি (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_STOP_PENDING))
		ফেরত SBI_EFAIL;

	যদি (exitnow)
		sbi_exit(scratch);

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা __sbi_hsm_suspend_default(কাঠামো sbi_scratch *scratch)
{
	/* Wait for interrupt */
	wfi();

	ফেরত 0;
}

শূন্য __sbi_hsm_suspend_non_ret_save(কাঠামো sbi_scratch *scratch)
{
	কাঠামো sbi_hsm_তথ্য *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_তথ্য_offset);

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

স্থির শূন্য __sbi_hsm_suspend_non_ret_restore(কাঠামো sbi_scratch *scratch)
{
	কাঠামো sbi_hsm_তথ্য *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_তথ্য_offset);

	যদি (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12)
		csr_write64(CSR_MENVCFG, hdata->saved_menvcfg);
	csr_write(CSR_MIDELEG, hdata->saved_mideleg);
	csr_write(CSR_MEDELEG, hdata->saved_medeleg);
	csr_write(CSR_MIE, hdata->saved_mie);
	csr_set(CSR_MIP, (hdata->saved_mip & (MIP_SSIP | MIP_STIP)));
}

শূন্য sbi_hsm_hart_resume_start(কাঠামো sbi_scratch *scratch)
{
	কাঠামো sbi_hsm_তথ্য *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_তথ্য_offset);

	/* If current HART was SUSPENDED then set RESUME_PENDING state */
	যদি (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_RESUME_PENDING))
		sbi_hart_hang();

	যদি (sbi_system_is_suspended())
		sbi_system_resume();
	অন্যথা
		hsm_device_hart_resume();
}

শূন্য __noreturn sbi_hsm_hart_resume_finish(কাঠামো sbi_scratch *scratch,
					   u32 hartid)
{
	কাঠামো sbi_hsm_তথ্য *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_তথ্য_offset);

	/* If current HART was RESUME_PENDING then set STARTED state */
	যদি (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_RESUME_PENDING,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	/*
	 * Restore some of the M-mode CSRs which we are re-configured by
	 * the warm-boot sequence.
	 */
	__sbi_hsm_suspend_non_ret_restore(scratch);

	sbi_hart_switch_mode(hartid, scratch->পরবর্তী_arg1,
			     scratch->পরবর্তী_addr,
			     scratch->পরবর্তী_mode, false);
}

পূর্ণসংখ্যা sbi_hsm_hart_suspend(কাঠামো sbi_scratch *scratch, u32 suspend_type,
			 ulong raddr, ulong rmode, ulong arg1)
{
	পূর্ণসংখ্যা ret;
	ধ্রুবক কাঠামো sbi_domain *dom = sbi_domain_thishart_ptr();
	কাঠামো sbi_hsm_তথ্য *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_তথ্য_offset);

	/* Sanity check on domain assigned to current HART */
	যদি (!dom)
		ফেরত SBI_EFAIL;

	/* Sanity check on suspend type */
	যদি (SBI_HSM_SUSPEND_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_RET_PLATFORM)
		ফেরত SBI_EINVAL;
	যদি (SBI_HSM_SUSPEND_NON_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_NON_RET_PLATFORM)
		ফেরত SBI_EINVAL;

	/* Additional sanity check for non-retentive suspend */
	যদি (suspend_type & SBI_HSM_SUSP_NON_RET_BIT) {
		/*
		 * For now, we only allow non-retentive suspend from
		 * S-mode or U-mode.
		 */
		যদি (rmode != PRV_S && rmode != PRV_U)
			ফেরত SBI_EFAIL;
		যদি (dom && !sbi_domain_check_addr(dom, raddr, rmode,
						  SBI_DOMAIN_EXECUTE))
			ফেরত SBI_EINVALID_ADDR;
	}

	/* Save the resume address and resume mode */
	scratch->পরবর্তী_arg1 = arg1;
	scratch->পরবর্তী_addr = raddr;
	scratch->পরবর্তী_mode = rmode;

	/* Directly move from STARTED to SUSPENDED state */
	যদি (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_SUSPENDED))
		ফেরত SBI_EFAIL;

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

	ফেরত ret;
}
