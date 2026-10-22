/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems, Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_csr_detect.h>
#अन्तर्भाव <sbi/sbi_platform.h>
#अन्तर्भाव <sbi/sbi_byteorder.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_trap.h>
#अन्तर्भाव <sbi/sbi_dbtr.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_hart_protection.h>
#अन्तर्भाव <sbi/riscv_encoding.h>
#अन्तर्भाव <sbi/riscv_asm.h>


/** Offset of pointer to HART's debug triggers info in scratch space */
स्थिर अचिह्नित दीर्घ hart_state_ptr_विस्थापनम्;

/** Device specific debug trigger operations */
स्थिर ध्रुव संरचना sbi_dbtr_device *dbtr_dev = रिक्त;

ध्रुव संरचना sbi_dbtr_device *sbi_dbtr_get_device(शून्य)
{
	प्रत्यागम dbtr_dev;
}

शून्य sbi_dbtr_समुच्चयः_device(ध्रुव संरचना sbi_dbtr_device *dev)
{
	यदि (!dev || dbtr_dev)
		प्रत्यागम;

	dbtr_dev = dev;
}

#परिभाषय dbtr_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), शून्य *, hart_state_ptr_विस्थापनम्)

#परिभाषय dbtr_thishart_state_ptr()				\
	dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#परिभाषय dbtr_समुच्चयः_hart_state_ptr(__scratch, __hart_state)		\
	sbi_scratch_write_type((__scratch), शून्य *, hart_state_ptr_विस्थापनम्, \
			       (__hart_state))

#परिभाषय tdata_read_safe(__csr)						\
	({								\
		संरचना sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
	})

#परिभाषय tdata_write_safe(__csr, __value)				\
	({								\
		संरचना sbi_trap_info __trap = {0};			\
		csr_write_allowed((__csr), &__trap, (__value));		\
	})

#परिभाषय tdata_implemented(__csr)					\
	({								\
		संरचना sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
		!__trap.cause;						\
	})

#परिभाषय सूचकांक_गन्तव्यम्_TRIGGER(सूचकांक)					\
	({								\
		संरचना sbi_dbtr_trigger *__trg = रिक्त;			\
		संरचना sbi_dbtr_hart_triggers_state *__hs = रिक्त;	\
		__hs = dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr()); \
		__trg = &__hs->triggers[सूचकांक];			\
		(__trg);						\
	})

#परिभाषय for_each_trig_प्रविष्टिः(आधारः, _max, _etype, प्रविष्टिः)		\
	प्रति (पूर्णाङ्क _idx = 0; प्रविष्टिः = ((_etype *)आधारः + _idx),		\
	     _idx < _max;						\
	     _idx++, प्रविष्टिः = ((_etype *)आधारः + _idx))

#परिभाषय DBTR_SHMEM_MAKE_PHYS(_p_hi, _p_lo) (_p_lo)

/* must call with hs != NULL */
स्थिर अन्तःस्थ bool sbi_dbtr_shmem_disabled(
	संरचना sbi_dbtr_hart_triggers_state *hs)
{
	प्रत्यागम (hs->shmem.phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR &&
		hs->shmem.phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR
		? true : false);
}

/* must call with hs != NULL */
स्थिर अन्तःस्थ शून्य sbi_dbtr_disable_shmem(
	संरचना sbi_dbtr_hart_triggers_state *hs)
{
	hs->shmem.phys_lo = SBI_DBTR_SHMEM_INVALID_ADDR;
	hs->shmem.phys_hi = SBI_DBTR_SHMEM_INVALID_ADDR;
}

/* must call with hs which is not disabled */
स्थिर अन्तःस्थ शून्य *hart_shmem_आधारः(
	संरचना sbi_dbtr_hart_triggers_state *hs)
{
	प्रत्यागम ((शून्य *)(अचिह्नित दीर्घ)DBTR_SHMEM_MAKE_PHYS(
			hs->shmem.phys_hi, hs->shmem.phys_lo));
}

स्थिर शून्य sbi_trigger_init(संरचना sbi_dbtr_trigger *trig,
			     अचिह्नित दीर्घ type_आवरणम्, अचिह्नित दीर्घ idx)
{
	trig->type_आवरणम् = type_आवरणम्;
	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;
	trig->सूचकांक_2 = idx;
}

स्थिर bool dbtr_trigger_hw_supported(अचिह्नित दीर्घ idx, अचिह्नित दीर्घ tdata1,
				      अचिह्नित दीर्घ tdata2,
				      अचिह्नित दीर्घ tdata3)
{
	यदि (dbtr_dev && dbtr_dev->trigger_supported)
		प्रत्यागम dbtr_dev->trigger_supported(idx, tdata1, tdata2,
						   tdata3);

	प्रत्यागम true;
}

स्थिर bool dbtr_trigger_any_hw_supported(
			संरचना sbi_dbtr_hart_triggers_state *hs,
			अचिह्नित दीर्घ tdata1, अचिह्नित दीर्घ tdata2,
			अचिह्नित दीर्घ tdata3)
{
	अचिह्नित दीर्घ type = TDATA1_GET_TYPE(tdata1);
	संरचना sbi_dbtr_trigger *trig;
	पूर्णाङ्क i;

	प्रति (i = 0; i < hs->योग_trigs; i++) {
		trig = सूचकांक_गन्तव्यम्_TRIGGER(i);
		यदि (__test_bit(type, &trig->type_आवरणम्) &&
		    dbtr_trigger_hw_supported(trig->सूचकांक_2, tdata1, tdata2,
					      tdata3))
			प्रत्यागम true;
	}

	प्रत्यागम false;
}

/*
 * Find the first free hardware trigger slot supporting the configuration.
 * Slots set in claimed_mask are treated as taken, which allows the caller
 * to track slot availability. A 32-bit mask covers RV_MAX_TRIGGERS (32);
 * for a larger number of triggers, this function needs to be updated.
 */
स्थिर पूर्णाङ्क dbtr_find_free_slot(संरचना sbi_dbtr_hart_triggers_state *hs,
			       u32 claimed_आवरणम्,
			       अचिह्नित दीर्घ tdata1, अचिह्नित दीर्घ tdata2,
			       अचिह्नित दीर्घ tdata3)
{
	अचिह्नित दीर्घ type = TDATA1_GET_TYPE(tdata1);
	संरचना sbi_dbtr_trigger *trig;
	पूर्णाङ्क i;

	प्रति (i = 0; i < hs->योग_trigs; i++) {
		trig = सूचकांक_गन्तव्यम्_TRIGGER(i);
		यदि (trig->state & RV_DBTR_BIT_आवरणम्(TS, MAPPED))
			अनुवर्त;
		यदि (claimed_आवरणम् & BIT(i))
			अनुवर्त;
		यदि (!__test_bit(type, &trig->type_आवरणम्))
			अनुवर्त;
		यदि (!dbtr_trigger_hw_supported(trig->सूचकांक_2, tdata1,
					       tdata2, tdata3))
			अनुवर्त;
		प्रत्यागम i;
	}

	प्रत्यागम SBI_ENOENT;
}

स्थिर अन्तःस्थ संरचना sbi_dbtr_trigger *sbi_alloc_trigger(अचिह्नित दीर्घ tdata1,
							 अचिह्नित दीर्घ tdata2,
							 अचिह्नित दीर्घ tdata3)
{
	पूर्णाङ्क i;
	संरचना sbi_dbtr_trigger *f_trig;
	संरचना sbi_dbtr_hart_triggers_state *hart_state;

	hart_state = dbtr_thishart_state_ptr();
	यदि (!hart_state)
		प्रत्यागम रिक्त;

	यदि (hart_state->उपलब्धम्_trigs <= 0)
		प्रत्यागम रिक्त;

	i = dbtr_find_free_slot(hart_state, 0, tdata1, tdata2, tdata3);
	यदि (i < 0)
		प्रत्यागम रिक्त;

	f_trig = सूचकांक_गन्तव्यम्_TRIGGER(i);
	hart_state->उपलब्धम्_trigs--;
	__set_bit(RV_DBTR_BIT(TS, MAPPED), &f_trig->state);

	प्रत्यागम f_trig;
}

स्थिर अन्तःस्थ शून्य sbi_free_trigger(संरचना sbi_dbtr_trigger *trig)
{
	संरचना sbi_dbtr_hart_triggers_state *hart_state;

	यदि (trig == रिक्त)
		प्रत्यागम;

	hart_state = dbtr_thishart_state_ptr();
	यदि (!hart_state)
		प्रत्यागम;

	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;

	hart_state->उपलब्धम्_trigs++;
}

पूर्णाङ्क sbi_dbtr_init(संरचना sbi_scratch *scratch, bool coldboot)
{
	संरचना sbi_trap_info trap = {0};
	अचिह्नित दीर्घ tdata1;
	अचिह्नित दीर्घ val;
	पूर्णाङ्क i;
	संरचना sbi_dbtr_hart_triggers_state *hart_state = रिक्त;

	यदि (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		प्रत्यागम SBI_SUCCESS;

	यदि (coldboot) {
		hart_state_ptr_विस्थापनम् = sbi_scratch_alloc_type_विस्थापनम्(शून्य *);
		यदि (!hart_state_ptr_विस्थापनम्)
			प्रत्यागम SBI_ENOMEM;
	}

	hart_state = dbtr_get_hart_state_ptr(scratch);
	यदि (!hart_state) {
		hart_state = sbi_zalloc(परिमाण(*hart_state));
		यदि (!hart_state)
			प्रत्यागम SBI_ENOMEM;
		hart_state->hartid = current_hartid();
		dbtr_समुच्चयः_hart_state_ptr(scratch, hart_state);
	}

	/* disable the shared memory */
	sbi_dbtr_disable_shmem(hart_state);

	/* Skip probing triggers if already probed */
	यदि (hart_state->probed)
		गच्छ _probed;

	प्रति (i = 0; i < RV_MAX_TRIGGERS; i++) {
		csr_write_allowed(CSR_TSELECT, &trap, i);
		यदि (trap.cause)
			भङ्ग;

		val = csr_read_allowed(CSR_TSELECT, &trap);
		यदि (trap.cause)
			भङ्ग;

		/*
		 * Read back tselect and check that it contains the
		 * written value
		 */
		यदि (val != i)
			भङ्ग;

		val = csr_read_allowed(CSR_TINFO, &trap);
		यदि (trap.cause) {
			/*
			 * If reading tinfo caused an exception, the
			 * debugger must read tdata1 to discover the
			 * type.
			 */
			tdata1 = csr_read_allowed(CSR_TDATA1,
						  &trap);
			यदि (trap.cause)
				भङ्ग;

			यदि (TDATA1_GET_TYPE(tdata1) == 0)
				भङ्ग;

			sbi_trigger_init(सूचकांक_गन्तव्यम्_TRIGGER(i),
					 BIT(TDATA1_GET_TYPE(tdata1)),
					 i);
			hart_state->योग_trigs++;
		} अन्यथा {
			यदि (val == 1)
				भङ्ग;

			sbi_trigger_init(सूचकांक_गन्तव्यम्_TRIGGER(i), val, i);
			hart_state->योग_trigs++;
		}
	}

	hart_state->probed = 1;

 _probed:
	hart_state->उपलब्धम्_trigs = hart_state->योग_trigs;

	प्रत्यागम SBI_SUCCESS;
}

पूर्णाङ्क sbi_dbtr_get_योग_triggers(शून्य)
{
	संरचना sbi_dbtr_hart_triggers_state *hs;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/*
	 * This function may be used during ecall registration.
	 * By that time the debug trigger module might not be
	 * initialized. If the extension is not supported, report
	 * number of triggers as 0.
	 */
	यदि (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		प्रत्यागम 0;

	hs = dbtr_thishart_state_ptr();
	यदि (!hs)
		प्रत्यागम 0;

	प्रत्यागम hs->योग_trigs;
}

पूर्णाङ्क sbi_dbtr_सज्जता_shmem(ध्रुव संरचना sbi_domain *dom, अचिह्नित दीर्घ smode,
			 अचिह्नित दीर्घ shmem_phys_lo,
			 अचिह्नित दीर्घ shmem_phys_hi)
{
	संरचना sbi_dbtr_hart_triggers_state *hart_state;

	यदि (dom && !sbi_domain_is_assigned_hart(dom, current_hartindex())) {
		sbi_dprintf("%s: calling hart not assigned to this domain\n",
			   __func__);
		प्रत्यागम SBI_ERR_DENIED;
	}

	hart_state = dbtr_thishart_state_ptr();
	यदि (!hart_state)
		प्रत्यागम SBI_ERR_FAILED;

	/* call is to disable shared memory */
	यदि (shmem_phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR
	    && shmem_phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR) {
		sbi_dbtr_disable_shmem(hart_state);
		प्रत्यागम SBI_SUCCESS;
	}

	/* the shared memory must be disabled on this hart */
	यदि (!sbi_dbtr_shmem_disabled(hart_state))
		प्रत्यागम SBI_ERR_ALREADY_उपलब्धम्;

	/* lower physical address must be XLEN/8 bytes aligned */
	यदि (shmem_phys_lo & SBI_DBTR_SHMEM_ALIGN_आवरणम्)
		प्रत्यागम SBI_ERR_INVALID_PARAM;

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
	यदि (shmem_phys_hi)
		प्रत्यागम SBI_EINVALID_ADDR;

	यदि (dom && !sbi_domain_check_addr(dom,
		  DBTR_SHMEM_MAKE_PHYS(shmem_phys_hi, shmem_phys_lo), smode,
		  SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		प्रत्यागम SBI_ERR_INVALID_स्थानाङ्कः;

	hart_state->shmem.phys_lo = shmem_phys_lo;
	hart_state->shmem.phys_hi = shmem_phys_hi;

	प्रत्यागम SBI_SUCCESS;
}

स्थिर शून्य dbtr_trigger_सज्जता(संरचना sbi_dbtr_trigger *trig,
			       संरचना sbi_dbtr_दत्त_सन्देशः *recv)
{
	अचिह्नित दीर्घ tdata1;

	यदि (!trig)
		प्रत्यागम;

	trig->tdata1 = lle_गन्तव्यम्_cpu(recv->tdata1);
	trig->tdata2 = lle_गन्तव्यम्_cpu(recv->tdata2);
	trig->tdata3 = lle_गन्तव्यम्_cpu(recv->tdata3);

	tdata1 = lle_गन्तव्यम्_cpu(recv->tdata1);

	trig->state = 0;

	__set_bit(RV_DBTR_BIT(TS, MAPPED), &trig->state);

	समुच्चयः_TRIG_HW_सूचकांक(trig->state, trig->सूचकांक_2);

	विकल्प (TDATA1_GET_TYPE(tdata1)) {
	पक्ष RISCV_DBTR_TRIG_MCONTROL:
		यदि (__test_bit(RV_DBTR_BIT(MC, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		यदि (__test_bit(RV_DBTR_BIT(MC, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);
		भङ्ग;
	पक्ष RISCV_DBTR_TRIG_MCONTROL6:
		यदि (__test_bit(RV_DBTR_BIT(MC6, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		यदि (__test_bit(RV_DBTR_BIT(MC6, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);

		यदि (__test_bit(RV_DBTR_BIT(MC6, VU), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VU), &trig->state);

		यदि (__test_bit(RV_DBTR_BIT(MC6, VS), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VS), &trig->state);
		भङ्ग;
	पक्ष RISCV_DBTR_TRIG_ICOUNT:
		यदि (__test_bit(RV_DBTR_BIT(ICOUNT, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		यदि (__test_bit(RV_DBTR_BIT(ICOUNT, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);

		यदि (__test_bit(RV_DBTR_BIT(ICOUNT, VU), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VU), &trig->state);

		यदि (__test_bit(RV_DBTR_BIT(ICOUNT, VS), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VS), &trig->state);
		भङ्ग;
	स्वभाव:
		sbi_dprintf("%s: Unknown type (tdata1: 0x%lx Type: %ld)\n",
			    __func__, tdata1, TDATA1_GET_TYPE(tdata1));
		भङ्ग;
	}
}

स्थिर अन्तःस्थ शून्य update_bit(अचिह्नित दीर्घ new, पूर्णाङ्क nr, चञ्चल अचिह्नित दीर्घ *addr)
{
	यदि (new)
		__set_bit(nr, addr);
	अन्यथा
		__clear_bit(nr, addr);
}

स्थिर शून्य dbtr_trigger_enable(संरचना sbi_dbtr_trigger *trig)
{
	अचिह्नित दीर्घ state;
	अचिह्नित दीर्घ tdata1;

	यदि (!trig || !(trig->state & RV_DBTR_BIT_आवरणम्(TS, MAPPED)))
		प्रत्यागम;

	state = trig->state;
	tdata1 = trig->tdata1;

	विकल्प (TDATA1_GET_TYPE(tdata1)) {
	पक्ष RISCV_DBTR_TRIG_MCONTROL:
		update_bit(state & RV_DBTR_BIT_आवरणम्(TS, U),
			   RV_DBTR_BIT(MC, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_आवरणम्(TS, S),
			   RV_DBTR_BIT(MC, S), &trig->tdata1);
		भङ्ग;
	पक्ष RISCV_DBTR_TRIG_MCONTROL6:
		update_bit(state & RV_DBTR_BIT_आवरणम्(TS, VU),
			   RV_DBTR_BIT(MC6, VU), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_आवरणम्(TS, VS),
			   RV_DBTR_BIT(MC6, VS), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_आवरणम्(TS, U),
			   RV_DBTR_BIT(MC6, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_आवरणम्(TS, S),
			   RV_DBTR_BIT(MC6, S), &trig->tdata1);
		भङ्ग;
	पक्ष RISCV_DBTR_TRIG_ICOUNT:
		update_bit(state & RV_DBTR_BIT_आवरणम्(TS, VU),
			   RV_DBTR_BIT(ICOUNT, VU), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_आवरणम्(TS, VS),
			   RV_DBTR_BIT(ICOUNT, VS), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_आवरणम्(TS, U),
			   RV_DBTR_BIT(ICOUNT, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_आवरणम्(TS, S),
			   RV_DBTR_BIT(ICOUNT, S), &trig->tdata1);
		भङ्ग;
	स्वभाव:
		भङ्ग;
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
	csr_write(CSR_TSELECT, trig->सूचकांक_2);
	csr_write(CSR_TDATA1, 0x0);
	tdata_write_safe(CSR_TDATA2, trig->tdata2);
	tdata_write_safe(CSR_TDATA3, trig->tdata3);
	csr_write(CSR_TDATA1, trig->tdata1);
}

स्थिर शून्य dbtr_trigger_disable(संरचना sbi_dbtr_trigger *trig)
{
	अचिह्नित दीर्घ tdata1;

	यदि (!trig || !(trig->state & RV_DBTR_BIT_आवरणम्(TS, MAPPED)))
		प्रत्यागम;

	tdata1 = trig->tdata1;

	विकल्प (TDATA1_GET_TYPE(tdata1)) {
	पक्ष RISCV_DBTR_TRIG_MCONTROL:
		__clear_bit(RV_DBTR_BIT(MC, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC, S), &trig->tdata1);
		भङ्ग;
	पक्ष RISCV_DBTR_TRIG_MCONTROL6:
		__clear_bit(RV_DBTR_BIT(MC6, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, S), &trig->tdata1);
		भङ्ग;
	पक्ष RISCV_DBTR_TRIG_ICOUNT:
		__clear_bit(RV_DBTR_BIT(ICOUNT, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, S), &trig->tdata1);
		भङ्ग;
	स्वभाव:
		भङ्ग;
	}

	csr_write(CSR_TSELECT, trig->सूचकांक_2);
	csr_write(CSR_TDATA1, trig->tdata1);
}

स्थिर शून्य dbtr_trigger_clear(संरचना sbi_dbtr_trigger *trig)
{
	यदि (!trig || !(trig->state & RV_DBTR_BIT_आवरणम्(TS, MAPPED)))
		प्रत्यागम;

	csr_write(CSR_TSELECT, trig->सूचकांक_2);
	csr_write(CSR_TDATA1, 0x0);
	tdata_write_safe(CSR_TDATA2, 0x0);
	tdata_write_safe(CSR_TDATA3, 0x0);
}

स्थिर पूर्णाङ्क dbtr_trigger_supported(अचिह्नित दीर्घ type)
{
	विकल्प (type) {
	पक्ष RISCV_DBTR_TRIG_MCONTROL:
	पक्ष RISCV_DBTR_TRIG_MCONTROL6:
	पक्ष RISCV_DBTR_TRIG_ICOUNT:
		प्रत्यागम 1;
	स्वभाव:
		भङ्ग;
	}

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क dbtr_trigger_valid(अचिह्नित दीर्घ type, अचिह्नित दीर्घ tdata)
{
	विकल्प (type) {
	पक्ष RISCV_DBTR_TRIG_MCONTROL:
		यदि (!(tdata & RV_DBTR_BIT_आवरणम्(MC, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_आवरणम्(MC, M)))
			प्रत्यागम 1;
		भङ्ग;
	पक्ष RISCV_DBTR_TRIG_MCONTROL6:
		यदि (!(tdata & RV_DBTR_BIT_आवरणम्(MC6, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_आवरणम्(MC6, M)))
			प्रत्यागम 1;
		भङ्ग;
	पक्ष RISCV_DBTR_TRIG_ICOUNT:
		यदि (!(tdata & RV_DBTR_BIT_आवरणम्(ICOUNT, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_आवरणम्(ICOUNT, M)))
			प्रत्यागम 1;
		भङ्ग;
	स्वभाव:
		भङ्ग;
	}

	प्रत्यागम 0;
}

पूर्णाङ्क sbi_dbtr_अङ्कः_trig(अचिह्नित दीर्घ दत्त, अचिह्नित दीर्घ *बहिर्गमनम्)
{
	अचिह्नित दीर्घ type = TDATA1_GET_TYPE(दत्त);
	u32 hartid = current_hartid();
	अचिह्नित दीर्घ योग = 0;
	संरचना sbi_dbtr_trigger *trig;
	पूर्णाङ्क i;
	संरचना sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	यदि (!hs)
		प्रत्यागम SBI_ERR_FAILED;

	यदि (दत्त == 0) {
		*बहिर्गमनम् = hs->योग_trigs;
		प्रत्यागम SBI_SUCCESS;
	}

	प्रति (i = 0; i < hs->योग_trigs; i++) {
		trig = सूचकांक_गन्तव्यम्_TRIGGER(i);

		यदि (__test_bit(type, &trig->type_आवरणम्) &&
		    dbtr_trigger_hw_supported(trig->सूचकांक_2, दत्त, 0, 0))
			योग++;
	}

	sbi_dprintf("%s: hart%d: total triggers of type %lu: %lu\n",
		    __func__, hartid, type, योग);

	*बहिर्गमनम् = योग;
	प्रत्यागम SBI_SUCCESS;
}

पूर्णाङ्क sbi_dbtr_read_trig(अचिह्नित दीर्घ smode,
		       अचिह्नित दीर्घ trig_idx_आधारः, अचिह्नित दीर्घ trig_गणनम्)
{
	संरचना sbi_dbtr_दत्त_सन्देशः *xmit;
	संरचना sbi_dbtr_trigger *trig;
	सङ्घ sbi_dbtr_shmem_प्रविष्टिः *प्रविष्टिः_2;
	शून्य *shmem_आधारः = रिक्त;
	संरचना sbi_dbtr_hart_triggers_state *hs = रिक्त;

	hs = dbtr_thishart_state_ptr();
	यदि (!hs)
		प्रत्यागम SBI_ERR_FAILED;

	यदि (trig_idx_आधारः >= hs->योग_trigs ||
	    trig_idx_आधारः + trig_गणनम् >= hs->योग_trigs)
		प्रत्यागम SBI_ERR_BAD_RANGE;

	यदि (sbi_dbtr_shmem_disabled(hs))
		प्रत्यागम SBI_ERR_NO_SHMEM;

	shmem_आधारः = hart_shmem_आधारः(hs);

	sbi_hart_protection_अस्थायी_map_range((अचिह्नित दीर्घ)shmem_आधारः,
					   trig_गणनम् * परिमाण(*प्रविष्टिः_2));
	for_each_trig_प्रविष्टिः(shmem_आधारः, trig_गणनम्, typeof(*प्रविष्टिः_2), प्रविष्टिः_2) {
		xmit = &प्रविष्टिः_2->दत्त;
		trig = सूचकांक_गन्तव्यम्_TRIGGER((_idx + trig_idx_आधारः));
		csr_write(CSR_TSELECT, trig->सूचकांक_2);
		trig->tdata1 = csr_read(CSR_TDATA1);
		trig->tdata2 = tdata_read_safe(CSR_TDATA2);
		trig->tdata3 = tdata_read_safe(CSR_TDATA3);
		xmit->tstate = cpu_गन्तव्यम्_lle(trig->state);
		xmit->tdata1 = cpu_गन्तव्यम्_lle(trig->tdata1);
		xmit->tdata2 = cpu_गन्तव्यम्_lle(trig->tdata2);
		xmit->tdata3 = cpu_गन्तव्यम्_lle(trig->tdata3);
	}
	sbi_hart_protection_अस्थायी_unmap_range((अचिह्नित दीर्घ)shmem_आधारः,
					     trig_गणनम् * परिमाण(*प्रविष्टिः_2));

	प्रत्यागम SBI_SUCCESS;
}

पूर्णाङ्क sbi_dbtr_install_trig(अचिह्नित दीर्घ smode,
			  अचिह्नित दीर्घ trig_गणनम्, अचिह्नित दीर्घ *बहिर्गमनम्)
{
	शून्य *shmem_आधारः = रिक्त;
	सङ्घ sbi_dbtr_shmem_प्रविष्टिः *प्रविष्टिः_2;
	संरचना sbi_dbtr_दत्त_सन्देशः *recv;
	संरचना sbi_dbtr_id_सन्देशः *xmit;
	अचिह्नित दीर्घ ctrl;
	u32 claimed = 0;
	पूर्णाङ्क slot;
	संरचना sbi_dbtr_trigger *trig;
	संरचना sbi_dbtr_hart_triggers_state *hs = रिक्त;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	यदि (!hs)
		प्रत्यागम SBI_ERR_FAILED;

	यदि (sbi_dbtr_shmem_disabled(hs))
		प्रत्यागम SBI_ERR_NO_SHMEM;

	shmem_आधारः = hart_shmem_आधारः(hs);
	sbi_hart_protection_अस्थायी_map_range((अचिह्नित दीर्घ)shmem_आधारः,
					   trig_गणनम् * परिमाण(*प्रविष्टिः_2));

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
	for_each_trig_प्रविष्टिः(shmem_आधारः, trig_गणनम्, typeof(*प्रविष्टिः_2), प्रविष्टिः_2) {
		recv = (संरचना sbi_dbtr_दत्त_सन्देशः *)(&प्रविष्टिः_2->दत्त);
		ctrl = recv->tdata1;

		यदि (!dbtr_trigger_supported(TDATA1_GET_TYPE(ctrl))) {
			*बहिर्गमनम् = _idx;
			sbi_hart_protection_अस्थायी_unmap_range((अचिह्नित दीर्घ)shmem_आधारः,
							     trig_गणनम् * परिमाण(*प्रविष्टिः_2));
			प्रत्यागम SBI_ERR_FAILED;
		}

		यदि (!dbtr_trigger_valid(TDATA1_GET_TYPE(ctrl), ctrl)) {
			*बहिर्गमनम् = _idx;
			sbi_hart_protection_अस्थायी_unmap_range((अचिह्नित दीर्घ)shmem_आधारः,
							     trig_गणनम् * परिमाण(*प्रविष्टिः_2));
			प्रत्यागम SBI_ERR_INVALID_PARAM;
		}

		यदि ((recv->tdata2 && !tdata2_impl) ||
		    (recv->tdata3 && !tdata3_impl)) {
			*बहिर्गमनम् = _idx;
			sbi_hart_protection_अस्थायी_unmap_range((अचिह्नित दीर्घ)shmem_आधारः,
							     trig_गणनम् * परिमाण(*प्रविष्टिः_2));
			प्रत्यागम SBI_ERR_NOT_SUPPORTED;
		}

		यदि (!dbtr_trigger_any_hw_supported(hs,
						   lle_गन्तव्यम्_cpu(recv->tdata1),
						   lle_गन्तव्यम्_cpu(recv->tdata2),
						   lle_गन्तव्यम्_cpu(recv->tdata3))) {
			*बहिर्गमनम् = _idx;
			sbi_hart_protection_अस्थायी_unmap_range((अचिह्नित दीर्घ)shmem_आधारः,
							     trig_गणनम् * परिमाण(*प्रविष्टिः_2));
			प्रत्यागम SBI_ERR_NOT_SUPPORTED;
		}
	}

	यदि (hs->उपलब्धम्_trigs < trig_गणनम्) {
		*बहिर्गमनम् = hs->उपलब्धम्_trigs;
		sbi_hart_protection_अस्थायी_unmap_range((अचिह्नित दीर्घ)shmem_आधारः,
						     trig_गणनम् * परिमाण(*प्रविष्टिः_2));
		प्रत्यागम SBI_ERR_FAILED;
	}

	/*
	 * Dry-run the allocation of the whole batch so that no trigger
	 * is installed if any of the requested configurations cannot be
	 * matched to a free hardware trigger slot.
	 */
	for_each_trig_प्रविष्टिः(shmem_आधारः, trig_गणनम्, typeof(*प्रविष्टिः_2), प्रविष्टिः_2) {
		recv = (संरचना sbi_dbtr_दत्त_सन्देशः *)(&प्रविष्टिः_2->दत्त);
		slot = dbtr_find_free_slot(hs, claimed,
					   lle_गन्तव्यम्_cpu(recv->tdata1),
					   lle_गन्तव्यम्_cpu(recv->tdata2),
					   lle_गन्तव्यम्_cpu(recv->tdata3));
		यदि (slot < 0) {
			*बहिर्गमनम् = _idx;
			sbi_hart_protection_अस्थायी_unmap_range((अचिह्नित दीर्घ)shmem_आधारः,
							     trig_गणनम् * परिमाण(*प्रविष्टिः_2));
			प्रत्यागम SBI_ERR_FAILED;
		}
		claimed |= BIT(slot);
	}

	/* Install triggers */
	for_each_trig_प्रविष्टिः(shmem_आधारः, trig_गणनम्, typeof(*प्रविष्टिः_2), प्रविष्टिः_2) {
		recv = (संरचना sbi_dbtr_दत्त_सन्देशः *)(&प्रविष्टिः_2->दत्त);
		xmit = (संरचना sbi_dbtr_id_सन्देशः *)(&प्रविष्टिः_2->id);

		/*
		 * The dry-run above matched every requested configuration
		 * to a free hardware trigger slot, so allocation must
		 * succeed.
		 */
		trig = sbi_alloc_trigger(lle_गन्तव्यम्_cpu(recv->tdata1),
					 lle_गन्तव्यम्_cpu(recv->tdata2),
					 lle_गन्तव्यम्_cpu(recv->tdata3));

		dbtr_trigger_सज्जता(trig,  recv);
		dbtr_trigger_enable(trig);
		xmit->idx = cpu_गन्तव्यम्_lle(trig->सूचकांक_2);

	}

	sbi_hart_protection_अस्थायी_unmap_range((अचिह्नित दीर्घ)shmem_आधारः,
					     trig_गणनम् * परिमाण(*प्रविष्टिः_2));

	प्रत्यागम SBI_SUCCESS;
}

पूर्णाङ्क sbi_dbtr_uninstall_trig(अचिह्नित दीर्घ trig_idx_आधारः,
			    अचिह्नित दीर्घ trig_idx_आवरणम्)
{
	अचिह्नित दीर्घ trig_आवरणम् = trig_idx_आवरणम् << trig_idx_आधारः;
	अचिह्नित दीर्घ idx = trig_idx_आधारः;
	संरचना sbi_dbtr_trigger *trig;
	संरचना sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	यदि (!hs)
		प्रत्यागम SBI_ERR_FAILED;

	for_each_समुच्चयः_bit_from(idx, &trig_आवरणम्, hs->योग_trigs) {
		trig = सूचकांक_गन्तव्यम्_TRIGGER(idx);
		यदि (!(trig->state & RV_DBTR_BIT_आवरणम्(TS, MAPPED)))
			प्रत्यागम SBI_ERR_INVALID_PARAM;

		dbtr_trigger_clear(trig);

		sbi_free_trigger(trig);
	}

	प्रत्यागम SBI_SUCCESS;
}

पूर्णाङ्क sbi_dbtr_enable_trig(अचिह्नित दीर्घ trig_idx_आधारः,
			 अचिह्नित दीर्घ trig_idx_आवरणम्)
{
	अचिह्नित दीर्घ trig_आवरणम् = trig_idx_आवरणम् << trig_idx_आधारः;
	अचिह्नित दीर्घ idx = trig_idx_आधारः;
	संरचना sbi_dbtr_trigger *trig;
	संरचना sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	यदि (!hs)
		प्रत्यागम SBI_ERR_FAILED;

	for_each_समुच्चयः_bit_from(idx, &trig_आवरणम्, hs->योग_trigs) {
		trig = सूचकांक_गन्तव्यम्_TRIGGER(idx);
		sbi_dprintf("%s: enable trigger %lu\n", __func__, idx);
		dbtr_trigger_enable(trig);
	}

	प्रत्यागम SBI_SUCCESS;
}

पूर्णाङ्क sbi_dbtr_update_trig(अचिह्नित दीर्घ smode,
			 अचिह्नित दीर्घ trig_गणनम्)
{
	अचिह्नित दीर्घ trig_idx;
	संरचना sbi_dbtr_trigger *trig;
	सङ्घ sbi_dbtr_shmem_प्रविष्टिः *प्रविष्टिः_2;
	शून्य *shmem_आधारः = रिक्त;
	संरचना sbi_dbtr_hart_triggers_state *hs = रिक्त;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	यदि (!hs)
		प्रत्यागम SBI_ERR_FAILED;

	यदि (sbi_dbtr_shmem_disabled(hs))
		प्रत्यागम SBI_ERR_NO_SHMEM;

	shmem_आधारः = hart_shmem_आधारः(hs);

	यदि (trig_गणनम् >= hs->योग_trigs)
		प्रत्यागम SBI_ERR_BAD_RANGE;

	/*
	 * SBI v3.0 sec 19.5 requires SBI_ERR_NOT_SUPPORTED when a trigger
	 * programs a non-zero value into an unimplemented optional CSR. Only
	 * the "whole CSR unimplemented" case is caught; WARL bits tied off
	 * inside an otherwise-implemented CSR are not.
	 */
	tdata2_impl = tdata_implemented(CSR_TDATA2);
	tdata3_impl = tdata_implemented(CSR_TDATA3);

	for_each_trig_प्रविष्टिः(shmem_आधारः, trig_गणनम्, typeof(*प्रविष्टिः_2), प्रविष्टिः_2) {
		sbi_hart_protection_अस्थायी_map_range((अचिह्नित दीर्घ)प्रविष्टिः_2, परिमाण(*प्रविष्टिः_2));
		trig_idx = प्रविष्टिः_2->id.idx;

		यदि (trig_idx >= hs->योग_trigs) {
			sbi_hart_protection_अस्थायी_unmap_range((अचिह्नित दीर्घ)प्रविष्टिः_2, परिमाण(*प्रविष्टिः_2));
			प्रत्यागम SBI_ERR_INVALID_PARAM;
		}

		trig = सूचकांक_गन्तव्यम्_TRIGGER(trig_idx);

		यदि (!(trig->state & RV_DBTR_BIT_आवरणम्(TS, MAPPED))) {
			sbi_hart_protection_अस्थायी_unmap_range((अचिह्नित दीर्घ)प्रविष्टिः_2, परिमाण(*प्रविष्टिः_2));
			प्रत्यागम SBI_ERR_FAILED;
		}

		यदि ((प्रविष्टिः_2->दत्त.tdata2 && !tdata2_impl) ||
		    (प्रविष्टिः_2->दत्त.tdata3 && !tdata3_impl)) {
			sbi_hart_protection_अस्थायी_unmap_range((अचिह्नित दीर्घ)प्रविष्टिः_2, परिमाण(*प्रविष्टिः_2));
			प्रत्यागम SBI_ERR_NOT_SUPPORTED;
		}

		यदि (!dbtr_trigger_hw_supported(trig->सूचकांक_2,
					       lle_गन्तव्यम्_cpu(प्रविष्टिः_2->दत्त.tdata1),
					       lle_गन्तव्यम्_cpu(प्रविष्टिः_2->दत्त.tdata2),
					       lle_गन्तव्यम्_cpu(प्रविष्टिः_2->दत्त.tdata3))) {
			sbi_hart_protection_अस्थायी_unmap_range((अचिह्नित दीर्घ)प्रविष्टिः_2, परिमाण(*प्रविष्टिः_2));
			प्रत्यागम SBI_ERR_NOT_SUPPORTED;
		}

		dbtr_trigger_सज्जता(trig, &प्रविष्टिः_2->दत्त);
		sbi_hart_protection_अस्थायी_unmap_range((अचिह्नित दीर्घ)प्रविष्टिः_2, परिमाण(*प्रविष्टिः_2));
		dbtr_trigger_enable(trig);
	}

	प्रत्यागम SBI_SUCCESS;
}

पूर्णाङ्क sbi_dbtr_disable_trig(अचिह्नित दीर्घ trig_idx_आधारः,
			  अचिह्नित दीर्घ trig_idx_आवरणम्)
{
	अचिह्नित दीर्घ trig_आवरणम् = trig_idx_आवरणम् << trig_idx_आधारः;
	अचिह्नित दीर्घ idx = trig_idx_आधारः;
	संरचना sbi_dbtr_trigger *trig;
	संरचना sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	यदि (!hs)
		प्रत्यागम SBI_ERR_FAILED;

	for_each_समुच्चयः_bit_from(idx, &trig_आवरणम्, hs->योग_trigs) {
		trig = सूचकांक_गन्तव्यम्_TRIGGER(idx);
		dbtr_trigger_disable(trig);
	}

	प्रत्यागम SBI_SUCCESS;
}
