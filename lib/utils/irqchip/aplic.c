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

#تعریف_کرو APLIC_MAX_IDC			(1UL << 14)
#تعریف_کرو APLIC_MAX_سروت		1024

#تعریف_کرو APLIC_DOMAINCFG		0x0000
#تعریف_کرو APLIC_DOMAINCFG_IE		(1 << 8)
#تعریف_کرو APLIC_DOMAINCFG_DM		(1 << 2)
#تعریف_کرو APLIC_DOMAINCFG_BE		(1 << 0)

#تعریف_کرو APLIC_SOURCECFG_ادھار		0x0004
#تعریف_کرو APLIC_SOURCECFG_D		(1 << 10)
#تعریف_کرو APLIC_SOURCECFG_CHILDIDX_نقاب	0x000003ff
#تعریف_کرو APLIC_SOURCECFG_SM_نقاب	0x00000007
#تعریف_کرو APLIC_SOURCECFG_SM_INACTIVE	0x0
#تعریف_کرو APLIC_SOURCECFG_SM_DETACH	0x1
#تعریف_کرو APLIC_SOURCECFG_SM_کنارا_RISE	0x4
#تعریف_کرو APLIC_SOURCECFG_SM_کنارا_FALL	0x5
#تعریف_کرو APLIC_SOURCECFG_SM_LEVEL_بلند	0x6
#تعریف_کرو APLIC_SOURCECFG_SM_LEVEL_کم	0x7

#تعریف_کرو APLIC_MMSICFGADDR		0x1bc0
#تعریف_کرو APLIC_MMSICFGADDRH		0x1bc4
#تعریف_کرو APLIC_SMSICFGADDR		0x1bc8
#تعریف_کرو APLIC_SMSICFGADDRH		0x1bcc

#تعریف_کرو APLIC_xMSICFGADDRH_L		(1UL << 31)
#تعریف_کرو APLIC_x_MSICFGADDRH_HHXS_نقاب	0x1f
#تعریف_کرو APLIC_x_MSICFGADDRH_HHXS_کھسکاؤ	24
#تعریف_کرو APLIC_x_MSICFGADDRH_LHXS_نقاب	0x7
#تعریف_کرو APLIC_x_MSICFGADDRH_LHXS_کھسکاؤ	20
#تعریف_کرو APLIC_x_MSICFGADDRH_HHXW_نقاب	0x7
#تعریف_کرو APLIC_x_MSICFGADDRH_HHXW_کھسکاؤ	16
#تعریف_کرو APLIC_x_MSICFGADDRH_LHXW_نقاب	0xf
#تعریف_کرو APLIC_x_MSICFGADDRH_LHXW_کھسکاؤ	12
#تعریف_کرو APLIC_x_MSICFGADDRH_BAPPN_نقاب	0xfff

#تعریف_کرو APLIC_x_MSICFGADDR_PPN_کھسکاؤ	12

#تعریف_کرو APLIC_xMSICFGADDR_PPN_HART(__lhxs) \
	((1UL << (__lhxs)) - 1)

#تعریف_کرو APLIC_x_MSICFGADDR_PPN_LHX_نقاب(__lhxw) \
	((1UL << (__lhxw)) - 1)
#تعریف_کرو APLIC_x_MSICFGADDR_PPN_LHX_کھسکاؤ(__lhxs) \
	((__lhxs))
#تعریف_کرو APLIC_xMSICFGADDR_PPN_LHX(__lhxw, __lhxs) \
	(APLIC_x_MSICFGADDR_PPN_LHX_نقاب(__lhxw) << \
	 APLIC_x_MSICFGADDR_PPN_LHX_کھسکاؤ(__lhxs))

#تعریف_کرو APLIC_x_MSICFGADDR_PPN_HHX_نقاب(__hhxw) \
	((1UL << (__hhxw)) - 1)
#تعریف_کرو APLIC_x_MSICFGADDR_PPN_HHX_کھسکاؤ(__hhxs) \
	((__hhxs) + APLIC_x_MSICFGADDR_PPN_کھسکاؤ)
#تعریف_کرو APLIC_xMSICFGADDR_PPN_HHX(__hhxw, __hhxs) \
	(APLIC_x_MSICFGADDR_PPN_HHX_نقاب(__hhxw) << \
	 APLIC_x_MSICFGADDR_PPN_HHX_کھسکاؤ(__hhxs))

#تعریف_کرو APLIC_نشانہ_GUEST_IDX(__gidx) \
	((((u32)(__gidx)) & APLIC_نشانہ_GUEST_IDX_نقاب) << \
	 APLIC_نشانہ_GUEST_IDX_کھسکاؤ)
#تعریف_کرو APLIC_نشانہ_EIID(__eiid) \
	(((u32)(__eiid)) & APLIC_نشانہ_EIID_نقاب)

#تعریف_کرو APLIC_SETIP_ادھار		0x1c00
#تعریف_کرو APLIC_SETIPNUM			0x1cdc

#تعریف_کرو APLIC_CLRIP_ادھار		0x1d00
#تعریف_کرو APLIC_CLRIPNUM			0x1ddc

#تعریف_کرو APLIC_SETIE_ادھار		0x1e00
#تعریف_کرو APLIC_SETIENUM			0x1edc

#تعریف_کرو APLIC_CLRIE_ادھار		0x1f00
#تعریف_کرو APLIC_CLRIENUM			0x1fdc

#تعریف_کرو APLIC_SETIPNUM_LE		0x2000
#تعریف_کرو APLIC_SETIPNUM_BE		0x2004

#تعریف_کرو APLIC_نشانہ_ادھار		0x3004
#تعریف_کرو APLIC_نشانہ_HART_IDX_کھسکاؤ	18
#تعریف_کرو APLIC_نشانہ_HART_IDX_نقاب	0x3fff
#تعریف_کرو APLIC_نشانہ_GUEST_IDX_کھسکاؤ	12
#تعریف_کرو APLIC_نشانہ_GUEST_IDX_نقاب	0x3f
#تعریف_کرو APLIC_نشانہ_IPRIO_نقاب	0xff
#تعریف_کرو APLIC_نشانہ_EIID_نقاب	0x7ff

#تعریف_کرو APLIC_نشانہ_HART_IDX(__hidx) \
	((((u32)(__hidx)) & APLIC_نشانہ_HART_IDX_نقاب) << \
	 APLIC_نشانہ_HART_IDX_کھسکاؤ)
#تعریف_کرو APLIC_نشانہ_IPRIO(__prio) \
	(((u32)(__prio)) & APLIC_نشانہ_IPRIO_نقاب)

#تعریف_کرو APLIC_IDC_ادھار			0x4000
#تعریف_کرو APLIC_IDC_ناپ			32

#تعریف_کرو APLIC_IDC_IDELIVERY		0x00

#تعریف_کرو APLIC_IDC_IFORCE		0x04

#تعریف_کرو APLIC_IDC_ITHRESHOLD		0x08

#تعریف_کرو APLIC_IDC_TOPI			0x18
#تعریف_کرو APLIC_IDC_TOPI_ID_کھسکاؤ	16
#تعریف_کرو APLIC_IDC_TOPI_ID_نقاب	0x3ff
#تعریف_کرو APLIC_IDC_TOPI_PRIO_نقاب	0xff

#تعریف_کرو APLIC_IDC_CLAIMI		0x1c

#تعریف_کرو APLIC_DEFAULT_ترجیح		1
#تعریف_کرو APLIC_DISABLE_IDELIVERY		0
#تعریف_کرو APLIC_ENABLE_IDELIVERY		1
#تعریف_کرو APLIC_DISABLE_ITHRESHOLD	1
#تعریف_کرو APLIC_ENABLE_ITHRESHOLD		0

بنية aplic_msi_انکڑے {
	بنية aplic_انکڑے *aplic;
	u32 hwirq;
	u32 ماپا_hwirq;
};

ساكن SBI_LIST_سر(aplic_list);
ساكن فراغ aplic_writel_msicfg(بنية aplic_msicfg_انکڑے *msicfg,
				فراغ *msicfgaddr, فراغ *msicfgaddrH);

ساكن مضمن bool aplic_is_سدھا_منوال(بنية aplic_انکڑے *aplic)
{
	أرجع aplic && aplic->عدد_idc;
}

ساكن مضمن فراغ aplic_sourcecfg_write(بنية aplic_انکڑے *aplic,
					 u32 hwirq, u32 val)
{
	writel(val, (فراغ *)(aplic->addr + APLIC_SOURCECFG_ادھار +
			     (hwirq - 1) * حجم(u32)));
}

ساكن مضمن u32 aplic_sourcecfg_read(بنية aplic_انکڑے *aplic, u32 hwirq)
{
	أرجع readl((فراغ *)(aplic->addr + APLIC_SOURCECFG_ادھار +
			      (hwirq - 1) * حجم(u32)));
}

ساكن مضمن فراغ aplic_نشانہ_write(بنية aplic_انکڑے *aplic,
				      u32 hwirq, u32 val)
{
	writel(val, (فراغ *)(aplic->addr + APLIC_نشانہ_ادھار +
			     (hwirq - 1) * حجم(u32)));
}

ساكن مضمن فراغ aplic_irq_setie(بنية aplic_انکڑے *aplic, u32 hwirq)
{
	writel(hwirq, (فراغ *)(aplic->addr + APLIC_SETIENUM));
}

ساكن مضمن فراغ aplic_irq_clrie(بنية aplic_انکڑے *aplic, u32 hwirq)
{
	writel(hwirq, (فراغ *)(aplic->addr + APLIC_CLRIENUM));
}

ساكن مضمن فراغ aplic_irq_clrip(بنية aplic_انکڑے *aplic, u32 hwirq)
{
	writel(hwirq, (فراغ *)(aplic->addr + APLIC_CLRIPNUM));
}

ساكن مضمن فراغ aplic_msi_irq_retrigger_level(بنية aplic_انکڑے *aplic, u32 hwirq)
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

ساكن مضمن فراغ *aplic_idc_ادھار(بنية aplic_انکڑے *aplic, u32 idc_اشاریہ)
{
	أرجع (فراغ *)(aplic->addr + APLIC_IDC_ادھار +
			idc_اشاریہ * APLIC_IDC_ناپ);
}

ساكن مضمن u32 aplic_idc_read(بنية aplic_انکڑے *aplic, u32 idc_اشاریہ,
				 u32 reg)
{
	أرجع readl(aplic_idc_ادھار(aplic, idc_اشاریہ) + reg);
}

ساكن مضمن فراغ aplic_idc_write(بنية aplic_انکڑے *aplic, u32 idc_اشاریہ,
				   u32 reg, u32 val)
{
	writel(val, aplic_idc_ادھار(aplic, idc_اشاریہ) + reg);
}

ساكن u32 aplic_hwirq_flags_منزل_sourcecfg(u32 hwirq_flags)
{
	بدل (hwirq_flags & SBI_HWIRQ_FLAGS_LEVEL_SENSE_نقاب) {
	حالة SBI_HWIRQ_FLAGS_کنارا_RISING:
		أرجع APLIC_SOURCECFG_SM_کنارا_RISE;
	حالة SBI_HWIRQ_FLAGS_کنارا_FALLING:
		أرجع APLIC_SOURCECFG_SM_کنارا_FALL;
	حالة SBI_HWIRQ_FLAGS_LEVEL_بلند:
		أرجع APLIC_SOURCECFG_SM_LEVEL_بلند;
	حالة SBI_HWIRQ_FLAGS_LEVEL_کم:
		أرجع APLIC_SOURCECFG_SM_LEVEL_کم;
	افتراضي:
		أرجع APLIC_SOURCECFG_SM_INACTIVE;
	}
}

ساكن bool aplic_hwirq_is_delegated(بنية aplic_انکڑے *aplic, u32 hwirq)
{
	u32 sourcecfg;

	sourcecfg = aplic_sourcecfg_read(aplic, hwirq);
	أرجع !!(sourcecfg & APLIC_SOURCECFG_D);
}

ساكن صحيح aplic_لبھو_idc_اشاریہ(بنية aplic_انکڑے *aplic, u32 hart_اشاریہ)
{
	u32 i;

	لكل (i = 0; i < aplic->عدد_idc; i++) {
		إذا (aplic->idc_map[i] == hart_اشاریہ)
			أرجع i;
	}

	أرجع SBI_ENOENT;
}

ساكن فراغ aplic_init(بنية aplic_انکڑے *aplic)
{
	بنية aplic_delegate_انکڑے *deleg;
	u32 i, j, tmp, domaincfg;
	صحيح locked;

	/* Set domain configuration to 0 */
	writel(0, (فراغ *)(aplic->addr + APLIC_DOMAINCFG));

	/* Disable all interrupts */
	لكل (i = 0; i <= aplic->عدد_سروت; i += 32)
		writel(-1U, (فراغ *)(aplic->addr + APLIC_CLRIE_ادھار +
				     (i / 32) * حجم(u32)));

	/* Set interrupt type and priority for all interrupts */
	لكل (i = 1; i <= aplic->عدد_سروت; i++) {
		/* Set IRQ source configuration to 0 */
		writel(0, (فراغ *)(aplic->addr + APLIC_SOURCECFG_ادھار +
			  (i - 1) * حجم(u32)));
		/* Set IRQ target hart index and priority to 1 */
		writel(APLIC_DEFAULT_ترجیح, (فراغ *)(aplic->addr +
						APLIC_نشانہ_ادھار +
						(i - 1) * حجم(u32)));
	}

	/* Configure IRQ delegation */
	لكل (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		إذا (!deleg->پہلا_irq || !deleg->آخری_irq)
			استمر;
		إذا (aplic->عدد_سروت < deleg->پہلا_irq ||
		    aplic->عدد_سروت < deleg->آخری_irq)
			استمر;
		إذا (deleg->بچہ_اشاریہ > APLIC_SOURCECFG_CHILDIDX_نقاب)
			استمر;
		إذا (deleg->پہلا_irq > deleg->آخری_irq) {
			tmp = deleg->پہلا_irq;
			deleg->پہلا_irq = deleg->آخری_irq;
			deleg->آخری_irq = tmp;
		}
		لكل (j = deleg->پہلا_irq; j <= deleg->آخری_irq; j++)
			aplic_sourcecfg_write(aplic, j, APLIC_SOURCECFG_D | deleg->بچہ_اشاریہ);
	}

	/* Default initialization of IDC structures */
	إذا (aplic_is_سدھا_منوال(aplic)) {
		لكل (i = 0; i < aplic->عدد_idc; i++) {
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
	إذا (!aplic_is_سدھا_منوال(aplic))
		domaincfg |= APLIC_DOMAINCFG_DM;

	writel(domaincfg, (فراغ *)(aplic->addr + APLIC_DOMAINCFG));
}

فراغ aplic_reinit_all(فراغ)
{
	بنية aplic_انکڑے *aplic;

	sbi_list_for_each_اندراج(aplic, &aplic_list, گرہ)
		aplic_init(aplic);
}

ساكن فراغ aplic_writel_msicfg(بنية aplic_msicfg_انکڑے *msicfg,
				فراغ *msicfgaddr, فراغ *msicfgaddrH)
{
	u32 val;
	بنانشانی لما ادھار_ppn;

	/* Compute the MSI base PPN */
	ادھار_ppn = msicfg->ادھار_addr >> APLIC_x_MSICFGADDR_PPN_کھسکاؤ;
	ادھار_ppn &= ~APLIC_xMSICFGADDR_PPN_HART(msicfg->lhxs);
	ادھار_ppn &= ~APLIC_xMSICFGADDR_PPN_LHX(msicfg->lhxw, msicfg->lhxs);
	ادھار_ppn &= ~APLIC_xMSICFGADDR_PPN_HHX(msicfg->hhxw, msicfg->hhxs);

	/* Write the lower MSI config register */
	writel((u32)ادھار_ppn, msicfgaddr);

	/* Write the upper MSI config register */
	val = (((u64)ادھار_ppn) >> 32) &
		APLIC_x_MSICFGADDRH_BAPPN_نقاب;
	val |= (msicfg->lhxw & APLIC_x_MSICFGADDRH_LHXW_نقاب)
		<< APLIC_x_MSICFGADDRH_LHXW_کھسکاؤ;
	val |= (msicfg->hhxw & APLIC_x_MSICFGADDRH_HHXW_نقاب)
		<< APLIC_x_MSICFGADDRH_HHXW_کھسکاؤ;
	val |= (msicfg->lhxs & APLIC_x_MSICFGADDRH_LHXS_نقاب)
		<< APLIC_x_MSICFGADDRH_LHXS_کھسکاؤ;
	val |= (msicfg->hhxs & APLIC_x_MSICFGADDRH_HHXS_نقاب)
		<< APLIC_x_MSICFGADDRH_HHXS_کھسکاؤ;
	writel(val, msicfgaddrH);
}

ساكن صحيح aplic_check_msicfg(بنية aplic_msicfg_انکڑے *msicfg)
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
	بنية aplic_انکڑے *aplic;
	u32 hart_اشاریہ;
	صحيح idc_اشاریہ;

	aplic = container_دا(chip, بنية aplic_انکڑے, irqchip);
	إذا (!aplic_is_سدھا_منوال(aplic))
		أرجع 0;

	hart_اشاریہ = current_hartindex();
	idc_اشاریہ  = aplic_لبھو_idc_اشاریہ(aplic, hart_اشاریہ);
	إذا (idc_اشاریہ < 0)
		أرجع 0;

	aplic_idc_write(aplic, idc_اشاریہ, APLIC_IDC_ITHRESHOLD,
			APLIC_ENABLE_ITHRESHOLD);
	aplic_idc_write(aplic, idc_اشاریہ, APLIC_IDC_IDELIVERY,
			APLIC_ENABLE_IDELIVERY);

	أرجع 0;
}

ساكن صحيح aplic_process_hwirqs(بنية sbi_irqchip_device *chip)
{
	بنية aplic_انکڑے *aplic;
	u32 hart_اشاریہ, claimi, hwirq;
	صحيح idc_اشاریہ, rc = 0, tmp;

	aplic = container_دا(chip, بنية aplic_انکڑے, irqchip);

	إذا (!aplic_is_سدھا_منوال(aplic))
		sbi_panic("aplic_process_hwirqs called in MSI mode.\n");

	hart_اشاریہ = current_hartindex();
	idc_اشاریہ  = aplic_لبھو_idc_اشاریہ(aplic, hart_اشاریہ);
	إذا (idc_اشاریہ < 0)
		أرجع 0;

	طالما ((claimi = aplic_idc_read(aplic, idc_اشاریہ, APLIC_IDC_CLAIMI))) {
		hwirq = (claimi >> APLIC_IDC_TOPI_ID_کھسکاؤ) &
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
	بنية aplic_انکڑے *aplic;
	u32 sm;

	aplic = container_دا(chip, بنية aplic_انکڑے, irqchip);
	إذا (aplic_is_سدھا_منوال(aplic))
		أرجع;

	sm = aplic_sourcecfg_read(aplic, hwirq) & APLIC_SOURCECFG_SM_نقاب;
	إذا (sm == APLIC_SOURCECFG_SM_LEVEL_بلند ||
	    sm == APLIC_SOURCECFG_SM_LEVEL_کم)
		aplic_msi_irq_retrigger_level(aplic, hwirq);
}

ساكن فراغ aplic_پروگرام_msi_نشانہ(بنية aplic_انکڑے *aplic,
				     u32 hwirq, u32 hart_اشاریہ,
				     u32 guest_اشاریہ, u32 eiid)
{
	u32 نشانہ;

	نشانہ = APLIC_نشانہ_HART_IDX(hart_اشاریہ) |
		 APLIC_نشانہ_GUEST_IDX(guest_اشاریہ) |
		 APLIC_نشانہ_EIID(eiid);

	aplic_نشانہ_write(aplic, hwirq, نشانہ);
}

ساكن u32 derive_hart_اشاریہ(بنية aplic_msicfg_انکڑے *msicfg,
			      ثابت بنية sbi_irqchip_msi_پیغام *پیغام)
{
	u64 addr = ((u64)پیغام->پتہ_hi << 32) | پیغام->پتہ_lo;
	u64 tppn = addr >> APLIC_x_MSICFGADDR_PPN_کھسکاؤ;
	u32 گروہ_اشاریہ, hart_اشاریہ;

	گروہ_اشاریہ = (tppn >> APLIC_x_MSICFGADDR_PPN_HHX_کھسکاؤ(msicfg->hhxs)) &
		      APLIC_x_MSICFGADDR_PPN_HHX_نقاب(msicfg->hhxw);

	hart_اشاریہ = (tppn >> msicfg->lhxs) &
		     ((1UL << msicfg->lhxw) - 1);

	hart_اشاریہ |= (گروہ_اشاریہ << msicfg->lhxw);

	أرجع hart_اشاریہ;
}

ساكن فراغ aplic_write_msi(u32 ماپا_hwirq,
			    ثابت بنية sbi_irqchip_msi_پیغام *پیغام,
			    فراغ *priv)
{
	بنية aplic_msi_انکڑے *msi_انکڑے = priv;
	u32 guest_اشاریہ = 0;
	u32 eiid;

	eiid = پیغام->انکڑے;
	إذا (!eiid || eiid > APLIC_نشانہ_EIID_نقاب)
		أرجع;

	aplic_پروگرام_msi_نشانہ(msi_انکڑے->aplic, msi_انکڑے->hwirq,
				derive_hart_اشاریہ(&msi_انکڑے->aplic->msicfg_mmode, پیغام),
				guest_اشاریہ, eiid);
}

ساكن صحيح aplic_msi_callback(u32 ماپا_hwirq, فراغ *priv)
{
	بنية aplic_msi_انکڑے *msi_انکڑے = priv;
	صحيح rc;

	rc = sbi_irqchip_process_hwirq(&msi_انکڑے->aplic->irqchip, msi_انکڑے->hwirq);
	إذا (rc && rc != SBI_ENOENT)
		sbi_printf("%s: hwirq=%lu failed rc=%d\n",
			__func__,
			(بنانشانی لما)ماپا_hwirq, rc);

	أرجع rc;
}

ساكن صحيح aplic_تیاری_msi(بنية aplic_انکڑے *aplic, u32 hwirq)
{
	بنية sbi_irqchip_device *ماپا;
	u32 پہلا_hwirq;
	بنية aplic_msi_انکڑے *msi_انکڑے;
	صحيح rc;

	ماپا = sbi_irqchip_لبھو_device(aplic->ماپا_unique_id);
	إذا (!ماپا) {
		sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
			__func__, (بنانشانی لما)hwirq);
		أرجع SBI_EINVAL;
	}

	msi_انکڑے = sbi_zalloc(حجم(بنية aplic_msi_انکڑے));
	إذا (!msi_انکڑے)
		أرجع SBI_ENOMEM;

	msi_انکڑے->aplic = aplic;
	msi_انکڑے->hwirq = hwirq;

	rc = sbi_irqchip_رجسٹر_msi(ماپا, 1,
					aplic_write_msi,
					aplic_msi_callback,
					msi_انکڑے,
					&پہلا_hwirq);
	إذا (rc) {
		sbi_printf("%s: register_msi failed hwirq=%lu rc=%d\n",
			__func__, (بنانشانی لما)hwirq, rc);
		sbi_free(msi_انکڑے);
		أرجع rc;
	}

	msi_انکڑے->ماپا_hwirq = پہلا_hwirq;
	sbi_irqchip_سیٹ_hwirq_priv(&aplic->irqchip, hwirq, msi_انکڑے);

	أرجع 0;
}

ساكن صحيح aplic_hwirq_تیاری(بنية sbi_irqchip_device *chip,
			     u32 hwirq, u32 hwirq_flags)
{
	بنية aplic_انکڑے *aplic;
	u32 sourcecfg;

	aplic = container_دا(chip, بنية aplic_انکڑے, irqchip);

	sourcecfg = aplic_hwirq_flags_منزل_sourcecfg(hwirq_flags);
	إذا (sourcecfg == APLIC_SOURCECFG_SM_INACTIVE) {
		sbi_printf("%s: unsupported flags=0x%lx hwirq=%lu\n",
			__func__,
			(بنانشانی لما)hwirq_flags,
			(بنانشانی لما)hwirq);
		أرجع SBI_EINVAL;
	}

	aplic_sourcecfg_write(aplic, hwirq, sourcecfg);

	إذا (aplic_hwirq_is_delegated(aplic, hwirq)) {
		sbi_printf("%s: hwirq=%lu is delegated\n",
			__func__, (بنانشانی لما)hwirq);
		أرجع SBI_ENOTSUPP;
	}

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);

	إذا (!aplic_is_سدھا_منوال(aplic))
		أرجع aplic_تیاری_msi(aplic, hwirq);

	أرجع 0;
}

ساكن فراغ aplic_hwirq_cleanup(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية aplic_انکڑے *aplic;
	بنية aplic_msi_انکڑے *msi_انکڑے;

	aplic = container_دا(chip, بنية aplic_انکڑے, irqchip);
	إذا (aplic_hwirq_is_delegated(aplic, hwirq))
		أرجع;

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);
	aplic_sourcecfg_write(aplic, hwirq, APLIC_SOURCECFG_SM_INACTIVE);
	aplic_نشانہ_write(aplic, hwirq, APLIC_DEFAULT_ترجیح);
	msi_انکڑے = (بنية aplic_msi_انکڑے *) sbi_irqchip_get_hwirq_priv(chip, hwirq);
	إذا (msi_انکڑے)
		sbi_free(msi_انکڑے);
}

ساكن صحيح aplic_hwirq_سیٹ_affinity(بنية sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_اشاریہ)
{
	بنية aplic_انکڑے *aplic;
	بنية aplic_msi_انکڑے *msi_انکڑے;
	بنية sbi_irqchip_device *ماپا;
	صحيح idc_اشاریہ;
	صحيح rc;

	aplic = container_دا(chip, بنية aplic_انکڑے, irqchip);
	إذا (aplic_hwirq_is_delegated(aplic, hwirq))
		أرجع SBI_ENOTSUPP;

	إذا (!aplic_is_سدھا_منوال(aplic)) {
		ماپا = sbi_irqchip_لبھو_device(aplic->ماپا_unique_id);
		إذا (!ماپا) {
			sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
				__func__, (بنانشانی لما)hwirq);
			أرجع SBI_EINVAL;
		}

		msi_انکڑے = sbi_irqchip_get_hwirq_priv(chip, hwirq);
		rc = sbi_irqchip_سیٹ_affinity(ماپا, msi_انکڑے->ماپا_hwirq, hart_اشاریہ);
		إذا (rc) {
			sbi_printf("%s: sbi_irqchip_set_affinity failed hwirq=%lu rc=%d\n",
				__func__, (بنانشانی لما)hwirq, rc);
			أرجع rc;
		}
		أرجع 0;
	}

	idc_اشاریہ = aplic_لبھو_idc_اشاریہ(aplic, hart_اشاریہ);
	إذا (idc_اشاریہ < 0)
		أرجع SBI_EINVAL;

	aplic_نشانہ_write(aplic, hwirq,
			   APLIC_نشانہ_HART_IDX(hart_اشاریہ) |
			   APLIC_نشانہ_IPRIO(APLIC_DEFAULT_ترجیح));

	أرجع 0;
}

ساكن فراغ aplic_hwirq_نقاب(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية aplic_انکڑے *aplic;

	aplic = container_دا(chip, بنية aplic_انکڑے, irqchip);
	إذا (aplic_hwirq_is_delegated(aplic, hwirq))
		أرجع;

	aplic_irq_clrie(aplic, hwirq);
}

ساكن فراغ aplic_hwirq_unmask(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية aplic_انکڑے *aplic;

	aplic = container_دا(chip, بنية aplic_انکڑے, irqchip);
	إذا (aplic_hwirq_is_delegated(aplic, hwirq))
		أرجع;

	aplic_irq_setie(aplic, hwirq);
}

ساكن بنية sbi_irqchip_device aplic_irqchip_template = {
	.warm_init			= aplic_warm_init,
	.process_hwirqs		= aplic_process_hwirqs,
	.hwirq_eoi			= aplic_hwirq_eoi,
	.hwirq_تیاری		= aplic_hwirq_تیاری,
	.hwirq_cleanup		= aplic_hwirq_cleanup,
	.hwirq_سیٹ_affinity	= aplic_hwirq_سیٹ_affinity,
	.hwirq_نقاب			= aplic_hwirq_نقاب,
	.hwirq_unmask		= aplic_hwirq_unmask,
};

صحيح aplic_cold_irqchip_init(بنية aplic_انکڑے *aplic)
{
	صحيح rc;
	بنية aplic_delegate_انکڑے *deleg;
	u32 پہلا_deleg_irq, آخری_deleg_irq, i;
	bool msi_منوال;

	msi_منوال = !aplic_is_سدھا_منوال(aplic);
	إذا (!aplic->عدد_سروت) {
		sbi_printf("%s: num_source is zero\n", __func__);
		أرجع SBI_EINVAL;
	}

	إذا (APLIC_MAX_سروت <= aplic->عدد_سروت) {
		sbi_printf("%s: num_source=%lu exceeds max=%lu\n",
			__func__,
			(بنانشانی لما)aplic->عدد_سروت,
			(بنانشانی لما)APLIC_MAX_سروت);
		أرجع SBI_EINVAL;
	}

	إذا (!msi_منوال && APLIC_MAX_IDC <= aplic->عدد_idc) {
		sbi_printf("%s: num_idc=%lu exceeds max=%lu\n",
			__func__,
			(بنانشانی لما)aplic->عدد_idc,
			(بنانشانی لما)APLIC_MAX_IDC);
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
	پہلا_deleg_irq = -1U;
	آخری_deleg_irq = 0;
	لكل (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		إذا (deleg->پہلا_irq < پہلا_deleg_irq)
			پہلا_deleg_irq = deleg->پہلا_irq;
		إذا (آخری_deleg_irq < deleg->آخری_irq)
			آخری_deleg_irq = deleg->آخری_irq;
	}

	إذا (aplic->targets_mmode ||
	    ((پہلا_deleg_irq < آخری_deleg_irq) &&
	    (آخری_deleg_irq == aplic->عدد_سروت) &&
	    (پہلا_deleg_irq == 1))) {
		rc = sbi_domain_جڑ_جوڑو_memrange(aplic->addr, aplic->ناپ, ورقہ_ناپ,
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
		aplic->irqchip.عدد_hwirq = aplic->عدد_سروت + 1;

		إذا (msi_منوال) {
			aplic->irqchip.warm_init      = عدم;
			aplic->irqchip.process_hwirqs = عدم;
			aplic->irqchip.hwirq_eoi      = عدم;
		}

		إذا (msi_منوال)
			sbi_hartmask_سیٹ_all(&aplic->irqchip.نشانہ_harts);
		وإلا
			لكل (i = 0; i < aplic->عدد_idc; i++)
				sbi_hartmask_سیٹ_hartindex(aplic->idc_map[i],
							   &aplic->irqchip.نشانہ_harts);

		rc = sbi_irqchip_جوڑو_device(&aplic->irqchip);
		إذا (rc) {
			sbi_printf("%s: sbi_irqchip_add_device failed rc=%d id=%lu mode=%s target_weight=%lu\n",
				__func__, rc,
				(بنانشانی لما)aplic->irqchip.id,
				msi_منوال ? "msi" : "direct",
				(بنانشانی لما)sbi_hartmask_بھار(
					&aplic->irqchip.نشانہ_harts));
			أرجع rc;
		}
	}
	/* Attach to the aplic list */
	sbi_list_جوڑو_پوچھ(&aplic->گرہ, &aplic_list);

	أرجع 0;
}
