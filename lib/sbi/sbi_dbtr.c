/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems, Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_csr_detect.h>
#ضمن <sbi/sbi_platform.h>
#ضمن <sbi/sbi_byteorder.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/sbi_dbtr.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_hart_protection.h>
#ضمن <sbi/riscv_encoding.h>
#ضمن <sbi/riscv_asm.h>


/** Offset of pointer to HART's debug triggers info in scratch space */
ساكن unsigned long hart_state_ptr_offset;

/** Device specific debug trigger operations */
ساكن ثابت بنية sbi_dbtr_device *dbtr_dev = عدم;

ثابت بنية sbi_dbtr_device *sbi_dbtr_get_device(فراغ)
{
	أرجع dbtr_dev;
}

فراغ sbi_dbtr_set_device(ثابت بنية sbi_dbtr_device *dev)
{
	𐤀𐤌 (!dev || dbtr_dev)
		أرجع;

	dbtr_dev = dev;
}

#define dbtr_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), فراغ *, hart_state_ptr_offset)

#define dbtr_thishart_state_ptr()				\
	dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define dbtr_set_hart_state_ptr(__scratch, __hart_state)		\
	sbi_scratch_write_type((__scratch), فراغ *, hart_state_ptr_offset, \
			       (__hart_state))

#define tdata_read_safe(__csr)						\
	({								\
		بنية sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
	})

#define tdata_write_safe(__csr, __value)				\
	({								\
		بنية sbi_trap_info __trap = {0};			\
		csr_write_allowed((__csr), &__trap, (__value));		\
	})

#define tdata_implemented(__csr)					\
	({								\
		بنية sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
		!__trap.cause;						\
	})

#define INDEX_TO_TRIGGER(_index)					\
	({								\
		بنية sbi_dbtr_trigger *__trg = عدم;			\
		بنية sbi_dbtr_hart_triggers_state *__hs = عدم;	\
		__hs = dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr()); \
		__trg = &__hs->triggers[_index];			\
		(__trg);						\
	})

#define for_each_trig_entry(_base, _max, _etype, _entry)		\
	لكل (صحيح _idx = 0; _entry = ((_etype *)_base + _idx),		\
	     _idx < _max;						\
	     _idx++, _entry = ((_etype *)_base + _idx))

#define DBTR_SHMEM_MAKE_PHYS(_p_hi, _p_lo) (_p_lo)

/* must call with hs != NULL */
ساكن مضمن bool sbi_dbtr_shmem_disabled(
	بنية sbi_dbtr_hart_triggers_state *hs)
{
	أرجع (hs->shmem.phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR &&
		hs->shmem.phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR
		? true : false);
}

/* must call with hs != NULL */
ساكن مضمن فراغ sbi_dbtr_disable_shmem(
	بنية sbi_dbtr_hart_triggers_state *hs)
{
	hs->shmem.phys_lo = SBI_DBTR_SHMEM_INVALID_ADDR;
	hs->shmem.phys_hi = SBI_DBTR_SHMEM_INVALID_ADDR;
}

/* must call with hs which is not disabled */
ساكن مضمن فراغ *hart_shmem_base(
	بنية sbi_dbtr_hart_triggers_state *hs)
{
	أرجع ((فراغ *)(unsigned long)DBTR_SHMEM_MAKE_PHYS(
			hs->shmem.phys_hi, hs->shmem.phys_lo));
}

ساكن فراغ sbi_trigger_init(بنية sbi_dbtr_trigger *trig,
			     unsigned long type_mask, unsigned long idx)
{
	trig->type_mask = type_mask;
	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;
	trig->index = idx;
}

ساكن bool dbtr_trigger_hw_supported(unsigned long idx, unsigned long tdata1,
				      unsigned long tdata2,
				      unsigned long tdata3)
{
	𐤀𐤌 (dbtr_dev && dbtr_dev->trigger_supported)
		أرجع dbtr_dev->trigger_supported(idx, tdata1, tdata2,
						   tdata3);

	أرجع true;
}

ساكن bool dbtr_trigger_any_hw_supported(
			بنية sbi_dbtr_hart_triggers_state *hs,
			unsigned long tdata1, unsigned long tdata2,
			unsigned long tdata3)
{
	unsigned long type = TDATA1_GET_TYPE(tdata1);
	بنية sbi_dbtr_trigger *trig;
	صحيح i;

	لكل (i = 0; i < hs->total_trigs; i++) {
		trig = INDEX_TO_TRIGGER(i);
		𐤀𐤌 (__test_bit(type, &trig->type_mask) &&
		    dbtr_trigger_hw_supported(trig->index, tdata1, tdata2,
					      tdata3))
			أرجع true;
	}

	أرجع false;
}

/*
 * Find the first free hardware trigger slot supporting the configuration.
 * Slots set in claimed_mask are treated as taken, which allows the caller
 * to track slot availability. A 32-bit mask covers RV_MAX_TRIGGERS (32);
 * for a larger number of triggers, this function needs to be updated.
 */
ساكن صحيح dbtr_find_free_slot(بنية sbi_dbtr_hart_triggers_state *hs,
			       u32 claimed_mask,
			       unsigned long tdata1, unsigned long tdata2,
			       unsigned long tdata3)
{
	unsigned long type = TDATA1_GET_TYPE(tdata1);
	بنية sbi_dbtr_trigger *trig;
	صحيح i;

	لكل (i = 0; i < hs->total_trigs; i++) {
		trig = INDEX_TO_TRIGGER(i);
		𐤀𐤌 (trig->state & RV_DBTR_BIT_MASK(TS, MAPPED))
			استمر;
		𐤀𐤌 (claimed_mask & BIT(i))
			استمر;
		𐤀𐤌 (!__test_bit(type, &trig->type_mask))
			استمر;
		𐤀𐤌 (!dbtr_trigger_hw_supported(trig->index, tdata1,
					       tdata2, tdata3))
			استمر;
		أرجع i;
	}

	أرجع SBI_ENOENT;
}

ساكن مضمن بنية sbi_dbtr_trigger *sbi_alloc_trigger(unsigned long tdata1,
							 unsigned long tdata2,
							 unsigned long tdata3)
{
	صحيح i;
	بنية sbi_dbtr_trigger *f_trig;
	بنية sbi_dbtr_hart_triggers_state *hart_state;

	hart_state = dbtr_thishart_state_ptr();
	𐤀𐤌 (!hart_state)
		أرجع عدم;

	𐤀𐤌 (hart_state->available_trigs <= 0)
		أرجع عدم;

	i = dbtr_find_free_slot(hart_state, 0, tdata1, tdata2, tdata3);
	𐤀𐤌 (i < 0)
		أرجع عدم;

	f_trig = INDEX_TO_TRIGGER(i);
	hart_state->available_trigs--;
	__set_bit(RV_DBTR_BIT(TS, MAPPED), &f_trig->state);

	أرجع f_trig;
}

ساكن مضمن فراغ sbi_free_trigger(بنية sbi_dbtr_trigger *trig)
{
	بنية sbi_dbtr_hart_triggers_state *hart_state;

	𐤀𐤌 (trig == عدم)
		أرجع;

	hart_state = dbtr_thishart_state_ptr();
	𐤀𐤌 (!hart_state)
		أرجع;

	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;

	hart_state->available_trigs++;
}

صحيح sbi_dbtr_init(بنية sbi_scratch *scratch, bool coldboot)
{
	بنية sbi_trap_info trap = {0};
	unsigned long tdata1;
	unsigned long val;
	صحيح i;
	بنية sbi_dbtr_hart_triggers_state *hart_state = عدم;

	𐤀𐤌 (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		أرجع SBI_SUCCESS;

	𐤀𐤌 (coldboot) {
		hart_state_ptr_offset = sbi_scratch_alloc_type_offset(فراغ *);
		𐤀𐤌 (!hart_state_ptr_offset)
			أرجع SBI_ENOMEM;
	}

	hart_state = dbtr_get_hart_state_ptr(scratch);
	𐤀𐤌 (!hart_state) {
		hart_state = sbi_zalloc(حجم(*hart_state));
		𐤀𐤌 (!hart_state)
			أرجع SBI_ENOMEM;
		hart_state->hartid = current_hartid();
		dbtr_set_hart_state_ptr(scratch, hart_state);
	}

	/* disable the shared memory */
	sbi_dbtr_disable_shmem(hart_state);

	/* Skip probing triggers if already probed */
	𐤀𐤌 (hart_state->probed)
		اذهب _probed;

	لكل (i = 0; i < RV_MAX_TRIGGERS; i++) {
		csr_write_allowed(CSR_TSELECT, &trap, i);
		𐤀𐤌 (trap.cause)
			اكسر;

		val = csr_read_allowed(CSR_TSELECT, &trap);
		𐤀𐤌 (trap.cause)
			اكسر;

		/*
		 * Read back tselect and check that it contains the
		 * written value
		 */
		𐤀𐤌 (val != i)
			اكسر;

		val = csr_read_allowed(CSR_TINFO, &trap);
		𐤀𐤌 (trap.cause) {
			/*
			 * If reading tinfo caused an exception, the
			 * debugger must read tdata1 to discover the
			 * type.
			 */
			tdata1 = csr_read_allowed(CSR_TDATA1,
						  &trap);
			𐤀𐤌 (trap.cause)
				اكسر;

			𐤀𐤌 (TDATA1_GET_TYPE(tdata1) == 0)
				اكسر;

			sbi_trigger_init(INDEX_TO_TRIGGER(i),
					 BIT(TDATA1_GET_TYPE(tdata1)),
					 i);
			hart_state->total_trigs++;
		} وإلا {
			𐤀𐤌 (val == 1)
				اكسر;

			sbi_trigger_init(INDEX_TO_TRIGGER(i), val, i);
			hart_state->total_trigs++;
		}
	}

	hart_state->probed = 1;

 _probed:
	hart_state->available_trigs = hart_state->total_trigs;

	أرجع SBI_SUCCESS;
}

صحيح sbi_dbtr_get_total_triggers(فراغ)
{
	بنية sbi_dbtr_hart_triggers_state *hs;
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/*
	 * This function may be used during ecall registration.
	 * By that time the debug trigger module might not be
	 * initialized. If the extension is not supported, report
	 * number of triggers as 0.
	 */
	𐤀𐤌 (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		أرجع 0;

	hs = dbtr_thishart_state_ptr();
	𐤀𐤌 (!hs)
		أرجع 0;

	أرجع hs->total_trigs;
}

صحيح sbi_dbtr_setup_shmem(ثابت بنية sbi_domain *dom, unsigned long smode,
			 unsigned long shmem_phys_lo,
			 unsigned long shmem_phys_hi)
{
	بنية sbi_dbtr_hart_triggers_state *hart_state;

	𐤀𐤌 (dom && !sbi_domain_is_assigned_hart(dom, current_hartindex())) {
		sbi_dprintf("%s: calling hart not assigned to this domain\n",
			   __func__);
		أرجع SBI_ERR_DENIED;
	}

	hart_state = dbtr_thishart_state_ptr();
	𐤀𐤌 (!hart_state)
		أرجع SBI_ERR_FAILED;

	/* call is to disable shared memory */
	𐤀𐤌 (shmem_phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR
	    && shmem_phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR) {
		sbi_dbtr_disable_shmem(hart_state);
		أرجع SBI_SUCCESS;
	}

	/* the shared memory must be disabled on this hart */
	𐤀𐤌 (!sbi_dbtr_shmem_disabled(hart_state))
		أرجع SBI_ERR_ALREADY_AVAILABLE;

	/* lower physical address must be XLEN/8 bytes aligned */
	𐤀𐤌 (shmem_phys_lo & SBI_DBTR_SHMEM_ALIGN_MASK)
		أرجع SBI_ERR_INVALID_PARAM;

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
	𐤀𐤌 (shmem_phys_hi)
		أرجع SBI_EINVALID_ADDR;

	𐤀𐤌 (dom && !sbi_domain_check_addr(dom,
		  DBTR_SHMEM_MAKE_PHYS(shmem_phys_hi, shmem_phys_lo), smode,
		  SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		أرجع SBI_ERR_INVALID_ADDRESS;

	hart_state->shmem.phys_lo = shmem_phys_lo;
	hart_state->shmem.phys_hi = shmem_phys_hi;

	أرجع SBI_SUCCESS;
}

ساكن فراغ dbtr_trigger_setup(بنية sbi_dbtr_trigger *trig,
			       بنية sbi_dbtr_data_msg *recv)
{
	unsigned long tdata1;

	𐤀𐤌 (!trig)
		أرجع;

	trig->tdata1 = lle_to_cpu(recv->tdata1);
	trig->tdata2 = lle_to_cpu(recv->tdata2);
	trig->tdata3 = lle_to_cpu(recv->tdata3);

	tdata1 = lle_to_cpu(recv->tdata1);

	trig->state = 0;

	__set_bit(RV_DBTR_BIT(TS, MAPPED), &trig->state);

	SET_TRIG_HW_INDEX(trig->state, trig->index);

	بدل (TDATA1_GET_TYPE(tdata1)) {
	حالة RISCV_DBTR_TRIG_MCONTROL:
		𐤀𐤌 (__test_bit(RV_DBTR_BIT(MC, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		𐤀𐤌 (__test_bit(RV_DBTR_BIT(MC, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);
		اكسر;
	حالة RISCV_DBTR_TRIG_MCONTROL6:
		𐤀𐤌 (__test_bit(RV_DBTR_BIT(MC6, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		𐤀𐤌 (__test_bit(RV_DBTR_BIT(MC6, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);

		𐤀𐤌 (__test_bit(RV_DBTR_BIT(MC6, VU), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VU), &trig->state);

		𐤀𐤌 (__test_bit(RV_DBTR_BIT(MC6, VS), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VS), &trig->state);
		اكسر;
	حالة RISCV_DBTR_TRIG_ICOUNT:
		𐤀𐤌 (__test_bit(RV_DBTR_BIT(ICOUNT, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		𐤀𐤌 (__test_bit(RV_DBTR_BIT(ICOUNT, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);

		𐤀𐤌 (__test_bit(RV_DBTR_BIT(ICOUNT, VU), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VU), &trig->state);

		𐤀𐤌 (__test_bit(RV_DBTR_BIT(ICOUNT, VS), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VS), &trig->state);
		اكسر;
	افتراضي:
		sbi_dprintf("%s: Unknown type (tdata1: 0x%lx Type: %ld)\n",
			    __func__, tdata1, TDATA1_GET_TYPE(tdata1));
		اكسر;
	}
}

ساكن مضمن فراغ update_bit(unsigned long new, صحيح nr, متطاير unsigned long *addr)
{
	𐤀𐤌 (new)
		__set_bit(nr, addr);
	وإلا
		__clear_bit(nr, addr);
}

ساكن فراغ dbtr_trigger_enable(بنية sbi_dbtr_trigger *trig)
{
	unsigned long state;
	unsigned long tdata1;

	𐤀𐤌 (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		أرجع;

	state = trig->state;
	tdata1 = trig->tdata1;

	بدل (TDATA1_GET_TYPE(tdata1)) {
	حالة RISCV_DBTR_TRIG_MCONTROL:
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(MC, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(MC, S), &trig->tdata1);
		اكسر;
	حالة RISCV_DBTR_TRIG_MCONTROL6:
		update_bit(state & RV_DBTR_BIT_MASK(TS, VU),
			   RV_DBTR_BIT(MC6, VU), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, VS),
			   RV_DBTR_BIT(MC6, VS), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(MC6, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(MC6, S), &trig->tdata1);
		اكسر;
	حالة RISCV_DBTR_TRIG_ICOUNT:
		update_bit(state & RV_DBTR_BIT_MASK(TS, VU),
			   RV_DBTR_BIT(ICOUNT, VU), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, VS),
			   RV_DBTR_BIT(ICOUNT, VS), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(ICOUNT, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(ICOUNT, S), &trig->tdata1);
		اكسر;
	افتراضي:
		اكسر;
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

ساكن فراغ dbtr_trigger_disable(بنية sbi_dbtr_trigger *trig)
{
	unsigned long tdata1;

	𐤀𐤌 (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		أرجع;

	tdata1 = trig->tdata1;

	بدل (TDATA1_GET_TYPE(tdata1)) {
	حالة RISCV_DBTR_TRIG_MCONTROL:
		__clear_bit(RV_DBTR_BIT(MC, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC, S), &trig->tdata1);
		اكسر;
	حالة RISCV_DBTR_TRIG_MCONTROL6:
		__clear_bit(RV_DBTR_BIT(MC6, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, S), &trig->tdata1);
		اكسر;
	حالة RISCV_DBTR_TRIG_ICOUNT:
		__clear_bit(RV_DBTR_BIT(ICOUNT, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, S), &trig->tdata1);
		اكسر;
	افتراضي:
		اكسر;
	}

	csr_write(CSR_TSELECT, trig->index);
	csr_write(CSR_TDATA1, trig->tdata1);
}

ساكن فراغ dbtr_trigger_clear(بنية sbi_dbtr_trigger *trig)
{
	𐤀𐤌 (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		أرجع;

	csr_write(CSR_TSELECT, trig->index);
	csr_write(CSR_TDATA1, 0x0);
	tdata_write_safe(CSR_TDATA2, 0x0);
	tdata_write_safe(CSR_TDATA3, 0x0);
}

ساكن صحيح dbtr_trigger_supported(unsigned long type)
{
	بدل (type) {
	حالة RISCV_DBTR_TRIG_MCONTROL:
	حالة RISCV_DBTR_TRIG_MCONTROL6:
	حالة RISCV_DBTR_TRIG_ICOUNT:
		أرجع 1;
	افتراضي:
		اكسر;
	}

	أرجع 0;
}

ساكن صحيح dbtr_trigger_valid(unsigned long type, unsigned long tdata)
{
	بدل (type) {
	حالة RISCV_DBTR_TRIG_MCONTROL:
		𐤀𐤌 (!(tdata & RV_DBTR_BIT_MASK(MC, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(MC, M)))
			أرجع 1;
		اكسر;
	حالة RISCV_DBTR_TRIG_MCONTROL6:
		𐤀𐤌 (!(tdata & RV_DBTR_BIT_MASK(MC6, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(MC6, M)))
			أرجع 1;
		اكسر;
	حالة RISCV_DBTR_TRIG_ICOUNT:
		𐤀𐤌 (!(tdata & RV_DBTR_BIT_MASK(ICOUNT, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(ICOUNT, M)))
			أرجع 1;
		اكسر;
	افتراضي:
		اكسر;
	}

	أرجع 0;
}

صحيح sbi_dbtr_num_trig(unsigned long data, unsigned long *out)
{
	unsigned long type = TDATA1_GET_TYPE(data);
	u32 hartid = current_hartid();
	unsigned long total = 0;
	بنية sbi_dbtr_trigger *trig;
	صحيح i;
	بنية sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	𐤀𐤌 (!hs)
		أرجع SBI_ERR_FAILED;

	𐤀𐤌 (data == 0) {
		*out = hs->total_trigs;
		أرجع SBI_SUCCESS;
	}

	لكل (i = 0; i < hs->total_trigs; i++) {
		trig = INDEX_TO_TRIGGER(i);

		𐤀𐤌 (__test_bit(type, &trig->type_mask) &&
		    dbtr_trigger_hw_supported(trig->index, data, 0, 0))
			total++;
	}

	sbi_dprintf("%s: hart%d: total triggers of type %lu: %lu\n",
		    __func__, hartid, type, total);

	*out = total;
	أرجع SBI_SUCCESS;
}

صحيح sbi_dbtr_read_trig(unsigned long smode,
		       unsigned long trig_idx_base, unsigned long trig_count)
{
	بنية sbi_dbtr_data_msg *xmit;
	بنية sbi_dbtr_trigger *trig;
	اتحاد sbi_dbtr_shmem_entry *entry;
	فراغ *shmem_base = عدم;
	بنية sbi_dbtr_hart_triggers_state *hs = عدم;

	hs = dbtr_thishart_state_ptr();
	𐤀𐤌 (!hs)
		أرجع SBI_ERR_FAILED;

	𐤀𐤌 (trig_idx_base >= hs->total_trigs ||
	    trig_idx_base + trig_count >= hs->total_trigs)
		أرجع SBI_ERR_BAD_RANGE;

	𐤀𐤌 (sbi_dbtr_shmem_disabled(hs))
		أرجع SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);

	sbi_hart_protection_temp_map_range((unsigned long)shmem_base,
					   trig_count * حجم(*entry));
	for_each_trig_entry(shmem_base, trig_count, typeof(*entry), entry) {
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
					     trig_count * حجم(*entry));

	أرجع SBI_SUCCESS;
}

صحيح sbi_dbtr_install_trig(unsigned long smode,
			  unsigned long trig_count, unsigned long *out)
{
	فراغ *shmem_base = عدم;
	اتحاد sbi_dbtr_shmem_entry *entry;
	بنية sbi_dbtr_data_msg *recv;
	بنية sbi_dbtr_id_msg *xmit;
	unsigned long ctrl;
	u32 claimed = 0;
	صحيح slot;
	بنية sbi_dbtr_trigger *trig;
	بنية sbi_dbtr_hart_triggers_state *hs = عدم;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	𐤀𐤌 (!hs)
		أرجع SBI_ERR_FAILED;

	𐤀𐤌 (sbi_dbtr_shmem_disabled(hs))
		أرجع SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);
	sbi_hart_protection_temp_map_range((unsigned long)shmem_base,
					   trig_count * حجم(*entry));

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
	for_each_trig_entry(shmem_base, trig_count, typeof(*entry), entry) {
		recv = (بنية sbi_dbtr_data_msg *)(&entry->data);
		ctrl = recv->tdata1;

		𐤀𐤌 (!dbtr_trigger_supported(TDATA1_GET_TYPE(ctrl))) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_count * حجم(*entry));
			أرجع SBI_ERR_FAILED;
		}

		𐤀𐤌 (!dbtr_trigger_valid(TDATA1_GET_TYPE(ctrl), ctrl)) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_count * حجم(*entry));
			أرجع SBI_ERR_INVALID_PARAM;
		}

		𐤀𐤌 ((recv->tdata2 && !tdata2_impl) ||
		    (recv->tdata3 && !tdata3_impl)) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_count * حجم(*entry));
			أرجع SBI_ERR_NOT_SUPPORTED;
		}

		𐤀𐤌 (!dbtr_trigger_any_hw_supported(hs,
						   lle_to_cpu(recv->tdata1),
						   lle_to_cpu(recv->tdata2),
						   lle_to_cpu(recv->tdata3))) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_count * حجم(*entry));
			أرجع SBI_ERR_NOT_SUPPORTED;
		}
	}

	𐤀𐤌 (hs->available_trigs < trig_count) {
		*out = hs->available_trigs;
		sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
						     trig_count * حجم(*entry));
		أرجع SBI_ERR_FAILED;
	}

	/*
	 * Dry-run the allocation of the whole batch so that no trigger
	 * is installed if any of the requested configurations cannot be
	 * matched to a free hardware trigger slot.
	 */
	for_each_trig_entry(shmem_base, trig_count, typeof(*entry), entry) {
		recv = (بنية sbi_dbtr_data_msg *)(&entry->data);
		slot = dbtr_find_free_slot(hs, claimed,
					   lle_to_cpu(recv->tdata1),
					   lle_to_cpu(recv->tdata2),
					   lle_to_cpu(recv->tdata3));
		𐤀𐤌 (slot < 0) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_count * حجم(*entry));
			أرجع SBI_ERR_FAILED;
		}
		claimed |= BIT(slot);
	}

	/* Install triggers */
	for_each_trig_entry(shmem_base, trig_count, typeof(*entry), entry) {
		recv = (بنية sbi_dbtr_data_msg *)(&entry->data);
		xmit = (بنية sbi_dbtr_id_msg *)(&entry->id);

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
					     trig_count * حجم(*entry));

	أرجع SBI_SUCCESS;
}

صحيح sbi_dbtr_uninstall_trig(unsigned long trig_idx_base,
			    unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	بنية sbi_dbtr_trigger *trig;
	بنية sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	𐤀𐤌 (!hs)
		أرجع SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->total_trigs) {
		trig = INDEX_TO_TRIGGER(idx);
		𐤀𐤌 (!(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
			أرجع SBI_ERR_INVALID_PARAM;

		dbtr_trigger_clear(trig);

		sbi_free_trigger(trig);
	}

	أرجع SBI_SUCCESS;
}

صحيح sbi_dbtr_enable_trig(unsigned long trig_idx_base,
			 unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	بنية sbi_dbtr_trigger *trig;
	بنية sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	𐤀𐤌 (!hs)
		أرجع SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->total_trigs) {
		trig = INDEX_TO_TRIGGER(idx);
		sbi_dprintf("%s: enable trigger %lu\n", __func__, idx);
		dbtr_trigger_enable(trig);
	}

	أرجع SBI_SUCCESS;
}

صحيح sbi_dbtr_update_trig(unsigned long smode,
			 unsigned long trig_count)
{
	unsigned long trig_idx;
	بنية sbi_dbtr_trigger *trig;
	اتحاد sbi_dbtr_shmem_entry *entry;
	فراغ *shmem_base = عدم;
	بنية sbi_dbtr_hart_triggers_state *hs = عدم;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	𐤀𐤌 (!hs)
		أرجع SBI_ERR_FAILED;

	𐤀𐤌 (sbi_dbtr_shmem_disabled(hs))
		أرجع SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);

	𐤀𐤌 (trig_count >= hs->total_trigs)
		أرجع SBI_ERR_BAD_RANGE;

	/*
	 * SBI v3.0 sec 19.5 requires SBI_ERR_NOT_SUPPORTED when a trigger
	 * programs a non-zero value into an unimplemented optional CSR. Only
	 * the "whole CSR unimplemented" case is caught; WARL bits tied off
	 * inside an otherwise-implemented CSR are not.
	 */
	tdata2_impl = tdata_implemented(CSR_TDATA2);
	tdata3_impl = tdata_implemented(CSR_TDATA3);

	for_each_trig_entry(shmem_base, trig_count, typeof(*entry), entry) {
		sbi_hart_protection_temp_map_range((unsigned long)entry, حجم(*entry));
		trig_idx = entry->id.idx;

		𐤀𐤌 (trig_idx >= hs->total_trigs) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, حجم(*entry));
			أرجع SBI_ERR_INVALID_PARAM;
		}

		trig = INDEX_TO_TRIGGER(trig_idx);

		𐤀𐤌 (!(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED))) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, حجم(*entry));
			أرجع SBI_ERR_FAILED;
		}

		𐤀𐤌 ((entry->data.tdata2 && !tdata2_impl) ||
		    (entry->data.tdata3 && !tdata3_impl)) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, حجم(*entry));
			أرجع SBI_ERR_NOT_SUPPORTED;
		}

		𐤀𐤌 (!dbtr_trigger_hw_supported(trig->index,
					       lle_to_cpu(entry->data.tdata1),
					       lle_to_cpu(entry->data.tdata2),
					       lle_to_cpu(entry->data.tdata3))) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, حجم(*entry));
			أرجع SBI_ERR_NOT_SUPPORTED;
		}

		dbtr_trigger_setup(trig, &entry->data);
		sbi_hart_protection_temp_unmap_range((unsigned long)entry, حجم(*entry));
		dbtr_trigger_enable(trig);
	}

	أرجع SBI_SUCCESS;
}

صحيح sbi_dbtr_disable_trig(unsigned long trig_idx_base,
			  unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	بنية sbi_dbtr_trigger *trig;
	بنية sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	𐤀𐤌 (!hs)
		أرجع SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->total_trigs) {
		trig = INDEX_TO_TRIGGER(idx);
		dbtr_trigger_disable(trig);
	}

	أرجع SBI_SUCCESS;
}
