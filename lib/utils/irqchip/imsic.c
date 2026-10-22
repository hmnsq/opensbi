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

#وصف_ڪريو IMSIC_MMIO_صفحو_LE		0x00
#وصف_ڪريو IMSIC_MMIO_صفحو_BE		0x04

#وصف_ڪريو IMSIC_MIN_ID			63
#وصف_ڪريو IMSIC_MAX_ID			2047

#وصف_ڪريو IMSIC_EIDELIVERY		0x70

#وصف_ڪريو IMSIC_EITHRESHOLD		0x72

#وصف_ڪريو IMSIC_TOPEI			0x76
#وصف_ڪريو IMSIC_TOPEI_ID_سرڪايو		16
#وصف_ڪريو IMSIC_TOPEI_ID_نقاب		0x7ff
#وصف_ڪريو IMSIC_TOPEI_PRIO_نقاب		0x7ff

#وصف_ڪريو IMSIC_EIP0			0x80

#وصف_ڪريو IMSIC_EIP63			0xbf

#وصف_ڪريو IMSIC_EIPx_بٽ			32

#وصف_ڪريو IMSIC_EIE0			0xc0

#وصف_ڪريو IMSIC_EIE63			0xff

#وصف_ڪريو IMSIC_EIEx_بٽ			32

#وصف_ڪريو IMSIC_DISABLE_EIDELIVERY	0
#وصف_ڪريو IMSIC_ENABLE_EIDELIVERY		1
#وصف_ڪريو IMSIC_DISABLE_EITHRESHOLD	1
#وصف_ڪريو IMSIC_ENABLE_EITHRESHOLD	0

#وصف_ڪريو IMSIC_IPI_ID			1

#وصف_ڪريو imsic_csr_write(__c, __v)	\
افعل { \
	csr_write(CSR_MISELECT, __c); \
	csr_write(CSR_MIREG, __v); \
} طالما (0)

#وصف_ڪريو imsic_csr_read(__c)	\
({ \
	بنانشان ڊگهو __v; \
	csr_write(CSR_MISELECT, __c); \
	__v = csr_read(CSR_MIREG); \
	__v; \
})

#وصف_ڪريو imsic_csr_سيٽ(__c, __v)		\
افعل { \
	csr_write(CSR_MISELECT, __c); \
	csr_سيٽ(CSR_MIREG, __v); \
} طالما (0)

#وصف_ڪريو imsic_csr_clear(__c, __v)	\
افعل { \
	csr_write(CSR_MISELECT, __c); \
	csr_clear(CSR_MIREG, __v); \
} طالما (0)

ساكن بنانشان ڊگهو imsic_ptr_وٿي;

#وصف_ڪريو imsic_get_hart_معلومات_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), فراغ *, imsic_ptr_وٿي)

#وصف_ڪريو imsic_سيٽ_hart_معلومات_ptr(__scratch, __imsic)			\
	sbi_scratch_write_type((__scratch), فراغ *, imsic_ptr_وٿي, (__imsic))

ساكن بنانشان ڊگهو imsic_file_وٿي;

#وصف_ڪريو imsic_get_hart_file(__scratch)					\
	sbi_scratch_read_type((__scratch), ڊگهو, imsic_file_وٿي)

#وصف_ڪريو imsic_سيٽ_hart_file(__scratch, __file)				\
	sbi_scratch_write_type((__scratch), ڊگهو, imsic_file_وٿي, (__file))

صحيح imsic_map_hartid_منزل_معلومات(u32 hartid, بنية imsic_معلومات *imsic, صحيح file)
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
	scratch = sbi_hartid_منزل_scratch(hartid);
	إذا (!scratch)
		أرجع 0;

	imsic_سيٽ_hart_معلومات_ptr(scratch, imsic);
	imsic_سيٽ_hart_file(scratch, file);
	أرجع 0;
}

بنية imsic_معلومات *imsic_get_معلومات(u32 hartindex)
{
	بنية sbi_scratch *scratch;

	إذا (!imsic_ptr_وٿي)
		أرجع عدم;

	scratch = sbi_hartindex_منزل_scratch(hartindex);
	إذا (!scratch)
		أرجع عدم;

	أرجع imsic_get_hart_معلومات_ptr(scratch);
}

صحيح imsic_get_هدف_file(u32 hartindex)
{
	بنية sbi_scratch *scratch;

	إذا (!imsic_file_وٿي)
		أرجع SBI_ENOENT;

	scratch = sbi_hartindex_منزل_scratch(hartindex);
	إذا (!scratch)
		أرجع SBI_ENOENT;

	أرجع imsic_get_hart_file(scratch);
}

ساكن صحيح imsic_process_hwirqs(بنية sbi_irqchip_device *chip)
{
	بنية imsic_معلومات *imsic;
	ulong mirq;
	u32 hwirq;
	صحيح rc, ret = 0;

	imsic = container_جو(chip, بنية imsic_معلومات, irqchip);
	إذا (!imsic || !imsic->targets_mmode)
		أرجع SBI_EINVAL;

	إذا (imsic_get_معلومات(current_hartindex()) != imsic)
		أرجع 0;

	طالما ((mirq = csr_مٽا_سٽا(CSR_MTOPEI, 0))) {
		hwirq = (mirq >> IMSIC_TOPEI_ID_سرڪايو) &
			IMSIC_TOPEI_ID_نقاب;

		إذا (hwirq > imsic->انگ_ids) {
			sbi_printf("%s: invalid hwirq=%lu num_ids=%lu\n",
					__func__,
					(بنانشان ڊگهو)hwirq,
					(بنانشان ڊگهو)imsic->انگ_ids);
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
					(بنانشان ڊگهو)hwirq, rc);
			ret = rc;
		}
	}

	أرجع ret;
}

ساكن فراغ imsic_ipi_send(u32 hart_اشاريو)
{
	بنانشان ڊگهو reloff;
	بنية imsic_regs *regs;
	بنية imsic_معلومات *معلومات;
	بنية sbi_scratch *scratch;
	صحيح file;

	scratch = sbi_hartindex_منزل_scratch(hart_اشاريو);
	إذا (!scratch)
		أرجع;

	معلومات = imsic_get_hart_معلومات_ptr(scratch);
	file = imsic_get_hart_file(scratch);
	إذا (!معلومات || !معلومات->targets_mmode)
		أرجع;

	regs = &معلومات->regs[0];
	reloff = file * (1UL << معلومات->guest_اشاريو_بٽ) * IMSIC_MMIO_صفحو_SZ;
	طالما (regs->سائيز && (regs->سائيز <= reloff)) {
		reloff -= regs->سائيز;
		regs++;
	}

	إذا (regs->سائيز && (reloff < regs->سائيز))
		writel_relaxed(IMSIC_IPI_ID,
			(فراغ *)(regs->addr + reloff + IMSIC_MMIO_صفحو_LE));
}

ساكن بنية sbi_ipi_device imsic_ipi_device = {
	.name		= "aia-imsic",
	.rating		= 300,
	.ipi_send	= imsic_ipi_send
};

ساكن فراغ imsic_local_eix_update(بنانشان ڊگهو id,
				   bool pend, bool val)
{
	بنانشان ڊگهو isel, ireg = 0;

	isel = id / __riscv_xlen;
	isel *= __riscv_xlen / IMSIC_EIPx_بٽ;
	isel += (pend) ? IMSIC_EIP0 : IMSIC_EIE0;
	ireg |= ٻٽو_انگ(id);

	إذا (val)
		imsic_csr_سيٽ(isel, ireg);
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
	بنية imsic_معلومات *imsic;
	بنية imsic_معلومات *hart_imsic;
	صحيح i;

	imsic = container_جو(dev, بنية imsic_معلومات, irqchip);
	hart_imsic = imsic_get_معلومات(current_hartindex());

	/* Sanity checks */
	إذا (!hart_imsic || hart_imsic != imsic ||
		!hart_imsic->targets_mmode)
		أرجع SBI_EINVAL;

	/* enable interrutps based on the irq state */
	لكل (i = 1; i < imsic->انگ_ids; i++) {
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

صحيح imsic_معلومات_check(بنية imsic_معلومات *imsic)
{
	u32 i, tmp;
	بنانشان ڊگهو بنياد_addr, addr, نقاب;

	/* Sanity checks */
	إذا (!imsic ||
	    (imsic->انگ_ids < IMSIC_MIN_ID) ||
	    (IMSIC_MAX_ID < imsic->انگ_ids))
		أرجع SBI_EINVAL;

	tmp = بٽ_في_LONG - IMSIC_MMIO_صفحو_سرڪايو;
	إذا (tmp < imsic->guest_اشاريو_بٽ)
		أرجع SBI_EINVAL;

	tmp = بٽ_في_LONG - IMSIC_MMIO_صفحو_سرڪايو -
	      imsic->guest_اشاريو_بٽ;
	إذا (tmp < imsic->hart_اشاريو_بٽ)
		أرجع SBI_EINVAL;

	tmp = بٽ_في_LONG - IMSIC_MMIO_صفحو_سرڪايو -
	      imsic->guest_اشاريو_بٽ - imsic->hart_اشاريو_بٽ;
	إذا (tmp < imsic->گروهه_اشاريو_بٽ)
		أرجع SBI_EINVAL;

	tmp = IMSIC_MMIO_صفحو_سرڪايو + imsic->guest_اشاريو_بٽ +
	      imsic->hart_اشاريو_بٽ;
	إذا (imsic->گروهه_اشاريو_سرڪايو < tmp)
		أرجع SBI_EINVAL;
	tmp = imsic->گروهه_اشاريو_بٽ + imsic->گروهه_اشاريو_سرڪايو - 1;
	إذا (tmp >= بٽ_في_LONG)
		أرجع SBI_EINVAL;

	/*
	 * Number of interrupt identities should be 1 less than
	 * multiple of 63
	 */
	إذا ((imsic->انگ_ids & IMSIC_MIN_ID) != IMSIC_MIN_ID)
		أرجع SBI_EINVAL;

	/* We should have at least one regset */
	إذا (!imsic->regs[0].سائيز)
		أرجع SBI_EINVAL;

	/* Match patter of each regset */
	بنياد_addr = imsic->regs[0].addr;
	بنياد_addr &= ~((1UL << (imsic->guest_اشاريو_بٽ +
				 imsic->hart_اشاريو_بٽ +
				 IMSIC_MMIO_صفحو_سرڪايو)) - 1);
	بنياد_addr &= ~(((1UL << imsic->گروهه_اشاريو_بٽ) - 1) <<
			imsic->گروهه_اشاريو_سرڪايو);
	لكل (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].سائيز; i++) {
		نقاب = (1UL << imsic->guest_اشاريو_بٽ) * IMSIC_MMIO_صفحو_SZ;
		نقاب -= 1UL;
		إذا (imsic->regs[i].سائيز & نقاب)
			أرجع SBI_EINVAL;

		addr = imsic->regs[i].addr;
		addr &= ~((1UL << (imsic->guest_اشاريو_بٽ +
					 imsic->hart_اشاريو_بٽ +
					 IMSIC_MMIO_صفحو_سرڪايو)) - 1);
		addr &= ~(((1UL << imsic->گروهه_اشاريو_بٽ) - 1) <<
				imsic->گروهه_اشاريو_سرڪايو);
		إذا (بنياد_addr != addr)
			أرجع SBI_EINVAL;
	}

	أرجع 0;
}

ساكن صحيح imsic_hwirq_تياري(بنية sbi_irqchip_device *chip, u32 hwirq, u32 hwirq_flags)
{
	إذا (hwirq_flags != SBI_HWIRQ_FLAGS_NONE)
		أرجع SBI_ENOTSUPP;

	sbi_irqchip_سيٽ_raw_handler(chip, hwirq,
				    sbi_irqchip_raw_handler_default);

	أرجع 0;
}

ساكن فراغ imsic_hwirq_cleanup(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية imsic_معلومات *imsic;

	إذا (!chip)
		أرجع;

	imsic = container_جو(chip, بنية imsic_معلومات, irqchip);
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

ساكن صحيح imsic_compose_msi_نياپو(بنية imsic_معلومات *imsic,
				 u32 hart_اشاريو, u32 eiid,
				 بنية sbi_irqchip_msi_نياپو *نياپو)
{
	بنية imsic_regs *regs;
	بنانشان ڊگهو reloff;
	u64 msi_addr;
	صحيح file;

	إذا (!imsic || !نياپو)
		أرجع SBI_EINVAL;

	إذا (!eiid || eiid == IMSIC_IPI_ID)
		أرجع 0;

	file = imsic_get_هدف_file(hart_اشاريو);
	إذا (file < 0) {
		sbi_printf("%s: no file for hart=%lu rc=%d\n",
				__func__,
				(بنانشان ڊگهو)hart_اشاريو, file);
		أرجع file;
	}

	regs   = &imsic->regs[0];
	reloff = file * (1UL << imsic->guest_اشاريو_بٽ) * IMSIC_MMIO_صفحو_SZ;

	طالما (regs->سائيز && regs->سائيز <= reloff) {
		reloff -= regs->سائيز;
		regs++;
	}

	إذا (!regs->سائيز || regs->سائيز <= reloff) {
		sbi_printf("%s: no regset for hart=%lu file=%d reloff=0x%lx\n",
			   __func__,
			   (بنانشان ڊگهو)hart_اشاريو, file, reloff);
		أرجع SBI_ENODEV;
	}

	msi_addr = (u64)regs->addr + reloff + IMSIC_MMIO_صفحو_LE;
	نياپو->پتو_lo = (u32)(msi_addr);
	نياپو->پتو_hi = (u32)(msi_addr >> 32);
	نياپو->معلومات       = eiid;

	أرجع 0;
}

ساكن صحيح imsic_پروگرام_msi(بنية sbi_irqchip_device *chip,
		      u32 eiid, u32 hart_اشاريو)
{
	بنية imsic_معلومات *imsic;
	بنية sbi_irqchip_msi_نياپو نياپو;
	صحيح rc;

	إذا (!chip)
		أرجع SBI_EINVAL;

	imsic = imsic_get_معلومات(hart_اشاريو);
	إذا (!imsic || !imsic->targets_mmode)
		أرجع SBI_ENODEV;

	rc = imsic_compose_msi_نياپو(imsic, hart_اشاريو, eiid, &نياپو);
	إذا (rc) {
		sbi_printf("%s: compose failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(بنانشان ڊگهو)eiid,
				(بنانشان ڊگهو)hart_اشاريو, rc);
	}

	rc = sbi_irqchip_write_msi(chip, eiid, &نياپو);
	إذا (rc) {
		sbi_printf("%s: write_msi failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(بنانشان ڊگهو)eiid,
				(بنانشان ڊگهو)hart_اشاريو, rc);
		أرجع rc;
	}

	أرجع 0;
}

ساكن صحيح imsic_hwirq_سيٽ_affinity(بنية sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_اشاريو)
{
	بنية imsic_معلومات *imsic;
	صحيح rc;

	إذا (!chip)
		أرجع SBI_EINVAL;

	imsic = container_جو(chip, بنية imsic_معلومات, irqchip);
	إذا (!imsic || !imsic->targets_mmode)
		أرجع SBI_EINVAL;

	إذا (!hwirq || hwirq == IMSIC_IPI_ID)
		أرجع 0;

	إذا (!sbi_hartmask_test_hartindex(hart_اشاريو, &chip->هدف_harts))
		أرجع SBI_EINVAL;

	rc = imsic_پروگرام_msi(chip, hwirq, hart_اشاريو);
	إذا (rc) {
		sbi_printf("%s: failed hwirq=%lu hart=%lu rc=%d\n",
				__func__,
				(بنانشان ڊگهو)hwirq,
				(بنانشان ڊگهو)hart_اشاريو, rc);
		أرجع rc;
	}

	أرجع 0;
}

ساكن فراغ imsic_hwirq_نقاب(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية imsic_معلومات *imsic;

	إذا (!chip)
		أرجع;

	imsic = container_جو(chip, بنية imsic_معلومات, irqchip);
	إذا (!imsic || !imsic->targets_mmode)
		أرجع;

	imsic_local_eix_update(hwirq, false, false);
}

ساكن فراغ imsic_hwirq_unmask(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية imsic_معلومات *imsic;

	إذا (!chip)
		أرجع;

	imsic = container_جو(chip, بنية imsic_معلومات, irqchip);
	إذا (!imsic || !imsic->targets_mmode)
		أرجع;

	إذا (!hwirq || hwirq == IMSIC_IPI_ID)
		أرجع;

	imsic_local_eix_update(hwirq, false, true);
}

ساكن بنية sbi_irqchip_device imsic_device = {
	.warm_init		= imsic_warm_irqchip_init,
	.process_hwirqs		= imsic_process_hwirqs,
	.hwirq_تياري		= imsic_hwirq_تياري,
	.hwirq_cleanup		= imsic_hwirq_cleanup,
	.hwirq_eoi		= imsic_hwirq_eoi,
	.hwirq_سيٽ_affinity	= imsic_hwirq_سيٽ_affinity,
	.hwirq_نقاب		= imsic_hwirq_نقاب,
	.hwirq_unmask		= imsic_hwirq_unmask,
};

صحيح imsic_cold_irqchip_init(بنية imsic_معلومات *imsic)
{
	صحيح i, rc;

	/* Sanity checks */
	rc = imsic_معلومات_check(imsic);
	إذا (rc)
		أرجع rc;

	/* We only initialize M-mode IMSIC */
	إذا (!imsic->targets_mmode)
		أرجع SBI_EINVAL;

	/* Allocate scratch space pointer */
	إذا (!imsic_ptr_وٿي) {
		imsic_ptr_وٿي = sbi_scratch_alloc_type_وٿي(فراغ *);
		إذا (!imsic_ptr_وٿي)
			أرجع SBI_ENOMEM;
	}

	/* Allocate scratch space file */
	إذا (!imsic_file_وٿي) {
		imsic_file_وٿي = sbi_scratch_alloc_type_وٿي(ڊگهو);
		إذا (!imsic_file_وٿي)
			أرجع SBI_ENOMEM;
	}

	/* Add IMSIC regions to the root domain */
	لكل (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].سائيز; i++) {
		rc = sbi_domain_پاڙ_جوڙيو_memrange(imsic->regs[i].addr,
						  imsic->regs[i].سائيز,
						  IMSIC_MMIO_صفحو_SZ,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		إذا (rc)
			أرجع rc;
	}

	imsic->irqchip          = imsic_device;
	imsic->irqchip.id       = imsic->unique_id;
	imsic_device.caps = SBI_IRQCHIP_CAPS_MSI;
	imsic->irqchip.انگ_hwirq = imsic->انگ_ids + 1;

	sbi_hartmask_سيٽ_all(&imsic->irqchip.هدف_harts);

	/* Register irqchip device */
	rc = sbi_irqchip_جوڙيو_device(&imsic->irqchip);
	إذا (rc) {
		sbi_printf("%s: sbi_irqchip_add_device failed rc=%d\n",
				__func__, rc);
		أرجع rc;
	}

	rc = sbi_irqchip_رجسٽر_reserved(&imsic->irqchip, 0, IMSIC_IPI_ID + 1);
	إذا (rc) {
		sbi_printf("%s: register_reserved failed rc=%d\n",
				__func__, rc);
		أرجع rc;
	}

	/* Register IPI device */
	sbi_ipi_جوڙيو_device(&imsic_ipi_device);

	أرجع 0;
}
