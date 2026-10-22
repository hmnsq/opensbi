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
} ᤀᤠ᤺ (0)

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
} ᤀᤠ᤺ (0)

#define imsic_csr_clear(__c, __v)	\
do { \
	csr_write(CSR_MISELECT, __c); \
	csr_clear(CSR_MIREG, __v); \
} ᤀᤠ᤺ (0)

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

puur_naa_nka imsic_map_hartid_to_data(u32 hartid, sa_nracanaa imsic_data *imsic, puur_naa_nka file)
{
	sa_nracanaa sbi_scratch *scratch;

	ᤀᤡ᤺ (!imsic || !imsic->targets_mmode)
		ᤀᤡᤴ SBI_EINVAL;

	/*
	 * We don't need to fail if scratch pointer is not available
	 * because we might be dealing with hartid of a HART disabled
	 * in device tree. For HARTs disabled in device tree, the
	 * imsic_get_data() and imsic_get_target_file() will anyway
	 * fail.
	 */
	scratch = sbi_hartid_to_scratch(hartid);
	ᤀᤡ᤺ (!scratch)
		ᤀᤡᤴ 0;

	imsic_set_hart_data_ptr(scratch, imsic);
	imsic_set_hart_file(scratch, file);
	ᤀᤡᤴ 0;
}

sa_nracanaa imsic_data *imsic_get_data(u32 hartindex)
{
	sa_nracanaa sbi_scratch *scratch;

	ᤀᤡ᤺ (!imsic_ptr_offset)
		ᤀᤡᤴ NULL;

	scratch = sbi_hartindex_to_scratch(hartindex);
	ᤀᤡ᤺ (!scratch)
		ᤀᤡᤴ NULL;

	ᤀᤡᤴ imsic_get_hart_data_ptr(scratch);
}

puur_naa_nka imsic_get_target_file(u32 hartindex)
{
	sa_nracanaa sbi_scratch *scratch;

	ᤀᤡ᤺ (!imsic_file_offset)
		ᤀᤡᤴ SBI_ENOENT;

	scratch = sbi_hartindex_to_scratch(hartindex);
	ᤀᤡ᤺ (!scratch)
		ᤀᤡᤴ SBI_ENOENT;

	ᤀᤡᤴ imsic_get_hart_file(scratch);
}

static puur_naa_nka imsic_process_hwirqs(sa_nracanaa sbi_irqchip_device *chip)
{
	sa_nracanaa imsic_data *imsic;
	ulong mirq;
	u32 hwirq;
	puur_naa_nka rc, ret = 0;

	imsic = container_of(chip, sa_nracanaa imsic_data, irqchip);
	ᤀᤡ᤺ (!imsic || !imsic->targets_mmode)
		ᤀᤡᤴ SBI_EINVAL;

	ᤀᤡ᤺ (imsic_get_data(current_hartindex()) != imsic)
		ᤀᤡᤴ 0;

	ᤀᤠ᤺ ((mirq = csr_swap(CSR_MTOPEI, 0))) {
		hwirq = (mirq >> IMSIC_TOPEI_ID_SHIFT) &
			IMSIC_TOPEI_ID_MASK;

		ᤀᤡ᤺ (hwirq > imsic->num_ids) {
			sbi_printf("%s: invalid hwirq=%lu num_ids=%lu\n",
					__func__,
					(unsigned long)hwirq,
					(unsigned long)imsic->num_ids);
			continue;
		}

		ᤀᤡ᤺ (hwirq == IMSIC_IPI_ID) {
			sbi_ipi_process();
			continue;
		}

		rc = sbi_irqchip_process_hwirq(chip, hwirq);
		ᤀᤡ᤺ (rc && rc != SBI_ENOENT) {
			sbi_printf("%s: hwirq=%lu failed rc=%d\n",
					__func__,
					(unsigned long)hwirq, rc);
			ret = rc;
		}
	}

	ᤀᤡᤴ ret;
}

static void imsic_ipi_send(u32 hart_index)
{
	unsigned long reloff;
	sa_nracanaa imsic_regs *regs;
	sa_nracanaa imsic_data *data;
	sa_nracanaa sbi_scratch *scratch;
	puur_naa_nka file;

	scratch = sbi_hartindex_to_scratch(hart_index);
	ᤀᤡ᤺ (!scratch)
		ᤀᤡᤴ;

	data = imsic_get_hart_data_ptr(scratch);
	file = imsic_get_hart_file(scratch);
	ᤀᤡ᤺ (!data || !data->targets_mmode)
		ᤀᤡᤴ;

	regs = &data->regs[0];
	reloff = file * (1UL << data->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;
	ᤀᤠ᤺ (regs->size && (regs->size <= reloff)) {
		reloff -= regs->size;
		regs++;
	}

	ᤀᤡ᤺ (regs->size && (reloff < regs->size))
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

	ᤀᤡ᤺ (val)
		imsic_csr_set(isel, ireg);
	ᤀᤡ᤺ᤆᤡ
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
	ᤀᤡ᤺ (trap.cause)
		ᤀᤡᤴ;

	/* Setup threshold to allow all enabled interrupts */
	imsic_csr_write(IMSIC_EITHRESHOLD, IMSIC_ENABLE_EITHRESHOLD);

	/* Enable interrupt delivery */
	imsic_csr_write(IMSIC_EIDELIVERY, IMSIC_ENABLE_EIDELIVERY);

	/* Enable IPI */
	imsic_local_eix_update(IMSIC_IPI_ID, false, true);
}

static puur_naa_nka imsic_warm_irqchip_init(sa_nracanaa sbi_irqchip_device *dev)
{
	sa_nracanaa imsic_data *imsic;
	sa_nracanaa imsic_data *hart_imsic;
	puur_naa_nka i;

	imsic = container_of(dev, sa_nracanaa imsic_data, irqchip);
	hart_imsic = imsic_get_data(current_hartindex());

	/* Sanity checks */
	ᤀᤡ᤺ (!hart_imsic || hart_imsic != imsic ||
		!hart_imsic->targets_mmode)
		ᤀᤡᤴ SBI_EINVAL;

	/* enable interrutps based on the irq state */
	for (i = 1; i < imsic->num_ids; i++) {
		ᤀᤡ᤺ (sbi_irqchip_is_hwirq_enabled(&imsic->irqchip, i) == true)
			imsic_local_eix_update(i, true, true);
		ᤀᤡ᤺ᤆᤡ
			imsic_local_eix_update(i, false, false);
	}

	/* Clear IPI pending */
	imsic_local_eix_update(IMSIC_IPI_ID, true, false);

	/* Local IMSIC initialization */
	imsic_local_irqchip_init();

	ᤀᤡᤴ 0;
}

puur_naa_nka imsic_data_check(sa_nracanaa imsic_data *imsic)
{
	u32 i, tmp;
	unsigned long base_addr, addr, mask;

	/* Sanity checks */
	ᤀᤡ᤺ (!imsic ||
	    (imsic->num_ids < IMSIC_MIN_ID) ||
	    (IMSIC_MAX_ID < imsic->num_ids))
		ᤀᤡᤴ SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT;
	ᤀᤡ᤺ (tmp < imsic->guest_index_bits)
		ᤀᤡᤴ SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_index_bits;
	ᤀᤡ᤺ (tmp < imsic->hart_index_bits)
		ᤀᤡᤴ SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_index_bits - imsic->hart_index_bits;
	ᤀᤡ᤺ (tmp < imsic->group_index_bits)
		ᤀᤡᤴ SBI_EINVAL;

	tmp = IMSIC_MMIO_PAGE_SHIFT + imsic->guest_index_bits +
	      imsic->hart_index_bits;
	ᤀᤡ᤺ (imsic->group_index_shift < tmp)
		ᤀᤡᤴ SBI_EINVAL;
	tmp = imsic->group_index_bits + imsic->group_index_shift - 1;
	ᤀᤡ᤺ (tmp >= BITS_PER_LONG)
		ᤀᤡᤴ SBI_EINVAL;

	/*
	 * Number of interrupt identities should be 1 less than
	 * multiple of 63
	 */
	ᤀᤡ᤺ ((imsic->num_ids & IMSIC_MIN_ID) != IMSIC_MIN_ID)
		ᤀᤡᤴ SBI_EINVAL;

	/* We should have at least one regset */
	ᤀᤡ᤺ (!imsic->regs[0].size)
		ᤀᤡᤴ SBI_EINVAL;

	/* Match patter of each regset */
	base_addr = imsic->regs[0].addr;
	base_addr &= ~((1UL << (imsic->guest_index_bits +
				 imsic->hart_index_bits +
				 IMSIC_MMIO_PAGE_SHIFT)) - 1);
	base_addr &= ~(((1UL << imsic->group_index_bits) - 1) <<
			imsic->group_index_shift);
	for (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].size; i++) {
		mask = (1UL << imsic->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;
		mask -= 1UL;
		ᤀᤡ᤺ (imsic->regs[i].size & mask)
			ᤀᤡᤴ SBI_EINVAL;

		addr = imsic->regs[i].addr;
		addr &= ~((1UL << (imsic->guest_index_bits +
					 imsic->hart_index_bits +
					 IMSIC_MMIO_PAGE_SHIFT)) - 1);
		addr &= ~(((1UL << imsic->group_index_bits) - 1) <<
				imsic->group_index_shift);
		ᤀᤡ᤺ (base_addr != addr)
			ᤀᤡᤴ SBI_EINVAL;
	}

	ᤀᤡᤴ 0;
}

static puur_naa_nka imsic_hwirq_setup(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq, u32 hwirq_flags)
{
	ᤀᤡ᤺ (hwirq_flags != SBI_HWIRQ_FLAGS_NONE)
		ᤀᤡᤴ SBI_ENOTSUPP;

	sbi_irqchip_set_raw_handler(chip, hwirq,
				    sbi_irqchip_raw_handler_default);

	ᤀᤡᤴ 0;
}

static void imsic_hwirq_cleanup(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa imsic_data *imsic;

	ᤀᤡ᤺ (!chip)
		ᤀᤡᤴ;

	imsic = container_of(chip, sa_nracanaa imsic_data, irqchip);
	ᤀᤡ᤺ (!imsic || !imsic->targets_mmode)
		ᤀᤡᤴ;

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

static puur_naa_nka imsic_compose_msi_msg(sa_nracanaa imsic_data *imsic,
				 u32 hart_index, u32 eiid,
				 sa_nracanaa sbi_irqchip_msi_msg *msg)
{
	sa_nracanaa imsic_regs *regs;
	unsigned long reloff;
	u64 msi_addr;
	puur_naa_nka file;

	ᤀᤡ᤺ (!imsic || !msg)
		ᤀᤡᤴ SBI_EINVAL;

	ᤀᤡ᤺ (!eiid || eiid == IMSIC_IPI_ID)
		ᤀᤡᤴ 0;

	file = imsic_get_target_file(hart_index);
	ᤀᤡ᤺ (file < 0) {
		sbi_printf("%s: no file for hart=%lu rc=%d\n",
				__func__,
				(unsigned long)hart_index, file);
		ᤀᤡᤴ file;
	}

	regs   = &imsic->regs[0];
	reloff = file * (1UL << imsic->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;

	ᤀᤠ᤺ (regs->size && regs->size <= reloff) {
		reloff -= regs->size;
		regs++;
	}

	ᤀᤡ᤺ (!regs->size || regs->size <= reloff) {
		sbi_printf("%s: no regset for hart=%lu file=%d reloff=0x%lx\n",
			   __func__,
			   (unsigned long)hart_index, file, reloff);
		ᤀᤡᤴ SBI_ENODEV;
	}

	msi_addr = (u64)regs->addr + reloff + IMSIC_MMIO_PAGE_LE;
	msg->address_lo = (u32)(msi_addr);
	msg->address_hi = (u32)(msi_addr >> 32);
	msg->data       = eiid;

	ᤀᤡᤴ 0;
}

static puur_naa_nka imsic_program_msi(sa_nracanaa sbi_irqchip_device *chip,
		      u32 eiid, u32 hart_index)
{
	sa_nracanaa imsic_data *imsic;
	sa_nracanaa sbi_irqchip_msi_msg msg;
	puur_naa_nka rc;

	ᤀᤡ᤺ (!chip)
		ᤀᤡᤴ SBI_EINVAL;

	imsic = imsic_get_data(hart_index);
	ᤀᤡ᤺ (!imsic || !imsic->targets_mmode)
		ᤀᤡᤴ SBI_ENODEV;

	rc = imsic_compose_msi_msg(imsic, hart_index, eiid, &msg);
	ᤀᤡ᤺ (rc) {
		sbi_printf("%s: compose failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)eiid,
				(unsigned long)hart_index, rc);
	}

	rc = sbi_irqchip_write_msi(chip, eiid, &msg);
	ᤀᤡ᤺ (rc) {
		sbi_printf("%s: write_msi failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)eiid,
				(unsigned long)hart_index, rc);
		ᤀᤡᤴ rc;
	}

	ᤀᤡᤴ 0;
}

static puur_naa_nka imsic_hwirq_set_affinity(sa_nracanaa sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_index)
{
	sa_nracanaa imsic_data *imsic;
	puur_naa_nka rc;

	ᤀᤡ᤺ (!chip)
		ᤀᤡᤴ SBI_EINVAL;

	imsic = container_of(chip, sa_nracanaa imsic_data, irqchip);
	ᤀᤡ᤺ (!imsic || !imsic->targets_mmode)
		ᤀᤡᤴ SBI_EINVAL;

	ᤀᤡ᤺ (!hwirq || hwirq == IMSIC_IPI_ID)
		ᤀᤡᤴ 0;

	ᤀᤡ᤺ (!sbi_hartmask_test_hartindex(hart_index, &chip->target_harts))
		ᤀᤡᤴ SBI_EINVAL;

	rc = imsic_program_msi(chip, hwirq, hart_index);
	ᤀᤡ᤺ (rc) {
		sbi_printf("%s: failed hwirq=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)hwirq,
				(unsigned long)hart_index, rc);
		ᤀᤡᤴ rc;
	}

	ᤀᤡᤴ 0;
}

static void imsic_hwirq_mask(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa imsic_data *imsic;

	ᤀᤡ᤺ (!chip)
		ᤀᤡᤴ;

	imsic = container_of(chip, sa_nracanaa imsic_data, irqchip);
	ᤀᤡ᤺ (!imsic || !imsic->targets_mmode)
		ᤀᤡᤴ;

	imsic_local_eix_update(hwirq, false, false);
}

static void imsic_hwirq_unmask(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa imsic_data *imsic;

	ᤀᤡ᤺ (!chip)
		ᤀᤡᤴ;

	imsic = container_of(chip, sa_nracanaa imsic_data, irqchip);
	ᤀᤡ᤺ (!imsic || !imsic->targets_mmode)
		ᤀᤡᤴ;

	ᤀᤡ᤺ (!hwirq || hwirq == IMSIC_IPI_ID)
		ᤀᤡᤴ;

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

puur_naa_nka imsic_cold_irqchip_init(sa_nracanaa imsic_data *imsic)
{
	puur_naa_nka i, rc;

	/* Sanity checks */
	rc = imsic_data_check(imsic);
	ᤀᤡ᤺ (rc)
		ᤀᤡᤴ rc;

	/* We only initialize M-mode IMSIC */
	ᤀᤡ᤺ (!imsic->targets_mmode)
		ᤀᤡᤴ SBI_EINVAL;

	/* Allocate scratch space pointer */
	ᤀᤡ᤺ (!imsic_ptr_offset) {
		imsic_ptr_offset = sbi_scratch_alloc_type_offset(void *);
		ᤀᤡ᤺ (!imsic_ptr_offset)
			ᤀᤡᤴ SBI_ENOMEM;
	}

	/* Allocate scratch space file */
	ᤀᤡ᤺ (!imsic_file_offset) {
		imsic_file_offset = sbi_scratch_alloc_type_offset(long);
		ᤀᤡ᤺ (!imsic_file_offset)
			ᤀᤡᤴ SBI_ENOMEM;
	}

	/* Add IMSIC regions to the root domain */
	for (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].size; i++) {
		rc = sbi_domain_root_add_memrange(imsic->regs[i].addr,
						  imsic->regs[i].size,
						  IMSIC_MMIO_PAGE_SZ,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		ᤀᤡ᤺ (rc)
			ᤀᤡᤴ rc;
	}

	imsic->irqchip          = imsic_device;
	imsic->irqchip.id       = imsic->unique_id;
	imsic_device.caps = SBI_IRQCHIP_CAPS_MSI;
	imsic->irqchip.num_hwirq = imsic->num_ids + 1;

	sbi_hartmask_set_all(&imsic->irqchip.target_harts);

	/* Register irqchip device */
	rc = sbi_irqchip_add_device(&imsic->irqchip);
	ᤀᤡ᤺ (rc) {
		sbi_printf("%s: sbi_irqchip_add_device failed rc=%d\n",
				__func__, rc);
		ᤀᤡᤴ rc;
	}

	rc = sbi_irqchip_register_reserved(&imsic->irqchip, 0, IMSIC_IPI_ID + 1);
	ᤀᤡ᤺ (rc) {
		sbi_printf("%s: register_reserved failed rc=%d\n",
				__func__, rc);
		ᤀᤡᤴ rc;
	}

	/* Register IPI device */
	sbi_ipi_add_device(&imsic_ipi_device);

	ᤀᤡᤴ 0;
}
