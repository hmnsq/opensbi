/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <sbi/riscv_asm.h>
#include <sbi/riscv_io.h>
#include <sbi/riscv_encoding.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_csr_detect.h>
#include <sbi/sbi_domain.h>
#include <sbi/sbi_ipi.h>
#include <sbi/sbi_irqchip.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_scratch.h>
#include <sbi_utils/irqchip/imsic.h>

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
do { \
	csr_write(CSR_MISELECT, __c); \
	csr_write(CSR_MIREG, __v); \
} ໃນຂະນະທີ່ (0)

#define imsic_csr_read(__c)	\
({ \
	unsigned long __v; \
	csr_write(CSR_MISELECT, __c); \
	__v = csr_read(CSR_MIREG); \
	__v; \
})

#define imsic_csr_set(__c, __v)		\
do { \
	csr_write(CSR_MISELECT, __c); \
	csr_set(CSR_MIREG, __v); \
} ໃນຂະນະທີ່ (0)

#define imsic_csr_clear(__c, __v)	\
do { \
	csr_write(CSR_MISELECT, __c); \
	csr_clear(CSR_MIREG, __v); \
} ໃນຂະນະທີ່ (0)

static unsigned long imsic_ptr_offset;

#define imsic_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), void *, imsic_ptr_offset)

#define imsic_set_hart_data_ptr(__scratch, __imsic)			\
	sbi_scratch_write_type((__scratch), void *, imsic_ptr_offset, (__imsic))

static unsigned long imsic_file_offset;

#define imsic_get_hart_file(__scratch)					\
	sbi_scratch_read_type((__scratch), long, imsic_file_offset)

#define imsic_set_hart_file(__scratch, __file)				\
	sbi_scratch_write_type((__scratch), long, imsic_file_offset, (__file))

ຈຳນວນເຕັມ imsic_map_hartid_to_data(u32 hartid, sa_nracanaa imsic_data *imsic, ຈຳນວນເຕັມ file)
{
	sa_nracanaa sbi_scratch *scratch;

	ຖ້າ (!imsic || !imsic->targets_mmode)
		ສົ່ງຄືນ SBI_EINVAL;

	/*
	 * We don't need to fail if scratch pointer is not available
	 * because we might be dealing with hartid of a HART disabled
	 * in device tree. For HARTs disabled in device tree, the
	 * imsic_get_data() and imsic_get_target_file() will anyway
	 * fail.
	 */
	scratch = sbi_hartid_to_scratch(hartid);
	ຖ້າ (!scratch)
		ສົ່ງຄືນ 0;

	imsic_set_hart_data_ptr(scratch, imsic);
	imsic_set_hart_file(scratch, file);
	ສົ່ງຄືນ 0;
}

sa_nracanaa imsic_data *imsic_get_data(u32 hartindex)
{
	sa_nracanaa sbi_scratch *scratch;

	ຖ້າ (!imsic_ptr_offset)
		ສົ່ງຄືນ NULL;

	scratch = sbi_hartindex_to_scratch(hartindex);
	ຖ້າ (!scratch)
		ສົ່ງຄືນ NULL;

	ສົ່ງຄືນ imsic_get_hart_data_ptr(scratch);
}

ຈຳນວນເຕັມ imsic_get_target_file(u32 hartindex)
{
	sa_nracanaa sbi_scratch *scratch;

	ຖ້າ (!imsic_file_offset)
		ສົ່ງຄືນ SBI_ENOENT;

	scratch = sbi_hartindex_to_scratch(hartindex);
	ຖ້າ (!scratch)
		ສົ່ງຄືນ SBI_ENOENT;

	ສົ່ງຄືນ imsic_get_hart_file(scratch);
}

static ຈຳນວນເຕັມ imsic_process_hwirqs(sa_nracanaa sbi_irqchip_device *chip)
{
	sa_nracanaa imsic_data *imsic;
	ulong mirq;
	u32 hwirq;
	ຈຳນວນເຕັມ rc, ret = 0;

	imsic = container_of(chip, sa_nracanaa imsic_data, irqchip);
	ຖ້າ (!imsic || !imsic->targets_mmode)
		ສົ່ງຄືນ SBI_EINVAL;

	ຖ້າ (imsic_get_data(current_hartindex()) != imsic)
		ສົ່ງຄືນ 0;

	ໃນຂະນະທີ່ ((mirq = csr_swap(CSR_MTOPEI, 0))) {
		hwirq = (mirq >> IMSIC_TOPEI_ID_SHIFT) &
			IMSIC_TOPEI_ID_MASK;

		ຖ້າ (hwirq > imsic->num_ids) {
			sbi_printf("%s: invalid hwirq=%lu num_ids=%lu\n",
					__func__,
					(unsigned long)hwirq,
					(unsigned long)imsic->num_ids);
			continue;
		}

		ຖ້າ (hwirq == IMSIC_IPI_ID) {
			sbi_ipi_process();
			continue;
		}

		rc = sbi_irqchip_process_hwirq(chip, hwirq);
		ຖ້າ (rc && rc != SBI_ENOENT) {
			sbi_printf("%s: hwirq=%lu failed rc=%d\n",
					__func__,
					(unsigned long)hwirq, rc);
			ret = rc;
		}
	}

	ສົ່ງຄືນ ret;
}

static void imsic_ipi_send(u32 hart_index)
{
	unsigned long reloff;
	sa_nracanaa imsic_regs *regs;
	sa_nracanaa imsic_data *data;
	sa_nracanaa sbi_scratch *scratch;
	ຈຳນວນເຕັມ file;

	scratch = sbi_hartindex_to_scratch(hart_index);
	ຖ້າ (!scratch)
		ສົ່ງຄືນ;

	data = imsic_get_hart_data_ptr(scratch);
	file = imsic_get_hart_file(scratch);
	ຖ້າ (!data || !data->targets_mmode)
		ສົ່ງຄືນ;

	regs = &data->regs[0];
	reloff = file * (1UL << data->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;
	ໃນຂະນະທີ່ (regs->size && (regs->size <= reloff)) {
		reloff -= regs->size;
		regs++;
	}

	ຖ້າ (regs->size && (reloff < regs->size))
		writel_relaxed(IMSIC_IPI_ID,
			(void *)(regs->addr + reloff + IMSIC_MMIO_PAGE_LE));
}

static sa_nracanaa sbi_ipi_device imsic_ipi_device = {
	.name		= "aia-imsic",
	.rating		= 300,
	.ipi_send	= imsic_ipi_send
};

static void imsic_local_eix_update(unsigned long id,
				   bool pend, bool val)
{
	unsigned long isel, ireg = 0;

	isel = id / __riscv_xlen;
	isel *= __riscv_xlen / IMSIC_EIPx_BITS;
	isel += (pend) ? IMSIC_EIP0 : IMSIC_EIE0;
	ireg |= BIT(id);

	ຖ້າ (val)
		imsic_csr_set(isel, ireg);
	ບໍ່ດັ່ງນັ້ນ
		imsic_csr_clear(isel, ireg);
}

void imsic_local_irqchip_init(void)
{
	sa_nracanaa sbi_trap_info trap = { 0 };

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
	ຖ້າ (trap.cause)
		ສົ່ງຄືນ;

	/* Setup threshold to allow all enabled interrupts */
	imsic_csr_write(IMSIC_EITHRESHOLD, IMSIC_ENABLE_EITHRESHOLD);

	/* Enable interrupt delivery */
	imsic_csr_write(IMSIC_EIDELIVERY, IMSIC_ENABLE_EIDELIVERY);

	/* Enable IPI */
	imsic_local_eix_update(IMSIC_IPI_ID, false, true);
}

static ຈຳນວນເຕັມ imsic_warm_irqchip_init(sa_nracanaa sbi_irqchip_device *dev)
{
	sa_nracanaa imsic_data *imsic;
	sa_nracanaa imsic_data *hart_imsic;
	ຈຳນວນເຕັມ i;

	imsic = container_of(dev, sa_nracanaa imsic_data, irqchip);
	hart_imsic = imsic_get_data(current_hartindex());

	/* Sanity checks */
	ຖ້າ (!hart_imsic || hart_imsic != imsic ||
		!hart_imsic->targets_mmode)
		ສົ່ງຄືນ SBI_EINVAL;

	/* enable interrutps based on the irq state */
	ສຳລັບ (i = 1; i < imsic->num_ids; i++) {
		ຖ້າ (sbi_irqchip_is_hwirq_enabled(&imsic->irqchip, i) == true)
			imsic_local_eix_update(i, true, true);
		ບໍ່ດັ່ງນັ້ນ
			imsic_local_eix_update(i, false, false);
	}

	/* Clear IPI pending */
	imsic_local_eix_update(IMSIC_IPI_ID, true, false);

	/* Local IMSIC initialization */
	imsic_local_irqchip_init();

	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ imsic_data_check(sa_nracanaa imsic_data *imsic)
{
	u32 i, tmp;
	unsigned long base_addr, addr, mask;

	/* Sanity checks */
	ຖ້າ (!imsic ||
	    (imsic->num_ids < IMSIC_MIN_ID) ||
	    (IMSIC_MAX_ID < imsic->num_ids))
		ສົ່ງຄືນ SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT;
	ຖ້າ (tmp < imsic->guest_index_bits)
		ສົ່ງຄືນ SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_index_bits;
	ຖ້າ (tmp < imsic->hart_index_bits)
		ສົ່ງຄືນ SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_index_bits - imsic->hart_index_bits;
	ຖ້າ (tmp < imsic->group_index_bits)
		ສົ່ງຄືນ SBI_EINVAL;

	tmp = IMSIC_MMIO_PAGE_SHIFT + imsic->guest_index_bits +
	      imsic->hart_index_bits;
	ຖ້າ (imsic->group_index_shift < tmp)
		ສົ່ງຄືນ SBI_EINVAL;
	tmp = imsic->group_index_bits + imsic->group_index_shift - 1;
	ຖ້າ (tmp >= BITS_PER_LONG)
		ສົ່ງຄືນ SBI_EINVAL;

	/*
	 * Number of interrupt identities should be 1 less than
	 * multiple of 63
	 */
	ຖ້າ ((imsic->num_ids & IMSIC_MIN_ID) != IMSIC_MIN_ID)
		ສົ່ງຄືນ SBI_EINVAL;

	/* We should have at least one regset */
	ຖ້າ (!imsic->regs[0].size)
		ສົ່ງຄືນ SBI_EINVAL;

	/* Match patter of each regset */
	base_addr = imsic->regs[0].addr;
	base_addr &= ~((1UL << (imsic->guest_index_bits +
				 imsic->hart_index_bits +
				 IMSIC_MMIO_PAGE_SHIFT)) - 1);
	base_addr &= ~(((1UL << imsic->group_index_bits) - 1) <<
			imsic->group_index_shift);
	ສຳລັບ (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].size; i++) {
		mask = (1UL << imsic->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;
		mask -= 1UL;
		ຖ້າ (imsic->regs[i].size & mask)
			ສົ່ງຄືນ SBI_EINVAL;

		addr = imsic->regs[i].addr;
		addr &= ~((1UL << (imsic->guest_index_bits +
					 imsic->hart_index_bits +
					 IMSIC_MMIO_PAGE_SHIFT)) - 1);
		addr &= ~(((1UL << imsic->group_index_bits) - 1) <<
				imsic->group_index_shift);
		ຖ້າ (base_addr != addr)
			ສົ່ງຄືນ SBI_EINVAL;
	}

	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ imsic_hwirq_setup(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq, u32 hwirq_flags)
{
	ຖ້າ (hwirq_flags != SBI_HWIRQ_FLAGS_NONE)
		ສົ່ງຄືນ SBI_ENOTSUPP;

	sbi_irqchip_set_raw_handler(chip, hwirq,
				    sbi_irqchip_raw_handler_default);

	ສົ່ງຄືນ 0;
}

static void imsic_hwirq_cleanup(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa imsic_data *imsic;

	ຖ້າ (!chip)
		ສົ່ງຄືນ;

	imsic = container_of(chip, sa_nracanaa imsic_data, irqchip);
	ຖ້າ (!imsic || !imsic->targets_mmode)
		ສົ່ງຄືນ;

	imsic_local_eix_update(hwirq, false, false);
	imsic_local_eix_update(hwirq, true, false);
}

static void imsic_hwirq_eoi(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	/*
	 * IMSIC interrupt claim/ack is already done by reading CSR_MTOPEI
	 * in imsic_process_hwirqs(). No extra EOI operation is required.
	 */
}

static ຈຳນວນເຕັມ imsic_compose_msi_msg(sa_nracanaa imsic_data *imsic,
				 u32 hart_index, u32 eiid,
				 sa_nracanaa sbi_irqchip_msi_msg *msg)
{
	sa_nracanaa imsic_regs *regs;
	unsigned long reloff;
	u64 msi_addr;
	ຈຳນວນເຕັມ file;

	ຖ້າ (!imsic || !msg)
		ສົ່ງຄືນ SBI_EINVAL;

	ຖ້າ (!eiid || eiid == IMSIC_IPI_ID)
		ສົ່ງຄືນ 0;

	file = imsic_get_target_file(hart_index);
	ຖ້າ (file < 0) {
		sbi_printf("%s: no file for hart=%lu rc=%d\n",
				__func__,
				(unsigned long)hart_index, file);
		ສົ່ງຄືນ file;
	}

	regs   = &imsic->regs[0];
	reloff = file * (1UL << imsic->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;

	ໃນຂະນະທີ່ (regs->size && regs->size <= reloff) {
		reloff -= regs->size;
		regs++;
	}

	ຖ້າ (!regs->size || regs->size <= reloff) {
		sbi_printf("%s: no regset for hart=%lu file=%d reloff=0x%lx\n",
			   __func__,
			   (unsigned long)hart_index, file, reloff);
		ສົ່ງຄືນ SBI_ENODEV;
	}

	msi_addr = (u64)regs->addr + reloff + IMSIC_MMIO_PAGE_LE;
	msg->address_lo = (u32)(msi_addr);
	msg->address_hi = (u32)(msi_addr >> 32);
	msg->data       = eiid;

	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ imsic_program_msi(sa_nracanaa sbi_irqchip_device *chip,
		      u32 eiid, u32 hart_index)
{
	sa_nracanaa imsic_data *imsic;
	sa_nracanaa sbi_irqchip_msi_msg msg;
	ຈຳນວນເຕັມ rc;

	ຖ້າ (!chip)
		ສົ່ງຄືນ SBI_EINVAL;

	imsic = imsic_get_data(hart_index);
	ຖ້າ (!imsic || !imsic->targets_mmode)
		ສົ່ງຄືນ SBI_ENODEV;

	rc = imsic_compose_msi_msg(imsic, hart_index, eiid, &msg);
	ຖ້າ (rc) {
		sbi_printf("%s: compose failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)eiid,
				(unsigned long)hart_index, rc);
	}

	rc = sbi_irqchip_write_msi(chip, eiid, &msg);
	ຖ້າ (rc) {
		sbi_printf("%s: write_msi failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)eiid,
				(unsigned long)hart_index, rc);
		ສົ່ງຄືນ rc;
	}

	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ imsic_hwirq_set_affinity(sa_nracanaa sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_index)
{
	sa_nracanaa imsic_data *imsic;
	ຈຳນວນເຕັມ rc;

	ຖ້າ (!chip)
		ສົ່ງຄືນ SBI_EINVAL;

	imsic = container_of(chip, sa_nracanaa imsic_data, irqchip);
	ຖ້າ (!imsic || !imsic->targets_mmode)
		ສົ່ງຄືນ SBI_EINVAL;

	ຖ້າ (!hwirq || hwirq == IMSIC_IPI_ID)
		ສົ່ງຄືນ 0;

	ຖ້າ (!sbi_hartmask_test_hartindex(hart_index, &chip->target_harts))
		ສົ່ງຄືນ SBI_EINVAL;

	rc = imsic_program_msi(chip, hwirq, hart_index);
	ຖ້າ (rc) {
		sbi_printf("%s: failed hwirq=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)hwirq,
				(unsigned long)hart_index, rc);
		ສົ່ງຄືນ rc;
	}

	ສົ່ງຄືນ 0;
}

static void imsic_hwirq_mask(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa imsic_data *imsic;

	ຖ້າ (!chip)
		ສົ່ງຄືນ;

	imsic = container_of(chip, sa_nracanaa imsic_data, irqchip);
	ຖ້າ (!imsic || !imsic->targets_mmode)
		ສົ່ງຄືນ;

	imsic_local_eix_update(hwirq, false, false);
}

static void imsic_hwirq_unmask(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa imsic_data *imsic;

	ຖ້າ (!chip)
		ສົ່ງຄືນ;

	imsic = container_of(chip, sa_nracanaa imsic_data, irqchip);
	ຖ້າ (!imsic || !imsic->targets_mmode)
		ສົ່ງຄືນ;

	ຖ້າ (!hwirq || hwirq == IMSIC_IPI_ID)
		ສົ່ງຄືນ;

	imsic_local_eix_update(hwirq, false, true);
}

static sa_nracanaa sbi_irqchip_device imsic_device = {
	.warm_init		= imsic_warm_irqchip_init,
	.process_hwirqs		= imsic_process_hwirqs,
	.hwirq_setup		= imsic_hwirq_setup,
	.hwirq_cleanup		= imsic_hwirq_cleanup,
	.hwirq_eoi		= imsic_hwirq_eoi,
	.hwirq_set_affinity	= imsic_hwirq_set_affinity,
	.hwirq_mask		= imsic_hwirq_mask,
	.hwirq_unmask		= imsic_hwirq_unmask,
};

ຈຳນວນເຕັມ imsic_cold_irqchip_init(sa_nracanaa imsic_data *imsic)
{
	ຈຳນວນເຕັມ i, rc;

	/* Sanity checks */
	rc = imsic_data_check(imsic);
	ຖ້າ (rc)
		ສົ່ງຄືນ rc;

	/* We only initialize M-mode IMSIC */
	ຖ້າ (!imsic->targets_mmode)
		ສົ່ງຄືນ SBI_EINVAL;

	/* Allocate scratch space pointer */
	ຖ້າ (!imsic_ptr_offset) {
		imsic_ptr_offset = sbi_scratch_alloc_type_offset(void *);
		ຖ້າ (!imsic_ptr_offset)
			ສົ່ງຄືນ SBI_ENOMEM;
	}

	/* Allocate scratch space file */
	ຖ້າ (!imsic_file_offset) {
		imsic_file_offset = sbi_scratch_alloc_type_offset(long);
		ຖ້າ (!imsic_file_offset)
			ສົ່ງຄືນ SBI_ENOMEM;
	}

	/* Add IMSIC regions to the root domain */
	ສຳລັບ (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].size; i++) {
		rc = sbi_domain_root_add_memrange(imsic->regs[i].addr,
						  imsic->regs[i].size,
						  IMSIC_MMIO_PAGE_SZ,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		ຖ້າ (rc)
			ສົ່ງຄືນ rc;
	}

	imsic->irqchip          = imsic_device;
	imsic->irqchip.id       = imsic->unique_id;
	imsic_device.caps = SBI_IRQCHIP_CAPS_MSI;
	imsic->irqchip.num_hwirq = imsic->num_ids + 1;

	sbi_hartmask_set_all(&imsic->irqchip.target_harts);

	/* Register irqchip device */
	rc = sbi_irqchip_add_device(&imsic->irqchip);
	ຖ້າ (rc) {
		sbi_printf("%s: sbi_irqchip_add_device failed rc=%d\n",
				__func__, rc);
		ສົ່ງຄືນ rc;
	}

	rc = sbi_irqchip_register_reserved(&imsic->irqchip, 0, IMSIC_IPI_ID + 1);
	ຖ້າ (rc) {
		sbi_printf("%s: register_reserved failed rc=%d\n",
				__func__, rc);
		ສົ່ງຄືນ rc;
	}

	/* Register IPI device */
	sbi_ipi_add_device(&imsic_ipi_device);

	ສົ່ງຄືນ 0;
}
