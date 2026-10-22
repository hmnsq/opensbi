/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi_utils/irqchip/aplic.h>

#تعریف_کرو APLIC_MAX_IDC			(1UL << 14)
#تعریف_کرو APLIC_MAX_ماخذ		1024

#تعریف_کرو APLIC_DOMAINCFG		0x0000
#تعریف_کرو APLIC_DOMAINCFG_IE		(1 << 8)
#تعریف_کرو APLIC_DOMAINCFG_DM		(1 << 2)
#تعریف_کرو APLIC_DOMAINCFG_BE		(1 << 0)

#تعریف_کرو APLIC_SOURCECFG_بنیاد		0x0004
#تعریف_کرو APLIC_SOURCECFG_D		(1 << 10)
#تعریف_کرو APLIC_SOURCECFG_CHILDIDX_نقاب	0x000003ff
#تعریف_کرو APLIC_SOURCECFG_SM_نقاب	0x00000007
#تعریف_کرو APLIC_SOURCECFG_SM_INACTIVE	0x0
#تعریف_کرو APLIC_SOURCECFG_SM_DETACH	0x1
#تعریف_کرو APLIC_SOURCECFG_SM_کنارہ_RISE	0x4
#تعریف_کرو APLIC_SOURCECFG_SM_کنارہ_FALL	0x5
#تعریف_کرو APLIC_SOURCECFG_SM_LEVEL_بلند	0x6
#تعریف_کرو APLIC_SOURCECFG_SM_LEVEL_کم	0x7

#تعریف_کرو APLIC_MMSICFGADDR		0x1bc0
#تعریف_کرو APLIC_MMSICFGADDRH		0x1bc4
#تعریف_کرو APLIC_SMSICFGADDR		0x1bc8
#تعریف_کرو APLIC_SMSICFGADDRH		0x1bcc

#تعریف_کرو APLIC_xMSICFGADDRH_L		(1UL << 31)
#تعریف_کرو APLIC_x_MSICFGADDRH_HHXS_نقاب	0x1f
#تعریف_کرو APLIC_x_MSICFGADDRH_HHXS_سرکاؤ	24
#تعریف_کرو APLIC_x_MSICFGADDRH_LHXS_نقاب	0x7
#تعریف_کرو APLIC_x_MSICFGADDRH_LHXS_سرکاؤ	20
#تعریف_کرو APLIC_x_MSICFGADDRH_HHXW_نقاب	0x7
#تعریف_کرو APLIC_x_MSICFGADDRH_HHXW_سرکاؤ	16
#تعریف_کرو APLIC_x_MSICFGADDRH_LHXW_نقاب	0xf
#تعریف_کرو APLIC_x_MSICFGADDRH_LHXW_سرکاؤ	12
#تعریف_کرو APLIC_x_MSICFGADDRH_BAPPN_نقاب	0xfff

#تعریف_کرو APLIC_x_MSICFGADDR_PPN_سرکاؤ	12

#تعریف_کرو APLIC_xMSICFGADDR_PPN_HART(__lhxs) \
	((1UL << (__lhxs)) - 1)

#تعریف_کرو APLIC_x_MSICFGADDR_PPN_LHX_نقاب(__lhxw) \
	((1UL << (__lhxw)) - 1)
#تعریف_کرو APLIC_x_MSICFGADDR_PPN_LHX_سرکاؤ(__lhxs) \
	((__lhxs))
#تعریف_کرو APLIC_xMSICFGADDR_PPN_LHX(__lhxw, __lhxs) \
	(APLIC_x_MSICFGADDR_PPN_LHX_نقاب(__lhxw) << \
	 APLIC_x_MSICFGADDR_PPN_LHX_سرکاؤ(__lhxs))

#تعریف_کرو APLIC_x_MSICFGADDR_PPN_HHX_نقاب(__hhxw) \
	((1UL << (__hhxw)) - 1)
#تعریف_کرو APLIC_x_MSICFGADDR_PPN_HHX_سرکاؤ(__hhxs) \
	((__hhxs) + APLIC_x_MSICFGADDR_PPN_سرکاؤ)
#تعریف_کرو APLIC_xMSICFGADDR_PPN_HHX(__hhxw, __hhxs) \
	(APLIC_x_MSICFGADDR_PPN_HHX_نقاب(__hhxw) << \
	 APLIC_x_MSICFGADDR_PPN_HHX_سرکاؤ(__hhxs))

#تعریف_کرو APLIC_ہدف_GUEST_IDX(__gidx) \
	((((u32)(__gidx)) & APLIC_ہدف_GUEST_IDX_نقاب) << \
	 APLIC_ہدف_GUEST_IDX_سرکاؤ)
#تعریف_کرو APLIC_ہدف_EIID(__eiid) \
	(((u32)(__eiid)) & APLIC_ہدف_EIID_نقاب)

#تعریف_کرو APLIC_SETIP_بنیاد		0x1c00
#تعریف_کرو APLIC_SETIPNUM			0x1cdc

#تعریف_کرو APLIC_CLRIP_بنیاد		0x1d00
#تعریف_کرو APLIC_CLRIPNUM			0x1ddc

#تعریف_کرو APLIC_SETIE_بنیاد		0x1e00
#تعریف_کرو APLIC_SETIENUM			0x1edc

#تعریف_کرو APLIC_CLRIE_بنیاد		0x1f00
#تعریف_کرو APLIC_CLRIENUM			0x1fdc

#تعریف_کرو APLIC_SETIPNUM_LE		0x2000
#تعریف_کرو APLIC_SETIPNUM_BE		0x2004

#تعریف_کرو APLIC_ہدف_بنیاد		0x3004
#تعریف_کرو APLIC_ہدف_HART_IDX_سرکاؤ	18
#تعریف_کرو APLIC_ہدف_HART_IDX_نقاب	0x3fff
#تعریف_کرو APLIC_ہدف_GUEST_IDX_سرکاؤ	12
#تعریف_کرو APLIC_ہدف_GUEST_IDX_نقاب	0x3f
#تعریف_کرو APLIC_ہدف_IPRIO_نقاب	0xff
#تعریف_کرو APLIC_ہدف_EIID_نقاب	0x7ff

#تعریف_کرو APLIC_ہدف_HART_IDX(__hidx) \
	((((u32)(__hidx)) & APLIC_ہدف_HART_IDX_نقاب) << \
	 APLIC_ہدف_HART_IDX_سرکاؤ)
#تعریف_کرو APLIC_ہدف_IPRIO(__prio) \
	(((u32)(__prio)) & APLIC_ہدف_IPRIO_نقاب)

#تعریف_کرو APLIC_IDC_بنیاد			0x4000
#تعریف_کرو APLIC_IDC_سائز			32

#تعریف_کرو APLIC_IDC_IDELIVERY		0x00

#تعریف_کرو APLIC_IDC_IFORCE		0x04

#تعریف_کرو APLIC_IDC_ITHRESHOLD		0x08

#تعریف_کرو APLIC_IDC_TOPI			0x18
#تعریف_کرو APLIC_IDC_TOPI_ID_سرکاؤ	16
#تعریف_کرو APLIC_IDC_TOPI_ID_نقاب	0x3ff
#تعریف_کرو APLIC_IDC_TOPI_PRIO_نقاب	0xff

#تعریف_کرو APLIC_IDC_CLAIMI		0x1c

#تعریف_کرو APLIC_DEFAULT_ترجیح		1
#تعریف_کرو APLIC_DISABLE_IDELIVERY		0
#تعریف_کرو APLIC_ENABLE_IDELIVERY		1
#تعریف_کرو APLIC_DISABLE_ITHRESHOLD	1
#تعریف_کرو APLIC_ENABLE_ITHRESHOLD		0

ساخت aplic_msi_ڈیٹا {
	ساخت aplic_ڈیٹا *aplic;
	u32 hwirq;
	u32 والد_hwirq;
};

ساکن SBI_LIST_سر(aplic_list);
ساکن خالی aplic_writel_msicfg(ساخت aplic_msicfg_ڈیٹا *msicfg,
				خالی *msicfgaddr, خالی *msicfgaddrH);

ساکن اندرونی bool aplic_is_براہ_راست_منوال(ساخت aplic_ڈیٹا *aplic)
{
	واپس aplic && aplic->عدد_idc;
}

ساکن اندرونی خالی aplic_sourcecfg_write(ساخت aplic_ڈیٹا *aplic,
					 u32 hwirq, u32 val)
{
	writel(val, (خالی *)(aplic->addr + APLIC_SOURCECFG_بنیاد +
			     (hwirq - 1) * حجم(u32)));
}

ساکن اندرونی u32 aplic_sourcecfg_read(ساخت aplic_ڈیٹا *aplic, u32 hwirq)
{
	واپس readl((خالی *)(aplic->addr + APLIC_SOURCECFG_بنیاد +
			      (hwirq - 1) * حجم(u32)));
}

ساکن اندرونی خالی aplic_ہدف_write(ساخت aplic_ڈیٹا *aplic,
				      u32 hwirq, u32 val)
{
	writel(val, (خالی *)(aplic->addr + APLIC_ہدف_بنیاد +
			     (hwirq - 1) * حجم(u32)));
}

ساکن اندرونی خالی aplic_irq_setie(ساخت aplic_ڈیٹا *aplic, u32 hwirq)
{
	writel(hwirq, (خالی *)(aplic->addr + APLIC_SETIENUM));
}

ساکن اندرونی خالی aplic_irq_clrie(ساخت aplic_ڈیٹا *aplic, u32 hwirq)
{
	writel(hwirq, (خالی *)(aplic->addr + APLIC_CLRIENUM));
}

ساکن اندرونی خالی aplic_irq_clrip(ساخت aplic_ڈیٹا *aplic, u32 hwirq)
{
	writel(hwirq, (خالی *)(aplic->addr + APLIC_CLRIPNUM));
}

ساکن اندرونی خالی aplic_msi_irq_retrigger_level(ساخت aplic_ڈیٹا *aplic, u32 hwirq)
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
	writel(hwirq, (خالی *)(aplic->addr + APLIC_SETIPNUM_LE));
}

ساکن اندرونی خالی *aplic_idc_بنیاد(ساخت aplic_ڈیٹا *aplic, u32 idc_اشاریہ)
{
	واپس (خالی *)(aplic->addr + APLIC_IDC_بنیاد +
			idc_اشاریہ * APLIC_IDC_سائز);
}

ساکن اندرونی u32 aplic_idc_read(ساخت aplic_ڈیٹا *aplic, u32 idc_اشاریہ,
				 u32 reg)
{
	واپس readl(aplic_idc_بنیاد(aplic, idc_اشاریہ) + reg);
}

ساکن اندرونی خالی aplic_idc_write(ساخت aplic_ڈیٹا *aplic, u32 idc_اشاریہ,
				   u32 reg, u32 val)
{
	writel(val, aplic_idc_بنیاد(aplic, idc_اشاریہ) + reg);
}

ساکن u32 aplic_hwirq_flags_مقصد_sourcecfg(u32 hwirq_flags)
{
	انتخاب (hwirq_flags & SBI_HWIRQ_FLAGS_LEVEL_SENSE_نقاب) {
	حالت SBI_HWIRQ_FLAGS_کنارہ_RISING:
		واپس APLIC_SOURCECFG_SM_کنارہ_RISE;
	حالت SBI_HWIRQ_FLAGS_کنارہ_FALLING:
		واپس APLIC_SOURCECFG_SM_کنارہ_FALL;
	حالت SBI_HWIRQ_FLAGS_LEVEL_بلند:
		واپس APLIC_SOURCECFG_SM_LEVEL_بلند;
	حالت SBI_HWIRQ_FLAGS_LEVEL_کم:
		واپس APLIC_SOURCECFG_SM_LEVEL_کم;
	طےشدہ:
		واپس APLIC_SOURCECFG_SM_INACTIVE;
	}
}

ساکن bool aplic_hwirq_is_delegated(ساخت aplic_ڈیٹا *aplic, u32 hwirq)
{
	u32 sourcecfg;

	sourcecfg = aplic_sourcecfg_read(aplic, hwirq);
	واپس !!(sourcecfg & APLIC_SOURCECFG_D);
}

ساکن صحیح aplic_ڈھونڈو_idc_اشاریہ(ساخت aplic_ڈیٹا *aplic, u32 hart_اشاریہ)
{
	u32 i;

	برائے (i = 0; i < aplic->عدد_idc; i++) {
		اگر (aplic->idc_map[i] == hart_اشاریہ)
			واپس i;
	}

	واپس SBI_ENOENT;
}

ساکن خالی aplic_init(ساخت aplic_ڈیٹا *aplic)
{
	ساخت aplic_delegate_ڈیٹا *deleg;
	u32 i, j, tmp, domaincfg;
	صحیح locked;

	/* Set domain configuration to 0 */
	writel(0, (خالی *)(aplic->addr + APLIC_DOMAINCFG));

	/* Disable all interrupts */
	برائے (i = 0; i <= aplic->عدد_ماخذ; i += 32)
		writel(-1U, (خالی *)(aplic->addr + APLIC_CLRIE_بنیاد +
				     (i / 32) * حجم(u32)));

	/* Set interrupt type and priority for all interrupts */
	برائے (i = 1; i <= aplic->عدد_ماخذ; i++) {
		/* Set IRQ source configuration to 0 */
		writel(0, (خالی *)(aplic->addr + APLIC_SOURCECFG_بنیاد +
			  (i - 1) * حجم(u32)));
		/* Set IRQ target hart index and priority to 1 */
		writel(APLIC_DEFAULT_ترجیح, (خالی *)(aplic->addr +
						APLIC_ہدف_بنیاد +
						(i - 1) * حجم(u32)));
	}

	/* Configure IRQ delegation */
	برائے (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		اگر (!deleg->پہلا_irq || !deleg->آخری_irq)
			جاری;
		اگر (aplic->عدد_ماخذ < deleg->پہلا_irq ||
		    aplic->عدد_ماخذ < deleg->آخری_irq)
			جاری;
		اگر (deleg->بچہ_اشاریہ > APLIC_SOURCECFG_CHILDIDX_نقاب)
			جاری;
		اگر (deleg->پہلا_irq > deleg->آخری_irq) {
			tmp = deleg->پہلا_irq;
			deleg->پہلا_irq = deleg->آخری_irq;
			deleg->آخری_irq = tmp;
		}
		برائے (j = deleg->پہلا_irq; j <= deleg->آخری_irq; j++)
			aplic_sourcecfg_write(aplic, j, APLIC_SOURCECFG_D | deleg->بچہ_اشاریہ);
	}

	/* Default initialization of IDC structures */
	اگر (aplic_is_براہ_راست_منوال(aplic)) {
		برائے (i = 0; i < aplic->عدد_idc; i++) {
			aplic_idc_write(aplic, i, APLIC_IDC_IDELIVERY,
					APLIC_DISABLE_IDELIVERY);
			aplic_idc_write(aplic, i, APLIC_IDC_IFORCE, 0);
			aplic_idc_write(aplic, i, APLIC_IDC_ITHRESHOLD,
					APLIC_DISABLE_ITHRESHOLD);
		}
	} ورنہ {
		/* MSI configuration */
		locked = readl((خالی *)(aplic->addr + APLIC_MMSICFGADDRH)) & APLIC_xMSICFGADDRH_L;
		اگر (aplic->targets_mmode && aplic->has_msicfg_mmode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_mmode,
						(خالی *)(aplic->addr + APLIC_MMSICFGADDR),
						(خالی *)(aplic->addr + APLIC_MMSICFGADDRH));
		}
		اگر (aplic->targets_mmode && aplic->has_msicfg_smode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_smode,
						(خالی *)(aplic->addr + APLIC_SMSICFGADDR),
						(خالی *)(aplic->addr + APLIC_SMSICFGADDRH));
		}
	}

	domaincfg = APLIC_DOMAINCFG_IE;
	اگر (!aplic_is_براہ_راست_منوال(aplic))
		domaincfg |= APLIC_DOMAINCFG_DM;

	writel(domaincfg, (خالی *)(aplic->addr + APLIC_DOMAINCFG));
}

خالی aplic_reinit_all(خالی)
{
	ساخت aplic_ڈیٹا *aplic;

	sbi_list_for_each_اندراج(aplic, &aplic_list, گرہ)
		aplic_init(aplic);
}

ساکن خالی aplic_writel_msicfg(ساخت aplic_msicfg_ڈیٹا *msicfg,
				خالی *msicfgaddr, خالی *msicfgaddrH)
{
	u32 val;
	غیرعلامتی طویل بنیاد_ppn;

	/* Compute the MSI base PPN */
	بنیاد_ppn = msicfg->بنیاد_addr >> APLIC_x_MSICFGADDR_PPN_سرکاؤ;
	بنیاد_ppn &= ~APLIC_xMSICFGADDR_PPN_HART(msicfg->lhxs);
	بنیاد_ppn &= ~APLIC_xMSICFGADDR_PPN_LHX(msicfg->lhxw, msicfg->lhxs);
	بنیاد_ppn &= ~APLIC_xMSICFGADDR_PPN_HHX(msicfg->hhxw, msicfg->hhxs);

	/* Write the lower MSI config register */
	writel((u32)بنیاد_ppn, msicfgaddr);

	/* Write the upper MSI config register */
	val = (((u64)بنیاد_ppn) >> 32) &
		APLIC_x_MSICFGADDRH_BAPPN_نقاب;
	val |= (msicfg->lhxw & APLIC_x_MSICFGADDRH_LHXW_نقاب)
		<< APLIC_x_MSICFGADDRH_LHXW_سرکاؤ;
	val |= (msicfg->hhxw & APLIC_x_MSICFGADDRH_HHXW_نقاب)
		<< APLIC_x_MSICFGADDRH_HHXW_سرکاؤ;
	val |= (msicfg->lhxs & APLIC_x_MSICFGADDRH_LHXS_نقاب)
		<< APLIC_x_MSICFGADDRH_LHXS_سرکاؤ;
	val |= (msicfg->hhxs & APLIC_x_MSICFGADDRH_HHXS_نقاب)
		<< APLIC_x_MSICFGADDRH_HHXS_سرکاؤ;
	writel(val, msicfgaddrH);
}

ساکن صحیح aplic_check_msicfg(ساخت aplic_msicfg_ڈیٹا *msicfg)
{
	اگر (APLIC_x_MSICFGADDRH_LHXS_نقاب < msicfg->lhxs)
		واپس SBI_EINVAL;

	اگر (APLIC_x_MSICFGADDRH_LHXW_نقاب < msicfg->lhxw)
		واپس SBI_EINVAL;

	اگر (APLIC_x_MSICFGADDRH_HHXS_نقاب < msicfg->hhxs)
		واپس SBI_EINVAL;

	اگر (APLIC_x_MSICFGADDRH_HHXW_نقاب < msicfg->hhxw)
		واپس SBI_EINVAL;

	واپس 0;
}

ساکن صحیح aplic_warm_init(ساخت sbi_irqchip_device *chip)
{
	ساخت aplic_ڈیٹا *aplic;
	u32 hart_اشاریہ;
	صحیح idc_اشاریہ;

	aplic = container_کا(chip, ساخت aplic_ڈیٹا, irqchip);
	اگر (!aplic_is_براہ_راست_منوال(aplic))
		واپس 0;

	hart_اشاریہ = current_hartindex();
	idc_اشاریہ  = aplic_ڈھونڈو_idc_اشاریہ(aplic, hart_اشاریہ);
	اگر (idc_اشاریہ < 0)
		واپس 0;

	aplic_idc_write(aplic, idc_اشاریہ, APLIC_IDC_ITHRESHOLD,
			APLIC_ENABLE_ITHRESHOLD);
	aplic_idc_write(aplic, idc_اشاریہ, APLIC_IDC_IDELIVERY,
			APLIC_ENABLE_IDELIVERY);

	واپس 0;
}

ساکن صحیح aplic_process_hwirqs(ساخت sbi_irqchip_device *chip)
{
	ساخت aplic_ڈیٹا *aplic;
	u32 hart_اشاریہ, claimi, hwirq;
	صحیح idc_اشاریہ, rc = 0, tmp;

	aplic = container_کا(chip, ساخت aplic_ڈیٹا, irqchip);

	اگر (!aplic_is_براہ_راست_منوال(aplic))
		sbi_panic("aplic_process_hwirqs called in MSI mode.\n");

	hart_اشاریہ = current_hartindex();
	idc_اشاریہ  = aplic_ڈھونڈو_idc_اشاریہ(aplic, hart_اشاریہ);
	اگر (idc_اشاریہ < 0)
		واپس 0;

	جب ((claimi = aplic_idc_read(aplic, idc_اشاریہ, APLIC_IDC_CLAIMI))) {
		hwirq = (claimi >> APLIC_IDC_TOPI_ID_سرکاؤ) &
			APLIC_IDC_TOPI_ID_نقاب;

		اگر (!hwirq)
			توڑو;

		اگر (aplic_hwirq_is_delegated(aplic, hwirq))
			جاری;

		tmp = sbi_irqchip_process_hwirq(chip, hwirq);
		اگر (tmp)
			rc = tmp;
	}

	واپس rc;
}

ساکن خالی aplic_hwirq_eoi(ساخت sbi_irqchip_device *chip, u32 hwirq)
{
	ساخت aplic_ڈیٹا *aplic;
	u32 sm;

	aplic = container_کا(chip, ساخت aplic_ڈیٹا, irqchip);
	اگر (aplic_is_براہ_راست_منوال(aplic))
		واپس;

	sm = aplic_sourcecfg_read(aplic, hwirq) & APLIC_SOURCECFG_SM_نقاب;
	اگر (sm == APLIC_SOURCECFG_SM_LEVEL_بلند ||
	    sm == APLIC_SOURCECFG_SM_LEVEL_کم)
		aplic_msi_irq_retrigger_level(aplic, hwirq);
}

ساکن خالی aplic_پروگرام_msi_ہدف(ساخت aplic_ڈیٹا *aplic,
				     u32 hwirq, u32 hart_اشاریہ,
				     u32 guest_اشاریہ, u32 eiid)
{
	u32 ہدف;

	ہدف = APLIC_ہدف_HART_IDX(hart_اشاریہ) |
		 APLIC_ہدف_GUEST_IDX(guest_اشاریہ) |
		 APLIC_ہدف_EIID(eiid);

	aplic_ہدف_write(aplic, hwirq, ہدف);
}

ساکن u32 derive_hart_اشاریہ(ساخت aplic_msicfg_ڈیٹا *msicfg,
			      ثابت ساخت sbi_irqchip_msi_پیغام *پیغام)
{
	u64 addr = ((u64)پیغام->پتہ_hi << 32) | پیغام->پتہ_lo;
	u64 tppn = addr >> APLIC_x_MSICFGADDR_PPN_سرکاؤ;
	u32 گروہ_اشاریہ, hart_اشاریہ;

	گروہ_اشاریہ = (tppn >> APLIC_x_MSICFGADDR_PPN_HHX_سرکاؤ(msicfg->hhxs)) &
		      APLIC_x_MSICFGADDR_PPN_HHX_نقاب(msicfg->hhxw);

	hart_اشاریہ = (tppn >> msicfg->lhxs) &
		     ((1UL << msicfg->lhxw) - 1);

	hart_اشاریہ |= (گروہ_اشاریہ << msicfg->lhxw);

	واپس hart_اشاریہ;
}

ساکن خالی aplic_write_msi(u32 والد_hwirq,
			    ثابت ساخت sbi_irqchip_msi_پیغام *پیغام,
			    خالی *priv)
{
	ساخت aplic_msi_ڈیٹا *msi_ڈیٹا = priv;
	u32 guest_اشاریہ = 0;
	u32 eiid;

	eiid = پیغام->ڈیٹا;
	اگر (!eiid || eiid > APLIC_ہدف_EIID_نقاب)
		واپس;

	aplic_پروگرام_msi_ہدف(msi_ڈیٹا->aplic, msi_ڈیٹا->hwirq,
				derive_hart_اشاریہ(&msi_ڈیٹا->aplic->msicfg_mmode, پیغام),
				guest_اشاریہ, eiid);
}

ساکن صحیح aplic_msi_callback(u32 والد_hwirq, خالی *priv)
{
	ساخت aplic_msi_ڈیٹا *msi_ڈیٹا = priv;
	صحیح rc;

	rc = sbi_irqchip_process_hwirq(&msi_ڈیٹا->aplic->irqchip, msi_ڈیٹا->hwirq);
	اگر (rc && rc != SBI_ENOENT)
		sbi_printf("%s: hwirq=%lu failed rc=%d\n",
			__func__,
			(غیرعلامتی طویل)والد_hwirq, rc);

	واپس rc;
}

ساکن صحیح aplic_تیاری_msi(ساخت aplic_ڈیٹا *aplic, u32 hwirq)
{
	ساخت sbi_irqchip_device *والد;
	u32 پہلا_hwirq;
	ساخت aplic_msi_ڈیٹا *msi_ڈیٹا;
	صحیح rc;

	والد = sbi_irqchip_ڈھونڈو_device(aplic->والد_unique_id);
	اگر (!والد) {
		sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
			__func__, (غیرعلامتی طویل)hwirq);
		واپس SBI_EINVAL;
	}

	msi_ڈیٹا = sbi_zalloc(حجم(ساخت aplic_msi_ڈیٹا));
	اگر (!msi_ڈیٹا)
		واپس SBI_ENOMEM;

	msi_ڈیٹا->aplic = aplic;
	msi_ڈیٹا->hwirq = hwirq;

	rc = sbi_irqchip_رجسٹر_msi(والد, 1,
					aplic_write_msi,
					aplic_msi_callback,
					msi_ڈیٹا,
					&پہلا_hwirq);
	اگر (rc) {
		sbi_printf("%s: register_msi failed hwirq=%lu rc=%d\n",
			__func__, (غیرعلامتی طویل)hwirq, rc);
		sbi_free(msi_ڈیٹا);
		واپس rc;
	}

	msi_ڈیٹا->والد_hwirq = پہلا_hwirq;
	sbi_irqchip_سیٹ_hwirq_priv(&aplic->irqchip, hwirq, msi_ڈیٹا);

	واپس 0;
}

ساکن صحیح aplic_hwirq_تیاری(ساخت sbi_irqchip_device *chip,
			     u32 hwirq, u32 hwirq_flags)
{
	ساخت aplic_ڈیٹا *aplic;
	u32 sourcecfg;

	aplic = container_کا(chip, ساخت aplic_ڈیٹا, irqchip);

	sourcecfg = aplic_hwirq_flags_مقصد_sourcecfg(hwirq_flags);
	اگر (sourcecfg == APLIC_SOURCECFG_SM_INACTIVE) {
		sbi_printf("%s: unsupported flags=0x%lx hwirq=%lu\n",
			__func__,
			(غیرعلامتی طویل)hwirq_flags,
			(غیرعلامتی طویل)hwirq);
		واپس SBI_EINVAL;
	}

	aplic_sourcecfg_write(aplic, hwirq, sourcecfg);

	اگر (aplic_hwirq_is_delegated(aplic, hwirq)) {
		sbi_printf("%s: hwirq=%lu is delegated\n",
			__func__, (غیرعلامتی طویل)hwirq);
		واپس SBI_ENOTSUPP;
	}

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);

	اگر (!aplic_is_براہ_راست_منوال(aplic))
		واپس aplic_تیاری_msi(aplic, hwirq);

	واپس 0;
}

ساکن خالی aplic_hwirq_cleanup(ساخت sbi_irqchip_device *chip, u32 hwirq)
{
	ساخت aplic_ڈیٹا *aplic;
	ساخت aplic_msi_ڈیٹا *msi_ڈیٹا;

	aplic = container_کا(chip, ساخت aplic_ڈیٹا, irqchip);
	اگر (aplic_hwirq_is_delegated(aplic, hwirq))
		واپس;

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);
	aplic_sourcecfg_write(aplic, hwirq, APLIC_SOURCECFG_SM_INACTIVE);
	aplic_ہدف_write(aplic, hwirq, APLIC_DEFAULT_ترجیح);
	msi_ڈیٹا = (ساخت aplic_msi_ڈیٹا *) sbi_irqchip_get_hwirq_priv(chip, hwirq);
	اگر (msi_ڈیٹا)
		sbi_free(msi_ڈیٹا);
}

ساکن صحیح aplic_hwirq_سیٹ_affinity(ساخت sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_اشاریہ)
{
	ساخت aplic_ڈیٹا *aplic;
	ساخت aplic_msi_ڈیٹا *msi_ڈیٹا;
	ساخت sbi_irqchip_device *والد;
	صحیح idc_اشاریہ;
	صحیح rc;

	aplic = container_کا(chip, ساخت aplic_ڈیٹا, irqchip);
	اگر (aplic_hwirq_is_delegated(aplic, hwirq))
		واپس SBI_ENOTSUPP;

	اگر (!aplic_is_براہ_راست_منوال(aplic)) {
		والد = sbi_irqchip_ڈھونڈو_device(aplic->والد_unique_id);
		اگر (!والد) {
			sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
				__func__, (غیرعلامتی طویل)hwirq);
			واپس SBI_EINVAL;
		}

		msi_ڈیٹا = sbi_irqchip_get_hwirq_priv(chip, hwirq);
		rc = sbi_irqchip_سیٹ_affinity(والد, msi_ڈیٹا->والد_hwirq, hart_اشاریہ);
		اگر (rc) {
			sbi_printf("%s: sbi_irqchip_set_affinity failed hwirq=%lu rc=%d\n",
				__func__, (غیرعلامتی طویل)hwirq, rc);
			واپس rc;
		}
		واپس 0;
	}

	idc_اشاریہ = aplic_ڈھونڈو_idc_اشاریہ(aplic, hart_اشاریہ);
	اگر (idc_اشاریہ < 0)
		واپس SBI_EINVAL;

	aplic_ہدف_write(aplic, hwirq,
			   APLIC_ہدف_HART_IDX(hart_اشاریہ) |
			   APLIC_ہدف_IPRIO(APLIC_DEFAULT_ترجیح));

	واپس 0;
}

ساکن خالی aplic_hwirq_نقاب(ساخت sbi_irqchip_device *chip, u32 hwirq)
{
	ساخت aplic_ڈیٹا *aplic;

	aplic = container_کا(chip, ساخت aplic_ڈیٹا, irqchip);
	اگر (aplic_hwirq_is_delegated(aplic, hwirq))
		واپس;

	aplic_irq_clrie(aplic, hwirq);
}

ساکن خالی aplic_hwirq_unmask(ساخت sbi_irqchip_device *chip, u32 hwirq)
{
	ساخت aplic_ڈیٹا *aplic;

	aplic = container_کا(chip, ساخت aplic_ڈیٹا, irqchip);
	اگر (aplic_hwirq_is_delegated(aplic, hwirq))
		واپس;

	aplic_irq_setie(aplic, hwirq);
}

ساکن ساخت sbi_irqchip_device aplic_irqchip_template = {
	.warm_init			= aplic_warm_init,
	.process_hwirqs		= aplic_process_hwirqs,
	.hwirq_eoi			= aplic_hwirq_eoi,
	.hwirq_تیاری		= aplic_hwirq_تیاری,
	.hwirq_cleanup		= aplic_hwirq_cleanup,
	.hwirq_سیٹ_affinity	= aplic_hwirq_سیٹ_affinity,
	.hwirq_نقاب			= aplic_hwirq_نقاب,
	.hwirq_unmask		= aplic_hwirq_unmask,
};

صحیح aplic_cold_irqchip_init(ساخت aplic_ڈیٹا *aplic)
{
	صحیح rc;
	ساخت aplic_delegate_ڈیٹا *deleg;
	u32 پہلا_deleg_irq, آخری_deleg_irq, i;
	bool msi_منوال;

	msi_منوال = !aplic_is_براہ_راست_منوال(aplic);
	اگر (!aplic->عدد_ماخذ) {
		sbi_printf("%s: num_source is zero\n", __func__);
		واپس SBI_EINVAL;
	}

	اگر (APLIC_MAX_ماخذ <= aplic->عدد_ماخذ) {
		sbi_printf("%s: num_source=%lu exceeds max=%lu\n",
			__func__,
			(غیرعلامتی طویل)aplic->عدد_ماخذ,
			(غیرعلامتی طویل)APLIC_MAX_ماخذ);
		واپس SBI_EINVAL;
	}

	اگر (!msi_منوال && APLIC_MAX_IDC <= aplic->عدد_idc) {
		sbi_printf("%s: num_idc=%lu exceeds max=%lu\n",
			__func__,
			(غیرعلامتی طویل)aplic->عدد_idc,
			(غیرعلامتی طویل)APLIC_MAX_IDC);
		واپس SBI_EINVAL;
	}

	اگر (aplic->targets_mmode && aplic->has_msicfg_mmode) {
		rc = aplic_check_msicfg(&aplic->msicfg_mmode);
		اگر (rc) {
			sbi_printf("%s: invalid M-mode msicfg rc=%d\n",
				__func__, rc);
			واپس rc;
		}
	}

	اگر (aplic->targets_mmode && aplic->has_msicfg_smode) {
		rc = aplic_check_msicfg(&aplic->msicfg_smode);
		اگر (rc) {
			sbi_printf("%s: invalid S-mode msicfg rc=%d\n",
				__func__, rc);
			واپس rc;
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
	برائے (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		اگر (deleg->پہلا_irq < پہلا_deleg_irq)
			پہلا_deleg_irq = deleg->پہلا_irq;
		اگر (آخری_deleg_irq < deleg->آخری_irq)
			آخری_deleg_irq = deleg->آخری_irq;
	}

	اگر (aplic->targets_mmode ||
	    ((پہلا_deleg_irq < آخری_deleg_irq) &&
	    (آخری_deleg_irq == aplic->عدد_ماخذ) &&
	    (پہلا_deleg_irq == 1))) {
		rc = sbi_domain_جڑ_جوڑو_memrange(aplic->addr, aplic->سائز, صفحہ_سائز,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		اگر (rc)
			واپس rc;
	}

	اگر ((aplic->targets_mmode)) {
		aplic->irqchip		 = aplic_irqchip_template;
		aplic->irqchip.id	 = aplic->unique_id;
		aplic->irqchip.caps = SBI_IRQCHIP_CAPS_WIRED;
		aplic->irqchip.عدد_hwirq = aplic->عدد_ماخذ + 1;

		اگر (msi_منوال) {
			aplic->irqchip.warm_init      = تہی;
			aplic->irqchip.process_hwirqs = تہی;
			aplic->irqchip.hwirq_eoi      = تہی;
		}

		اگر (msi_منوال)
			sbi_hartmask_سیٹ_all(&aplic->irqchip.ہدف_harts);
		ورنہ
			برائے (i = 0; i < aplic->عدد_idc; i++)
				sbi_hartmask_سیٹ_hartindex(aplic->idc_map[i],
							   &aplic->irqchip.ہدف_harts);

		rc = sbi_irqchip_جوڑو_device(&aplic->irqchip);
		اگر (rc) {
			sbi_printf("%s: sbi_irqchip_add_device failed rc=%d id=%lu mode=%s target_weight=%lu\n",
				__func__, rc,
				(غیرعلامتی طویل)aplic->irqchip.id,
				msi_منوال ? "msi" : "direct",
				(غیرعلامتی طویل)sbi_hartmask_وزن(
					&aplic->irqchip.ہدف_harts));
			واپس rc;
		}
	}
	/* Attach to the aplic list */
	sbi_list_جوڑو_دم(&aplic->گرہ, &aplic_list);

	واپس 0;
}
