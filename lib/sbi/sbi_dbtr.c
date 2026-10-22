/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems, Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_csr_detect.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_platform.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_byteorder.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_dbtr.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart_protection.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_encoding.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>


/** Offset of pointer to HART's debug triggers info in scratch space */
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ hart_state_ptr_ਵਿੱਥ;

/** Device specific debug trigger operations */
ਸਥਿਰ ਅਚਲ ਬਣਤਰ sbi_dbtr_device *dbtr_dev = ਨਿਰਾ;

ਅਚਲ ਬਣਤਰ sbi_dbtr_device *sbi_dbtr_get_device(ਖਾਲੀ)
{
	ਵਾਪਸ dbtr_dev;
}

ਖਾਲੀ sbi_dbtr_ਸੈੱਟ_device(ਅਚਲ ਬਣਤਰ sbi_dbtr_device *dev)
{
	ਜੇ (!dev || dbtr_dev)
		ਵਾਪਸ;

	dbtr_dev = dev;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ dbtr_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ਖਾਲੀ *, hart_state_ptr_ਵਿੱਥ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ dbtr_thishart_state_ptr()				\
	dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ dbtr_ਸੈੱਟ_hart_state_ptr(__scratch, __hart_state)		\
	sbi_scratch_write_type((__scratch), ਖਾਲੀ *, hart_state_ptr_ਵਿੱਥ, \
			       (__hart_state))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ tdata_read_ਸੁਰੱਖਿਅਤ(__csr)						\
	({								\
		ਬਣਤਰ sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
	})

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ tdata_write_ਸੁਰੱਖਿਅਤ(__csr, __value)				\
	({								\
		ਬਣਤਰ sbi_trap_info __trap = {0};			\
		csr_write_allowed((__csr), &__trap, (__value));		\
	})

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ tdata_implemented(__csr)					\
	({								\
		ਬਣਤਰ sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
		!__trap.cause;						\
	})

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_TRIGGER(ਸੂਚਕਾਂਕ)					\
	({								\
		ਬਣਤਰ sbi_dbtr_trigger *__trg = ਨਿਰਾ;			\
		ਬਣਤਰ sbi_dbtr_hart_triggers_state *__hs = ਨਿਰਾ;	\
		__hs = dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr()); \
		__trg = &__hs->triggers[ਸੂਚਕਾਂਕ];			\
		(__trg);						\
	})

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ for_each_trig_ਇੰਦਰਾਜ(ਅਧਾਰ, _max, _etype, ਇੰਦਰਾਜ)		\
	ਲਈ (ਪੂਰਨ _idx = 0; ਇੰਦਰਾਜ = ((_etype *)ਅਧਾਰ + _idx),		\
	     _idx < _max;						\
	     _idx++, ਇੰਦਰਾਜ = ((_etype *)ਅਧਾਰ + _idx))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBTR_SHMEM_MAKE_PHYS(_p_hi, _p_lo) (_p_lo)

/* must call with hs != NULL */
ਸਥਿਰ ਸਤਰਅੰਦਰ bool sbi_dbtr_shmem_disabled(
	ਬਣਤਰ sbi_dbtr_hart_triggers_state *hs)
{
	ਵਾਪਸ (hs->shmem.phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR &&
		hs->shmem.phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR
		? true : false);
}

/* must call with hs != NULL */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ sbi_dbtr_disable_shmem(
	ਬਣਤਰ sbi_dbtr_hart_triggers_state *hs)
{
	hs->shmem.phys_lo = SBI_DBTR_SHMEM_INVALID_ADDR;
	hs->shmem.phys_hi = SBI_DBTR_SHMEM_INVALID_ADDR;
}

/* must call with hs which is not disabled */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *hart_shmem_ਅਧਾਰ(
	ਬਣਤਰ sbi_dbtr_hart_triggers_state *hs)
{
	ਵਾਪਸ ((ਖਾਲੀ *)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)DBTR_SHMEM_MAKE_PHYS(
			hs->shmem.phys_hi, hs->shmem.phys_lo));
}

ਸਥਿਰ ਖਾਲੀ sbi_trigger_init(ਬਣਤਰ sbi_dbtr_trigger *trig,
			     ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ type_ਨਕਾਬ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ idx)
{
	trig->type_ਨਕਾਬ = type_ਨਕਾਬ;
	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;
	trig->ਸੂਚਕਾਂਕ_2 = idx;
}

ਸਥਿਰ bool dbtr_trigger_hw_supported(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ idx, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tdata1,
				      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tdata2,
				      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tdata3)
{
	ਜੇ (dbtr_dev && dbtr_dev->trigger_supported)
		ਵਾਪਸ dbtr_dev->trigger_supported(idx, tdata1, tdata2,
						   tdata3);

	ਵਾਪਸ true;
}

ਸਥਿਰ bool dbtr_trigger_any_hw_supported(
			ਬਣਤਰ sbi_dbtr_hart_triggers_state *hs,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tdata1, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tdata2,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tdata3)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ type = TDATA1_GET_TYPE(tdata1);
	ਬਣਤਰ sbi_dbtr_trigger *trig;
	ਪੂਰਨ i;

	ਲਈ (i = 0; i < hs->ਕੁੱਲ_trigs; i++) {
		trig = ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_TRIGGER(i);
		ਜੇ (__test_bit(type, &trig->type_ਨਕਾਬ) &&
		    dbtr_trigger_hw_supported(trig->ਸੂਚਕਾਂਕ_2, tdata1, tdata2,
					      tdata3))
			ਵਾਪਸ true;
	}

	ਵਾਪਸ false;
}

/*
 * Find the first free hardware trigger slot supporting the configuration.
 * Slots set in claimed_mask are treated as taken, which allows the caller
 * to track slot availability. A 32-bit mask covers RV_MAX_TRIGGERS (32);
 * for a larger number of triggers, this function needs to be updated.
 */
ਸਥਿਰ ਪੂਰਨ dbtr_ਲੱਭੋ_free_ਥਾਂ(ਬਣਤਰ sbi_dbtr_hart_triggers_state *hs,
			       u32 claimed_ਨਕਾਬ,
			       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tdata1, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tdata2,
			       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tdata3)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ type = TDATA1_GET_TYPE(tdata1);
	ਬਣਤਰ sbi_dbtr_trigger *trig;
	ਪੂਰਨ i;

	ਲਈ (i = 0; i < hs->ਕੁੱਲ_trigs; i++) {
		trig = ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_TRIGGER(i);
		ਜੇ (trig->state & RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(TS, MAPPED))
			ਜਾਰੀ;
		ਜੇ (claimed_ਨਕਾਬ & ਦੋਆਧਾਰੀ_ਅੰਕ(i))
			ਜਾਰੀ;
		ਜੇ (!__test_bit(type, &trig->type_ਨਕਾਬ))
			ਜਾਰੀ;
		ਜੇ (!dbtr_trigger_hw_supported(trig->ਸੂਚਕਾਂਕ_2, tdata1,
					       tdata2, tdata3))
			ਜਾਰੀ;
		ਵਾਪਸ i;
	}

	ਵਾਪਸ SBI_ENOENT;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਬਣਤਰ sbi_dbtr_trigger *sbi_alloc_trigger(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tdata1,
							 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tdata2,
							 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tdata3)
{
	ਪੂਰਨ i;
	ਬਣਤਰ sbi_dbtr_trigger *f_trig;
	ਬਣਤਰ sbi_dbtr_hart_triggers_state *hart_state;

	hart_state = dbtr_thishart_state_ptr();
	ਜੇ (!hart_state)
		ਵਾਪਸ ਨਿਰਾ;

	ਜੇ (hart_state->ਉਪਲਬਧ_trigs <= 0)
		ਵਾਪਸ ਨਿਰਾ;

	i = dbtr_ਲੱਭੋ_free_ਥਾਂ(hart_state, 0, tdata1, tdata2, tdata3);
	ਜੇ (i < 0)
		ਵਾਪਸ ਨਿਰਾ;

	f_trig = ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_TRIGGER(i);
	hart_state->ਉਪਲਬਧ_trigs--;
	__set_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(TS, MAPPED), &f_trig->state);

	ਵਾਪਸ f_trig;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ sbi_free_trigger(ਬਣਤਰ sbi_dbtr_trigger *trig)
{
	ਬਣਤਰ sbi_dbtr_hart_triggers_state *hart_state;

	ਜੇ (trig == ਨਿਰਾ)
		ਵਾਪਸ;

	hart_state = dbtr_thishart_state_ptr();
	ਜੇ (!hart_state)
		ਵਾਪਸ;

	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;

	hart_state->ਉਪਲਬਧ_trigs++;
}

ਪੂਰਨ sbi_dbtr_init(ਬਣਤਰ sbi_scratch *scratch, bool coldboot)
{
	ਬਣਤਰ sbi_trap_info trap = {0};
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tdata1;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ val;
	ਪੂਰਨ i;
	ਬਣਤਰ sbi_dbtr_hart_triggers_state *hart_state = ਨਿਰਾ;

	ਜੇ (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		ਵਾਪਸ SBI_SUCCESS;

	ਜੇ (coldboot) {
		hart_state_ptr_ਵਿੱਥ = sbi_scratch_alloc_type_ਵਿੱਥ(ਖਾਲੀ *);
		ਜੇ (!hart_state_ptr_ਵਿੱਥ)
			ਵਾਪਸ SBI_ENOMEM;
	}

	hart_state = dbtr_get_hart_state_ptr(scratch);
	ਜੇ (!hart_state) {
		hart_state = sbi_zalloc(ਆਕਾਰ(*hart_state));
		ਜੇ (!hart_state)
			ਵਾਪਸ SBI_ENOMEM;
		hart_state->hartid = current_hartid();
		dbtr_ਸੈੱਟ_hart_state_ptr(scratch, hart_state);
	}

	/* disable the shared memory */
	sbi_dbtr_disable_shmem(hart_state);

	/* Skip probing triggers if already probed */
	ਜੇ (hart_state->probed)
		ਜਾਓ _probed;

	ਲਈ (i = 0; i < RV_MAX_TRIGGERS; i++) {
		csr_write_allowed(CSR_TSELECT, &trap, i);
		ਜੇ (trap.cause)
			ਰੋਕੋ;

		val = csr_read_allowed(CSR_TSELECT, &trap);
		ਜੇ (trap.cause)
			ਰੋਕੋ;

		/*
		 * Read back tselect and check that it contains the
		 * written value
		 */
		ਜੇ (val != i)
			ਰੋਕੋ;

		val = csr_read_allowed(CSR_TINFO, &trap);
		ਜੇ (trap.cause) {
			/*
			 * If reading tinfo caused an exception, the
			 * debugger must read tdata1 to discover the
			 * type.
			 */
			tdata1 = csr_read_allowed(CSR_TDATA1,
						  &trap);
			ਜੇ (trap.cause)
				ਰੋਕੋ;

			ਜੇ (TDATA1_GET_TYPE(tdata1) == 0)
				ਰੋਕੋ;

			sbi_trigger_init(ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_TRIGGER(i),
					 ਦੋਆਧਾਰੀ_ਅੰਕ(TDATA1_GET_TYPE(tdata1)),
					 i);
			hart_state->ਕੁੱਲ_trigs++;
		} ਨਹੀਂਤਾਂ {
			ਜੇ (val == 1)
				ਰੋਕੋ;

			sbi_trigger_init(ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_TRIGGER(i), val, i);
			hart_state->ਕੁੱਲ_trigs++;
		}
	}

	hart_state->probed = 1;

 _probed:
	hart_state->ਉਪਲਬਧ_trigs = hart_state->ਕੁੱਲ_trigs;

	ਵਾਪਸ SBI_SUCCESS;
}

ਪੂਰਨ sbi_dbtr_get_ਕੁੱਲ_triggers(ਖਾਲੀ)
{
	ਬਣਤਰ sbi_dbtr_hart_triggers_state *hs;
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/*
	 * This function may be used during ecall registration.
	 * By that time the debug trigger module might not be
	 * initialized. If the extension is not supported, report
	 * number of triggers as 0.
	 */
	ਜੇ (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		ਵਾਪਸ 0;

	hs = dbtr_thishart_state_ptr();
	ਜੇ (!hs)
		ਵਾਪਸ 0;

	ਵਾਪਸ hs->ਕੁੱਲ_trigs;
}

ਪੂਰਨ sbi_dbtr_ਤਿਆਰੀ_shmem(ਅਚਲ ਬਣਤਰ sbi_domain *dom, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ smode,
			 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ shmem_phys_lo,
			 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ shmem_phys_hi)
{
	ਬਣਤਰ sbi_dbtr_hart_triggers_state *hart_state;

	ਜੇ (dom && !sbi_domain_is_assigned_hart(dom, current_hartindex())) {
		sbi_dprintf("%s: calling hart not assigned to this domain\n",
			   __func__);
		ਵਾਪਸ SBI_ERR_DENIED;
	}

	hart_state = dbtr_thishart_state_ptr();
	ਜੇ (!hart_state)
		ਵਾਪਸ SBI_ERR_FAILED;

	/* call is to disable shared memory */
	ਜੇ (shmem_phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR
	    && shmem_phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR) {
		sbi_dbtr_disable_shmem(hart_state);
		ਵਾਪਸ SBI_SUCCESS;
	}

	/* the shared memory must be disabled on this hart */
	ਜੇ (!sbi_dbtr_shmem_disabled(hart_state))
		ਵਾਪਸ SBI_ERR_ALREADY_ਉਪਲਬਧ;

	/* lower physical address must be XLEN/8 bytes aligned */
	ਜੇ (shmem_phys_lo & SBI_DBTR_SHMEM_ALIGN_ਨਕਾਬ)
		ਵਾਪਸ SBI_ERR_INVALID_PARAM;

	/*
	* On RV32, the M-mode can only access the first 4GB of
	* the physical address space because M-mode does not have
	* MMU to access full 34-bit physical address space.
	* So fail if the upper 32 bits of the physical address
	* is non-zero on RV32.
	*
	* On RV64, kernel sets upper 64bit address part to zero.
	* So fail if the upper 64bit of the physical address
	* is non-zero on RV64.
	*/
	ਜੇ (shmem_phys_hi)
		ਵਾਪਸ SBI_EINVALID_ADDR;

	ਜੇ (dom && !sbi_domain_check_addr(dom,
		  DBTR_SHMEM_MAKE_PHYS(shmem_phys_hi, shmem_phys_lo), smode,
		  SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		ਵਾਪਸ SBI_ERR_INVALID_ਪਤਾ;

	hart_state->shmem.phys_lo = shmem_phys_lo;
	hart_state->shmem.phys_hi = shmem_phys_hi;

	ਵਾਪਸ SBI_SUCCESS;
}

ਸਥਿਰ ਖਾਲੀ dbtr_trigger_ਤਿਆਰੀ(ਬਣਤਰ sbi_dbtr_trigger *trig,
			       ਬਣਤਰ sbi_dbtr_ਅੰਕੜੇ_ਸੁਨੇਹਾ *recv)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tdata1;

	ਜੇ (!trig)
		ਵਾਪਸ;

	trig->tdata1 = lle_ਮੰਜ਼ਿਲ_cpu(recv->tdata1);
	trig->tdata2 = lle_ਮੰਜ਼ਿਲ_cpu(recv->tdata2);
	trig->tdata3 = lle_ਮੰਜ਼ਿਲ_cpu(recv->tdata3);

	tdata1 = lle_ਮੰਜ਼ਿਲ_cpu(recv->tdata1);

	trig->state = 0;

	__set_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(TS, MAPPED), &trig->state);

	ਸੈੱਟ_TRIG_HW_ਸੂਚਕਾਂਕ(trig->state, trig->ਸੂਚਕਾਂਕ_2);

	ਚੋਣ (TDATA1_GET_TYPE(tdata1)) {
	ਹਾਲਤ RISCV_DBTR_TRIG_MCONTROL:
		ਜੇ (__test_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(MC, U), &tdata1))
			__set_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(TS, U), &trig->state);

		ਜੇ (__test_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(MC, S), &tdata1))
			__set_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(TS, S), &trig->state);
		ਰੋਕੋ;
	ਹਾਲਤ RISCV_DBTR_TRIG_MCONTROL6:
		ਜੇ (__test_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(MC6, U), &tdata1))
			__set_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(TS, U), &trig->state);

		ਜੇ (__test_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(MC6, S), &tdata1))
			__set_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(TS, S), &trig->state);

		ਜੇ (__test_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(MC6, VU), &tdata1))
			__set_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(TS, VU), &trig->state);

		ਜੇ (__test_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(MC6, VS), &tdata1))
			__set_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(TS, VS), &trig->state);
		ਰੋਕੋ;
	ਹਾਲਤ RISCV_DBTR_TRIG_ICOUNT:
		ਜੇ (__test_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(ICOUNT, U), &tdata1))
			__set_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(TS, U), &trig->state);

		ਜੇ (__test_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(ICOUNT, S), &tdata1))
			__set_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(TS, S), &trig->state);

		ਜੇ (__test_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(ICOUNT, VU), &tdata1))
			__set_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(TS, VU), &trig->state);

		ਜੇ (__test_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(ICOUNT, VS), &tdata1))
			__set_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(TS, VS), &trig->state);
		ਰੋਕੋ;
	ਮੂਲ:
		sbi_dprintf("%s: Unknown type (tdata1: 0x%lx Type: %ld)\n",
			    __func__, tdata1, TDATA1_GET_TYPE(tdata1));
		ਰੋਕੋ;
	}
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ update_ਦੋਆਧਾਰੀ_ਅੰਕ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ new, ਪੂਰਨ nr, ਅਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *addr)
{
	ਜੇ (new)
		__set_bit(nr, addr);
	ਨਹੀਂਤਾਂ
		__clear_bit(nr, addr);
}

ਸਥਿਰ ਖਾਲੀ dbtr_trigger_enable(ਬਣਤਰ sbi_dbtr_trigger *trig)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ state;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tdata1;

	ਜੇ (!trig || !(trig->state & RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(TS, MAPPED)))
		ਵਾਪਸ;

	state = trig->state;
	tdata1 = trig->tdata1;

	ਚੋਣ (TDATA1_GET_TYPE(tdata1)) {
	ਹਾਲਤ RISCV_DBTR_TRIG_MCONTROL:
		update_ਦੋਆਧਾਰੀ_ਅੰਕ(state & RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(TS, U),
			   RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(MC, U), &trig->tdata1);
		update_ਦੋਆਧਾਰੀ_ਅੰਕ(state & RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(TS, S),
			   RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(MC, S), &trig->tdata1);
		ਰੋਕੋ;
	ਹਾਲਤ RISCV_DBTR_TRIG_MCONTROL6:
		update_ਦੋਆਧਾਰੀ_ਅੰਕ(state & RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(TS, VU),
			   RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(MC6, VU), &trig->tdata1);
		update_ਦੋਆਧਾਰੀ_ਅੰਕ(state & RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(TS, VS),
			   RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(MC6, VS), &trig->tdata1);
		update_ਦੋਆਧਾਰੀ_ਅੰਕ(state & RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(TS, U),
			   RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(MC6, U), &trig->tdata1);
		update_ਦੋਆਧਾਰੀ_ਅੰਕ(state & RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(TS, S),
			   RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(MC6, S), &trig->tdata1);
		ਰੋਕੋ;
	ਹਾਲਤ RISCV_DBTR_TRIG_ICOUNT:
		update_ਦੋਆਧਾਰੀ_ਅੰਕ(state & RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(TS, VU),
			   RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(ICOUNT, VU), &trig->tdata1);
		update_ਦੋਆਧਾਰੀ_ਅੰਕ(state & RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(TS, VS),
			   RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(ICOUNT, VS), &trig->tdata1);
		update_ਦੋਆਧਾਰੀ_ਅੰਕ(state & RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(TS, U),
			   RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(ICOUNT, U), &trig->tdata1);
		update_ਦੋਆਧਾਰੀ_ਅੰਕ(state & RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(TS, S),
			   RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(ICOUNT, S), &trig->tdata1);
		ਰੋਕੋ;
	ਮੂਲ:
		ਰੋਕੋ;
	}

	/*
	 * RISC-V Debug Support v1.0.0 section 5.5:
	 * Debugger cannot simply set a trigger by writing tdata1, then tdata2,
	 * etc. The current value of tdata2 might not be legal with the new
	 * value of tdata1. To help with this situation, it is guaranteed that
	 * writing 0 to tdata1 disables the trigger, and leaves it in a state
	 * where tdata2 and tdata3 can be written with any value that makes
	 * sense for any trigger type supported by this trigger.
	 */
	csr_write(CSR_TSELECT, trig->ਸੂਚਕਾਂਕ_2);
	csr_write(CSR_TDATA1, 0x0);
	tdata_write_ਸੁਰੱਖਿਅਤ(CSR_TDATA2, trig->tdata2);
	tdata_write_ਸੁਰੱਖਿਅਤ(CSR_TDATA3, trig->tdata3);
	csr_write(CSR_TDATA1, trig->tdata1);
}

ਸਥਿਰ ਖਾਲੀ dbtr_trigger_disable(ਬਣਤਰ sbi_dbtr_trigger *trig)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tdata1;

	ਜੇ (!trig || !(trig->state & RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(TS, MAPPED)))
		ਵਾਪਸ;

	tdata1 = trig->tdata1;

	ਚੋਣ (TDATA1_GET_TYPE(tdata1)) {
	ਹਾਲਤ RISCV_DBTR_TRIG_MCONTROL:
		__clear_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(MC, U), &trig->tdata1);
		__clear_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(MC, S), &trig->tdata1);
		ਰੋਕੋ;
	ਹਾਲਤ RISCV_DBTR_TRIG_MCONTROL6:
		__clear_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(MC6, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(MC6, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(MC6, U), &trig->tdata1);
		__clear_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(MC6, S), &trig->tdata1);
		ਰੋਕੋ;
	ਹਾਲਤ RISCV_DBTR_TRIG_ICOUNT:
		__clear_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(ICOUNT, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(ICOUNT, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(ICOUNT, U), &trig->tdata1);
		__clear_bit(RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ(ICOUNT, S), &trig->tdata1);
		ਰੋਕੋ;
	ਮੂਲ:
		ਰੋਕੋ;
	}

	csr_write(CSR_TSELECT, trig->ਸੂਚਕਾਂਕ_2);
	csr_write(CSR_TDATA1, trig->tdata1);
}

ਸਥਿਰ ਖਾਲੀ dbtr_trigger_clear(ਬਣਤਰ sbi_dbtr_trigger *trig)
{
	ਜੇ (!trig || !(trig->state & RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(TS, MAPPED)))
		ਵਾਪਸ;

	csr_write(CSR_TSELECT, trig->ਸੂਚਕਾਂਕ_2);
	csr_write(CSR_TDATA1, 0x0);
	tdata_write_ਸੁਰੱਖਿਅਤ(CSR_TDATA2, 0x0);
	tdata_write_ਸੁਰੱਖਿਅਤ(CSR_TDATA3, 0x0);
}

ਸਥਿਰ ਪੂਰਨ dbtr_trigger_supported(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ type)
{
	ਚੋਣ (type) {
	ਹਾਲਤ RISCV_DBTR_TRIG_MCONTROL:
	ਹਾਲਤ RISCV_DBTR_TRIG_MCONTROL6:
	ਹਾਲਤ RISCV_DBTR_TRIG_ICOUNT:
		ਵਾਪਸ 1;
	ਮੂਲ:
		ਰੋਕੋ;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ dbtr_trigger_valid(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ type, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tdata)
{
	ਚੋਣ (type) {
	ਹਾਲਤ RISCV_DBTR_TRIG_MCONTROL:
		ਜੇ (!(tdata & RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(MC, DMODE)) &&
		    !(tdata & RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(MC, M)))
			ਵਾਪਸ 1;
		ਰੋਕੋ;
	ਹਾਲਤ RISCV_DBTR_TRIG_MCONTROL6:
		ਜੇ (!(tdata & RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(MC6, DMODE)) &&
		    !(tdata & RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(MC6, M)))
			ਵਾਪਸ 1;
		ਰੋਕੋ;
	ਹਾਲਤ RISCV_DBTR_TRIG_ICOUNT:
		ਜੇ (!(tdata & RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(ICOUNT, DMODE)) &&
		    !(tdata & RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(ICOUNT, M)))
			ਵਾਪਸ 1;
		ਰੋਕੋ;
	ਮੂਲ:
		ਰੋਕੋ;
	}

	ਵਾਪਸ 0;
}

ਪੂਰਨ sbi_dbtr_ਅੰਕ_trig(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਬਾਹਰਲਾ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ type = TDATA1_GET_TYPE(ਅੰਕੜੇ);
	u32 hartid = current_hartid();
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਕੁੱਲ = 0;
	ਬਣਤਰ sbi_dbtr_trigger *trig;
	ਪੂਰਨ i;
	ਬਣਤਰ sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	ਜੇ (!hs)
		ਵਾਪਸ SBI_ERR_FAILED;

	ਜੇ (ਅੰਕੜੇ == 0) {
		*ਬਾਹਰਲਾ = hs->ਕੁੱਲ_trigs;
		ਵਾਪਸ SBI_SUCCESS;
	}

	ਲਈ (i = 0; i < hs->ਕੁੱਲ_trigs; i++) {
		trig = ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_TRIGGER(i);

		ਜੇ (__test_bit(type, &trig->type_ਨਕਾਬ) &&
		    dbtr_trigger_hw_supported(trig->ਸੂਚਕਾਂਕ_2, ਅੰਕੜੇ, 0, 0))
			ਕੁੱਲ++;
	}

	sbi_dprintf("%s: hart%d: total triggers of type %lu: %lu\n",
		    __func__, hartid, type, ਕੁੱਲ);

	*ਬਾਹਰਲਾ = ਕੁੱਲ;
	ਵਾਪਸ SBI_SUCCESS;
}

ਪੂਰਨ sbi_dbtr_read_trig(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ smode,
		       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ trig_idx_ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ trig_ਗਿਣਤੀ)
{
	ਬਣਤਰ sbi_dbtr_ਅੰਕੜੇ_ਸੁਨੇਹਾ *xmit;
	ਬਣਤਰ sbi_dbtr_trigger *trig;
	ਸੰਘ sbi_dbtr_shmem_ਇੰਦਰਾਜ *ਇੰਦਰਾਜ_2;
	ਖਾਲੀ *shmem_ਅਧਾਰ = ਨਿਰਾ;
	ਬਣਤਰ sbi_dbtr_hart_triggers_state *hs = ਨਿਰਾ;

	hs = dbtr_thishart_state_ptr();
	ਜੇ (!hs)
		ਵਾਪਸ SBI_ERR_FAILED;

	ਜੇ (trig_idx_ਅਧਾਰ >= hs->ਕੁੱਲ_trigs ||
	    trig_idx_ਅਧਾਰ + trig_ਗਿਣਤੀ >= hs->ਕੁੱਲ_trigs)
		ਵਾਪਸ SBI_ERR_BAD_RANGE;

	ਜੇ (sbi_dbtr_shmem_disabled(hs))
		ਵਾਪਸ SBI_ERR_NO_SHMEM;

	shmem_ਅਧਾਰ = hart_shmem_ਅਧਾਰ(hs);

	sbi_hart_protection_ਆਰਜ਼ੀ_map_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)shmem_ਅਧਾਰ,
					   trig_ਗਿਣਤੀ * ਆਕਾਰ(*ਇੰਦਰਾਜ_2));
	for_each_trig_ਇੰਦਰਾਜ(shmem_ਅਧਾਰ, trig_ਗਿਣਤੀ, typeof(*ਇੰਦਰਾਜ_2), ਇੰਦਰਾਜ_2) {
		xmit = &ਇੰਦਰਾਜ_2->ਅੰਕੜੇ;
		trig = ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_TRIGGER((_idx + trig_idx_ਅਧਾਰ));
		csr_write(CSR_TSELECT, trig->ਸੂਚਕਾਂਕ_2);
		trig->tdata1 = csr_read(CSR_TDATA1);
		trig->tdata2 = tdata_read_ਸੁਰੱਖਿਅਤ(CSR_TDATA2);
		trig->tdata3 = tdata_read_ਸੁਰੱਖਿਅਤ(CSR_TDATA3);
		xmit->tstate = cpu_ਮੰਜ਼ਿਲ_lle(trig->state);
		xmit->tdata1 = cpu_ਮੰਜ਼ਿਲ_lle(trig->tdata1);
		xmit->tdata2 = cpu_ਮੰਜ਼ਿਲ_lle(trig->tdata2);
		xmit->tdata3 = cpu_ਮੰਜ਼ਿਲ_lle(trig->tdata3);
	}
	sbi_hart_protection_ਆਰਜ਼ੀ_unmap_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)shmem_ਅਧਾਰ,
					     trig_ਗਿਣਤੀ * ਆਕਾਰ(*ਇੰਦਰਾਜ_2));

	ਵਾਪਸ SBI_SUCCESS;
}

ਪੂਰਨ sbi_dbtr_install_trig(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ smode,
			  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ trig_ਗਿਣਤੀ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਬਾਹਰਲਾ)
{
	ਖਾਲੀ *shmem_ਅਧਾਰ = ਨਿਰਾ;
	ਸੰਘ sbi_dbtr_shmem_ਇੰਦਰਾਜ *ਇੰਦਰਾਜ_2;
	ਬਣਤਰ sbi_dbtr_ਅੰਕੜੇ_ਸੁਨੇਹਾ *recv;
	ਬਣਤਰ sbi_dbtr_id_ਸੁਨੇਹਾ *xmit;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ctrl;
	u32 claimed = 0;
	ਪੂਰਨ ਥਾਂ;
	ਬਣਤਰ sbi_dbtr_trigger *trig;
	ਬਣਤਰ sbi_dbtr_hart_triggers_state *hs = ਨਿਰਾ;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	ਜੇ (!hs)
		ਵਾਪਸ SBI_ERR_FAILED;

	ਜੇ (sbi_dbtr_shmem_disabled(hs))
		ਵਾਪਸ SBI_ERR_NO_SHMEM;

	shmem_ਅਧਾਰ = hart_shmem_ਅਧਾਰ(hs);
	sbi_hart_protection_ਆਰਜ਼ੀ_map_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)shmem_ਅਧਾਰ,
					   trig_ਗਿਣਤੀ * ਆਕਾਰ(*ਇੰਦਰਾਜ_2));

	/*
	 * SBI v3.0 sec 19.4 requires SBI_ERR_NOT_SUPPORTED when a trigger
	 * programs a non-zero value into an unimplemented optional CSR. Only
	 * the "whole CSR unimplemented" case is caught here; WARL bits tied
	 * off inside an otherwise-implemented CSR are delegated to the
	 * device-specific trigger_supported() callback via
	 * dbtr_trigger_any_hw_supported().
	 */
	tdata2_impl = tdata_implemented(CSR_TDATA2);
	tdata3_impl = tdata_implemented(CSR_TDATA3);

	/* Check requested triggers configuration */
	for_each_trig_ਇੰਦਰਾਜ(shmem_ਅਧਾਰ, trig_ਗਿਣਤੀ, typeof(*ਇੰਦਰਾਜ_2), ਇੰਦਰਾਜ_2) {
		recv = (ਬਣਤਰ sbi_dbtr_ਅੰਕੜੇ_ਸੁਨੇਹਾ *)(&ਇੰਦਰਾਜ_2->ਅੰਕੜੇ);
		ctrl = recv->tdata1;

		ਜੇ (!dbtr_trigger_supported(TDATA1_GET_TYPE(ctrl))) {
			*ਬਾਹਰਲਾ = _idx;
			sbi_hart_protection_ਆਰਜ਼ੀ_unmap_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)shmem_ਅਧਾਰ,
							     trig_ਗਿਣਤੀ * ਆਕਾਰ(*ਇੰਦਰਾਜ_2));
			ਵਾਪਸ SBI_ERR_FAILED;
		}

		ਜੇ (!dbtr_trigger_valid(TDATA1_GET_TYPE(ctrl), ctrl)) {
			*ਬਾਹਰਲਾ = _idx;
			sbi_hart_protection_ਆਰਜ਼ੀ_unmap_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)shmem_ਅਧਾਰ,
							     trig_ਗਿਣਤੀ * ਆਕਾਰ(*ਇੰਦਰਾਜ_2));
			ਵਾਪਸ SBI_ERR_INVALID_PARAM;
		}

		ਜੇ ((recv->tdata2 && !tdata2_impl) ||
		    (recv->tdata3 && !tdata3_impl)) {
			*ਬਾਹਰਲਾ = _idx;
			sbi_hart_protection_ਆਰਜ਼ੀ_unmap_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)shmem_ਅਧਾਰ,
							     trig_ਗਿਣਤੀ * ਆਕਾਰ(*ਇੰਦਰਾਜ_2));
			ਵਾਪਸ SBI_ERR_NOT_SUPPORTED;
		}

		ਜੇ (!dbtr_trigger_any_hw_supported(hs,
						   lle_ਮੰਜ਼ਿਲ_cpu(recv->tdata1),
						   lle_ਮੰਜ਼ਿਲ_cpu(recv->tdata2),
						   lle_ਮੰਜ਼ਿਲ_cpu(recv->tdata3))) {
			*ਬਾਹਰਲਾ = _idx;
			sbi_hart_protection_ਆਰਜ਼ੀ_unmap_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)shmem_ਅਧਾਰ,
							     trig_ਗਿਣਤੀ * ਆਕਾਰ(*ਇੰਦਰਾਜ_2));
			ਵਾਪਸ SBI_ERR_NOT_SUPPORTED;
		}
	}

	ਜੇ (hs->ਉਪਲਬਧ_trigs < trig_ਗਿਣਤੀ) {
		*ਬਾਹਰਲਾ = hs->ਉਪਲਬਧ_trigs;
		sbi_hart_protection_ਆਰਜ਼ੀ_unmap_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)shmem_ਅਧਾਰ,
						     trig_ਗਿਣਤੀ * ਆਕਾਰ(*ਇੰਦਰਾਜ_2));
		ਵਾਪਸ SBI_ERR_FAILED;
	}

	/*
	 * Dry-run the allocation of the whole batch so that no trigger
	 * is installed if any of the requested configurations cannot be
	 * matched to a free hardware trigger slot.
	 */
	for_each_trig_ਇੰਦਰਾਜ(shmem_ਅਧਾਰ, trig_ਗਿਣਤੀ, typeof(*ਇੰਦਰਾਜ_2), ਇੰਦਰਾਜ_2) {
		recv = (ਬਣਤਰ sbi_dbtr_ਅੰਕੜੇ_ਸੁਨੇਹਾ *)(&ਇੰਦਰਾਜ_2->ਅੰਕੜੇ);
		ਥਾਂ = dbtr_ਲੱਭੋ_free_ਥਾਂ(hs, claimed,
					   lle_ਮੰਜ਼ਿਲ_cpu(recv->tdata1),
					   lle_ਮੰਜ਼ਿਲ_cpu(recv->tdata2),
					   lle_ਮੰਜ਼ਿਲ_cpu(recv->tdata3));
		ਜੇ (ਥਾਂ < 0) {
			*ਬਾਹਰਲਾ = _idx;
			sbi_hart_protection_ਆਰਜ਼ੀ_unmap_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)shmem_ਅਧਾਰ,
							     trig_ਗਿਣਤੀ * ਆਕਾਰ(*ਇੰਦਰਾਜ_2));
			ਵਾਪਸ SBI_ERR_FAILED;
		}
		claimed |= ਦੋਆਧਾਰੀ_ਅੰਕ(ਥਾਂ);
	}

	/* Install triggers */
	for_each_trig_ਇੰਦਰਾਜ(shmem_ਅਧਾਰ, trig_ਗਿਣਤੀ, typeof(*ਇੰਦਰਾਜ_2), ਇੰਦਰਾਜ_2) {
		recv = (ਬਣਤਰ sbi_dbtr_ਅੰਕੜੇ_ਸੁਨੇਹਾ *)(&ਇੰਦਰਾਜ_2->ਅੰਕੜੇ);
		xmit = (ਬਣਤਰ sbi_dbtr_id_ਸੁਨੇਹਾ *)(&ਇੰਦਰਾਜ_2->id);

		/*
		 * The dry-run above matched every requested configuration
		 * to a free hardware trigger slot, so allocation must
		 * succeed.
		 */
		trig = sbi_alloc_trigger(lle_ਮੰਜ਼ਿਲ_cpu(recv->tdata1),
					 lle_ਮੰਜ਼ਿਲ_cpu(recv->tdata2),
					 lle_ਮੰਜ਼ਿਲ_cpu(recv->tdata3));

		dbtr_trigger_ਤਿਆਰੀ(trig,  recv);
		dbtr_trigger_enable(trig);
		xmit->idx = cpu_ਮੰਜ਼ਿਲ_lle(trig->ਸੂਚਕਾਂਕ_2);

	}

	sbi_hart_protection_ਆਰਜ਼ੀ_unmap_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)shmem_ਅਧਾਰ,
					     trig_ਗਿਣਤੀ * ਆਕਾਰ(*ਇੰਦਰਾਜ_2));

	ਵਾਪਸ SBI_SUCCESS;
}

ਪੂਰਨ sbi_dbtr_uninstall_trig(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ trig_idx_ਅਧਾਰ,
			    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ trig_idx_ਨਕਾਬ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ trig_ਨਕਾਬ = trig_idx_ਨਕਾਬ << trig_idx_ਅਧਾਰ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ idx = trig_idx_ਅਧਾਰ;
	ਬਣਤਰ sbi_dbtr_trigger *trig;
	ਬਣਤਰ sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	ਜੇ (!hs)
		ਵਾਪਸ SBI_ERR_FAILED;

	for_each_ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_from(idx, &trig_ਨਕਾਬ, hs->ਕੁੱਲ_trigs) {
		trig = ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_TRIGGER(idx);
		ਜੇ (!(trig->state & RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(TS, MAPPED)))
			ਵਾਪਸ SBI_ERR_INVALID_PARAM;

		dbtr_trigger_clear(trig);

		sbi_free_trigger(trig);
	}

	ਵਾਪਸ SBI_SUCCESS;
}

ਪੂਰਨ sbi_dbtr_enable_trig(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ trig_idx_ਅਧਾਰ,
			 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ trig_idx_ਨਕਾਬ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ trig_ਨਕਾਬ = trig_idx_ਨਕਾਬ << trig_idx_ਅਧਾਰ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ idx = trig_idx_ਅਧਾਰ;
	ਬਣਤਰ sbi_dbtr_trigger *trig;
	ਬਣਤਰ sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	ਜੇ (!hs)
		ਵਾਪਸ SBI_ERR_FAILED;

	for_each_ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_from(idx, &trig_ਨਕਾਬ, hs->ਕੁੱਲ_trigs) {
		trig = ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_TRIGGER(idx);
		sbi_dprintf("%s: enable trigger %lu\n", __func__, idx);
		dbtr_trigger_enable(trig);
	}

	ਵਾਪਸ SBI_SUCCESS;
}

ਪੂਰਨ sbi_dbtr_update_trig(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ smode,
			 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ trig_ਗਿਣਤੀ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ trig_idx;
	ਬਣਤਰ sbi_dbtr_trigger *trig;
	ਸੰਘ sbi_dbtr_shmem_ਇੰਦਰਾਜ *ਇੰਦਰਾਜ_2;
	ਖਾਲੀ *shmem_ਅਧਾਰ = ਨਿਰਾ;
	ਬਣਤਰ sbi_dbtr_hart_triggers_state *hs = ਨਿਰਾ;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	ਜੇ (!hs)
		ਵਾਪਸ SBI_ERR_FAILED;

	ਜੇ (sbi_dbtr_shmem_disabled(hs))
		ਵਾਪਸ SBI_ERR_NO_SHMEM;

	shmem_ਅਧਾਰ = hart_shmem_ਅਧਾਰ(hs);

	ਜੇ (trig_ਗਿਣਤੀ >= hs->ਕੁੱਲ_trigs)
		ਵਾਪਸ SBI_ERR_BAD_RANGE;

	/*
	 * SBI v3.0 sec 19.5 requires SBI_ERR_NOT_SUPPORTED when a trigger
	 * programs a non-zero value into an unimplemented optional CSR. Only
	 * the "whole CSR unimplemented" case is caught; WARL bits tied off
	 * inside an otherwise-implemented CSR are not.
	 */
	tdata2_impl = tdata_implemented(CSR_TDATA2);
	tdata3_impl = tdata_implemented(CSR_TDATA3);

	for_each_trig_ਇੰਦਰਾਜ(shmem_ਅਧਾਰ, trig_ਗਿਣਤੀ, typeof(*ਇੰਦਰਾਜ_2), ਇੰਦਰਾਜ_2) {
		sbi_hart_protection_ਆਰਜ਼ੀ_map_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)ਇੰਦਰਾਜ_2, ਆਕਾਰ(*ਇੰਦਰਾਜ_2));
		trig_idx = ਇੰਦਰਾਜ_2->id.idx;

		ਜੇ (trig_idx >= hs->ਕੁੱਲ_trigs) {
			sbi_hart_protection_ਆਰਜ਼ੀ_unmap_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)ਇੰਦਰਾਜ_2, ਆਕਾਰ(*ਇੰਦਰਾਜ_2));
			ਵਾਪਸ SBI_ERR_INVALID_PARAM;
		}

		trig = ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_TRIGGER(trig_idx);

		ਜੇ (!(trig->state & RV_DBTR_ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(TS, MAPPED))) {
			sbi_hart_protection_ਆਰਜ਼ੀ_unmap_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)ਇੰਦਰਾਜ_2, ਆਕਾਰ(*ਇੰਦਰਾਜ_2));
			ਵਾਪਸ SBI_ERR_FAILED;
		}

		ਜੇ ((ਇੰਦਰਾਜ_2->ਅੰਕੜੇ.tdata2 && !tdata2_impl) ||
		    (ਇੰਦਰਾਜ_2->ਅੰਕੜੇ.tdata3 && !tdata3_impl)) {
			sbi_hart_protection_ਆਰਜ਼ੀ_unmap_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)ਇੰਦਰਾਜ_2, ਆਕਾਰ(*ਇੰਦਰਾਜ_2));
			ਵਾਪਸ SBI_ERR_NOT_SUPPORTED;
		}

		ਜੇ (!dbtr_trigger_hw_supported(trig->ਸੂਚਕਾਂਕ_2,
					       lle_ਮੰਜ਼ਿਲ_cpu(ਇੰਦਰਾਜ_2->ਅੰਕੜੇ.tdata1),
					       lle_ਮੰਜ਼ਿਲ_cpu(ਇੰਦਰਾਜ_2->ਅੰਕੜੇ.tdata2),
					       lle_ਮੰਜ਼ਿਲ_cpu(ਇੰਦਰਾਜ_2->ਅੰਕੜੇ.tdata3))) {
			sbi_hart_protection_ਆਰਜ਼ੀ_unmap_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)ਇੰਦਰਾਜ_2, ਆਕਾਰ(*ਇੰਦਰਾਜ_2));
			ਵਾਪਸ SBI_ERR_NOT_SUPPORTED;
		}

		dbtr_trigger_ਤਿਆਰੀ(trig, &ਇੰਦਰਾਜ_2->ਅੰਕੜੇ);
		sbi_hart_protection_ਆਰਜ਼ੀ_unmap_range((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)ਇੰਦਰਾਜ_2, ਆਕਾਰ(*ਇੰਦਰਾਜ_2));
		dbtr_trigger_enable(trig);
	}

	ਵਾਪਸ SBI_SUCCESS;
}

ਪੂਰਨ sbi_dbtr_disable_trig(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ trig_idx_ਅਧਾਰ,
			  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ trig_idx_ਨਕਾਬ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ trig_ਨਕਾਬ = trig_idx_ਨਕਾਬ << trig_idx_ਅਧਾਰ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ idx = trig_idx_ਅਧਾਰ;
	ਬਣਤਰ sbi_dbtr_trigger *trig;
	ਬਣਤਰ sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	ਜੇ (!hs)
		ਵਾਪਸ SBI_ERR_FAILED;

	for_each_ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_from(idx, &trig_ਨਕਾਬ, hs->ਕੁੱਲ_trigs) {
		trig = ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_TRIGGER(idx);
		dbtr_trigger_disable(trig);
	}

	ਵਾਪਸ SBI_SUCCESS;
}
