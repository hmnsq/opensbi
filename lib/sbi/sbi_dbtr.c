/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems, Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_csr_detect.h>
#समावेश <sbi/sbi_platform.h>
#समावेश <sbi/sbi_byteorder.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_trap.h>
#समावेश <sbi/sbi_dbtr.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi/sbi_hart_protection.h>
#समावेश <sbi/riscv_encoding.h>
#समावेश <sbi/riscv_asm.h>


/** Offset of pointer to HART's debug triggers info in scratch space */
अटल अचिन्हित दीर्घ hart_state_ptr_offset;

/** Device specific debug trigger operations */
अटल स्थिर काष्ठा sbi_dbtr_device *dbtr_dev = शून्य;

स्थिर काष्ठा sbi_dbtr_device *sbi_dbtr_get_device(व्योम)
{
	वापस dbtr_dev;
}

व्योम sbi_dbtr_set_device(स्थिर काष्ठा sbi_dbtr_device *dev)
{
	अगर (!dev || dbtr_dev)
		वापस;

	dbtr_dev = dev;
}

#घोषणा dbtr_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), व्योम *, hart_state_ptr_offset)

#घोषणा dbtr_thishart_state_ptr()				\
	dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#घोषणा dbtr_set_hart_state_ptr(__scratch, __hart_state)		\
	sbi_scratch_write_type((__scratch), व्योम *, hart_state_ptr_offset, \
			       (__hart_state))

#घोषणा tdata_read_safe(__csr)						\
	({								\
		काष्ठा sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
	})

#घोषणा tdata_write_safe(__csr, __value)				\
	({								\
		काष्ठा sbi_trap_info __trap = {0};			\
		csr_write_allowed((__csr), &__trap, (__value));		\
	})

#घोषणा tdata_implemented(__csr)					\
	({								\
		काष्ठा sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
		!__trap.cause;						\
	})

#घोषणा सूचकांक_TO_TRIGGER(सूचकांक)					\
	({								\
		काष्ठा sbi_dbtr_trigger *__trg = शून्य;			\
		काष्ठा sbi_dbtr_hart_triggers_state *__hs = शून्य;	\
		__hs = dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr()); \
		__trg = &__hs->triggers[सूचकांक];			\
		(__trg);						\
	})

#घोषणा for_each_trig_entry(_base, _max, _etype, _entry)		\
	क्रम (पूर्णांक _idx = 0; _entry = ((_etype *)_base + _idx),		\
	     _idx < _max;						\
	     _idx++, _entry = ((_etype *)_base + _idx))

#घोषणा DBTR_SHMEM_MAKE_PHYS(_p_hi, _p_lo) (_p_lo)

/* must call with hs != NULL */
अटल अंतरभूत bool sbi_dbtr_shmem_disabled(
	काष्ठा sbi_dbtr_hart_triggers_state *hs)
{
	वापस (hs->shmem.phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR &&
		hs->shmem.phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR
		? true : false);
}

/* must call with hs != NULL */
अटल अंतरभूत व्योम sbi_dbtr_disable_shmem(
	काष्ठा sbi_dbtr_hart_triggers_state *hs)
{
	hs->shmem.phys_lo = SBI_DBTR_SHMEM_INVALID_ADDR;
	hs->shmem.phys_hi = SBI_DBTR_SHMEM_INVALID_ADDR;
}

/* must call with hs which is not disabled */
अटल अंतरभूत व्योम *hart_shmem_base(
	काष्ठा sbi_dbtr_hart_triggers_state *hs)
{
	वापस ((व्योम *)(अचिन्हित दीर्घ)DBTR_SHMEM_MAKE_PHYS(
			hs->shmem.phys_hi, hs->shmem.phys_lo));
}

अटल व्योम sbi_trigger_init(काष्ठा sbi_dbtr_trigger *trig,
			     अचिन्हित दीर्घ type_mask, अचिन्हित दीर्घ idx)
{
	trig->type_mask = type_mask;
	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;
	trig->सूचकांक_2 = idx;
}

अटल bool dbtr_trigger_hw_supported(अचिन्हित दीर्घ idx, अचिन्हित दीर्घ tdata1,
				      अचिन्हित दीर्घ tdata2,
				      अचिन्हित दीर्घ tdata3)
{
	अगर (dbtr_dev && dbtr_dev->trigger_supported)
		वापस dbtr_dev->trigger_supported(idx, tdata1, tdata2,
						   tdata3);

	वापस true;
}

अटल bool dbtr_trigger_any_hw_supported(
			काष्ठा sbi_dbtr_hart_triggers_state *hs,
			अचिन्हित दीर्घ tdata1, अचिन्हित दीर्घ tdata2,
			अचिन्हित दीर्घ tdata3)
{
	अचिन्हित दीर्घ type = TDATA1_GET_TYPE(tdata1);
	काष्ठा sbi_dbtr_trigger *trig;
	पूर्णांक i;

	क्रम (i = 0; i < hs->योग_trigs; i++) {
		trig = सूचकांक_TO_TRIGGER(i);
		अगर (__test_bit(type, &trig->type_mask) &&
		    dbtr_trigger_hw_supported(trig->सूचकांक_2, tdata1, tdata2,
					      tdata3))
			वापस true;
	}

	वापस false;
}

/*
 * Find the first free hardware trigger slot supporting the configuration.
 * Slots set in claimed_mask are treated as taken, which allows the caller
 * to track slot availability. A 32-bit mask covers RV_MAX_TRIGGERS (32);
 * for a larger number of triggers, this function needs to be updated.
 */
अटल पूर्णांक dbtr_find_free_slot(काष्ठा sbi_dbtr_hart_triggers_state *hs,
			       u32 claimed_mask,
			       अचिन्हित दीर्घ tdata1, अचिन्हित दीर्घ tdata2,
			       अचिन्हित दीर्घ tdata3)
{
	अचिन्हित दीर्घ type = TDATA1_GET_TYPE(tdata1);
	काष्ठा sbi_dbtr_trigger *trig;
	पूर्णांक i;

	क्रम (i = 0; i < hs->योग_trigs; i++) {
		trig = सूचकांक_TO_TRIGGER(i);
		अगर (trig->state & RV_DBTR_BIT_MASK(TS, MAPPED))
			जारी;
		अगर (claimed_mask & BIT(i))
			जारी;
		अगर (!__test_bit(type, &trig->type_mask))
			जारी;
		अगर (!dbtr_trigger_hw_supported(trig->सूचकांक_2, tdata1,
					       tdata2, tdata3))
			जारी;
		वापस i;
	}

	वापस SBI_ENOENT;
}

अटल अंतरभूत काष्ठा sbi_dbtr_trigger *sbi_alloc_trigger(अचिन्हित दीर्घ tdata1,
							 अचिन्हित दीर्घ tdata2,
							 अचिन्हित दीर्घ tdata3)
{
	पूर्णांक i;
	काष्ठा sbi_dbtr_trigger *f_trig;
	काष्ठा sbi_dbtr_hart_triggers_state *hart_state;

	hart_state = dbtr_thishart_state_ptr();
	अगर (!hart_state)
		वापस शून्य;

	अगर (hart_state->available_trigs <= 0)
		वापस शून्य;

	i = dbtr_find_free_slot(hart_state, 0, tdata1, tdata2, tdata3);
	अगर (i < 0)
		वापस शून्य;

	f_trig = सूचकांक_TO_TRIGGER(i);
	hart_state->available_trigs--;
	__set_bit(RV_DBTR_BIT(TS, MAPPED), &f_trig->state);

	वापस f_trig;
}

अटल अंतरभूत व्योम sbi_free_trigger(काष्ठा sbi_dbtr_trigger *trig)
{
	काष्ठा sbi_dbtr_hart_triggers_state *hart_state;

	अगर (trig == शून्य)
		वापस;

	hart_state = dbtr_thishart_state_ptr();
	अगर (!hart_state)
		वापस;

	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;

	hart_state->available_trigs++;
}

पूर्णांक sbi_dbtr_init(काष्ठा sbi_scratch *scratch, bool coldboot)
{
	काष्ठा sbi_trap_info trap = {0};
	अचिन्हित दीर्घ tdata1;
	अचिन्हित दीर्घ val;
	पूर्णांक i;
	काष्ठा sbi_dbtr_hart_triggers_state *hart_state = शून्य;

	अगर (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		वापस SBI_SUCCESS;

	अगर (coldboot) {
		hart_state_ptr_offset = sbi_scratch_alloc_type_offset(व्योम *);
		अगर (!hart_state_ptr_offset)
			वापस SBI_ENOMEM;
	}

	hart_state = dbtr_get_hart_state_ptr(scratch);
	अगर (!hart_state) {
		hart_state = sbi_zalloc(माप(*hart_state));
		अगर (!hart_state)
			वापस SBI_ENOMEM;
		hart_state->hartid = current_hartid();
		dbtr_set_hart_state_ptr(scratch, hart_state);
	}

	/* disable the shared memory */
	sbi_dbtr_disable_shmem(hart_state);

	/* Skip probing triggers if already probed */
	अगर (hart_state->probed)
		जाओ _probed;

	क्रम (i = 0; i < RV_MAX_TRIGGERS; i++) {
		csr_write_allowed(CSR_TSELECT, &trap, i);
		अगर (trap.cause)
			अवरोध;

		val = csr_read_allowed(CSR_TSELECT, &trap);
		अगर (trap.cause)
			अवरोध;

		/*
		 * Read back tselect and check that it contains the
		 * written value
		 */
		अगर (val != i)
			अवरोध;

		val = csr_read_allowed(CSR_TINFO, &trap);
		अगर (trap.cause) {
			/*
			 * If reading tinfo caused an exception, the
			 * debugger must read tdata1 to discover the
			 * type.
			 */
			tdata1 = csr_read_allowed(CSR_TDATA1,
						  &trap);
			अगर (trap.cause)
				अवरोध;

			अगर (TDATA1_GET_TYPE(tdata1) == 0)
				अवरोध;

			sbi_trigger_init(सूचकांक_TO_TRIGGER(i),
					 BIT(TDATA1_GET_TYPE(tdata1)),
					 i);
			hart_state->योग_trigs++;
		} अन्यथा {
			अगर (val == 1)
				अवरोध;

			sbi_trigger_init(सूचकांक_TO_TRIGGER(i), val, i);
			hart_state->योग_trigs++;
		}
	}

	hart_state->probed = 1;

 _probed:
	hart_state->available_trigs = hart_state->योग_trigs;

	वापस SBI_SUCCESS;
}

पूर्णांक sbi_dbtr_get_योग_triggers(व्योम)
{
	काष्ठा sbi_dbtr_hart_triggers_state *hs;
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/*
	 * This function may be used during ecall registration.
	 * By that time the debug trigger module might not be
	 * initialized. If the extension is not supported, report
	 * number of triggers as 0.
	 */
	अगर (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		वापस 0;

	hs = dbtr_thishart_state_ptr();
	अगर (!hs)
		वापस 0;

	वापस hs->योग_trigs;
}

पूर्णांक sbi_dbtr_setup_shmem(स्थिर काष्ठा sbi_domain *dom, अचिन्हित दीर्घ smode,
			 अचिन्हित दीर्घ shmem_phys_lo,
			 अचिन्हित दीर्घ shmem_phys_hi)
{
	काष्ठा sbi_dbtr_hart_triggers_state *hart_state;

	अगर (dom && !sbi_domain_is_assigned_hart(dom, current_hartindex())) {
		sbi_dprintf("%s: calling hart not assigned to this domain\n",
			   __func__);
		वापस SBI_ERR_DENIED;
	}

	hart_state = dbtr_thishart_state_ptr();
	अगर (!hart_state)
		वापस SBI_ERR_FAILED;

	/* call is to disable shared memory */
	अगर (shmem_phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR
	    && shmem_phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR) {
		sbi_dbtr_disable_shmem(hart_state);
		वापस SBI_SUCCESS;
	}

	/* the shared memory must be disabled on this hart */
	अगर (!sbi_dbtr_shmem_disabled(hart_state))
		वापस SBI_ERR_ALREADY_AVAILABLE;

	/* lower physical address must be XLEN/8 bytes aligned */
	अगर (shmem_phys_lo & SBI_DBTR_SHMEM_ALIGN_MASK)
		वापस SBI_ERR_INVALID_PARAM;

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
	अगर (shmem_phys_hi)
		वापस SBI_EINVALID_ADDR;

	अगर (dom && !sbi_domain_check_addr(dom,
		  DBTR_SHMEM_MAKE_PHYS(shmem_phys_hi, shmem_phys_lo), smode,
		  SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		वापस SBI_ERR_INVALID_ADDRESS;

	hart_state->shmem.phys_lo = shmem_phys_lo;
	hart_state->shmem.phys_hi = shmem_phys_hi;

	वापस SBI_SUCCESS;
}

अटल व्योम dbtr_trigger_setup(काष्ठा sbi_dbtr_trigger *trig,
			       काष्ठा sbi_dbtr_तथ्य_संदेश *recv)
{
	अचिन्हित दीर्घ tdata1;

	अगर (!trig)
		वापस;

	trig->tdata1 = lle_to_cpu(recv->tdata1);
	trig->tdata2 = lle_to_cpu(recv->tdata2);
	trig->tdata3 = lle_to_cpu(recv->tdata3);

	tdata1 = lle_to_cpu(recv->tdata1);

	trig->state = 0;

	__set_bit(RV_DBTR_BIT(TS, MAPPED), &trig->state);

	SET_TRIG_HW_सूचकांक(trig->state, trig->सूचकांक_2);

	चयन (TDATA1_GET_TYPE(tdata1)) {
	हाल RISCV_DBTR_TRIG_MCONTROL:
		अगर (__test_bit(RV_DBTR_BIT(MC, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		अगर (__test_bit(RV_DBTR_BIT(MC, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);
		अवरोध;
	हाल RISCV_DBTR_TRIG_MCONTROL6:
		अगर (__test_bit(RV_DBTR_BIT(MC6, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		अगर (__test_bit(RV_DBTR_BIT(MC6, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);

		अगर (__test_bit(RV_DBTR_BIT(MC6, VU), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VU), &trig->state);

		अगर (__test_bit(RV_DBTR_BIT(MC6, VS), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VS), &trig->state);
		अवरोध;
	हाल RISCV_DBTR_TRIG_ICOUNT:
		अगर (__test_bit(RV_DBTR_BIT(ICOUNT, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		अगर (__test_bit(RV_DBTR_BIT(ICOUNT, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);

		अगर (__test_bit(RV_DBTR_BIT(ICOUNT, VU), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VU), &trig->state);

		अगर (__test_bit(RV_DBTR_BIT(ICOUNT, VS), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VS), &trig->state);
		अवरोध;
	शेष:
		sbi_dprintf("%s: Unknown type (tdata1: 0x%lx Type: %ld)\n",
			    __func__, tdata1, TDATA1_GET_TYPE(tdata1));
		अवरोध;
	}
}

अटल अंतरभूत व्योम update_bit(अचिन्हित दीर्घ new, पूर्णांक nr, अस्थिर अचिन्हित दीर्घ *addr)
{
	अगर (new)
		__set_bit(nr, addr);
	अन्यथा
		__clear_bit(nr, addr);
}

अटल व्योम dbtr_trigger_enable(काष्ठा sbi_dbtr_trigger *trig)
{
	अचिन्हित दीर्घ state;
	अचिन्हित दीर्घ tdata1;

	अगर (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		वापस;

	state = trig->state;
	tdata1 = trig->tdata1;

	चयन (TDATA1_GET_TYPE(tdata1)) {
	हाल RISCV_DBTR_TRIG_MCONTROL:
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(MC, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(MC, S), &trig->tdata1);
		अवरोध;
	हाल RISCV_DBTR_TRIG_MCONTROL6:
		update_bit(state & RV_DBTR_BIT_MASK(TS, VU),
			   RV_DBTR_BIT(MC6, VU), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, VS),
			   RV_DBTR_BIT(MC6, VS), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(MC6, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(MC6, S), &trig->tdata1);
		अवरोध;
	हाल RISCV_DBTR_TRIG_ICOUNT:
		update_bit(state & RV_DBTR_BIT_MASK(TS, VU),
			   RV_DBTR_BIT(ICOUNT, VU), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, VS),
			   RV_DBTR_BIT(ICOUNT, VS), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(ICOUNT, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(ICOUNT, S), &trig->tdata1);
		अवरोध;
	शेष:
		अवरोध;
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

अटल व्योम dbtr_trigger_disable(काष्ठा sbi_dbtr_trigger *trig)
{
	अचिन्हित दीर्घ tdata1;

	अगर (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		वापस;

	tdata1 = trig->tdata1;

	चयन (TDATA1_GET_TYPE(tdata1)) {
	हाल RISCV_DBTR_TRIG_MCONTROL:
		__clear_bit(RV_DBTR_BIT(MC, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC, S), &trig->tdata1);
		अवरोध;
	हाल RISCV_DBTR_TRIG_MCONTROL6:
		__clear_bit(RV_DBTR_BIT(MC6, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, S), &trig->tdata1);
		अवरोध;
	हाल RISCV_DBTR_TRIG_ICOUNT:
		__clear_bit(RV_DBTR_BIT(ICOUNT, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, S), &trig->tdata1);
		अवरोध;
	शेष:
		अवरोध;
	}

	csr_write(CSR_TSELECT, trig->सूचकांक_2);
	csr_write(CSR_TDATA1, trig->tdata1);
}

अटल व्योम dbtr_trigger_clear(काष्ठा sbi_dbtr_trigger *trig)
{
	अगर (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		वापस;

	csr_write(CSR_TSELECT, trig->सूचकांक_2);
	csr_write(CSR_TDATA1, 0x0);
	tdata_write_safe(CSR_TDATA2, 0x0);
	tdata_write_safe(CSR_TDATA3, 0x0);
}

अटल पूर्णांक dbtr_trigger_supported(अचिन्हित दीर्घ type)
{
	चयन (type) {
	हाल RISCV_DBTR_TRIG_MCONTROL:
	हाल RISCV_DBTR_TRIG_MCONTROL6:
	हाल RISCV_DBTR_TRIG_ICOUNT:
		वापस 1;
	शेष:
		अवरोध;
	}

	वापस 0;
}

अटल पूर्णांक dbtr_trigger_valid(अचिन्हित दीर्घ type, अचिन्हित दीर्घ tdata)
{
	चयन (type) {
	हाल RISCV_DBTR_TRIG_MCONTROL:
		अगर (!(tdata & RV_DBTR_BIT_MASK(MC, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(MC, M)))
			वापस 1;
		अवरोध;
	हाल RISCV_DBTR_TRIG_MCONTROL6:
		अगर (!(tdata & RV_DBTR_BIT_MASK(MC6, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(MC6, M)))
			वापस 1;
		अवरोध;
	हाल RISCV_DBTR_TRIG_ICOUNT:
		अगर (!(tdata & RV_DBTR_BIT_MASK(ICOUNT, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(ICOUNT, M)))
			वापस 1;
		अवरोध;
	शेष:
		अवरोध;
	}

	वापस 0;
}

पूर्णांक sbi_dbtr_अंक_trig(अचिन्हित दीर्घ तथ्य, अचिन्हित दीर्घ *out)
{
	अचिन्हित दीर्घ type = TDATA1_GET_TYPE(तथ्य);
	u32 hartid = current_hartid();
	अचिन्हित दीर्घ योग = 0;
	काष्ठा sbi_dbtr_trigger *trig;
	पूर्णांक i;
	काष्ठा sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	अगर (!hs)
		वापस SBI_ERR_FAILED;

	अगर (तथ्य == 0) {
		*out = hs->योग_trigs;
		वापस SBI_SUCCESS;
	}

	क्रम (i = 0; i < hs->योग_trigs; i++) {
		trig = सूचकांक_TO_TRIGGER(i);

		अगर (__test_bit(type, &trig->type_mask) &&
		    dbtr_trigger_hw_supported(trig->सूचकांक_2, तथ्य, 0, 0))
			योग++;
	}

	sbi_dprintf("%s: hart%d: total triggers of type %lu: %lu\n",
		    __func__, hartid, type, योग);

	*out = योग;
	वापस SBI_SUCCESS;
}

पूर्णांक sbi_dbtr_read_trig(अचिन्हित दीर्घ smode,
		       अचिन्हित दीर्घ trig_idx_base, अचिन्हित दीर्घ trig_गिनती)
{
	काष्ठा sbi_dbtr_तथ्य_संदेश *xmit;
	काष्ठा sbi_dbtr_trigger *trig;
	जोड़ sbi_dbtr_shmem_entry *entry;
	व्योम *shmem_base = शून्य;
	काष्ठा sbi_dbtr_hart_triggers_state *hs = शून्य;

	hs = dbtr_thishart_state_ptr();
	अगर (!hs)
		वापस SBI_ERR_FAILED;

	अगर (trig_idx_base >= hs->योग_trigs ||
	    trig_idx_base + trig_गिनती >= hs->योग_trigs)
		वापस SBI_ERR_BAD_RANGE;

	अगर (sbi_dbtr_shmem_disabled(hs))
		वापस SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);

	sbi_hart_protection_अस्थायी_map_range((अचिन्हित दीर्घ)shmem_base,
					   trig_गिनती * माप(*entry));
	for_each_trig_entry(shmem_base, trig_गिनती, typeof(*entry), entry) {
		xmit = &entry->तथ्य;
		trig = सूचकांक_TO_TRIGGER((_idx + trig_idx_base));
		csr_write(CSR_TSELECT, trig->सूचकांक_2);
		trig->tdata1 = csr_read(CSR_TDATA1);
		trig->tdata2 = tdata_read_safe(CSR_TDATA2);
		trig->tdata3 = tdata_read_safe(CSR_TDATA3);
		xmit->tstate = cpu_to_lle(trig->state);
		xmit->tdata1 = cpu_to_lle(trig->tdata1);
		xmit->tdata2 = cpu_to_lle(trig->tdata2);
		xmit->tdata3 = cpu_to_lle(trig->tdata3);
	}
	sbi_hart_protection_अस्थायी_unmap_range((अचिन्हित दीर्घ)shmem_base,
					     trig_गिनती * माप(*entry));

	वापस SBI_SUCCESS;
}

पूर्णांक sbi_dbtr_install_trig(अचिन्हित दीर्घ smode,
			  अचिन्हित दीर्घ trig_गिनती, अचिन्हित दीर्घ *out)
{
	व्योम *shmem_base = शून्य;
	जोड़ sbi_dbtr_shmem_entry *entry;
	काष्ठा sbi_dbtr_तथ्य_संदेश *recv;
	काष्ठा sbi_dbtr_id_संदेश *xmit;
	अचिन्हित दीर्घ ctrl;
	u32 claimed = 0;
	पूर्णांक slot;
	काष्ठा sbi_dbtr_trigger *trig;
	काष्ठा sbi_dbtr_hart_triggers_state *hs = शून्य;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	अगर (!hs)
		वापस SBI_ERR_FAILED;

	अगर (sbi_dbtr_shmem_disabled(hs))
		वापस SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);
	sbi_hart_protection_अस्थायी_map_range((अचिन्हित दीर्घ)shmem_base,
					   trig_गिनती * माप(*entry));

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
	for_each_trig_entry(shmem_base, trig_गिनती, typeof(*entry), entry) {
		recv = (काष्ठा sbi_dbtr_तथ्य_संदेश *)(&entry->तथ्य);
		ctrl = recv->tdata1;

		अगर (!dbtr_trigger_supported(TDATA1_GET_TYPE(ctrl))) {
			*out = _idx;
			sbi_hart_protection_अस्थायी_unmap_range((अचिन्हित दीर्घ)shmem_base,
							     trig_गिनती * माप(*entry));
			वापस SBI_ERR_FAILED;
		}

		अगर (!dbtr_trigger_valid(TDATA1_GET_TYPE(ctrl), ctrl)) {
			*out = _idx;
			sbi_hart_protection_अस्थायी_unmap_range((अचिन्हित दीर्घ)shmem_base,
							     trig_गिनती * माप(*entry));
			वापस SBI_ERR_INVALID_PARAM;
		}

		अगर ((recv->tdata2 && !tdata2_impl) ||
		    (recv->tdata3 && !tdata3_impl)) {
			*out = _idx;
			sbi_hart_protection_अस्थायी_unmap_range((अचिन्हित दीर्घ)shmem_base,
							     trig_गिनती * माप(*entry));
			वापस SBI_ERR_NOT_SUPPORTED;
		}

		अगर (!dbtr_trigger_any_hw_supported(hs,
						   lle_to_cpu(recv->tdata1),
						   lle_to_cpu(recv->tdata2),
						   lle_to_cpu(recv->tdata3))) {
			*out = _idx;
			sbi_hart_protection_अस्थायी_unmap_range((अचिन्हित दीर्घ)shmem_base,
							     trig_गिनती * माप(*entry));
			वापस SBI_ERR_NOT_SUPPORTED;
		}
	}

	अगर (hs->available_trigs < trig_गिनती) {
		*out = hs->available_trigs;
		sbi_hart_protection_अस्थायी_unmap_range((अचिन्हित दीर्घ)shmem_base,
						     trig_गिनती * माप(*entry));
		वापस SBI_ERR_FAILED;
	}

	/*
	 * Dry-run the allocation of the whole batch so that no trigger
	 * is installed if any of the requested configurations cannot be
	 * matched to a free hardware trigger slot.
	 */
	for_each_trig_entry(shmem_base, trig_गिनती, typeof(*entry), entry) {
		recv = (काष्ठा sbi_dbtr_तथ्य_संदेश *)(&entry->तथ्य);
		slot = dbtr_find_free_slot(hs, claimed,
					   lle_to_cpu(recv->tdata1),
					   lle_to_cpu(recv->tdata2),
					   lle_to_cpu(recv->tdata3));
		अगर (slot < 0) {
			*out = _idx;
			sbi_hart_protection_अस्थायी_unmap_range((अचिन्हित दीर्घ)shmem_base,
							     trig_गिनती * माप(*entry));
			वापस SBI_ERR_FAILED;
		}
		claimed |= BIT(slot);
	}

	/* Install triggers */
	for_each_trig_entry(shmem_base, trig_गिनती, typeof(*entry), entry) {
		recv = (काष्ठा sbi_dbtr_तथ्य_संदेश *)(&entry->तथ्य);
		xmit = (काष्ठा sbi_dbtr_id_संदेश *)(&entry->id);

		/*
		 * The dry-run above matched every requested configuration
		 * to a free hardware trigger slot, so allocation must
		 * succeed.
		 */
		trig = sbi_alloc_trigger(lle_to_cpu(recv->tdata1),
					 lle_to_cpu(recv->tdata2),
					 lle_to_cpu(recv->tdata3));

		dbtr_trigger_setup(trig,  recv);
		dbtr_trigger_enable(trig);
		xmit->idx = cpu_to_lle(trig->सूचकांक_2);

	}

	sbi_hart_protection_अस्थायी_unmap_range((अचिन्हित दीर्घ)shmem_base,
					     trig_गिनती * माप(*entry));

	वापस SBI_SUCCESS;
}

पूर्णांक sbi_dbtr_uninstall_trig(अचिन्हित दीर्घ trig_idx_base,
			    अचिन्हित दीर्घ trig_idx_mask)
{
	अचिन्हित दीर्घ trig_mask = trig_idx_mask << trig_idx_base;
	अचिन्हित दीर्घ idx = trig_idx_base;
	काष्ठा sbi_dbtr_trigger *trig;
	काष्ठा sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	अगर (!hs)
		वापस SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->योग_trigs) {
		trig = सूचकांक_TO_TRIGGER(idx);
		अगर (!(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
			वापस SBI_ERR_INVALID_PARAM;

		dbtr_trigger_clear(trig);

		sbi_free_trigger(trig);
	}

	वापस SBI_SUCCESS;
}

पूर्णांक sbi_dbtr_enable_trig(अचिन्हित दीर्घ trig_idx_base,
			 अचिन्हित दीर्घ trig_idx_mask)
{
	अचिन्हित दीर्घ trig_mask = trig_idx_mask << trig_idx_base;
	अचिन्हित दीर्घ idx = trig_idx_base;
	काष्ठा sbi_dbtr_trigger *trig;
	काष्ठा sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	अगर (!hs)
		वापस SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->योग_trigs) {
		trig = सूचकांक_TO_TRIGGER(idx);
		sbi_dprintf("%s: enable trigger %lu\n", __func__, idx);
		dbtr_trigger_enable(trig);
	}

	वापस SBI_SUCCESS;
}

पूर्णांक sbi_dbtr_update_trig(अचिन्हित दीर्घ smode,
			 अचिन्हित दीर्घ trig_गिनती)
{
	अचिन्हित दीर्घ trig_idx;
	काष्ठा sbi_dbtr_trigger *trig;
	जोड़ sbi_dbtr_shmem_entry *entry;
	व्योम *shmem_base = शून्य;
	काष्ठा sbi_dbtr_hart_triggers_state *hs = शून्य;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	अगर (!hs)
		वापस SBI_ERR_FAILED;

	अगर (sbi_dbtr_shmem_disabled(hs))
		वापस SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);

	अगर (trig_गिनती >= hs->योग_trigs)
		वापस SBI_ERR_BAD_RANGE;

	/*
	 * SBI v3.0 sec 19.5 requires SBI_ERR_NOT_SUPPORTED when a trigger
	 * programs a non-zero value into an unimplemented optional CSR. Only
	 * the "whole CSR unimplemented" case is caught; WARL bits tied off
	 * inside an otherwise-implemented CSR are not.
	 */
	tdata2_impl = tdata_implemented(CSR_TDATA2);
	tdata3_impl = tdata_implemented(CSR_TDATA3);

	for_each_trig_entry(shmem_base, trig_गिनती, typeof(*entry), entry) {
		sbi_hart_protection_अस्थायी_map_range((अचिन्हित दीर्घ)entry, माप(*entry));
		trig_idx = entry->id.idx;

		अगर (trig_idx >= hs->योग_trigs) {
			sbi_hart_protection_अस्थायी_unmap_range((अचिन्हित दीर्घ)entry, माप(*entry));
			वापस SBI_ERR_INVALID_PARAM;
		}

		trig = सूचकांक_TO_TRIGGER(trig_idx);

		अगर (!(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED))) {
			sbi_hart_protection_अस्थायी_unmap_range((अचिन्हित दीर्घ)entry, माप(*entry));
			वापस SBI_ERR_FAILED;
		}

		अगर ((entry->तथ्य.tdata2 && !tdata2_impl) ||
		    (entry->तथ्य.tdata3 && !tdata3_impl)) {
			sbi_hart_protection_अस्थायी_unmap_range((अचिन्हित दीर्घ)entry, माप(*entry));
			वापस SBI_ERR_NOT_SUPPORTED;
		}

		अगर (!dbtr_trigger_hw_supported(trig->सूचकांक_2,
					       lle_to_cpu(entry->तथ्य.tdata1),
					       lle_to_cpu(entry->तथ्य.tdata2),
					       lle_to_cpu(entry->तथ्य.tdata3))) {
			sbi_hart_protection_अस्थायी_unmap_range((अचिन्हित दीर्घ)entry, माप(*entry));
			वापस SBI_ERR_NOT_SUPPORTED;
		}

		dbtr_trigger_setup(trig, &entry->तथ्य);
		sbi_hart_protection_अस्थायी_unmap_range((अचिन्हित दीर्घ)entry, माप(*entry));
		dbtr_trigger_enable(trig);
	}

	वापस SBI_SUCCESS;
}

पूर्णांक sbi_dbtr_disable_trig(अचिन्हित दीर्घ trig_idx_base,
			  अचिन्हित दीर्घ trig_idx_mask)
{
	अचिन्हित दीर्घ trig_mask = trig_idx_mask << trig_idx_base;
	अचिन्हित दीर्घ idx = trig_idx_base;
	काष्ठा sbi_dbtr_trigger *trig;
	काष्ठा sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	अगर (!hs)
		वापस SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->योग_trigs) {
		trig = सूचकांक_TO_TRIGGER(idx);
		dbtr_trigger_disable(trig);
	}

	वापस SBI_SUCCESS;
}
