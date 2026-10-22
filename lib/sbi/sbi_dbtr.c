/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems, Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_csr_detect.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_byteorder.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_trap.h>
#شامل <sbi/sbi_dbtr.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_hart_protection.h>
#شامل <sbi/riscv_encoding.h>
#شامل <sbi/riscv_asm.h>


/** Offset of pointer to HART's debug triggers info in scratch space */
ایستا unsigned long hart_state_ptr_offset;

/** Device specific debug trigger operations */
ایستا ثابت ساختار sbi_dbtr_device *dbtr_dev = پوچ;

ثابت ساختار sbi_dbtr_device *sbi_dbtr_get_device(تهی)
{
	بازگشت dbtr_dev;
}

تهی sbi_dbtr_set_device(ثابت ساختار sbi_dbtr_device *dev)
{
	اگر (!dev || dbtr_dev)
		بازگشت;

	dbtr_dev = dev;
}

#define dbtr_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), تهی *, hart_state_ptr_offset)

#define dbtr_thishart_state_ptr()				\
	dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define dbtr_set_hart_state_ptr(__scratch, __hart_state)		\
	sbi_scratch_write_type((__scratch), تهی *, hart_state_ptr_offset, \
			       (__hart_state))

#define tdata_read_safe(__csr)						\
	({								\
		ساختار sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
	})

#define tdata_write_safe(__csr, __value)				\
	({								\
		ساختار sbi_trap_info __trap = {0};			\
		csr_write_allowed((__csr), &__trap, (__value));		\
	})

#define tdata_implemented(__csr)					\
	({								\
		ساختار sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
		!__trap.cause;						\
	})

#define INDEX_TO_TRIGGER(_index)					\
	({								\
		ساختار sbi_dbtr_trigger *__trg = پوچ;			\
		ساختار sbi_dbtr_hart_triggers_state *__hs = پوچ;	\
		__hs = dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr()); \
		__trg = &__hs->triggers[_index];			\
		(__trg);						\
	})

#define for_each_trig_entry(_base, _max, _etype, _entry)		\
	برای (صحیح _idx = 0; _entry = ((_etype *)_base + _idx),		\
	     _idx < _max;						\
	     _idx++, _entry = ((_etype *)_base + _idx))

#define DBTR_SHMEM_MAKE_PHYS(_p_hi, _p_lo) (_p_lo)

/* must call with hs != NULL */
ایستا درونخطی bool sbi_dbtr_shmem_disabled(
	ساختار sbi_dbtr_hart_triggers_state *hs)
{
	بازگشت (hs->shmem.phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR &&
		hs->shmem.phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR
		? true : false);
}

/* must call with hs != NULL */
ایستا درونخطی تهی sbi_dbtr_disable_shmem(
	ساختار sbi_dbtr_hart_triggers_state *hs)
{
	hs->shmem.phys_lo = SBI_DBTR_SHMEM_INVALID_ADDR;
	hs->shmem.phys_hi = SBI_DBTR_SHMEM_INVALID_ADDR;
}

/* must call with hs which is not disabled */
ایستا درونخطی تهی *hart_shmem_base(
	ساختار sbi_dbtr_hart_triggers_state *hs)
{
	بازگشت ((تهی *)(unsigned long)DBTR_SHMEM_MAKE_PHYS(
			hs->shmem.phys_hi, hs->shmem.phys_lo));
}

ایستا تهی sbi_trigger_init(ساختار sbi_dbtr_trigger *trig,
			     unsigned long type_mask, unsigned long idx)
{
	trig->type_mask = type_mask;
	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;
	trig->index = idx;
}

ایستا bool dbtr_trigger_hw_supported(unsigned long idx, unsigned long tdata1,
				      unsigned long tdata2,
				      unsigned long tdata3)
{
	اگر (dbtr_dev && dbtr_dev->trigger_supported)
		بازگشت dbtr_dev->trigger_supported(idx, tdata1, tdata2,
						   tdata3);

	بازگشت true;
}

ایستا bool dbtr_trigger_any_hw_supported(
			ساختار sbi_dbtr_hart_triggers_state *hs,
			unsigned long tdata1, unsigned long tdata2,
			unsigned long tdata3)
{
	unsigned long type = TDATA1_GET_TYPE(tdata1);
	ساختار sbi_dbtr_trigger *trig;
	صحیح i;

	برای (i = 0; i < hs->total_trigs; i++) {
		trig = INDEX_TO_TRIGGER(i);
		اگر (__test_bit(type, &trig->type_mask) &&
		    dbtr_trigger_hw_supported(trig->index, tdata1, tdata2,
					      tdata3))
			بازگشت true;
	}

	بازگشت false;
}

/*
 * Find the first free hardware trigger slot supporting the configuration.
 * Slots set in claimed_mask are treated as taken, which allows the caller
 * to track slot availability. A 32-bit mask covers RV_MAX_TRIGGERS (32);
 * for a larger number of triggers, this function needs to be updated.
 */
ایستا صحیح dbtr_find_free_slot(ساختار sbi_dbtr_hart_triggers_state *hs,
			       u32 claimed_mask,
			       unsigned long tdata1, unsigned long tdata2,
			       unsigned long tdata3)
{
	unsigned long type = TDATA1_GET_TYPE(tdata1);
	ساختار sbi_dbtr_trigger *trig;
	صحیح i;

	برای (i = 0; i < hs->total_trigs; i++) {
		trig = INDEX_TO_TRIGGER(i);
		اگر (trig->state & RV_DBTR_BIT_MASK(TS, MAPPED))
			ادامه;
		اگر (claimed_mask & BIT(i))
			ادامه;
		اگر (!__test_bit(type, &trig->type_mask))
			ادامه;
		اگر (!dbtr_trigger_hw_supported(trig->index, tdata1,
					       tdata2, tdata3))
			ادامه;
		بازگشت i;
	}

	بازگشت SBI_ENOENT;
}

ایستا درونخطی ساختار sbi_dbtr_trigger *sbi_alloc_trigger(unsigned long tdata1,
							 unsigned long tdata2,
							 unsigned long tdata3)
{
	صحیح i;
	ساختار sbi_dbtr_trigger *f_trig;
	ساختار sbi_dbtr_hart_triggers_state *hart_state;

	hart_state = dbtr_thishart_state_ptr();
	اگر (!hart_state)
		بازگشت پوچ;

	اگر (hart_state->available_trigs <= 0)
		بازگشت پوچ;

	i = dbtr_find_free_slot(hart_state, 0, tdata1, tdata2, tdata3);
	اگر (i < 0)
		بازگشت پوچ;

	f_trig = INDEX_TO_TRIGGER(i);
	hart_state->available_trigs--;
	__set_bit(RV_DBTR_BIT(TS, MAPPED), &f_trig->state);

	بازگشت f_trig;
}

ایستا درونخطی تهی sbi_free_trigger(ساختار sbi_dbtr_trigger *trig)
{
	ساختار sbi_dbtr_hart_triggers_state *hart_state;

	اگر (trig == پوچ)
		بازگشت;

	hart_state = dbtr_thishart_state_ptr();
	اگر (!hart_state)
		بازگشت;

	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;

	hart_state->available_trigs++;
}

صحیح sbi_dbtr_init(ساختار sbi_scratch *scratch, bool coldboot)
{
	ساختار sbi_trap_info trap = {0};
	unsigned long tdata1;
	unsigned long val;
	صحیح i;
	ساختار sbi_dbtr_hart_triggers_state *hart_state = پوچ;

	اگر (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		بازگشت SBI_SUCCESS;

	اگر (coldboot) {
		hart_state_ptr_offset = sbi_scratch_alloc_type_offset(تهی *);
		اگر (!hart_state_ptr_offset)
			بازگشت SBI_ENOMEM;
	}

	hart_state = dbtr_get_hart_state_ptr(scratch);
	اگر (!hart_state) {
		hart_state = sbi_zalloc(اندازه(*hart_state));
		اگر (!hart_state)
			بازگشت SBI_ENOMEM;
		hart_state->hartid = current_hartid();
		dbtr_set_hart_state_ptr(scratch, hart_state);
	}

	/* disable the shared memory */
	sbi_dbtr_disable_shmem(hart_state);

	/* Skip probing triggers if already probed */
	اگر (hart_state->probed)
		برو _probed;

	برای (i = 0; i < RV_MAX_TRIGGERS; i++) {
		csr_write_allowed(CSR_TSELECT, &trap, i);
		اگر (trap.cause)
			بشکن;

		val = csr_read_allowed(CSR_TSELECT, &trap);
		اگر (trap.cause)
			بشکن;

		/*
		 * Read back tselect and check that it contains the
		 * written value
		 */
		اگر (val != i)
			بشکن;

		val = csr_read_allowed(CSR_TINFO, &trap);
		اگر (trap.cause) {
			/*
			 * If reading tinfo caused an exception, the
			 * debugger must read tdata1 to discover the
			 * type.
			 */
			tdata1 = csr_read_allowed(CSR_TDATA1,
						  &trap);
			اگر (trap.cause)
				بشکن;

			اگر (TDATA1_GET_TYPE(tdata1) == 0)
				بشکن;

			sbi_trigger_init(INDEX_TO_TRIGGER(i),
					 BIT(TDATA1_GET_TYPE(tdata1)),
					 i);
			hart_state->total_trigs++;
		} وگرنه {
			اگر (val == 1)
				بشکن;

			sbi_trigger_init(INDEX_TO_TRIGGER(i), val, i);
			hart_state->total_trigs++;
		}
	}

	hart_state->probed = 1;

 _probed:
	hart_state->available_trigs = hart_state->total_trigs;

	بازگشت SBI_SUCCESS;
}

صحیح sbi_dbtr_get_total_triggers(تهی)
{
	ساختار sbi_dbtr_hart_triggers_state *hs;
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/*
	 * This function may be used during ecall registration.
	 * By that time the debug trigger module might not be
	 * initialized. If the extension is not supported, report
	 * number of triggers as 0.
	 */
	اگر (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		بازگشت 0;

	hs = dbtr_thishart_state_ptr();
	اگر (!hs)
		بازگشت 0;

	بازگشت hs->total_trigs;
}

صحیح sbi_dbtr_setup_shmem(ثابت ساختار sbi_domain *dom, unsigned long smode,
			 unsigned long shmem_phys_lo,
			 unsigned long shmem_phys_hi)
{
	ساختار sbi_dbtr_hart_triggers_state *hart_state;

	اگر (dom && !sbi_domain_is_assigned_hart(dom, current_hartindex())) {
		sbi_dprintf("%s: calling hart not assigned to this domain\n",
			   __func__);
		بازگشت SBI_ERR_DENIED;
	}

	hart_state = dbtr_thishart_state_ptr();
	اگر (!hart_state)
		بازگشت SBI_ERR_FAILED;

	/* call is to disable shared memory */
	اگر (shmem_phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR
	    && shmem_phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR) {
		sbi_dbtr_disable_shmem(hart_state);
		بازگشت SBI_SUCCESS;
	}

	/* the shared memory must be disabled on this hart */
	اگر (!sbi_dbtr_shmem_disabled(hart_state))
		بازگشت SBI_ERR_ALREADY_AVAILABLE;

	/* lower physical address must be XLEN/8 bytes aligned */
	اگر (shmem_phys_lo & SBI_DBTR_SHMEM_ALIGN_MASK)
		بازگشت SBI_ERR_INVALID_PARAM;

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
	اگر (shmem_phys_hi)
		بازگشت SBI_EINVALID_ADDR;

	اگر (dom && !sbi_domain_check_addr(dom,
		  DBTR_SHMEM_MAKE_PHYS(shmem_phys_hi, shmem_phys_lo), smode,
		  SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		بازگشت SBI_ERR_INVALID_ADDRESS;

	hart_state->shmem.phys_lo = shmem_phys_lo;
	hart_state->shmem.phys_hi = shmem_phys_hi;

	بازگشت SBI_SUCCESS;
}

ایستا تهی dbtr_trigger_setup(ساختار sbi_dbtr_trigger *trig,
			       ساختار sbi_dbtr_data_پیام *recv)
{
	unsigned long tdata1;

	اگر (!trig)
		بازگشت;

	trig->tdata1 = lle_to_cpu(recv->tdata1);
	trig->tdata2 = lle_to_cpu(recv->tdata2);
	trig->tdata3 = lle_to_cpu(recv->tdata3);

	tdata1 = lle_to_cpu(recv->tdata1);

	trig->state = 0;

	__set_bit(RV_DBTR_BIT(TS, MAPPED), &trig->state);

	SET_TRIG_HW_INDEX(trig->state, trig->index);

	انتخاب (TDATA1_GET_TYPE(tdata1)) {
	حالت RISCV_DBTR_TRIG_MCONTROL:
		اگر (__test_bit(RV_DBTR_BIT(MC, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		اگر (__test_bit(RV_DBTR_BIT(MC, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);
		بشکن;
	حالت RISCV_DBTR_TRIG_MCONTROL6:
		اگر (__test_bit(RV_DBTR_BIT(MC6, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		اگر (__test_bit(RV_DBTR_BIT(MC6, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);

		اگر (__test_bit(RV_DBTR_BIT(MC6, VU), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VU), &trig->state);

		اگر (__test_bit(RV_DBTR_BIT(MC6, VS), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VS), &trig->state);
		بشکن;
	حالت RISCV_DBTR_TRIG_ICOUNT:
		اگر (__test_bit(RV_DBTR_BIT(ICOUNT, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		اگر (__test_bit(RV_DBTR_BIT(ICOUNT, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);

		اگر (__test_bit(RV_DBTR_BIT(ICOUNT, VU), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VU), &trig->state);

		اگر (__test_bit(RV_DBTR_BIT(ICOUNT, VS), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VS), &trig->state);
		بشکن;
	پیشفرض:
		sbi_dprintf("%s: Unknown type (tdata1: 0x%lx Type: %ld)\n",
			    __func__, tdata1, TDATA1_GET_TYPE(tdata1));
		بشکن;
	}
}

ایستا درونخطی تهی update_bit(unsigned long new, صحیح nr, فرار unsigned long *addr)
{
	اگر (new)
		__set_bit(nr, addr);
	وگرنه
		__clear_bit(nr, addr);
}

ایستا تهی dbtr_trigger_enable(ساختار sbi_dbtr_trigger *trig)
{
	unsigned long state;
	unsigned long tdata1;

	اگر (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		بازگشت;

	state = trig->state;
	tdata1 = trig->tdata1;

	انتخاب (TDATA1_GET_TYPE(tdata1)) {
	حالت RISCV_DBTR_TRIG_MCONTROL:
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(MC, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(MC, S), &trig->tdata1);
		بشکن;
	حالت RISCV_DBTR_TRIG_MCONTROL6:
		update_bit(state & RV_DBTR_BIT_MASK(TS, VU),
			   RV_DBTR_BIT(MC6, VU), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, VS),
			   RV_DBTR_BIT(MC6, VS), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(MC6, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(MC6, S), &trig->tdata1);
		بشکن;
	حالت RISCV_DBTR_TRIG_ICOUNT:
		update_bit(state & RV_DBTR_BIT_MASK(TS, VU),
			   RV_DBTR_BIT(ICOUNT, VU), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, VS),
			   RV_DBTR_BIT(ICOUNT, VS), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(ICOUNT, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(ICOUNT, S), &trig->tdata1);
		بشکن;
	پیشفرض:
		بشکن;
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

ایستا تهی dbtr_trigger_disable(ساختار sbi_dbtr_trigger *trig)
{
	unsigned long tdata1;

	اگر (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		بازگشت;

	tdata1 = trig->tdata1;

	انتخاب (TDATA1_GET_TYPE(tdata1)) {
	حالت RISCV_DBTR_TRIG_MCONTROL:
		__clear_bit(RV_DBTR_BIT(MC, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC, S), &trig->tdata1);
		بشکن;
	حالت RISCV_DBTR_TRIG_MCONTROL6:
		__clear_bit(RV_DBTR_BIT(MC6, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, S), &trig->tdata1);
		بشکن;
	حالت RISCV_DBTR_TRIG_ICOUNT:
		__clear_bit(RV_DBTR_BIT(ICOUNT, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, S), &trig->tdata1);
		بشکن;
	پیشفرض:
		بشکن;
	}

	csr_write(CSR_TSELECT, trig->index);
	csr_write(CSR_TDATA1, trig->tdata1);
}

ایستا تهی dbtr_trigger_clear(ساختار sbi_dbtr_trigger *trig)
{
	اگر (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		بازگشت;

	csr_write(CSR_TSELECT, trig->index);
	csr_write(CSR_TDATA1, 0x0);
	tdata_write_safe(CSR_TDATA2, 0x0);
	tdata_write_safe(CSR_TDATA3, 0x0);
}

ایستا صحیح dbtr_trigger_supported(unsigned long type)
{
	انتخاب (type) {
	حالت RISCV_DBTR_TRIG_MCONTROL:
	حالت RISCV_DBTR_TRIG_MCONTROL6:
	حالت RISCV_DBTR_TRIG_ICOUNT:
		بازگشت 1;
	پیشفرض:
		بشکن;
	}

	بازگشت 0;
}

ایستا صحیح dbtr_trigger_valid(unsigned long type, unsigned long tdata)
{
	انتخاب (type) {
	حالت RISCV_DBTR_TRIG_MCONTROL:
		اگر (!(tdata & RV_DBTR_BIT_MASK(MC, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(MC, M)))
			بازگشت 1;
		بشکن;
	حالت RISCV_DBTR_TRIG_MCONTROL6:
		اگر (!(tdata & RV_DBTR_BIT_MASK(MC6, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(MC6, M)))
			بازگشت 1;
		بشکن;
	حالت RISCV_DBTR_TRIG_ICOUNT:
		اگر (!(tdata & RV_DBTR_BIT_MASK(ICOUNT, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(ICOUNT, M)))
			بازگشت 1;
		بشکن;
	پیشفرض:
		بشکن;
	}

	بازگشت 0;
}

صحیح sbi_dbtr_رقم_trig(unsigned long data, unsigned long *out)
{
	unsigned long type = TDATA1_GET_TYPE(data);
	u32 hartid = current_hartid();
	unsigned long total = 0;
	ساختار sbi_dbtr_trigger *trig;
	صحیح i;
	ساختار sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	اگر (!hs)
		بازگشت SBI_ERR_FAILED;

	اگر (data == 0) {
		*out = hs->total_trigs;
		بازگشت SBI_SUCCESS;
	}

	برای (i = 0; i < hs->total_trigs; i++) {
		trig = INDEX_TO_TRIGGER(i);

		اگر (__test_bit(type, &trig->type_mask) &&
		    dbtr_trigger_hw_supported(trig->index, data, 0, 0))
			total++;
	}

	sbi_dprintf("%s: hart%d: total triggers of type %lu: %lu\n",
		    __func__, hartid, type, total);

	*out = total;
	بازگشت SBI_SUCCESS;
}

صحیح sbi_dbtr_read_trig(unsigned long smode,
		       unsigned long trig_idx_base, unsigned long trig_شمار)
{
	ساختار sbi_dbtr_data_پیام *xmit;
	ساختار sbi_dbtr_trigger *trig;
	اجتماع sbi_dbtr_shmem_entry *entry;
	تهی *shmem_base = پوچ;
	ساختار sbi_dbtr_hart_triggers_state *hs = پوچ;

	hs = dbtr_thishart_state_ptr();
	اگر (!hs)
		بازگشت SBI_ERR_FAILED;

	اگر (trig_idx_base >= hs->total_trigs ||
	    trig_idx_base + trig_شمار >= hs->total_trigs)
		بازگشت SBI_ERR_BAD_RANGE;

	اگر (sbi_dbtr_shmem_disabled(hs))
		بازگشت SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);

	sbi_hart_protection_temp_map_range((unsigned long)shmem_base,
					   trig_شمار * اندازه(*entry));
	for_each_trig_entry(shmem_base, trig_شمار, typeof(*entry), entry) {
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
					     trig_شمار * اندازه(*entry));

	بازگشت SBI_SUCCESS;
}

صحیح sbi_dbtr_install_trig(unsigned long smode,
			  unsigned long trig_شمار, unsigned long *out)
{
	تهی *shmem_base = پوچ;
	اجتماع sbi_dbtr_shmem_entry *entry;
	ساختار sbi_dbtr_data_پیام *recv;
	ساختار sbi_dbtr_id_پیام *xmit;
	unsigned long ctrl;
	u32 claimed = 0;
	صحیح slot;
	ساختار sbi_dbtr_trigger *trig;
	ساختار sbi_dbtr_hart_triggers_state *hs = پوچ;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	اگر (!hs)
		بازگشت SBI_ERR_FAILED;

	اگر (sbi_dbtr_shmem_disabled(hs))
		بازگشت SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);
	sbi_hart_protection_temp_map_range((unsigned long)shmem_base,
					   trig_شمار * اندازه(*entry));

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
	for_each_trig_entry(shmem_base, trig_شمار, typeof(*entry), entry) {
		recv = (ساختار sbi_dbtr_data_پیام *)(&entry->data);
		ctrl = recv->tdata1;

		اگر (!dbtr_trigger_supported(TDATA1_GET_TYPE(ctrl))) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_شمار * اندازه(*entry));
			بازگشت SBI_ERR_FAILED;
		}

		اگر (!dbtr_trigger_valid(TDATA1_GET_TYPE(ctrl), ctrl)) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_شمار * اندازه(*entry));
			بازگشت SBI_ERR_INVALID_PARAM;
		}

		اگر ((recv->tdata2 && !tdata2_impl) ||
		    (recv->tdata3 && !tdata3_impl)) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_شمار * اندازه(*entry));
			بازگشت SBI_ERR_NOT_SUPPORTED;
		}

		اگر (!dbtr_trigger_any_hw_supported(hs,
						   lle_to_cpu(recv->tdata1),
						   lle_to_cpu(recv->tdata2),
						   lle_to_cpu(recv->tdata3))) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_شمار * اندازه(*entry));
			بازگشت SBI_ERR_NOT_SUPPORTED;
		}
	}

	اگر (hs->available_trigs < trig_شمار) {
		*out = hs->available_trigs;
		sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
						     trig_شمار * اندازه(*entry));
		بازگشت SBI_ERR_FAILED;
	}

	/*
	 * Dry-run the allocation of the whole batch so that no trigger
	 * is installed if any of the requested configurations cannot be
	 * matched to a free hardware trigger slot.
	 */
	for_each_trig_entry(shmem_base, trig_شمار, typeof(*entry), entry) {
		recv = (ساختار sbi_dbtr_data_پیام *)(&entry->data);
		slot = dbtr_find_free_slot(hs, claimed,
					   lle_to_cpu(recv->tdata1),
					   lle_to_cpu(recv->tdata2),
					   lle_to_cpu(recv->tdata3));
		اگر (slot < 0) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_شمار * اندازه(*entry));
			بازگشت SBI_ERR_FAILED;
		}
		claimed |= BIT(slot);
	}

	/* Install triggers */
	for_each_trig_entry(shmem_base, trig_شمار, typeof(*entry), entry) {
		recv = (ساختار sbi_dbtr_data_پیام *)(&entry->data);
		xmit = (ساختار sbi_dbtr_id_پیام *)(&entry->id);

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
					     trig_شمار * اندازه(*entry));

	بازگشت SBI_SUCCESS;
}

صحیح sbi_dbtr_uninstall_trig(unsigned long trig_idx_base,
			    unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	ساختار sbi_dbtr_trigger *trig;
	ساختار sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	اگر (!hs)
		بازگشت SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->total_trigs) {
		trig = INDEX_TO_TRIGGER(idx);
		اگر (!(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
			بازگشت SBI_ERR_INVALID_PARAM;

		dbtr_trigger_clear(trig);

		sbi_free_trigger(trig);
	}

	بازگشت SBI_SUCCESS;
}

صحیح sbi_dbtr_enable_trig(unsigned long trig_idx_base,
			 unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	ساختار sbi_dbtr_trigger *trig;
	ساختار sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	اگر (!hs)
		بازگشت SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->total_trigs) {
		trig = INDEX_TO_TRIGGER(idx);
		sbi_dprintf("%s: enable trigger %lu\n", __func__, idx);
		dbtr_trigger_enable(trig);
	}

	بازگشت SBI_SUCCESS;
}

صحیح sbi_dbtr_update_trig(unsigned long smode,
			 unsigned long trig_شمار)
{
	unsigned long trig_idx;
	ساختار sbi_dbtr_trigger *trig;
	اجتماع sbi_dbtr_shmem_entry *entry;
	تهی *shmem_base = پوچ;
	ساختار sbi_dbtr_hart_triggers_state *hs = پوچ;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	اگر (!hs)
		بازگشت SBI_ERR_FAILED;

	اگر (sbi_dbtr_shmem_disabled(hs))
		بازگشت SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);

	اگر (trig_شمار >= hs->total_trigs)
		بازگشت SBI_ERR_BAD_RANGE;

	/*
	 * SBI v3.0 sec 19.5 requires SBI_ERR_NOT_SUPPORTED when a trigger
	 * programs a non-zero value into an unimplemented optional CSR. Only
	 * the "whole CSR unimplemented" case is caught; WARL bits tied off
	 * inside an otherwise-implemented CSR are not.
	 */
	tdata2_impl = tdata_implemented(CSR_TDATA2);
	tdata3_impl = tdata_implemented(CSR_TDATA3);

	for_each_trig_entry(shmem_base, trig_شمار, typeof(*entry), entry) {
		sbi_hart_protection_temp_map_range((unsigned long)entry, اندازه(*entry));
		trig_idx = entry->id.idx;

		اگر (trig_idx >= hs->total_trigs) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, اندازه(*entry));
			بازگشت SBI_ERR_INVALID_PARAM;
		}

		trig = INDEX_TO_TRIGGER(trig_idx);

		اگر (!(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED))) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, اندازه(*entry));
			بازگشت SBI_ERR_FAILED;
		}

		اگر ((entry->data.tdata2 && !tdata2_impl) ||
		    (entry->data.tdata3 && !tdata3_impl)) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, اندازه(*entry));
			بازگشت SBI_ERR_NOT_SUPPORTED;
		}

		اگر (!dbtr_trigger_hw_supported(trig->index,
					       lle_to_cpu(entry->data.tdata1),
					       lle_to_cpu(entry->data.tdata2),
					       lle_to_cpu(entry->data.tdata3))) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, اندازه(*entry));
			بازگشت SBI_ERR_NOT_SUPPORTED;
		}

		dbtr_trigger_setup(trig, &entry->data);
		sbi_hart_protection_temp_unmap_range((unsigned long)entry, اندازه(*entry));
		dbtr_trigger_enable(trig);
	}

	بازگشت SBI_SUCCESS;
}

صحیح sbi_dbtr_disable_trig(unsigned long trig_idx_base,
			  unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	ساختار sbi_dbtr_trigger *trig;
	ساختار sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	اگر (!hs)
		بازگشت SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->total_trigs) {
		trig = INDEX_TO_TRIGGER(idx);
		dbtr_trigger_disable(trig);
	}

	بازگشت SBI_SUCCESS;
}
