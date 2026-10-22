/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_barrier.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_encoding.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_atomic.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitops.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hartmask.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hsm.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_init.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ipi.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_system.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_timer.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>

#ವ್ಯಾಖ್ಯಾನಿಸು __sbi_hsm_hart_change_state(hdata, oldstate, newstate)		\
({									\
	ದೀರ್ಘ state = atomic_cmpxchg(&(hdata)->state, oldstate, newstate); \
	ಆದರೆ (state != (oldstate))					\
		sbi_printf("%s: ERR: The hart is in invalid state [%lu]\n", \
			   __func__, state);				\
	state == (oldstate);						\
})

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_hsm_device *hsm_dev = ಖಾಲಿ;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ hart_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ;
ಸ್ಥಿರ bool hsm_device_has_hart_hotplug(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ hsm_device_hart_stop(ಶೂನ್ಯ);

/** Per hart specific data to manage state transition **/
ರಚನೆ sbi_hsm_ದತ್ತಾಂಶ {
	atomic_t state;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ suspend_type;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ saved_mie;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ saved_mip;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ saved_medeleg;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ saved_mideleg;
	u64 saved_menvcfg;
	atomic_t ಆರಂಭ_ticket;
};

bool sbi_hsm_hart_change_state(ರಚನೆ sbi_scratch *scratch, ದೀರ್ಘ oldstate,
			       ದೀರ್ಘ newstate)
{
	ರಚನೆ sbi_hsm_ದತ್ತಾಂಶ *hdata = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch,
							    hart_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ);
	ಹಿಂತಿರುಗಿ __sbi_hsm_hart_change_state(hdata, oldstate, newstate);
}

ಪೂರ್ಣಾಂಕ __sbi_hsm_hart_get_state(u32 hartindex)
{
	ರಚನೆ sbi_hsm_ದತ್ತಾಂಶ *hdata;
	ರಚನೆ sbi_scratch *scratch;

	scratch = sbi_hartindex_ಗಮ್ಯ_scratch(hartindex);
	ಆದರೆ (!scratch)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	hdata = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, hart_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ);
	ಹಿಂತಿರುಗಿ atomic_read(&hdata->state);
}

ಪೂರ್ಣಾಂಕ sbi_hsm_hart_get_state(ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain *dom, u32 hartid)
{
	u32 hartindex = sbi_hartid_ಗಮ್ಯ_hartindex(hartid);

	ಆದರೆ (!sbi_domain_is_assigned_hart(dom, hartindex))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಹಿಂತಿರುಗಿ __sbi_hsm_hart_get_state(hartindex);
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
ಸ್ಥಿರ bool hsm_ಆರಂಭ_ticket_acquire(ರಚನೆ sbi_hsm_ದತ್ತಾಂಶ *hdata)
{
	ಹಿಂತಿರುಗಿ (atomic_cmpxchg(&hdata->ಆರಂಭ_ticket, 0, 1) == 0);
}

/*
 * Release the ticket for the given target hart.
 *
 * The function has "release" semantics: no memory operations preceding it
 * in the current hart can be seen after it by other harts.
 */
ಸ್ಥಿರ ಶೂನ್ಯ hsm_ಆರಂಭ_ticket_release(ರಚನೆ sbi_hsm_ದತ್ತಾಂಶ *hdata)
{
	RISCV_FENCE(rw, w);
	atomic_write(&hdata->ಆರಂಭ_ticket, 0);
}

/**
 * Get the mask of harts which are valid IPI targets
 * @param dom the domain to be used for output HART mask
 * @param mask the output hartmask to fill
 * @return 0 on success and SBI_Exxx (< 0) on failure
 */
ಪೂರ್ಣಾಂಕ sbi_hsm_hart_interruptible_ಮುಖವಾಡ(ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain *dom,
				    ರಚನೆ sbi_hartmask *ಮುಖವಾಡ)
{
	ಪೂರ್ಣಾಂಕ hstate, ret;
	u32 i;

	ret = sbi_domain_get_assigned_hartmask(dom, ಮುಖವಾಡ);
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ ret;

	sbi_hartmask_for_each_hartindex(i, ಮುಖವಾಡ) {
		hstate = __sbi_hsm_hart_get_state(i);
		ಆದರೆ (hstate != SBI_HSM_STATE_STARTED &&
		    hstate != SBI_HSM_STATE_SUSPENDED &&
		    hstate != SBI_HSM_STATE_RESUME_PENDING)
			sbi_hartmask_clear_hartindex(i, ಮುಖವಾಡ);
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ __noreturn sbi_hsm_hart_ಆರಂಭ_ಮುಕ್ತಾಯ(ರಚನೆ sbi_scratch *scratch,
					  u32 hartid)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಂದಿನ_arg1;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಂದಿನ_addr;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಂದಿನ_ಬಹುಲಕ;
	ರಚನೆ sbi_hsm_ದತ್ತಾಂಶ *hdata = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch,
							    hart_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ);

	ಆದರೆ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_ಆರಂಭ_PENDING,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	ಮುಂದಿನ_arg1 = scratch->ಮುಂದಿನ_arg1;
	ಮುಂದಿನ_addr = scratch->ಮುಂದಿನ_addr;
	ಮುಂದಿನ_ಬಹುಲಕ = scratch->ಮುಂದಿನ_ಬಹುಲಕ;
	hsm_ಆರಂಭ_ticket_release(hdata);

	sbi_hart_switch_ಬಹುಲಕ(hartid, ಮುಂದಿನ_arg1, ಮುಂದಿನ_addr, ಮುಂದಿನ_ಬಹುಲಕ, false);
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_hsm_hart_wait(ರಚನೆ sbi_scratch *scratch)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ saved_mie;
	ರಚನೆ sbi_hsm_ದತ್ತಾಂಶ *hdata = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch,
							    hart_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ);
	/* Save MIE CSR */
	saved_mie = csr_read(CSR_MIE);

	/* Set MSIE and MEIE bits to receive IPI */
	csr_ಗಣ(CSR_MIE, MIP_MSIP | MIP_MEIP);

	/* Wait for state transition requested by sbi_hsm_hart_start() */
	ಆಗ (atomic_read(&hdata->state) != SBI_HSM_STATE_ಆರಂಭ_PENDING) {
		/*
		 * If the hsm_dev is ready and it support the hotplug, we can
		 * use the hsm stop for more power saving
		 */
		ಆದರೆ (hsm_device_has_hart_hotplug()) {
			sbi_revert_ನಮೂದು_ಎಣಿಕೆ(scratch);
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

ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_hsm_device *sbi_hsm_get_device(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ hsm_dev;
}

ಶೂನ್ಯ sbi_hsm_ಗಣ_device(ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_hsm_device *dev)
{
	ಆದರೆ (!dev || hsm_dev)
		ಹಿಂತಿರುಗಿ;

	hsm_dev = dev;
}

ಸ್ಥಿರ bool hsm_device_has_hart_hotplug(ಶೂನ್ಯ)
{
	ಆದರೆ (hsm_dev && hsm_dev->hart_ಆರಂಭ && hsm_dev->hart_stop)
		ಹಿಂತಿರುಗಿ true;
	ಹಿಂತಿರುಗಿ false;
}

ಸ್ಥಿರ bool hsm_device_has_hart_secondary_boot(ಶೂನ್ಯ)
{
	ಆದರೆ (hsm_dev && hsm_dev->hart_ಆರಂಭ && !hsm_dev->hart_stop)
		ಹಿಂತಿರುಗಿ true;
	ಹಿಂತಿರುಗಿ false;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ hsm_device_hart_ಆರಂಭ(u32 hartid, ulong saddr)
{
	ಆದರೆ (hsm_dev && hsm_dev->hart_ಆರಂಭ)
		ಹಿಂತಿರುಗಿ hsm_dev->hart_ಆರಂಭ(hartid, saddr);
	ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ hsm_device_hart_stop(ಶೂನ್ಯ)
{
	ಆದರೆ (hsm_dev && hsm_dev->hart_stop)
		ಹಿಂತಿರುಗಿ hsm_dev->hart_stop();
	ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ hsm_device_hart_suspend(u32 suspend_type, ulong mmode_resume_addr)
{
	ಆದರೆ (hsm_dev && hsm_dev->hart_suspend)
		ಹಿಂತಿರುಗಿ hsm_dev->hart_suspend(suspend_type, mmode_resume_addr);
	ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;
}

ಸ್ಥಿರ ಶೂನ್ಯ hsm_device_hart_resume(ಶೂನ್ಯ)
{
	ಆದರೆ (hsm_dev && hsm_dev->hart_resume)
		hsm_dev->hart_resume();
}

ಪೂರ್ಣಾಂಕ sbi_hsm_init(ರಚನೆ sbi_scratch *scratch, bool cold_boot)
{
	ರಚನೆ sbi_scratch *rscratch;
	ರಚನೆ sbi_hsm_ದತ್ತಾಂಶ *hdata;

	ಆದರೆ (cold_boot) {
		hart_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ = sbi_scratch_alloc_ಸ್ಥಾನಾಂತರ(ಗಾತ್ರ(*hdata));
		ಆದರೆ (!hart_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;

		/* Initialize hart state data for every hart */
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_ಗಮ್ಯ_scratch(i);
			ಆದರೆ (!rscratch)
				ಮುಂದುವರಿ;

			hdata = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(rscratch,
						       hart_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ);
			ATOMIC_INIT(&hdata->state,
				    (i == current_hartindex()) ?
				    SBI_HSM_STATE_ಆರಂಭ_PENDING :
				    SBI_HSM_STATE_STOPPED);
			ATOMIC_INIT(&hdata->ಆರಂಭ_ticket, 0);
		}
	} ಇಲ್ಲದಿದ್ದರೆ {
		sbi_hsm_hart_wait(scratch);
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ __noreturn sbi_hsm_exit(ರಚನೆ sbi_scratch *scratch)
{
	ರಚನೆ sbi_hsm_ದತ್ತಾಂಶ *hdata = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch,
							    hart_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ);
	ಶೂನ್ಯ (*jump_warmboot)(ಶೂನ್ಯ) = (ಶೂನ್ಯ (*)(ಶೂನ್ಯ))scratch->warmboot_addr;

	ಆದರೆ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STOP_PENDING,
					 SBI_HSM_STATE_STOPPED))
		ಹೋಗು fail_exit;

	ಆದರೆ (hsm_device_has_hart_hotplug()) {
		ಆದರೆ (hsm_device_hart_stop() != SBI_ENOTSUPP)
			ಹೋಗು fail_exit;
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

ಪೂರ್ಣಾಂಕ sbi_hsm_hart_ಆರಂಭ(ರಚನೆ sbi_scratch *scratch,
		       ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain *dom,
		       u32 hartid, ulong saddr, ulong smode, ulong arg1)
{
	u32 hartindex = sbi_hartid_ಗಮ್ಯ_hartindex(hartid);
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ init_ಎಣಿಕೆ, ನಮೂದು_ಎಣಿಕೆ;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ hstate;
	ರಚನೆ sbi_scratch *rscratch;
	ರಚನೆ sbi_hsm_ದತ್ತಾಂಶ *hdata;
	ಪೂರ್ಣಾಂಕ rc;

	/* For now, we only allow start mode to be S-mode or U-mode. */
	ಆದರೆ (smode != PRV_S && smode != PRV_U)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಆದರೆ (dom && !sbi_domain_is_assigned_hart(dom, hartindex))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಆದರೆ (dom && !sbi_domain_check_addr(dom, saddr, smode,
					  SBI_DOMAIN_EXECUTE))
		ಹಿಂತಿರುಗಿ SBI_EINVALID_ADDR;

	rscratch = sbi_hartindex_ಗಮ್ಯ_scratch(hartindex);
	ಆದರೆ (!rscratch)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	hdata = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(rscratch, hart_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ);
	ಆದರೆ (!hsm_ಆರಂಭ_ticket_acquire(hdata))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	init_ಎಣಿಕೆ = sbi_init_ಎಣಿಕೆ(hartindex);
	ನಮೂದು_ಎಣಿಕೆ = sbi_ನಮೂದು_ಎಣಿಕೆ(hartindex);

	rscratch->ಮುಂದಿನ_arg1 = arg1;
	rscratch->ಮುಂದಿನ_addr = saddr;
	rscratch->ಮುಂದಿನ_ಬಹುಲಕ = smode;

	/*
	 * atomic_cmpxchg() is an implicit barrier. It makes sure that
	 * other harts see reading of init_count and writing to *rscratch
	 * before hdata->state is set to SBI_HSM_STATE_START_PENDING.
	 */
	hstate = atomic_cmpxchg(&hdata->state, SBI_HSM_STATE_STOPPED,
				SBI_HSM_STATE_ಆರಂಭ_PENDING);
	ಆದರೆ (hstate == SBI_HSM_STATE_STARTED) {
		rc = SBI_EALREADY;
		ಹೋಗು err;
	}

	/**
	 * if a hart is already transition to start or stop, another start call
	 * is considered as invalid request.
	 */
	ಆದರೆ (hstate != SBI_HSM_STATE_STOPPED) {
		rc = SBI_EINVAL;
		ಹೋಗು err;
	}

	ಆದರೆ ((hsm_device_has_hart_hotplug() && (ನಮೂದು_ಎಣಿಕೆ == init_ಎಣಿಕೆ)) ||
	   (hsm_device_has_hart_secondary_boot() && !init_ಎಣಿಕೆ)) {
		rc = hsm_device_hart_ಆರಂಭ(hartid, scratch->warmboot_addr);
	} ಇಲ್ಲದಿದ್ದರೆ {
		rc = sbi_ipi_raw_send(hartindex, true);
	}

	ಆದರೆ (!rc)
		ಹಿಂತಿರುಗಿ 0;

	/* If it fails to start, change hart state back to stop */
	__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_ಆರಂಭ_PENDING,
				    SBI_HSM_STATE_STOPPED);
err:
	hsm_ಆರಂಭ_ticket_release(hdata);
	ಹಿಂತಿರುಗಿ rc;
}

ಪೂರ್ಣಾಂಕ sbi_hsm_hart_stop(ರಚನೆ sbi_scratch *scratch, bool exitnow)
{
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain *dom = sbi_domain_thishart_ptr();
	ರಚನೆ sbi_hsm_ದತ್ತಾಂಶ *hdata = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch,
							    hart_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ);

	ಆದರೆ (!dom)
		ಹಿಂತಿರುಗಿ SBI_EFAIL;

	ಆದರೆ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_STOP_PENDING))
		ಹಿಂತಿರುಗಿ SBI_EFAIL;

	ಆದರೆ (exitnow)
		sbi_exit(scratch);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ __sbi_hsm_suspend_default(ರಚನೆ sbi_scratch *scratch)
{
	/* Wait for interrupt */
	wfi();

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ __sbi_hsm_suspend_non_ret_save(ರಚನೆ sbi_scratch *scratch)
{
	ರಚನೆ sbi_hsm_ದತ್ತಾಂಶ *hdata = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch,
							    hart_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ);

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
	ಆದರೆ (sbi_hart_priv_ಆವೃತ್ತಿ(scratch) >= SBI_HART_PRIV_VER_1_12)
		hdata->saved_menvcfg = csr_read64(CSR_MENVCFG);
}

ಸ್ಥಿರ ಶೂನ್ಯ __sbi_hsm_suspend_non_ret_restore(ರಚನೆ sbi_scratch *scratch)
{
	ರಚನೆ sbi_hsm_ದತ್ತಾಂಶ *hdata = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch,
							    hart_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ);

	ಆದರೆ (sbi_hart_priv_ಆವೃತ್ತಿ(scratch) >= SBI_HART_PRIV_VER_1_12)
		csr_write64(CSR_MENVCFG, hdata->saved_menvcfg);
	csr_write(CSR_MIDELEG, hdata->saved_mideleg);
	csr_write(CSR_MEDELEG, hdata->saved_medeleg);
	csr_write(CSR_MIE, hdata->saved_mie);
	csr_ಗಣ(CSR_MIP, (hdata->saved_mip & (MIP_SSIP | MIP_STIP)));
}

ಶೂನ್ಯ sbi_hsm_hart_resume_ಆರಂಭ(ರಚನೆ sbi_scratch *scratch)
{
	ರಚನೆ sbi_hsm_ದತ್ತಾಂಶ *hdata = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch,
							    hart_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ);

	/* If current HART was SUSPENDED then set RESUME_PENDING state */
	ಆದರೆ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_RESUME_PENDING))
		sbi_hart_hang();

	ಆದರೆ (sbi_system_is_suspended())
		sbi_system_resume();
	ಇಲ್ಲದಿದ್ದರೆ
		hsm_device_hart_resume();
}

ಶೂನ್ಯ __noreturn sbi_hsm_hart_resume_ಮುಕ್ತಾಯ(ರಚನೆ sbi_scratch *scratch,
					   u32 hartid)
{
	ರಚನೆ sbi_hsm_ದತ್ತಾಂಶ *hdata = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch,
							    hart_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ);

	/* If current HART was RESUME_PENDING then set STARTED state */
	ಆದರೆ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_RESUME_PENDING,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	/*
	 * Restore some of the M-mode CSRs which we are re-configured by
	 * the warm-boot sequence.
	 */
	__sbi_hsm_suspend_non_ret_restore(scratch);

	sbi_hart_switch_ಬಹುಲಕ(hartid, scratch->ಮುಂದಿನ_arg1,
			     scratch->ಮುಂದಿನ_addr,
			     scratch->ಮುಂದಿನ_ಬಹುಲಕ, false);
}

ಪೂರ್ಣಾಂಕ sbi_hsm_hart_suspend(ರಚನೆ sbi_scratch *scratch, u32 suspend_type,
			 ulong raddr, ulong rmode, ulong arg1)
{
	ಪೂರ್ಣಾಂಕ ret;
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain *dom = sbi_domain_thishart_ptr();
	ರಚನೆ sbi_hsm_ದತ್ತಾಂಶ *hdata = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch,
							    hart_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ);

	/* Sanity check on domain assigned to current HART */
	ಆದರೆ (!dom)
		ಹಿಂತಿರುಗಿ SBI_EFAIL;

	/* Sanity check on suspend type */
	ಆದರೆ (SBI_HSM_SUSPEND_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_RET_PLATFORM)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಆದರೆ (SBI_HSM_SUSPEND_NON_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_NON_RET_PLATFORM)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/* Additional sanity check for non-retentive suspend */
	ಆದರೆ (suspend_type & SBI_HSM_SUSP_NON_RET_ಬಿಟ್) {
		/*
		 * For now, we only allow non-retentive suspend from
		 * S-mode or U-mode.
		 */
		ಆದರೆ (rmode != PRV_S && rmode != PRV_U)
			ಹಿಂತಿರುಗಿ SBI_EFAIL;
		ಆದರೆ (dom && !sbi_domain_check_addr(dom, raddr, rmode,
						  SBI_DOMAIN_EXECUTE))
			ಹಿಂತಿರುಗಿ SBI_EINVALID_ADDR;
	}

	/* Save the resume address and resume mode */
	scratch->ಮುಂದಿನ_arg1 = arg1;
	scratch->ಮುಂದಿನ_addr = raddr;
	scratch->ಮುಂದಿನ_ಬಹುಲಕ = rmode;

	/* Directly move from STARTED to SUSPENDED state */
	ಆದರೆ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_SUSPENDED))
		ಹಿಂತಿರುಗಿ SBI_EFAIL;

	/* Save the suspend type */
	hdata->suspend_type = suspend_type;

	/*
	 * Save context which will be restored after resuming from
	 * non-retentive suspend.
	 */
	ಆದರೆ (suspend_type & SBI_HSM_SUSP_NON_RET_ಬಿಟ್)
		__sbi_hsm_suspend_non_ret_save(scratch);

	/* Try platform specific suspend */
	ret = hsm_device_hart_suspend(suspend_type, scratch->warmboot_addr);
	ಆದರೆ (ret == SBI_ENOTSUPP) {
		/* Try generic implementation of default suspend types */
		ಆದರೆ (suspend_type == SBI_HSM_SUSPEND_RET_DEFAULT ||
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
	ಆದರೆ (ret == 0 && (suspend_type & SBI_HSM_SUSP_NON_RET_ಬಿಟ್)) {
		ಶೂನ್ಯ (*jump_warmboot)(ಶೂನ್ಯ) =
			(ಶೂನ್ಯ (*)(ಶೂನ್ಯ))scratch->warmboot_addr;

		jump_warmboot();
	}

	/*
	 * We might have successfully resumed from retentive suspend
	 * or suspend failed. In both cases, we restore state of hart.
	 */
	ಆದರೆ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	ಹಿಂತಿರುಗಿ ret;
}
