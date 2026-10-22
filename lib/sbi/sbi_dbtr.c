/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems, Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#అన్తర్భావ <sbi/sbi_ecall_interface.h>
#అన్తర్భావ <sbi/sbi_csr_detect.h>
#అన్తర్భావ <sbi/sbi_platform.h>
#అన్తర్భావ <sbi/sbi_byteorder.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_trap.h>
#అన్తర్భావ <sbi/sbi_dbtr.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi/sbi_hart_protection.h>
#అన్తర్భావ <sbi/riscv_encoding.h>
#అన్తర్భావ <sbi/riscv_asm.h>


/** Offset of pointer to HART's debug triggers info in scratch space */
స్థిర గుర్తులేని దీర్ఘ hart_state_ptr_స్థానభ్రంశం;

/** Device specific debug trigger operations */
స్థిర స్థిరరాశి నిర్మాణం sbi_dbtr_device *dbtr_dev = ఖాళీ;

స్థిరరాశి నిర్మాణం sbi_dbtr_device *sbi_dbtr_get_device(శూన్య)
{
	తిరిగి dbtr_dev;
}

శూన్య sbi_dbtr_సమితి_device(స్థిరరాశి నిర్మాణం sbi_dbtr_device *dev)
{
	ఒకవేళ (!dev || dbtr_dev)
		తిరిగి;

	dbtr_dev = dev;
}

#నిర్వచించు dbtr_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), శూన్య *, hart_state_ptr_స్థానభ్రంశం)

#నిర్వచించు dbtr_thishart_state_ptr()				\
	dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#నిర్వచించు dbtr_సమితి_hart_state_ptr(__scratch, __hart_state)		\
	sbi_scratch_write_type((__scratch), శూన్య *, hart_state_ptr_స్థానభ్రంశం, \
			       (__hart_state))

#నిర్వచించు tdata_read_సురక్షిత(__csr)						\
	({								\
		నిర్మాణం sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
	})

#నిర్వచించు tdata_write_సురక్షిత(__csr, __value)				\
	({								\
		నిర్మాణం sbi_trap_info __trap = {0};			\
		csr_write_allowed((__csr), &__trap, (__value));		\
	})

#నిర్వచించు tdata_implemented(__csr)					\
	({								\
		నిర్మాణం sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
		!__trap.cause;						\
	})

#నిర్వచించు సూచీ_గమ్యం_TRIGGER(సూచీ)					\
	({								\
		నిర్మాణం sbi_dbtr_trigger *__trg = ఖాళీ;			\
		నిర్మాణం sbi_dbtr_hart_triggers_state *__hs = ఖాళీ;	\
		__hs = dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr()); \
		__trg = &__hs->triggers[సూచీ];			\
		(__trg);						\
	})

#నిర్వచించు for_each_trig_నమోదు(పునాది, _max, _etype, నమోదు_2)		\
	కొరకు (పూర్ణాంకం _idx = 0; నమోదు_2 = ((_etype *)పునాది + _idx),		\
	     _idx < _max;						\
	     _idx++, నమోదు_2 = ((_etype *)పునాది + _idx))

#నిర్వచించు DBTR_SHMEM_MAKE_PHYS(_p_hi, _p_lo) (_p_lo)

/* must call with hs != NULL */
స్థిర అంతఃవరుస bool sbi_dbtr_shmem_disabled(
	నిర్మాణం sbi_dbtr_hart_triggers_state *hs)
{
	తిరిగి (hs->shmem.phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR &&
		hs->shmem.phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR
		? true : false);
}

/* must call with hs != NULL */
స్థిర అంతఃవరుస శూన్య sbi_dbtr_disable_shmem(
	నిర్మాణం sbi_dbtr_hart_triggers_state *hs)
{
	hs->shmem.phys_lo = SBI_DBTR_SHMEM_INVALID_ADDR;
	hs->shmem.phys_hi = SBI_DBTR_SHMEM_INVALID_ADDR;
}

/* must call with hs which is not disabled */
స్థిర అంతఃవరుస శూన్య *hart_shmem_పునాది(
	నిర్మాణం sbi_dbtr_hart_triggers_state *hs)
{
	తిరిగి ((శూన్య *)(గుర్తులేని దీర్ఘ)DBTR_SHMEM_MAKE_PHYS(
			hs->shmem.phys_hi, hs->shmem.phys_lo));
}

స్థిర శూన్య sbi_trigger_init(నిర్మాణం sbi_dbtr_trigger *trig,
			     గుర్తులేని దీర్ఘ type_ముసుగు, గుర్తులేని దీర్ఘ idx)
{
	trig->type_ముసుగు = type_ముసుగు;
	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;
	trig->సూచీ_2 = idx;
}

స్థిర bool dbtr_trigger_hw_supported(గుర్తులేని దీర్ఘ idx, గుర్తులేని దీర్ఘ tdata1,
				      గుర్తులేని దీర్ఘ tdata2,
				      గుర్తులేని దీర్ఘ tdata3)
{
	ఒకవేళ (dbtr_dev && dbtr_dev->trigger_supported)
		తిరిగి dbtr_dev->trigger_supported(idx, tdata1, tdata2,
						   tdata3);

	తిరిగి true;
}

స్థిర bool dbtr_trigger_any_hw_supported(
			నిర్మాణం sbi_dbtr_hart_triggers_state *hs,
			గుర్తులేని దీర్ఘ tdata1, గుర్తులేని దీర్ఘ tdata2,
			గుర్తులేని దీర్ఘ tdata3)
{
	గుర్తులేని దీర్ఘ type = TDATA1_GET_TYPE(tdata1);
	నిర్మాణం sbi_dbtr_trigger *trig;
	పూర్ణాంకం i;

	కొరకు (i = 0; i < hs->మొత్తం_trigs; i++) {
		trig = సూచీ_గమ్యం_TRIGGER(i);
		ఒకవేళ (__test_bit(type, &trig->type_ముసుగు) &&
		    dbtr_trigger_hw_supported(trig->సూచీ_2, tdata1, tdata2,
					      tdata3))
			తిరిగి true;
	}

	తిరిగి false;
}

/*
 * Find the first free hardware trigger slot supporting the configuration.
 * Slots set in claimed_mask are treated as taken, which allows the caller
 * to track slot availability. A 32-bit mask covers RV_MAX_TRIGGERS (32);
 * for a larger number of triggers, this function needs to be updated.
 */
స్థిర పూర్ణాంకం dbtr_కనుగొను_free_స్థానం(నిర్మాణం sbi_dbtr_hart_triggers_state *hs,
			       u32 claimed_ముసుగు,
			       గుర్తులేని దీర్ఘ tdata1, గుర్తులేని దీర్ఘ tdata2,
			       గుర్తులేని దీర్ఘ tdata3)
{
	గుర్తులేని దీర్ఘ type = TDATA1_GET_TYPE(tdata1);
	నిర్మాణం sbi_dbtr_trigger *trig;
	పూర్ణాంకం i;

	కొరకు (i = 0; i < hs->మొత్తం_trigs; i++) {
		trig = సూచీ_గమ్యం_TRIGGER(i);
		ఒకవేళ (trig->state & RV_DBTR_బిట్టు_ముసుగు(TS, MAPPED))
			కొనసాగు;
		ఒకవేళ (claimed_ముసుగు & బిట్టు(i))
			కొనసాగు;
		ఒకవేళ (!__test_bit(type, &trig->type_ముసుగు))
			కొనసాగు;
		ఒకవేళ (!dbtr_trigger_hw_supported(trig->సూచీ_2, tdata1,
					       tdata2, tdata3))
			కొనసాగు;
		తిరిగి i;
	}

	తిరిగి SBI_ENOENT;
}

స్థిర అంతఃవరుస నిర్మాణం sbi_dbtr_trigger *sbi_alloc_trigger(గుర్తులేని దీర్ఘ tdata1,
							 గుర్తులేని దీర్ఘ tdata2,
							 గుర్తులేని దీర్ఘ tdata3)
{
	పూర్ణాంకం i;
	నిర్మాణం sbi_dbtr_trigger *f_trig;
	నిర్మాణం sbi_dbtr_hart_triggers_state *hart_state;

	hart_state = dbtr_thishart_state_ptr();
	ఒకవేళ (!hart_state)
		తిరిగి ఖాళీ;

	ఒకవేళ (hart_state->అందుబాటు_trigs <= 0)
		తిరిగి ఖాళీ;

	i = dbtr_కనుగొను_free_స్థానం(hart_state, 0, tdata1, tdata2, tdata3);
	ఒకవేళ (i < 0)
		తిరిగి ఖాళీ;

	f_trig = సూచీ_గమ్యం_TRIGGER(i);
	hart_state->అందుబాటు_trigs--;
	__set_bit(RV_DBTR_బిట్టు(TS, MAPPED), &f_trig->state);

	తిరిగి f_trig;
}

స్థిర అంతఃవరుస శూన్య sbi_free_trigger(నిర్మాణం sbi_dbtr_trigger *trig)
{
	నిర్మాణం sbi_dbtr_hart_triggers_state *hart_state;

	ఒకవేళ (trig == ఖాళీ)
		తిరిగి;

	hart_state = dbtr_thishart_state_ptr();
	ఒకవేళ (!hart_state)
		తిరిగి;

	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;

	hart_state->అందుబాటు_trigs++;
}

పూర్ణాంకం sbi_dbtr_init(నిర్మాణం sbi_scratch *scratch, bool coldboot)
{
	నిర్మాణం sbi_trap_info trap = {0};
	గుర్తులేని దీర్ఘ tdata1;
	గుర్తులేని దీర్ఘ val;
	పూర్ణాంకం i;
	నిర్మాణం sbi_dbtr_hart_triggers_state *hart_state = ఖాళీ;

	ఒకవేళ (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		తిరిగి SBI_SUCCESS;

	ఒకవేళ (coldboot) {
		hart_state_ptr_స్థానభ్రంశం = sbi_scratch_alloc_type_స్థానభ్రంశం(శూన్య *);
		ఒకవేళ (!hart_state_ptr_స్థానభ్రంశం)
			తిరిగి SBI_ENOMEM;
	}

	hart_state = dbtr_get_hart_state_ptr(scratch);
	ఒకవేళ (!hart_state) {
		hart_state = sbi_zalloc(పరిమాణం(*hart_state));
		ఒకవేళ (!hart_state)
			తిరిగి SBI_ENOMEM;
		hart_state->hartid = current_hartid();
		dbtr_సమితి_hart_state_ptr(scratch, hart_state);
	}

	/* disable the shared memory */
	sbi_dbtr_disable_shmem(hart_state);

	/* Skip probing triggers if already probed */
	ఒకవేళ (hart_state->probed)
		వెళ్ళు _probed;

	కొరకు (i = 0; i < RV_MAX_TRIGGERS; i++) {
		csr_write_allowed(CSR_TSELECT, &trap, i);
		ఒకవేళ (trap.cause)
			ఆపు;

		val = csr_read_allowed(CSR_TSELECT, &trap);
		ఒకవేళ (trap.cause)
			ఆపు;

		/*
		 * Read back tselect and check that it contains the
		 * written value
		 */
		ఒకవేళ (val != i)
			ఆపు;

		val = csr_read_allowed(CSR_TINFO, &trap);
		ఒకవేళ (trap.cause) {
			/*
			 * If reading tinfo caused an exception, the
			 * debugger must read tdata1 to discover the
			 * type.
			 */
			tdata1 = csr_read_allowed(CSR_TDATA1,
						  &trap);
			ఒకవేళ (trap.cause)
				ఆపు;

			ఒకవేళ (TDATA1_GET_TYPE(tdata1) == 0)
				ఆపు;

			sbi_trigger_init(సూచీ_గమ్యం_TRIGGER(i),
					 బిట్టు(TDATA1_GET_TYPE(tdata1)),
					 i);
			hart_state->మొత్తం_trigs++;
		} లేకపోతే {
			ఒకవేళ (val == 1)
				ఆపు;

			sbi_trigger_init(సూచీ_గమ్యం_TRIGGER(i), val, i);
			hart_state->మొత్తం_trigs++;
		}
	}

	hart_state->probed = 1;

 _probed:
	hart_state->అందుబాటు_trigs = hart_state->మొత్తం_trigs;

	తిరిగి SBI_SUCCESS;
}

పూర్ణాంకం sbi_dbtr_get_మొత్తం_triggers(శూన్య)
{
	నిర్మాణం sbi_dbtr_hart_triggers_state *hs;
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/*
	 * This function may be used during ecall registration.
	 * By that time the debug trigger module might not be
	 * initialized. If the extension is not supported, report
	 * number of triggers as 0.
	 */
	ఒకవేళ (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		తిరిగి 0;

	hs = dbtr_thishart_state_ptr();
	ఒకవేళ (!hs)
		తిరిగి 0;

	తిరిగి hs->మొత్తం_trigs;
}

పూర్ణాంకం sbi_dbtr_అమరిక_shmem(స్థిరరాశి నిర్మాణం sbi_domain *dom, గుర్తులేని దీర్ఘ smode,
			 గుర్తులేని దీర్ఘ shmem_phys_lo,
			 గుర్తులేని దీర్ఘ shmem_phys_hi)
{
	నిర్మాణం sbi_dbtr_hart_triggers_state *hart_state;

	ఒకవేళ (dom && !sbi_domain_is_assigned_hart(dom, current_hartindex())) {
		sbi_dprintf("%s: calling hart not assigned to this domain\n",
			   __func__);
		తిరిగి SBI_ERR_DENIED;
	}

	hart_state = dbtr_thishart_state_ptr();
	ఒకవేళ (!hart_state)
		తిరిగి SBI_ERR_FAILED;

	/* call is to disable shared memory */
	ఒకవేళ (shmem_phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR
	    && shmem_phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR) {
		sbi_dbtr_disable_shmem(hart_state);
		తిరిగి SBI_SUCCESS;
	}

	/* the shared memory must be disabled on this hart */
	ఒకవేళ (!sbi_dbtr_shmem_disabled(hart_state))
		తిరిగి SBI_ERR_ALREADY_అందుబాటు;

	/* lower physical address must be XLEN/8 bytes aligned */
	ఒకవేళ (shmem_phys_lo & SBI_DBTR_SHMEM_ALIGN_ముసుగు)
		తిరిగి SBI_ERR_INVALID_PARAM;

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
	ఒకవేళ (shmem_phys_hi)
		తిరిగి SBI_EINVALID_ADDR;

	ఒకవేళ (dom && !sbi_domain_check_addr(dom,
		  DBTR_SHMEM_MAKE_PHYS(shmem_phys_hi, shmem_phys_lo), smode,
		  SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		తిరిగి SBI_ERR_INVALID_చిరునామా;

	hart_state->shmem.phys_lo = shmem_phys_lo;
	hart_state->shmem.phys_hi = shmem_phys_hi;

	తిరిగి SBI_SUCCESS;
}

స్థిర శూన్య dbtr_trigger_అమరిక(నిర్మాణం sbi_dbtr_trigger *trig,
			       నిర్మాణం sbi_dbtr_దత్తాంశం_సందేశం *recv)
{
	గుర్తులేని దీర్ఘ tdata1;

	ఒకవేళ (!trig)
		తిరిగి;

	trig->tdata1 = lle_గమ్యం_cpu(recv->tdata1);
	trig->tdata2 = lle_గమ్యం_cpu(recv->tdata2);
	trig->tdata3 = lle_గమ్యం_cpu(recv->tdata3);

	tdata1 = lle_గమ్యం_cpu(recv->tdata1);

	trig->state = 0;

	__set_bit(RV_DBTR_బిట్టు(TS, MAPPED), &trig->state);

	సమితి_TRIG_HW_సూచీ(trig->state, trig->సూచీ_2);

	ఎంపిక (TDATA1_GET_TYPE(tdata1)) {
	స్థితి RISCV_DBTR_TRIG_MCONTROL:
		ఒకవేళ (__test_bit(RV_DBTR_బిట్టు(MC, U), &tdata1))
			__set_bit(RV_DBTR_బిట్టు(TS, U), &trig->state);

		ఒకవేళ (__test_bit(RV_DBTR_బిట్టు(MC, S), &tdata1))
			__set_bit(RV_DBTR_బిట్టు(TS, S), &trig->state);
		ఆపు;
	స్థితి RISCV_DBTR_TRIG_MCONTROL6:
		ఒకవేళ (__test_bit(RV_DBTR_బిట్టు(MC6, U), &tdata1))
			__set_bit(RV_DBTR_బిట్టు(TS, U), &trig->state);

		ఒకవేళ (__test_bit(RV_DBTR_బిట్టు(MC6, S), &tdata1))
			__set_bit(RV_DBTR_బిట్టు(TS, S), &trig->state);

		ఒకవేళ (__test_bit(RV_DBTR_బిట్టు(MC6, VU), &tdata1))
			__set_bit(RV_DBTR_బిట్టు(TS, VU), &trig->state);

		ఒకవేళ (__test_bit(RV_DBTR_బిట్టు(MC6, VS), &tdata1))
			__set_bit(RV_DBTR_బిట్టు(TS, VS), &trig->state);
		ఆపు;
	స్థితి RISCV_DBTR_TRIG_ICOUNT:
		ఒకవేళ (__test_bit(RV_DBTR_బిట్టు(ICOUNT, U), &tdata1))
			__set_bit(RV_DBTR_బిట్టు(TS, U), &trig->state);

		ఒకవేళ (__test_bit(RV_DBTR_బిట్టు(ICOUNT, S), &tdata1))
			__set_bit(RV_DBTR_బిట్టు(TS, S), &trig->state);

		ఒకవేళ (__test_bit(RV_DBTR_బిట్టు(ICOUNT, VU), &tdata1))
			__set_bit(RV_DBTR_బిట్టు(TS, VU), &trig->state);

		ఒకవేళ (__test_bit(RV_DBTR_బిట్టు(ICOUNT, VS), &tdata1))
			__set_bit(RV_DBTR_బిట్టు(TS, VS), &trig->state);
		ఆపు;
	అప్రమేయ:
		sbi_dprintf("%s: Unknown type (tdata1: 0x%lx Type: %ld)\n",
			    __func__, tdata1, TDATA1_GET_TYPE(tdata1));
		ఆపు;
	}
}

స్థిర అంతఃవరుస శూన్య update_బిట్టు(గుర్తులేని దీర్ఘ new, పూర్ణాంకం nr, అస్థిర గుర్తులేని దీర్ఘ *addr)
{
	ఒకవేళ (new)
		__set_bit(nr, addr);
	లేకపోతే
		__clear_bit(nr, addr);
}

స్థిర శూన్య dbtr_trigger_enable(నిర్మాణం sbi_dbtr_trigger *trig)
{
	గుర్తులేని దీర్ఘ state;
	గుర్తులేని దీర్ఘ tdata1;

	ఒకవేళ (!trig || !(trig->state & RV_DBTR_బిట్టు_ముసుగు(TS, MAPPED)))
		తిరిగి;

	state = trig->state;
	tdata1 = trig->tdata1;

	ఎంపిక (TDATA1_GET_TYPE(tdata1)) {
	స్థితి RISCV_DBTR_TRIG_MCONTROL:
		update_బిట్టు(state & RV_DBTR_బిట్టు_ముసుగు(TS, U),
			   RV_DBTR_బిట్టు(MC, U), &trig->tdata1);
		update_బిట్టు(state & RV_DBTR_బిట్టు_ముసుగు(TS, S),
			   RV_DBTR_బిట్టు(MC, S), &trig->tdata1);
		ఆపు;
	స్థితి RISCV_DBTR_TRIG_MCONTROL6:
		update_బిట్టు(state & RV_DBTR_బిట్టు_ముసుగు(TS, VU),
			   RV_DBTR_బిట్టు(MC6, VU), &trig->tdata1);
		update_బిట్టు(state & RV_DBTR_బిట్టు_ముసుగు(TS, VS),
			   RV_DBTR_బిట్టు(MC6, VS), &trig->tdata1);
		update_బిట్టు(state & RV_DBTR_బిట్టు_ముసుగు(TS, U),
			   RV_DBTR_బిట్టు(MC6, U), &trig->tdata1);
		update_బిట్టు(state & RV_DBTR_బిట్టు_ముసుగు(TS, S),
			   RV_DBTR_బిట్టు(MC6, S), &trig->tdata1);
		ఆపు;
	స్థితి RISCV_DBTR_TRIG_ICOUNT:
		update_బిట్టు(state & RV_DBTR_బిట్టు_ముసుగు(TS, VU),
			   RV_DBTR_బిట్టు(ICOUNT, VU), &trig->tdata1);
		update_బిట్టు(state & RV_DBTR_బిట్టు_ముసుగు(TS, VS),
			   RV_DBTR_బిట్టు(ICOUNT, VS), &trig->tdata1);
		update_బిట్టు(state & RV_DBTR_బిట్టు_ముసుగు(TS, U),
			   RV_DBTR_బిట్టు(ICOUNT, U), &trig->tdata1);
		update_బిట్టు(state & RV_DBTR_బిట్టు_ముసుగు(TS, S),
			   RV_DBTR_బిట్టు(ICOUNT, S), &trig->tdata1);
		ఆపు;
	అప్రమేయ:
		ఆపు;
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
	csr_write(CSR_TSELECT, trig->సూచీ_2);
	csr_write(CSR_TDATA1, 0x0);
	tdata_write_సురక్షిత(CSR_TDATA2, trig->tdata2);
	tdata_write_సురక్షిత(CSR_TDATA3, trig->tdata3);
	csr_write(CSR_TDATA1, trig->tdata1);
}

స్థిర శూన్య dbtr_trigger_disable(నిర్మాణం sbi_dbtr_trigger *trig)
{
	గుర్తులేని దీర్ఘ tdata1;

	ఒకవేళ (!trig || !(trig->state & RV_DBTR_బిట్టు_ముసుగు(TS, MAPPED)))
		తిరిగి;

	tdata1 = trig->tdata1;

	ఎంపిక (TDATA1_GET_TYPE(tdata1)) {
	స్థితి RISCV_DBTR_TRIG_MCONTROL:
		__clear_bit(RV_DBTR_బిట్టు(MC, U), &trig->tdata1);
		__clear_bit(RV_DBTR_బిట్టు(MC, S), &trig->tdata1);
		ఆపు;
	స్థితి RISCV_DBTR_TRIG_MCONTROL6:
		__clear_bit(RV_DBTR_బిట్టు(MC6, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_బిట్టు(MC6, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_బిట్టు(MC6, U), &trig->tdata1);
		__clear_bit(RV_DBTR_బిట్టు(MC6, S), &trig->tdata1);
		ఆపు;
	స్థితి RISCV_DBTR_TRIG_ICOUNT:
		__clear_bit(RV_DBTR_బిట్టు(ICOUNT, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_బిట్టు(ICOUNT, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_బిట్టు(ICOUNT, U), &trig->tdata1);
		__clear_bit(RV_DBTR_బిట్టు(ICOUNT, S), &trig->tdata1);
		ఆపు;
	అప్రమేయ:
		ఆపు;
	}

	csr_write(CSR_TSELECT, trig->సూచీ_2);
	csr_write(CSR_TDATA1, trig->tdata1);
}

స్థిర శూన్య dbtr_trigger_clear(నిర్మాణం sbi_dbtr_trigger *trig)
{
	ఒకవేళ (!trig || !(trig->state & RV_DBTR_బిట్టు_ముసుగు(TS, MAPPED)))
		తిరిగి;

	csr_write(CSR_TSELECT, trig->సూచీ_2);
	csr_write(CSR_TDATA1, 0x0);
	tdata_write_సురక్షిత(CSR_TDATA2, 0x0);
	tdata_write_సురక్షిత(CSR_TDATA3, 0x0);
}

స్థిర పూర్ణాంకం dbtr_trigger_supported(గుర్తులేని దీర్ఘ type)
{
	ఎంపిక (type) {
	స్థితి RISCV_DBTR_TRIG_MCONTROL:
	స్థితి RISCV_DBTR_TRIG_MCONTROL6:
	స్థితి RISCV_DBTR_TRIG_ICOUNT:
		తిరిగి 1;
	అప్రమేయ:
		ఆపు;
	}

	తిరిగి 0;
}

స్థిర పూర్ణాంకం dbtr_trigger_valid(గుర్తులేని దీర్ఘ type, గుర్తులేని దీర్ఘ tdata)
{
	ఎంపిక (type) {
	స్థితి RISCV_DBTR_TRIG_MCONTROL:
		ఒకవేళ (!(tdata & RV_DBTR_బిట్టు_ముసుగు(MC, DMODE)) &&
		    !(tdata & RV_DBTR_బిట్టు_ముసుగు(MC, M)))
			తిరిగి 1;
		ఆపు;
	స్థితి RISCV_DBTR_TRIG_MCONTROL6:
		ఒకవేళ (!(tdata & RV_DBTR_బిట్టు_ముసుగు(MC6, DMODE)) &&
		    !(tdata & RV_DBTR_బిట్టు_ముసుగు(MC6, M)))
			తిరిగి 1;
		ఆపు;
	స్థితి RISCV_DBTR_TRIG_ICOUNT:
		ఒకవేళ (!(tdata & RV_DBTR_బిట్టు_ముసుగు(ICOUNT, DMODE)) &&
		    !(tdata & RV_DBTR_బిట్టు_ముసుగు(ICOUNT, M)))
			తిరిగి 1;
		ఆపు;
	అప్రమేయ:
		ఆపు;
	}

	తిరిగి 0;
}

పూర్ణాంకం sbi_dbtr_అంకె_trig(గుర్తులేని దీర్ఘ దత్తాంశం, గుర్తులేని దీర్ఘ *బహిర్గతం)
{
	గుర్తులేని దీర్ఘ type = TDATA1_GET_TYPE(దత్తాంశం);
	u32 hartid = current_hartid();
	గుర్తులేని దీర్ఘ మొత్తం = 0;
	నిర్మాణం sbi_dbtr_trigger *trig;
	పూర్ణాంకం i;
	నిర్మాణం sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	ఒకవేళ (!hs)
		తిరిగి SBI_ERR_FAILED;

	ఒకవేళ (దత్తాంశం == 0) {
		*బహిర్గతం = hs->మొత్తం_trigs;
		తిరిగి SBI_SUCCESS;
	}

	కొరకు (i = 0; i < hs->మొత్తం_trigs; i++) {
		trig = సూచీ_గమ్యం_TRIGGER(i);

		ఒకవేళ (__test_bit(type, &trig->type_ముసుగు) &&
		    dbtr_trigger_hw_supported(trig->సూచీ_2, దత్తాంశం, 0, 0))
			మొత్తం++;
	}

	sbi_dprintf("%s: hart%d: total triggers of type %lu: %lu\n",
		    __func__, hartid, type, మొత్తం);

	*బహిర్గతం = మొత్తం;
	తిరిగి SBI_SUCCESS;
}

పూర్ణాంకం sbi_dbtr_read_trig(గుర్తులేని దీర్ఘ smode,
		       గుర్తులేని దీర్ఘ trig_idx_పునాది, గుర్తులేని దీర్ఘ trig_లెక్క)
{
	నిర్మాణం sbi_dbtr_దత్తాంశం_సందేశం *xmit;
	నిర్మాణం sbi_dbtr_trigger *trig;
	సంఘం sbi_dbtr_shmem_నమోదు *నమోదు_3;
	శూన్య *shmem_పునాది = ఖాళీ;
	నిర్మాణం sbi_dbtr_hart_triggers_state *hs = ఖాళీ;

	hs = dbtr_thishart_state_ptr();
	ఒకవేళ (!hs)
		తిరిగి SBI_ERR_FAILED;

	ఒకవేళ (trig_idx_పునాది >= hs->మొత్తం_trigs ||
	    trig_idx_పునాది + trig_లెక్క >= hs->మొత్తం_trigs)
		తిరిగి SBI_ERR_BAD_RANGE;

	ఒకవేళ (sbi_dbtr_shmem_disabled(hs))
		తిరిగి SBI_ERR_NO_SHMEM;

	shmem_పునాది = hart_shmem_పునాది(hs);

	sbi_hart_protection_తాత్కాలిక_map_range((గుర్తులేని దీర్ఘ)shmem_పునాది,
					   trig_లెక్క * పరిమాణం(*నమోదు_3));
	for_each_trig_నమోదు(shmem_పునాది, trig_లెక్క, typeof(*నమోదు_3), నమోదు_3) {
		xmit = &నమోదు_3->దత్తాంశం;
		trig = సూచీ_గమ్యం_TRIGGER((_idx + trig_idx_పునాది));
		csr_write(CSR_TSELECT, trig->సూచీ_2);
		trig->tdata1 = csr_read(CSR_TDATA1);
		trig->tdata2 = tdata_read_సురక్షిత(CSR_TDATA2);
		trig->tdata3 = tdata_read_సురక్షిత(CSR_TDATA3);
		xmit->tstate = cpu_గమ్యం_lle(trig->state);
		xmit->tdata1 = cpu_గమ్యం_lle(trig->tdata1);
		xmit->tdata2 = cpu_గమ్యం_lle(trig->tdata2);
		xmit->tdata3 = cpu_గమ్యం_lle(trig->tdata3);
	}
	sbi_hart_protection_తాత్కాలిక_unmap_range((గుర్తులేని దీర్ఘ)shmem_పునాది,
					     trig_లెక్క * పరిమాణం(*నమోదు_3));

	తిరిగి SBI_SUCCESS;
}

పూర్ణాంకం sbi_dbtr_install_trig(గుర్తులేని దీర్ఘ smode,
			  గుర్తులేని దీర్ఘ trig_లెక్క, గుర్తులేని దీర్ఘ *బహిర్గతం)
{
	శూన్య *shmem_పునాది = ఖాళీ;
	సంఘం sbi_dbtr_shmem_నమోదు *నమోదు_3;
	నిర్మాణం sbi_dbtr_దత్తాంశం_సందేశం *recv;
	నిర్మాణం sbi_dbtr_id_సందేశం *xmit;
	గుర్తులేని దీర్ఘ ctrl;
	u32 claimed = 0;
	పూర్ణాంకం స్థానం;
	నిర్మాణం sbi_dbtr_trigger *trig;
	నిర్మాణం sbi_dbtr_hart_triggers_state *hs = ఖాళీ;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	ఒకవేళ (!hs)
		తిరిగి SBI_ERR_FAILED;

	ఒకవేళ (sbi_dbtr_shmem_disabled(hs))
		తిరిగి SBI_ERR_NO_SHMEM;

	shmem_పునాది = hart_shmem_పునాది(hs);
	sbi_hart_protection_తాత్కాలిక_map_range((గుర్తులేని దీర్ఘ)shmem_పునాది,
					   trig_లెక్క * పరిమాణం(*నమోదు_3));

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
	for_each_trig_నమోదు(shmem_పునాది, trig_లెక్క, typeof(*నమోదు_3), నమోదు_3) {
		recv = (నిర్మాణం sbi_dbtr_దత్తాంశం_సందేశం *)(&నమోదు_3->దత్తాంశం);
		ctrl = recv->tdata1;

		ఒకవేళ (!dbtr_trigger_supported(TDATA1_GET_TYPE(ctrl))) {
			*బహిర్గతం = _idx;
			sbi_hart_protection_తాత్కాలిక_unmap_range((గుర్తులేని దీర్ఘ)shmem_పునాది,
							     trig_లెక్క * పరిమాణం(*నమోదు_3));
			తిరిగి SBI_ERR_FAILED;
		}

		ఒకవేళ (!dbtr_trigger_valid(TDATA1_GET_TYPE(ctrl), ctrl)) {
			*బహిర్గతం = _idx;
			sbi_hart_protection_తాత్కాలిక_unmap_range((గుర్తులేని దీర్ఘ)shmem_పునాది,
							     trig_లెక్క * పరిమాణం(*నమోదు_3));
			తిరిగి SBI_ERR_INVALID_PARAM;
		}

		ఒకవేళ ((recv->tdata2 && !tdata2_impl) ||
		    (recv->tdata3 && !tdata3_impl)) {
			*బహిర్గతం = _idx;
			sbi_hart_protection_తాత్కాలిక_unmap_range((గుర్తులేని దీర్ఘ)shmem_పునాది,
							     trig_లెక్క * పరిమాణం(*నమోదు_3));
			తిరిగి SBI_ERR_NOT_SUPPORTED;
		}

		ఒకవేళ (!dbtr_trigger_any_hw_supported(hs,
						   lle_గమ్యం_cpu(recv->tdata1),
						   lle_గమ్యం_cpu(recv->tdata2),
						   lle_గమ్యం_cpu(recv->tdata3))) {
			*బహిర్గతం = _idx;
			sbi_hart_protection_తాత్కాలిక_unmap_range((గుర్తులేని దీర్ఘ)shmem_పునాది,
							     trig_లెక్క * పరిమాణం(*నమోదు_3));
			తిరిగి SBI_ERR_NOT_SUPPORTED;
		}
	}

	ఒకవేళ (hs->అందుబాటు_trigs < trig_లెక్క) {
		*బహిర్గతం = hs->అందుబాటు_trigs;
		sbi_hart_protection_తాత్కాలిక_unmap_range((గుర్తులేని దీర్ఘ)shmem_పునాది,
						     trig_లెక్క * పరిమాణం(*నమోదు_3));
		తిరిగి SBI_ERR_FAILED;
	}

	/*
	 * Dry-run the allocation of the whole batch so that no trigger
	 * is installed if any of the requested configurations cannot be
	 * matched to a free hardware trigger slot.
	 */
	for_each_trig_నమోదు(shmem_పునాది, trig_లెక్క, typeof(*నమోదు_3), నమోదు_3) {
		recv = (నిర్మాణం sbi_dbtr_దత్తాంశం_సందేశం *)(&నమోదు_3->దత్తాంశం);
		స్థానం = dbtr_కనుగొను_free_స్థానం(hs, claimed,
					   lle_గమ్యం_cpu(recv->tdata1),
					   lle_గమ్యం_cpu(recv->tdata2),
					   lle_గమ్యం_cpu(recv->tdata3));
		ఒకవేళ (స్థానం < 0) {
			*బహిర్గతం = _idx;
			sbi_hart_protection_తాత్కాలిక_unmap_range((గుర్తులేని దీర్ఘ)shmem_పునాది,
							     trig_లెక్క * పరిమాణం(*నమోదు_3));
			తిరిగి SBI_ERR_FAILED;
		}
		claimed |= బిట్టు(స్థానం);
	}

	/* Install triggers */
	for_each_trig_నమోదు(shmem_పునాది, trig_లెక్క, typeof(*నమోదు_3), నమోదు_3) {
		recv = (నిర్మాణం sbi_dbtr_దత్తాంశం_సందేశం *)(&నమోదు_3->దత్తాంశం);
		xmit = (నిర్మాణం sbi_dbtr_id_సందేశం *)(&నమోదు_3->id);

		/*
		 * The dry-run above matched every requested configuration
		 * to a free hardware trigger slot, so allocation must
		 * succeed.
		 */
		trig = sbi_alloc_trigger(lle_గమ్యం_cpu(recv->tdata1),
					 lle_గమ్యం_cpu(recv->tdata2),
					 lle_గమ్యం_cpu(recv->tdata3));

		dbtr_trigger_అమరిక(trig,  recv);
		dbtr_trigger_enable(trig);
		xmit->idx = cpu_గమ్యం_lle(trig->సూచీ_2);

	}

	sbi_hart_protection_తాత్కాలిక_unmap_range((గుర్తులేని దీర్ఘ)shmem_పునాది,
					     trig_లెక్క * పరిమాణం(*నమోదు_3));

	తిరిగి SBI_SUCCESS;
}

పూర్ణాంకం sbi_dbtr_uninstall_trig(గుర్తులేని దీర్ఘ trig_idx_పునాది,
			    గుర్తులేని దీర్ఘ trig_idx_ముసుగు)
{
	గుర్తులేని దీర్ఘ trig_ముసుగు = trig_idx_ముసుగు << trig_idx_పునాది;
	గుర్తులేని దీర్ఘ idx = trig_idx_పునాది;
	నిర్మాణం sbi_dbtr_trigger *trig;
	నిర్మాణం sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	ఒకవేళ (!hs)
		తిరిగి SBI_ERR_FAILED;

	for_each_సమితి_బిట్టు_from(idx, &trig_ముసుగు, hs->మొత్తం_trigs) {
		trig = సూచీ_గమ్యం_TRIGGER(idx);
		ఒకవేళ (!(trig->state & RV_DBTR_బిట్టు_ముసుగు(TS, MAPPED)))
			తిరిగి SBI_ERR_INVALID_PARAM;

		dbtr_trigger_clear(trig);

		sbi_free_trigger(trig);
	}

	తిరిగి SBI_SUCCESS;
}

పూర్ణాంకం sbi_dbtr_enable_trig(గుర్తులేని దీర్ఘ trig_idx_పునాది,
			 గుర్తులేని దీర్ఘ trig_idx_ముసుగు)
{
	గుర్తులేని దీర్ఘ trig_ముసుగు = trig_idx_ముసుగు << trig_idx_పునాది;
	గుర్తులేని దీర్ఘ idx = trig_idx_పునాది;
	నిర్మాణం sbi_dbtr_trigger *trig;
	నిర్మాణం sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	ఒకవేళ (!hs)
		తిరిగి SBI_ERR_FAILED;

	for_each_సమితి_బిట్టు_from(idx, &trig_ముసుగు, hs->మొత్తం_trigs) {
		trig = సూచీ_గమ్యం_TRIGGER(idx);
		sbi_dprintf("%s: enable trigger %lu\n", __func__, idx);
		dbtr_trigger_enable(trig);
	}

	తిరిగి SBI_SUCCESS;
}

పూర్ణాంకం sbi_dbtr_update_trig(గుర్తులేని దీర్ఘ smode,
			 గుర్తులేని దీర్ఘ trig_లెక్క)
{
	గుర్తులేని దీర్ఘ trig_idx;
	నిర్మాణం sbi_dbtr_trigger *trig;
	సంఘం sbi_dbtr_shmem_నమోదు *నమోదు_3;
	శూన్య *shmem_పునాది = ఖాళీ;
	నిర్మాణం sbi_dbtr_hart_triggers_state *hs = ఖాళీ;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	ఒకవేళ (!hs)
		తిరిగి SBI_ERR_FAILED;

	ఒకవేళ (sbi_dbtr_shmem_disabled(hs))
		తిరిగి SBI_ERR_NO_SHMEM;

	shmem_పునాది = hart_shmem_పునాది(hs);

	ఒకవేళ (trig_లెక్క >= hs->మొత్తం_trigs)
		తిరిగి SBI_ERR_BAD_RANGE;

	/*
	 * SBI v3.0 sec 19.5 requires SBI_ERR_NOT_SUPPORTED when a trigger
	 * programs a non-zero value into an unimplemented optional CSR. Only
	 * the "whole CSR unimplemented" case is caught; WARL bits tied off
	 * inside an otherwise-implemented CSR are not.
	 */
	tdata2_impl = tdata_implemented(CSR_TDATA2);
	tdata3_impl = tdata_implemented(CSR_TDATA3);

	for_each_trig_నమోదు(shmem_పునాది, trig_లెక్క, typeof(*నమోదు_3), నమోదు_3) {
		sbi_hart_protection_తాత్కాలిక_map_range((గుర్తులేని దీర్ఘ)నమోదు_3, పరిమాణం(*నమోదు_3));
		trig_idx = నమోదు_3->id.idx;

		ఒకవేళ (trig_idx >= hs->మొత్తం_trigs) {
			sbi_hart_protection_తాత్కాలిక_unmap_range((గుర్తులేని దీర్ఘ)నమోదు_3, పరిమాణం(*నమోదు_3));
			తిరిగి SBI_ERR_INVALID_PARAM;
		}

		trig = సూచీ_గమ్యం_TRIGGER(trig_idx);

		ఒకవేళ (!(trig->state & RV_DBTR_బిట్టు_ముసుగు(TS, MAPPED))) {
			sbi_hart_protection_తాత్కాలిక_unmap_range((గుర్తులేని దీర్ఘ)నమోదు_3, పరిమాణం(*నమోదు_3));
			తిరిగి SBI_ERR_FAILED;
		}

		ఒకవేళ ((నమోదు_3->దత్తాంశం.tdata2 && !tdata2_impl) ||
		    (నమోదు_3->దత్తాంశం.tdata3 && !tdata3_impl)) {
			sbi_hart_protection_తాత్కాలిక_unmap_range((గుర్తులేని దీర్ఘ)నమోదు_3, పరిమాణం(*నమోదు_3));
			తిరిగి SBI_ERR_NOT_SUPPORTED;
		}

		ఒకవేళ (!dbtr_trigger_hw_supported(trig->సూచీ_2,
					       lle_గమ్యం_cpu(నమోదు_3->దత్తాంశం.tdata1),
					       lle_గమ్యం_cpu(నమోదు_3->దత్తాంశం.tdata2),
					       lle_గమ్యం_cpu(నమోదు_3->దత్తాంశం.tdata3))) {
			sbi_hart_protection_తాత్కాలిక_unmap_range((గుర్తులేని దీర్ఘ)నమోదు_3, పరిమాణం(*నమోదు_3));
			తిరిగి SBI_ERR_NOT_SUPPORTED;
		}

		dbtr_trigger_అమరిక(trig, &నమోదు_3->దత్తాంశం);
		sbi_hart_protection_తాత్కాలిక_unmap_range((గుర్తులేని దీర్ఘ)నమోదు_3, పరిమాణం(*నమోదు_3));
		dbtr_trigger_enable(trig);
	}

	తిరిగి SBI_SUCCESS;
}

పూర్ణాంకం sbi_dbtr_disable_trig(గుర్తులేని దీర్ఘ trig_idx_పునాది,
			  గుర్తులేని దీర్ఘ trig_idx_ముసుగు)
{
	గుర్తులేని దీర్ఘ trig_ముసుగు = trig_idx_ముసుగు << trig_idx_పునాది;
	గుర్తులేని దీర్ఘ idx = trig_idx_పునాది;
	నిర్మాణం sbi_dbtr_trigger *trig;
	నిర్మాణం sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	ఒకవేళ (!hs)
		తిరిగి SBI_ERR_FAILED;

	for_each_సమితి_బిట్టు_from(idx, &trig_ముసుగు, hs->మొత్తం_trigs) {
		trig = సూచీ_గమ్యం_TRIGGER(idx);
		dbtr_trigger_disable(trig);
	}

	తిరిగి SBI_SUCCESS;
}
