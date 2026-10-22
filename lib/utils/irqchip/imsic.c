/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/riscv_encoding.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_csr_detect.h>
#અન્તર્ભાવ <sbi/sbi_domain.h>
#અન્તર્ભાવ <sbi/sbi_ipi.h>
#અન્તર્ભાવ <sbi/sbi_irqchip.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi_utils/irqchip/imsic.h>

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
કરો { \
	csr_write(CSR_MISELECT, __c); \
	csr_write(CSR_MIREG, __v); \
} જ્યારે (0)

#define imsic_csr_read(__c)	\
({ \
	unsigned long __v; \
	csr_write(CSR_MISELECT, __c); \
	__v = csr_read(CSR_MIREG); \
	__v; \
})

#define imsic_csr_set(__c, __v)		\
કરો { \
	csr_write(CSR_MISELECT, __c); \
	csr_set(CSR_MIREG, __v); \
} જ્યારે (0)

#define imsic_csr_clear(__c, __v)	\
કરો { \
	csr_write(CSR_MISELECT, __c); \
	csr_clear(CSR_MIREG, __v); \
} જ્યારે (0)

સ્થિર unsigned long imsic_ptr_offset;

#define imsic_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), શૂન્ય *, imsic_ptr_offset)

#define imsic_set_hart_data_ptr(__scratch, __imsic)			\
	sbi_scratch_write_type((__scratch), શૂન્ય *, imsic_ptr_offset, (__imsic))

સ્થિર unsigned long imsic_file_offset;

#define imsic_get_hart_file(__scratch)					\
	sbi_scratch_read_type((__scratch), long, imsic_file_offset)

#define imsic_set_hart_file(__scratch, __file)				\
	sbi_scratch_write_type((__scratch), long, imsic_file_offset, (__file))

પૂર્ણાંક imsic_map_hartid_to_data(u32 hartid, સંરચના imsic_data *imsic, પૂર્ણાંક file)
{
	સંરચના sbi_scratch *scratch;

	જો (!imsic || !imsic->targets_mmode)
		પાછા SBI_EINVAL;

	/*
	 * We don't need to fail if scratch pointer is not available
	 * because we might be dealing with hartid of a HART disabled
	 * in device tree. For HARTs disabled in device tree, the
	 * imsic_get_data() and imsic_get_target_file() will anyway
	 * fail.
	 */
	scratch = sbi_hartid_to_scratch(hartid);
	જો (!scratch)
		પાછા 0;

	imsic_set_hart_data_ptr(scratch, imsic);
	imsic_set_hart_file(scratch, file);
	પાછા 0;
}

સંરચના imsic_data *imsic_get_data(u32 hartindex)
{
	સંરચના sbi_scratch *scratch;

	જો (!imsic_ptr_offset)
		પાછા ખાલી;

	scratch = sbi_hartindex_to_scratch(hartindex);
	જો (!scratch)
		પાછા ખાલી;

	પાછા imsic_get_hart_data_ptr(scratch);
}

પૂર્ણાંક imsic_get_target_file(u32 hartindex)
{
	સંરચના sbi_scratch *scratch;

	જો (!imsic_file_offset)
		પાછા SBI_ENOENT;

	scratch = sbi_hartindex_to_scratch(hartindex);
	જો (!scratch)
		પાછા SBI_ENOENT;

	પાછા imsic_get_hart_file(scratch);
}

સ્થિર પૂર્ણાંક imsic_process_hwirqs(સંરચના sbi_irqchip_device *chip)
{
	સંરચના imsic_data *imsic;
	ulong mirq;
	u32 hwirq;
	પૂર્ણાંક rc, ret = 0;

	imsic = container_of(chip, સંરચના imsic_data, irqchip);
	જો (!imsic || !imsic->targets_mmode)
		પાછા SBI_EINVAL;

	જો (imsic_get_data(current_hartindex()) != imsic)
		પાછા 0;

	જ્યારે ((mirq = csr_swap(CSR_MTOPEI, 0))) {
		hwirq = (mirq >> IMSIC_TOPEI_ID_SHIFT) &
			IMSIC_TOPEI_ID_MASK;

		જો (hwirq > imsic->અંક_ids) {
			sbi_printf("%s: invalid hwirq=%lu num_ids=%lu\n",
					__func__,
					(unsigned long)hwirq,
					(unsigned long)imsic->અંક_ids);
			ચાલુ;
		}

		જો (hwirq == IMSIC_IPI_ID) {
			sbi_ipi_process();
			ચાલુ;
		}

		rc = sbi_irqchip_process_hwirq(chip, hwirq);
		જો (rc && rc != SBI_ENOENT) {
			sbi_printf("%s: hwirq=%lu failed rc=%d\n",
					__func__,
					(unsigned long)hwirq, rc);
			ret = rc;
		}
	}

	પાછા ret;
}

સ્થિર શૂન્ય imsic_ipi_send(u32 hart_index)
{
	unsigned long reloff;
	સંરચના imsic_regs *regs;
	સંરચના imsic_data *data;
	સંરચના sbi_scratch *scratch;
	પૂર્ણાંક file;

	scratch = sbi_hartindex_to_scratch(hart_index);
	જો (!scratch)
		પાછા;

	data = imsic_get_hart_data_ptr(scratch);
	file = imsic_get_hart_file(scratch);
	જો (!data || !data->targets_mmode)
		પાછા;

	regs = &data->regs[0];
	reloff = file * (1UL << data->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;
	જ્યારે (regs->size && (regs->size <= reloff)) {
		reloff -= regs->size;
		regs++;
	}

	જો (regs->size && (reloff < regs->size))
		writel_relaxed(IMSIC_IPI_ID,
			(શૂન્ય *)(regs->addr + reloff + IMSIC_MMIO_PAGE_LE));
}

સ્થિર સંરચના sbi_ipi_device imsic_ipi_device = {
	.name		= "aia-imsic",
	.rating		= 300,
	.ipi_send	= imsic_ipi_send
};

સ્થિર શૂન્ય imsic_local_eix_update(unsigned long id,
				   bool pend, bool val)
{
	unsigned long isel, ireg = 0;

	isel = id / __riscv_xlen;
	isel *= __riscv_xlen / IMSIC_EIPx_BITS;
	isel += (pend) ? IMSIC_EIP0 : IMSIC_EIE0;
	ireg |= BIT(id);

	જો (val)
		imsic_csr_set(isel, ireg);
	નહીંતર
		imsic_csr_clear(isel, ireg);
}

શૂન્ય imsic_local_irqchip_init(શૂન્ય)
{
	સંરચના sbi_trap_info trap = { 0 };

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
	જો (trap.cause)
		પાછા;

	/* Setup threshold to allow all enabled interrupts */
	imsic_csr_write(IMSIC_EITHRESHOLD, IMSIC_ENABLE_EITHRESHOLD);

	/* Enable interrupt delivery */
	imsic_csr_write(IMSIC_EIDELIVERY, IMSIC_ENABLE_EIDELIVERY);

	/* Enable IPI */
	imsic_local_eix_update(IMSIC_IPI_ID, false, true);
}

સ્થિર પૂર્ણાંક imsic_warm_irqchip_init(સંરચના sbi_irqchip_device *dev)
{
	સંરચના imsic_data *imsic;
	સંરચના imsic_data *hart_imsic;
	પૂર્ણાંક i;

	imsic = container_of(dev, સંરચના imsic_data, irqchip);
	hart_imsic = imsic_get_data(current_hartindex());

	/* Sanity checks */
	જો (!hart_imsic || hart_imsic != imsic ||
		!hart_imsic->targets_mmode)
		પાછા SBI_EINVAL;

	/* enable interrutps based on the irq state */
	માટે (i = 1; i < imsic->અંક_ids; i++) {
		જો (sbi_irqchip_is_hwirq_enabled(&imsic->irqchip, i) == true)
			imsic_local_eix_update(i, true, true);
		નહીંતર
			imsic_local_eix_update(i, false, false);
	}

	/* Clear IPI pending */
	imsic_local_eix_update(IMSIC_IPI_ID, true, false);

	/* Local IMSIC initialization */
	imsic_local_irqchip_init();

	પાછા 0;
}

પૂર્ણાંક imsic_data_check(સંરચના imsic_data *imsic)
{
	u32 i, tmp;
	unsigned long base_addr, addr, mask;

	/* Sanity checks */
	જો (!imsic ||
	    (imsic->અંક_ids < IMSIC_MIN_ID) ||
	    (IMSIC_MAX_ID < imsic->અંક_ids))
		પાછા SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT;
	જો (tmp < imsic->guest_index_bits)
		પાછા SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_index_bits;
	જો (tmp < imsic->hart_index_bits)
		પાછા SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_index_bits - imsic->hart_index_bits;
	જો (tmp < imsic->group_index_bits)
		પાછા SBI_EINVAL;

	tmp = IMSIC_MMIO_PAGE_SHIFT + imsic->guest_index_bits +
	      imsic->hart_index_bits;
	જો (imsic->group_index_shift < tmp)
		પાછા SBI_EINVAL;
	tmp = imsic->group_index_bits + imsic->group_index_shift - 1;
	જો (tmp >= BITS_PER_LONG)
		પાછા SBI_EINVAL;

	/*
	 * Number of interrupt identities should be 1 less than
	 * multiple of 63
	 */
	જો ((imsic->અંક_ids & IMSIC_MIN_ID) != IMSIC_MIN_ID)
		પાછા SBI_EINVAL;

	/* We should have at least one regset */
	જો (!imsic->regs[0].size)
		પાછા SBI_EINVAL;

	/* Match patter of each regset */
	base_addr = imsic->regs[0].addr;
	base_addr &= ~((1UL << (imsic->guest_index_bits +
				 imsic->hart_index_bits +
				 IMSIC_MMIO_PAGE_SHIFT)) - 1);
	base_addr &= ~(((1UL << imsic->group_index_bits) - 1) <<
			imsic->group_index_shift);
	માટે (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].size; i++) {
		mask = (1UL << imsic->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;
		mask -= 1UL;
		જો (imsic->regs[i].size & mask)
			પાછા SBI_EINVAL;

		addr = imsic->regs[i].addr;
		addr &= ~((1UL << (imsic->guest_index_bits +
					 imsic->hart_index_bits +
					 IMSIC_MMIO_PAGE_SHIFT)) - 1);
		addr &= ~(((1UL << imsic->group_index_bits) - 1) <<
				imsic->group_index_shift);
		જો (base_addr != addr)
			પાછા SBI_EINVAL;
	}

	પાછા 0;
}

સ્થિર પૂર્ણાંક imsic_hwirq_setup(સંરચના sbi_irqchip_device *chip, u32 hwirq, u32 hwirq_flags)
{
	જો (hwirq_flags != SBI_HWIRQ_FLAGS_NONE)
		પાછા SBI_ENOTSUPP;

	sbi_irqchip_set_raw_handler(chip, hwirq,
				    sbi_irqchip_raw_handler_default);

	પાછા 0;
}

સ્થિર શૂન્ય imsic_hwirq_cleanup(સંરચના sbi_irqchip_device *chip, u32 hwirq)
{
	સંરચના imsic_data *imsic;

	જો (!chip)
		પાછા;

	imsic = container_of(chip, સંરચના imsic_data, irqchip);
	જો (!imsic || !imsic->targets_mmode)
		પાછા;

	imsic_local_eix_update(hwirq, false, false);
	imsic_local_eix_update(hwirq, true, false);
}

સ્થિર શૂન્ય imsic_hwirq_eoi(સંરચના sbi_irqchip_device *chip, u32 hwirq)
{
	/*
	 * IMSIC interrupt claim/ack is already done by reading CSR_MTOPEI
	 * in imsic_process_hwirqs(). No extra EOI operation is required.
	 */
}

સ્થિર પૂર્ણાંક imsic_compose_msi_સંદેશ(સંરચના imsic_data *imsic,
				 u32 hart_index, u32 eiid,
				 સંરચના sbi_irqchip_msi_સંદેશ *સંદેશ)
{
	સંરચના imsic_regs *regs;
	unsigned long reloff;
	u64 msi_addr;
	પૂર્ણાંક file;

	જો (!imsic || !સંદેશ)
		પાછા SBI_EINVAL;

	જો (!eiid || eiid == IMSIC_IPI_ID)
		પાછા 0;

	file = imsic_get_target_file(hart_index);
	જો (file < 0) {
		sbi_printf("%s: no file for hart=%lu rc=%d\n",
				__func__,
				(unsigned long)hart_index, file);
		પાછા file;
	}

	regs   = &imsic->regs[0];
	reloff = file * (1UL << imsic->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;

	જ્યારે (regs->size && regs->size <= reloff) {
		reloff -= regs->size;
		regs++;
	}

	જો (!regs->size || regs->size <= reloff) {
		sbi_printf("%s: no regset for hart=%lu file=%d reloff=0x%lx\n",
			   __func__,
			   (unsigned long)hart_index, file, reloff);
		પાછા SBI_ENODEV;
	}

	msi_addr = (u64)regs->addr + reloff + IMSIC_MMIO_PAGE_LE;
	સંદેશ->address_lo = (u32)(msi_addr);
	સંદેશ->address_hi = (u32)(msi_addr >> 32);
	સંદેશ->data       = eiid;

	પાછા 0;
}

સ્થિર પૂર્ણાંક imsic_program_msi(સંરચના sbi_irqchip_device *chip,
		      u32 eiid, u32 hart_index)
{
	સંરચના imsic_data *imsic;
	સંરચના sbi_irqchip_msi_સંદેશ સંદેશ;
	પૂર્ણાંક rc;

	જો (!chip)
		પાછા SBI_EINVAL;

	imsic = imsic_get_data(hart_index);
	જો (!imsic || !imsic->targets_mmode)
		પાછા SBI_ENODEV;

	rc = imsic_compose_msi_સંદેશ(imsic, hart_index, eiid, &સંદેશ);
	જો (rc) {
		sbi_printf("%s: compose failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)eiid,
				(unsigned long)hart_index, rc);
	}

	rc = sbi_irqchip_write_msi(chip, eiid, &સંદેશ);
	જો (rc) {
		sbi_printf("%s: write_msi failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)eiid,
				(unsigned long)hart_index, rc);
		પાછા rc;
	}

	પાછા 0;
}

સ્થિર પૂર્ણાંક imsic_hwirq_set_affinity(સંરચના sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_index)
{
	સંરચના imsic_data *imsic;
	પૂર્ણાંક rc;

	જો (!chip)
		પાછા SBI_EINVAL;

	imsic = container_of(chip, સંરચના imsic_data, irqchip);
	જો (!imsic || !imsic->targets_mmode)
		પાછા SBI_EINVAL;

	જો (!hwirq || hwirq == IMSIC_IPI_ID)
		પાછા 0;

	જો (!sbi_hartmask_test_hartindex(hart_index, &chip->target_harts))
		પાછા SBI_EINVAL;

	rc = imsic_program_msi(chip, hwirq, hart_index);
	જો (rc) {
		sbi_printf("%s: failed hwirq=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)hwirq,
				(unsigned long)hart_index, rc);
		પાછા rc;
	}

	પાછા 0;
}

સ્થિર શૂન્ય imsic_hwirq_mask(સંરચના sbi_irqchip_device *chip, u32 hwirq)
{
	સંરચના imsic_data *imsic;

	જો (!chip)
		પાછા;

	imsic = container_of(chip, સંરચના imsic_data, irqchip);
	જો (!imsic || !imsic->targets_mmode)
		પાછા;

	imsic_local_eix_update(hwirq, false, false);
}

સ્થિર શૂન્ય imsic_hwirq_unmask(સંરચના sbi_irqchip_device *chip, u32 hwirq)
{
	સંરચના imsic_data *imsic;

	જો (!chip)
		પાછા;

	imsic = container_of(chip, સંરચના imsic_data, irqchip);
	જો (!imsic || !imsic->targets_mmode)
		પાછા;

	જો (!hwirq || hwirq == IMSIC_IPI_ID)
		પાછા;

	imsic_local_eix_update(hwirq, false, true);
}

સ્થિર સંરચના sbi_irqchip_device imsic_device = {
	.warm_init		= imsic_warm_irqchip_init,
	.process_hwirqs		= imsic_process_hwirqs,
	.hwirq_setup		= imsic_hwirq_setup,
	.hwirq_cleanup		= imsic_hwirq_cleanup,
	.hwirq_eoi		= imsic_hwirq_eoi,
	.hwirq_set_affinity	= imsic_hwirq_set_affinity,
	.hwirq_mask		= imsic_hwirq_mask,
	.hwirq_unmask		= imsic_hwirq_unmask,
};

પૂર્ણાંક imsic_cold_irqchip_init(સંરચના imsic_data *imsic)
{
	પૂર્ણાંક i, rc;

	/* Sanity checks */
	rc = imsic_data_check(imsic);
	જો (rc)
		પાછા rc;

	/* We only initialize M-mode IMSIC */
	જો (!imsic->targets_mmode)
		પાછા SBI_EINVAL;

	/* Allocate scratch space pointer */
	જો (!imsic_ptr_offset) {
		imsic_ptr_offset = sbi_scratch_alloc_type_offset(શૂન્ય *);
		જો (!imsic_ptr_offset)
			પાછા SBI_ENOMEM;
	}

	/* Allocate scratch space file */
	જો (!imsic_file_offset) {
		imsic_file_offset = sbi_scratch_alloc_type_offset(long);
		જો (!imsic_file_offset)
			પાછા SBI_ENOMEM;
	}

	/* Add IMSIC regions to the root domain */
	માટે (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].size; i++) {
		rc = sbi_domain_મૂળ_add_memrange(imsic->regs[i].addr,
						  imsic->regs[i].size,
						  IMSIC_MMIO_PAGE_SZ,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		જો (rc)
			પાછા rc;
	}

	imsic->irqchip          = imsic_device;
	imsic->irqchip.id       = imsic->unique_id;
	imsic_device.caps = SBI_IRQCHIP_CAPS_MSI;
	imsic->irqchip.અંક_hwirq = imsic->અંક_ids + 1;

	sbi_hartmask_set_all(&imsic->irqchip.target_harts);

	/* Register irqchip device */
	rc = sbi_irqchip_add_device(&imsic->irqchip);
	જો (rc) {
		sbi_printf("%s: sbi_irqchip_add_device failed rc=%d\n",
				__func__, rc);
		પાછા rc;
	}

	rc = sbi_irqchip_register_reserved(&imsic->irqchip, 0, IMSIC_IPI_ID + 1);
	જો (rc) {
		sbi_printf("%s: register_reserved failed rc=%d\n",
				__func__, rc);
		પાછા rc;
	}

	/* Register IPI device */
	sbi_ipi_add_device(&imsic_ipi_device);

	પાછા 0;
}
