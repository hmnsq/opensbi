/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_barrier.h>
#شامل <sbi/riscv_encoding.h>
#شامل <sbi/riscv_atomic.h>
#شامل <sbi/sbi_bitops.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_hartmask.h>
#شامل <sbi/sbi_hsm.h>
#شامل <sbi/sbi_init.h>
#شامل <sbi/sbi_ipi.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_system.h>
#شامل <sbi/sbi_timer.h>
#شامل <sbi/sbi_console.h>

#تعریف_کرو __sbi_hsm_hart_change_state(hdata, oldstate, newstate)		\
({									\
	طویل state = atomic_cmpxchg(&(hdata)->state, oldstate, newstate); \
	اگر (state != (oldstate))					\
		sbi_printf("%s: ERR: The hart is in invalid state [%lu]\n", \
			   __func__, state);				\
	state == (oldstate);						\
})

ساکن ثابت ساخت sbi_hsm_device *hsm_dev = تہی;
ساکن غیرعلامتی طویل hart_ڈیٹا_انحراف;
ساکن bool hsm_device_has_hart_hotplug(خالی);
ساکن صحیح hsm_device_hart_stop(خالی);

/** Per hart specific data to manage state transition **/
ساخت sbi_hsm_ڈیٹا {
	atomic_t state;
	غیرعلامتی طویل suspend_type;
	غیرعلامتی طویل saved_mie;
	غیرعلامتی طویل saved_mip;
	غیرعلامتی طویل saved_medeleg;
	غیرعلامتی طویل saved_mideleg;
	u64 saved_menvcfg;
	atomic_t آغاز_ticket;
};

bool sbi_hsm_hart_change_state(ساخت sbi_scratch *scratch, طویل oldstate,
			       طویل newstate)
{
	ساخت sbi_hsm_ڈیٹا *hdata = sbi_scratch_انحراف_ptr(scratch,
							    hart_ڈیٹا_انحراف);
	واپس __sbi_hsm_hart_change_state(hdata, oldstate, newstate);
}

صحیح __sbi_hsm_hart_get_state(u32 hartindex)
{
	ساخت sbi_hsm_ڈیٹا *hdata;
	ساخت sbi_scratch *scratch;

	scratch = sbi_hartindex_مقصد_scratch(hartindex);
	اگر (!scratch)
		واپس SBI_EINVAL;

	hdata = sbi_scratch_انحراف_ptr(scratch, hart_ڈیٹا_انحراف);
	واپس atomic_read(&hdata->state);
}

صحیح sbi_hsm_hart_get_state(ثابت ساخت sbi_domain *dom, u32 hartid)
{
	u32 hartindex = sbi_hartid_مقصد_hartindex(hartid);

	اگر (!sbi_domain_is_assigned_hart(dom, hartindex))
		واپس SBI_EINVAL;

	واپس __sbi_hsm_hart_get_state(hartindex);
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
ساکن bool hsm_آغاز_ticket_acquire(ساخت sbi_hsm_ڈیٹا *hdata)
{
	واپس (atomic_cmpxchg(&hdata->آغاز_ticket, 0, 1) == 0);
}

/*
 * Release the ticket for the given target hart.
 *
 * The function has "release" semantics: no memory operations preceding it
 * in the current hart can be seen after it by other harts.
 */
ساکن خالی hsm_آغاز_ticket_release(ساخت sbi_hsm_ڈیٹا *hdata)
{
	RISCV_FENCE(rw, w);
	atomic_write(&hdata->آغاز_ticket, 0);
}

/**
 * Get the mask of harts which are valid IPI targets
 * @param dom the domain to be used for output HART mask
 * @param mask the output hartmask to fill
 * @return 0 on success and SBI_Exxx (< 0) on failure
 */
صحیح sbi_hsm_hart_interruptible_نقاب(ثابت ساخت sbi_domain *dom,
				    ساخت sbi_hartmask *نقاب)
{
	صحیح hstate, ret;
	u32 i;

	ret = sbi_domain_get_assigned_hartmask(dom, نقاب);
	اگر (ret)
		واپس ret;

	sbi_hartmask_for_each_hartindex(i, نقاب) {
		hstate = __sbi_hsm_hart_get_state(i);
		اگر (hstate != SBI_HSM_STATE_STARTED &&
		    hstate != SBI_HSM_STATE_SUSPENDED &&
		    hstate != SBI_HSM_STATE_RESUME_PENDING)
			sbi_hartmask_clear_hartindex(i, نقاب);
	}

	واپس 0;
}

خالی __noreturn sbi_hsm_hart_آغاز_اختتام(ساخت sbi_scratch *scratch,
					  u32 hartid)
{
	غیرعلامتی طویل اگلا_arg1;
	غیرعلامتی طویل اگلا_addr;
	غیرعلامتی طویل اگلا_منوال;
	ساخت sbi_hsm_ڈیٹا *hdata = sbi_scratch_انحراف_ptr(scratch,
							    hart_ڈیٹا_انحراف);

	اگر (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_آغاز_PENDING,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	اگلا_arg1 = scratch->اگلا_arg1;
	اگلا_addr = scratch->اگلا_addr;
	اگلا_منوال = scratch->اگلا_منوال;
	hsm_آغاز_ticket_release(hdata);

	sbi_hart_switch_منوال(hartid, اگلا_arg1, اگلا_addr, اگلا_منوال, false);
}

ساکن خالی sbi_hsm_hart_wait(ساخت sbi_scratch *scratch)
{
	غیرعلامتی طویل saved_mie;
	ساخت sbi_hsm_ڈیٹا *hdata = sbi_scratch_انحراف_ptr(scratch,
							    hart_ڈیٹا_انحراف);
	/* Save MIE CSR */
	saved_mie = csr_read(CSR_MIE);

	/* Set MSIE and MEIE bits to receive IPI */
	csr_سیٹ(CSR_MIE, MIP_MSIP | MIP_MEIP);

	/* Wait for state transition requested by sbi_hsm_hart_start() */
	جب (atomic_read(&hdata->state) != SBI_HSM_STATE_آغاز_PENDING) {
		/*
		 * If the hsm_dev is ready and it support the hotplug, we can
		 * use the hsm stop for more power saving
		 */
		اگر (hsm_device_has_hart_hotplug()) {
			sbi_revert_اندراج_گنتی(scratch);
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

ثابت ساخت sbi_hsm_device *sbi_hsm_get_device(خالی)
{
	واپس hsm_dev;
}

خالی sbi_hsm_سیٹ_device(ثابت ساخت sbi_hsm_device *dev)
{
	اگر (!dev || hsm_dev)
		واپس;

	hsm_dev = dev;
}

ساکن bool hsm_device_has_hart_hotplug(خالی)
{
	اگر (hsm_dev && hsm_dev->hart_آغاز && hsm_dev->hart_stop)
		واپس true;
	واپس false;
}

ساکن bool hsm_device_has_hart_secondary_boot(خالی)
{
	اگر (hsm_dev && hsm_dev->hart_آغاز && !hsm_dev->hart_stop)
		واپس true;
	واپس false;
}

ساکن صحیح hsm_device_hart_آغاز(u32 hartid, ulong saddr)
{
	اگر (hsm_dev && hsm_dev->hart_آغاز)
		واپس hsm_dev->hart_آغاز(hartid, saddr);
	واپس SBI_ENOTSUPP;
}

ساکن صحیح hsm_device_hart_stop(خالی)
{
	اگر (hsm_dev && hsm_dev->hart_stop)
		واپس hsm_dev->hart_stop();
	واپس SBI_ENOTSUPP;
}

ساکن صحیح hsm_device_hart_suspend(u32 suspend_type, ulong mmode_resume_addr)
{
	اگر (hsm_dev && hsm_dev->hart_suspend)
		واپس hsm_dev->hart_suspend(suspend_type, mmode_resume_addr);
	واپس SBI_ENOTSUPP;
}

ساکن خالی hsm_device_hart_resume(خالی)
{
	اگر (hsm_dev && hsm_dev->hart_resume)
		hsm_dev->hart_resume();
}

صحیح sbi_hsm_init(ساخت sbi_scratch *scratch, bool cold_boot)
{
	ساخت sbi_scratch *rscratch;
	ساخت sbi_hsm_ڈیٹا *hdata;

	اگر (cold_boot) {
		hart_ڈیٹا_انحراف = sbi_scratch_alloc_انحراف(حجم(*hdata));
		اگر (!hart_ڈیٹا_انحراف)
			واپس SBI_ENOMEM;

		/* Initialize hart state data for every hart */
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_مقصد_scratch(i);
			اگر (!rscratch)
				جاری;

			hdata = sbi_scratch_انحراف_ptr(rscratch,
						       hart_ڈیٹا_انحراف);
			ATOMIC_INIT(&hdata->state,
				    (i == current_hartindex()) ?
				    SBI_HSM_STATE_آغاز_PENDING :
				    SBI_HSM_STATE_STOPPED);
			ATOMIC_INIT(&hdata->آغاز_ticket, 0);
		}
	} ورنہ {
		sbi_hsm_hart_wait(scratch);
	}

	واپس 0;
}

خالی __noreturn sbi_hsm_exit(ساخت sbi_scratch *scratch)
{
	ساخت sbi_hsm_ڈیٹا *hdata = sbi_scratch_انحراف_ptr(scratch,
							    hart_ڈیٹا_انحراف);
	خالی (*jump_warmboot)(خالی) = (خالی (*)(خالی))scratch->warmboot_addr;

	اگر (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STOP_PENDING,
					 SBI_HSM_STATE_STOPPED))
		جاؤ fail_exit;

	اگر (hsm_device_has_hart_hotplug()) {
		اگر (hsm_device_hart_stop() != SBI_ENOTSUPP)
			جاؤ fail_exit;
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

صحیح sbi_hsm_hart_آغاز(ساخت sbi_scratch *scratch,
		       ثابت ساخت sbi_domain *dom,
		       u32 hartid, ulong saddr, ulong smode, ulong arg1)
{
	u32 hartindex = sbi_hartid_مقصد_hartindex(hartid);
	غیرعلامتی طویل init_گنتی, اندراج_گنتی;
	غیرعلامتی صحیح hstate;
	ساخت sbi_scratch *rscratch;
	ساخت sbi_hsm_ڈیٹا *hdata;
	صحیح rc;

	/* For now, we only allow start mode to be S-mode or U-mode. */
	اگر (smode != PRV_S && smode != PRV_U)
		واپس SBI_EINVAL;
	اگر (dom && !sbi_domain_is_assigned_hart(dom, hartindex))
		واپس SBI_EINVAL;
	اگر (dom && !sbi_domain_check_addr(dom, saddr, smode,
					  SBI_DOMAIN_EXECUTE))
		واپس SBI_EINVALID_ADDR;

	rscratch = sbi_hartindex_مقصد_scratch(hartindex);
	اگر (!rscratch)
		واپس SBI_EINVAL;

	hdata = sbi_scratch_انحراف_ptr(rscratch, hart_ڈیٹا_انحراف);
	اگر (!hsm_آغاز_ticket_acquire(hdata))
		واپس SBI_EINVAL;

	init_گنتی = sbi_init_گنتی(hartindex);
	اندراج_گنتی = sbi_اندراج_گنتی(hartindex);

	rscratch->اگلا_arg1 = arg1;
	rscratch->اگلا_addr = saddr;
	rscratch->اگلا_منوال = smode;

	/*
	 * atomic_cmpxchg() is an implicit barrier. It makes sure that
	 * other harts see reading of init_count and writing to *rscratch
	 * before hdata->state is set to SBI_HSM_STATE_START_PENDING.
	 */
	hstate = atomic_cmpxchg(&hdata->state, SBI_HSM_STATE_STOPPED,
				SBI_HSM_STATE_آغاز_PENDING);
	اگر (hstate == SBI_HSM_STATE_STARTED) {
		rc = SBI_EALREADY;
		جاؤ err;
	}

	/**
	 * if a hart is already transition to start or stop, another start call
	 * is considered as invalid request.
	 */
	اگر (hstate != SBI_HSM_STATE_STOPPED) {
		rc = SBI_EINVAL;
		جاؤ err;
	}

	اگر ((hsm_device_has_hart_hotplug() && (اندراج_گنتی == init_گنتی)) ||
	   (hsm_device_has_hart_secondary_boot() && !init_گنتی)) {
		rc = hsm_device_hart_آغاز(hartid, scratch->warmboot_addr);
	} ورنہ {
		rc = sbi_ipi_raw_send(hartindex, true);
	}

	اگر (!rc)
		واپس 0;

	/* If it fails to start, change hart state back to stop */
	__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_آغاز_PENDING,
				    SBI_HSM_STATE_STOPPED);
err:
	hsm_آغاز_ticket_release(hdata);
	واپس rc;
}

صحیح sbi_hsm_hart_stop(ساخت sbi_scratch *scratch, bool exitnow)
{
	ثابت ساخت sbi_domain *dom = sbi_domain_thishart_ptr();
	ساخت sbi_hsm_ڈیٹا *hdata = sbi_scratch_انحراف_ptr(scratch,
							    hart_ڈیٹا_انحراف);

	اگر (!dom)
		واپس SBI_EFAIL;

	اگر (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_STOP_PENDING))
		واپس SBI_EFAIL;

	اگر (exitnow)
		sbi_exit(scratch);

	واپس 0;
}

ساکن صحیح __sbi_hsm_suspend_default(ساخت sbi_scratch *scratch)
{
	/* Wait for interrupt */
	wfi();

	واپس 0;
}

خالی __sbi_hsm_suspend_non_ret_save(ساخت sbi_scratch *scratch)
{
	ساخت sbi_hsm_ڈیٹا *hdata = sbi_scratch_انحراف_ptr(scratch,
							    hart_ڈیٹا_انحراف);

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
	اگر (sbi_hart_priv_نسخہ(scratch) >= SBI_HART_PRIV_VER_1_12)
		hdata->saved_menvcfg = csr_read64(CSR_MENVCFG);
}

ساکن خالی __sbi_hsm_suspend_non_ret_restore(ساخت sbi_scratch *scratch)
{
	ساخت sbi_hsm_ڈیٹا *hdata = sbi_scratch_انحراف_ptr(scratch,
							    hart_ڈیٹا_انحراف);

	اگر (sbi_hart_priv_نسخہ(scratch) >= SBI_HART_PRIV_VER_1_12)
		csr_write64(CSR_MENVCFG, hdata->saved_menvcfg);
	csr_write(CSR_MIDELEG, hdata->saved_mideleg);
	csr_write(CSR_MEDELEG, hdata->saved_medeleg);
	csr_write(CSR_MIE, hdata->saved_mie);
	csr_سیٹ(CSR_MIP, (hdata->saved_mip & (MIP_SSIP | MIP_STIP)));
}

خالی sbi_hsm_hart_resume_آغاز(ساخت sbi_scratch *scratch)
{
	ساخت sbi_hsm_ڈیٹا *hdata = sbi_scratch_انحراف_ptr(scratch,
							    hart_ڈیٹا_انحراف);

	/* If current HART was SUSPENDED then set RESUME_PENDING state */
	اگر (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_RESUME_PENDING))
		sbi_hart_hang();

	اگر (sbi_system_is_suspended())
		sbi_system_resume();
	ورنہ
		hsm_device_hart_resume();
}

خالی __noreturn sbi_hsm_hart_resume_اختتام(ساخت sbi_scratch *scratch,
					   u32 hartid)
{
	ساخت sbi_hsm_ڈیٹا *hdata = sbi_scratch_انحراف_ptr(scratch,
							    hart_ڈیٹا_انحراف);

	/* If current HART was RESUME_PENDING then set STARTED state */
	اگر (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_RESUME_PENDING,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	/*
	 * Restore some of the M-mode CSRs which we are re-configured by
	 * the warm-boot sequence.
	 */
	__sbi_hsm_suspend_non_ret_restore(scratch);

	sbi_hart_switch_منوال(hartid, scratch->اگلا_arg1,
			     scratch->اگلا_addr,
			     scratch->اگلا_منوال, false);
}

صحیح sbi_hsm_hart_suspend(ساخت sbi_scratch *scratch, u32 suspend_type,
			 ulong raddr, ulong rmode, ulong arg1)
{
	صحیح ret;
	ثابت ساخت sbi_domain *dom = sbi_domain_thishart_ptr();
	ساخت sbi_hsm_ڈیٹا *hdata = sbi_scratch_انحراف_ptr(scratch,
							    hart_ڈیٹا_انحراف);

	/* Sanity check on domain assigned to current HART */
	اگر (!dom)
		واپس SBI_EFAIL;

	/* Sanity check on suspend type */
	اگر (SBI_HSM_SUSPEND_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_RET_PLATFORM)
		واپس SBI_EINVAL;
	اگر (SBI_HSM_SUSPEND_NON_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_NON_RET_PLATFORM)
		واپس SBI_EINVAL;

	/* Additional sanity check for non-retentive suspend */
	اگر (suspend_type & SBI_HSM_SUSP_NON_RET_ثنائی_ہندسہ) {
		/*
		 * For now, we only allow non-retentive suspend from
		 * S-mode or U-mode.
		 */
		اگر (rmode != PRV_S && rmode != PRV_U)
			واپس SBI_EFAIL;
		اگر (dom && !sbi_domain_check_addr(dom, raddr, rmode,
						  SBI_DOMAIN_EXECUTE))
			واپس SBI_EINVALID_ADDR;
	}

	/* Save the resume address and resume mode */
	scratch->اگلا_arg1 = arg1;
	scratch->اگلا_addr = raddr;
	scratch->اگلا_منوال = rmode;

	/* Directly move from STARTED to SUSPENDED state */
	اگر (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_SUSPENDED))
		واپس SBI_EFAIL;

	/* Save the suspend type */
	hdata->suspend_type = suspend_type;

	/*
	 * Save context which will be restored after resuming from
	 * non-retentive suspend.
	 */
	اگر (suspend_type & SBI_HSM_SUSP_NON_RET_ثنائی_ہندسہ)
		__sbi_hsm_suspend_non_ret_save(scratch);

	/* Try platform specific suspend */
	ret = hsm_device_hart_suspend(suspend_type, scratch->warmboot_addr);
	اگر (ret == SBI_ENOTSUPP) {
		/* Try generic implementation of default suspend types */
		اگر (suspend_type == SBI_HSM_SUSPEND_RET_DEFAULT ||
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
	اگر (ret == 0 && (suspend_type & SBI_HSM_SUSP_NON_RET_ثنائی_ہندسہ)) {
		خالی (*jump_warmboot)(خالی) =
			(خالی (*)(خالی))scratch->warmboot_addr;

		jump_warmboot();
	}

	/*
	 * We might have successfully resumed from retentive suspend
	 * or suspend failed. In both cases, we restore state of hart.
	 */
	اگر (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	واپس ret;
}
