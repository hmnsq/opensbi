/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi_utils/irqchip/aplic.h>

#وصف_ڪريو APLIC_MAX_IDC			(1UL << 14)
#وصف_ڪريو APLIC_MAX_ذريعو		1024

#وصف_ڪريو APLIC_DOMAINCFG		0x0000
#وصف_ڪريو APLIC_DOMAINCFG_IE		(1 << 8)
#وصف_ڪريو APLIC_DOMAINCFG_DM		(1 << 2)
#وصف_ڪريو APLIC_DOMAINCFG_BE		(1 << 0)

#وصف_ڪريو APLIC_SOURCECFG_بنياد		0x0004
#وصف_ڪريو APLIC_SOURCECFG_D		(1 << 10)
#وصف_ڪريو APLIC_SOURCECFG_CHILDIDX_نقاب	0x000003ff
#وصف_ڪريو APLIC_SOURCECFG_SM_نقاب	0x00000007
#وصف_ڪريو APLIC_SOURCECFG_SM_INACTIVE	0x0
#وصف_ڪريو APLIC_SOURCECFG_SM_DETACH	0x1
#وصف_ڪريو APLIC_SOURCECFG_SM_ڪنارو_RISE	0x4
#وصف_ڪريو APLIC_SOURCECFG_SM_ڪنارو_FALL	0x5
#وصف_ڪريو APLIC_SOURCECFG_SM_LEVEL_مٿاهون	0x6
#وصف_ڪريو APLIC_SOURCECFG_SM_LEVEL_هيٺيون	0x7

#وصف_ڪريو APLIC_MMSICFGADDR		0x1bc0
#وصف_ڪريو APLIC_MMSICFGADDRH		0x1bc4
#وصف_ڪريو APLIC_SMSICFGADDR		0x1bc8
#وصف_ڪريو APLIC_SMSICFGADDRH		0x1bcc

#وصف_ڪريو APLIC_xMSICFGADDRH_L		(1UL << 31)
#وصف_ڪريو APLIC_x_MSICFGADDRH_HHXS_نقاب	0x1f
#وصف_ڪريو APLIC_x_MSICFGADDRH_HHXS_سرڪايو	24
#وصف_ڪريو APLIC_x_MSICFGADDRH_LHXS_نقاب	0x7
#وصف_ڪريو APLIC_x_MSICFGADDRH_LHXS_سرڪايو	20
#وصف_ڪريو APLIC_x_MSICFGADDRH_HHXW_نقاب	0x7
#وصف_ڪريو APLIC_x_MSICFGADDRH_HHXW_سرڪايو	16
#وصف_ڪريو APLIC_x_MSICFGADDRH_LHXW_نقاب	0xf
#وصف_ڪريو APLIC_x_MSICFGADDRH_LHXW_سرڪايو	12
#وصف_ڪريو APLIC_x_MSICFGADDRH_BAPPN_نقاب	0xfff

#وصف_ڪريو APLIC_x_MSICFGADDR_PPN_سرڪايو	12

#وصف_ڪريو APLIC_xMSICFGADDR_PPN_HART(__lhxs) \
	((1UL << (__lhxs)) - 1)

#وصف_ڪريو APLIC_x_MSICFGADDR_PPN_LHX_نقاب(__lhxw) \
	((1UL << (__lhxw)) - 1)
#وصف_ڪريو APLIC_x_MSICFGADDR_PPN_LHX_سرڪايو(__lhxs) \
	((__lhxs))
#وصف_ڪريو APLIC_xMSICFGADDR_PPN_LHX(__lhxw, __lhxs) \
	(APLIC_x_MSICFGADDR_PPN_LHX_نقاب(__lhxw) << \
	 APLIC_x_MSICFGADDR_PPN_LHX_سرڪايو(__lhxs))

#وصف_ڪريو APLIC_x_MSICFGADDR_PPN_HHX_نقاب(__hhxw) \
	((1UL << (__hhxw)) - 1)
#وصف_ڪريو APLIC_x_MSICFGADDR_PPN_HHX_سرڪايو(__hhxs) \
	((__hhxs) + APLIC_x_MSICFGADDR_PPN_سرڪايو)
#وصف_ڪريو APLIC_xMSICFGADDR_PPN_HHX(__hhxw, __hhxs) \
	(APLIC_x_MSICFGADDR_PPN_HHX_نقاب(__hhxw) << \
	 APLIC_x_MSICFGADDR_PPN_HHX_سرڪايو(__hhxs))

#وصف_ڪريو APLIC_هدف_GUEST_IDX(__gidx) \
	((((u32)(__gidx)) & APLIC_هدف_GUEST_IDX_نقاب) << \
	 APLIC_هدف_GUEST_IDX_سرڪايو)
#وصف_ڪريو APLIC_هدف_EIID(__eiid) \
	(((u32)(__eiid)) & APLIC_هدف_EIID_نقاب)

#وصف_ڪريو APLIC_SETIP_بنياد		0x1c00
#وصف_ڪريو APLIC_SETIPNUM			0x1cdc

#وصف_ڪريو APLIC_CLRIP_بنياد		0x1d00
#وصف_ڪريو APLIC_CLRIPNUM			0x1ddc

#وصف_ڪريو APLIC_SETIE_بنياد		0x1e00
#وصف_ڪريو APLIC_SETIENUM			0x1edc

#وصف_ڪريو APLIC_CLRIE_بنياد		0x1f00
#وصف_ڪريو APLIC_CLRIENUM			0x1fdc

#وصف_ڪريو APLIC_SETIPNUM_LE		0x2000
#وصف_ڪريو APLIC_SETIPNUM_BE		0x2004

#وصف_ڪريو APLIC_هدف_بنياد		0x3004
#وصف_ڪريو APLIC_هدف_HART_IDX_سرڪايو	18
#وصف_ڪريو APLIC_هدف_HART_IDX_نقاب	0x3fff
#وصف_ڪريو APLIC_هدف_GUEST_IDX_سرڪايو	12
#وصف_ڪريو APLIC_هدف_GUEST_IDX_نقاب	0x3f
#وصف_ڪريو APLIC_هدف_IPRIO_نقاب	0xff
#وصف_ڪريو APLIC_هدف_EIID_نقاب	0x7ff

#وصف_ڪريو APLIC_هدف_HART_IDX(__hidx) \
	((((u32)(__hidx)) & APLIC_هدف_HART_IDX_نقاب) << \
	 APLIC_هدف_HART_IDX_سرڪايو)
#وصف_ڪريو APLIC_هدف_IPRIO(__prio) \
	(((u32)(__prio)) & APLIC_هدف_IPRIO_نقاب)

#وصف_ڪريو APLIC_IDC_بنياد			0x4000
#وصف_ڪريو APLIC_IDC_سائيز			32

#وصف_ڪريو APLIC_IDC_IDELIVERY		0x00

#وصف_ڪريو APLIC_IDC_IFORCE		0x04

#وصف_ڪريو APLIC_IDC_ITHRESHOLD		0x08

#وصف_ڪريو APLIC_IDC_TOPI			0x18
#وصف_ڪريو APLIC_IDC_TOPI_ID_سرڪايو	16
#وصف_ڪريو APLIC_IDC_TOPI_ID_نقاب	0x3ff
#وصف_ڪريو APLIC_IDC_TOPI_PRIO_نقاب	0xff

#وصف_ڪريو APLIC_IDC_CLAIMI		0x1c

#وصف_ڪريو APLIC_DEFAULT_ترجيح		1
#وصف_ڪريو APLIC_DISABLE_IDELIVERY		0
#وصف_ڪريو APLIC_ENABLE_IDELIVERY		1
#وصف_ڪريو APLIC_DISABLE_ITHRESHOLD	1
#وصف_ڪريو APLIC_ENABLE_ITHRESHOLD		0

بنية aplic_msi_معلومات {
	بنية aplic_معلومات *aplic;
	u32 hwirq;
	u32 والد_hwirq;
};

ساكن SBI_LIST_سر(aplic_list);
ساكن فراغ aplic_writel_msicfg(بنية aplic_msicfg_معلومات *msicfg,
				فراغ *msicfgaddr, فراغ *msicfgaddrH);

ساكن مضمن bool aplic_is_سڌو_گهڻائي(بنية aplic_معلومات *aplic)
{
	أرجع aplic && aplic->انگ_idc;
}

ساكن مضمن فراغ aplic_sourcecfg_write(بنية aplic_معلومات *aplic,
					 u32 hwirq, u32 val)
{
	writel(val, (فراغ *)(aplic->addr + APLIC_SOURCECFG_بنياد +
			     (hwirq - 1) * حجم(u32)));
}

ساكن مضمن u32 aplic_sourcecfg_read(بنية aplic_معلومات *aplic, u32 hwirq)
{
	أرجع readl((فراغ *)(aplic->addr + APLIC_SOURCECFG_بنياد +
			      (hwirq - 1) * حجم(u32)));
}

ساكن مضمن فراغ aplic_هدف_write(بنية aplic_معلومات *aplic,
				      u32 hwirq, u32 val)
{
	writel(val, (فراغ *)(aplic->addr + APLIC_هدف_بنياد +
			     (hwirq - 1) * حجم(u32)));
}

ساكن مضمن فراغ aplic_irq_setie(بنية aplic_معلومات *aplic, u32 hwirq)
{
	writel(hwirq, (فراغ *)(aplic->addr + APLIC_SETIENUM));
}

ساكن مضمن فراغ aplic_irq_clrie(بنية aplic_معلومات *aplic, u32 hwirq)
{
	writel(hwirq, (فراغ *)(aplic->addr + APLIC_CLRIENUM));
}

ساكن مضمن فراغ aplic_irq_clrip(بنية aplic_معلومات *aplic, u32 hwirq)
{
	writel(hwirq, (فراغ *)(aplic->addr + APLIC_CLRIPNUM));
}

ساكن مضمن فراغ aplic_msi_irq_retrigger_level(بنية aplic_معلومات *aplic, u32 hwirq)
{
	/*
	 * The section "4.9.2 Special consideration for level-sensitive interrupt
	 * sources" of the RISC-V AIA specification says:
	 *
	 * A second option is for the interrupt service routine to write the
	 * APLIC’s source identity number for the interrupt to the domain’s
	 * setipnum register just before exiting. This will cause the interrupt’s
	 * pending bit to be set to one again if the source is still asserting
	 * an interrupt, but not if the source is not asserting an interrupt.
	 */
	writel(hwirq, (فراغ *)(aplic->addr + APLIC_SETIPNUM_LE));
}

ساكن مضمن فراغ *aplic_idc_بنياد(بنية aplic_معلومات *aplic, u32 idc_اشاريو)
{
	أرجع (فراغ *)(aplic->addr + APLIC_IDC_بنياد +
			idc_اشاريو * APLIC_IDC_سائيز);
}

ساكن مضمن u32 aplic_idc_read(بنية aplic_معلومات *aplic, u32 idc_اشاريو,
				 u32 reg)
{
	أرجع readl(aplic_idc_بنياد(aplic, idc_اشاريو) + reg);
}

ساكن مضمن فراغ aplic_idc_write(بنية aplic_معلومات *aplic, u32 idc_اشاريو,
				   u32 reg, u32 val)
{
	writel(val, aplic_idc_بنياد(aplic, idc_اشاريو) + reg);
}

ساكن u32 aplic_hwirq_flags_منزل_sourcecfg(u32 hwirq_flags)
{
	بدل (hwirq_flags & SBI_HWIRQ_FLAGS_LEVEL_SENSE_نقاب) {
	حالة SBI_HWIRQ_FLAGS_ڪنارو_RISING:
		أرجع APLIC_SOURCECFG_SM_ڪنارو_RISE;
	حالة SBI_HWIRQ_FLAGS_ڪنارو_FALLING:
		أرجع APLIC_SOURCECFG_SM_ڪنارو_FALL;
	حالة SBI_HWIRQ_FLAGS_LEVEL_مٿاهون:
		أرجع APLIC_SOURCECFG_SM_LEVEL_مٿاهون;
	حالة SBI_HWIRQ_FLAGS_LEVEL_هيٺيون:
		أرجع APLIC_SOURCECFG_SM_LEVEL_هيٺيون;
	افتراضي:
		أرجع APLIC_SOURCECFG_SM_INACTIVE;
	}
}

ساكن bool aplic_hwirq_is_delegated(بنية aplic_معلومات *aplic, u32 hwirq)
{
	u32 sourcecfg;

	sourcecfg = aplic_sourcecfg_read(aplic, hwirq);
	أرجع !!(sourcecfg & APLIC_SOURCECFG_D);
}

ساكن صحيح aplic_ڳوليو_idc_اشاريو(بنية aplic_معلومات *aplic, u32 hart_اشاريو)
{
	u32 i;

	لكل (i = 0; i < aplic->انگ_idc; i++) {
		إذا (aplic->idc_map[i] == hart_اشاريو)
			أرجع i;
	}

	أرجع SBI_ENOENT;
}

ساكن فراغ aplic_init(بنية aplic_معلومات *aplic)
{
	بنية aplic_delegate_معلومات *deleg;
	u32 i, j, tmp, domaincfg;
	صحيح locked;

	/* Set domain configuration to 0 */
	writel(0, (فراغ *)(aplic->addr + APLIC_DOMAINCFG));

	/* Disable all interrupts */
	لكل (i = 0; i <= aplic->انگ_ذريعو; i += 32)
		writel(-1U, (فراغ *)(aplic->addr + APLIC_CLRIE_بنياد +
				     (i / 32) * حجم(u32)));

	/* Set interrupt type and priority for all interrupts */
	لكل (i = 1; i <= aplic->انگ_ذريعو; i++) {
		/* Set IRQ source configuration to 0 */
		writel(0, (فراغ *)(aplic->addr + APLIC_SOURCECFG_بنياد +
			  (i - 1) * حجم(u32)));
		/* Set IRQ target hart index and priority to 1 */
		writel(APLIC_DEFAULT_ترجيح, (فراغ *)(aplic->addr +
						APLIC_هدف_بنياد +
						(i - 1) * حجم(u32)));
	}

	/* Configure IRQ delegation */
	لكل (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		إذا (!deleg->پهريون_irq || !deleg->آخري_irq)
			استمر;
		إذا (aplic->انگ_ذريعو < deleg->پهريون_irq ||
		    aplic->انگ_ذريعو < deleg->آخري_irq)
			استمر;
		إذا (deleg->ٻار_اشاريو > APLIC_SOURCECFG_CHILDIDX_نقاب)
			استمر;
		إذا (deleg->پهريون_irq > deleg->آخري_irq) {
			tmp = deleg->پهريون_irq;
			deleg->پهريون_irq = deleg->آخري_irq;
			deleg->آخري_irq = tmp;
		}
		لكل (j = deleg->پهريون_irq; j <= deleg->آخري_irq; j++)
			aplic_sourcecfg_write(aplic, j, APLIC_SOURCECFG_D | deleg->ٻار_اشاريو);
	}

	/* Default initialization of IDC structures */
	إذا (aplic_is_سڌو_گهڻائي(aplic)) {
		لكل (i = 0; i < aplic->انگ_idc; i++) {
			aplic_idc_write(aplic, i, APLIC_IDC_IDELIVERY,
					APLIC_DISABLE_IDELIVERY);
			aplic_idc_write(aplic, i, APLIC_IDC_IFORCE, 0);
			aplic_idc_write(aplic, i, APLIC_IDC_ITHRESHOLD,
					APLIC_DISABLE_ITHRESHOLD);
		}
	} وإلا {
		/* MSI configuration */
		locked = readl((فراغ *)(aplic->addr + APLIC_MMSICFGADDRH)) & APLIC_xMSICFGADDRH_L;
		إذا (aplic->targets_mmode && aplic->has_msicfg_mmode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_mmode,
						(فراغ *)(aplic->addr + APLIC_MMSICFGADDR),
						(فراغ *)(aplic->addr + APLIC_MMSICFGADDRH));
		}
		إذا (aplic->targets_mmode && aplic->has_msicfg_smode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_smode,
						(فراغ *)(aplic->addr + APLIC_SMSICFGADDR),
						(فراغ *)(aplic->addr + APLIC_SMSICFGADDRH));
		}
	}

	domaincfg = APLIC_DOMAINCFG_IE;
	إذا (!aplic_is_سڌو_گهڻائي(aplic))
		domaincfg |= APLIC_DOMAINCFG_DM;

	writel(domaincfg, (فراغ *)(aplic->addr + APLIC_DOMAINCFG));
}

فراغ aplic_reinit_all(فراغ)
{
	بنية aplic_معلومات *aplic;

	sbi_list_for_each_داخلا(aplic, &aplic_list, ڳنڍ)
		aplic_init(aplic);
}

ساكن فراغ aplic_writel_msicfg(بنية aplic_msicfg_معلومات *msicfg,
				فراغ *msicfgaddr, فراغ *msicfgaddrH)
{
	u32 val;
	بنانشان ڊگهو بنياد_ppn;

	/* Compute the MSI base PPN */
	بنياد_ppn = msicfg->بنياد_addr >> APLIC_x_MSICFGADDR_PPN_سرڪايو;
	بنياد_ppn &= ~APLIC_xMSICFGADDR_PPN_HART(msicfg->lhxs);
	بنياد_ppn &= ~APLIC_xMSICFGADDR_PPN_LHX(msicfg->lhxw, msicfg->lhxs);
	بنياد_ppn &= ~APLIC_xMSICFGADDR_PPN_HHX(msicfg->hhxw, msicfg->hhxs);

	/* Write the lower MSI config register */
	writel((u32)بنياد_ppn, msicfgaddr);

	/* Write the upper MSI config register */
	val = (((u64)بنياد_ppn) >> 32) &
		APLIC_x_MSICFGADDRH_BAPPN_نقاب;
	val |= (msicfg->lhxw & APLIC_x_MSICFGADDRH_LHXW_نقاب)
		<< APLIC_x_MSICFGADDRH_LHXW_سرڪايو;
	val |= (msicfg->hhxw & APLIC_x_MSICFGADDRH_HHXW_نقاب)
		<< APLIC_x_MSICFGADDRH_HHXW_سرڪايو;
	val |= (msicfg->lhxs & APLIC_x_MSICFGADDRH_LHXS_نقاب)
		<< APLIC_x_MSICFGADDRH_LHXS_سرڪايو;
	val |= (msicfg->hhxs & APLIC_x_MSICFGADDRH_HHXS_نقاب)
		<< APLIC_x_MSICFGADDRH_HHXS_سرڪايو;
	writel(val, msicfgaddrH);
}

ساكن صحيح aplic_check_msicfg(بنية aplic_msicfg_معلومات *msicfg)
{
	إذا (APLIC_x_MSICFGADDRH_LHXS_نقاب < msicfg->lhxs)
		أرجع SBI_EINVAL;

	إذا (APLIC_x_MSICFGADDRH_LHXW_نقاب < msicfg->lhxw)
		أرجع SBI_EINVAL;

	إذا (APLIC_x_MSICFGADDRH_HHXS_نقاب < msicfg->hhxs)
		أرجع SBI_EINVAL;

	إذا (APLIC_x_MSICFGADDRH_HHXW_نقاب < msicfg->hhxw)
		أرجع SBI_EINVAL;

	أرجع 0;
}

ساكن صحيح aplic_warm_init(بنية sbi_irqchip_device *chip)
{
	بنية aplic_معلومات *aplic;
	u32 hart_اشاريو;
	صحيح idc_اشاريو;

	aplic = container_جو(chip, بنية aplic_معلومات, irqchip);
	إذا (!aplic_is_سڌو_گهڻائي(aplic))
		أرجع 0;

	hart_اشاريو = current_hartindex();
	idc_اشاريو  = aplic_ڳوليو_idc_اشاريو(aplic, hart_اشاريو);
	إذا (idc_اشاريو < 0)
		أرجع 0;

	aplic_idc_write(aplic, idc_اشاريو, APLIC_IDC_ITHRESHOLD,
			APLIC_ENABLE_ITHRESHOLD);
	aplic_idc_write(aplic, idc_اشاريو, APLIC_IDC_IDELIVERY,
			APLIC_ENABLE_IDELIVERY);

	أرجع 0;
}

ساكن صحيح aplic_process_hwirqs(بنية sbi_irqchip_device *chip)
{
	بنية aplic_معلومات *aplic;
	u32 hart_اشاريو, claimi, hwirq;
	صحيح idc_اشاريو, rc = 0, tmp;

	aplic = container_جو(chip, بنية aplic_معلومات, irqchip);

	إذا (!aplic_is_سڌو_گهڻائي(aplic))
		sbi_panic("aplic_process_hwirqs called in MSI mode.\n");

	hart_اشاريو = current_hartindex();
	idc_اشاريو  = aplic_ڳوليو_idc_اشاريو(aplic, hart_اشاريو);
	إذا (idc_اشاريو < 0)
		أرجع 0;

	طالما ((claimi = aplic_idc_read(aplic, idc_اشاريو, APLIC_IDC_CLAIMI))) {
		hwirq = (claimi >> APLIC_IDC_TOPI_ID_سرڪايو) &
			APLIC_IDC_TOPI_ID_نقاب;

		إذا (!hwirq)
			اكسر;

		إذا (aplic_hwirq_is_delegated(aplic, hwirq))
			استمر;

		tmp = sbi_irqchip_process_hwirq(chip, hwirq);
		إذا (tmp)
			rc = tmp;
	}

	أرجع rc;
}

ساكن فراغ aplic_hwirq_eoi(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية aplic_معلومات *aplic;
	u32 sm;

	aplic = container_جو(chip, بنية aplic_معلومات, irqchip);
	إذا (aplic_is_سڌو_گهڻائي(aplic))
		أرجع;

	sm = aplic_sourcecfg_read(aplic, hwirq) & APLIC_SOURCECFG_SM_نقاب;
	إذا (sm == APLIC_SOURCECFG_SM_LEVEL_مٿاهون ||
	    sm == APLIC_SOURCECFG_SM_LEVEL_هيٺيون)
		aplic_msi_irq_retrigger_level(aplic, hwirq);
}

ساكن فراغ aplic_پروگرام_msi_هدف(بنية aplic_معلومات *aplic,
				     u32 hwirq, u32 hart_اشاريو,
				     u32 guest_اشاريو, u32 eiid)
{
	u32 هدف;

	هدف = APLIC_هدف_HART_IDX(hart_اشاريو) |
		 APLIC_هدف_GUEST_IDX(guest_اشاريو) |
		 APLIC_هدف_EIID(eiid);

	aplic_هدف_write(aplic, hwirq, هدف);
}

ساكن u32 derive_hart_اشاريو(بنية aplic_msicfg_معلومات *msicfg,
			      ثابت بنية sbi_irqchip_msi_نياپو *نياپو)
{
	u64 addr = ((u64)نياپو->پتو_hi << 32) | نياپو->پتو_lo;
	u64 tppn = addr >> APLIC_x_MSICFGADDR_PPN_سرڪايو;
	u32 گروهه_اشاريو, hart_اشاريو;

	گروهه_اشاريو = (tppn >> APLIC_x_MSICFGADDR_PPN_HHX_سرڪايو(msicfg->hhxs)) &
		      APLIC_x_MSICFGADDR_PPN_HHX_نقاب(msicfg->hhxw);

	hart_اشاريو = (tppn >> msicfg->lhxs) &
		     ((1UL << msicfg->lhxw) - 1);

	hart_اشاريو |= (گروهه_اشاريو << msicfg->lhxw);

	أرجع hart_اشاريو;
}

ساكن فراغ aplic_write_msi(u32 والد_hwirq,
			    ثابت بنية sbi_irqchip_msi_نياپو *نياپو,
			    فراغ *priv)
{
	بنية aplic_msi_معلومات *msi_معلومات = priv;
	u32 guest_اشاريو = 0;
	u32 eiid;

	eiid = نياپو->معلومات;
	إذا (!eiid || eiid > APLIC_هدف_EIID_نقاب)
		أرجع;

	aplic_پروگرام_msi_هدف(msi_معلومات->aplic, msi_معلومات->hwirq,
				derive_hart_اشاريو(&msi_معلومات->aplic->msicfg_mmode, نياپو),
				guest_اشاريو, eiid);
}

ساكن صحيح aplic_msi_callback(u32 والد_hwirq, فراغ *priv)
{
	بنية aplic_msi_معلومات *msi_معلومات = priv;
	صحيح rc;

	rc = sbi_irqchip_process_hwirq(&msi_معلومات->aplic->irqchip, msi_معلومات->hwirq);
	إذا (rc && rc != SBI_ENOENT)
		sbi_printf("%s: hwirq=%lu failed rc=%d\n",
			__func__,
			(بنانشان ڊگهو)والد_hwirq, rc);

	أرجع rc;
}

ساكن صحيح aplic_تياري_msi(بنية aplic_معلومات *aplic, u32 hwirq)
{
	بنية sbi_irqchip_device *والد;
	u32 پهريون_hwirq;
	بنية aplic_msi_معلومات *msi_معلومات;
	صحيح rc;

	والد = sbi_irqchip_ڳوليو_device(aplic->والد_unique_id);
	إذا (!والد) {
		sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
			__func__, (بنانشان ڊگهو)hwirq);
		أرجع SBI_EINVAL;
	}

	msi_معلومات = sbi_zalloc(حجم(بنية aplic_msi_معلومات));
	إذا (!msi_معلومات)
		أرجع SBI_ENOMEM;

	msi_معلومات->aplic = aplic;
	msi_معلومات->hwirq = hwirq;

	rc = sbi_irqchip_رجسٽر_msi(والد, 1,
					aplic_write_msi,
					aplic_msi_callback,
					msi_معلومات,
					&پهريون_hwirq);
	إذا (rc) {
		sbi_printf("%s: register_msi failed hwirq=%lu rc=%d\n",
			__func__, (بنانشان ڊگهو)hwirq, rc);
		sbi_free(msi_معلومات);
		أرجع rc;
	}

	msi_معلومات->والد_hwirq = پهريون_hwirq;
	sbi_irqchip_سيٽ_hwirq_priv(&aplic->irqchip, hwirq, msi_معلومات);

	أرجع 0;
}

ساكن صحيح aplic_hwirq_تياري(بنية sbi_irqchip_device *chip,
			     u32 hwirq, u32 hwirq_flags)
{
	بنية aplic_معلومات *aplic;
	u32 sourcecfg;

	aplic = container_جو(chip, بنية aplic_معلومات, irqchip);

	sourcecfg = aplic_hwirq_flags_منزل_sourcecfg(hwirq_flags);
	إذا (sourcecfg == APLIC_SOURCECFG_SM_INACTIVE) {
		sbi_printf("%s: unsupported flags=0x%lx hwirq=%lu\n",
			__func__,
			(بنانشان ڊگهو)hwirq_flags,
			(بنانشان ڊگهو)hwirq);
		أرجع SBI_EINVAL;
	}

	aplic_sourcecfg_write(aplic, hwirq, sourcecfg);

	إذا (aplic_hwirq_is_delegated(aplic, hwirq)) {
		sbi_printf("%s: hwirq=%lu is delegated\n",
			__func__, (بنانشان ڊگهو)hwirq);
		أرجع SBI_ENOTSUPP;
	}

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);

	إذا (!aplic_is_سڌو_گهڻائي(aplic))
		أرجع aplic_تياري_msi(aplic, hwirq);

	أرجع 0;
}

ساكن فراغ aplic_hwirq_cleanup(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية aplic_معلومات *aplic;
	بنية aplic_msi_معلومات *msi_معلومات;

	aplic = container_جو(chip, بنية aplic_معلومات, irqchip);
	إذا (aplic_hwirq_is_delegated(aplic, hwirq))
		أرجع;

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);
	aplic_sourcecfg_write(aplic, hwirq, APLIC_SOURCECFG_SM_INACTIVE);
	aplic_هدف_write(aplic, hwirq, APLIC_DEFAULT_ترجيح);
	msi_معلومات = (بنية aplic_msi_معلومات *) sbi_irqchip_get_hwirq_priv(chip, hwirq);
	إذا (msi_معلومات)
		sbi_free(msi_معلومات);
}

ساكن صحيح aplic_hwirq_سيٽ_affinity(بنية sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_اشاريو)
{
	بنية aplic_معلومات *aplic;
	بنية aplic_msi_معلومات *msi_معلومات;
	بنية sbi_irqchip_device *والد;
	صحيح idc_اشاريو;
	صحيح rc;

	aplic = container_جو(chip, بنية aplic_معلومات, irqchip);
	إذا (aplic_hwirq_is_delegated(aplic, hwirq))
		أرجع SBI_ENOTSUPP;

	إذا (!aplic_is_سڌو_گهڻائي(aplic)) {
		والد = sbi_irqchip_ڳوليو_device(aplic->والد_unique_id);
		إذا (!والد) {
			sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
				__func__, (بنانشان ڊگهو)hwirq);
			أرجع SBI_EINVAL;
		}

		msi_معلومات = sbi_irqchip_get_hwirq_priv(chip, hwirq);
		rc = sbi_irqchip_سيٽ_affinity(والد, msi_معلومات->والد_hwirq, hart_اشاريو);
		إذا (rc) {
			sbi_printf("%s: sbi_irqchip_set_affinity failed hwirq=%lu rc=%d\n",
				__func__, (بنانشان ڊگهو)hwirq, rc);
			أرجع rc;
		}
		أرجع 0;
	}

	idc_اشاريو = aplic_ڳوليو_idc_اشاريو(aplic, hart_اشاريو);
	إذا (idc_اشاريو < 0)
		أرجع SBI_EINVAL;

	aplic_هدف_write(aplic, hwirq,
			   APLIC_هدف_HART_IDX(hart_اشاريو) |
			   APLIC_هدف_IPRIO(APLIC_DEFAULT_ترجيح));

	أرجع 0;
}

ساكن فراغ aplic_hwirq_نقاب(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية aplic_معلومات *aplic;

	aplic = container_جو(chip, بنية aplic_معلومات, irqchip);
	إذا (aplic_hwirq_is_delegated(aplic, hwirq))
		أرجع;

	aplic_irq_clrie(aplic, hwirq);
}

ساكن فراغ aplic_hwirq_unmask(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية aplic_معلومات *aplic;

	aplic = container_جو(chip, بنية aplic_معلومات, irqchip);
	إذا (aplic_hwirq_is_delegated(aplic, hwirq))
		أرجع;

	aplic_irq_setie(aplic, hwirq);
}

ساكن بنية sbi_irqchip_device aplic_irqchip_template = {
	.warm_init			= aplic_warm_init,
	.process_hwirqs		= aplic_process_hwirqs,
	.hwirq_eoi			= aplic_hwirq_eoi,
	.hwirq_تياري		= aplic_hwirq_تياري,
	.hwirq_cleanup		= aplic_hwirq_cleanup,
	.hwirq_سيٽ_affinity	= aplic_hwirq_سيٽ_affinity,
	.hwirq_نقاب			= aplic_hwirq_نقاب,
	.hwirq_unmask		= aplic_hwirq_unmask,
};

صحيح aplic_cold_irqchip_init(بنية aplic_معلومات *aplic)
{
	صحيح rc;
	بنية aplic_delegate_معلومات *deleg;
	u32 پهريون_deleg_irq, آخري_deleg_irq, i;
	bool msi_گهڻائي;

	msi_گهڻائي = !aplic_is_سڌو_گهڻائي(aplic);
	إذا (!aplic->انگ_ذريعو) {
		sbi_printf("%s: num_source is zero\n", __func__);
		أرجع SBI_EINVAL;
	}

	إذا (APLIC_MAX_ذريعو <= aplic->انگ_ذريعو) {
		sbi_printf("%s: num_source=%lu exceeds max=%lu\n",
			__func__,
			(بنانشان ڊگهو)aplic->انگ_ذريعو,
			(بنانشان ڊگهو)APLIC_MAX_ذريعو);
		أرجع SBI_EINVAL;
	}

	إذا (!msi_گهڻائي && APLIC_MAX_IDC <= aplic->انگ_idc) {
		sbi_printf("%s: num_idc=%lu exceeds max=%lu\n",
			__func__,
			(بنانشان ڊگهو)aplic->انگ_idc,
			(بنانشان ڊگهو)APLIC_MAX_IDC);
		أرجع SBI_EINVAL;
	}

	إذا (aplic->targets_mmode && aplic->has_msicfg_mmode) {
		rc = aplic_check_msicfg(&aplic->msicfg_mmode);
		إذا (rc) {
			sbi_printf("%s: invalid M-mode msicfg rc=%d\n",
				__func__, rc);
			أرجع rc;
		}
	}

	إذا (aplic->targets_mmode && aplic->has_msicfg_smode) {
		rc = aplic_check_msicfg(&aplic->msicfg_smode);
		إذا (rc) {
			sbi_printf("%s: invalid S-mode msicfg rc=%d\n",
				__func__, rc);
			أرجع rc;
		}
	}

	/* Init the APLIC registers */
	aplic_init(aplic);

	/*
	 * Add APLIC region to the root domain if:
	 * 1) It targets M-mode of any HART directly or via MSIs
	 * 2) All interrupts are delegated to some child APLIC
	 */
	پهريون_deleg_irq = -1U;
	آخري_deleg_irq = 0;
	لكل (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		إذا (deleg->پهريون_irq < پهريون_deleg_irq)
			پهريون_deleg_irq = deleg->پهريون_irq;
		إذا (آخري_deleg_irq < deleg->آخري_irq)
			آخري_deleg_irq = deleg->آخري_irq;
	}

	إذا (aplic->targets_mmode ||
	    ((پهريون_deleg_irq < آخري_deleg_irq) &&
	    (آخري_deleg_irq == aplic->انگ_ذريعو) &&
	    (پهريون_deleg_irq == 1))) {
		rc = sbi_domain_پاڙ_جوڙيو_memrange(aplic->addr, aplic->سائيز, صفحو_سائيز,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		إذا (rc)
			أرجع rc;
	}

	إذا ((aplic->targets_mmode)) {
		aplic->irqchip		 = aplic_irqchip_template;
		aplic->irqchip.id	 = aplic->unique_id;
		aplic->irqchip.caps = SBI_IRQCHIP_CAPS_WIRED;
		aplic->irqchip.انگ_hwirq = aplic->انگ_ذريعو + 1;

		إذا (msi_گهڻائي) {
			aplic->irqchip.warm_init      = عدم;
			aplic->irqchip.process_hwirqs = عدم;
			aplic->irqchip.hwirq_eoi      = عدم;
		}

		إذا (msi_گهڻائي)
			sbi_hartmask_سيٽ_all(&aplic->irqchip.هدف_harts);
		وإلا
			لكل (i = 0; i < aplic->انگ_idc; i++)
				sbi_hartmask_سيٽ_hartindex(aplic->idc_map[i],
							   &aplic->irqchip.هدف_harts);

		rc = sbi_irqchip_جوڙيو_device(&aplic->irqchip);
		إذا (rc) {
			sbi_printf("%s: sbi_irqchip_add_device failed rc=%d id=%lu mode=%s target_weight=%lu\n",
				__func__, rc,
				(بنانشان ڊگهو)aplic->irqchip.id,
				msi_گهڻائي ? "msi" : "direct",
				(بنانشان ڊگهو)sbi_hartmask_وزن(
					&aplic->irqchip.هدف_harts));
			أرجع rc;
		}
	}
	/* Attach to the aplic list */
	sbi_list_جوڙيو_پڇ(&aplic->ڳنڍ, &aplic_list);

	أرجع 0;
}
