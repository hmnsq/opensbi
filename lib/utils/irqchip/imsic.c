/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/riscv_encoding.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_csr_detect.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_ipi.h>
#അന്തര്ഭാവ <sbi/sbi_irqchip.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi_utils/irqchip/imsic.h>

#define IMSIC_MMIO_PAGE_LE		0x00
#define IMSIC_MMIO_PAGE_BE		0x04

#define IMSIC_MIN_ID			63
#define IMSIC_MAX_ID			2047

#define IMSIC_EIDELIVERY		0x70

#define IMSIC_EITHRESHOLD		0x72

#define IMSIC_TOPEI			0x76
#define IMSIC_TOPEI_ID_SHIFT		16
#define IMSIC_TOPEI_ID_MASK		0x7ff
#define IMSIC_TOPEI_PRIO_MASK		0x7ff

#define IMSIC_EIP0			0x80

#define IMSIC_EIP63			0xbf

#define IMSIC_EIPx_BITS			32

#define IMSIC_EIE0			0xc0

#define IMSIC_EIE63			0xff

#define IMSIC_EIEx_BITS			32

#define IMSIC_DISABLE_EIDELIVERY	0
#define IMSIC_ENABLE_EIDELIVERY		1
#define IMSIC_DISABLE_EITHRESHOLD	1
#define IMSIC_ENABLE_EITHRESHOLD	0

#define IMSIC_IPI_ID			1

#define imsic_csr_write(__c, __v)	\
ചെയ്യുക { \
	csr_write(CSR_MISELECT, __c); \
	csr_write(CSR_MIREG, __v); \
} ആകുമ്പോൾ (0)

#define imsic_csr_read(__c)	\
({ \
	unsigned long __v; \
	csr_write(CSR_MISELECT, __c); \
	__v = csr_read(CSR_MIREG); \
	__v; \
})

#define imsic_csr_set(__c, __v)		\
ചെയ്യുക { \
	csr_write(CSR_MISELECT, __c); \
	csr_set(CSR_MIREG, __v); \
} ആകുമ്പോൾ (0)

#define imsic_csr_clear(__c, __v)	\
ചെയ്യുക { \
	csr_write(CSR_MISELECT, __c); \
	csr_clear(CSR_MIREG, __v); \
} ആകുമ്പോൾ (0)

സ്ഥിതം unsigned long imsic_ptr_offset;

#define imsic_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ശൂന്യം *, imsic_ptr_offset)

#define imsic_set_hart_data_ptr(__scratch, __imsic)			\
	sbi_scratch_write_type((__scratch), ശൂന്യം *, imsic_ptr_offset, (__imsic))

സ്ഥിതം unsigned long imsic_file_offset;

#define imsic_get_hart_file(__scratch)					\
	sbi_scratch_read_type((__scratch), long, imsic_file_offset)

#define imsic_set_hart_file(__scratch, __file)				\
	sbi_scratch_write_type((__scratch), long, imsic_file_offset, (__file))

പൂർണ്ണസംഖ്യ imsic_map_hartid_to_data(u32 hartid, ഘടന imsic_data *imsic, പൂർണ്ണസംഖ്യ file)
{
	ഘടന sbi_scratch *scratch;

	എങ്കിൽ (!imsic || !imsic->targets_mmode)
		തിരികെ SBI_EINVAL;

	/*
	 * We don't need to fail if scratch pointer is not available
	 * because we might be dealing with hartid of a HART disabled
	 * in device tree. For HARTs disabled in device tree, the
	 * imsic_get_data() and imsic_get_target_file() will anyway
	 * fail.
	 */
	scratch = sbi_hartid_to_scratch(hartid);
	എങ്കിൽ (!scratch)
		തിരികെ 0;

	imsic_set_hart_data_ptr(scratch, imsic);
	imsic_set_hart_file(scratch, file);
	തിരികെ 0;
}

ഘടന imsic_data *imsic_get_data(u32 hartindex)
{
	ഘടന sbi_scratch *scratch;

	എങ്കിൽ (!imsic_ptr_offset)
		തിരികെ ശൂന്യ;

	scratch = sbi_hartindex_to_scratch(hartindex);
	എങ്കിൽ (!scratch)
		തിരികെ ശൂന്യ;

	തിരികെ imsic_get_hart_data_ptr(scratch);
}

പൂർണ്ണസംഖ്യ imsic_get_target_file(u32 hartindex)
{
	ഘടന sbi_scratch *scratch;

	എങ്കിൽ (!imsic_file_offset)
		തിരികെ SBI_ENOENT;

	scratch = sbi_hartindex_to_scratch(hartindex);
	എങ്കിൽ (!scratch)
		തിരികെ SBI_ENOENT;

	തിരികെ imsic_get_hart_file(scratch);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ imsic_process_hwirqs(ഘടന sbi_irqchip_device *chip)
{
	ഘടന imsic_data *imsic;
	ulong mirq;
	u32 hwirq;
	പൂർണ്ണസംഖ്യ rc, ret = 0;

	imsic = container_of(chip, ഘടന imsic_data, irqchip);
	എങ്കിൽ (!imsic || !imsic->targets_mmode)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (imsic_get_data(current_hartindex()) != imsic)
		തിരികെ 0;

	ആകുമ്പോൾ ((mirq = csr_swap(CSR_MTOPEI, 0))) {
		hwirq = (mirq >> IMSIC_TOPEI_ID_SHIFT) &
			IMSIC_TOPEI_ID_MASK;

		എങ്കിൽ (hwirq > imsic->അക്കം_ids) {
			sbi_printf("%s: invalid hwirq=%lu num_ids=%lu\n",
					__func__,
					(unsigned long)hwirq,
					(unsigned long)imsic->അക്കം_ids);
			തുടരുക;
		}

		എങ്കിൽ (hwirq == IMSIC_IPI_ID) {
			sbi_ipi_process();
			തുടരുക;
		}

		rc = sbi_irqchip_process_hwirq(chip, hwirq);
		എങ്കിൽ (rc && rc != SBI_ENOENT) {
			sbi_printf("%s: hwirq=%lu failed rc=%d\n",
					__func__,
					(unsigned long)hwirq, rc);
			ret = rc;
		}
	}

	തിരികെ ret;
}

സ്ഥിതം ശൂന്യം imsic_ipi_send(u32 hart_index)
{
	unsigned long reloff;
	ഘടന imsic_regs *regs;
	ഘടന imsic_data *data;
	ഘടന sbi_scratch *scratch;
	പൂർണ്ണസംഖ്യ file;

	scratch = sbi_hartindex_to_scratch(hart_index);
	എങ്കിൽ (!scratch)
		തിരികെ;

	data = imsic_get_hart_data_ptr(scratch);
	file = imsic_get_hart_file(scratch);
	എങ്കിൽ (!data || !data->targets_mmode)
		തിരികെ;

	regs = &data->regs[0];
	reloff = file * (1UL << data->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;
	ആകുമ്പോൾ (regs->size && (regs->size <= reloff)) {
		reloff -= regs->size;
		regs++;
	}

	എങ്കിൽ (regs->size && (reloff < regs->size))
		writel_relaxed(IMSIC_IPI_ID,
			(ശൂന്യം *)(regs->addr + reloff + IMSIC_MMIO_PAGE_LE));
}

സ്ഥിതം ഘടന sbi_ipi_device imsic_ipi_device = {
	.name		= "aia-imsic",
	.rating		= 300,
	.ipi_send	= imsic_ipi_send
};

സ്ഥിതം ശൂന്യം imsic_local_eix_update(unsigned long id,
				   bool pend, bool val)
{
	unsigned long isel, ireg = 0;

	isel = id / __riscv_xlen;
	isel *= __riscv_xlen / IMSIC_EIPx_BITS;
	isel += (pend) ? IMSIC_EIP0 : IMSIC_EIE0;
	ireg |= BIT(id);

	എങ്കിൽ (val)
		imsic_csr_set(isel, ireg);
	അല്ലെങ്കിൽ
		imsic_csr_clear(isel, ireg);
}

ശൂന്യം imsic_local_irqchip_init(ശൂന്യം)
{
	ഘടന sbi_trap_info trap = { 0 };

	/*
	 * This function is expected to be called from:
	 * 1) nascent_init() platform callback which is called
	 *    very early on each HART in boot-up path and and
	 *    HSM resume path.
	 * 2) irqchip_init() platform callback which is called
	 *    in boot-up path.
	 */

	/* If Smaia not available then do nothing */
	csr_read_allowed(CSR_MTOPI, &trap);
	എങ്കിൽ (trap.cause)
		തിരികെ;

	/* Setup threshold to allow all enabled interrupts */
	imsic_csr_write(IMSIC_EITHRESHOLD, IMSIC_ENABLE_EITHRESHOLD);

	/* Enable interrupt delivery */
	imsic_csr_write(IMSIC_EIDELIVERY, IMSIC_ENABLE_EIDELIVERY);

	/* Enable IPI */
	imsic_local_eix_update(IMSIC_IPI_ID, false, true);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ imsic_warm_irqchip_init(ഘടന sbi_irqchip_device *dev)
{
	ഘടന imsic_data *imsic;
	ഘടന imsic_data *hart_imsic;
	പൂർണ്ണസംഖ്യ i;

	imsic = container_of(dev, ഘടന imsic_data, irqchip);
	hart_imsic = imsic_get_data(current_hartindex());

	/* Sanity checks */
	എങ്കിൽ (!hart_imsic || hart_imsic != imsic ||
		!hart_imsic->targets_mmode)
		തിരികെ SBI_EINVAL;

	/* enable interrutps based on the irq state */
	വേണ്ടി (i = 1; i < imsic->അക്കം_ids; i++) {
		എങ്കിൽ (sbi_irqchip_is_hwirq_enabled(&imsic->irqchip, i) == true)
			imsic_local_eix_update(i, true, true);
		അല്ലെങ്കിൽ
			imsic_local_eix_update(i, false, false);
	}

	/* Clear IPI pending */
	imsic_local_eix_update(IMSIC_IPI_ID, true, false);

	/* Local IMSIC initialization */
	imsic_local_irqchip_init();

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ imsic_data_check(ഘടന imsic_data *imsic)
{
	u32 i, tmp;
	unsigned long base_addr, addr, mask;

	/* Sanity checks */
	എങ്കിൽ (!imsic ||
	    (imsic->അക്കം_ids < IMSIC_MIN_ID) ||
	    (IMSIC_MAX_ID < imsic->അക്കം_ids))
		തിരികെ SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT;
	എങ്കിൽ (tmp < imsic->guest_index_bits)
		തിരികെ SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_index_bits;
	എങ്കിൽ (tmp < imsic->hart_index_bits)
		തിരികെ SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_index_bits - imsic->hart_index_bits;
	എങ്കിൽ (tmp < imsic->group_index_bits)
		തിരികെ SBI_EINVAL;

	tmp = IMSIC_MMIO_PAGE_SHIFT + imsic->guest_index_bits +
	      imsic->hart_index_bits;
	എങ്കിൽ (imsic->group_index_shift < tmp)
		തിരികെ SBI_EINVAL;
	tmp = imsic->group_index_bits + imsic->group_index_shift - 1;
	എങ്കിൽ (tmp >= BITS_PER_LONG)
		തിരികെ SBI_EINVAL;

	/*
	 * Number of interrupt identities should be 1 less than
	 * multiple of 63
	 */
	എങ്കിൽ ((imsic->അക്കം_ids & IMSIC_MIN_ID) != IMSIC_MIN_ID)
		തിരികെ SBI_EINVAL;

	/* We should have at least one regset */
	എങ്കിൽ (!imsic->regs[0].size)
		തിരികെ SBI_EINVAL;

	/* Match patter of each regset */
	base_addr = imsic->regs[0].addr;
	base_addr &= ~((1UL << (imsic->guest_index_bits +
				 imsic->hart_index_bits +
				 IMSIC_MMIO_PAGE_SHIFT)) - 1);
	base_addr &= ~(((1UL << imsic->group_index_bits) - 1) <<
			imsic->group_index_shift);
	വേണ്ടി (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].size; i++) {
		mask = (1UL << imsic->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;
		mask -= 1UL;
		എങ്കിൽ (imsic->regs[i].size & mask)
			തിരികെ SBI_EINVAL;

		addr = imsic->regs[i].addr;
		addr &= ~((1UL << (imsic->guest_index_bits +
					 imsic->hart_index_bits +
					 IMSIC_MMIO_PAGE_SHIFT)) - 1);
		addr &= ~(((1UL << imsic->group_index_bits) - 1) <<
				imsic->group_index_shift);
		എങ്കിൽ (base_addr != addr)
			തിരികെ SBI_EINVAL;
	}

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ imsic_hwirq_setup(ഘടന sbi_irqchip_device *chip, u32 hwirq, u32 hwirq_flags)
{
	എങ്കിൽ (hwirq_flags != SBI_HWIRQ_FLAGS_NONE)
		തിരികെ SBI_ENOTSUPP;

	sbi_irqchip_set_raw_handler(chip, hwirq,
				    sbi_irqchip_raw_handler_default);

	തിരികെ 0;
}

സ്ഥിതം ശൂന്യം imsic_hwirq_cleanup(ഘടന sbi_irqchip_device *chip, u32 hwirq)
{
	ഘടന imsic_data *imsic;

	എങ്കിൽ (!chip)
		തിരികെ;

	imsic = container_of(chip, ഘടന imsic_data, irqchip);
	എങ്കിൽ (!imsic || !imsic->targets_mmode)
		തിരികെ;

	imsic_local_eix_update(hwirq, false, false);
	imsic_local_eix_update(hwirq, true, false);
}

സ്ഥിതം ശൂന്യം imsic_hwirq_eoi(ഘടന sbi_irqchip_device *chip, u32 hwirq)
{
	/*
	 * IMSIC interrupt claim/ack is already done by reading CSR_MTOPEI
	 * in imsic_process_hwirqs(). No extra EOI operation is required.
	 */
}

സ്ഥിതം പൂർണ്ണസംഖ്യ imsic_compose_msi_സന്ദേശം(ഘടന imsic_data *imsic,
				 u32 hart_index, u32 eiid,
				 ഘടന sbi_irqchip_msi_സന്ദേശം *സന്ദേശം)
{
	ഘടന imsic_regs *regs;
	unsigned long reloff;
	u64 msi_addr;
	പൂർണ്ണസംഖ്യ file;

	എങ്കിൽ (!imsic || !സന്ദേശം)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (!eiid || eiid == IMSIC_IPI_ID)
		തിരികെ 0;

	file = imsic_get_target_file(hart_index);
	എങ്കിൽ (file < 0) {
		sbi_printf("%s: no file for hart=%lu rc=%d\n",
				__func__,
				(unsigned long)hart_index, file);
		തിരികെ file;
	}

	regs   = &imsic->regs[0];
	reloff = file * (1UL << imsic->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;

	ആകുമ്പോൾ (regs->size && regs->size <= reloff) {
		reloff -= regs->size;
		regs++;
	}

	എങ്കിൽ (!regs->size || regs->size <= reloff) {
		sbi_printf("%s: no regset for hart=%lu file=%d reloff=0x%lx\n",
			   __func__,
			   (unsigned long)hart_index, file, reloff);
		തിരികെ SBI_ENODEV;
	}

	msi_addr = (u64)regs->addr + reloff + IMSIC_MMIO_PAGE_LE;
	സന്ദേശം->address_lo = (u32)(msi_addr);
	സന്ദേശം->address_hi = (u32)(msi_addr >> 32);
	സന്ദേശം->data       = eiid;

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ imsic_program_msi(ഘടന sbi_irqchip_device *chip,
		      u32 eiid, u32 hart_index)
{
	ഘടന imsic_data *imsic;
	ഘടന sbi_irqchip_msi_സന്ദേശം സന്ദേശം;
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ (!chip)
		തിരികെ SBI_EINVAL;

	imsic = imsic_get_data(hart_index);
	എങ്കിൽ (!imsic || !imsic->targets_mmode)
		തിരികെ SBI_ENODEV;

	rc = imsic_compose_msi_സന്ദേശം(imsic, hart_index, eiid, &സന്ദേശം);
	എങ്കിൽ (rc) {
		sbi_printf("%s: compose failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)eiid,
				(unsigned long)hart_index, rc);
	}

	rc = sbi_irqchip_write_msi(chip, eiid, &സന്ദേശം);
	എങ്കിൽ (rc) {
		sbi_printf("%s: write_msi failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)eiid,
				(unsigned long)hart_index, rc);
		തിരികെ rc;
	}

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ imsic_hwirq_set_affinity(ഘടന sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_index)
{
	ഘടന imsic_data *imsic;
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ (!chip)
		തിരികെ SBI_EINVAL;

	imsic = container_of(chip, ഘടന imsic_data, irqchip);
	എങ്കിൽ (!imsic || !imsic->targets_mmode)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (!hwirq || hwirq == IMSIC_IPI_ID)
		തിരികെ 0;

	എങ്കിൽ (!sbi_hartmask_test_hartindex(hart_index, &chip->target_harts))
		തിരികെ SBI_EINVAL;

	rc = imsic_program_msi(chip, hwirq, hart_index);
	എങ്കിൽ (rc) {
		sbi_printf("%s: failed hwirq=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)hwirq,
				(unsigned long)hart_index, rc);
		തിരികെ rc;
	}

	തിരികെ 0;
}

സ്ഥിതം ശൂന്യം imsic_hwirq_mask(ഘടന sbi_irqchip_device *chip, u32 hwirq)
{
	ഘടന imsic_data *imsic;

	എങ്കിൽ (!chip)
		തിരികെ;

	imsic = container_of(chip, ഘടന imsic_data, irqchip);
	എങ്കിൽ (!imsic || !imsic->targets_mmode)
		തിരികെ;

	imsic_local_eix_update(hwirq, false, false);
}

സ്ഥിതം ശൂന്യം imsic_hwirq_unmask(ഘടന sbi_irqchip_device *chip, u32 hwirq)
{
	ഘടന imsic_data *imsic;

	എങ്കിൽ (!chip)
		തിരികെ;

	imsic = container_of(chip, ഘടന imsic_data, irqchip);
	എങ്കിൽ (!imsic || !imsic->targets_mmode)
		തിരികെ;

	എങ്കിൽ (!hwirq || hwirq == IMSIC_IPI_ID)
		തിരികെ;

	imsic_local_eix_update(hwirq, false, true);
}

സ്ഥിതം ഘടന sbi_irqchip_device imsic_device = {
	.warm_init		= imsic_warm_irqchip_init,
	.process_hwirqs		= imsic_process_hwirqs,
	.hwirq_setup		= imsic_hwirq_setup,
	.hwirq_cleanup		= imsic_hwirq_cleanup,
	.hwirq_eoi		= imsic_hwirq_eoi,
	.hwirq_set_affinity	= imsic_hwirq_set_affinity,
	.hwirq_mask		= imsic_hwirq_mask,
	.hwirq_unmask		= imsic_hwirq_unmask,
};

പൂർണ്ണസംഖ്യ imsic_cold_irqchip_init(ഘടന imsic_data *imsic)
{
	പൂർണ്ണസംഖ്യ i, rc;

	/* Sanity checks */
	rc = imsic_data_check(imsic);
	എങ്കിൽ (rc)
		തിരികെ rc;

	/* We only initialize M-mode IMSIC */
	എങ്കിൽ (!imsic->targets_mmode)
		തിരികെ SBI_EINVAL;

	/* Allocate scratch space pointer */
	എങ്കിൽ (!imsic_ptr_offset) {
		imsic_ptr_offset = sbi_scratch_alloc_type_offset(ശൂന്യം *);
		എങ്കിൽ (!imsic_ptr_offset)
			തിരികെ SBI_ENOMEM;
	}

	/* Allocate scratch space file */
	എങ്കിൽ (!imsic_file_offset) {
		imsic_file_offset = sbi_scratch_alloc_type_offset(long);
		എങ്കിൽ (!imsic_file_offset)
			തിരികെ SBI_ENOMEM;
	}

	/* Add IMSIC regions to the root domain */
	വേണ്ടി (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].size; i++) {
		rc = sbi_domain_വേര്_add_memrange(imsic->regs[i].addr,
						  imsic->regs[i].size,
						  IMSIC_MMIO_PAGE_SZ,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		എങ്കിൽ (rc)
			തിരികെ rc;
	}

	imsic->irqchip          = imsic_device;
	imsic->irqchip.id       = imsic->unique_id;
	imsic_device.caps = SBI_IRQCHIP_CAPS_MSI;
	imsic->irqchip.അക്കം_hwirq = imsic->അക്കം_ids + 1;

	sbi_hartmask_set_all(&imsic->irqchip.target_harts);

	/* Register irqchip device */
	rc = sbi_irqchip_add_device(&imsic->irqchip);
	എങ്കിൽ (rc) {
		sbi_printf("%s: sbi_irqchip_add_device failed rc=%d\n",
				__func__, rc);
		തിരികെ rc;
	}

	rc = sbi_irqchip_register_reserved(&imsic->irqchip, 0, IMSIC_IPI_ID + 1);
	എങ്കിൽ (rc) {
		sbi_printf("%s: register_reserved failed rc=%d\n",
				__func__, rc);
		തിരികെ rc;
	}

	/* Register IPI device */
	sbi_ipi_add_device(&imsic_ipi_device);

	തിരികെ 0;
}
