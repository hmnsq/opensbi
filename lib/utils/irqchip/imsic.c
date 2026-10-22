/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/riscv_encoding.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_csr_detect.h>
#அந்தர்பாவ <sbi/sbi_domain.h>
#அந்தர்பாவ <sbi/sbi_ipi.h>
#அந்தர்பாவ <sbi/sbi_irqchip.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi_utils/irqchip/imsic.h>

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
செய் { \
	csr_write(CSR_MISELECT, __c); \
	csr_write(CSR_MIREG, __v); \
} போது (0)

#define imsic_csr_read(__c)	\
({ \
	unsigned long __v; \
	csr_write(CSR_MISELECT, __c); \
	__v = csr_read(CSR_MIREG); \
	__v; \
})

#define imsic_csr_set(__c, __v)		\
செய் { \
	csr_write(CSR_MISELECT, __c); \
	csr_set(CSR_MIREG, __v); \
} போது (0)

#define imsic_csr_clear(__c, __v)	\
செய் { \
	csr_write(CSR_MISELECT, __c); \
	csr_clear(CSR_MIREG, __v); \
} போது (0)

நிலையான unsigned long imsic_ptr_offset;

#define imsic_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), வெறுமை *, imsic_ptr_offset)

#define imsic_set_hart_data_ptr(__scratch, __imsic)			\
	sbi_scratch_write_type((__scratch), வெறுமை *, imsic_ptr_offset, (__imsic))

நிலையான unsigned long imsic_file_offset;

#define imsic_get_hart_file(__scratch)					\
	sbi_scratch_read_type((__scratch), long, imsic_file_offset)

#define imsic_set_hart_file(__scratch, __file)				\
	sbi_scratch_write_type((__scratch), long, imsic_file_offset, (__file))

முழு imsic_map_hartid_to_data(u32 hartid, கட்டமைப்பு imsic_data *imsic, முழு file)
{
	கட்டமைப்பு sbi_scratch *scratch;

	எனில் (!imsic || !imsic->targets_mmode)
		திரும்பு SBI_EINVAL;

	/*
	 * We don't need to fail if scratch pointer is not available
	 * because we might be dealing with hartid of a HART disabled
	 * in device tree. For HARTs disabled in device tree, the
	 * imsic_get_data() and imsic_get_target_file() will anyway
	 * fail.
	 */
	scratch = sbi_hartid_to_scratch(hartid);
	எனில் (!scratch)
		திரும்பு 0;

	imsic_set_hart_data_ptr(scratch, imsic);
	imsic_set_hart_file(scratch, file);
	திரும்பு 0;
}

கட்டமைப்பு imsic_data *imsic_get_data(u32 hartindex)
{
	கட்டமைப்பு sbi_scratch *scratch;

	எனில் (!imsic_ptr_offset)
		திரும்பு வெற்று;

	scratch = sbi_hartindex_to_scratch(hartindex);
	எனில் (!scratch)
		திரும்பு வெற்று;

	திரும்பு imsic_get_hart_data_ptr(scratch);
}

முழு imsic_get_target_file(u32 hartindex)
{
	கட்டமைப்பு sbi_scratch *scratch;

	எனில் (!imsic_file_offset)
		திரும்பு SBI_ENOENT;

	scratch = sbi_hartindex_to_scratch(hartindex);
	எனில் (!scratch)
		திரும்பு SBI_ENOENT;

	திரும்பு imsic_get_hart_file(scratch);
}

நிலையான முழு imsic_process_hwirqs(கட்டமைப்பு sbi_irqchip_device *chip)
{
	கட்டமைப்பு imsic_data *imsic;
	ulong mirq;
	u32 hwirq;
	முழு rc, ret = 0;

	imsic = container_of(chip, கட்டமைப்பு imsic_data, irqchip);
	எனில் (!imsic || !imsic->targets_mmode)
		திரும்பு SBI_EINVAL;

	எனில் (imsic_get_data(current_hartindex()) != imsic)
		திரும்பு 0;

	போது ((mirq = csr_swap(CSR_MTOPEI, 0))) {
		hwirq = (mirq >> IMSIC_TOPEI_ID_SHIFT) &
			IMSIC_TOPEI_ID_MASK;

		எனில் (hwirq > imsic->இலக்கம்_ids) {
			sbi_printf("%s: invalid hwirq=%lu num_ids=%lu\n",
					__func__,
					(unsigned long)hwirq,
					(unsigned long)imsic->இலக்கம்_ids);
			தொடர்;
		}

		எனில் (hwirq == IMSIC_IPI_ID) {
			sbi_ipi_process();
			தொடர்;
		}

		rc = sbi_irqchip_process_hwirq(chip, hwirq);
		எனில் (rc && rc != SBI_ENOENT) {
			sbi_printf("%s: hwirq=%lu failed rc=%d\n",
					__func__,
					(unsigned long)hwirq, rc);
			ret = rc;
		}
	}

	திரும்பு ret;
}

நிலையான வெறுமை imsic_ipi_send(u32 hart_index)
{
	unsigned long reloff;
	கட்டமைப்பு imsic_regs *regs;
	கட்டமைப்பு imsic_data *data;
	கட்டமைப்பு sbi_scratch *scratch;
	முழு file;

	scratch = sbi_hartindex_to_scratch(hart_index);
	எனில் (!scratch)
		திரும்பு;

	data = imsic_get_hart_data_ptr(scratch);
	file = imsic_get_hart_file(scratch);
	எனில் (!data || !data->targets_mmode)
		திரும்பு;

	regs = &data->regs[0];
	reloff = file * (1UL << data->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;
	போது (regs->size && (regs->size <= reloff)) {
		reloff -= regs->size;
		regs++;
	}

	எனில் (regs->size && (reloff < regs->size))
		writel_relaxed(IMSIC_IPI_ID,
			(வெறுமை *)(regs->addr + reloff + IMSIC_MMIO_PAGE_LE));
}

நிலையான கட்டமைப்பு sbi_ipi_device imsic_ipi_device = {
	.name		= "aia-imsic",
	.rating		= 300,
	.ipi_send	= imsic_ipi_send
};

நிலையான வெறுமை imsic_local_eix_update(unsigned long id,
				   bool pend, bool val)
{
	unsigned long isel, ireg = 0;

	isel = id / __riscv_xlen;
	isel *= __riscv_xlen / IMSIC_EIPx_BITS;
	isel += (pend) ? IMSIC_EIP0 : IMSIC_EIE0;
	ireg |= BIT(id);

	எனில் (val)
		imsic_csr_set(isel, ireg);
	இல்லையெனில்
		imsic_csr_clear(isel, ireg);
}

வெறுமை imsic_local_irqchip_init(வெறுமை)
{
	கட்டமைப்பு sbi_trap_info trap = { 0 };

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
	எனில் (trap.cause)
		திரும்பு;

	/* Setup threshold to allow all enabled interrupts */
	imsic_csr_write(IMSIC_EITHRESHOLD, IMSIC_ENABLE_EITHRESHOLD);

	/* Enable interrupt delivery */
	imsic_csr_write(IMSIC_EIDELIVERY, IMSIC_ENABLE_EIDELIVERY);

	/* Enable IPI */
	imsic_local_eix_update(IMSIC_IPI_ID, false, true);
}

நிலையான முழு imsic_warm_irqchip_init(கட்டமைப்பு sbi_irqchip_device *dev)
{
	கட்டமைப்பு imsic_data *imsic;
	கட்டமைப்பு imsic_data *hart_imsic;
	முழு i;

	imsic = container_of(dev, கட்டமைப்பு imsic_data, irqchip);
	hart_imsic = imsic_get_data(current_hartindex());

	/* Sanity checks */
	எனில் (!hart_imsic || hart_imsic != imsic ||
		!hart_imsic->targets_mmode)
		திரும்பு SBI_EINVAL;

	/* enable interrutps based on the irq state */
	க்கு (i = 1; i < imsic->இலக்கம்_ids; i++) {
		எனில் (sbi_irqchip_is_hwirq_enabled(&imsic->irqchip, i) == true)
			imsic_local_eix_update(i, true, true);
		இல்லையெனில்
			imsic_local_eix_update(i, false, false);
	}

	/* Clear IPI pending */
	imsic_local_eix_update(IMSIC_IPI_ID, true, false);

	/* Local IMSIC initialization */
	imsic_local_irqchip_init();

	திரும்பு 0;
}

முழு imsic_data_check(கட்டமைப்பு imsic_data *imsic)
{
	u32 i, tmp;
	unsigned long base_addr, addr, mask;

	/* Sanity checks */
	எனில் (!imsic ||
	    (imsic->இலக்கம்_ids < IMSIC_MIN_ID) ||
	    (IMSIC_MAX_ID < imsic->இலக்கம்_ids))
		திரும்பு SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT;
	எனில் (tmp < imsic->guest_index_bits)
		திரும்பு SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_index_bits;
	எனில் (tmp < imsic->hart_index_bits)
		திரும்பு SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_index_bits - imsic->hart_index_bits;
	எனில் (tmp < imsic->group_index_bits)
		திரும்பு SBI_EINVAL;

	tmp = IMSIC_MMIO_PAGE_SHIFT + imsic->guest_index_bits +
	      imsic->hart_index_bits;
	எனில் (imsic->group_index_shift < tmp)
		திரும்பு SBI_EINVAL;
	tmp = imsic->group_index_bits + imsic->group_index_shift - 1;
	எனில் (tmp >= BITS_PER_LONG)
		திரும்பு SBI_EINVAL;

	/*
	 * Number of interrupt identities should be 1 less than
	 * multiple of 63
	 */
	எனில் ((imsic->இலக்கம்_ids & IMSIC_MIN_ID) != IMSIC_MIN_ID)
		திரும்பு SBI_EINVAL;

	/* We should have at least one regset */
	எனில் (!imsic->regs[0].size)
		திரும்பு SBI_EINVAL;

	/* Match patter of each regset */
	base_addr = imsic->regs[0].addr;
	base_addr &= ~((1UL << (imsic->guest_index_bits +
				 imsic->hart_index_bits +
				 IMSIC_MMIO_PAGE_SHIFT)) - 1);
	base_addr &= ~(((1UL << imsic->group_index_bits) - 1) <<
			imsic->group_index_shift);
	க்கு (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].size; i++) {
		mask = (1UL << imsic->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;
		mask -= 1UL;
		எனில் (imsic->regs[i].size & mask)
			திரும்பு SBI_EINVAL;

		addr = imsic->regs[i].addr;
		addr &= ~((1UL << (imsic->guest_index_bits +
					 imsic->hart_index_bits +
					 IMSIC_MMIO_PAGE_SHIFT)) - 1);
		addr &= ~(((1UL << imsic->group_index_bits) - 1) <<
				imsic->group_index_shift);
		எனில் (base_addr != addr)
			திரும்பு SBI_EINVAL;
	}

	திரும்பு 0;
}

நிலையான முழு imsic_hwirq_setup(கட்டமைப்பு sbi_irqchip_device *chip, u32 hwirq, u32 hwirq_flags)
{
	எனில் (hwirq_flags != SBI_HWIRQ_FLAGS_NONE)
		திரும்பு SBI_ENOTSUPP;

	sbi_irqchip_set_raw_handler(chip, hwirq,
				    sbi_irqchip_raw_handler_default);

	திரும்பு 0;
}

நிலையான வெறுமை imsic_hwirq_cleanup(கட்டமைப்பு sbi_irqchip_device *chip, u32 hwirq)
{
	கட்டமைப்பு imsic_data *imsic;

	எனில் (!chip)
		திரும்பு;

	imsic = container_of(chip, கட்டமைப்பு imsic_data, irqchip);
	எனில் (!imsic || !imsic->targets_mmode)
		திரும்பு;

	imsic_local_eix_update(hwirq, false, false);
	imsic_local_eix_update(hwirq, true, false);
}

நிலையான வெறுமை imsic_hwirq_eoi(கட்டமைப்பு sbi_irqchip_device *chip, u32 hwirq)
{
	/*
	 * IMSIC interrupt claim/ack is already done by reading CSR_MTOPEI
	 * in imsic_process_hwirqs(). No extra EOI operation is required.
	 */
}

நிலையான முழு imsic_compose_msi_செய்தி(கட்டமைப்பு imsic_data *imsic,
				 u32 hart_index, u32 eiid,
				 கட்டமைப்பு sbi_irqchip_msi_செய்தி *செய்தி)
{
	கட்டமைப்பு imsic_regs *regs;
	unsigned long reloff;
	u64 msi_addr;
	முழு file;

	எனில் (!imsic || !செய்தி)
		திரும்பு SBI_EINVAL;

	எனில் (!eiid || eiid == IMSIC_IPI_ID)
		திரும்பு 0;

	file = imsic_get_target_file(hart_index);
	எனில் (file < 0) {
		sbi_printf("%s: no file for hart=%lu rc=%d\n",
				__func__,
				(unsigned long)hart_index, file);
		திரும்பு file;
	}

	regs   = &imsic->regs[0];
	reloff = file * (1UL << imsic->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;

	போது (regs->size && regs->size <= reloff) {
		reloff -= regs->size;
		regs++;
	}

	எனில் (!regs->size || regs->size <= reloff) {
		sbi_printf("%s: no regset for hart=%lu file=%d reloff=0x%lx\n",
			   __func__,
			   (unsigned long)hart_index, file, reloff);
		திரும்பு SBI_ENODEV;
	}

	msi_addr = (u64)regs->addr + reloff + IMSIC_MMIO_PAGE_LE;
	செய்தி->address_lo = (u32)(msi_addr);
	செய்தி->address_hi = (u32)(msi_addr >> 32);
	செய்தி->data       = eiid;

	திரும்பு 0;
}

நிலையான முழு imsic_program_msi(கட்டமைப்பு sbi_irqchip_device *chip,
		      u32 eiid, u32 hart_index)
{
	கட்டமைப்பு imsic_data *imsic;
	கட்டமைப்பு sbi_irqchip_msi_செய்தி செய்தி;
	முழு rc;

	எனில் (!chip)
		திரும்பு SBI_EINVAL;

	imsic = imsic_get_data(hart_index);
	எனில் (!imsic || !imsic->targets_mmode)
		திரும்பு SBI_ENODEV;

	rc = imsic_compose_msi_செய்தி(imsic, hart_index, eiid, &செய்தி);
	எனில் (rc) {
		sbi_printf("%s: compose failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)eiid,
				(unsigned long)hart_index, rc);
	}

	rc = sbi_irqchip_write_msi(chip, eiid, &செய்தி);
	எனில் (rc) {
		sbi_printf("%s: write_msi failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)eiid,
				(unsigned long)hart_index, rc);
		திரும்பு rc;
	}

	திரும்பு 0;
}

நிலையான முழு imsic_hwirq_set_affinity(கட்டமைப்பு sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_index)
{
	கட்டமைப்பு imsic_data *imsic;
	முழு rc;

	எனில் (!chip)
		திரும்பு SBI_EINVAL;

	imsic = container_of(chip, கட்டமைப்பு imsic_data, irqchip);
	எனில் (!imsic || !imsic->targets_mmode)
		திரும்பு SBI_EINVAL;

	எனில் (!hwirq || hwirq == IMSIC_IPI_ID)
		திரும்பு 0;

	எனில் (!sbi_hartmask_test_hartindex(hart_index, &chip->target_harts))
		திரும்பு SBI_EINVAL;

	rc = imsic_program_msi(chip, hwirq, hart_index);
	எனில் (rc) {
		sbi_printf("%s: failed hwirq=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)hwirq,
				(unsigned long)hart_index, rc);
		திரும்பு rc;
	}

	திரும்பு 0;
}

நிலையான வெறுமை imsic_hwirq_mask(கட்டமைப்பு sbi_irqchip_device *chip, u32 hwirq)
{
	கட்டமைப்பு imsic_data *imsic;

	எனில் (!chip)
		திரும்பு;

	imsic = container_of(chip, கட்டமைப்பு imsic_data, irqchip);
	எனில் (!imsic || !imsic->targets_mmode)
		திரும்பு;

	imsic_local_eix_update(hwirq, false, false);
}

நிலையான வெறுமை imsic_hwirq_unmask(கட்டமைப்பு sbi_irqchip_device *chip, u32 hwirq)
{
	கட்டமைப்பு imsic_data *imsic;

	எனில் (!chip)
		திரும்பு;

	imsic = container_of(chip, கட்டமைப்பு imsic_data, irqchip);
	எனில் (!imsic || !imsic->targets_mmode)
		திரும்பு;

	எனில் (!hwirq || hwirq == IMSIC_IPI_ID)
		திரும்பு;

	imsic_local_eix_update(hwirq, false, true);
}

நிலையான கட்டமைப்பு sbi_irqchip_device imsic_device = {
	.warm_init		= imsic_warm_irqchip_init,
	.process_hwirqs		= imsic_process_hwirqs,
	.hwirq_setup		= imsic_hwirq_setup,
	.hwirq_cleanup		= imsic_hwirq_cleanup,
	.hwirq_eoi		= imsic_hwirq_eoi,
	.hwirq_set_affinity	= imsic_hwirq_set_affinity,
	.hwirq_mask		= imsic_hwirq_mask,
	.hwirq_unmask		= imsic_hwirq_unmask,
};

முழு imsic_cold_irqchip_init(கட்டமைப்பு imsic_data *imsic)
{
	முழு i, rc;

	/* Sanity checks */
	rc = imsic_data_check(imsic);
	எனில் (rc)
		திரும்பு rc;

	/* We only initialize M-mode IMSIC */
	எனில் (!imsic->targets_mmode)
		திரும்பு SBI_EINVAL;

	/* Allocate scratch space pointer */
	எனில் (!imsic_ptr_offset) {
		imsic_ptr_offset = sbi_scratch_alloc_type_offset(வெறுமை *);
		எனில் (!imsic_ptr_offset)
			திரும்பு SBI_ENOMEM;
	}

	/* Allocate scratch space file */
	எனில் (!imsic_file_offset) {
		imsic_file_offset = sbi_scratch_alloc_type_offset(long);
		எனில் (!imsic_file_offset)
			திரும்பு SBI_ENOMEM;
	}

	/* Add IMSIC regions to the root domain */
	க்கு (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].size; i++) {
		rc = sbi_domain_வேர்_add_memrange(imsic->regs[i].addr,
						  imsic->regs[i].size,
						  IMSIC_MMIO_PAGE_SZ,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		எனில் (rc)
			திரும்பு rc;
	}

	imsic->irqchip          = imsic_device;
	imsic->irqchip.id       = imsic->unique_id;
	imsic_device.caps = SBI_IRQCHIP_CAPS_MSI;
	imsic->irqchip.இலக்கம்_hwirq = imsic->இலக்கம்_ids + 1;

	sbi_hartmask_set_all(&imsic->irqchip.target_harts);

	/* Register irqchip device */
	rc = sbi_irqchip_add_device(&imsic->irqchip);
	எனில் (rc) {
		sbi_printf("%s: sbi_irqchip_add_device failed rc=%d\n",
				__func__, rc);
		திரும்பு rc;
	}

	rc = sbi_irqchip_register_reserved(&imsic->irqchip, 0, IMSIC_IPI_ID + 1);
	எனில் (rc) {
		sbi_printf("%s: register_reserved failed rc=%d\n",
				__func__, rc);
		திரும்பு rc;
	}

	/* Register IPI device */
	sbi_ipi_add_device(&imsic_ipi_device);

	திரும்பு 0;
}
