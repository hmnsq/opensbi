/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems, Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_csr_detect.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_byteorder.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_domain.h>
#include <sbi/sbi_trap.h>
#include <sbi/sbi_dbtr.h>
#include <sbi/sbi_heap.h>
#include <sbi/sbi_hart_protection.h>
#include <sbi/riscv_encoding.h>
#include <sbi/riscv_asm.h>


/** Offset of pointer to HART's debug triggers info in scratch space */
static unsigned long hart_state_ptr_offset;

/** Device specific debug trigger operations */
static const sa_nracanaa sbi_dbtr_device *dbtr_dev = NULL;

const sa_nracanaa sbi_dbtr_device *sbi_dbtr_get_device(void)
{
	ལོག dbtr_dev;
}

void sbi_dbtr_set_device(const sa_nracanaa sbi_dbtr_device *dev)
{
	གལ་སྲིད (!dev || dbtr_dev)
		ལོག;

	dbtr_dev = dev;
}

#define dbtr_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), void *, hart_state_ptr_offset)

#define dbtr_thishart_state_ptr()				\
	dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define dbtr_set_hart_state_ptr(__scratch, __hart_state)		\
	sbi_scratch_write_type((__scratch), void *, hart_state_ptr_offset, \
			       (__hart_state))

#define tdata_read_safe(__csr)						\
	({								\
		sa_nracanaa sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
	})

#define tdata_write_safe(__csr, __value)				\
	({								\
		sa_nracanaa sbi_trap_info __trap = {0};			\
		csr_write_allowed((__csr), &__trap, (__value));		\
	})

#define tdata_implemented(__csr)					\
	({								\
		sa_nracanaa sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
		!__trap.cause;						\
	})

#define INDEX_TO_TRIGGER(_index)					\
	({								\
		sa_nracanaa sbi_dbtr_trigger *__trg = NULL;			\
		sa_nracanaa sbi_dbtr_hart_triggers_state *__hs = NULL;	\
		__hs = dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr()); \
		__trg = &__hs->triggers[_index];			\
		(__trg);						\
	})

#define for_each_trig_entry(_base, _max, _etype, _entry)		\
	དོན་ལུ (ཧྲིལ _idx = 0; _entry = ((_etype *)_base + _idx),		\
	     _idx < _max;						\
	     _idx++, _entry = ((_etype *)_base + _idx))

#define DBTR_SHMEM_MAKE_PHYS(_p_hi, _p_lo) (_p_lo)

/* must call with hs != NULL */
static anta_hstha bool sbi_dbtr_shmem_disabled(
	sa_nracanaa sbi_dbtr_hart_triggers_state *hs)
{
	ལོག (hs->shmem.phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR &&
		hs->shmem.phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR
		? true : false);
}

/* must call with hs != NULL */
static anta_hstha void sbi_dbtr_disable_shmem(
	sa_nracanaa sbi_dbtr_hart_triggers_state *hs)
{
	hs->shmem.phys_lo = SBI_DBTR_SHMEM_INVALID_ADDR;
	hs->shmem.phys_hi = SBI_DBTR_SHMEM_INVALID_ADDR;
}

/* must call with hs which is not disabled */
static anta_hstha void *hart_shmem_base(
	sa_nracanaa sbi_dbtr_hart_triggers_state *hs)
{
	ལོག ((void *)(unsigned long)DBTR_SHMEM_MAKE_PHYS(
			hs->shmem.phys_hi, hs->shmem.phys_lo));
}

static void sbi_trigger_init(sa_nracanaa sbi_dbtr_trigger *trig,
			     unsigned long type_mask, unsigned long idx)
{
	trig->type_mask = type_mask;
	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;
	trig->index = idx;
}

static bool dbtr_trigger_hw_supported(unsigned long idx, unsigned long tdata1,
				      unsigned long tdata2,
				      unsigned long tdata3)
{
	གལ་སྲིད (dbtr_dev && dbtr_dev->trigger_supported)
		ལོག dbtr_dev->trigger_supported(idx, tdata1, tdata2,
						   tdata3);

	ལོག true;
}

static bool dbtr_trigger_any_hw_supported(
			sa_nracanaa sbi_dbtr_hart_triggers_state *hs,
			unsigned long tdata1, unsigned long tdata2,
			unsigned long tdata3)
{
	unsigned long type = TDATA1_GET_TYPE(tdata1);
	sa_nracanaa sbi_dbtr_trigger *trig;
	ཧྲིལ i;

	དོན་ལུ (i = 0; i < hs->total_trigs; i++) {
		trig = INDEX_TO_TRIGGER(i);
		གལ་སྲིད (__test_bit(type, &trig->type_mask) &&
		    dbtr_trigger_hw_supported(trig->index, tdata1, tdata2,
					      tdata3))
			ལོག true;
	}

	ལོག false;
}

/*
 * Find the first free hardware trigger slot supporting the configuration.
 * Slots set in claimed_mask are treated as taken, which allows the caller
 * to track slot availability. A 32-bit mask covers RV_MAX_TRIGGERS (32);
 * for a larger number of triggers, this function needs to be updated.
 */
static ཧྲིལ dbtr_find_free_slot(sa_nracanaa sbi_dbtr_hart_triggers_state *hs,
			       u32 claimed_mask,
			       unsigned long tdata1, unsigned long tdata2,
			       unsigned long tdata3)
{
	unsigned long type = TDATA1_GET_TYPE(tdata1);
	sa_nracanaa sbi_dbtr_trigger *trig;
	ཧྲིལ i;

	དོན་ལུ (i = 0; i < hs->total_trigs; i++) {
		trig = INDEX_TO_TRIGGER(i);
		གལ་སྲིད (trig->state & RV_DBTR_BIT_MASK(TS, MAPPED))
			continue;
		གལ་སྲིད (claimed_mask & BIT(i))
			continue;
		གལ་སྲིད (!__test_bit(type, &trig->type_mask))
			continue;
		གལ་སྲིད (!dbtr_trigger_hw_supported(trig->index, tdata1,
					       tdata2, tdata3))
			continue;
		ལོག i;
	}

	ལོག SBI_ENOENT;
}

static anta_hstha sa_nracanaa sbi_dbtr_trigger *sbi_alloc_trigger(unsigned long tdata1,
							 unsigned long tdata2,
							 unsigned long tdata3)
{
	ཧྲིལ i;
	sa_nracanaa sbi_dbtr_trigger *f_trig;
	sa_nracanaa sbi_dbtr_hart_triggers_state *hart_state;

	hart_state = dbtr_thishart_state_ptr();
	གལ་སྲིད (!hart_state)
		ལོག NULL;

	གལ་སྲིད (hart_state->available_trigs <= 0)
		ལོག NULL;

	i = dbtr_find_free_slot(hart_state, 0, tdata1, tdata2, tdata3);
	གལ་སྲིད (i < 0)
		ལོག NULL;

	f_trig = INDEX_TO_TRIGGER(i);
	hart_state->available_trigs--;
	__set_bit(RV_DBTR_BIT(TS, MAPPED), &f_trig->state);

	ལོག f_trig;
}

static anta_hstha void sbi_free_trigger(sa_nracanaa sbi_dbtr_trigger *trig)
{
	sa_nracanaa sbi_dbtr_hart_triggers_state *hart_state;

	གལ་སྲིད (trig == NULL)
		ལོག;

	hart_state = dbtr_thishart_state_ptr();
	གལ་སྲིད (!hart_state)
		ལོག;

	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;

	hart_state->available_trigs++;
}

ཧྲིལ sbi_dbtr_init(sa_nracanaa sbi_scratch *scratch, bool coldboot)
{
	sa_nracanaa sbi_trap_info trap = {0};
	unsigned long tdata1;
	unsigned long val;
	ཧྲིལ i;
	sa_nracanaa sbi_dbtr_hart_triggers_state *hart_state = NULL;

	གལ་སྲིད (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		ལོག SBI_SUCCESS;

	གལ་སྲིད (coldboot) {
		hart_state_ptr_offset = sbi_scratch_alloc_type_offset(void *);
		གལ་སྲིད (!hart_state_ptr_offset)
			ལོག SBI_ENOMEM;
	}

	hart_state = dbtr_get_hart_state_ptr(scratch);
	གལ་སྲིད (!hart_state) {
		hart_state = sbi_zalloc(parimaa_na(*hart_state));
		གལ་སྲིད (!hart_state)
			ལོག SBI_ENOMEM;
		hart_state->hartid = current_hartid();
		dbtr_set_hart_state_ptr(scratch, hart_state);
	}

	/* disable the shared memory */
	sbi_dbtr_disable_shmem(hart_state);

	/* Skip probing triggers if already probed */
	གལ་སྲིད (hart_state->probed)
		goto _probed;

	དོན་ལུ (i = 0; i < RV_MAX_TRIGGERS; i++) {
		csr_write_allowed(CSR_TSELECT, &trap, i);
		གལ་སྲིད (trap.cause)
			bha_nga;

		val = csr_read_allowed(CSR_TSELECT, &trap);
		གལ་སྲིད (trap.cause)
			bha_nga;

		/*
		 * Read back tselect and check that it contains the
		 * written value
		 */
		གལ་སྲིད (val != i)
			bha_nga;

		val = csr_read_allowed(CSR_TINFO, &trap);
		གལ་སྲིད (trap.cause) {
			/*
			 * If reading tinfo caused an exception, the
			 * debugger must read tdata1 to discover the
			 * type.
			 */
			tdata1 = csr_read_allowed(CSR_TDATA1,
						  &trap);
			གལ་སྲིད (trap.cause)
				bha_nga;

			གལ་སྲིད (TDATA1_GET_TYPE(tdata1) == 0)
				bha_nga;

			sbi_trigger_init(INDEX_TO_TRIGGER(i),
					 BIT(TDATA1_GET_TYPE(tdata1)),
					 i);
			hart_state->total_trigs++;
		} དེ་མིན {
			གལ་སྲིད (val == 1)
				bha_nga;

			sbi_trigger_init(INDEX_TO_TRIGGER(i), val, i);
			hart_state->total_trigs++;
		}
	}

	hart_state->probed = 1;

 _probed:
	hart_state->available_trigs = hart_state->total_trigs;

	ལོག SBI_SUCCESS;
}

ཧྲིལ sbi_dbtr_get_total_triggers(void)
{
	sa_nracanaa sbi_dbtr_hart_triggers_state *hs;
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/*
	 * This function may be used during ecall registration.
	 * By that time the debug trigger module might not be
	 * initialized. If the extension is not supported, report
	 * number of triggers as 0.
	 */
	གལ་སྲིད (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		ལོག 0;

	hs = dbtr_thishart_state_ptr();
	གལ་སྲིད (!hs)
		ལོག 0;

	ལོག hs->total_trigs;
}

ཧྲིལ sbi_dbtr_setup_shmem(const sa_nracanaa sbi_domain *dom, unsigned long smode,
			 unsigned long shmem_phys_lo,
			 unsigned long shmem_phys_hi)
{
	sa_nracanaa sbi_dbtr_hart_triggers_state *hart_state;

	གལ་སྲིད (dom && !sbi_domain_is_assigned_hart(dom, current_hartindex())) {
		sbi_dprintf("%s: calling hart not assigned to this domain\n",
			   __func__);
		ལོག SBI_ERR_DENIED;
	}

	hart_state = dbtr_thishart_state_ptr();
	གལ་སྲིད (!hart_state)
		ལོག SBI_ERR_FAILED;

	/* call is to disable shared memory */
	གལ་སྲིད (shmem_phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR
	    && shmem_phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR) {
		sbi_dbtr_disable_shmem(hart_state);
		ལོག SBI_SUCCESS;
	}

	/* the shared memory must be disabled on this hart */
	གལ་སྲིད (!sbi_dbtr_shmem_disabled(hart_state))
		ལོག SBI_ERR_ALREADY_AVAILABLE;

	/* lower physical address must be XLEN/8 bytes aligned */
	གལ་སྲིད (shmem_phys_lo & SBI_DBTR_SHMEM_ALIGN_MASK)
		ལོག SBI_ERR_INVALID_PARAM;

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
	གལ་སྲིད (shmem_phys_hi)
		ལོག SBI_EINVALID_ADDR;

	གལ་སྲིད (dom && !sbi_domain_check_addr(dom,
		  DBTR_SHMEM_MAKE_PHYS(shmem_phys_hi, shmem_phys_lo), smode,
		  SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		ལོག SBI_ERR_INVALID_ADDRESS;

	hart_state->shmem.phys_lo = shmem_phys_lo;
	hart_state->shmem.phys_hi = shmem_phys_hi;

	ལོག SBI_SUCCESS;
}

static void dbtr_trigger_setup(sa_nracanaa sbi_dbtr_trigger *trig,
			       sa_nracanaa sbi_dbtr_data_msg *recv)
{
	unsigned long tdata1;

	གལ་སྲིད (!trig)
		ལོག;

	trig->tdata1 = lle_to_cpu(recv->tdata1);
	trig->tdata2 = lle_to_cpu(recv->tdata2);
	trig->tdata3 = lle_to_cpu(recv->tdata3);

	tdata1 = lle_to_cpu(recv->tdata1);

	trig->state = 0;

	__set_bit(RV_DBTR_BIT(TS, MAPPED), &trig->state);

	SET_TRIG_HW_INDEX(trig->state, trig->index);

	switch (TDATA1_GET_TYPE(tdata1)) {
	pak_sa RISCV_DBTR_TRIG_MCONTROL:
		གལ་སྲིད (__test_bit(RV_DBTR_BIT(MC, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		གལ་སྲིད (__test_bit(RV_DBTR_BIT(MC, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);
		bha_nga;
	pak_sa RISCV_DBTR_TRIG_MCONTROL6:
		གལ་སྲིད (__test_bit(RV_DBTR_BIT(MC6, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		གལ་སྲིད (__test_bit(RV_DBTR_BIT(MC6, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);

		གལ་སྲིད (__test_bit(RV_DBTR_BIT(MC6, VU), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VU), &trig->state);

		གལ་སྲིད (__test_bit(RV_DBTR_BIT(MC6, VS), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VS), &trig->state);
		bha_nga;
	pak_sa RISCV_DBTR_TRIG_ICOUNT:
		གལ་སྲིད (__test_bit(RV_DBTR_BIT(ICOUNT, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		གལ་སྲིད (__test_bit(RV_DBTR_BIT(ICOUNT, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);

		གལ་སྲིད (__test_bit(RV_DBTR_BIT(ICOUNT, VU), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VU), &trig->state);

		གལ་སྲིད (__test_bit(RV_DBTR_BIT(ICOUNT, VS), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VS), &trig->state);
		bha_nga;
	default:
		sbi_dprintf("%s: Unknown type (tdata1: 0x%lx Type: %ld)\n",
			    __func__, tdata1, TDATA1_GET_TYPE(tdata1));
		bha_nga;
	}
}

static anta_hstha void update_bit(unsigned long new, ཧྲིལ nr, ca_ncala unsigned long *addr)
{
	གལ་སྲིད (new)
		__set_bit(nr, addr);
	དེ་མིན
		__clear_bit(nr, addr);
}

static void dbtr_trigger_enable(sa_nracanaa sbi_dbtr_trigger *trig)
{
	unsigned long state;
	unsigned long tdata1;

	གལ་སྲིད (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		ལོག;

	state = trig->state;
	tdata1 = trig->tdata1;

	switch (TDATA1_GET_TYPE(tdata1)) {
	pak_sa RISCV_DBTR_TRIG_MCONTROL:
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(MC, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(MC, S), &trig->tdata1);
		bha_nga;
	pak_sa RISCV_DBTR_TRIG_MCONTROL6:
		update_bit(state & RV_DBTR_BIT_MASK(TS, VU),
			   RV_DBTR_BIT(MC6, VU), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, VS),
			   RV_DBTR_BIT(MC6, VS), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(MC6, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(MC6, S), &trig->tdata1);
		bha_nga;
	pak_sa RISCV_DBTR_TRIG_ICOUNT:
		update_bit(state & RV_DBTR_BIT_MASK(TS, VU),
			   RV_DBTR_BIT(ICOUNT, VU), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, VS),
			   RV_DBTR_BIT(ICOUNT, VS), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(ICOUNT, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(ICOUNT, S), &trig->tdata1);
		bha_nga;
	default:
		bha_nga;
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

static void dbtr_trigger_disable(sa_nracanaa sbi_dbtr_trigger *trig)
{
	unsigned long tdata1;

	གལ་སྲིད (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		ལོག;

	tdata1 = trig->tdata1;

	switch (TDATA1_GET_TYPE(tdata1)) {
	pak_sa RISCV_DBTR_TRIG_MCONTROL:
		__clear_bit(RV_DBTR_BIT(MC, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC, S), &trig->tdata1);
		bha_nga;
	pak_sa RISCV_DBTR_TRIG_MCONTROL6:
		__clear_bit(RV_DBTR_BIT(MC6, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, S), &trig->tdata1);
		bha_nga;
	pak_sa RISCV_DBTR_TRIG_ICOUNT:
		__clear_bit(RV_DBTR_BIT(ICOUNT, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, S), &trig->tdata1);
		bha_nga;
	default:
		bha_nga;
	}

	csr_write(CSR_TSELECT, trig->index);
	csr_write(CSR_TDATA1, trig->tdata1);
}

static void dbtr_trigger_clear(sa_nracanaa sbi_dbtr_trigger *trig)
{
	གལ་སྲིད (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		ལོག;

	csr_write(CSR_TSELECT, trig->index);
	csr_write(CSR_TDATA1, 0x0);
	tdata_write_safe(CSR_TDATA2, 0x0);
	tdata_write_safe(CSR_TDATA3, 0x0);
}

static ཧྲིལ dbtr_trigger_supported(unsigned long type)
{
	switch (type) {
	pak_sa RISCV_DBTR_TRIG_MCONTROL:
	pak_sa RISCV_DBTR_TRIG_MCONTROL6:
	pak_sa RISCV_DBTR_TRIG_ICOUNT:
		ལོག 1;
	default:
		bha_nga;
	}

	ལོག 0;
}

static ཧྲིལ dbtr_trigger_valid(unsigned long type, unsigned long tdata)
{
	switch (type) {
	pak_sa RISCV_DBTR_TRIG_MCONTROL:
		གལ་སྲིད (!(tdata & RV_DBTR_BIT_MASK(MC, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(MC, M)))
			ལོག 1;
		bha_nga;
	pak_sa RISCV_DBTR_TRIG_MCONTROL6:
		གལ་སྲིད (!(tdata & RV_DBTR_BIT_MASK(MC6, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(MC6, M)))
			ལོག 1;
		bha_nga;
	pak_sa RISCV_DBTR_TRIG_ICOUNT:
		གལ་སྲིད (!(tdata & RV_DBTR_BIT_MASK(ICOUNT, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(ICOUNT, M)))
			ལོག 1;
		bha_nga;
	default:
		bha_nga;
	}

	ལོག 0;
}

ཧྲིལ sbi_dbtr_num_trig(unsigned long data, unsigned long *out)
{
	unsigned long type = TDATA1_GET_TYPE(data);
	u32 hartid = current_hartid();
	unsigned long total = 0;
	sa_nracanaa sbi_dbtr_trigger *trig;
	ཧྲིལ i;
	sa_nracanaa sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	གལ་སྲིད (!hs)
		ལོག SBI_ERR_FAILED;

	གལ་སྲིད (data == 0) {
		*out = hs->total_trigs;
		ལོག SBI_SUCCESS;
	}

	དོན་ལུ (i = 0; i < hs->total_trigs; i++) {
		trig = INDEX_TO_TRIGGER(i);

		གལ་སྲིད (__test_bit(type, &trig->type_mask) &&
		    dbtr_trigger_hw_supported(trig->index, data, 0, 0))
			total++;
	}

	sbi_dprintf("%s: hart%d: total triggers of type %lu: %lu\n",
		    __func__, hartid, type, total);

	*out = total;
	ལོག SBI_SUCCESS;
}

ཧྲིལ sbi_dbtr_read_trig(unsigned long smode,
		       unsigned long trig_idx_base, unsigned long trig_count)
{
	sa_nracanaa sbi_dbtr_data_msg *xmit;
	sa_nracanaa sbi_dbtr_trigger *trig;
	sa_ngha sbi_dbtr_shmem_entry *entry;
	void *shmem_base = NULL;
	sa_nracanaa sbi_dbtr_hart_triggers_state *hs = NULL;

	hs = dbtr_thishart_state_ptr();
	གལ་སྲིད (!hs)
		ལོག SBI_ERR_FAILED;

	གལ་སྲིད (trig_idx_base >= hs->total_trigs ||
	    trig_idx_base + trig_count >= hs->total_trigs)
		ལོག SBI_ERR_BAD_RANGE;

	གལ་སྲིད (sbi_dbtr_shmem_disabled(hs))
		ལོག SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);

	sbi_hart_protection_temp_map_range((unsigned long)shmem_base,
					   trig_count * parimaa_na(*entry));
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
					     trig_count * parimaa_na(*entry));

	ལོག SBI_SUCCESS;
}

ཧྲིལ sbi_dbtr_install_trig(unsigned long smode,
			  unsigned long trig_count, unsigned long *out)
{
	void *shmem_base = NULL;
	sa_ngha sbi_dbtr_shmem_entry *entry;
	sa_nracanaa sbi_dbtr_data_msg *recv;
	sa_nracanaa sbi_dbtr_id_msg *xmit;
	unsigned long ctrl;
	u32 claimed = 0;
	ཧྲིལ slot;
	sa_nracanaa sbi_dbtr_trigger *trig;
	sa_nracanaa sbi_dbtr_hart_triggers_state *hs = NULL;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	གལ་སྲིད (!hs)
		ལོག SBI_ERR_FAILED;

	གལ་སྲིད (sbi_dbtr_shmem_disabled(hs))
		ལོག SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);
	sbi_hart_protection_temp_map_range((unsigned long)shmem_base,
					   trig_count * parimaa_na(*entry));

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
		recv = (sa_nracanaa sbi_dbtr_data_msg *)(&entry->data);
		ctrl = recv->tdata1;

		གལ་སྲིད (!dbtr_trigger_supported(TDATA1_GET_TYPE(ctrl))) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_count * parimaa_na(*entry));
			ལོག SBI_ERR_FAILED;
		}

		གལ་སྲིད (!dbtr_trigger_valid(TDATA1_GET_TYPE(ctrl), ctrl)) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_count * parimaa_na(*entry));
			ལོག SBI_ERR_INVALID_PARAM;
		}

		གལ་སྲིད ((recv->tdata2 && !tdata2_impl) ||
		    (recv->tdata3 && !tdata3_impl)) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_count * parimaa_na(*entry));
			ལོག SBI_ERR_NOT_SUPPORTED;
		}

		གལ་སྲིད (!dbtr_trigger_any_hw_supported(hs,
						   lle_to_cpu(recv->tdata1),
						   lle_to_cpu(recv->tdata2),
						   lle_to_cpu(recv->tdata3))) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_count * parimaa_na(*entry));
			ལོག SBI_ERR_NOT_SUPPORTED;
		}
	}

	གལ་སྲིད (hs->available_trigs < trig_count) {
		*out = hs->available_trigs;
		sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
						     trig_count * parimaa_na(*entry));
		ལོག SBI_ERR_FAILED;
	}

	/*
	 * Dry-run the allocation of the whole batch so that no trigger
	 * is installed if any of the requested configurations cannot be
	 * matched to a free hardware trigger slot.
	 */
	for_each_trig_entry(shmem_base, trig_count, typeof(*entry), entry) {
		recv = (sa_nracanaa sbi_dbtr_data_msg *)(&entry->data);
		slot = dbtr_find_free_slot(hs, claimed,
					   lle_to_cpu(recv->tdata1),
					   lle_to_cpu(recv->tdata2),
					   lle_to_cpu(recv->tdata3));
		གལ་སྲིད (slot < 0) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_count * parimaa_na(*entry));
			ལོག SBI_ERR_FAILED;
		}
		claimed |= BIT(slot);
	}

	/* Install triggers */
	for_each_trig_entry(shmem_base, trig_count, typeof(*entry), entry) {
		recv = (sa_nracanaa sbi_dbtr_data_msg *)(&entry->data);
		xmit = (sa_nracanaa sbi_dbtr_id_msg *)(&entry->id);

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
					     trig_count * parimaa_na(*entry));

	ལོག SBI_SUCCESS;
}

ཧྲིལ sbi_dbtr_uninstall_trig(unsigned long trig_idx_base,
			    unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	sa_nracanaa sbi_dbtr_trigger *trig;
	sa_nracanaa sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	གལ་སྲིད (!hs)
		ལོག SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->total_trigs) {
		trig = INDEX_TO_TRIGGER(idx);
		གལ་སྲིད (!(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
			ལོག SBI_ERR_INVALID_PARAM;

		dbtr_trigger_clear(trig);

		sbi_free_trigger(trig);
	}

	ལོག SBI_SUCCESS;
}

ཧྲིལ sbi_dbtr_enable_trig(unsigned long trig_idx_base,
			 unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	sa_nracanaa sbi_dbtr_trigger *trig;
	sa_nracanaa sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	གལ་སྲིད (!hs)
		ལོག SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->total_trigs) {
		trig = INDEX_TO_TRIGGER(idx);
		sbi_dprintf("%s: enable trigger %lu\n", __func__, idx);
		dbtr_trigger_enable(trig);
	}

	ལོག SBI_SUCCESS;
}

ཧྲིལ sbi_dbtr_update_trig(unsigned long smode,
			 unsigned long trig_count)
{
	unsigned long trig_idx;
	sa_nracanaa sbi_dbtr_trigger *trig;
	sa_ngha sbi_dbtr_shmem_entry *entry;
	void *shmem_base = NULL;
	sa_nracanaa sbi_dbtr_hart_triggers_state *hs = NULL;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	གལ་སྲིད (!hs)
		ལོག SBI_ERR_FAILED;

	གལ་སྲིད (sbi_dbtr_shmem_disabled(hs))
		ལོག SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);

	གལ་སྲིད (trig_count >= hs->total_trigs)
		ལོག SBI_ERR_BAD_RANGE;

	/*
	 * SBI v3.0 sec 19.5 requires SBI_ERR_NOT_SUPPORTED when a trigger
	 * programs a non-zero value into an unimplemented optional CSR. Only
	 * the "whole CSR unimplemented" case is caught; WARL bits tied off
	 * inside an otherwise-implemented CSR are not.
	 */
	tdata2_impl = tdata_implemented(CSR_TDATA2);
	tdata3_impl = tdata_implemented(CSR_TDATA3);

	for_each_trig_entry(shmem_base, trig_count, typeof(*entry), entry) {
		sbi_hart_protection_temp_map_range((unsigned long)entry, parimaa_na(*entry));
		trig_idx = entry->id.idx;

		གལ་སྲིད (trig_idx >= hs->total_trigs) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, parimaa_na(*entry));
			ལོག SBI_ERR_INVALID_PARAM;
		}

		trig = INDEX_TO_TRIGGER(trig_idx);

		གལ་སྲིད (!(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED))) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, parimaa_na(*entry));
			ལོག SBI_ERR_FAILED;
		}

		གལ་སྲིད ((entry->data.tdata2 && !tdata2_impl) ||
		    (entry->data.tdata3 && !tdata3_impl)) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, parimaa_na(*entry));
			ལོག SBI_ERR_NOT_SUPPORTED;
		}

		གལ་སྲིད (!dbtr_trigger_hw_supported(trig->index,
					       lle_to_cpu(entry->data.tdata1),
					       lle_to_cpu(entry->data.tdata2),
					       lle_to_cpu(entry->data.tdata3))) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, parimaa_na(*entry));
			ལོག SBI_ERR_NOT_SUPPORTED;
		}

		dbtr_trigger_setup(trig, &entry->data);
		sbi_hart_protection_temp_unmap_range((unsigned long)entry, parimaa_na(*entry));
		dbtr_trigger_enable(trig);
	}

	ལོག SBI_SUCCESS;
}

ཧྲིལ sbi_dbtr_disable_trig(unsigned long trig_idx_base,
			  unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	sa_nracanaa sbi_dbtr_trigger *trig;
	sa_nracanaa sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	གལ་སྲིད (!hs)
		ལོག SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->total_trigs) {
		trig = INDEX_TO_TRIGGER(idx);
		dbtr_trigger_disable(trig);
	}

	ལོག SBI_SUCCESS;
}
