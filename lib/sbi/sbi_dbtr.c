/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems, Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#અન્તર્ભાવ <sbi/sbi_ecall_interface.h>
#અન્તર્ભાવ <sbi/sbi_csr_detect.h>
#અન્તર્ભાવ <sbi/sbi_platform.h>
#અન્તર્ભાવ <sbi/sbi_byteorder.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_domain.h>
#અન્તર્ભાવ <sbi/sbi_trap.h>
#અન્તર્ભાવ <sbi/sbi_dbtr.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi/sbi_hart_protection.h>
#અન્તર્ભાવ <sbi/riscv_encoding.h>
#અન્તર્ભાવ <sbi/riscv_asm.h>


/** Offset of pointer to HART's debug triggers info in scratch space */
સ્થિર unsigned long hart_state_ptr_offset;

/** Device specific debug trigger operations */
સ્થિર અચલ સંરચના sbi_dbtr_device *dbtr_dev = ખાલી;

અચલ સંરચના sbi_dbtr_device *sbi_dbtr_get_device(શૂન્ય)
{
	પાછા dbtr_dev;
}

શૂન્ય sbi_dbtr_set_device(અચલ સંરચના sbi_dbtr_device *dev)
{
	જો (!dev || dbtr_dev)
		પાછા;

	dbtr_dev = dev;
}

#define dbtr_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), શૂન્ય *, hart_state_ptr_offset)

#define dbtr_thishart_state_ptr()				\
	dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define dbtr_set_hart_state_ptr(__scratch, __hart_state)		\
	sbi_scratch_write_type((__scratch), શૂન્ય *, hart_state_ptr_offset, \
			       (__hart_state))

#define tdata_read_safe(__csr)						\
	({								\
		સંરચના sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
	})

#define tdata_write_safe(__csr, __value)				\
	({								\
		સંરચના sbi_trap_info __trap = {0};			\
		csr_write_allowed((__csr), &__trap, (__value));		\
	})

#define tdata_implemented(__csr)					\
	({								\
		સંરચના sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
		!__trap.cause;						\
	})

#define INDEX_TO_TRIGGER(_index)					\
	({								\
		સંરચના sbi_dbtr_trigger *__trg = ખાલી;			\
		સંરચના sbi_dbtr_hart_triggers_state *__hs = ખાલી;	\
		__hs = dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr()); \
		__trg = &__hs->triggers[_index];			\
		(__trg);						\
	})

#define for_each_trig_entry(_base, _max, _etype, _entry)		\
	માટે (પૂર્ણાંક _idx = 0; _entry = ((_etype *)_base + _idx),		\
	     _idx < _max;						\
	     _idx++, _entry = ((_etype *)_base + _idx))

#define DBTR_SHMEM_MAKE_PHYS(_p_hi, _p_lo) (_p_lo)

/* must call with hs != NULL */
સ્થિર અંતઃરેખ bool sbi_dbtr_shmem_disabled(
	સંરચના sbi_dbtr_hart_triggers_state *hs)
{
	પાછા (hs->shmem.phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR &&
		hs->shmem.phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR
		? true : false);
}

/* must call with hs != NULL */
સ્થિર અંતઃરેખ શૂન્ય sbi_dbtr_disable_shmem(
	સંરચના sbi_dbtr_hart_triggers_state *hs)
{
	hs->shmem.phys_lo = SBI_DBTR_SHMEM_INVALID_ADDR;
	hs->shmem.phys_hi = SBI_DBTR_SHMEM_INVALID_ADDR;
}

/* must call with hs which is not disabled */
સ્થિર અંતઃરેખ શૂન્ય *hart_shmem_base(
	સંરચના sbi_dbtr_hart_triggers_state *hs)
{
	પાછા ((શૂન્ય *)(unsigned long)DBTR_SHMEM_MAKE_PHYS(
			hs->shmem.phys_hi, hs->shmem.phys_lo));
}

સ્થિર શૂન્ય sbi_trigger_init(સંરચના sbi_dbtr_trigger *trig,
			     unsigned long type_mask, unsigned long idx)
{
	trig->type_mask = type_mask;
	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;
	trig->index = idx;
}

સ્થિર bool dbtr_trigger_hw_supported(unsigned long idx, unsigned long tdata1,
				      unsigned long tdata2,
				      unsigned long tdata3)
{
	જો (dbtr_dev && dbtr_dev->trigger_supported)
		પાછા dbtr_dev->trigger_supported(idx, tdata1, tdata2,
						   tdata3);

	પાછા true;
}

સ્થિર bool dbtr_trigger_any_hw_supported(
			સંરચના sbi_dbtr_hart_triggers_state *hs,
			unsigned long tdata1, unsigned long tdata2,
			unsigned long tdata3)
{
	unsigned long type = TDATA1_GET_TYPE(tdata1);
	સંરચના sbi_dbtr_trigger *trig;
	પૂર્ણાંક i;

	માટે (i = 0; i < hs->total_trigs; i++) {
		trig = INDEX_TO_TRIGGER(i);
		જો (__test_bit(type, &trig->type_mask) &&
		    dbtr_trigger_hw_supported(trig->index, tdata1, tdata2,
					      tdata3))
			પાછા true;
	}

	પાછા false;
}

/*
 * Find the first free hardware trigger slot supporting the configuration.
 * Slots set in claimed_mask are treated as taken, which allows the caller
 * to track slot availability. A 32-bit mask covers RV_MAX_TRIGGERS (32);
 * for a larger number of triggers, this function needs to be updated.
 */
સ્થિર પૂર્ણાંક dbtr_find_free_slot(સંરચના sbi_dbtr_hart_triggers_state *hs,
			       u32 claimed_mask,
			       unsigned long tdata1, unsigned long tdata2,
			       unsigned long tdata3)
{
	unsigned long type = TDATA1_GET_TYPE(tdata1);
	સંરચના sbi_dbtr_trigger *trig;
	પૂર્ણાંક i;

	માટે (i = 0; i < hs->total_trigs; i++) {
		trig = INDEX_TO_TRIGGER(i);
		જો (trig->state & RV_DBTR_BIT_MASK(TS, MAPPED))
			ચાલુ;
		જો (claimed_mask & BIT(i))
			ચાલુ;
		જો (!__test_bit(type, &trig->type_mask))
			ચાલુ;
		જો (!dbtr_trigger_hw_supported(trig->index, tdata1,
					       tdata2, tdata3))
			ચાલુ;
		પાછા i;
	}

	પાછા SBI_ENOENT;
}

સ્થિર અંતઃરેખ સંરચના sbi_dbtr_trigger *sbi_alloc_trigger(unsigned long tdata1,
							 unsigned long tdata2,
							 unsigned long tdata3)
{
	પૂર્ણાંક i;
	સંરચના sbi_dbtr_trigger *f_trig;
	સંરચના sbi_dbtr_hart_triggers_state *hart_state;

	hart_state = dbtr_thishart_state_ptr();
	જો (!hart_state)
		પાછા ખાલી;

	જો (hart_state->available_trigs <= 0)
		પાછા ખાલી;

	i = dbtr_find_free_slot(hart_state, 0, tdata1, tdata2, tdata3);
	જો (i < 0)
		પાછા ખાલી;

	f_trig = INDEX_TO_TRIGGER(i);
	hart_state->available_trigs--;
	__set_bit(RV_DBTR_BIT(TS, MAPPED), &f_trig->state);

	પાછા f_trig;
}

સ્થિર અંતઃરેખ શૂન્ય sbi_free_trigger(સંરચના sbi_dbtr_trigger *trig)
{
	સંરચના sbi_dbtr_hart_triggers_state *hart_state;

	જો (trig == ખાલી)
		પાછા;

	hart_state = dbtr_thishart_state_ptr();
	જો (!hart_state)
		પાછા;

	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;

	hart_state->available_trigs++;
}

પૂર્ણાંક sbi_dbtr_init(સંરચના sbi_scratch *scratch, bool coldboot)
{
	સંરચના sbi_trap_info trap = {0};
	unsigned long tdata1;
	unsigned long val;
	પૂર્ણાંક i;
	સંરચના sbi_dbtr_hart_triggers_state *hart_state = ખાલી;

	જો (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		પાછા SBI_SUCCESS;

	જો (coldboot) {
		hart_state_ptr_offset = sbi_scratch_alloc_type_offset(શૂન્ય *);
		જો (!hart_state_ptr_offset)
			પાછા SBI_ENOMEM;
	}

	hart_state = dbtr_get_hart_state_ptr(scratch);
	જો (!hart_state) {
		hart_state = sbi_zalloc(કદ(*hart_state));
		જો (!hart_state)
			પાછા SBI_ENOMEM;
		hart_state->hartid = current_hartid();
		dbtr_set_hart_state_ptr(scratch, hart_state);
	}

	/* disable the shared memory */
	sbi_dbtr_disable_shmem(hart_state);

	/* Skip probing triggers if already probed */
	જો (hart_state->probed)
		જાઓ _probed;

	માટે (i = 0; i < RV_MAX_TRIGGERS; i++) {
		csr_write_allowed(CSR_TSELECT, &trap, i);
		જો (trap.cause)
			તોડો;

		val = csr_read_allowed(CSR_TSELECT, &trap);
		જો (trap.cause)
			તોડો;

		/*
		 * Read back tselect and check that it contains the
		 * written value
		 */
		જો (val != i)
			તોડો;

		val = csr_read_allowed(CSR_TINFO, &trap);
		જો (trap.cause) {
			/*
			 * If reading tinfo caused an exception, the
			 * debugger must read tdata1 to discover the
			 * type.
			 */
			tdata1 = csr_read_allowed(CSR_TDATA1,
						  &trap);
			જો (trap.cause)
				તોડો;

			જો (TDATA1_GET_TYPE(tdata1) == 0)
				તોડો;

			sbi_trigger_init(INDEX_TO_TRIGGER(i),
					 BIT(TDATA1_GET_TYPE(tdata1)),
					 i);
			hart_state->total_trigs++;
		} નહીંતર {
			જો (val == 1)
				તોડો;

			sbi_trigger_init(INDEX_TO_TRIGGER(i), val, i);
			hart_state->total_trigs++;
		}
	}

	hart_state->probed = 1;

 _probed:
	hart_state->available_trigs = hart_state->total_trigs;

	પાછા SBI_SUCCESS;
}

પૂર્ણાંક sbi_dbtr_get_total_triggers(શૂન્ય)
{
	સંરચના sbi_dbtr_hart_triggers_state *hs;
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/*
	 * This function may be used during ecall registration.
	 * By that time the debug trigger module might not be
	 * initialized. If the extension is not supported, report
	 * number of triggers as 0.
	 */
	જો (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		પાછા 0;

	hs = dbtr_thishart_state_ptr();
	જો (!hs)
		પાછા 0;

	પાછા hs->total_trigs;
}

પૂર્ણાંક sbi_dbtr_setup_shmem(અચલ સંરચના sbi_domain *dom, unsigned long smode,
			 unsigned long shmem_phys_lo,
			 unsigned long shmem_phys_hi)
{
	સંરચના sbi_dbtr_hart_triggers_state *hart_state;

	જો (dom && !sbi_domain_is_assigned_hart(dom, current_hartindex())) {
		sbi_dprintf("%s: calling hart not assigned to this domain\n",
			   __func__);
		પાછા SBI_ERR_DENIED;
	}

	hart_state = dbtr_thishart_state_ptr();
	જો (!hart_state)
		પાછા SBI_ERR_FAILED;

	/* call is to disable shared memory */
	જો (shmem_phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR
	    && shmem_phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR) {
		sbi_dbtr_disable_shmem(hart_state);
		પાછા SBI_SUCCESS;
	}

	/* the shared memory must be disabled on this hart */
	જો (!sbi_dbtr_shmem_disabled(hart_state))
		પાછા SBI_ERR_ALREADY_AVAILABLE;

	/* lower physical address must be XLEN/8 bytes aligned */
	જો (shmem_phys_lo & SBI_DBTR_SHMEM_ALIGN_MASK)
		પાછા SBI_ERR_INVALID_PARAM;

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
	જો (shmem_phys_hi)
		પાછા SBI_EINVALID_ADDR;

	જો (dom && !sbi_domain_check_addr(dom,
		  DBTR_SHMEM_MAKE_PHYS(shmem_phys_hi, shmem_phys_lo), smode,
		  SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		પાછા SBI_ERR_INVALID_ADDRESS;

	hart_state->shmem.phys_lo = shmem_phys_lo;
	hart_state->shmem.phys_hi = shmem_phys_hi;

	પાછા SBI_SUCCESS;
}

સ્થિર શૂન્ય dbtr_trigger_setup(સંરચના sbi_dbtr_trigger *trig,
			       સંરચના sbi_dbtr_data_સંદેશ *recv)
{
	unsigned long tdata1;

	જો (!trig)
		પાછા;

	trig->tdata1 = lle_to_cpu(recv->tdata1);
	trig->tdata2 = lle_to_cpu(recv->tdata2);
	trig->tdata3 = lle_to_cpu(recv->tdata3);

	tdata1 = lle_to_cpu(recv->tdata1);

	trig->state = 0;

	__set_bit(RV_DBTR_BIT(TS, MAPPED), &trig->state);

	SET_TRIG_HW_INDEX(trig->state, trig->index);

	પસંદગી (TDATA1_GET_TYPE(tdata1)) {
	સ્થિતિ RISCV_DBTR_TRIG_MCONTROL:
		જો (__test_bit(RV_DBTR_BIT(MC, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		જો (__test_bit(RV_DBTR_BIT(MC, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);
		તોડો;
	સ્થિતિ RISCV_DBTR_TRIG_MCONTROL6:
		જો (__test_bit(RV_DBTR_BIT(MC6, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		જો (__test_bit(RV_DBTR_BIT(MC6, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);

		જો (__test_bit(RV_DBTR_BIT(MC6, VU), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VU), &trig->state);

		જો (__test_bit(RV_DBTR_BIT(MC6, VS), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VS), &trig->state);
		તોડો;
	સ્થિતિ RISCV_DBTR_TRIG_ICOUNT:
		જો (__test_bit(RV_DBTR_BIT(ICOUNT, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		જો (__test_bit(RV_DBTR_BIT(ICOUNT, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);

		જો (__test_bit(RV_DBTR_BIT(ICOUNT, VU), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VU), &trig->state);

		જો (__test_bit(RV_DBTR_BIT(ICOUNT, VS), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VS), &trig->state);
		તોડો;
	મૂળભૂત:
		sbi_dprintf("%s: Unknown type (tdata1: 0x%lx Type: %ld)\n",
			    __func__, tdata1, TDATA1_GET_TYPE(tdata1));
		તોડો;
	}
}

સ્થિર અંતઃરેખ શૂન્ય update_bit(unsigned long new, પૂર્ણાંક nr, અસ્થિર unsigned long *addr)
{
	જો (new)
		__set_bit(nr, addr);
	નહીંતર
		__clear_bit(nr, addr);
}

સ્થિર શૂન્ય dbtr_trigger_enable(સંરચના sbi_dbtr_trigger *trig)
{
	unsigned long state;
	unsigned long tdata1;

	જો (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		પાછા;

	state = trig->state;
	tdata1 = trig->tdata1;

	પસંદગી (TDATA1_GET_TYPE(tdata1)) {
	સ્થિતિ RISCV_DBTR_TRIG_MCONTROL:
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(MC, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(MC, S), &trig->tdata1);
		તોડો;
	સ્થિતિ RISCV_DBTR_TRIG_MCONTROL6:
		update_bit(state & RV_DBTR_BIT_MASK(TS, VU),
			   RV_DBTR_BIT(MC6, VU), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, VS),
			   RV_DBTR_BIT(MC6, VS), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(MC6, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(MC6, S), &trig->tdata1);
		તોડો;
	સ્થિતિ RISCV_DBTR_TRIG_ICOUNT:
		update_bit(state & RV_DBTR_BIT_MASK(TS, VU),
			   RV_DBTR_BIT(ICOUNT, VU), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, VS),
			   RV_DBTR_BIT(ICOUNT, VS), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(ICOUNT, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(ICOUNT, S), &trig->tdata1);
		તોડો;
	મૂળભૂત:
		તોડો;
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
	csr_write(CSR_TSELECT, trig->index);
	csr_write(CSR_TDATA1, 0x0);
	tdata_write_safe(CSR_TDATA2, trig->tdata2);
	tdata_write_safe(CSR_TDATA3, trig->tdata3);
	csr_write(CSR_TDATA1, trig->tdata1);
}

સ્થિર શૂન્ય dbtr_trigger_disable(સંરચના sbi_dbtr_trigger *trig)
{
	unsigned long tdata1;

	જો (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		પાછા;

	tdata1 = trig->tdata1;

	પસંદગી (TDATA1_GET_TYPE(tdata1)) {
	સ્થિતિ RISCV_DBTR_TRIG_MCONTROL:
		__clear_bit(RV_DBTR_BIT(MC, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC, S), &trig->tdata1);
		તોડો;
	સ્થિતિ RISCV_DBTR_TRIG_MCONTROL6:
		__clear_bit(RV_DBTR_BIT(MC6, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, S), &trig->tdata1);
		તોડો;
	સ્થિતિ RISCV_DBTR_TRIG_ICOUNT:
		__clear_bit(RV_DBTR_BIT(ICOUNT, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, S), &trig->tdata1);
		તોડો;
	મૂળભૂત:
		તોડો;
	}

	csr_write(CSR_TSELECT, trig->index);
	csr_write(CSR_TDATA1, trig->tdata1);
}

સ્થિર શૂન્ય dbtr_trigger_clear(સંરચના sbi_dbtr_trigger *trig)
{
	જો (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		પાછા;

	csr_write(CSR_TSELECT, trig->index);
	csr_write(CSR_TDATA1, 0x0);
	tdata_write_safe(CSR_TDATA2, 0x0);
	tdata_write_safe(CSR_TDATA3, 0x0);
}

સ્થિર પૂર્ણાંક dbtr_trigger_supported(unsigned long type)
{
	પસંદગી (type) {
	સ્થિતિ RISCV_DBTR_TRIG_MCONTROL:
	સ્થિતિ RISCV_DBTR_TRIG_MCONTROL6:
	સ્થિતિ RISCV_DBTR_TRIG_ICOUNT:
		પાછા 1;
	મૂળભૂત:
		તોડો;
	}

	પાછા 0;
}

સ્થિર પૂર્ણાંક dbtr_trigger_valid(unsigned long type, unsigned long tdata)
{
	પસંદગી (type) {
	સ્થિતિ RISCV_DBTR_TRIG_MCONTROL:
		જો (!(tdata & RV_DBTR_BIT_MASK(MC, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(MC, M)))
			પાછા 1;
		તોડો;
	સ્થિતિ RISCV_DBTR_TRIG_MCONTROL6:
		જો (!(tdata & RV_DBTR_BIT_MASK(MC6, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(MC6, M)))
			પાછા 1;
		તોડો;
	સ્થિતિ RISCV_DBTR_TRIG_ICOUNT:
		જો (!(tdata & RV_DBTR_BIT_MASK(ICOUNT, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(ICOUNT, M)))
			પાછા 1;
		તોડો;
	મૂળભૂત:
		તોડો;
	}

	પાછા 0;
}

પૂર્ણાંક sbi_dbtr_અંક_trig(unsigned long data, unsigned long *out)
{
	unsigned long type = TDATA1_GET_TYPE(data);
	u32 hartid = current_hartid();
	unsigned long total = 0;
	સંરચના sbi_dbtr_trigger *trig;
	પૂર્ણાંક i;
	સંરચના sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	જો (!hs)
		પાછા SBI_ERR_FAILED;

	જો (data == 0) {
		*out = hs->total_trigs;
		પાછા SBI_SUCCESS;
	}

	માટે (i = 0; i < hs->total_trigs; i++) {
		trig = INDEX_TO_TRIGGER(i);

		જો (__test_bit(type, &trig->type_mask) &&
		    dbtr_trigger_hw_supported(trig->index, data, 0, 0))
			total++;
	}

	sbi_dprintf("%s: hart%d: total triggers of type %lu: %lu\n",
		    __func__, hartid, type, total);

	*out = total;
	પાછા SBI_SUCCESS;
}

પૂર્ણાંક sbi_dbtr_read_trig(unsigned long smode,
		       unsigned long trig_idx_base, unsigned long trig_ગણતરી)
{
	સંરચના sbi_dbtr_data_સંદેશ *xmit;
	સંરચના sbi_dbtr_trigger *trig;
	સંઘ sbi_dbtr_shmem_entry *entry;
	શૂન્ય *shmem_base = ખાલી;
	સંરચના sbi_dbtr_hart_triggers_state *hs = ખાલી;

	hs = dbtr_thishart_state_ptr();
	જો (!hs)
		પાછા SBI_ERR_FAILED;

	જો (trig_idx_base >= hs->total_trigs ||
	    trig_idx_base + trig_ગણતરી >= hs->total_trigs)
		પાછા SBI_ERR_BAD_RANGE;

	જો (sbi_dbtr_shmem_disabled(hs))
		પાછા SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);

	sbi_hart_protection_temp_map_range((unsigned long)shmem_base,
					   trig_ગણતરી * કદ(*entry));
	for_each_trig_entry(shmem_base, trig_ગણતરી, typeof(*entry), entry) {
		xmit = &entry->data;
		trig = INDEX_TO_TRIGGER((_idx + trig_idx_base));
		csr_write(CSR_TSELECT, trig->index);
		trig->tdata1 = csr_read(CSR_TDATA1);
		trig->tdata2 = tdata_read_safe(CSR_TDATA2);
		trig->tdata3 = tdata_read_safe(CSR_TDATA3);
		xmit->tstate = cpu_to_lle(trig->state);
		xmit->tdata1 = cpu_to_lle(trig->tdata1);
		xmit->tdata2 = cpu_to_lle(trig->tdata2);
		xmit->tdata3 = cpu_to_lle(trig->tdata3);
	}
	sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
					     trig_ગણતરી * કદ(*entry));

	પાછા SBI_SUCCESS;
}

પૂર્ણાંક sbi_dbtr_install_trig(unsigned long smode,
			  unsigned long trig_ગણતરી, unsigned long *out)
{
	શૂન્ય *shmem_base = ખાલી;
	સંઘ sbi_dbtr_shmem_entry *entry;
	સંરચના sbi_dbtr_data_સંદેશ *recv;
	સંરચના sbi_dbtr_id_સંદેશ *xmit;
	unsigned long ctrl;
	u32 claimed = 0;
	પૂર્ણાંક slot;
	સંરચના sbi_dbtr_trigger *trig;
	સંરચના sbi_dbtr_hart_triggers_state *hs = ખાલી;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	જો (!hs)
		પાછા SBI_ERR_FAILED;

	જો (sbi_dbtr_shmem_disabled(hs))
		પાછા SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);
	sbi_hart_protection_temp_map_range((unsigned long)shmem_base,
					   trig_ગણતરી * કદ(*entry));

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
	for_each_trig_entry(shmem_base, trig_ગણતરી, typeof(*entry), entry) {
		recv = (સંરચના sbi_dbtr_data_સંદેશ *)(&entry->data);
		ctrl = recv->tdata1;

		જો (!dbtr_trigger_supported(TDATA1_GET_TYPE(ctrl))) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_ગણતરી * કદ(*entry));
			પાછા SBI_ERR_FAILED;
		}

		જો (!dbtr_trigger_valid(TDATA1_GET_TYPE(ctrl), ctrl)) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_ગણતરી * કદ(*entry));
			પાછા SBI_ERR_INVALID_PARAM;
		}

		જો ((recv->tdata2 && !tdata2_impl) ||
		    (recv->tdata3 && !tdata3_impl)) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_ગણતરી * કદ(*entry));
			પાછા SBI_ERR_NOT_SUPPORTED;
		}

		જો (!dbtr_trigger_any_hw_supported(hs,
						   lle_to_cpu(recv->tdata1),
						   lle_to_cpu(recv->tdata2),
						   lle_to_cpu(recv->tdata3))) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_ગણતરી * કદ(*entry));
			પાછા SBI_ERR_NOT_SUPPORTED;
		}
	}

	જો (hs->available_trigs < trig_ગણતરી) {
		*out = hs->available_trigs;
		sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
						     trig_ગણતરી * કદ(*entry));
		પાછા SBI_ERR_FAILED;
	}

	/*
	 * Dry-run the allocation of the whole batch so that no trigger
	 * is installed if any of the requested configurations cannot be
	 * matched to a free hardware trigger slot.
	 */
	for_each_trig_entry(shmem_base, trig_ગણતરી, typeof(*entry), entry) {
		recv = (સંરચના sbi_dbtr_data_સંદેશ *)(&entry->data);
		slot = dbtr_find_free_slot(hs, claimed,
					   lle_to_cpu(recv->tdata1),
					   lle_to_cpu(recv->tdata2),
					   lle_to_cpu(recv->tdata3));
		જો (slot < 0) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_ગણતરી * કદ(*entry));
			પાછા SBI_ERR_FAILED;
		}
		claimed |= BIT(slot);
	}

	/* Install triggers */
	for_each_trig_entry(shmem_base, trig_ગણતરી, typeof(*entry), entry) {
		recv = (સંરચના sbi_dbtr_data_સંદેશ *)(&entry->data);
		xmit = (સંરચના sbi_dbtr_id_સંદેશ *)(&entry->id);

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
		xmit->idx = cpu_to_lle(trig->index);

	}

	sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
					     trig_ગણતરી * કદ(*entry));

	પાછા SBI_SUCCESS;
}

પૂર્ણાંક sbi_dbtr_uninstall_trig(unsigned long trig_idx_base,
			    unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	સંરચના sbi_dbtr_trigger *trig;
	સંરચના sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	જો (!hs)
		પાછા SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->total_trigs) {
		trig = INDEX_TO_TRIGGER(idx);
		જો (!(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
			પાછા SBI_ERR_INVALID_PARAM;

		dbtr_trigger_clear(trig);

		sbi_free_trigger(trig);
	}

	પાછા SBI_SUCCESS;
}

પૂર્ણાંક sbi_dbtr_enable_trig(unsigned long trig_idx_base,
			 unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	સંરચના sbi_dbtr_trigger *trig;
	સંરચના sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	જો (!hs)
		પાછા SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->total_trigs) {
		trig = INDEX_TO_TRIGGER(idx);
		sbi_dprintf("%s: enable trigger %lu\n", __func__, idx);
		dbtr_trigger_enable(trig);
	}

	પાછા SBI_SUCCESS;
}

પૂર્ણાંક sbi_dbtr_update_trig(unsigned long smode,
			 unsigned long trig_ગણતરી)
{
	unsigned long trig_idx;
	સંરચના sbi_dbtr_trigger *trig;
	સંઘ sbi_dbtr_shmem_entry *entry;
	શૂન્ય *shmem_base = ખાલી;
	સંરચના sbi_dbtr_hart_triggers_state *hs = ખાલી;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	જો (!hs)
		પાછા SBI_ERR_FAILED;

	જો (sbi_dbtr_shmem_disabled(hs))
		પાછા SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);

	જો (trig_ગણતરી >= hs->total_trigs)
		પાછા SBI_ERR_BAD_RANGE;

	/*
	 * SBI v3.0 sec 19.5 requires SBI_ERR_NOT_SUPPORTED when a trigger
	 * programs a non-zero value into an unimplemented optional CSR. Only
	 * the "whole CSR unimplemented" case is caught; WARL bits tied off
	 * inside an otherwise-implemented CSR are not.
	 */
	tdata2_impl = tdata_implemented(CSR_TDATA2);
	tdata3_impl = tdata_implemented(CSR_TDATA3);

	for_each_trig_entry(shmem_base, trig_ગણતરી, typeof(*entry), entry) {
		sbi_hart_protection_temp_map_range((unsigned long)entry, કદ(*entry));
		trig_idx = entry->id.idx;

		જો (trig_idx >= hs->total_trigs) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, કદ(*entry));
			પાછા SBI_ERR_INVALID_PARAM;
		}

		trig = INDEX_TO_TRIGGER(trig_idx);

		જો (!(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED))) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, કદ(*entry));
			પાછા SBI_ERR_FAILED;
		}

		જો ((entry->data.tdata2 && !tdata2_impl) ||
		    (entry->data.tdata3 && !tdata3_impl)) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, કદ(*entry));
			પાછા SBI_ERR_NOT_SUPPORTED;
		}

		જો (!dbtr_trigger_hw_supported(trig->index,
					       lle_to_cpu(entry->data.tdata1),
					       lle_to_cpu(entry->data.tdata2),
					       lle_to_cpu(entry->data.tdata3))) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, કદ(*entry));
			પાછા SBI_ERR_NOT_SUPPORTED;
		}

		dbtr_trigger_setup(trig, &entry->data);
		sbi_hart_protection_temp_unmap_range((unsigned long)entry, કદ(*entry));
		dbtr_trigger_enable(trig);
	}

	પાછા SBI_SUCCESS;
}

પૂર્ણાંક sbi_dbtr_disable_trig(unsigned long trig_idx_base,
			  unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	સંરચના sbi_dbtr_trigger *trig;
	સંરચના sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	જો (!hs)
		પાછા SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->total_trigs) {
		trig = INDEX_TO_TRIGGER(idx);
		dbtr_trigger_disable(trig);
	}

	પાછા SBI_SUCCESS;
}
