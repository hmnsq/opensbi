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

#تعریف_کړه APLIC_MAX_IDC			(1UL << 14)
#تعریف_کړه APLIC_MAX_سرچینه		1024

#تعریف_کړه APLIC_DOMAINCFG		0x0000
#تعریف_کړه APLIC_DOMAINCFG_IE		(1 << 8)
#تعریف_کړه APLIC_DOMAINCFG_DM		(1 << 2)
#تعریف_کړه APLIC_DOMAINCFG_BE		(1 << 0)

#تعریف_کړه APLIC_SOURCECFG_بنسټ		0x0004
#تعریف_کړه APLIC_SOURCECFG_D		(1 << 10)
#تعریف_کړه APLIC_SOURCECFG_CHILDIDX_ماسک	0x000003ff
#تعریف_کړه APLIC_SOURCECFG_SM_ماسک	0x00000007
#تعریف_کړه APLIC_SOURCECFG_SM_INACTIVE	0x0
#تعریف_کړه APLIC_SOURCECFG_SM_DETACH	0x1
#تعریف_کړه APLIC_SOURCECFG_SM_څنډه_RISE	0x4
#تعریف_کړه APLIC_SOURCECFG_SM_څنډه_FALL	0x5
#تعریف_کړه APLIC_SOURCECFG_SM_LEVEL_لوړ	0x6
#تعریف_کړه APLIC_SOURCECFG_SM_LEVEL_ټیټ	0x7

#تعریف_کړه APLIC_MMSICFGADDR		0x1bc0
#تعریف_کړه APLIC_MMSICFGADDRH		0x1bc4
#تعریف_کړه APLIC_SMSICFGADDR		0x1bc8
#تعریف_کړه APLIC_SMSICFGADDRH		0x1bcc

#تعریف_کړه APLIC_xMSICFGADDRH_L		(1UL << 31)
#تعریف_کړه APLIC_x_MSICFGADDRH_HHXS_ماسک	0x1f
#تعریف_کړه APLIC_x_MSICFGADDRH_HHXS_ښويدنه	24
#تعریف_کړه APLIC_x_MSICFGADDRH_LHXS_ماسک	0x7
#تعریف_کړه APLIC_x_MSICFGADDRH_LHXS_ښويدنه	20
#تعریف_کړه APLIC_x_MSICFGADDRH_HHXW_ماسک	0x7
#تعریف_کړه APLIC_x_MSICFGADDRH_HHXW_ښويدنه	16
#تعریف_کړه APLIC_x_MSICFGADDRH_LHXW_ماسک	0xf
#تعریف_کړه APLIC_x_MSICFGADDRH_LHXW_ښويدنه	12
#تعریف_کړه APLIC_x_MSICFGADDRH_BAPPN_ماسک	0xfff

#تعریف_کړه APLIC_x_MSICFGADDR_PPN_ښويدنه	12

#تعریف_کړه APLIC_xMSICFGADDR_PPN_HART(__lhxs) \
	((1UL << (__lhxs)) - 1)

#تعریف_کړه APLIC_x_MSICFGADDR_PPN_LHX_ماسک(__lhxw) \
	((1UL << (__lhxw)) - 1)
#تعریف_کړه APLIC_x_MSICFGADDR_PPN_LHX_ښويدنه(__lhxs) \
	((__lhxs))
#تعریف_کړه APLIC_xMSICFGADDR_PPN_LHX(__lhxw, __lhxs) \
	(APLIC_x_MSICFGADDR_PPN_LHX_ماسک(__lhxw) << \
	 APLIC_x_MSICFGADDR_PPN_LHX_ښويدنه(__lhxs))

#تعریف_کړه APLIC_x_MSICFGADDR_PPN_HHX_ماسک(__hhxw) \
	((1UL << (__hhxw)) - 1)
#تعریف_کړه APLIC_x_MSICFGADDR_PPN_HHX_ښويدنه(__hhxs) \
	((__hhxs) + APLIC_x_MSICFGADDR_PPN_ښويدنه)
#تعریف_کړه APLIC_xMSICFGADDR_PPN_HHX(__hhxw, __hhxs) \
	(APLIC_x_MSICFGADDR_PPN_HHX_ماسک(__hhxw) << \
	 APLIC_x_MSICFGADDR_PPN_HHX_ښويدنه(__hhxs))

#تعریف_کړه APLIC_موخه_GUEST_IDX(__gidx) \
	((((u32)(__gidx)) & APLIC_موخه_GUEST_IDX_ماسک) << \
	 APLIC_موخه_GUEST_IDX_ښويدنه)
#تعریف_کړه APLIC_موخه_EIID(__eiid) \
	(((u32)(__eiid)) & APLIC_موخه_EIID_ماسک)

#تعریف_کړه APLIC_SETIP_بنسټ		0x1c00
#تعریف_کړه APLIC_SETIPNUM			0x1cdc

#تعریف_کړه APLIC_CLRIP_بنسټ		0x1d00
#تعریف_کړه APLIC_CLRIPNUM			0x1ddc

#تعریف_کړه APLIC_SETIE_بنسټ		0x1e00
#تعریف_کړه APLIC_SETIENUM			0x1edc

#تعریف_کړه APLIC_CLRIE_بنسټ		0x1f00
#تعریف_کړه APLIC_CLRIENUM			0x1fdc

#تعریف_کړه APLIC_SETIPNUM_LE		0x2000
#تعریف_کړه APLIC_SETIPNUM_BE		0x2004

#تعریف_کړه APLIC_موخه_بنسټ		0x3004
#تعریف_کړه APLIC_موخه_HART_IDX_ښويدنه	18
#تعریف_کړه APLIC_موخه_HART_IDX_ماسک	0x3fff
#تعریف_کړه APLIC_موخه_GUEST_IDX_ښويدنه	12
#تعریف_کړه APLIC_موخه_GUEST_IDX_ماسک	0x3f
#تعریف_کړه APLIC_موخه_IPRIO_ماسک	0xff
#تعریف_کړه APLIC_موخه_EIID_ماسک	0x7ff

#تعریف_کړه APLIC_موخه_HART_IDX(__hidx) \
	((((u32)(__hidx)) & APLIC_موخه_HART_IDX_ماسک) << \
	 APLIC_موخه_HART_IDX_ښويدنه)
#تعریف_کړه APLIC_موخه_IPRIO(__prio) \
	(((u32)(__prio)) & APLIC_موخه_IPRIO_ماسک)

#تعریف_کړه APLIC_IDC_بنسټ			0x4000
#تعریف_کړه APLIC_IDC_اندازه			32

#تعریف_کړه APLIC_IDC_IDELIVERY		0x00

#تعریف_کړه APLIC_IDC_IFORCE		0x04

#تعریف_کړه APLIC_IDC_ITHRESHOLD		0x08

#تعریف_کړه APLIC_IDC_TOPI			0x18
#تعریف_کړه APLIC_IDC_TOPI_ID_ښويدنه	16
#تعریف_کړه APLIC_IDC_TOPI_ID_ماسک	0x3ff
#تعریف_کړه APLIC_IDC_TOPI_PRIO_ماسک	0xff

#تعریف_کړه APLIC_IDC_CLAIMI		0x1c

#تعریف_کړه APLIC_DEFAULT_لومړیتوب		1
#تعریف_کړه APLIC_DISABLE_IDELIVERY		0
#تعریف_کړه APLIC_ENABLE_IDELIVERY		1
#تعریف_کړه APLIC_DISABLE_ITHRESHOLD	1
#تعریف_کړه APLIC_ENABLE_ITHRESHOLD		0

بنية aplic_msi_معلومات {
	بنية aplic_معلومات *aplic;
	u32 hwirq;
	u32 پلار_hwirq;
};

ساكن SBI_LIST_سر(aplic_list);
ساكن فراغ aplic_writel_msicfg(بنية aplic_msicfg_معلومات *msicfg,
				فراغ *msicfgaddr, فراغ *msicfgaddrH);

ساكن مضمن bool aplic_is_مستقیم_ډېرپېښ(بنية aplic_معلومات *aplic)
{
	أرجع aplic && aplic->عدد_idc;
}

ساكن مضمن فراغ aplic_sourcecfg_write(بنية aplic_معلومات *aplic,
					 u32 hwirq, u32 val)
{
	writel(val, (فراغ *)(aplic->addr + APLIC_SOURCECFG_بنسټ +
			     (hwirq - 1) * حجم(u32)));
}

ساكن مضمن u32 aplic_sourcecfg_read(بنية aplic_معلومات *aplic, u32 hwirq)
{
	أرجع readl((فراغ *)(aplic->addr + APLIC_SOURCECFG_بنسټ +
			      (hwirq - 1) * حجم(u32)));
}

ساكن مضمن فراغ aplic_موخه_write(بنية aplic_معلومات *aplic,
				      u32 hwirq, u32 val)
{
	writel(val, (فراغ *)(aplic->addr + APLIC_موخه_بنسټ +
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

ساكن مضمن فراغ *aplic_idc_بنسټ(بنية aplic_معلومات *aplic, u32 idc_شاخص)
{
	أرجع (فراغ *)(aplic->addr + APLIC_IDC_بنسټ +
			idc_شاخص * APLIC_IDC_اندازه);
}

ساكن مضمن u32 aplic_idc_read(بنية aplic_معلومات *aplic, u32 idc_شاخص,
				 u32 reg)
{
	أرجع readl(aplic_idc_بنسټ(aplic, idc_شاخص) + reg);
}

ساكن مضمن فراغ aplic_idc_write(بنية aplic_معلومات *aplic, u32 idc_شاخص,
				   u32 reg, u32 val)
{
	writel(val, aplic_idc_بنسټ(aplic, idc_شاخص) + reg);
}

ساكن u32 aplic_hwirq_flags_منزل_sourcecfg(u32 hwirq_flags)
{
	بدل (hwirq_flags & SBI_HWIRQ_FLAGS_LEVEL_SENSE_ماسک) {
	حالة SBI_HWIRQ_FLAGS_څنډه_RISING:
		أرجع APLIC_SOURCECFG_SM_څنډه_RISE;
	حالة SBI_HWIRQ_FLAGS_څنډه_FALLING:
		أرجع APLIC_SOURCECFG_SM_څنډه_FALL;
	حالة SBI_HWIRQ_FLAGS_LEVEL_لوړ:
		أرجع APLIC_SOURCECFG_SM_LEVEL_لوړ;
	حالة SBI_HWIRQ_FLAGS_LEVEL_ټیټ:
		أرجع APLIC_SOURCECFG_SM_LEVEL_ټیټ;
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

ساكن صحيح aplic_پیدا_کړه_idc_شاخص(بنية aplic_معلومات *aplic, u32 hart_شاخص)
{
	u32 i;

	لكل (i = 0; i < aplic->عدد_idc; i++) {
		إذا (aplic->idc_map[i] == hart_شاخص)
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
	لكل (i = 0; i <= aplic->عدد_سرچینه; i += 32)
		writel(-1U, (فراغ *)(aplic->addr + APLIC_CLRIE_بنسټ +
				     (i / 32) * حجم(u32)));

	/* Set interrupt type and priority for all interrupts */
	لكل (i = 1; i <= aplic->عدد_سرچینه; i++) {
		/* Set IRQ source configuration to 0 */
		writel(0, (فراغ *)(aplic->addr + APLIC_SOURCECFG_بنسټ +
			  (i - 1) * حجم(u32)));
		/* Set IRQ target hart index and priority to 1 */
		writel(APLIC_DEFAULT_لومړیتوب, (فراغ *)(aplic->addr +
						APLIC_موخه_بنسټ +
						(i - 1) * حجم(u32)));
	}

	/* Configure IRQ delegation */
	لكل (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		إذا (!deleg->لومړی_irq || !deleg->وروستی_irq)
			استمر;
		إذا (aplic->عدد_سرچینه < deleg->لومړی_irq ||
		    aplic->عدد_سرچینه < deleg->وروستی_irq)
			استمر;
		إذا (deleg->ماشوم_شاخص > APLIC_SOURCECFG_CHILDIDX_ماسک)
			استمر;
		إذا (deleg->لومړی_irq > deleg->وروستی_irq) {
			tmp = deleg->لومړی_irq;
			deleg->لومړی_irq = deleg->وروستی_irq;
			deleg->وروستی_irq = tmp;
		}
		لكل (j = deleg->لومړی_irq; j <= deleg->وروستی_irq; j++)
			aplic_sourcecfg_write(aplic, j, APLIC_SOURCECFG_D | deleg->ماشوم_شاخص);
	}

	/* Default initialization of IDC structures */
	إذا (aplic_is_مستقیم_ډېرپېښ(aplic)) {
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
	إذا (!aplic_is_مستقیم_ډېرپېښ(aplic))
		domaincfg |= APLIC_DOMAINCFG_DM;

	writel(domaincfg, (فراغ *)(aplic->addr + APLIC_DOMAINCFG));
}

فراغ aplic_reinit_all(فراغ)
{
	بنية aplic_معلومات *aplic;

	sbi_list_for_each_ثبت(aplic, &aplic_list, غوټه)
		aplic_init(aplic);
}

ساكن فراغ aplic_writel_msicfg(بنية aplic_msicfg_معلومات *msicfg,
				فراغ *msicfgaddr, فراغ *msicfgaddrH)
{
	u32 val;
	بینښه اوږد بنسټ_ppn;

	/* Compute the MSI base PPN */
	بنسټ_ppn = msicfg->بنسټ_addr >> APLIC_x_MSICFGADDR_PPN_ښويدنه;
	بنسټ_ppn &= ~APLIC_xMSICFGADDR_PPN_HART(msicfg->lhxs);
	بنسټ_ppn &= ~APLIC_xMSICFGADDR_PPN_LHX(msicfg->lhxw, msicfg->lhxs);
	بنسټ_ppn &= ~APLIC_xMSICFGADDR_PPN_HHX(msicfg->hhxw, msicfg->hhxs);

	/* Write the lower MSI config register */
	writel((u32)بنسټ_ppn, msicfgaddr);

	/* Write the upper MSI config register */
	val = (((u64)بنسټ_ppn) >> 32) &
		APLIC_x_MSICFGADDRH_BAPPN_ماسک;
	val |= (msicfg->lhxw & APLIC_x_MSICFGADDRH_LHXW_ماسک)
		<< APLIC_x_MSICFGADDRH_LHXW_ښويدنه;
	val |= (msicfg->hhxw & APLIC_x_MSICFGADDRH_HHXW_ماسک)
		<< APLIC_x_MSICFGADDRH_HHXW_ښويدنه;
	val |= (msicfg->lhxs & APLIC_x_MSICFGADDRH_LHXS_ماسک)
		<< APLIC_x_MSICFGADDRH_LHXS_ښويدنه;
	val |= (msicfg->hhxs & APLIC_x_MSICFGADDRH_HHXS_ماسک)
		<< APLIC_x_MSICFGADDRH_HHXS_ښويدنه;
	writel(val, msicfgaddrH);
}

ساكن صحيح aplic_check_msicfg(بنية aplic_msicfg_معلومات *msicfg)
{
	إذا (APLIC_x_MSICFGADDRH_LHXS_ماسک < msicfg->lhxs)
		أرجع SBI_EINVAL;

	إذا (APLIC_x_MSICFGADDRH_LHXW_ماسک < msicfg->lhxw)
		أرجع SBI_EINVAL;

	إذا (APLIC_x_MSICFGADDRH_HHXS_ماسک < msicfg->hhxs)
		أرجع SBI_EINVAL;

	إذا (APLIC_x_MSICFGADDRH_HHXW_ماسک < msicfg->hhxw)
		أرجع SBI_EINVAL;

	أرجع 0;
}

ساكن صحيح aplic_warm_init(بنية sbi_irqchip_device *chip)
{
	بنية aplic_معلومات *aplic;
	u32 hart_شاخص;
	صحيح idc_شاخص;

	aplic = container_د(chip, بنية aplic_معلومات, irqchip);
	إذا (!aplic_is_مستقیم_ډېرپېښ(aplic))
		أرجع 0;

	hart_شاخص = current_hartindex();
	idc_شاخص  = aplic_پیدا_کړه_idc_شاخص(aplic, hart_شاخص);
	إذا (idc_شاخص < 0)
		أرجع 0;

	aplic_idc_write(aplic, idc_شاخص, APLIC_IDC_ITHRESHOLD,
			APLIC_ENABLE_ITHRESHOLD);
	aplic_idc_write(aplic, idc_شاخص, APLIC_IDC_IDELIVERY,
			APLIC_ENABLE_IDELIVERY);

	أرجع 0;
}

ساكن صحيح aplic_process_hwirqs(بنية sbi_irqchip_device *chip)
{
	بنية aplic_معلومات *aplic;
	u32 hart_شاخص, claimi, hwirq;
	صحيح idc_شاخص, rc = 0, tmp;

	aplic = container_د(chip, بنية aplic_معلومات, irqchip);

	إذا (!aplic_is_مستقیم_ډېرپېښ(aplic))
		sbi_panic("aplic_process_hwirqs called in MSI mode.\n");

	hart_شاخص = current_hartindex();
	idc_شاخص  = aplic_پیدا_کړه_idc_شاخص(aplic, hart_شاخص);
	إذا (idc_شاخص < 0)
		أرجع 0;

	طالما ((claimi = aplic_idc_read(aplic, idc_شاخص, APLIC_IDC_CLAIMI))) {
		hwirq = (claimi >> APLIC_IDC_TOPI_ID_ښويدنه) &
			APLIC_IDC_TOPI_ID_ماسک;

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

	aplic = container_د(chip, بنية aplic_معلومات, irqchip);
	إذا (aplic_is_مستقیم_ډېرپېښ(aplic))
		أرجع;

	sm = aplic_sourcecfg_read(aplic, hwirq) & APLIC_SOURCECFG_SM_ماسک;
	إذا (sm == APLIC_SOURCECFG_SM_LEVEL_لوړ ||
	    sm == APLIC_SOURCECFG_SM_LEVEL_ټیټ)
		aplic_msi_irq_retrigger_level(aplic, hwirq);
}

ساكن فراغ aplic_پروګرام_msi_موخه(بنية aplic_معلومات *aplic,
				     u32 hwirq, u32 hart_شاخص,
				     u32 guest_شاخص, u32 eiid)
{
	u32 موخه;

	موخه = APLIC_موخه_HART_IDX(hart_شاخص) |
		 APLIC_موخه_GUEST_IDX(guest_شاخص) |
		 APLIC_موخه_EIID(eiid);

	aplic_موخه_write(aplic, hwirq, موخه);
}

ساكن u32 derive_hart_شاخص(بنية aplic_msicfg_معلومات *msicfg,
			      ثابت بنية sbi_irqchip_msi_پیغام *پیغام)
{
	u64 addr = ((u64)پیغام->پته_hi << 32) | پیغام->پته_lo;
	u64 tppn = addr >> APLIC_x_MSICFGADDR_PPN_ښويدنه;
	u32 ډله_شاخص, hart_شاخص;

	ډله_شاخص = (tppn >> APLIC_x_MSICFGADDR_PPN_HHX_ښويدنه(msicfg->hhxs)) &
		      APLIC_x_MSICFGADDR_PPN_HHX_ماسک(msicfg->hhxw);

	hart_شاخص = (tppn >> msicfg->lhxs) &
		     ((1UL << msicfg->lhxw) - 1);

	hart_شاخص |= (ډله_شاخص << msicfg->lhxw);

	أرجع hart_شاخص;
}

ساكن فراغ aplic_write_msi(u32 پلار_hwirq,
			    ثابت بنية sbi_irqchip_msi_پیغام *پیغام,
			    فراغ *priv)
{
	بنية aplic_msi_معلومات *msi_معلومات = priv;
	u32 guest_شاخص = 0;
	u32 eiid;

	eiid = پیغام->معلومات;
	إذا (!eiid || eiid > APLIC_موخه_EIID_ماسک)
		أرجع;

	aplic_پروګرام_msi_موخه(msi_معلومات->aplic, msi_معلومات->hwirq,
				derive_hart_شاخص(&msi_معلومات->aplic->msicfg_mmode, پیغام),
				guest_شاخص, eiid);
}

ساكن صحيح aplic_msi_callback(u32 پلار_hwirq, فراغ *priv)
{
	بنية aplic_msi_معلومات *msi_معلومات = priv;
	صحيح rc;

	rc = sbi_irqchip_process_hwirq(&msi_معلومات->aplic->irqchip, msi_معلومات->hwirq);
	إذا (rc && rc != SBI_ENOENT)
		sbi_printf("%s: hwirq=%lu failed rc=%d\n",
			__func__,
			(بینښه اوږد)پلار_hwirq, rc);

	أرجع rc;
}

ساكن صحيح aplic_تیاری_msi(بنية aplic_معلومات *aplic, u32 hwirq)
{
	بنية sbi_irqchip_device *پلار;
	u32 لومړی_hwirq;
	بنية aplic_msi_معلومات *msi_معلومات;
	صحيح rc;

	پلار = sbi_irqchip_پیدا_کړه_device(aplic->پلار_unique_id);
	إذا (!پلار) {
		sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
			__func__, (بینښه اوږد)hwirq);
		أرجع SBI_EINVAL;
	}

	msi_معلومات = sbi_zalloc(حجم(بنية aplic_msi_معلومات));
	إذا (!msi_معلومات)
		أرجع SBI_ENOMEM;

	msi_معلومات->aplic = aplic;
	msi_معلومات->hwirq = hwirq;

	rc = sbi_irqchip_ثبتوونکی_msi(پلار, 1,
					aplic_write_msi,
					aplic_msi_callback,
					msi_معلومات,
					&لومړی_hwirq);
	إذا (rc) {
		sbi_printf("%s: register_msi failed hwirq=%lu rc=%d\n",
			__func__, (بینښه اوږد)hwirq, rc);
		sbi_free(msi_معلومات);
		أرجع rc;
	}

	msi_معلومات->پلار_hwirq = لومړی_hwirq;
	sbi_irqchip_سېټ_hwirq_priv(&aplic->irqchip, hwirq, msi_معلومات);

	أرجع 0;
}

ساكن صحيح aplic_hwirq_تیاری(بنية sbi_irqchip_device *chip,
			     u32 hwirq, u32 hwirq_flags)
{
	بنية aplic_معلومات *aplic;
	u32 sourcecfg;

	aplic = container_د(chip, بنية aplic_معلومات, irqchip);

	sourcecfg = aplic_hwirq_flags_منزل_sourcecfg(hwirq_flags);
	إذا (sourcecfg == APLIC_SOURCECFG_SM_INACTIVE) {
		sbi_printf("%s: unsupported flags=0x%lx hwirq=%lu\n",
			__func__,
			(بینښه اوږد)hwirq_flags,
			(بینښه اوږد)hwirq);
		أرجع SBI_EINVAL;
	}

	aplic_sourcecfg_write(aplic, hwirq, sourcecfg);

	إذا (aplic_hwirq_is_delegated(aplic, hwirq)) {
		sbi_printf("%s: hwirq=%lu is delegated\n",
			__func__, (بینښه اوږد)hwirq);
		أرجع SBI_ENOTSUPP;
	}

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);

	إذا (!aplic_is_مستقیم_ډېرپېښ(aplic))
		أرجع aplic_تیاری_msi(aplic, hwirq);

	أرجع 0;
}

ساكن فراغ aplic_hwirq_cleanup(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية aplic_معلومات *aplic;
	بنية aplic_msi_معلومات *msi_معلومات;

	aplic = container_د(chip, بنية aplic_معلومات, irqchip);
	إذا (aplic_hwirq_is_delegated(aplic, hwirq))
		أرجع;

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);
	aplic_sourcecfg_write(aplic, hwirq, APLIC_SOURCECFG_SM_INACTIVE);
	aplic_موخه_write(aplic, hwirq, APLIC_DEFAULT_لومړیتوب);
	msi_معلومات = (بنية aplic_msi_معلومات *) sbi_irqchip_get_hwirq_priv(chip, hwirq);
	إذا (msi_معلومات)
		sbi_free(msi_معلومات);
}

ساكن صحيح aplic_hwirq_سېټ_affinity(بنية sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_شاخص)
{
	بنية aplic_معلومات *aplic;
	بنية aplic_msi_معلومات *msi_معلومات;
	بنية sbi_irqchip_device *پلار;
	صحيح idc_شاخص;
	صحيح rc;

	aplic = container_د(chip, بنية aplic_معلومات, irqchip);
	إذا (aplic_hwirq_is_delegated(aplic, hwirq))
		أرجع SBI_ENOTSUPP;

	إذا (!aplic_is_مستقیم_ډېرپېښ(aplic)) {
		پلار = sbi_irqchip_پیدا_کړه_device(aplic->پلار_unique_id);
		إذا (!پلار) {
			sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
				__func__, (بینښه اوږد)hwirq);
			أرجع SBI_EINVAL;
		}

		msi_معلومات = sbi_irqchip_get_hwirq_priv(chip, hwirq);
		rc = sbi_irqchip_سېټ_affinity(پلار, msi_معلومات->پلار_hwirq, hart_شاخص);
		إذا (rc) {
			sbi_printf("%s: sbi_irqchip_set_affinity failed hwirq=%lu rc=%d\n",
				__func__, (بینښه اوږد)hwirq, rc);
			أرجع rc;
		}
		أرجع 0;
	}

	idc_شاخص = aplic_پیدا_کړه_idc_شاخص(aplic, hart_شاخص);
	إذا (idc_شاخص < 0)
		أرجع SBI_EINVAL;

	aplic_موخه_write(aplic, hwirq,
			   APLIC_موخه_HART_IDX(hart_شاخص) |
			   APLIC_موخه_IPRIO(APLIC_DEFAULT_لومړیتوب));

	أرجع 0;
}

ساكن فراغ aplic_hwirq_ماسک(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية aplic_معلومات *aplic;

	aplic = container_د(chip, بنية aplic_معلومات, irqchip);
	إذا (aplic_hwirq_is_delegated(aplic, hwirq))
		أرجع;

	aplic_irq_clrie(aplic, hwirq);
}

ساكن فراغ aplic_hwirq_unmask(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية aplic_معلومات *aplic;

	aplic = container_د(chip, بنية aplic_معلومات, irqchip);
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
	.hwirq_سېټ_affinity	= aplic_hwirq_سېټ_affinity,
	.hwirq_ماسک			= aplic_hwirq_ماسک,
	.hwirq_unmask		= aplic_hwirq_unmask,
};

صحيح aplic_cold_irqchip_init(بنية aplic_معلومات *aplic)
{
	صحيح rc;
	بنية aplic_delegate_معلومات *deleg;
	u32 لومړی_deleg_irq, وروستی_deleg_irq, i;
	bool msi_ډېرپېښ;

	msi_ډېرپېښ = !aplic_is_مستقیم_ډېرپېښ(aplic);
	إذا (!aplic->عدد_سرچینه) {
		sbi_printf("%s: num_source is zero\n", __func__);
		أرجع SBI_EINVAL;
	}

	إذا (APLIC_MAX_سرچینه <= aplic->عدد_سرچینه) {
		sbi_printf("%s: num_source=%lu exceeds max=%lu\n",
			__func__,
			(بینښه اوږد)aplic->عدد_سرچینه,
			(بینښه اوږد)APLIC_MAX_سرچینه);
		أرجع SBI_EINVAL;
	}

	إذا (!msi_ډېرپېښ && APLIC_MAX_IDC <= aplic->عدد_idc) {
		sbi_printf("%s: num_idc=%lu exceeds max=%lu\n",
			__func__,
			(بینښه اوږد)aplic->عدد_idc,
			(بینښه اوږد)APLIC_MAX_IDC);
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
	لومړی_deleg_irq = -1U;
	وروستی_deleg_irq = 0;
	لكل (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		إذا (deleg->لومړی_irq < لومړی_deleg_irq)
			لومړی_deleg_irq = deleg->لومړی_irq;
		إذا (وروستی_deleg_irq < deleg->وروستی_irq)
			وروستی_deleg_irq = deleg->وروستی_irq;
	}

	إذا (aplic->targets_mmode ||
	    ((لومړی_deleg_irq < وروستی_deleg_irq) &&
	    (وروستی_deleg_irq == aplic->عدد_سرچینه) &&
	    (لومړی_deleg_irq == 1))) {
		rc = sbi_domain_ریښه_جمع_کړه_memrange(aplic->addr, aplic->اندازه, پاڼه_اندازه,
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
		aplic->irqchip.عدد_hwirq = aplic->عدد_سرچینه + 1;

		إذا (msi_ډېرپېښ) {
			aplic->irqchip.warm_init      = عدم;
			aplic->irqchip.process_hwirqs = عدم;
			aplic->irqchip.hwirq_eoi      = عدم;
		}

		إذا (msi_ډېرپېښ)
			sbi_hartmask_سېټ_all(&aplic->irqchip.موخه_harts);
		وإلا
			لكل (i = 0; i < aplic->عدد_idc; i++)
				sbi_hartmask_سېټ_hartindex(aplic->idc_map[i],
							   &aplic->irqchip.موخه_harts);

		rc = sbi_irqchip_جمع_کړه_device(&aplic->irqchip);
		إذا (rc) {
			sbi_printf("%s: sbi_irqchip_add_device failed rc=%d id=%lu mode=%s target_weight=%lu\n",
				__func__, rc,
				(بینښه اوږد)aplic->irqchip.id,
				msi_ډېرپېښ ? "msi" : "direct",
				(بینښه اوږد)sbi_hartmask_وزن(
					&aplic->irqchip.موخه_harts));
			أرجع rc;
		}
	}
	/* Attach to the aplic list */
	sbi_list_جمع_کړه_لکۍ(&aplic->غوټه, &aplic_list);

	أرجع 0;
}
