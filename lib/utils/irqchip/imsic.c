/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_io.h>
#ضمن <sbi/riscv_encoding.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_csr_detect.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_ipi.h>
#ضمن <sbi/sbi_irqchip.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi_utils/irqchip/imsic.h>

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
افعل { \
	csr_write(CSR_MISELECT, __c); \
	csr_write(CSR_MIREG, __v); \
} طالما (0)

#define imsic_csr_read(__c)	\
({ \
	unsigned long __v; \
	csr_write(CSR_MISELECT, __c); \
	__v = csr_read(CSR_MIREG); \
	__v; \
})

#define imsic_csr_set(__c, __v)		\
افعل { \
	csr_write(CSR_MISELECT, __c); \
	csr_set(CSR_MIREG, __v); \
} طالما (0)

#define imsic_csr_clear(__c, __v)	\
افعل { \
	csr_write(CSR_MISELECT, __c); \
	csr_clear(CSR_MIREG, __v); \
} طالما (0)

ساكن unsigned long imsic_ptr_offset;

#define imsic_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), فراغ *, imsic_ptr_offset)

#define imsic_set_hart_data_ptr(__scratch, __imsic)			\
	sbi_scratch_write_type((__scratch), فراغ *, imsic_ptr_offset, (__imsic))

ساكن unsigned long imsic_file_offset;

#define imsic_get_hart_file(__scratch)					\
	sbi_scratch_read_type((__scratch), long, imsic_file_offset)

#define imsic_set_hart_file(__scratch, __file)				\
	sbi_scratch_write_type((__scratch), long, imsic_file_offset, (__file))

صحيح imsic_map_hartid_to_data(u32 hartid, بنية imsic_data *imsic, صحيح file)
{
	بنية sbi_scratch *scratch;

	إذا (!imsic || !imsic->targets_mmode)
		أرجع SBI_EINVAL;

	/*
	 * We don't need to fail if scratch pointer is not available
	 * because we might be dealing with hartid of a HART disabled
	 * in device tree. For HARTs disabled in device tree, the
	 * imsic_get_data() and imsic_get_target_file() will anyway
	 * fail.
	 */
	scratch = sbi_hartid_to_scratch(hartid);
	إذا (!scratch)
		أرجع 0;

	imsic_set_hart_data_ptr(scratch, imsic);
	imsic_set_hart_file(scratch, file);
	أرجع 0;
}

بنية imsic_data *imsic_get_data(u32 hartindex)
{
	بنية sbi_scratch *scratch;

	إذا (!imsic_ptr_offset)
		أرجع عدم;

	scratch = sbi_hartindex_to_scratch(hartindex);
	إذا (!scratch)
		أرجع عدم;

	أرجع imsic_get_hart_data_ptr(scratch);
}

صحيح imsic_get_target_file(u32 hartindex)
{
	بنية sbi_scratch *scratch;

	إذا (!imsic_file_offset)
		أرجع SBI_ENOENT;

	scratch = sbi_hartindex_to_scratch(hartindex);
	إذا (!scratch)
		أرجع SBI_ENOENT;

	أرجع imsic_get_hart_file(scratch);
}

ساكن صحيح imsic_process_hwirqs(بنية sbi_irqchip_device *chip)
{
	بنية imsic_data *imsic;
	ulong mirq;
	u32 hwirq;
	صحيح rc, ret = 0;

	imsic = container_of(chip, بنية imsic_data, irqchip);
	إذا (!imsic || !imsic->targets_mmode)
		أرجع SBI_EINVAL;

	إذا (imsic_get_data(current_hartindex()) != imsic)
		أرجع 0;

	طالما ((mirq = csr_swap(CSR_MTOPEI, 0))) {
		hwirq = (mirq >> IMSIC_TOPEI_ID_SHIFT) &
			IMSIC_TOPEI_ID_MASK;

		إذا (hwirq > imsic->ژمارە_ids) {
			sbi_printf("%s: invalid hwirq=%lu num_ids=%lu\n",
					__func__,
					(unsigned long)hwirq,
					(unsigned long)imsic->ژمارە_ids);
			استمر;
		}

		إذا (hwirq == IMSIC_IPI_ID) {
			sbi_ipi_process();
			استمر;
		}

		rc = sbi_irqchip_process_hwirq(chip, hwirq);
		إذا (rc && rc != SBI_ENOENT) {
			sbi_printf("%s: hwirq=%lu failed rc=%d\n",
					__func__,
					(unsigned long)hwirq, rc);
			ret = rc;
		}
	}

	أرجع ret;
}

ساكن فراغ imsic_ipi_send(u32 hart_index)
{
	unsigned long reloff;
	بنية imsic_regs *regs;
	بنية imsic_data *data;
	بنية sbi_scratch *scratch;
	صحيح file;

	scratch = sbi_hartindex_to_scratch(hart_index);
	إذا (!scratch)
		أرجع;

	data = imsic_get_hart_data_ptr(scratch);
	file = imsic_get_hart_file(scratch);
	إذا (!data || !data->targets_mmode)
		أرجع;

	regs = &data->regs[0];
	reloff = file * (1UL << data->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;
	طالما (regs->size && (regs->size <= reloff)) {
		reloff -= regs->size;
		regs++;
	}

	إذا (regs->size && (reloff < regs->size))
		writel_relaxed(IMSIC_IPI_ID,
			(فراغ *)(regs->addr + reloff + IMSIC_MMIO_PAGE_LE));
}

ساكن بنية sbi_ipi_device imsic_ipi_device = {
	.name		= "aia-imsic",
	.rating		= 300,
	.ipi_send	= imsic_ipi_send
};

ساكن فراغ imsic_local_eix_update(unsigned long id,
				   bool pend, bool val)
{
	unsigned long isel, ireg = 0;

	isel = id / __riscv_xlen;
	isel *= __riscv_xlen / IMSIC_EIPx_BITS;
	isel += (pend) ? IMSIC_EIP0 : IMSIC_EIE0;
	ireg |= BIT(id);

	إذا (val)
		imsic_csr_set(isel, ireg);
	وإلا
		imsic_csr_clear(isel, ireg);
}

فراغ imsic_local_irqchip_init(فراغ)
{
	بنية sbi_trap_info trap = { 0 };

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
	إذا (trap.cause)
		أرجع;

	/* Setup threshold to allow all enabled interrupts */
	imsic_csr_write(IMSIC_EITHRESHOLD, IMSIC_ENABLE_EITHRESHOLD);

	/* Enable interrupt delivery */
	imsic_csr_write(IMSIC_EIDELIVERY, IMSIC_ENABLE_EIDELIVERY);

	/* Enable IPI */
	imsic_local_eix_update(IMSIC_IPI_ID, false, true);
}

ساكن صحيح imsic_warm_irqchip_init(بنية sbi_irqchip_device *dev)
{
	بنية imsic_data *imsic;
	بنية imsic_data *hart_imsic;
	صحيح i;

	imsic = container_of(dev, بنية imsic_data, irqchip);
	hart_imsic = imsic_get_data(current_hartindex());

	/* Sanity checks */
	إذا (!hart_imsic || hart_imsic != imsic ||
		!hart_imsic->targets_mmode)
		أرجع SBI_EINVAL;

	/* enable interrutps based on the irq state */
	لكل (i = 1; i < imsic->ژمارە_ids; i++) {
		إذا (sbi_irqchip_is_hwirq_enabled(&imsic->irqchip, i) == true)
			imsic_local_eix_update(i, true, true);
		وإلا
			imsic_local_eix_update(i, false, false);
	}

	/* Clear IPI pending */
	imsic_local_eix_update(IMSIC_IPI_ID, true, false);

	/* Local IMSIC initialization */
	imsic_local_irqchip_init();

	أرجع 0;
}

صحيح imsic_data_check(بنية imsic_data *imsic)
{
	u32 i, tmp;
	unsigned long base_addr, addr, mask;

	/* Sanity checks */
	إذا (!imsic ||
	    (imsic->ژمارە_ids < IMSIC_MIN_ID) ||
	    (IMSIC_MAX_ID < imsic->ژمارە_ids))
		أرجع SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT;
	إذا (tmp < imsic->guest_index_bits)
		أرجع SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_index_bits;
	إذا (tmp < imsic->hart_index_bits)
		أرجع SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_index_bits - imsic->hart_index_bits;
	إذا (tmp < imsic->group_index_bits)
		أرجع SBI_EINVAL;

	tmp = IMSIC_MMIO_PAGE_SHIFT + imsic->guest_index_bits +
	      imsic->hart_index_bits;
	إذا (imsic->group_index_shift < tmp)
		أرجع SBI_EINVAL;
	tmp = imsic->group_index_bits + imsic->group_index_shift - 1;
	إذا (tmp >= BITS_PER_LONG)
		أرجع SBI_EINVAL;

	/*
	 * Number of interrupt identities should be 1 less than
	 * multiple of 63
	 */
	إذا ((imsic->ژمارە_ids & IMSIC_MIN_ID) != IMSIC_MIN_ID)
		أرجع SBI_EINVAL;

	/* We should have at least one regset */
	إذا (!imsic->regs[0].size)
		أرجع SBI_EINVAL;

	/* Match patter of each regset */
	base_addr = imsic->regs[0].addr;
	base_addr &= ~((1UL << (imsic->guest_index_bits +
				 imsic->hart_index_bits +
				 IMSIC_MMIO_PAGE_SHIFT)) - 1);
	base_addr &= ~(((1UL << imsic->group_index_bits) - 1) <<
			imsic->group_index_shift);
	لكل (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].size; i++) {
		mask = (1UL << imsic->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;
		mask -= 1UL;
		إذا (imsic->regs[i].size & mask)
			أرجع SBI_EINVAL;

		addr = imsic->regs[i].addr;
		addr &= ~((1UL << (imsic->guest_index_bits +
					 imsic->hart_index_bits +
					 IMSIC_MMIO_PAGE_SHIFT)) - 1);
		addr &= ~(((1UL << imsic->group_index_bits) - 1) <<
				imsic->group_index_shift);
		إذا (base_addr != addr)
			أرجع SBI_EINVAL;
	}

	أرجع 0;
}

ساكن صحيح imsic_hwirq_setup(بنية sbi_irqchip_device *chip, u32 hwirq, u32 hwirq_flags)
{
	إذا (hwirq_flags != SBI_HWIRQ_FLAGS_NONE)
		أرجع SBI_ENOTSUPP;

	sbi_irqchip_set_raw_handler(chip, hwirq,
				    sbi_irqchip_raw_handler_default);

	أرجع 0;
}

ساكن فراغ imsic_hwirq_cleanup(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية imsic_data *imsic;

	إذا (!chip)
		أرجع;

	imsic = container_of(chip, بنية imsic_data, irqchip);
	إذا (!imsic || !imsic->targets_mmode)
		أرجع;

	imsic_local_eix_update(hwirq, false, false);
	imsic_local_eix_update(hwirq, true, false);
}

ساكن فراغ imsic_hwirq_eoi(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	/*
	 * IMSIC interrupt claim/ack is already done by reading CSR_MTOPEI
	 * in imsic_process_hwirqs(). No extra EOI operation is required.
	 */
}

ساكن صحيح imsic_compose_msi_پەیام(بنية imsic_data *imsic,
				 u32 hart_index, u32 eiid,
				 بنية sbi_irqchip_msi_پەیام *پەیام)
{
	بنية imsic_regs *regs;
	unsigned long reloff;
	u64 msi_addr;
	صحيح file;

	إذا (!imsic || !پەیام)
		أرجع SBI_EINVAL;

	إذا (!eiid || eiid == IMSIC_IPI_ID)
		أرجع 0;

	file = imsic_get_target_file(hart_index);
	إذا (file < 0) {
		sbi_printf("%s: no file for hart=%lu rc=%d\n",
				__func__,
				(unsigned long)hart_index, file);
		أرجع file;
	}

	regs   = &imsic->regs[0];
	reloff = file * (1UL << imsic->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;

	طالما (regs->size && regs->size <= reloff) {
		reloff -= regs->size;
		regs++;
	}

	إذا (!regs->size || regs->size <= reloff) {
		sbi_printf("%s: no regset for hart=%lu file=%d reloff=0x%lx\n",
			   __func__,
			   (unsigned long)hart_index, file, reloff);
		أرجع SBI_ENODEV;
	}

	msi_addr = (u64)regs->addr + reloff + IMSIC_MMIO_PAGE_LE;
	پەیام->address_lo = (u32)(msi_addr);
	پەیام->address_hi = (u32)(msi_addr >> 32);
	پەیام->data       = eiid;

	أرجع 0;
}

ساكن صحيح imsic_program_msi(بنية sbi_irqchip_device *chip,
		      u32 eiid, u32 hart_index)
{
	بنية imsic_data *imsic;
	بنية sbi_irqchip_msi_پەیام پەیام;
	صحيح rc;

	إذا (!chip)
		أرجع SBI_EINVAL;

	imsic = imsic_get_data(hart_index);
	إذا (!imsic || !imsic->targets_mmode)
		أرجع SBI_ENODEV;

	rc = imsic_compose_msi_پەیام(imsic, hart_index, eiid, &پەیام);
	إذا (rc) {
		sbi_printf("%s: compose failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)eiid,
				(unsigned long)hart_index, rc);
	}

	rc = sbi_irqchip_write_msi(chip, eiid, &پەیام);
	إذا (rc) {
		sbi_printf("%s: write_msi failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)eiid,
				(unsigned long)hart_index, rc);
		أرجع rc;
	}

	أرجع 0;
}

ساكن صحيح imsic_hwirq_set_affinity(بنية sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_index)
{
	بنية imsic_data *imsic;
	صحيح rc;

	إذا (!chip)
		أرجع SBI_EINVAL;

	imsic = container_of(chip, بنية imsic_data, irqchip);
	إذا (!imsic || !imsic->targets_mmode)
		أرجع SBI_EINVAL;

	إذا (!hwirq || hwirq == IMSIC_IPI_ID)
		أرجع 0;

	إذا (!sbi_hartmask_test_hartindex(hart_index, &chip->target_harts))
		أرجع SBI_EINVAL;

	rc = imsic_program_msi(chip, hwirq, hart_index);
	إذا (rc) {
		sbi_printf("%s: failed hwirq=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)hwirq,
				(unsigned long)hart_index, rc);
		أرجع rc;
	}

	أرجع 0;
}

ساكن فراغ imsic_hwirq_mask(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية imsic_data *imsic;

	إذا (!chip)
		أرجع;

	imsic = container_of(chip, بنية imsic_data, irqchip);
	إذا (!imsic || !imsic->targets_mmode)
		أرجع;

	imsic_local_eix_update(hwirq, false, false);
}

ساكن فراغ imsic_hwirq_unmask(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية imsic_data *imsic;

	إذا (!chip)
		أرجع;

	imsic = container_of(chip, بنية imsic_data, irqchip);
	إذا (!imsic || !imsic->targets_mmode)
		أرجع;

	إذا (!hwirq || hwirq == IMSIC_IPI_ID)
		أرجع;

	imsic_local_eix_update(hwirq, false, true);
}

ساكن بنية sbi_irqchip_device imsic_device = {
	.warm_init		= imsic_warm_irqchip_init,
	.process_hwirqs		= imsic_process_hwirqs,
	.hwirq_setup		= imsic_hwirq_setup,
	.hwirq_cleanup		= imsic_hwirq_cleanup,
	.hwirq_eoi		= imsic_hwirq_eoi,
	.hwirq_set_affinity	= imsic_hwirq_set_affinity,
	.hwirq_mask		= imsic_hwirq_mask,
	.hwirq_unmask		= imsic_hwirq_unmask,
};

صحيح imsic_cold_irqchip_init(بنية imsic_data *imsic)
{
	صحيح i, rc;

	/* Sanity checks */
	rc = imsic_data_check(imsic);
	إذا (rc)
		أرجع rc;

	/* We only initialize M-mode IMSIC */
	إذا (!imsic->targets_mmode)
		أرجع SBI_EINVAL;

	/* Allocate scratch space pointer */
	إذا (!imsic_ptr_offset) {
		imsic_ptr_offset = sbi_scratch_alloc_type_offset(فراغ *);
		إذا (!imsic_ptr_offset)
			أرجع SBI_ENOMEM;
	}

	/* Allocate scratch space file */
	إذا (!imsic_file_offset) {
		imsic_file_offset = sbi_scratch_alloc_type_offset(long);
		إذا (!imsic_file_offset)
			أرجع SBI_ENOMEM;
	}

	/* Add IMSIC regions to the root domain */
	لكل (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].size; i++) {
		rc = sbi_domain_ڕەگ_add_memrange(imsic->regs[i].addr,
						  imsic->regs[i].size,
						  IMSIC_MMIO_PAGE_SZ,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		إذا (rc)
			أرجع rc;
	}

	imsic->irqchip          = imsic_device;
	imsic->irqchip.id       = imsic->unique_id;
	imsic_device.caps = SBI_IRQCHIP_CAPS_MSI;
	imsic->irqchip.ژمارە_hwirq = imsic->ژمارە_ids + 1;

	sbi_hartmask_set_all(&imsic->irqchip.target_harts);

	/* Register irqchip device */
	rc = sbi_irqchip_add_device(&imsic->irqchip);
	إذا (rc) {
		sbi_printf("%s: sbi_irqchip_add_device failed rc=%d\n",
				__func__, rc);
		أرجع rc;
	}

	rc = sbi_irqchip_register_reserved(&imsic->irqchip, 0, IMSIC_IPI_ID + 1);
	إذا (rc) {
		sbi_printf("%s: register_reserved failed rc=%d\n",
				__func__, rc);
		أرجع rc;
	}

	/* Register IPI device */
	sbi_ipi_add_device(&imsic_ipi_device);

	أرجع 0;
}
