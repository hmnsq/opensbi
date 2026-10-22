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

#تعریف_کرو IMSIC_MMIO_ورقہ_LE		0x00
#تعریف_کرو IMSIC_MMIO_ورقہ_BE		0x04

#تعریف_کرو IMSIC_MIN_ID			63
#تعریف_کرو IMSIC_MAX_ID			2047

#تعریف_کرو IMSIC_EIDELIVERY		0x70

#تعریف_کرو IMSIC_EITHRESHOLD		0x72

#تعریف_کرو IMSIC_TOPEI			0x76
#تعریف_کرو IMSIC_TOPEI_ID_کھسکاؤ		16
#تعریف_کرو IMSIC_TOPEI_ID_نقاب		0x7ff
#تعریف_کرو IMSIC_TOPEI_PRIO_نقاب		0x7ff

#تعریف_کرو IMSIC_EIP0			0x80

#تعریف_کرو IMSIC_EIP63			0xbf

#تعریف_کرو IMSIC_EIPx_بٹ			32

#تعریف_کرو IMSIC_EIE0			0xc0

#تعریف_کرو IMSIC_EIE63			0xff

#تعریف_کرو IMSIC_EIEx_بٹ			32

#تعریف_کرو IMSIC_DISABLE_EIDELIVERY	0
#تعریف_کرو IMSIC_ENABLE_EIDELIVERY		1
#تعریف_کرو IMSIC_DISABLE_EITHRESHOLD	1
#تعریف_کرو IMSIC_ENABLE_EITHRESHOLD	0

#تعریف_کرو IMSIC_IPI_ID			1

#تعریف_کرو imsic_csr_write(__c, __v)	\
افعل { \
	csr_write(CSR_MISELECT, __c); \
	csr_write(CSR_MIREG, __v); \
} طالما (0)

#تعریف_کرو imsic_csr_read(__c)	\
({ \
	بنانشانی لما __v; \
	csr_write(CSR_MISELECT, __c); \
	__v = csr_read(CSR_MIREG); \
	__v; \
})

#تعریف_کرو imsic_csr_سیٹ(__c, __v)		\
افعل { \
	csr_write(CSR_MISELECT, __c); \
	csr_سیٹ(CSR_MIREG, __v); \
} طالما (0)

#تعریف_کرو imsic_csr_clear(__c, __v)	\
افعل { \
	csr_write(CSR_MISELECT, __c); \
	csr_clear(CSR_MIREG, __v); \
} طالما (0)

ساكن بنانشانی لما imsic_ptr_وتھ;

#تعریف_کرو imsic_get_hart_انکڑے_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), فراغ *, imsic_ptr_وتھ)

#تعریف_کرو imsic_سیٹ_hart_انکڑے_ptr(__scratch, __imsic)			\
	sbi_scratch_write_type((__scratch), فراغ *, imsic_ptr_وتھ, (__imsic))

ساكن بنانشانی لما imsic_file_وتھ;

#تعریف_کرو imsic_get_hart_file(__scratch)					\
	sbi_scratch_read_type((__scratch), لما, imsic_file_وتھ)

#تعریف_کرو imsic_سیٹ_hart_file(__scratch, __file)				\
	sbi_scratch_write_type((__scratch), لما, imsic_file_وتھ, (__file))

صحيح imsic_map_hartid_منزل_انکڑے(u32 hartid, بنية imsic_انکڑے *imsic, صحيح file)
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

	imsic_سیٹ_hart_انکڑے_ptr(scratch, imsic);
	imsic_سیٹ_hart_file(scratch, file);
	أرجع 0;
}

بنية imsic_انکڑے *imsic_get_انکڑے(u32 hartindex)
{
	بنية sbi_scratch *scratch;

	إذا (!imsic_ptr_وتھ)
		أرجع عدم;

	scratch = sbi_hartindex_منزل_scratch(hartindex);
	إذا (!scratch)
		أرجع عدم;

	أرجع imsic_get_hart_انکڑے_ptr(scratch);
}

صحيح imsic_get_نشانہ_file(u32 hartindex)
{
	بنية sbi_scratch *scratch;

	إذا (!imsic_file_وتھ)
		أرجع SBI_ENOENT;

	scratch = sbi_hartindex_منزل_scratch(hartindex);
	إذا (!scratch)
		أرجع SBI_ENOENT;

	أرجع imsic_get_hart_file(scratch);
}

ساكن صحيح imsic_process_hwirqs(بنية sbi_irqchip_device *chip)
{
	بنية imsic_انکڑے *imsic;
	ulong mirq;
	u32 hwirq;
	صحيح rc, ret = 0;

	imsic = container_دا(chip, بنية imsic_انکڑے, irqchip);
	إذا (!imsic || !imsic->targets_mmode)
		أرجع SBI_EINVAL;

	إذا (imsic_get_انکڑے(current_hartindex()) != imsic)
		أرجع 0;

	طالما ((mirq = csr_وٹاندرا(CSR_MTOPEI, 0))) {
		hwirq = (mirq >> IMSIC_TOPEI_ID_کھسکاؤ) &
			IMSIC_TOPEI_ID_نقاب;

		إذا (hwirq > imsic->عدد_ids) {
			sbi_printf("%s: invalid hwirq=%lu num_ids=%lu\n",
					__func__,
					(بنانشانی لما)hwirq,
					(بنانشانی لما)imsic->عدد_ids);
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
					(بنانشانی لما)hwirq, rc);
			ret = rc;
		}
	}

	أرجع ret;
}

ساكن فراغ imsic_ipi_send(u32 hart_اشاریہ)
{
	بنانشانی لما reloff;
	بنية imsic_regs *regs;
	بنية imsic_انکڑے *انکڑے;
	بنية sbi_scratch *scratch;
	صحيح file;

	scratch = sbi_hartindex_منزل_scratch(hart_اشاریہ);
	إذا (!scratch)
		أرجع;

	انکڑے = imsic_get_hart_انکڑے_ptr(scratch);
	file = imsic_get_hart_file(scratch);
	إذا (!انکڑے || !انکڑے->targets_mmode)
		أرجع;

	regs = &انکڑے->regs[0];
	reloff = file * (1UL << انکڑے->guest_اشاریہ_بٹ) * IMSIC_MMIO_ورقہ_SZ;
	طالما (regs->ناپ && (regs->ناپ <= reloff)) {
		reloff -= regs->ناپ;
		regs++;
	}

	إذا (regs->ناپ && (reloff < regs->ناپ))
		writel_relaxed(IMSIC_IPI_ID,
			(فراغ *)(regs->addr + reloff + IMSIC_MMIO_ورقہ_LE));
}

ساكن بنية sbi_ipi_device imsic_ipi_device = {
	.name		= "aia-imsic",
	.rating		= 300,
	.ipi_send	= imsic_ipi_send
};

ساكن فراغ imsic_local_eix_update(بنانشانی لما id,
				   bool pend, bool val)
{
	بنانشانی لما isel, ireg = 0;

	isel = id / __riscv_xlen;
	isel *= __riscv_xlen / IMSIC_EIPx_بٹ;
	isel += (pend) ? IMSIC_EIP0 : IMSIC_EIE0;
	ireg |= ثنائی_ہندسہ(id);

	إذا (val)
		imsic_csr_سیٹ(isel, ireg);
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
	بنية imsic_انکڑے *imsic;
	بنية imsic_انکڑے *hart_imsic;
	صحيح i;

	imsic = container_دا(dev, بنية imsic_انکڑے, irqchip);
	hart_imsic = imsic_get_انکڑے(current_hartindex());

	/* Sanity checks */
	إذا (!hart_imsic || hart_imsic != imsic ||
		!hart_imsic->targets_mmode)
		أرجع SBI_EINVAL;

	/* enable interrutps based on the irq state */
	لكل (i = 1; i < imsic->عدد_ids; i++) {
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

صحيح imsic_انکڑے_check(بنية imsic_انکڑے *imsic)
{
	u32 i, tmp;
	بنانشانی لما ادھار_addr, addr, نقاب;

	/* Sanity checks */
	إذا (!imsic ||
	    (imsic->عدد_ids < IMSIC_MIN_ID) ||
	    (IMSIC_MAX_ID < imsic->عدد_ids))
		أرجع SBI_EINVAL;

	tmp = بٹ_فی_LONG - IMSIC_MMIO_ورقہ_کھسکاؤ;
	إذا (tmp < imsic->guest_اشاریہ_بٹ)
		أرجع SBI_EINVAL;

	tmp = بٹ_فی_LONG - IMSIC_MMIO_ورقہ_کھسکاؤ -
	      imsic->guest_اشاریہ_بٹ;
	إذا (tmp < imsic->hart_اشاریہ_بٹ)
		أرجع SBI_EINVAL;

	tmp = بٹ_فی_LONG - IMSIC_MMIO_ورقہ_کھسکاؤ -
	      imsic->guest_اشاریہ_بٹ - imsic->hart_اشاریہ_بٹ;
	إذا (tmp < imsic->گروہ_اشاریہ_بٹ)
		أرجع SBI_EINVAL;

	tmp = IMSIC_MMIO_ورقہ_کھسکاؤ + imsic->guest_اشاریہ_بٹ +
	      imsic->hart_اشاریہ_بٹ;
	إذا (imsic->گروہ_اشاریہ_کھسکاؤ < tmp)
		أرجع SBI_EINVAL;
	tmp = imsic->گروہ_اشاریہ_بٹ + imsic->گروہ_اشاریہ_کھسکاؤ - 1;
	إذا (tmp >= بٹ_فی_LONG)
		أرجع SBI_EINVAL;

	/*
	 * Number of interrupt identities should be 1 less than
	 * multiple of 63
	 */
	إذا ((imsic->عدد_ids & IMSIC_MIN_ID) != IMSIC_MIN_ID)
		أرجع SBI_EINVAL;

	/* We should have at least one regset */
	إذا (!imsic->regs[0].ناپ)
		أرجع SBI_EINVAL;

	/* Match patter of each regset */
	ادھار_addr = imsic->regs[0].addr;
	ادھار_addr &= ~((1UL << (imsic->guest_اشاریہ_بٹ +
				 imsic->hart_اشاریہ_بٹ +
				 IMSIC_MMIO_ورقہ_کھسکاؤ)) - 1);
	ادھار_addr &= ~(((1UL << imsic->گروہ_اشاریہ_بٹ) - 1) <<
			imsic->گروہ_اشاریہ_کھسکاؤ);
	لكل (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].ناپ; i++) {
		نقاب = (1UL << imsic->guest_اشاریہ_بٹ) * IMSIC_MMIO_ورقہ_SZ;
		نقاب -= 1UL;
		إذا (imsic->regs[i].ناپ & نقاب)
			أرجع SBI_EINVAL;

		addr = imsic->regs[i].addr;
		addr &= ~((1UL << (imsic->guest_اشاریہ_بٹ +
					 imsic->hart_اشاریہ_بٹ +
					 IMSIC_MMIO_ورقہ_کھسکاؤ)) - 1);
		addr &= ~(((1UL << imsic->گروہ_اشاریہ_بٹ) - 1) <<
				imsic->گروہ_اشاریہ_کھسکاؤ);
		إذا (ادھار_addr != addr)
			أرجع SBI_EINVAL;
	}

	أرجع 0;
}

ساكن صحيح imsic_hwirq_تیاری(بنية sbi_irqchip_device *chip, u32 hwirq, u32 hwirq_flags)
{
	إذا (hwirq_flags != SBI_HWIRQ_FLAGS_NONE)
		أرجع SBI_ENOTSUPP;

	sbi_irqchip_سیٹ_raw_handler(chip, hwirq,
				    sbi_irqchip_raw_handler_default);

	أرجع 0;
}

ساكن فراغ imsic_hwirq_cleanup(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية imsic_انکڑے *imsic;

	إذا (!chip)
		أرجع;

	imsic = container_دا(chip, بنية imsic_انکڑے, irqchip);
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

ساكن صحيح imsic_compose_msi_پیغام(بنية imsic_انکڑے *imsic,
				 u32 hart_اشاریہ, u32 eiid,
				 بنية sbi_irqchip_msi_پیغام *پیغام)
{
	بنية imsic_regs *regs;
	بنانشانی لما reloff;
	u64 msi_addr;
	صحيح file;

	إذا (!imsic || !پیغام)
		أرجع SBI_EINVAL;

	إذا (!eiid || eiid == IMSIC_IPI_ID)
		أرجع 0;

	file = imsic_get_نشانہ_file(hart_اشاریہ);
	إذا (file < 0) {
		sbi_printf("%s: no file for hart=%lu rc=%d\n",
				__func__,
				(بنانشانی لما)hart_اشاریہ, file);
		أرجع file;
	}

	regs   = &imsic->regs[0];
	reloff = file * (1UL << imsic->guest_اشاریہ_بٹ) * IMSIC_MMIO_ورقہ_SZ;

	طالما (regs->ناپ && regs->ناپ <= reloff) {
		reloff -= regs->ناپ;
		regs++;
	}

	إذا (!regs->ناپ || regs->ناپ <= reloff) {
		sbi_printf("%s: no regset for hart=%lu file=%d reloff=0x%lx\n",
			   __func__,
			   (بنانشانی لما)hart_اشاریہ, file, reloff);
		أرجع SBI_ENODEV;
	}

	msi_addr = (u64)regs->addr + reloff + IMSIC_MMIO_ورقہ_LE;
	پیغام->پتہ_lo = (u32)(msi_addr);
	پیغام->پتہ_hi = (u32)(msi_addr >> 32);
	پیغام->انکڑے       = eiid;

	أرجع 0;
}

ساكن صحيح imsic_پروگرام_msi(بنية sbi_irqchip_device *chip,
		      u32 eiid, u32 hart_اشاریہ)
{
	بنية imsic_انکڑے *imsic;
	بنية sbi_irqchip_msi_پیغام پیغام;
	صحيح rc;

	إذا (!chip)
		أرجع SBI_EINVAL;

	imsic = imsic_get_انکڑے(hart_اشاریہ);
	إذا (!imsic || !imsic->targets_mmode)
		أرجع SBI_ENODEV;

	rc = imsic_compose_msi_پیغام(imsic, hart_اشاریہ, eiid, &پیغام);
	إذا (rc) {
		sbi_printf("%s: compose failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(بنانشانی لما)eiid,
				(بنانشانی لما)hart_اشاریہ, rc);
	}

	rc = sbi_irqchip_write_msi(chip, eiid, &پیغام);
	إذا (rc) {
		sbi_printf("%s: write_msi failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(بنانشانی لما)eiid,
				(بنانشانی لما)hart_اشاریہ, rc);
		أرجع rc;
	}

	أرجع 0;
}

ساكن صحيح imsic_hwirq_سیٹ_affinity(بنية sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_اشاریہ)
{
	بنية imsic_انکڑے *imsic;
	صحيح rc;

	إذا (!chip)
		أرجع SBI_EINVAL;

	imsic = container_دا(chip, بنية imsic_انکڑے, irqchip);
	إذا (!imsic || !imsic->targets_mmode)
		أرجع SBI_EINVAL;

	إذا (!hwirq || hwirq == IMSIC_IPI_ID)
		أرجع 0;

	إذا (!sbi_hartmask_test_hartindex(hart_اشاریہ, &chip->نشانہ_harts))
		أرجع SBI_EINVAL;

	rc = imsic_پروگرام_msi(chip, hwirq, hart_اشاریہ);
	إذا (rc) {
		sbi_printf("%s: failed hwirq=%lu hart=%lu rc=%d\n",
				__func__,
				(بنانشانی لما)hwirq,
				(بنانشانی لما)hart_اشاریہ, rc);
		أرجع rc;
	}

	أرجع 0;
}

ساكن فراغ imsic_hwirq_نقاب(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية imsic_انکڑے *imsic;

	إذا (!chip)
		أرجع;

	imsic = container_دا(chip, بنية imsic_انکڑے, irqchip);
	إذا (!imsic || !imsic->targets_mmode)
		أرجع;

	imsic_local_eix_update(hwirq, false, false);
}

ساكن فراغ imsic_hwirq_unmask(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية imsic_انکڑے *imsic;

	إذا (!chip)
		أرجع;

	imsic = container_دا(chip, بنية imsic_انکڑے, irqchip);
	إذا (!imsic || !imsic->targets_mmode)
		أرجع;

	إذا (!hwirq || hwirq == IMSIC_IPI_ID)
		أرجع;

	imsic_local_eix_update(hwirq, false, true);
}

ساكن بنية sbi_irqchip_device imsic_device = {
	.warm_init		= imsic_warm_irqchip_init,
	.process_hwirqs		= imsic_process_hwirqs,
	.hwirq_تیاری		= imsic_hwirq_تیاری,
	.hwirq_cleanup		= imsic_hwirq_cleanup,
	.hwirq_eoi		= imsic_hwirq_eoi,
	.hwirq_سیٹ_affinity	= imsic_hwirq_سیٹ_affinity,
	.hwirq_نقاب		= imsic_hwirq_نقاب,
	.hwirq_unmask		= imsic_hwirq_unmask,
};

صحيح imsic_cold_irqchip_init(بنية imsic_انکڑے *imsic)
{
	صحيح i, rc;

	/* Sanity checks */
	rc = imsic_انکڑے_check(imsic);
	إذا (rc)
		أرجع rc;

	/* We only initialize M-mode IMSIC */
	إذا (!imsic->targets_mmode)
		أرجع SBI_EINVAL;

	/* Allocate scratch space pointer */
	إذا (!imsic_ptr_وتھ) {
		imsic_ptr_وتھ = sbi_scratch_alloc_type_وتھ(فراغ *);
		إذا (!imsic_ptr_وتھ)
			أرجع SBI_ENOMEM;
	}

	/* Allocate scratch space file */
	إذا (!imsic_file_وتھ) {
		imsic_file_وتھ = sbi_scratch_alloc_type_وتھ(لما);
		إذا (!imsic_file_وتھ)
			أرجع SBI_ENOMEM;
	}

	/* Add IMSIC regions to the root domain */
	لكل (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].ناپ; i++) {
		rc = sbi_domain_جڑ_جوڑو_memrange(imsic->regs[i].addr,
						  imsic->regs[i].ناپ,
						  IMSIC_MMIO_ورقہ_SZ,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		إذا (rc)
			أرجع rc;
	}

	imsic->irqchip          = imsic_device;
	imsic->irqchip.id       = imsic->unique_id;
	imsic_device.caps = SBI_IRQCHIP_CAPS_MSI;
	imsic->irqchip.عدد_hwirq = imsic->عدد_ids + 1;

	sbi_hartmask_سیٹ_all(&imsic->irqchip.نشانہ_harts);

	/* Register irqchip device */
	rc = sbi_irqchip_جوڑو_device(&imsic->irqchip);
	إذا (rc) {
		sbi_printf("%s: sbi_irqchip_add_device failed rc=%d\n",
				__func__, rc);
		أرجع rc;
	}

	rc = sbi_irqchip_رجسٹر_reserved(&imsic->irqchip, 0, IMSIC_IPI_ID + 1);
	إذا (rc) {
		sbi_printf("%s: register_reserved failed rc=%d\n",
				__func__, rc);
		أرجع rc;
	}

	/* Register IPI device */
	sbi_ipi_جوڑو_device(&imsic_ipi_device);

	أرجع 0;
}
