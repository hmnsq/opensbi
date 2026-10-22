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
} 동안 (0)

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
} 동안 (0)

#define imsic_csr_clear(__c, __v)	\
do { \
	csr_write(CSR_MISELECT, __c); \
	csr_clear(CSR_MIREG, __v); \
} 동안 (0)

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

정수 imsic_map_hartid_to_data(u32 hartid, struct imsic_data *imsic, 정수 file)
{
	struct sbi_scratch *scratch;

	만약 (!imsic || !imsic->targets_mmode)
		반환 SBI_EINVAL;

	/*
	 * We don't need to fail if scratch pointer is not available
	 * because we might be dealing with hartid of a HART disabled
	 * in device tree. For HARTs disabled in device tree, the
	 * imsic_get_data() and imsic_get_target_file() will anyway
	 * fail.
	 */
	scratch = sbi_hartid_to_scratch(hartid);
	만약 (!scratch)
		반환 0;

	imsic_set_hart_data_ptr(scratch, imsic);
	imsic_set_hart_file(scratch, file);
	반환 0;
}

struct imsic_data *imsic_get_data(u32 hartindex)
{
	struct sbi_scratch *scratch;

	만약 (!imsic_ptr_offset)
		반환 NULL;

	scratch = sbi_hartindex_to_scratch(hartindex);
	만약 (!scratch)
		반환 NULL;

	반환 imsic_get_hart_data_ptr(scratch);
}

정수 imsic_get_target_file(u32 hartindex)
{
	struct sbi_scratch *scratch;

	만약 (!imsic_file_offset)
		반환 SBI_ENOENT;

	scratch = sbi_hartindex_to_scratch(hartindex);
	만약 (!scratch)
		반환 SBI_ENOENT;

	반환 imsic_get_hart_file(scratch);
}

static 정수 imsic_process_hwirqs(struct sbi_irqchip_device *chip)
{
	struct imsic_data *imsic;
	ulong mirq;
	u32 hwirq;
	정수 rc, ret = 0;

	imsic = container_of(chip, struct imsic_data, irqchip);
	만약 (!imsic || !imsic->targets_mmode)
		반환 SBI_EINVAL;

	만약 (imsic_get_data(current_hartindex()) != imsic)
		반환 0;

	동안 ((mirq = csr_swap(CSR_MTOPEI, 0))) {
		hwirq = (mirq >> IMSIC_TOPEI_ID_SHIFT) &
			IMSIC_TOPEI_ID_MASK;

		만약 (hwirq > imsic->num_ids) {
			sbi_printf("%s: invalid hwirq=%lu num_ids=%lu\n",
					__func__,
					(unsigned long)hwirq,
					(unsigned long)imsic->num_ids);
			continue;
		}

		만약 (hwirq == IMSIC_IPI_ID) {
			sbi_ipi_process();
			continue;
		}

		rc = sbi_irqchip_process_hwirq(chip, hwirq);
		만약 (rc && rc != SBI_ENOENT) {
			sbi_printf("%s: hwirq=%lu failed rc=%d\n",
					__func__,
					(unsigned long)hwirq, rc);
			ret = rc;
		}
	}

	반환 ret;
}

static void imsic_ipi_send(u32 hart_index)
{
	unsigned long reloff;
	struct imsic_regs *regs;
	struct imsic_data *data;
	struct sbi_scratch *scratch;
	정수 file;

	scratch = sbi_hartindex_to_scratch(hart_index);
	만약 (!scratch)
		반환;

	data = imsic_get_hart_data_ptr(scratch);
	file = imsic_get_hart_file(scratch);
	만약 (!data || !data->targets_mmode)
		반환;

	regs = &data->regs[0];
	reloff = file * (1UL << data->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;
	동안 (regs->size && (regs->size <= reloff)) {
		reloff -= regs->size;
		regs++;
	}

	만약 (regs->size && (reloff < regs->size))
		writel_relaxed(IMSIC_IPI_ID,
			(void *)(regs->addr + reloff + IMSIC_MMIO_PAGE_LE));
}

static struct sbi_ipi_device imsic_ipi_device = {
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

	만약 (val)
		imsic_csr_set(isel, ireg);
	아니면
		imsic_csr_clear(isel, ireg);
}

void imsic_local_irqchip_init(void)
{
	struct sbi_trap_info trap = { 0 };

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
	만약 (trap.cause)
		반환;

	/* Setup threshold to allow all enabled interrupts */
	imsic_csr_write(IMSIC_EITHRESHOLD, IMSIC_ENABLE_EITHRESHOLD);

	/* Enable interrupt delivery */
	imsic_csr_write(IMSIC_EIDELIVERY, IMSIC_ENABLE_EIDELIVERY);

	/* Enable IPI */
	imsic_local_eix_update(IMSIC_IPI_ID, false, true);
}

static 정수 imsic_warm_irqchip_init(struct sbi_irqchip_device *dev)
{
	struct imsic_data *imsic;
	struct imsic_data *hart_imsic;
	정수 i;

	imsic = container_of(dev, struct imsic_data, irqchip);
	hart_imsic = imsic_get_data(current_hartindex());

	/* Sanity checks */
	만약 (!hart_imsic || hart_imsic != imsic ||
		!hart_imsic->targets_mmode)
		반환 SBI_EINVAL;

	/* enable interrutps based on the irq state */
	위해 (i = 1; i < imsic->num_ids; i++) {
		만약 (sbi_irqchip_is_hwirq_enabled(&imsic->irqchip, i) == true)
			imsic_local_eix_update(i, true, true);
		아니면
			imsic_local_eix_update(i, false, false);
	}

	/* Clear IPI pending */
	imsic_local_eix_update(IMSIC_IPI_ID, true, false);

	/* Local IMSIC initialization */
	imsic_local_irqchip_init();

	반환 0;
}

정수 imsic_data_check(struct imsic_data *imsic)
{
	u32 i, tmp;
	unsigned long base_addr, addr, mask;

	/* Sanity checks */
	만약 (!imsic ||
	    (imsic->num_ids < IMSIC_MIN_ID) ||
	    (IMSIC_MAX_ID < imsic->num_ids))
		반환 SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT;
	만약 (tmp < imsic->guest_index_bits)
		반환 SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_index_bits;
	만약 (tmp < imsic->hart_index_bits)
		반환 SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_index_bits - imsic->hart_index_bits;
	만약 (tmp < imsic->group_index_bits)
		반환 SBI_EINVAL;

	tmp = IMSIC_MMIO_PAGE_SHIFT + imsic->guest_index_bits +
	      imsic->hart_index_bits;
	만약 (imsic->group_index_shift < tmp)
		반환 SBI_EINVAL;
	tmp = imsic->group_index_bits + imsic->group_index_shift - 1;
	만약 (tmp >= BITS_PER_LONG)
		반환 SBI_EINVAL;

	/*
	 * Number of interrupt identities should be 1 less than
	 * multiple of 63
	 */
	만약 ((imsic->num_ids & IMSIC_MIN_ID) != IMSIC_MIN_ID)
		반환 SBI_EINVAL;

	/* We should have at least one regset */
	만약 (!imsic->regs[0].size)
		반환 SBI_EINVAL;

	/* Match patter of each regset */
	base_addr = imsic->regs[0].addr;
	base_addr &= ~((1UL << (imsic->guest_index_bits +
				 imsic->hart_index_bits +
				 IMSIC_MMIO_PAGE_SHIFT)) - 1);
	base_addr &= ~(((1UL << imsic->group_index_bits) - 1) <<
			imsic->group_index_shift);
	위해 (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].size; i++) {
		mask = (1UL << imsic->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;
		mask -= 1UL;
		만약 (imsic->regs[i].size & mask)
			반환 SBI_EINVAL;

		addr = imsic->regs[i].addr;
		addr &= ~((1UL << (imsic->guest_index_bits +
					 imsic->hart_index_bits +
					 IMSIC_MMIO_PAGE_SHIFT)) - 1);
		addr &= ~(((1UL << imsic->group_index_bits) - 1) <<
				imsic->group_index_shift);
		만약 (base_addr != addr)
			반환 SBI_EINVAL;
	}

	반환 0;
}

static 정수 imsic_hwirq_setup(struct sbi_irqchip_device *chip, u32 hwirq, u32 hwirq_flags)
{
	만약 (hwirq_flags != SBI_HWIRQ_FLAGS_NONE)
		반환 SBI_ENOTSUPP;

	sbi_irqchip_set_raw_handler(chip, hwirq,
				    sbi_irqchip_raw_handler_default);

	반환 0;
}

static void imsic_hwirq_cleanup(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct imsic_data *imsic;

	만약 (!chip)
		반환;

	imsic = container_of(chip, struct imsic_data, irqchip);
	만약 (!imsic || !imsic->targets_mmode)
		반환;

	imsic_local_eix_update(hwirq, false, false);
	imsic_local_eix_update(hwirq, true, false);
}

static void imsic_hwirq_eoi(struct sbi_irqchip_device *chip, u32 hwirq)
{
	/*
	 * IMSIC interrupt claim/ack is already done by reading CSR_MTOPEI
	 * in imsic_process_hwirqs(). No extra EOI operation is required.
	 */
}

static 정수 imsic_compose_msi_msg(struct imsic_data *imsic,
				 u32 hart_index, u32 eiid,
				 struct sbi_irqchip_msi_msg *msg)
{
	struct imsic_regs *regs;
	unsigned long reloff;
	u64 msi_addr;
	정수 file;

	만약 (!imsic || !msg)
		반환 SBI_EINVAL;

	만약 (!eiid || eiid == IMSIC_IPI_ID)
		반환 0;

	file = imsic_get_target_file(hart_index);
	만약 (file < 0) {
		sbi_printf("%s: no file for hart=%lu rc=%d\n",
				__func__,
				(unsigned long)hart_index, file);
		반환 file;
	}

	regs   = &imsic->regs[0];
	reloff = file * (1UL << imsic->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;

	동안 (regs->size && regs->size <= reloff) {
		reloff -= regs->size;
		regs++;
	}

	만약 (!regs->size || regs->size <= reloff) {
		sbi_printf("%s: no regset for hart=%lu file=%d reloff=0x%lx\n",
			   __func__,
			   (unsigned long)hart_index, file, reloff);
		반환 SBI_ENODEV;
	}

	msi_addr = (u64)regs->addr + reloff + IMSIC_MMIO_PAGE_LE;
	msg->address_lo = (u32)(msi_addr);
	msg->address_hi = (u32)(msi_addr >> 32);
	msg->data       = eiid;

	반환 0;
}

static 정수 imsic_program_msi(struct sbi_irqchip_device *chip,
		      u32 eiid, u32 hart_index)
{
	struct imsic_data *imsic;
	struct sbi_irqchip_msi_msg msg;
	정수 rc;

	만약 (!chip)
		반환 SBI_EINVAL;

	imsic = imsic_get_data(hart_index);
	만약 (!imsic || !imsic->targets_mmode)
		반환 SBI_ENODEV;

	rc = imsic_compose_msi_msg(imsic, hart_index, eiid, &msg);
	만약 (rc) {
		sbi_printf("%s: compose failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)eiid,
				(unsigned long)hart_index, rc);
	}

	rc = sbi_irqchip_write_msi(chip, eiid, &msg);
	만약 (rc) {
		sbi_printf("%s: write_msi failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)eiid,
				(unsigned long)hart_index, rc);
		반환 rc;
	}

	반환 0;
}

static 정수 imsic_hwirq_set_affinity(struct sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_index)
{
	struct imsic_data *imsic;
	정수 rc;

	만약 (!chip)
		반환 SBI_EINVAL;

	imsic = container_of(chip, struct imsic_data, irqchip);
	만약 (!imsic || !imsic->targets_mmode)
		반환 SBI_EINVAL;

	만약 (!hwirq || hwirq == IMSIC_IPI_ID)
		반환 0;

	만약 (!sbi_hartmask_test_hartindex(hart_index, &chip->target_harts))
		반환 SBI_EINVAL;

	rc = imsic_program_msi(chip, hwirq, hart_index);
	만약 (rc) {
		sbi_printf("%s: failed hwirq=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)hwirq,
				(unsigned long)hart_index, rc);
		반환 rc;
	}

	반환 0;
}

static void imsic_hwirq_mask(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct imsic_data *imsic;

	만약 (!chip)
		반환;

	imsic = container_of(chip, struct imsic_data, irqchip);
	만약 (!imsic || !imsic->targets_mmode)
		반환;

	imsic_local_eix_update(hwirq, false, false);
}

static void imsic_hwirq_unmask(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct imsic_data *imsic;

	만약 (!chip)
		반환;

	imsic = container_of(chip, struct imsic_data, irqchip);
	만약 (!imsic || !imsic->targets_mmode)
		반환;

	만약 (!hwirq || hwirq == IMSIC_IPI_ID)
		반환;

	imsic_local_eix_update(hwirq, false, true);
}

static struct sbi_irqchip_device imsic_device = {
	.warm_init		= imsic_warm_irqchip_init,
	.process_hwirqs		= imsic_process_hwirqs,
	.hwirq_setup		= imsic_hwirq_setup,
	.hwirq_cleanup		= imsic_hwirq_cleanup,
	.hwirq_eoi		= imsic_hwirq_eoi,
	.hwirq_set_affinity	= imsic_hwirq_set_affinity,
	.hwirq_mask		= imsic_hwirq_mask,
	.hwirq_unmask		= imsic_hwirq_unmask,
};

정수 imsic_cold_irqchip_init(struct imsic_data *imsic)
{
	정수 i, rc;

	/* Sanity checks */
	rc = imsic_data_check(imsic);
	만약 (rc)
		반환 rc;

	/* We only initialize M-mode IMSIC */
	만약 (!imsic->targets_mmode)
		반환 SBI_EINVAL;

	/* Allocate scratch space pointer */
	만약 (!imsic_ptr_offset) {
		imsic_ptr_offset = sbi_scratch_alloc_type_offset(void *);
		만약 (!imsic_ptr_offset)
			반환 SBI_ENOMEM;
	}

	/* Allocate scratch space file */
	만약 (!imsic_file_offset) {
		imsic_file_offset = sbi_scratch_alloc_type_offset(long);
		만약 (!imsic_file_offset)
			반환 SBI_ENOMEM;
	}

	/* Add IMSIC regions to the root domain */
	위해 (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].size; i++) {
		rc = sbi_domain_root_add_memrange(imsic->regs[i].addr,
						  imsic->regs[i].size,
						  IMSIC_MMIO_PAGE_SZ,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		만약 (rc)
			반환 rc;
	}

	imsic->irqchip          = imsic_device;
	imsic->irqchip.id       = imsic->unique_id;
	imsic_device.caps = SBI_IRQCHIP_CAPS_MSI;
	imsic->irqchip.num_hwirq = imsic->num_ids + 1;

	sbi_hartmask_set_all(&imsic->irqchip.target_harts);

	/* Register irqchip device */
	rc = sbi_irqchip_add_device(&imsic->irqchip);
	만약 (rc) {
		sbi_printf("%s: sbi_irqchip_add_device failed rc=%d\n",
				__func__, rc);
		반환 rc;
	}

	rc = sbi_irqchip_register_reserved(&imsic->irqchip, 0, IMSIC_IPI_ID + 1);
	만약 (rc) {
		sbi_printf("%s: register_reserved failed rc=%d\n",
				__func__, rc);
		반환 rc;
	}

	/* Register IPI device */
	sbi_ipi_add_device(&imsic_ipi_device);

	반환 0;
}
