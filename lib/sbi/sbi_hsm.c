/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_barrier.h>
#ضمن <sbi/riscv_encoding.h>
#ضمن <sbi/riscv_atomic.h>
#ضمن <sbi/sbi_bitops.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_hartmask.h>
#ضمن <sbi/sbi_hsm.h>
#ضمن <sbi/sbi_init.h>
#ضمن <sbi/sbi_ipi.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi/sbi_system.h>
#ضمن <sbi/sbi_timer.h>
#ضمن <sbi/sbi_console.h>

#تعریف_کن __sbi_hsm_hart_change_state(hdata, oldstate, newstate)		\
({									\
	دراز state = atomic_cmpxchg(&(hdata)->state, oldstate, newstate); \
	إذا (state != (oldstate))					\
		sbi_printf("%s: ERR: The hart is in invalid state [%lu]\n", \
			   __func__, state);				\
	state == (oldstate);						\
})

ساكن ثابت بنية sbi_hsm_device *hsm_dev = عدم;
ساكن بیعلامت دراز hart_داده_جابه_جایی;
ساكن bool hsm_device_has_hart_hotplug(فراغ);
ساكن صحيح hsm_device_hart_stop(فراغ);

/** Per hart specific data to manage state transition **/
بنية sbi_hsm_داده {
	atomic_t state;
	بیعلامت دراز suspend_type;
	بیعلامت دراز saved_mie;
	بیعلامت دراز saved_mip;
	بیعلامت دراز saved_medeleg;
	بیعلامت دراز saved_mideleg;
	u64 saved_menvcfg;
	atomic_t شروع_ticket;
};

bool sbi_hsm_hart_change_state(بنية sbi_scratch *scratch, دراز oldstate,
			       دراز newstate)
{
	بنية sbi_hsm_داده *hdata = sbi_scratch_جابه_جایی_ptr(scratch,
							    hart_داده_جابه_جایی);
	أرجع __sbi_hsm_hart_change_state(hdata, oldstate, newstate);
}

صحيح __sbi_hsm_hart_get_state(u32 hartindex)
{
	بنية sbi_hsm_داده *hdata;
	بنية sbi_scratch *scratch;

	scratch = sbi_hartindex_مقصد_scratch(hartindex);
	إذا (!scratch)
		أرجع SBI_EINVAL;

	hdata = sbi_scratch_جابه_جایی_ptr(scratch, hart_داده_جابه_جایی);
	أرجع atomic_read(&hdata->state);
}

صحيح sbi_hsm_hart_get_state(ثابت بنية sbi_domain *dom, u32 hartid)
{
	u32 hartindex = sbi_hartid_مقصد_hartindex(hartid);

	إذا (!sbi_domain_is_assigned_hart(dom, hartindex))
		أرجع SBI_EINVAL;

	أرجع __sbi_hsm_hart_get_state(hartindex);
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
ساكن bool hsm_شروع_ticket_acquire(بنية sbi_hsm_داده *hdata)
{
	أرجع (atomic_cmpxchg(&hdata->شروع_ticket, 0, 1) == 0);
}

/*
 * Release the ticket for the given target hart.
 *
 * The function has "release" semantics: no memory operations preceding it
 * in the current hart can be seen after it by other harts.
 */
ساكن فراغ hsm_شروع_ticket_release(بنية sbi_hsm_داده *hdata)
{
	RISCV_FENCE(rw, w);
	atomic_write(&hdata->شروع_ticket, 0);
}

/**
 * Get the mask of harts which are valid IPI targets
 * @param dom the domain to be used for output HART mask
 * @param mask the output hartmask to fill
 * @return 0 on success and SBI_Exxx (< 0) on failure
 */
صحيح sbi_hsm_hart_interruptible_ماسک(ثابت بنية sbi_domain *dom,
				    بنية sbi_hartmask *ماسک)
{
	صحيح hstate, ret;
	u32 i;

	ret = sbi_domain_get_assigned_hartmask(dom, ماسک);
	إذا (ret)
		أرجع ret;

	sbi_hartmask_for_each_hartindex(i, ماسک) {
		hstate = __sbi_hsm_hart_get_state(i);
		إذا (hstate != SBI_HSM_STATE_STARTED &&
		    hstate != SBI_HSM_STATE_SUSPENDED &&
		    hstate != SBI_HSM_STATE_RESUME_PENDING)
			sbi_hartmask_clear_hartindex(i, ماسک);
	}

	أرجع 0;
}

فراغ __noreturn sbi_hsm_hart_شروع_پایان(بنية sbi_scratch *scratch,
					  u32 hartid)
{
	بیعلامت دراز بعدی_arg1;
	بیعلامت دراز بعدی_addr;
	بیعلامت دراز بعدی_نما;
	بنية sbi_hsm_داده *hdata = sbi_scratch_جابه_جایی_ptr(scratch,
							    hart_داده_جابه_جایی);

	إذا (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_شروع_PENDING,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	بعدی_arg1 = scratch->بعدی_arg1;
	بعدی_addr = scratch->بعدی_addr;
	بعدی_نما = scratch->بعدی_نما;
	hsm_شروع_ticket_release(hdata);

	sbi_hart_switch_نما(hartid, بعدی_arg1, بعدی_addr, بعدی_نما, false);
}

ساكن فراغ sbi_hsm_hart_wait(بنية sbi_scratch *scratch)
{
	بیعلامت دراز saved_mie;
	بنية sbi_hsm_داده *hdata = sbi_scratch_جابه_جایی_ptr(scratch,
							    hart_داده_جابه_جایی);
	/* Save MIE CSR */
	saved_mie = csr_read(CSR_MIE);

	/* Set MSIE and MEIE bits to receive IPI */
	csr_مجموعه(CSR_MIE, MIP_MSIP | MIP_MEIP);

	/* Wait for state transition requested by sbi_hsm_hart_start() */
	طالما (atomic_read(&hdata->state) != SBI_HSM_STATE_شروع_PENDING) {
		/*
		 * If the hsm_dev is ready and it support the hotplug, we can
		 * use the hsm stop for more power saving
		 */
		إذا (hsm_device_has_hart_hotplug()) {
			sbi_revert_مدخل_شمار(scratch);
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

ثابت بنية sbi_hsm_device *sbi_hsm_get_device(فراغ)
{
	أرجع hsm_dev;
}

فراغ sbi_hsm_مجموعه_device(ثابت بنية sbi_hsm_device *dev)
{
	إذا (!dev || hsm_dev)
		أرجع;

	hsm_dev = dev;
}

ساكن bool hsm_device_has_hart_hotplug(فراغ)
{
	إذا (hsm_dev && hsm_dev->hart_شروع && hsm_dev->hart_stop)
		أرجع true;
	أرجع false;
}

ساكن bool hsm_device_has_hart_secondary_boot(فراغ)
{
	إذا (hsm_dev && hsm_dev->hart_شروع && !hsm_dev->hart_stop)
		أرجع true;
	أرجع false;
}

ساكن صحيح hsm_device_hart_شروع(u32 hartid, ulong saddr)
{
	إذا (hsm_dev && hsm_dev->hart_شروع)
		أرجع hsm_dev->hart_شروع(hartid, saddr);
	أرجع SBI_ENOTSUPP;
}

ساكن صحيح hsm_device_hart_stop(فراغ)
{
	إذا (hsm_dev && hsm_dev->hart_stop)
		أرجع hsm_dev->hart_stop();
	أرجع SBI_ENOTSUPP;
}

ساكن صحيح hsm_device_hart_suspend(u32 suspend_type, ulong mmode_resume_addr)
{
	إذا (hsm_dev && hsm_dev->hart_suspend)
		أرجع hsm_dev->hart_suspend(suspend_type, mmode_resume_addr);
	أرجع SBI_ENOTSUPP;
}

ساكن فراغ hsm_device_hart_resume(فراغ)
{
	إذا (hsm_dev && hsm_dev->hart_resume)
		hsm_dev->hart_resume();
}

صحيح sbi_hsm_init(بنية sbi_scratch *scratch, bool cold_boot)
{
	بنية sbi_scratch *rscratch;
	بنية sbi_hsm_داده *hdata;

	إذا (cold_boot) {
		hart_داده_جابه_جایی = sbi_scratch_alloc_جابه_جایی(حجم(*hdata));
		إذا (!hart_داده_جابه_جایی)
			أرجع SBI_ENOMEM;

		/* Initialize hart state data for every hart */
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_مقصد_scratch(i);
			إذا (!rscratch)
				استمر;

			hdata = sbi_scratch_جابه_جایی_ptr(rscratch,
						       hart_داده_جابه_جایی);
			ATOMIC_INIT(&hdata->state,
				    (i == current_hartindex()) ?
				    SBI_HSM_STATE_شروع_PENDING :
				    SBI_HSM_STATE_STOPPED);
			ATOMIC_INIT(&hdata->شروع_ticket, 0);
		}
	} وإلا {
		sbi_hsm_hart_wait(scratch);
	}

	أرجع 0;
}

فراغ __noreturn sbi_hsm_exit(بنية sbi_scratch *scratch)
{
	بنية sbi_hsm_داده *hdata = sbi_scratch_جابه_جایی_ptr(scratch,
							    hart_داده_جابه_جایی);
	فراغ (*jump_warmboot)(فراغ) = (فراغ (*)(فراغ))scratch->warmboot_addr;

	إذا (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STOP_PENDING,
					 SBI_HSM_STATE_STOPPED))
		اذهب fail_exit;

	إذا (hsm_device_has_hart_hotplug()) {
		إذا (hsm_device_hart_stop() != SBI_ENOTSUPP)
			اذهب fail_exit;
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

صحيح sbi_hsm_hart_شروع(بنية sbi_scratch *scratch,
		       ثابت بنية sbi_domain *dom,
		       u32 hartid, ulong saddr, ulong smode, ulong arg1)
{
	u32 hartindex = sbi_hartid_مقصد_hartindex(hartid);
	بیعلامت دراز init_شمار, مدخل_شمار;
	بیعلامت صحيح hstate;
	بنية sbi_scratch *rscratch;
	بنية sbi_hsm_داده *hdata;
	صحيح rc;

	/* For now, we only allow start mode to be S-mode or U-mode. */
	إذا (smode != PRV_S && smode != PRV_U)
		أرجع SBI_EINVAL;
	إذا (dom && !sbi_domain_is_assigned_hart(dom, hartindex))
		أرجع SBI_EINVAL;
	إذا (dom && !sbi_domain_check_addr(dom, saddr, smode,
					  SBI_DOMAIN_EXECUTE))
		أرجع SBI_EINVALID_ADDR;

	rscratch = sbi_hartindex_مقصد_scratch(hartindex);
	إذا (!rscratch)
		أرجع SBI_EINVAL;

	hdata = sbi_scratch_جابه_جایی_ptr(rscratch, hart_داده_جابه_جایی);
	إذا (!hsm_شروع_ticket_acquire(hdata))
		أرجع SBI_EINVAL;

	init_شمار = sbi_init_شمار(hartindex);
	مدخل_شمار = sbi_مدخل_شمار(hartindex);

	rscratch->بعدی_arg1 = arg1;
	rscratch->بعدی_addr = saddr;
	rscratch->بعدی_نما = smode;

	/*
	 * atomic_cmpxchg() is an implicit barrier. It makes sure that
	 * other harts see reading of init_count and writing to *rscratch
	 * before hdata->state is set to SBI_HSM_STATE_START_PENDING.
	 */
	hstate = atomic_cmpxchg(&hdata->state, SBI_HSM_STATE_STOPPED,
				SBI_HSM_STATE_شروع_PENDING);
	إذا (hstate == SBI_HSM_STATE_STARTED) {
		rc = SBI_EALREADY;
		اذهب err;
	}

	/**
	 * if a hart is already transition to start or stop, another start call
	 * is considered as invalid request.
	 */
	إذا (hstate != SBI_HSM_STATE_STOPPED) {
		rc = SBI_EINVAL;
		اذهب err;
	}

	إذا ((hsm_device_has_hart_hotplug() && (مدخل_شمار == init_شمار)) ||
	   (hsm_device_has_hart_secondary_boot() && !init_شمار)) {
		rc = hsm_device_hart_شروع(hartid, scratch->warmboot_addr);
	} وإلا {
		rc = sbi_ipi_raw_send(hartindex, true);
	}

	إذا (!rc)
		أرجع 0;

	/* If it fails to start, change hart state back to stop */
	__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_شروع_PENDING,
				    SBI_HSM_STATE_STOPPED);
err:
	hsm_شروع_ticket_release(hdata);
	أرجع rc;
}

صحيح sbi_hsm_hart_stop(بنية sbi_scratch *scratch, bool exitnow)
{
	ثابت بنية sbi_domain *dom = sbi_domain_thishart_ptr();
	بنية sbi_hsm_داده *hdata = sbi_scratch_جابه_جایی_ptr(scratch,
							    hart_داده_جابه_جایی);

	إذا (!dom)
		أرجع SBI_EFAIL;

	إذا (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_STOP_PENDING))
		أرجع SBI_EFAIL;

	إذا (exitnow)
		sbi_exit(scratch);

	أرجع 0;
}

ساكن صحيح __sbi_hsm_suspend_default(بنية sbi_scratch *scratch)
{
	/* Wait for interrupt */
	wfi();

	أرجع 0;
}

فراغ __sbi_hsm_suspend_non_ret_save(بنية sbi_scratch *scratch)
{
	بنية sbi_hsm_داده *hdata = sbi_scratch_جابه_جایی_ptr(scratch,
							    hart_داده_جابه_جایی);

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
	إذا (sbi_hart_priv_نسخه(scratch) >= SBI_HART_PRIV_VER_1_12)
		hdata->saved_menvcfg = csr_read64(CSR_MENVCFG);
}

ساكن فراغ __sbi_hsm_suspend_non_ret_restore(بنية sbi_scratch *scratch)
{
	بنية sbi_hsm_داده *hdata = sbi_scratch_جابه_جایی_ptr(scratch,
							    hart_داده_جابه_جایی);

	إذا (sbi_hart_priv_نسخه(scratch) >= SBI_HART_PRIV_VER_1_12)
		csr_write64(CSR_MENVCFG, hdata->saved_menvcfg);
	csr_write(CSR_MIDELEG, hdata->saved_mideleg);
	csr_write(CSR_MEDELEG, hdata->saved_medeleg);
	csr_write(CSR_MIE, hdata->saved_mie);
	csr_مجموعه(CSR_MIP, (hdata->saved_mip & (MIP_SSIP | MIP_STIP)));
}

فراغ sbi_hsm_hart_resume_شروع(بنية sbi_scratch *scratch)
{
	بنية sbi_hsm_داده *hdata = sbi_scratch_جابه_جایی_ptr(scratch,
							    hart_داده_جابه_جایی);

	/* If current HART was SUSPENDED then set RESUME_PENDING state */
	إذا (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_RESUME_PENDING))
		sbi_hart_hang();

	إذا (sbi_system_is_suspended())
		sbi_system_resume();
	وإلا
		hsm_device_hart_resume();
}

فراغ __noreturn sbi_hsm_hart_resume_پایان(بنية sbi_scratch *scratch,
					   u32 hartid)
{
	بنية sbi_hsm_داده *hdata = sbi_scratch_جابه_جایی_ptr(scratch,
							    hart_داده_جابه_جایی);

	/* If current HART was RESUME_PENDING then set STARTED state */
	إذا (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_RESUME_PENDING,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	/*
	 * Restore some of the M-mode CSRs which we are re-configured by
	 * the warm-boot sequence.
	 */
	__sbi_hsm_suspend_non_ret_restore(scratch);

	sbi_hart_switch_نما(hartid, scratch->بعدی_arg1,
			     scratch->بعدی_addr,
			     scratch->بعدی_نما, false);
}

صحيح sbi_hsm_hart_suspend(بنية sbi_scratch *scratch, u32 suspend_type,
			 ulong raddr, ulong rmode, ulong arg1)
{
	صحيح ret;
	ثابت بنية sbi_domain *dom = sbi_domain_thishart_ptr();
	بنية sbi_hsm_داده *hdata = sbi_scratch_جابه_جایی_ptr(scratch,
							    hart_داده_جابه_جایی);

	/* Sanity check on domain assigned to current HART */
	إذا (!dom)
		أرجع SBI_EFAIL;

	/* Sanity check on suspend type */
	إذا (SBI_HSM_SUSPEND_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_RET_PLATFORM)
		أرجع SBI_EINVAL;
	إذا (SBI_HSM_SUSPEND_NON_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_NON_RET_PLATFORM)
		أرجع SBI_EINVAL;

	/* Additional sanity check for non-retentive suspend */
	إذا (suspend_type & SBI_HSM_SUSP_NON_RET_بیت) {
		/*
		 * For now, we only allow non-retentive suspend from
		 * S-mode or U-mode.
		 */
		إذا (rmode != PRV_S && rmode != PRV_U)
			أرجع SBI_EFAIL;
		إذا (dom && !sbi_domain_check_addr(dom, raddr, rmode,
						  SBI_DOMAIN_EXECUTE))
			أرجع SBI_EINVALID_ADDR;
	}

	/* Save the resume address and resume mode */
	scratch->بعدی_arg1 = arg1;
	scratch->بعدی_addr = raddr;
	scratch->بعدی_نما = rmode;

	/* Directly move from STARTED to SUSPENDED state */
	إذا (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_SUSPENDED))
		أرجع SBI_EFAIL;

	/* Save the suspend type */
	hdata->suspend_type = suspend_type;

	/*
	 * Save context which will be restored after resuming from
	 * non-retentive suspend.
	 */
	إذا (suspend_type & SBI_HSM_SUSP_NON_RET_بیت)
		__sbi_hsm_suspend_non_ret_save(scratch);

	/* Try platform specific suspend */
	ret = hsm_device_hart_suspend(suspend_type, scratch->warmboot_addr);
	إذا (ret == SBI_ENOTSUPP) {
		/* Try generic implementation of default suspend types */
		إذا (suspend_type == SBI_HSM_SUSPEND_RET_DEFAULT ||
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
	إذا (ret == 0 && (suspend_type & SBI_HSM_SUSP_NON_RET_بیت)) {
		فراغ (*jump_warmboot)(فراغ) =
			(فراغ (*)(فراغ))scratch->warmboot_addr;

		jump_warmboot();
	}

	/*
	 * We might have successfully resumed from retentive suspend
	 * or suspend failed. In both cases, we restore state of hart.
	 */
	إذا (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	أرجع ret;
}
