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

#تعریف_کن APLIC_MAX_IDC			(1UL << 14)
#تعریف_کن APLIC_MAX_منبع		1024

#تعریف_کن APLIC_DOMAINCFG		0x0000
#تعریف_کن APLIC_DOMAINCFG_IE		(1 << 8)
#تعریف_کن APLIC_DOMAINCFG_DM		(1 << 2)
#تعریف_کن APLIC_DOMAINCFG_BE		(1 << 0)

#تعریف_کن APLIC_SOURCECFG_پایه		0x0004
#تعریف_کن APLIC_SOURCECFG_D		(1 << 10)
#تعریف_کن APLIC_SOURCECFG_CHILDIDX_ماسک	0x000003ff
#تعریف_کن APLIC_SOURCECFG_SM_ماسک	0x00000007
#تعریف_کن APLIC_SOURCECFG_SM_INACTIVE	0x0
#تعریف_کن APLIC_SOURCECFG_SM_DETACH	0x1
#تعریف_کن APLIC_SOURCECFG_SM_یال_RISE	0x4
#تعریف_کن APLIC_SOURCECFG_SM_یال_FALL	0x5
#تعریف_کن APLIC_SOURCECFG_SM_LEVEL_بالا	0x6
#تعریف_کن APLIC_SOURCECFG_SM_LEVEL_کم	0x7

#تعریف_کن APLIC_MMSICFGADDR		0x1bc0
#تعریف_کن APLIC_MMSICFGADDRH		0x1bc4
#تعریف_کن APLIC_SMSICFGADDR		0x1bc8
#تعریف_کن APLIC_SMSICFGADDRH		0x1bcc

#تعریف_کن APLIC_xMSICFGADDRH_L		(1UL << 31)
#تعریف_کن APLIC_x_MSICFGADDRH_HHXS_ماسک	0x1f
#تعریف_کن APLIC_x_MSICFGADDRH_HHXS_لغزش	24
#تعریف_کن APLIC_x_MSICFGADDRH_LHXS_ماسک	0x7
#تعریف_کن APLIC_x_MSICFGADDRH_LHXS_لغزش	20
#تعریف_کن APLIC_x_MSICFGADDRH_HHXW_ماسک	0x7
#تعریف_کن APLIC_x_MSICFGADDRH_HHXW_لغزش	16
#تعریف_کن APLIC_x_MSICFGADDRH_LHXW_ماسک	0xf
#تعریف_کن APLIC_x_MSICFGADDRH_LHXW_لغزش	12
#تعریف_کن APLIC_x_MSICFGADDRH_BAPPN_ماسک	0xfff

#تعریف_کن APLIC_x_MSICFGADDR_PPN_لغزش	12

#تعریف_کن APLIC_xMSICFGADDR_PPN_HART(__lhxs) \
	((1UL << (__lhxs)) - 1)

#تعریف_کن APLIC_x_MSICFGADDR_PPN_LHX_ماسک(__lhxw) \
	((1UL << (__lhxw)) - 1)
#تعریف_کن APLIC_x_MSICFGADDR_PPN_LHX_لغزش(__lhxs) \
	((__lhxs))
#تعریف_کن APLIC_xMSICFGADDR_PPN_LHX(__lhxw, __lhxs) \
	(APLIC_x_MSICFGADDR_PPN_LHX_ماسک(__lhxw) << \
	 APLIC_x_MSICFGADDR_PPN_LHX_لغزش(__lhxs))

#تعریف_کن APLIC_x_MSICFGADDR_PPN_HHX_ماسک(__hhxw) \
	((1UL << (__hhxw)) - 1)
#تعریف_کن APLIC_x_MSICFGADDR_PPN_HHX_لغزش(__hhxs) \
	((__hhxs) + APLIC_x_MSICFGADDR_PPN_لغزش)
#تعریف_کن APLIC_xMSICFGADDR_PPN_HHX(__hhxw, __hhxs) \
	(APLIC_x_MSICFGADDR_PPN_HHX_ماسک(__hhxw) << \
	 APLIC_x_MSICFGADDR_PPN_HHX_لغزش(__hhxs))

#تعریف_کن APLIC_هدف_GUEST_IDX(__gidx) \
	((((u32)(__gidx)) & APLIC_هدف_GUEST_IDX_ماسک) << \
	 APLIC_هدف_GUEST_IDX_لغزش)
#تعریف_کن APLIC_هدف_EIID(__eiid) \
	(((u32)(__eiid)) & APLIC_هدف_EIID_ماسک)

#تعریف_کن APLIC_SETIP_پایه		0x1c00
#تعریف_کن APLIC_SETIPNUM			0x1cdc

#تعریف_کن APLIC_CLRIP_پایه		0x1d00
#تعریف_کن APLIC_CLRIPNUM			0x1ddc

#تعریف_کن APLIC_SETIE_پایه		0x1e00
#تعریف_کن APLIC_SETIENUM			0x1edc

#تعریف_کن APLIC_CLRIE_پایه		0x1f00
#تعریف_کن APLIC_CLRIENUM			0x1fdc

#تعریف_کن APLIC_SETIPNUM_LE		0x2000
#تعریف_کن APLIC_SETIPNUM_BE		0x2004

#تعریف_کن APLIC_هدف_پایه		0x3004
#تعریف_کن APLIC_هدف_HART_IDX_لغزش	18
#تعریف_کن APLIC_هدف_HART_IDX_ماسک	0x3fff
#تعریف_کن APLIC_هدف_GUEST_IDX_لغزش	12
#تعریف_کن APLIC_هدف_GUEST_IDX_ماسک	0x3f
#تعریف_کن APLIC_هدف_IPRIO_ماسک	0xff
#تعریف_کن APLIC_هدف_EIID_ماسک	0x7ff

#تعریف_کن APLIC_هدف_HART_IDX(__hidx) \
	((((u32)(__hidx)) & APLIC_هدف_HART_IDX_ماسک) << \
	 APLIC_هدف_HART_IDX_لغزش)
#تعریف_کن APLIC_هدف_IPRIO(__prio) \
	(((u32)(__prio)) & APLIC_هدف_IPRIO_ماسک)

#تعریف_کن APLIC_IDC_پایه			0x4000
#تعریف_کن APLIC_IDC_اندازه			32

#تعریف_کن APLIC_IDC_IDELIVERY		0x00

#تعریف_کن APLIC_IDC_IFORCE		0x04

#تعریف_کن APLIC_IDC_ITHRESHOLD		0x08

#تعریف_کن APLIC_IDC_TOPI			0x18
#تعریف_کن APLIC_IDC_TOPI_ID_لغزش	16
#تعریف_کن APLIC_IDC_TOPI_ID_ماسک	0x3ff
#تعریف_کن APLIC_IDC_TOPI_PRIO_ماسک	0xff

#تعریف_کن APLIC_IDC_CLAIMI		0x1c

#تعریف_کن APLIC_DEFAULT_اولویت		1
#تعریف_کن APLIC_DISABLE_IDELIVERY		0
#تعریف_کن APLIC_ENABLE_IDELIVERY		1
#تعریف_کن APLIC_DISABLE_ITHRESHOLD	1
#تعریف_کن APLIC_ENABLE_ITHRESHOLD		0

بنية aplic_msi_داده {
	بنية aplic_داده *aplic;
	u32 hwirq;
	u32 والد_hwirq;
};

ساكن SBI_LIST_سر(aplic_list);
ساكن فراغ aplic_writel_msicfg(بنية aplic_msicfg_داده *msicfg,
				فراغ *msicfgaddr, فراغ *msicfgaddrH);

ساكن مضمن bool aplic_is_مستقیم_نما(بنية aplic_داده *aplic)
{
	أرجع aplic && aplic->رقم_idc;
}

ساكن مضمن فراغ aplic_sourcecfg_write(بنية aplic_داده *aplic,
					 u32 hwirq, u32 val)
{
	writel(val, (فراغ *)(aplic->addr + APLIC_SOURCECFG_پایه +
			     (hwirq - 1) * حجم(u32)));
}

ساكن مضمن u32 aplic_sourcecfg_read(بنية aplic_داده *aplic, u32 hwirq)
{
	أرجع readl((فراغ *)(aplic->addr + APLIC_SOURCECFG_پایه +
			      (hwirq - 1) * حجم(u32)));
}

ساكن مضمن فراغ aplic_هدف_write(بنية aplic_داده *aplic,
				      u32 hwirq, u32 val)
{
	writel(val, (فراغ *)(aplic->addr + APLIC_هدف_پایه +
			     (hwirq - 1) * حجم(u32)));
}

ساكن مضمن فراغ aplic_irq_setie(بنية aplic_داده *aplic, u32 hwirq)
{
	writel(hwirq, (فراغ *)(aplic->addr + APLIC_SETIENUM));
}

ساكن مضمن فراغ aplic_irq_clrie(بنية aplic_داده *aplic, u32 hwirq)
{
	writel(hwirq, (فراغ *)(aplic->addr + APLIC_CLRIENUM));
}

ساكن مضمن فراغ aplic_irq_clrip(بنية aplic_داده *aplic, u32 hwirq)
{
	writel(hwirq, (فراغ *)(aplic->addr + APLIC_CLRIPNUM));
}

ساكن مضمن فراغ aplic_msi_irq_retrigger_level(بنية aplic_داده *aplic, u32 hwirq)
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

ساكن مضمن فراغ *aplic_idc_پایه(بنية aplic_داده *aplic, u32 idc_شاخص)
{
	أرجع (فراغ *)(aplic->addr + APLIC_IDC_پایه +
			idc_شاخص * APLIC_IDC_اندازه);
}

ساكن مضمن u32 aplic_idc_read(بنية aplic_داده *aplic, u32 idc_شاخص,
				 u32 reg)
{
	أرجع readl(aplic_idc_پایه(aplic, idc_شاخص) + reg);
}

ساكن مضمن فراغ aplic_idc_write(بنية aplic_داده *aplic, u32 idc_شاخص,
				   u32 reg, u32 val)
{
	writel(val, aplic_idc_پایه(aplic, idc_شاخص) + reg);
}

ساكن u32 aplic_hwirq_flags_مقصد_sourcecfg(u32 hwirq_flags)
{
	بدل (hwirq_flags & SBI_HWIRQ_FLAGS_LEVEL_SENSE_ماسک) {
	حالة SBI_HWIRQ_FLAGS_یال_RISING:
		أرجع APLIC_SOURCECFG_SM_یال_RISE;
	حالة SBI_HWIRQ_FLAGS_یال_FALLING:
		أرجع APLIC_SOURCECFG_SM_یال_FALL;
	حالة SBI_HWIRQ_FLAGS_LEVEL_بالا:
		أرجع APLIC_SOURCECFG_SM_LEVEL_بالا;
	حالة SBI_HWIRQ_FLAGS_LEVEL_کم:
		أرجع APLIC_SOURCECFG_SM_LEVEL_کم;
	افتراضي:
		أرجع APLIC_SOURCECFG_SM_INACTIVE;
	}
}

ساكن bool aplic_hwirq_is_delegated(بنية aplic_داده *aplic, u32 hwirq)
{
	u32 sourcecfg;

	sourcecfg = aplic_sourcecfg_read(aplic, hwirq);
	أرجع !!(sourcecfg & APLIC_SOURCECFG_D);
}

ساكن صحيح aplic_پیدا_کن_idc_شاخص(بنية aplic_داده *aplic, u32 hart_شاخص)
{
	u32 i;

	لكل (i = 0; i < aplic->رقم_idc; i++) {
		إذا (aplic->idc_map[i] == hart_شاخص)
			أرجع i;
	}

	أرجع SBI_ENOENT;
}

ساكن فراغ aplic_init(بنية aplic_داده *aplic)
{
	بنية aplic_delegate_داده *deleg;
	u32 i, j, tmp, domaincfg;
	صحيح locked;

	/* Set domain configuration to 0 */
	writel(0, (فراغ *)(aplic->addr + APLIC_DOMAINCFG));

	/* Disable all interrupts */
	لكل (i = 0; i <= aplic->رقم_منبع; i += 32)
		writel(-1U, (فراغ *)(aplic->addr + APLIC_CLRIE_پایه +
				     (i / 32) * حجم(u32)));

	/* Set interrupt type and priority for all interrupts */
	لكل (i = 1; i <= aplic->رقم_منبع; i++) {
		/* Set IRQ source configuration to 0 */
		writel(0, (فراغ *)(aplic->addr + APLIC_SOURCECFG_پایه +
			  (i - 1) * حجم(u32)));
		/* Set IRQ target hart index and priority to 1 */
		writel(APLIC_DEFAULT_اولویت, (فراغ *)(aplic->addr +
						APLIC_هدف_پایه +
						(i - 1) * حجم(u32)));
	}

	/* Configure IRQ delegation */
	لكل (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		إذا (!deleg->اولی_irq || !deleg->آخرین_irq)
			استمر;
		إذا (aplic->رقم_منبع < deleg->اولی_irq ||
		    aplic->رقم_منبع < deleg->آخرین_irq)
			استمر;
		إذا (deleg->فرزند_شاخص > APLIC_SOURCECFG_CHILDIDX_ماسک)
			استمر;
		إذا (deleg->اولی_irq > deleg->آخرین_irq) {
			tmp = deleg->اولی_irq;
			deleg->اولی_irq = deleg->آخرین_irq;
			deleg->آخرین_irq = tmp;
		}
		لكل (j = deleg->اولی_irq; j <= deleg->آخرین_irq; j++)
			aplic_sourcecfg_write(aplic, j, APLIC_SOURCECFG_D | deleg->فرزند_شاخص);
	}

	/* Default initialization of IDC structures */
	إذا (aplic_is_مستقیم_نما(aplic)) {
		لكل (i = 0; i < aplic->رقم_idc; i++) {
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
	إذا (!aplic_is_مستقیم_نما(aplic))
		domaincfg |= APLIC_DOMAINCFG_DM;

	writel(domaincfg, (فراغ *)(aplic->addr + APLIC_DOMAINCFG));
}

فراغ aplic_reinit_all(فراغ)
{
	بنية aplic_داده *aplic;

	sbi_list_for_each_مدخل(aplic, &aplic_list, گره)
		aplic_init(aplic);
}

ساكن فراغ aplic_writel_msicfg(بنية aplic_msicfg_داده *msicfg,
				فراغ *msicfgaddr, فراغ *msicfgaddrH)
{
	u32 val;
	بیعلامت دراز پایه_ppn;

	/* Compute the MSI base PPN */
	پایه_ppn = msicfg->پایه_addr >> APLIC_x_MSICFGADDR_PPN_لغزش;
	پایه_ppn &= ~APLIC_xMSICFGADDR_PPN_HART(msicfg->lhxs);
	پایه_ppn &= ~APLIC_xMSICFGADDR_PPN_LHX(msicfg->lhxw, msicfg->lhxs);
	پایه_ppn &= ~APLIC_xMSICFGADDR_PPN_HHX(msicfg->hhxw, msicfg->hhxs);

	/* Write the lower MSI config register */
	writel((u32)پایه_ppn, msicfgaddr);

	/* Write the upper MSI config register */
	val = (((u64)پایه_ppn) >> 32) &
		APLIC_x_MSICFGADDRH_BAPPN_ماسک;
	val |= (msicfg->lhxw & APLIC_x_MSICFGADDRH_LHXW_ماسک)
		<< APLIC_x_MSICFGADDRH_LHXW_لغزش;
	val |= (msicfg->hhxw & APLIC_x_MSICFGADDRH_HHXW_ماسک)
		<< APLIC_x_MSICFGADDRH_HHXW_لغزش;
	val |= (msicfg->lhxs & APLIC_x_MSICFGADDRH_LHXS_ماسک)
		<< APLIC_x_MSICFGADDRH_LHXS_لغزش;
	val |= (msicfg->hhxs & APLIC_x_MSICFGADDRH_HHXS_ماسک)
		<< APLIC_x_MSICFGADDRH_HHXS_لغزش;
	writel(val, msicfgaddrH);
}

ساكن صحيح aplic_check_msicfg(بنية aplic_msicfg_داده *msicfg)
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
	بنية aplic_داده *aplic;
	u32 hart_شاخص;
	صحيح idc_شاخص;

	aplic = container_از(chip, بنية aplic_داده, irqchip);
	إذا (!aplic_is_مستقیم_نما(aplic))
		أرجع 0;

	hart_شاخص = current_hartindex();
	idc_شاخص  = aplic_پیدا_کن_idc_شاخص(aplic, hart_شاخص);
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
	بنية aplic_داده *aplic;
	u32 hart_شاخص, claimi, hwirq;
	صحيح idc_شاخص, rc = 0, tmp;

	aplic = container_از(chip, بنية aplic_داده, irqchip);

	إذا (!aplic_is_مستقیم_نما(aplic))
		sbi_panic("aplic_process_hwirqs called in MSI mode.\n");

	hart_شاخص = current_hartindex();
	idc_شاخص  = aplic_پیدا_کن_idc_شاخص(aplic, hart_شاخص);
	إذا (idc_شاخص < 0)
		أرجع 0;

	طالما ((claimi = aplic_idc_read(aplic, idc_شاخص, APLIC_IDC_CLAIMI))) {
		hwirq = (claimi >> APLIC_IDC_TOPI_ID_لغزش) &
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
	بنية aplic_داده *aplic;
	u32 sm;

	aplic = container_از(chip, بنية aplic_داده, irqchip);
	إذا (aplic_is_مستقیم_نما(aplic))
		أرجع;

	sm = aplic_sourcecfg_read(aplic, hwirq) & APLIC_SOURCECFG_SM_ماسک;
	إذا (sm == APLIC_SOURCECFG_SM_LEVEL_بالا ||
	    sm == APLIC_SOURCECFG_SM_LEVEL_کم)
		aplic_msi_irq_retrigger_level(aplic, hwirq);
}

ساكن فراغ aplic_برنامه_msi_هدف(بنية aplic_داده *aplic,
				     u32 hwirq, u32 hart_شاخص,
				     u32 guest_شاخص, u32 eiid)
{
	u32 هدف;

	هدف = APLIC_هدف_HART_IDX(hart_شاخص) |
		 APLIC_هدف_GUEST_IDX(guest_شاخص) |
		 APLIC_هدف_EIID(eiid);

	aplic_هدف_write(aplic, hwirq, هدف);
}

ساكن u32 derive_hart_شاخص(بنية aplic_msicfg_داده *msicfg,
			      ثابت بنية sbi_irqchip_msi_پیام *پیام)
{
	u64 addr = ((u64)پیام->آدرس_hi << 32) | پیام->آدرس_lo;
	u64 tppn = addr >> APLIC_x_MSICFGADDR_PPN_لغزش;
	u32 گروه_شاخص, hart_شاخص;

	گروه_شاخص = (tppn >> APLIC_x_MSICFGADDR_PPN_HHX_لغزش(msicfg->hhxs)) &
		      APLIC_x_MSICFGADDR_PPN_HHX_ماسک(msicfg->hhxw);

	hart_شاخص = (tppn >> msicfg->lhxs) &
		     ((1UL << msicfg->lhxw) - 1);

	hart_شاخص |= (گروه_شاخص << msicfg->lhxw);

	أرجع hart_شاخص;
}

ساكن فراغ aplic_write_msi(u32 والد_hwirq,
			    ثابت بنية sbi_irqchip_msi_پیام *پیام,
			    فراغ *priv)
{
	بنية aplic_msi_داده *msi_داده = priv;
	u32 guest_شاخص = 0;
	u32 eiid;

	eiid = پیام->داده;
	إذا (!eiid || eiid > APLIC_هدف_EIID_ماسک)
		أرجع;

	aplic_برنامه_msi_هدف(msi_داده->aplic, msi_داده->hwirq,
				derive_hart_شاخص(&msi_داده->aplic->msicfg_mmode, پیام),
				guest_شاخص, eiid);
}

ساكن صحيح aplic_msi_callback(u32 والد_hwirq, فراغ *priv)
{
	بنية aplic_msi_داده *msi_داده = priv;
	صحيح rc;

	rc = sbi_irqchip_process_hwirq(&msi_داده->aplic->irqchip, msi_داده->hwirq);
	إذا (rc && rc != SBI_ENOENT)
		sbi_printf("%s: hwirq=%lu failed rc=%d\n",
			__func__,
			(بیعلامت دراز)والد_hwirq, rc);

	أرجع rc;
}

ساكن صحيح aplic_راهاندازی_msi(بنية aplic_داده *aplic, u32 hwirq)
{
	بنية sbi_irqchip_device *والد;
	u32 اولی_hwirq;
	بنية aplic_msi_داده *msi_داده;
	صحيح rc;

	والد = sbi_irqchip_پیدا_کن_device(aplic->والد_unique_id);
	إذا (!والد) {
		sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
			__func__, (بیعلامت دراز)hwirq);
		أرجع SBI_EINVAL;
	}

	msi_داده = sbi_zalloc(حجم(بنية aplic_msi_داده));
	إذا (!msi_داده)
		أرجع SBI_ENOMEM;

	msi_داده->aplic = aplic;
	msi_داده->hwirq = hwirq;

	rc = sbi_irqchip_ثبات_msi(والد, 1,
					aplic_write_msi,
					aplic_msi_callback,
					msi_داده,
					&اولی_hwirq);
	إذا (rc) {
		sbi_printf("%s: register_msi failed hwirq=%lu rc=%d\n",
			__func__, (بیعلامت دراز)hwirq, rc);
		sbi_free(msi_داده);
		أرجع rc;
	}

	msi_داده->والد_hwirq = اولی_hwirq;
	sbi_irqchip_مجموعه_hwirq_priv(&aplic->irqchip, hwirq, msi_داده);

	أرجع 0;
}

ساكن صحيح aplic_hwirq_راهاندازی(بنية sbi_irqchip_device *chip,
			     u32 hwirq, u32 hwirq_flags)
{
	بنية aplic_داده *aplic;
	u32 sourcecfg;

	aplic = container_از(chip, بنية aplic_داده, irqchip);

	sourcecfg = aplic_hwirq_flags_مقصد_sourcecfg(hwirq_flags);
	إذا (sourcecfg == APLIC_SOURCECFG_SM_INACTIVE) {
		sbi_printf("%s: unsupported flags=0x%lx hwirq=%lu\n",
			__func__,
			(بیعلامت دراز)hwirq_flags,
			(بیعلامت دراز)hwirq);
		أرجع SBI_EINVAL;
	}

	aplic_sourcecfg_write(aplic, hwirq, sourcecfg);

	إذا (aplic_hwirq_is_delegated(aplic, hwirq)) {
		sbi_printf("%s: hwirq=%lu is delegated\n",
			__func__, (بیعلامت دراز)hwirq);
		أرجع SBI_ENOTSUPP;
	}

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);

	إذا (!aplic_is_مستقیم_نما(aplic))
		أرجع aplic_راهاندازی_msi(aplic, hwirq);

	أرجع 0;
}

ساكن فراغ aplic_hwirq_cleanup(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية aplic_داده *aplic;
	بنية aplic_msi_داده *msi_داده;

	aplic = container_از(chip, بنية aplic_داده, irqchip);
	إذا (aplic_hwirq_is_delegated(aplic, hwirq))
		أرجع;

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);
	aplic_sourcecfg_write(aplic, hwirq, APLIC_SOURCECFG_SM_INACTIVE);
	aplic_هدف_write(aplic, hwirq, APLIC_DEFAULT_اولویت);
	msi_داده = (بنية aplic_msi_داده *) sbi_irqchip_get_hwirq_priv(chip, hwirq);
	إذا (msi_داده)
		sbi_free(msi_داده);
}

ساكن صحيح aplic_hwirq_مجموعه_affinity(بنية sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_شاخص)
{
	بنية aplic_داده *aplic;
	بنية aplic_msi_داده *msi_داده;
	بنية sbi_irqchip_device *والد;
	صحيح idc_شاخص;
	صحيح rc;

	aplic = container_از(chip, بنية aplic_داده, irqchip);
	إذا (aplic_hwirq_is_delegated(aplic, hwirq))
		أرجع SBI_ENOTSUPP;

	إذا (!aplic_is_مستقیم_نما(aplic)) {
		والد = sbi_irqchip_پیدا_کن_device(aplic->والد_unique_id);
		إذا (!والد) {
			sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
				__func__, (بیعلامت دراز)hwirq);
			أرجع SBI_EINVAL;
		}

		msi_داده = sbi_irqchip_get_hwirq_priv(chip, hwirq);
		rc = sbi_irqchip_مجموعه_affinity(والد, msi_داده->والد_hwirq, hart_شاخص);
		إذا (rc) {
			sbi_printf("%s: sbi_irqchip_set_affinity failed hwirq=%lu rc=%d\n",
				__func__, (بیعلامت دراز)hwirq, rc);
			أرجع rc;
		}
		أرجع 0;
	}

	idc_شاخص = aplic_پیدا_کن_idc_شاخص(aplic, hart_شاخص);
	إذا (idc_شاخص < 0)
		أرجع SBI_EINVAL;

	aplic_هدف_write(aplic, hwirq,
			   APLIC_هدف_HART_IDX(hart_شاخص) |
			   APLIC_هدف_IPRIO(APLIC_DEFAULT_اولویت));

	أرجع 0;
}

ساكن فراغ aplic_hwirq_ماسک(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية aplic_داده *aplic;

	aplic = container_از(chip, بنية aplic_داده, irqchip);
	إذا (aplic_hwirq_is_delegated(aplic, hwirq))
		أرجع;

	aplic_irq_clrie(aplic, hwirq);
}

ساكن فراغ aplic_hwirq_unmask(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية aplic_داده *aplic;

	aplic = container_از(chip, بنية aplic_داده, irqchip);
	إذا (aplic_hwirq_is_delegated(aplic, hwirq))
		أرجع;

	aplic_irq_setie(aplic, hwirq);
}

ساكن بنية sbi_irqchip_device aplic_irqchip_template = {
	.warm_init			= aplic_warm_init,
	.process_hwirqs		= aplic_process_hwirqs,
	.hwirq_eoi			= aplic_hwirq_eoi,
	.hwirq_راهاندازی		= aplic_hwirq_راهاندازی,
	.hwirq_cleanup		= aplic_hwirq_cleanup,
	.hwirq_مجموعه_affinity	= aplic_hwirq_مجموعه_affinity,
	.hwirq_ماسک			= aplic_hwirq_ماسک,
	.hwirq_unmask		= aplic_hwirq_unmask,
};

صحيح aplic_cold_irqchip_init(بنية aplic_داده *aplic)
{
	صحيح rc;
	بنية aplic_delegate_داده *deleg;
	u32 اولی_deleg_irq, آخرین_deleg_irq, i;
	bool msi_نما;

	msi_نما = !aplic_is_مستقیم_نما(aplic);
	إذا (!aplic->رقم_منبع) {
		sbi_printf("%s: num_source is zero\n", __func__);
		أرجع SBI_EINVAL;
	}

	إذا (APLIC_MAX_منبع <= aplic->رقم_منبع) {
		sbi_printf("%s: num_source=%lu exceeds max=%lu\n",
			__func__,
			(بیعلامت دراز)aplic->رقم_منبع,
			(بیعلامت دراز)APLIC_MAX_منبع);
		أرجع SBI_EINVAL;
	}

	إذا (!msi_نما && APLIC_MAX_IDC <= aplic->رقم_idc) {
		sbi_printf("%s: num_idc=%lu exceeds max=%lu\n",
			__func__,
			(بیعلامت دراز)aplic->رقم_idc,
			(بیعلامت دراز)APLIC_MAX_IDC);
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
	اولی_deleg_irq = -1U;
	آخرین_deleg_irq = 0;
	لكل (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		إذا (deleg->اولی_irq < اولی_deleg_irq)
			اولی_deleg_irq = deleg->اولی_irq;
		إذا (آخرین_deleg_irq < deleg->آخرین_irq)
			آخرین_deleg_irq = deleg->آخرین_irq;
	}

	إذا (aplic->targets_mmode ||
	    ((اولی_deleg_irq < آخرین_deleg_irq) &&
	    (آخرین_deleg_irq == aplic->رقم_منبع) &&
	    (اولی_deleg_irq == 1))) {
		rc = sbi_domain_ریشه_جمع_کن_memrange(aplic->addr, aplic->اندازه, صفحه_اندازه,
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
		aplic->irqchip.رقم_hwirq = aplic->رقم_منبع + 1;

		إذا (msi_نما) {
			aplic->irqchip.warm_init      = عدم;
			aplic->irqchip.process_hwirqs = عدم;
			aplic->irqchip.hwirq_eoi      = عدم;
		}

		إذا (msi_نما)
			sbi_hartmask_مجموعه_all(&aplic->irqchip.هدف_harts);
		وإلا
			لكل (i = 0; i < aplic->رقم_idc; i++)
				sbi_hartmask_مجموعه_hartindex(aplic->idc_map[i],
							   &aplic->irqchip.هدف_harts);

		rc = sbi_irqchip_جمع_کن_device(&aplic->irqchip);
		إذا (rc) {
			sbi_printf("%s: sbi_irqchip_add_device failed rc=%d id=%lu mode=%s target_weight=%lu\n",
				__func__, rc,
				(بیعلامت دراز)aplic->irqchip.id,
				msi_نما ? "msi" : "direct",
				(بیعلامت دراز)sbi_hartmask_وزن(
					&aplic->irqchip.هدف_harts));
			أرجع rc;
		}
	}
	/* Attach to the aplic list */
	sbi_list_جمع_کن_دم(&aplic->گره, &aplic_list);

	أرجع 0;
}
