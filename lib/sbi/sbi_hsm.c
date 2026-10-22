/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_barrier.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_encoding.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_atomic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitops.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hartmask.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hsm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_init.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ipi.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_system.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __sbi_hsm_hart_change_state(hdata, oldstate, newstate)		\
({									\
	ਲੰਮਾ state = atomic_cmpxchg(&(hdata)->state, oldstate, newstate); \
	ਜੇ (state != (oldstate))					\
		sbi_printf("%s: ERR: The hart is in invalid state [%lu]\n", \
			   __func__, state);				\
	state == (oldstate);						\
})

ਸਥਿਰ ਅਚਲ ਬਣਤਰ sbi_hsm_device *hsm_dev = ਨਿਰਾ;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ hart_ਅੰਕੜੇ_ਵਿੱਥ;
ਸਥਿਰ bool hsm_device_has_hart_hotplug(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ hsm_device_hart_stop(ਖਾਲੀ);

/** Per hart specific data to manage state transition **/
ਬਣਤਰ sbi_hsm_ਅੰਕੜੇ {
	atomic_t state;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ suspend_type;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ saved_mie;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ saved_mip;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ saved_medeleg;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ saved_mideleg;
	u64 saved_menvcfg;
	atomic_t ਸ਼ੁਰੂ_ticket;
};

bool sbi_hsm_hart_change_state(ਬਣਤਰ sbi_scratch *scratch, ਲੰਮਾ oldstate,
			       ਲੰਮਾ newstate)
{
	ਬਣਤਰ sbi_hsm_ਅੰਕੜੇ *hdata = sbi_scratch_ਵਿੱਥ_ptr(scratch,
							    hart_ਅੰਕੜੇ_ਵਿੱਥ);
	ਵਾਪਸ __sbi_hsm_hart_change_state(hdata, oldstate, newstate);
}

ਪੂਰਨ __sbi_hsm_hart_get_state(u32 hartindex)
{
	ਬਣਤਰ sbi_hsm_ਅੰਕੜੇ *hdata;
	ਬਣਤਰ sbi_scratch *scratch;

	scratch = sbi_hartindex_ਮੰਜ਼ਿਲ_scratch(hartindex);
	ਜੇ (!scratch)
		ਵਾਪਸ SBI_EINVAL;

	hdata = sbi_scratch_ਵਿੱਥ_ptr(scratch, hart_ਅੰਕੜੇ_ਵਿੱਥ);
	ਵਾਪਸ atomic_read(&hdata->state);
}

ਪੂਰਨ sbi_hsm_hart_get_state(ਅਚਲ ਬਣਤਰ sbi_domain *dom, u32 hartid)
{
	u32 hartindex = sbi_hartid_ਮੰਜ਼ਿਲ_hartindex(hartid);

	ਜੇ (!sbi_domain_is_assigned_hart(dom, hartindex))
		ਵਾਪਸ SBI_EINVAL;

	ਵਾਪਸ __sbi_hsm_hart_get_state(hartindex);
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
ਸਥਿਰ bool hsm_ਸ਼ੁਰੂ_ticket_acquire(ਬਣਤਰ sbi_hsm_ਅੰਕੜੇ *hdata)
{
	ਵਾਪਸ (atomic_cmpxchg(&hdata->ਸ਼ੁਰੂ_ticket, 0, 1) == 0);
}

/*
 * Release the ticket for the given target hart.
 *
 * The function has "release" semantics: no memory operations preceding it
 * in the current hart can be seen after it by other harts.
 */
ਸਥਿਰ ਖਾਲੀ hsm_ਸ਼ੁਰੂ_ticket_release(ਬਣਤਰ sbi_hsm_ਅੰਕੜੇ *hdata)
{
	RISCV_FENCE(rw, w);
	atomic_write(&hdata->ਸ਼ੁਰੂ_ticket, 0);
}

/**
 * Get the mask of harts which are valid IPI targets
 * @param dom the domain to be used for output HART mask
 * @param mask the output hartmask to fill
 * @return 0 on success and SBI_Exxx (< 0) on failure
 */
ਪੂਰਨ sbi_hsm_hart_interruptible_ਨਕਾਬ(ਅਚਲ ਬਣਤਰ sbi_domain *dom,
				    ਬਣਤਰ sbi_hartmask *ਨਕਾਬ)
{
	ਪੂਰਨ hstate, ret;
	u32 i;

	ret = sbi_domain_get_assigned_hartmask(dom, ਨਕਾਬ);
	ਜੇ (ret)
		ਵਾਪਸ ret;

	sbi_hartmask_for_each_hartindex(i, ਨਕਾਬ) {
		hstate = __sbi_hsm_hart_get_state(i);
		ਜੇ (hstate != SBI_HSM_STATE_STARTED &&
		    hstate != SBI_HSM_STATE_SUSPENDED &&
		    hstate != SBI_HSM_STATE_RESUME_PENDING)
			sbi_hartmask_clear_hartindex(i, ਨਕਾਬ);
	}

	ਵਾਪਸ 0;
}

ਖਾਲੀ __noreturn sbi_hsm_hart_ਸ਼ੁਰੂ_ਅੰਤ(ਬਣਤਰ sbi_scratch *scratch,
					  u32 hartid)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅਗਲਾ_arg1;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅਗਲਾ_addr;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅਗਲਾ_ਬਹੁਲਕ;
	ਬਣਤਰ sbi_hsm_ਅੰਕੜੇ *hdata = sbi_scratch_ਵਿੱਥ_ptr(scratch,
							    hart_ਅੰਕੜੇ_ਵਿੱਥ);

	ਜੇ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_ਸ਼ੁਰੂ_PENDING,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	ਅਗਲਾ_arg1 = scratch->ਅਗਲਾ_arg1;
	ਅਗਲਾ_addr = scratch->ਅਗਲਾ_addr;
	ਅਗਲਾ_ਬਹੁਲਕ = scratch->ਅਗਲਾ_ਬਹੁਲਕ;
	hsm_ਸ਼ੁਰੂ_ticket_release(hdata);

	sbi_hart_switch_ਬਹੁਲਕ(hartid, ਅਗਲਾ_arg1, ਅਗਲਾ_addr, ਅਗਲਾ_ਬਹੁਲਕ, false);
}

ਸਥਿਰ ਖਾਲੀ sbi_hsm_hart_wait(ਬਣਤਰ sbi_scratch *scratch)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ saved_mie;
	ਬਣਤਰ sbi_hsm_ਅੰਕੜੇ *hdata = sbi_scratch_ਵਿੱਥ_ptr(scratch,
							    hart_ਅੰਕੜੇ_ਵਿੱਥ);
	/* Save MIE CSR */
	saved_mie = csr_read(CSR_MIE);

	/* Set MSIE and MEIE bits to receive IPI */
	csr_ਸੈੱਟ(CSR_MIE, MIP_MSIP | MIP_MEIP);

	/* Wait for state transition requested by sbi_hsm_hart_start() */
	ਜਦੋਂ (atomic_read(&hdata->state) != SBI_HSM_STATE_ਸ਼ੁਰੂ_PENDING) {
		/*
		 * If the hsm_dev is ready and it support the hotplug, we can
		 * use the hsm stop for more power saving
		 */
		ਜੇ (hsm_device_has_hart_hotplug()) {
			sbi_revert_ਇੰਦਰਾਜ_ਗਿਣਤੀ(scratch);
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

ਅਚਲ ਬਣਤਰ sbi_hsm_device *sbi_hsm_get_device(ਖਾਲੀ)
{
	ਵਾਪਸ hsm_dev;
}

ਖਾਲੀ sbi_hsm_ਸੈੱਟ_device(ਅਚਲ ਬਣਤਰ sbi_hsm_device *dev)
{
	ਜੇ (!dev || hsm_dev)
		ਵਾਪਸ;

	hsm_dev = dev;
}

ਸਥਿਰ bool hsm_device_has_hart_hotplug(ਖਾਲੀ)
{
	ਜੇ (hsm_dev && hsm_dev->hart_ਸ਼ੁਰੂ && hsm_dev->hart_stop)
		ਵਾਪਸ true;
	ਵਾਪਸ false;
}

ਸਥਿਰ bool hsm_device_has_hart_secondary_boot(ਖਾਲੀ)
{
	ਜੇ (hsm_dev && hsm_dev->hart_ਸ਼ੁਰੂ && !hsm_dev->hart_stop)
		ਵਾਪਸ true;
	ਵਾਪਸ false;
}

ਸਥਿਰ ਪੂਰਨ hsm_device_hart_ਸ਼ੁਰੂ(u32 hartid, ulong saddr)
{
	ਜੇ (hsm_dev && hsm_dev->hart_ਸ਼ੁਰੂ)
		ਵਾਪਸ hsm_dev->hart_ਸ਼ੁਰੂ(hartid, saddr);
	ਵਾਪਸ SBI_ENOTSUPP;
}

ਸਥਿਰ ਪੂਰਨ hsm_device_hart_stop(ਖਾਲੀ)
{
	ਜੇ (hsm_dev && hsm_dev->hart_stop)
		ਵਾਪਸ hsm_dev->hart_stop();
	ਵਾਪਸ SBI_ENOTSUPP;
}

ਸਥਿਰ ਪੂਰਨ hsm_device_hart_suspend(u32 suspend_type, ulong mmode_resume_addr)
{
	ਜੇ (hsm_dev && hsm_dev->hart_suspend)
		ਵਾਪਸ hsm_dev->hart_suspend(suspend_type, mmode_resume_addr);
	ਵਾਪਸ SBI_ENOTSUPP;
}

ਸਥਿਰ ਖਾਲੀ hsm_device_hart_resume(ਖਾਲੀ)
{
	ਜੇ (hsm_dev && hsm_dev->hart_resume)
		hsm_dev->hart_resume();
}

ਪੂਰਨ sbi_hsm_init(ਬਣਤਰ sbi_scratch *scratch, bool cold_boot)
{
	ਬਣਤਰ sbi_scratch *rscratch;
	ਬਣਤਰ sbi_hsm_ਅੰਕੜੇ *hdata;

	ਜੇ (cold_boot) {
		hart_ਅੰਕੜੇ_ਵਿੱਥ = sbi_scratch_alloc_ਵਿੱਥ(ਆਕਾਰ(*hdata));
		ਜੇ (!hart_ਅੰਕੜੇ_ਵਿੱਥ)
			ਵਾਪਸ SBI_ENOMEM;

		/* Initialize hart state data for every hart */
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_ਮੰਜ਼ਿਲ_scratch(i);
			ਜੇ (!rscratch)
				ਜਾਰੀ;

			hdata = sbi_scratch_ਵਿੱਥ_ptr(rscratch,
						       hart_ਅੰਕੜੇ_ਵਿੱਥ);
			ATOMIC_INIT(&hdata->state,
				    (i == current_hartindex()) ?
				    SBI_HSM_STATE_ਸ਼ੁਰੂ_PENDING :
				    SBI_HSM_STATE_STOPPED);
			ATOMIC_INIT(&hdata->ਸ਼ੁਰੂ_ticket, 0);
		}
	} ਨਹੀਂਤਾਂ {
		sbi_hsm_hart_wait(scratch);
	}

	ਵਾਪਸ 0;
}

ਖਾਲੀ __noreturn sbi_hsm_exit(ਬਣਤਰ sbi_scratch *scratch)
{
	ਬਣਤਰ sbi_hsm_ਅੰਕੜੇ *hdata = sbi_scratch_ਵਿੱਥ_ptr(scratch,
							    hart_ਅੰਕੜੇ_ਵਿੱਥ);
	ਖਾਲੀ (*jump_warmboot)(ਖਾਲੀ) = (ਖਾਲੀ (*)(ਖਾਲੀ))scratch->warmboot_addr;

	ਜੇ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STOP_PENDING,
					 SBI_HSM_STATE_STOPPED))
		ਜਾਓ fail_exit;

	ਜੇ (hsm_device_has_hart_hotplug()) {
		ਜੇ (hsm_device_hart_stop() != SBI_ENOTSUPP)
			ਜਾਓ fail_exit;
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

ਪੂਰਨ sbi_hsm_hart_ਸ਼ੁਰੂ(ਬਣਤਰ sbi_scratch *scratch,
		       ਅਚਲ ਬਣਤਰ sbi_domain *dom,
		       u32 hartid, ulong saddr, ulong smode, ulong arg1)
{
	u32 hartindex = sbi_hartid_ਮੰਜ਼ਿਲ_hartindex(hartid);
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ init_ਗਿਣਤੀ, ਇੰਦਰਾਜ_ਗਿਣਤੀ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ hstate;
	ਬਣਤਰ sbi_scratch *rscratch;
	ਬਣਤਰ sbi_hsm_ਅੰਕੜੇ *hdata;
	ਪੂਰਨ rc;

	/* For now, we only allow start mode to be S-mode or U-mode. */
	ਜੇ (smode != PRV_S && smode != PRV_U)
		ਵਾਪਸ SBI_EINVAL;
	ਜੇ (dom && !sbi_domain_is_assigned_hart(dom, hartindex))
		ਵਾਪਸ SBI_EINVAL;
	ਜੇ (dom && !sbi_domain_check_addr(dom, saddr, smode,
					  SBI_DOMAIN_EXECUTE))
		ਵਾਪਸ SBI_EINVALID_ADDR;

	rscratch = sbi_hartindex_ਮੰਜ਼ਿਲ_scratch(hartindex);
	ਜੇ (!rscratch)
		ਵਾਪਸ SBI_EINVAL;

	hdata = sbi_scratch_ਵਿੱਥ_ptr(rscratch, hart_ਅੰਕੜੇ_ਵਿੱਥ);
	ਜੇ (!hsm_ਸ਼ੁਰੂ_ticket_acquire(hdata))
		ਵਾਪਸ SBI_EINVAL;

	init_ਗਿਣਤੀ = sbi_init_ਗਿਣਤੀ(hartindex);
	ਇੰਦਰਾਜ_ਗਿਣਤੀ = sbi_ਇੰਦਰਾਜ_ਗਿਣਤੀ(hartindex);

	rscratch->ਅਗਲਾ_arg1 = arg1;
	rscratch->ਅਗਲਾ_addr = saddr;
	rscratch->ਅਗਲਾ_ਬਹੁਲਕ = smode;

	/*
	 * atomic_cmpxchg() is an implicit barrier. It makes sure that
	 * other harts see reading of init_count and writing to *rscratch
	 * before hdata->state is set to SBI_HSM_STATE_START_PENDING.
	 */
	hstate = atomic_cmpxchg(&hdata->state, SBI_HSM_STATE_STOPPED,
				SBI_HSM_STATE_ਸ਼ੁਰੂ_PENDING);
	ਜੇ (hstate == SBI_HSM_STATE_STARTED) {
		rc = SBI_EALREADY;
		ਜਾਓ err;
	}

	/**
	 * if a hart is already transition to start or stop, another start call
	 * is considered as invalid request.
	 */
	ਜੇ (hstate != SBI_HSM_STATE_STOPPED) {
		rc = SBI_EINVAL;
		ਜਾਓ err;
	}

	ਜੇ ((hsm_device_has_hart_hotplug() && (ਇੰਦਰਾਜ_ਗਿਣਤੀ == init_ਗਿਣਤੀ)) ||
	   (hsm_device_has_hart_secondary_boot() && !init_ਗਿਣਤੀ)) {
		rc = hsm_device_hart_ਸ਼ੁਰੂ(hartid, scratch->warmboot_addr);
	} ਨਹੀਂਤਾਂ {
		rc = sbi_ipi_raw_send(hartindex, true);
	}

	ਜੇ (!rc)
		ਵਾਪਸ 0;

	/* If it fails to start, change hart state back to stop */
	__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_ਸ਼ੁਰੂ_PENDING,
				    SBI_HSM_STATE_STOPPED);
err:
	hsm_ਸ਼ੁਰੂ_ticket_release(hdata);
	ਵਾਪਸ rc;
}

ਪੂਰਨ sbi_hsm_hart_stop(ਬਣਤਰ sbi_scratch *scratch, bool exitnow)
{
	ਅਚਲ ਬਣਤਰ sbi_domain *dom = sbi_domain_thishart_ptr();
	ਬਣਤਰ sbi_hsm_ਅੰਕੜੇ *hdata = sbi_scratch_ਵਿੱਥ_ptr(scratch,
							    hart_ਅੰਕੜੇ_ਵਿੱਥ);

	ਜੇ (!dom)
		ਵਾਪਸ SBI_EFAIL;

	ਜੇ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_STOP_PENDING))
		ਵਾਪਸ SBI_EFAIL;

	ਜੇ (exitnow)
		sbi_exit(scratch);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ __sbi_hsm_suspend_default(ਬਣਤਰ sbi_scratch *scratch)
{
	/* Wait for interrupt */
	wfi();

	ਵਾਪਸ 0;
}

ਖਾਲੀ __sbi_hsm_suspend_non_ret_save(ਬਣਤਰ sbi_scratch *scratch)
{
	ਬਣਤਰ sbi_hsm_ਅੰਕੜੇ *hdata = sbi_scratch_ਵਿੱਥ_ptr(scratch,
							    hart_ਅੰਕੜੇ_ਵਿੱਥ);

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
	ਜੇ (sbi_hart_priv_ਸੰਸਕਰਣ(scratch) >= SBI_HART_PRIV_VER_1_12)
		hdata->saved_menvcfg = csr_read64(CSR_MENVCFG);
}

ਸਥਿਰ ਖਾਲੀ __sbi_hsm_suspend_non_ret_restore(ਬਣਤਰ sbi_scratch *scratch)
{
	ਬਣਤਰ sbi_hsm_ਅੰਕੜੇ *hdata = sbi_scratch_ਵਿੱਥ_ptr(scratch,
							    hart_ਅੰਕੜੇ_ਵਿੱਥ);

	ਜੇ (sbi_hart_priv_ਸੰਸਕਰਣ(scratch) >= SBI_HART_PRIV_VER_1_12)
		csr_write64(CSR_MENVCFG, hdata->saved_menvcfg);
	csr_write(CSR_MIDELEG, hdata->saved_mideleg);
	csr_write(CSR_MEDELEG, hdata->saved_medeleg);
	csr_write(CSR_MIE, hdata->saved_mie);
	csr_ਸੈੱਟ(CSR_MIP, (hdata->saved_mip & (MIP_SSIP | MIP_STIP)));
}

ਖਾਲੀ sbi_hsm_hart_resume_ਸ਼ੁਰੂ(ਬਣਤਰ sbi_scratch *scratch)
{
	ਬਣਤਰ sbi_hsm_ਅੰਕੜੇ *hdata = sbi_scratch_ਵਿੱਥ_ptr(scratch,
							    hart_ਅੰਕੜੇ_ਵਿੱਥ);

	/* If current HART was SUSPENDED then set RESUME_PENDING state */
	ਜੇ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_RESUME_PENDING))
		sbi_hart_hang();

	ਜੇ (sbi_system_is_suspended())
		sbi_system_resume();
	ਨਹੀਂਤਾਂ
		hsm_device_hart_resume();
}

ਖਾਲੀ __noreturn sbi_hsm_hart_resume_ਅੰਤ(ਬਣਤਰ sbi_scratch *scratch,
					   u32 hartid)
{
	ਬਣਤਰ sbi_hsm_ਅੰਕੜੇ *hdata = sbi_scratch_ਵਿੱਥ_ptr(scratch,
							    hart_ਅੰਕੜੇ_ਵਿੱਥ);

	/* If current HART was RESUME_PENDING then set STARTED state */
	ਜੇ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_RESUME_PENDING,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	/*
	 * Restore some of the M-mode CSRs which we are re-configured by
	 * the warm-boot sequence.
	 */
	__sbi_hsm_suspend_non_ret_restore(scratch);

	sbi_hart_switch_ਬਹੁਲਕ(hartid, scratch->ਅਗਲਾ_arg1,
			     scratch->ਅਗਲਾ_addr,
			     scratch->ਅਗਲਾ_ਬਹੁਲਕ, false);
}

ਪੂਰਨ sbi_hsm_hart_suspend(ਬਣਤਰ sbi_scratch *scratch, u32 suspend_type,
			 ulong raddr, ulong rmode, ulong arg1)
{
	ਪੂਰਨ ret;
	ਅਚਲ ਬਣਤਰ sbi_domain *dom = sbi_domain_thishart_ptr();
	ਬਣਤਰ sbi_hsm_ਅੰਕੜੇ *hdata = sbi_scratch_ਵਿੱਥ_ptr(scratch,
							    hart_ਅੰਕੜੇ_ਵਿੱਥ);

	/* Sanity check on domain assigned to current HART */
	ਜੇ (!dom)
		ਵਾਪਸ SBI_EFAIL;

	/* Sanity check on suspend type */
	ਜੇ (SBI_HSM_SUSPEND_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_RET_PLATFORM)
		ਵਾਪਸ SBI_EINVAL;
	ਜੇ (SBI_HSM_SUSPEND_NON_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_NON_RET_PLATFORM)
		ਵਾਪਸ SBI_EINVAL;

	/* Additional sanity check for non-retentive suspend */
	ਜੇ (suspend_type & SBI_HSM_SUSP_NON_RET_ਦੋਆਧਾਰੀ_ਅੰਕ) {
		/*
		 * For now, we only allow non-retentive suspend from
		 * S-mode or U-mode.
		 */
		ਜੇ (rmode != PRV_S && rmode != PRV_U)
			ਵਾਪਸ SBI_EFAIL;
		ਜੇ (dom && !sbi_domain_check_addr(dom, raddr, rmode,
						  SBI_DOMAIN_EXECUTE))
			ਵਾਪਸ SBI_EINVALID_ADDR;
	}

	/* Save the resume address and resume mode */
	scratch->ਅਗਲਾ_arg1 = arg1;
	scratch->ਅਗਲਾ_addr = raddr;
	scratch->ਅਗਲਾ_ਬਹੁਲਕ = rmode;

	/* Directly move from STARTED to SUSPENDED state */
	ਜੇ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_SUSPENDED))
		ਵਾਪਸ SBI_EFAIL;

	/* Save the suspend type */
	hdata->suspend_type = suspend_type;

	/*
	 * Save context which will be restored after resuming from
	 * non-retentive suspend.
	 */
	ਜੇ (suspend_type & SBI_HSM_SUSP_NON_RET_ਦੋਆਧਾਰੀ_ਅੰਕ)
		__sbi_hsm_suspend_non_ret_save(scratch);

	/* Try platform specific suspend */
	ret = hsm_device_hart_suspend(suspend_type, scratch->warmboot_addr);
	ਜੇ (ret == SBI_ENOTSUPP) {
		/* Try generic implementation of default suspend types */
		ਜੇ (suspend_type == SBI_HSM_SUSPEND_RET_DEFAULT ||
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
	ਜੇ (ret == 0 && (suspend_type & SBI_HSM_SUSP_NON_RET_ਦੋਆਧਾਰੀ_ਅੰਕ)) {
		ਖਾਲੀ (*jump_warmboot)(ਖਾਲੀ) =
			(ਖਾਲੀ (*)(ਖਾਲੀ))scratch->warmboot_addr;

		jump_warmboot();
	}

	/*
	 * We might have successfully resumed from retentive suspend
	 * or suspend failed. In both cases, we restore state of hart.
	 */
	ਜੇ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	ਵਾਪਸ ret;
}
