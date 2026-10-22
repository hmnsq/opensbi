/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_io.h>
#شامل <sbi/riscv_encoding.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_csr_detect.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_ipi.h>
#شامل <sbi/sbi_irqchip.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi_utils/irqchip/imsic.h>

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
انجامبده { \
	csr_write(CSR_MISELECT, __c); \
	csr_write(CSR_MIREG, __v); \
} وقتی (0)

#define imsic_csr_read(__c)	\
({ \
	unsigned long __v; \
	csr_write(CSR_MISELECT, __c); \
	__v = csr_read(CSR_MIREG); \
	__v; \
})

#define imsic_csr_set(__c, __v)		\
انجامبده { \
	csr_write(CSR_MISELECT, __c); \
	csr_set(CSR_MIREG, __v); \
} وقتی (0)

#define imsic_csr_clear(__c, __v)	\
انجامبده { \
	csr_write(CSR_MISELECT, __c); \
	csr_clear(CSR_MIREG, __v); \
} وقتی (0)

ایستا unsigned long imsic_ptr_offset;

#define imsic_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), تهی *, imsic_ptr_offset)

#define imsic_set_hart_data_ptr(__scratch, __imsic)			\
	sbi_scratch_write_type((__scratch), تهی *, imsic_ptr_offset, (__imsic))

ایستا unsigned long imsic_file_offset;

#define imsic_get_hart_file(__scratch)					\
	sbi_scratch_read_type((__scratch), long, imsic_file_offset)

#define imsic_set_hart_file(__scratch, __file)				\
	sbi_scratch_write_type((__scratch), long, imsic_file_offset, (__file))

صحیح imsic_map_hartid_to_data(u32 hartid, ساختار imsic_data *imsic, صحیح file)
{
	ساختار sbi_scratch *scratch;

	اگر (!imsic || !imsic->targets_mmode)
		بازگشت SBI_EINVAL;

	/*
	 * We don't need to fail if scratch pointer is not available
	 * because we might be dealing with hartid of a HART disabled
	 * in device tree. For HARTs disabled in device tree, the
	 * imsic_get_data() and imsic_get_target_file() will anyway
	 * fail.
	 */
	scratch = sbi_hartid_to_scratch(hartid);
	اگر (!scratch)
		بازگشت 0;

	imsic_set_hart_data_ptr(scratch, imsic);
	imsic_set_hart_file(scratch, file);
	بازگشت 0;
}

ساختار imsic_data *imsic_get_data(u32 hartindex)
{
	ساختار sbi_scratch *scratch;

	اگر (!imsic_ptr_offset)
		بازگشت پوچ;

	scratch = sbi_hartindex_to_scratch(hartindex);
	اگر (!scratch)
		بازگشت پوچ;

	بازگشت imsic_get_hart_data_ptr(scratch);
}

صحیح imsic_get_target_file(u32 hartindex)
{
	ساختار sbi_scratch *scratch;

	اگر (!imsic_file_offset)
		بازگشت SBI_ENOENT;

	scratch = sbi_hartindex_to_scratch(hartindex);
	اگر (!scratch)
		بازگشت SBI_ENOENT;

	بازگشت imsic_get_hart_file(scratch);
}

ایستا صحیح imsic_process_hwirqs(ساختار sbi_irqchip_device *chip)
{
	ساختار imsic_data *imsic;
	ulong mirq;
	u32 hwirq;
	صحیح rc, ret = 0;

	imsic = container_of(chip, ساختار imsic_data, irqchip);
	اگر (!imsic || !imsic->targets_mmode)
		بازگشت SBI_EINVAL;

	اگر (imsic_get_data(current_hartindex()) != imsic)
		بازگشت 0;

	وقتی ((mirq = csr_swap(CSR_MTOPEI, 0))) {
		hwirq = (mirq >> IMSIC_TOPEI_ID_SHIFT) &
			IMSIC_TOPEI_ID_MASK;

		اگر (hwirq > imsic->رقم_ids) {
			sbi_printf("%s: invalid hwirq=%lu num_ids=%lu\n",
					__func__,
					(unsigned long)hwirq,
					(unsigned long)imsic->رقم_ids);
			ادامه;
		}

		اگر (hwirq == IMSIC_IPI_ID) {
			sbi_ipi_process();
			ادامه;
		}

		rc = sbi_irqchip_process_hwirq(chip, hwirq);
		اگر (rc && rc != SBI_ENOENT) {
			sbi_printf("%s: hwirq=%lu failed rc=%d\n",
					__func__,
					(unsigned long)hwirq, rc);
			ret = rc;
		}
	}

	بازگشت ret;
}

ایستا تهی imsic_ipi_send(u32 hart_index)
{
	unsigned long reloff;
	ساختار imsic_regs *regs;
	ساختار imsic_data *data;
	ساختار sbi_scratch *scratch;
	صحیح file;

	scratch = sbi_hartindex_to_scratch(hart_index);
	اگر (!scratch)
		بازگشت;

	data = imsic_get_hart_data_ptr(scratch);
	file = imsic_get_hart_file(scratch);
	اگر (!data || !data->targets_mmode)
		بازگشت;

	regs = &data->regs[0];
	reloff = file * (1UL << data->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;
	وقتی (regs->size && (regs->size <= reloff)) {
		reloff -= regs->size;
		regs++;
	}

	اگر (regs->size && (reloff < regs->size))
		writel_relaxed(IMSIC_IPI_ID,
			(تهی *)(regs->addr + reloff + IMSIC_MMIO_PAGE_LE));
}

ایستا ساختار sbi_ipi_device imsic_ipi_device = {
	.name		= "aia-imsic",
	.rating		= 300,
	.ipi_send	= imsic_ipi_send
};

ایستا تهی imsic_local_eix_update(unsigned long id,
				   bool pend, bool val)
{
	unsigned long isel, ireg = 0;

	isel = id / __riscv_xlen;
	isel *= __riscv_xlen / IMSIC_EIPx_BITS;
	isel += (pend) ? IMSIC_EIP0 : IMSIC_EIE0;
	ireg |= BIT(id);

	اگر (val)
		imsic_csr_set(isel, ireg);
	وگرنه
		imsic_csr_clear(isel, ireg);
}

تهی imsic_local_irqchip_init(تهی)
{
	ساختار sbi_trap_info trap = { 0 };

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
	اگر (trap.cause)
		بازگشت;

	/* Setup threshold to allow all enabled interrupts */
	imsic_csr_write(IMSIC_EITHRESHOLD, IMSIC_ENABLE_EITHRESHOLD);

	/* Enable interrupt delivery */
	imsic_csr_write(IMSIC_EIDELIVERY, IMSIC_ENABLE_EIDELIVERY);

	/* Enable IPI */
	imsic_local_eix_update(IMSIC_IPI_ID, false, true);
}

ایستا صحیح imsic_warm_irqchip_init(ساختار sbi_irqchip_device *dev)
{
	ساختار imsic_data *imsic;
	ساختار imsic_data *hart_imsic;
	صحیح i;

	imsic = container_of(dev, ساختار imsic_data, irqchip);
	hart_imsic = imsic_get_data(current_hartindex());

	/* Sanity checks */
	اگر (!hart_imsic || hart_imsic != imsic ||
		!hart_imsic->targets_mmode)
		بازگشت SBI_EINVAL;

	/* enable interrutps based on the irq state */
	برای (i = 1; i < imsic->رقم_ids; i++) {
		اگر (sbi_irqchip_is_hwirq_enabled(&imsic->irqchip, i) == true)
			imsic_local_eix_update(i, true, true);
		وگرنه
			imsic_local_eix_update(i, false, false);
	}

	/* Clear IPI pending */
	imsic_local_eix_update(IMSIC_IPI_ID, true, false);

	/* Local IMSIC initialization */
	imsic_local_irqchip_init();

	بازگشت 0;
}

صحیح imsic_data_check(ساختار imsic_data *imsic)
{
	u32 i, tmp;
	unsigned long base_addr, addr, mask;

	/* Sanity checks */
	اگر (!imsic ||
	    (imsic->رقم_ids < IMSIC_MIN_ID) ||
	    (IMSIC_MAX_ID < imsic->رقم_ids))
		بازگشت SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT;
	اگر (tmp < imsic->guest_index_bits)
		بازگشت SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_index_bits;
	اگر (tmp < imsic->hart_index_bits)
		بازگشت SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_index_bits - imsic->hart_index_bits;
	اگر (tmp < imsic->group_index_bits)
		بازگشت SBI_EINVAL;

	tmp = IMSIC_MMIO_PAGE_SHIFT + imsic->guest_index_bits +
	      imsic->hart_index_bits;
	اگر (imsic->group_index_shift < tmp)
		بازگشت SBI_EINVAL;
	tmp = imsic->group_index_bits + imsic->group_index_shift - 1;
	اگر (tmp >= BITS_PER_LONG)
		بازگشت SBI_EINVAL;

	/*
	 * Number of interrupt identities should be 1 less than
	 * multiple of 63
	 */
	اگر ((imsic->رقم_ids & IMSIC_MIN_ID) != IMSIC_MIN_ID)
		بازگشت SBI_EINVAL;

	/* We should have at least one regset */
	اگر (!imsic->regs[0].size)
		بازگشت SBI_EINVAL;

	/* Match patter of each regset */
	base_addr = imsic->regs[0].addr;
	base_addr &= ~((1UL << (imsic->guest_index_bits +
				 imsic->hart_index_bits +
				 IMSIC_MMIO_PAGE_SHIFT)) - 1);
	base_addr &= ~(((1UL << imsic->group_index_bits) - 1) <<
			imsic->group_index_shift);
	برای (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].size; i++) {
		mask = (1UL << imsic->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;
		mask -= 1UL;
		اگر (imsic->regs[i].size & mask)
			بازگشت SBI_EINVAL;

		addr = imsic->regs[i].addr;
		addr &= ~((1UL << (imsic->guest_index_bits +
					 imsic->hart_index_bits +
					 IMSIC_MMIO_PAGE_SHIFT)) - 1);
		addr &= ~(((1UL << imsic->group_index_bits) - 1) <<
				imsic->group_index_shift);
		اگر (base_addr != addr)
			بازگشت SBI_EINVAL;
	}

	بازگشت 0;
}

ایستا صحیح imsic_hwirq_setup(ساختار sbi_irqchip_device *chip, u32 hwirq, u32 hwirq_flags)
{
	اگر (hwirq_flags != SBI_HWIRQ_FLAGS_NONE)
		بازگشت SBI_ENOTSUPP;

	sbi_irqchip_set_raw_handler(chip, hwirq,
				    sbi_irqchip_raw_handler_default);

	بازگشت 0;
}

ایستا تهی imsic_hwirq_cleanup(ساختار sbi_irqchip_device *chip, u32 hwirq)
{
	ساختار imsic_data *imsic;

	اگر (!chip)
		بازگشت;

	imsic = container_of(chip, ساختار imsic_data, irqchip);
	اگر (!imsic || !imsic->targets_mmode)
		بازگشت;

	imsic_local_eix_update(hwirq, false, false);
	imsic_local_eix_update(hwirq, true, false);
}

ایستا تهی imsic_hwirq_eoi(ساختار sbi_irqchip_device *chip, u32 hwirq)
{
	/*
	 * IMSIC interrupt claim/ack is already done by reading CSR_MTOPEI
	 * in imsic_process_hwirqs(). No extra EOI operation is required.
	 */
}

ایستا صحیح imsic_compose_msi_پیام(ساختار imsic_data *imsic,
				 u32 hart_index, u32 eiid,
				 ساختار sbi_irqchip_msi_پیام *پیام)
{
	ساختار imsic_regs *regs;
	unsigned long reloff;
	u64 msi_addr;
	صحیح file;

	اگر (!imsic || !پیام)
		بازگشت SBI_EINVAL;

	اگر (!eiid || eiid == IMSIC_IPI_ID)
		بازگشت 0;

	file = imsic_get_target_file(hart_index);
	اگر (file < 0) {
		sbi_printf("%s: no file for hart=%lu rc=%d\n",
				__func__,
				(unsigned long)hart_index, file);
		بازگشت file;
	}

	regs   = &imsic->regs[0];
	reloff = file * (1UL << imsic->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;

	وقتی (regs->size && regs->size <= reloff) {
		reloff -= regs->size;
		regs++;
	}

	اگر (!regs->size || regs->size <= reloff) {
		sbi_printf("%s: no regset for hart=%lu file=%d reloff=0x%lx\n",
			   __func__,
			   (unsigned long)hart_index, file, reloff);
		بازگشت SBI_ENODEV;
	}

	msi_addr = (u64)regs->addr + reloff + IMSIC_MMIO_PAGE_LE;
	پیام->address_lo = (u32)(msi_addr);
	پیام->address_hi = (u32)(msi_addr >> 32);
	پیام->data       = eiid;

	بازگشت 0;
}

ایستا صحیح imsic_program_msi(ساختار sbi_irqchip_device *chip,
		      u32 eiid, u32 hart_index)
{
	ساختار imsic_data *imsic;
	ساختار sbi_irqchip_msi_پیام پیام;
	صحیح rc;

	اگر (!chip)
		بازگشت SBI_EINVAL;

	imsic = imsic_get_data(hart_index);
	اگر (!imsic || !imsic->targets_mmode)
		بازگشت SBI_ENODEV;

	rc = imsic_compose_msi_پیام(imsic, hart_index, eiid, &پیام);
	اگر (rc) {
		sbi_printf("%s: compose failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)eiid,
				(unsigned long)hart_index, rc);
	}

	rc = sbi_irqchip_write_msi(chip, eiid, &پیام);
	اگر (rc) {
		sbi_printf("%s: write_msi failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)eiid,
				(unsigned long)hart_index, rc);
		بازگشت rc;
	}

	بازگشت 0;
}

ایستا صحیح imsic_hwirq_set_affinity(ساختار sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_index)
{
	ساختار imsic_data *imsic;
	صحیح rc;

	اگر (!chip)
		بازگشت SBI_EINVAL;

	imsic = container_of(chip, ساختار imsic_data, irqchip);
	اگر (!imsic || !imsic->targets_mmode)
		بازگشت SBI_EINVAL;

	اگر (!hwirq || hwirq == IMSIC_IPI_ID)
		بازگشت 0;

	اگر (!sbi_hartmask_test_hartindex(hart_index, &chip->target_harts))
		بازگشت SBI_EINVAL;

	rc = imsic_program_msi(chip, hwirq, hart_index);
	اگر (rc) {
		sbi_printf("%s: failed hwirq=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)hwirq,
				(unsigned long)hart_index, rc);
		بازگشت rc;
	}

	بازگشت 0;
}

ایستا تهی imsic_hwirq_mask(ساختار sbi_irqchip_device *chip, u32 hwirq)
{
	ساختار imsic_data *imsic;

	اگر (!chip)
		بازگشت;

	imsic = container_of(chip, ساختار imsic_data, irqchip);
	اگر (!imsic || !imsic->targets_mmode)
		بازگشت;

	imsic_local_eix_update(hwirq, false, false);
}

ایستا تهی imsic_hwirq_unmask(ساختار sbi_irqchip_device *chip, u32 hwirq)
{
	ساختار imsic_data *imsic;

	اگر (!chip)
		بازگشت;

	imsic = container_of(chip, ساختار imsic_data, irqchip);
	اگر (!imsic || !imsic->targets_mmode)
		بازگشت;

	اگر (!hwirq || hwirq == IMSIC_IPI_ID)
		بازگشت;

	imsic_local_eix_update(hwirq, false, true);
}

ایستا ساختار sbi_irqchip_device imsic_device = {
	.warm_init		= imsic_warm_irqchip_init,
	.process_hwirqs		= imsic_process_hwirqs,
	.hwirq_setup		= imsic_hwirq_setup,
	.hwirq_cleanup		= imsic_hwirq_cleanup,
	.hwirq_eoi		= imsic_hwirq_eoi,
	.hwirq_set_affinity	= imsic_hwirq_set_affinity,
	.hwirq_mask		= imsic_hwirq_mask,
	.hwirq_unmask		= imsic_hwirq_unmask,
};

صحیح imsic_cold_irqchip_init(ساختار imsic_data *imsic)
{
	صحیح i, rc;

	/* Sanity checks */
	rc = imsic_data_check(imsic);
	اگر (rc)
		بازگشت rc;

	/* We only initialize M-mode IMSIC */
	اگر (!imsic->targets_mmode)
		بازگشت SBI_EINVAL;

	/* Allocate scratch space pointer */
	اگر (!imsic_ptr_offset) {
		imsic_ptr_offset = sbi_scratch_alloc_type_offset(تهی *);
		اگر (!imsic_ptr_offset)
			بازگشت SBI_ENOMEM;
	}

	/* Allocate scratch space file */
	اگر (!imsic_file_offset) {
		imsic_file_offset = sbi_scratch_alloc_type_offset(long);
		اگر (!imsic_file_offset)
			بازگشت SBI_ENOMEM;
	}

	/* Add IMSIC regions to the root domain */
	برای (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].size; i++) {
		rc = sbi_domain_ریشه_add_memrange(imsic->regs[i].addr,
						  imsic->regs[i].size,
						  IMSIC_MMIO_PAGE_SZ,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		اگر (rc)
			بازگشت rc;
	}

	imsic->irqchip          = imsic_device;
	imsic->irqchip.id       = imsic->unique_id;
	imsic_device.caps = SBI_IRQCHIP_CAPS_MSI;
	imsic->irqchip.رقم_hwirq = imsic->رقم_ids + 1;

	sbi_hartmask_set_all(&imsic->irqchip.target_harts);

	/* Register irqchip device */
	rc = sbi_irqchip_add_device(&imsic->irqchip);
	اگر (rc) {
		sbi_printf("%s: sbi_irqchip_add_device failed rc=%d\n",
				__func__, rc);
		بازگشت rc;
	}

	rc = sbi_irqchip_register_reserved(&imsic->irqchip, 0, IMSIC_IPI_ID + 1);
	اگر (rc) {
		sbi_printf("%s: register_reserved failed rc=%d\n",
				__func__, rc);
		بازگشت rc;
	}

	/* Register IPI device */
	sbi_ipi_add_device(&imsic_ipi_device);

	بازگشت 0;
}
