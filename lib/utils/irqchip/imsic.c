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

#تعریف_کرو IMSIC_MMIO_صفحہ_LE		0x00
#تعریف_کرو IMSIC_MMIO_صفحہ_BE		0x04

#تعریف_کرو IMSIC_MIN_ID			63
#تعریف_کرو IMSIC_MAX_ID			2047

#تعریف_کرو IMSIC_EIDELIVERY		0x70

#تعریف_کرو IMSIC_EITHRESHOLD		0x72

#تعریف_کرو IMSIC_TOPEI			0x76
#تعریف_کرو IMSIC_TOPEI_ID_سرکاؤ		16
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
کرو { \
	csr_write(CSR_MISELECT, __c); \
	csr_write(CSR_MIREG, __v); \
} جب (0)

#تعریف_کرو imsic_csr_read(__c)	\
({ \
	غیرعلامتی طویل __v; \
	csr_write(CSR_MISELECT, __c); \
	__v = csr_read(CSR_MIREG); \
	__v; \
})

#تعریف_کرو imsic_csr_سیٹ(__c, __v)		\
کرو { \
	csr_write(CSR_MISELECT, __c); \
	csr_سیٹ(CSR_MIREG, __v); \
} جب (0)

#تعریف_کرو imsic_csr_clear(__c, __v)	\
کرو { \
	csr_write(CSR_MISELECT, __c); \
	csr_clear(CSR_MIREG, __v); \
} جب (0)

ساکن غیرعلامتی طویل imsic_ptr_انحراف;

#تعریف_کرو imsic_get_hart_ڈیٹا_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), خالی *, imsic_ptr_انحراف)

#تعریف_کرو imsic_سیٹ_hart_ڈیٹا_ptr(__scratch, __imsic)			\
	sbi_scratch_write_type((__scratch), خالی *, imsic_ptr_انحراف, (__imsic))

ساکن غیرعلامتی طویل imsic_file_انحراف;

#تعریف_کرو imsic_get_hart_file(__scratch)					\
	sbi_scratch_read_type((__scratch), طویل, imsic_file_انحراف)

#تعریف_کرو imsic_سیٹ_hart_file(__scratch, __file)				\
	sbi_scratch_write_type((__scratch), طویل, imsic_file_انحراف, (__file))

صحیح imsic_map_hartid_مقصد_ڈیٹا(u32 hartid, ساخت imsic_ڈیٹا *imsic, صحیح file)
{
	ساخت sbi_scratch *scratch;

	اگر (!imsic || !imsic->targets_mmode)
		واپس SBI_EINVAL;

	/*
	 * We don't need to fail if scratch pointer is not available
	 * because we might be dealing with hartid of a HART disabled
	 * in device tree. For HARTs disabled in device tree, the
	 * imsic_get_data() and imsic_get_target_file() will anyway
	 * fail.
	 */
	scratch = sbi_hartid_مقصد_scratch(hartid);
	اگر (!scratch)
		واپس 0;

	imsic_سیٹ_hart_ڈیٹا_ptr(scratch, imsic);
	imsic_سیٹ_hart_file(scratch, file);
	واپس 0;
}

ساخت imsic_ڈیٹا *imsic_get_ڈیٹا(u32 hartindex)
{
	ساخت sbi_scratch *scratch;

	اگر (!imsic_ptr_انحراف)
		واپس تہی;

	scratch = sbi_hartindex_مقصد_scratch(hartindex);
	اگر (!scratch)
		واپس تہی;

	واپس imsic_get_hart_ڈیٹا_ptr(scratch);
}

صحیح imsic_get_ہدف_file(u32 hartindex)
{
	ساخت sbi_scratch *scratch;

	اگر (!imsic_file_انحراف)
		واپس SBI_ENOENT;

	scratch = sbi_hartindex_مقصد_scratch(hartindex);
	اگر (!scratch)
		واپس SBI_ENOENT;

	واپس imsic_get_hart_file(scratch);
}

ساکن صحیح imsic_process_hwirqs(ساخت sbi_irqchip_device *chip)
{
	ساخت imsic_ڈیٹا *imsic;
	ulong mirq;
	u32 hwirq;
	صحیح rc, ret = 0;

	imsic = container_کا(chip, ساخت imsic_ڈیٹا, irqchip);
	اگر (!imsic || !imsic->targets_mmode)
		واپس SBI_EINVAL;

	اگر (imsic_get_ڈیٹا(current_hartindex()) != imsic)
		واپس 0;

	جب ((mirq = csr_تبادلہ(CSR_MTOPEI, 0))) {
		hwirq = (mirq >> IMSIC_TOPEI_ID_سرکاؤ) &
			IMSIC_TOPEI_ID_نقاب;

		اگر (hwirq > imsic->عدد_ids) {
			sbi_printf("%s: invalid hwirq=%lu num_ids=%lu\n",
					__func__,
					(غیرعلامتی طویل)hwirq,
					(غیرعلامتی طویل)imsic->عدد_ids);
			جاری;
		}

		اگر (hwirq == IMSIC_IPI_ID) {
			sbi_ipi_process();
			جاری;
		}

		rc = sbi_irqchip_process_hwirq(chip, hwirq);
		اگر (rc && rc != SBI_ENOENT) {
			sbi_printf("%s: hwirq=%lu failed rc=%d\n",
					__func__,
					(غیرعلامتی طویل)hwirq, rc);
			ret = rc;
		}
	}

	واپس ret;
}

ساکن خالی imsic_ipi_send(u32 hart_اشاریہ)
{
	غیرعلامتی طویل reloff;
	ساخت imsic_regs *regs;
	ساخت imsic_ڈیٹا *ڈیٹا;
	ساخت sbi_scratch *scratch;
	صحیح file;

	scratch = sbi_hartindex_مقصد_scratch(hart_اشاریہ);
	اگر (!scratch)
		واپس;

	ڈیٹا = imsic_get_hart_ڈیٹا_ptr(scratch);
	file = imsic_get_hart_file(scratch);
	اگر (!ڈیٹا || !ڈیٹا->targets_mmode)
		واپس;

	regs = &ڈیٹا->regs[0];
	reloff = file * (1UL << ڈیٹا->guest_اشاریہ_بٹ) * IMSIC_MMIO_صفحہ_SZ;
	جب (regs->سائز && (regs->سائز <= reloff)) {
		reloff -= regs->سائز;
		regs++;
	}

	اگر (regs->سائز && (reloff < regs->سائز))
		writel_relaxed(IMSIC_IPI_ID,
			(خالی *)(regs->addr + reloff + IMSIC_MMIO_صفحہ_LE));
}

ساکن ساخت sbi_ipi_device imsic_ipi_device = {
	.name		= "aia-imsic",
	.rating		= 300,
	.ipi_send	= imsic_ipi_send
};

ساکن خالی imsic_local_eix_update(غیرعلامتی طویل id,
				   bool pend, bool val)
{
	غیرعلامتی طویل isel, ireg = 0;

	isel = id / __riscv_xlen;
	isel *= __riscv_xlen / IMSIC_EIPx_بٹ;
	isel += (pend) ? IMSIC_EIP0 : IMSIC_EIE0;
	ireg |= ثنائی_ہندسہ(id);

	اگر (val)
		imsic_csr_سیٹ(isel, ireg);
	ورنہ
		imsic_csr_clear(isel, ireg);
}

خالی imsic_local_irqchip_init(خالی)
{
	ساخت sbi_trap_info trap = { 0 };

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
		واپس;

	/* Setup threshold to allow all enabled interrupts */
	imsic_csr_write(IMSIC_EITHRESHOLD, IMSIC_ENABLE_EITHRESHOLD);

	/* Enable interrupt delivery */
	imsic_csr_write(IMSIC_EIDELIVERY, IMSIC_ENABLE_EIDELIVERY);

	/* Enable IPI */
	imsic_local_eix_update(IMSIC_IPI_ID, false, true);
}

ساکن صحیح imsic_warm_irqchip_init(ساخت sbi_irqchip_device *dev)
{
	ساخت imsic_ڈیٹا *imsic;
	ساخت imsic_ڈیٹا *hart_imsic;
	صحیح i;

	imsic = container_کا(dev, ساخت imsic_ڈیٹا, irqchip);
	hart_imsic = imsic_get_ڈیٹا(current_hartindex());

	/* Sanity checks */
	اگر (!hart_imsic || hart_imsic != imsic ||
		!hart_imsic->targets_mmode)
		واپس SBI_EINVAL;

	/* enable interrutps based on the irq state */
	برائے (i = 1; i < imsic->عدد_ids; i++) {
		اگر (sbi_irqchip_is_hwirq_enabled(&imsic->irqchip, i) == true)
			imsic_local_eix_update(i, true, true);
		ورنہ
			imsic_local_eix_update(i, false, false);
	}

	/* Clear IPI pending */
	imsic_local_eix_update(IMSIC_IPI_ID, true, false);

	/* Local IMSIC initialization */
	imsic_local_irqchip_init();

	واپس 0;
}

صحیح imsic_ڈیٹا_check(ساخت imsic_ڈیٹا *imsic)
{
	u32 i, tmp;
	غیرعلامتی طویل بنیاد_addr, addr, نقاب;

	/* Sanity checks */
	اگر (!imsic ||
	    (imsic->عدد_ids < IMSIC_MIN_ID) ||
	    (IMSIC_MAX_ID < imsic->عدد_ids))
		واپس SBI_EINVAL;

	tmp = بٹ_فی_LONG - IMSIC_MMIO_صفحہ_سرکاؤ;
	اگر (tmp < imsic->guest_اشاریہ_بٹ)
		واپس SBI_EINVAL;

	tmp = بٹ_فی_LONG - IMSIC_MMIO_صفحہ_سرکاؤ -
	      imsic->guest_اشاریہ_بٹ;
	اگر (tmp < imsic->hart_اشاریہ_بٹ)
		واپس SBI_EINVAL;

	tmp = بٹ_فی_LONG - IMSIC_MMIO_صفحہ_سرکاؤ -
	      imsic->guest_اشاریہ_بٹ - imsic->hart_اشاریہ_بٹ;
	اگر (tmp < imsic->گروہ_اشاریہ_بٹ)
		واپس SBI_EINVAL;

	tmp = IMSIC_MMIO_صفحہ_سرکاؤ + imsic->guest_اشاریہ_بٹ +
	      imsic->hart_اشاریہ_بٹ;
	اگر (imsic->گروہ_اشاریہ_سرکاؤ < tmp)
		واپس SBI_EINVAL;
	tmp = imsic->گروہ_اشاریہ_بٹ + imsic->گروہ_اشاریہ_سرکاؤ - 1;
	اگر (tmp >= بٹ_فی_LONG)
		واپس SBI_EINVAL;

	/*
	 * Number of interrupt identities should be 1 less than
	 * multiple of 63
	 */
	اگر ((imsic->عدد_ids & IMSIC_MIN_ID) != IMSIC_MIN_ID)
		واپس SBI_EINVAL;

	/* We should have at least one regset */
	اگر (!imsic->regs[0].سائز)
		واپس SBI_EINVAL;

	/* Match patter of each regset */
	بنیاد_addr = imsic->regs[0].addr;
	بنیاد_addr &= ~((1UL << (imsic->guest_اشاریہ_بٹ +
				 imsic->hart_اشاریہ_بٹ +
				 IMSIC_MMIO_صفحہ_سرکاؤ)) - 1);
	بنیاد_addr &= ~(((1UL << imsic->گروہ_اشاریہ_بٹ) - 1) <<
			imsic->گروہ_اشاریہ_سرکاؤ);
	برائے (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].سائز; i++) {
		نقاب = (1UL << imsic->guest_اشاریہ_بٹ) * IMSIC_MMIO_صفحہ_SZ;
		نقاب -= 1UL;
		اگر (imsic->regs[i].سائز & نقاب)
			واپس SBI_EINVAL;

		addr = imsic->regs[i].addr;
		addr &= ~((1UL << (imsic->guest_اشاریہ_بٹ +
					 imsic->hart_اشاریہ_بٹ +
					 IMSIC_MMIO_صفحہ_سرکاؤ)) - 1);
		addr &= ~(((1UL << imsic->گروہ_اشاریہ_بٹ) - 1) <<
				imsic->گروہ_اشاریہ_سرکاؤ);
		اگر (بنیاد_addr != addr)
			واپس SBI_EINVAL;
	}

	واپس 0;
}

ساکن صحیح imsic_hwirq_تیاری(ساخت sbi_irqchip_device *chip, u32 hwirq, u32 hwirq_flags)
{
	اگر (hwirq_flags != SBI_HWIRQ_FLAGS_NONE)
		واپس SBI_ENOTSUPP;

	sbi_irqchip_سیٹ_raw_handler(chip, hwirq,
				    sbi_irqchip_raw_handler_default);

	واپس 0;
}

ساکن خالی imsic_hwirq_cleanup(ساخت sbi_irqchip_device *chip, u32 hwirq)
{
	ساخت imsic_ڈیٹا *imsic;

	اگر (!chip)
		واپس;

	imsic = container_کا(chip, ساخت imsic_ڈیٹا, irqchip);
	اگر (!imsic || !imsic->targets_mmode)
		واپس;

	imsic_local_eix_update(hwirq, false, false);
	imsic_local_eix_update(hwirq, true, false);
}

ساکن خالی imsic_hwirq_eoi(ساخت sbi_irqchip_device *chip, u32 hwirq)
{
	/*
	 * IMSIC interrupt claim/ack is already done by reading CSR_MTOPEI
	 * in imsic_process_hwirqs(). No extra EOI operation is required.
	 */
}

ساکن صحیح imsic_compose_msi_پیغام(ساخت imsic_ڈیٹا *imsic,
				 u32 hart_اشاریہ, u32 eiid,
				 ساخت sbi_irqchip_msi_پیغام *پیغام)
{
	ساخت imsic_regs *regs;
	غیرعلامتی طویل reloff;
	u64 msi_addr;
	صحیح file;

	اگر (!imsic || !پیغام)
		واپس SBI_EINVAL;

	اگر (!eiid || eiid == IMSIC_IPI_ID)
		واپس 0;

	file = imsic_get_ہدف_file(hart_اشاریہ);
	اگر (file < 0) {
		sbi_printf("%s: no file for hart=%lu rc=%d\n",
				__func__,
				(غیرعلامتی طویل)hart_اشاریہ, file);
		واپس file;
	}

	regs   = &imsic->regs[0];
	reloff = file * (1UL << imsic->guest_اشاریہ_بٹ) * IMSIC_MMIO_صفحہ_SZ;

	جب (regs->سائز && regs->سائز <= reloff) {
		reloff -= regs->سائز;
		regs++;
	}

	اگر (!regs->سائز || regs->سائز <= reloff) {
		sbi_printf("%s: no regset for hart=%lu file=%d reloff=0x%lx\n",
			   __func__,
			   (غیرعلامتی طویل)hart_اشاریہ, file, reloff);
		واپس SBI_ENODEV;
	}

	msi_addr = (u64)regs->addr + reloff + IMSIC_MMIO_صفحہ_LE;
	پیغام->پتہ_lo = (u32)(msi_addr);
	پیغام->پتہ_hi = (u32)(msi_addr >> 32);
	پیغام->ڈیٹا       = eiid;

	واپس 0;
}

ساکن صحیح imsic_پروگرام_msi(ساخت sbi_irqchip_device *chip,
		      u32 eiid, u32 hart_اشاریہ)
{
	ساخت imsic_ڈیٹا *imsic;
	ساخت sbi_irqchip_msi_پیغام پیغام;
	صحیح rc;

	اگر (!chip)
		واپس SBI_EINVAL;

	imsic = imsic_get_ڈیٹا(hart_اشاریہ);
	اگر (!imsic || !imsic->targets_mmode)
		واپس SBI_ENODEV;

	rc = imsic_compose_msi_پیغام(imsic, hart_اشاریہ, eiid, &پیغام);
	اگر (rc) {
		sbi_printf("%s: compose failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(غیرعلامتی طویل)eiid,
				(غیرعلامتی طویل)hart_اشاریہ, rc);
	}

	rc = sbi_irqchip_write_msi(chip, eiid, &پیغام);
	اگر (rc) {
		sbi_printf("%s: write_msi failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(غیرعلامتی طویل)eiid,
				(غیرعلامتی طویل)hart_اشاریہ, rc);
		واپس rc;
	}

	واپس 0;
}

ساکن صحیح imsic_hwirq_سیٹ_affinity(ساخت sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_اشاریہ)
{
	ساخت imsic_ڈیٹا *imsic;
	صحیح rc;

	اگر (!chip)
		واپس SBI_EINVAL;

	imsic = container_کا(chip, ساخت imsic_ڈیٹا, irqchip);
	اگر (!imsic || !imsic->targets_mmode)
		واپس SBI_EINVAL;

	اگر (!hwirq || hwirq == IMSIC_IPI_ID)
		واپس 0;

	اگر (!sbi_hartmask_test_hartindex(hart_اشاریہ, &chip->ہدف_harts))
		واپس SBI_EINVAL;

	rc = imsic_پروگرام_msi(chip, hwirq, hart_اشاریہ);
	اگر (rc) {
		sbi_printf("%s: failed hwirq=%lu hart=%lu rc=%d\n",
				__func__,
				(غیرعلامتی طویل)hwirq,
				(غیرعلامتی طویل)hart_اشاریہ, rc);
		واپس rc;
	}

	واپس 0;
}

ساکن خالی imsic_hwirq_نقاب(ساخت sbi_irqchip_device *chip, u32 hwirq)
{
	ساخت imsic_ڈیٹا *imsic;

	اگر (!chip)
		واپس;

	imsic = container_کا(chip, ساخت imsic_ڈیٹا, irqchip);
	اگر (!imsic || !imsic->targets_mmode)
		واپس;

	imsic_local_eix_update(hwirq, false, false);
}

ساکن خالی imsic_hwirq_unmask(ساخت sbi_irqchip_device *chip, u32 hwirq)
{
	ساخت imsic_ڈیٹا *imsic;

	اگر (!chip)
		واپس;

	imsic = container_کا(chip, ساخت imsic_ڈیٹا, irqchip);
	اگر (!imsic || !imsic->targets_mmode)
		واپس;

	اگر (!hwirq || hwirq == IMSIC_IPI_ID)
		واپس;

	imsic_local_eix_update(hwirq, false, true);
}

ساکن ساخت sbi_irqchip_device imsic_device = {
	.warm_init		= imsic_warm_irqchip_init,
	.process_hwirqs		= imsic_process_hwirqs,
	.hwirq_تیاری		= imsic_hwirq_تیاری,
	.hwirq_cleanup		= imsic_hwirq_cleanup,
	.hwirq_eoi		= imsic_hwirq_eoi,
	.hwirq_سیٹ_affinity	= imsic_hwirq_سیٹ_affinity,
	.hwirq_نقاب		= imsic_hwirq_نقاب,
	.hwirq_unmask		= imsic_hwirq_unmask,
};

صحیح imsic_cold_irqchip_init(ساخت imsic_ڈیٹا *imsic)
{
	صحیح i, rc;

	/* Sanity checks */
	rc = imsic_ڈیٹا_check(imsic);
	اگر (rc)
		واپس rc;

	/* We only initialize M-mode IMSIC */
	اگر (!imsic->targets_mmode)
		واپس SBI_EINVAL;

	/* Allocate scratch space pointer */
	اگر (!imsic_ptr_انحراف) {
		imsic_ptr_انحراف = sbi_scratch_alloc_type_انحراف(خالی *);
		اگر (!imsic_ptr_انحراف)
			واپس SBI_ENOMEM;
	}

	/* Allocate scratch space file */
	اگر (!imsic_file_انحراف) {
		imsic_file_انحراف = sbi_scratch_alloc_type_انحراف(طویل);
		اگر (!imsic_file_انحراف)
			واپس SBI_ENOMEM;
	}

	/* Add IMSIC regions to the root domain */
	برائے (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].سائز; i++) {
		rc = sbi_domain_جڑ_جوڑو_memrange(imsic->regs[i].addr,
						  imsic->regs[i].سائز,
						  IMSIC_MMIO_صفحہ_SZ,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		اگر (rc)
			واپس rc;
	}

	imsic->irqchip          = imsic_device;
	imsic->irqchip.id       = imsic->unique_id;
	imsic_device.caps = SBI_IRQCHIP_CAPS_MSI;
	imsic->irqchip.عدد_hwirq = imsic->عدد_ids + 1;

	sbi_hartmask_سیٹ_all(&imsic->irqchip.ہدف_harts);

	/* Register irqchip device */
	rc = sbi_irqchip_جوڑو_device(&imsic->irqchip);
	اگر (rc) {
		sbi_printf("%s: sbi_irqchip_add_device failed rc=%d\n",
				__func__, rc);
		واپس rc;
	}

	rc = sbi_irqchip_رجسٹر_reserved(&imsic->irqchip, 0, IMSIC_IPI_ID + 1);
	اگر (rc) {
		sbi_printf("%s: register_reserved failed rc=%d\n",
				__func__, rc);
		واپس rc;
	}

	/* Register IPI device */
	sbi_ipi_جوڑو_device(&imsic_ipi_device);

	واپس 0;
}
