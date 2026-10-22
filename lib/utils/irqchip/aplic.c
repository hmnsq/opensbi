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

#define APLIC_MAX_IDC			(1UL << 14)
#define APLIC_MAX_SOURCE		1024

#define APLIC_DOMAINCFG		0x0000
#define APLIC_DOMAINCFG_IE		(1 << 8)
#define APLIC_DOMAINCFG_DM		(1 << 2)
#define APLIC_DOMAINCFG_BE		(1 << 0)

#define APLIC_SOURCECFG_BASE		0x0004
#define APLIC_SOURCECFG_D		(1 << 10)
#define APLIC_SOURCECFG_CHILDIDX_MASK	0x000003ff
#define APLIC_SOURCECFG_SM_MASK	0x00000007
#define APLIC_SOURCECFG_SM_INACTIVE	0x0
#define APLIC_SOURCECFG_SM_DETACH	0x1
#define APLIC_SOURCECFG_SM_EDGE_RISE	0x4
#define APLIC_SOURCECFG_SM_EDGE_FALL	0x5
#define APLIC_SOURCECFG_SM_LEVEL_بەرز	0x6
#define APLIC_SOURCECFG_SM_LEVEL_نزم	0x7

#define APLIC_MMSICFGADDR		0x1bc0
#define APLIC_MMSICFGADDRH		0x1bc4
#define APLIC_SMSICFGADDR		0x1bc8
#define APLIC_SMSICFGADDRH		0x1bcc

#define APLIC_xMSICFGADDRH_L		(1UL << 31)
#define APLIC_xMSICFGADDRH_HHXS_MASK	0x1f
#define APLIC_xMSICFGADDRH_HHXS_SHIFT	24
#define APLIC_xMSICFGADDRH_LHXS_MASK	0x7
#define APLIC_xMSICFGADDRH_LHXS_SHIFT	20
#define APLIC_xMSICFGADDRH_HHXW_MASK	0x7
#define APLIC_xMSICFGADDRH_HHXW_SHIFT	16
#define APLIC_xMSICFGADDRH_LHXW_MASK	0xf
#define APLIC_xMSICFGADDRH_LHXW_SHIFT	12
#define APLIC_xMSICFGADDRH_BAPPN_MASK	0xfff

#define APLIC_xMSICFGADDR_PPN_SHIFT	12

#define APLIC_xMSICFGADDR_PPN_HART(__lhxs) \
	((1UL << (__lhxs)) - 1)

#define APLIC_xMSICFGADDR_PPN_LHX_MASK(__lhxw) \
	((1UL << (__lhxw)) - 1)
#define APLIC_xMSICFGADDR_PPN_LHX_SHIFT(__lhxs) \
	((__lhxs))
#define APLIC_xMSICFGADDR_PPN_LHX(__lhxw, __lhxs) \
	(APLIC_xMSICFGADDR_PPN_LHX_MASK(__lhxw) << \
	 APLIC_xMSICFGADDR_PPN_LHX_SHIFT(__lhxs))

#define APLIC_xMSICFGADDR_PPN_HHX_MASK(__hhxw) \
	((1UL << (__hhxw)) - 1)
#define APLIC_xMSICFGADDR_PPN_HHX_SHIFT(__hhxs) \
	((__hhxs) + APLIC_xMSICFGADDR_PPN_SHIFT)
#define APLIC_xMSICFGADDR_PPN_HHX(__hhxw, __hhxs) \
	(APLIC_xMSICFGADDR_PPN_HHX_MASK(__hhxw) << \
	 APLIC_xMSICFGADDR_PPN_HHX_SHIFT(__hhxs))

#define APLIC_TARGET_GUEST_IDX(__gidx) \
	((((u32)(__gidx)) & APLIC_TARGET_GUEST_IDX_MASK) << \
	 APLIC_TARGET_GUEST_IDX_SHIFT)
#define APLIC_TARGET_EIID(__eiid) \
	(((u32)(__eiid)) & APLIC_TARGET_EIID_MASK)

#define APLIC_SETIP_BASE		0x1c00
#define APLIC_SETIPNUM			0x1cdc

#define APLIC_CLRIP_BASE		0x1d00
#define APLIC_CLRIPNUM			0x1ddc

#define APLIC_SETIE_BASE		0x1e00
#define APLIC_SETIENUM			0x1edc

#define APLIC_CLRIE_BASE		0x1f00
#define APLIC_CLRIENUM			0x1fdc

#define APLIC_SETIPNUM_LE		0x2000
#define APLIC_SETIPNUM_BE		0x2004

#define APLIC_TARGET_BASE		0x3004
#define APLIC_TARGET_HART_IDX_SHIFT	18
#define APLIC_TARGET_HART_IDX_MASK	0x3fff
#define APLIC_TARGET_GUEST_IDX_SHIFT	12
#define APLIC_TARGET_GUEST_IDX_MASK	0x3f
#define APLIC_TARGET_IPRIO_MASK	0xff
#define APLIC_TARGET_EIID_MASK	0x7ff

#define APLIC_TARGET_HART_IDX(__hidx) \
	((((u32)(__hidx)) & APLIC_TARGET_HART_IDX_MASK) << \
	 APLIC_TARGET_HART_IDX_SHIFT)
#define APLIC_TARGET_IPRIO(__prio) \
	(((u32)(__prio)) & APLIC_TARGET_IPRIO_MASK)

#define APLIC_IDC_BASE			0x4000
#define APLIC_IDC_SIZE			32

#define APLIC_IDC_IDELIVERY		0x00

#define APLIC_IDC_IFORCE		0x04

#define APLIC_IDC_ITHRESHOLD		0x08

#define APLIC_IDC_TOPI			0x18
#define APLIC_IDC_TOPI_ID_SHIFT	16
#define APLIC_IDC_TOPI_ID_MASK	0x3ff
#define APLIC_IDC_TOPI_PRIO_MASK	0xff

#define APLIC_IDC_CLAIMI		0x1c

#define APLIC_DEFAULT_PRIORITY		1
#define APLIC_DISABLE_IDELIVERY		0
#define APLIC_ENABLE_IDELIVERY		1
#define APLIC_DISABLE_ITHRESHOLD	1
#define APLIC_ENABLE_ITHRESHOLD		0

بنية aplic_msi_data {
	بنية aplic_data *aplic;
	u32 hwirq;
	u32 parent_hwirq;
};

ساكن SBI_LIST_HEAD(aplic_list);
ساكن فراغ aplic_writel_msicfg(بنية aplic_msicfg_data *msicfg,
				فراغ *msicfgaddr, فراغ *msicfgaddrH);

ساكن مضمن bool aplic_is_direct_mode(بنية aplic_data *aplic)
{
	أرجع aplic && aplic->ژمارە_idc;
}

ساكن مضمن فراغ aplic_sourcecfg_write(بنية aplic_data *aplic,
					 u32 hwirq, u32 val)
{
	writel(val, (فراغ *)(aplic->addr + APLIC_SOURCECFG_BASE +
			     (hwirq - 1) * حجم(u32)));
}

ساكن مضمن u32 aplic_sourcecfg_read(بنية aplic_data *aplic, u32 hwirq)
{
	أرجع readl((فراغ *)(aplic->addr + APLIC_SOURCECFG_BASE +
			      (hwirq - 1) * حجم(u32)));
}

ساكن مضمن فراغ aplic_target_write(بنية aplic_data *aplic,
				      u32 hwirq, u32 val)
{
	writel(val, (فراغ *)(aplic->addr + APLIC_TARGET_BASE +
			     (hwirq - 1) * حجم(u32)));
}

ساكن مضمن فراغ aplic_irq_setie(بنية aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (فراغ *)(aplic->addr + APLIC_SETIENUM));
}

ساكن مضمن فراغ aplic_irq_clrie(بنية aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (فراغ *)(aplic->addr + APLIC_CLRIENUM));
}

ساكن مضمن فراغ aplic_irq_clrip(بنية aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (فراغ *)(aplic->addr + APLIC_CLRIPNUM));
}

ساكن مضمن فراغ aplic_msi_irq_retrigger_level(بنية aplic_data *aplic, u32 hwirq)
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

ساكن مضمن فراغ *aplic_idc_base(بنية aplic_data *aplic, u32 idc_index)
{
	أرجع (فراغ *)(aplic->addr + APLIC_IDC_BASE +
			idc_index * APLIC_IDC_SIZE);
}

ساكن مضمن u32 aplic_idc_read(بنية aplic_data *aplic, u32 idc_index,
				 u32 reg)
{
	أرجع readl(aplic_idc_base(aplic, idc_index) + reg);
}

ساكن مضمن فراغ aplic_idc_write(بنية aplic_data *aplic, u32 idc_index,
				   u32 reg, u32 val)
{
	writel(val, aplic_idc_base(aplic, idc_index) + reg);
}

ساكن u32 aplic_hwirq_flags_to_sourcecfg(u32 hwirq_flags)
{
	بدل (hwirq_flags & SBI_HWIRQ_FLAGS_LEVEL_SENSE_MASK) {
	حالة SBI_HWIRQ_FLAGS_EDGE_RISING:
		أرجع APLIC_SOURCECFG_SM_EDGE_RISE;
	حالة SBI_HWIRQ_FLAGS_EDGE_FALLING:
		أرجع APLIC_SOURCECFG_SM_EDGE_FALL;
	حالة SBI_HWIRQ_FLAGS_LEVEL_بەرز:
		أرجع APLIC_SOURCECFG_SM_LEVEL_بەرز;
	حالة SBI_HWIRQ_FLAGS_LEVEL_نزم:
		أرجع APLIC_SOURCECFG_SM_LEVEL_نزم;
	افتراضي:
		أرجع APLIC_SOURCECFG_SM_INACTIVE;
	}
}

ساكن bool aplic_hwirq_is_delegated(بنية aplic_data *aplic, u32 hwirq)
{
	u32 sourcecfg;

	sourcecfg = aplic_sourcecfg_read(aplic, hwirq);
	أرجع !!(sourcecfg & APLIC_SOURCECFG_D);
}

ساكن صحيح aplic_find_idc_index(بنية aplic_data *aplic, u32 hart_index)
{
	u32 i;

	لكل (i = 0; i < aplic->ژمارە_idc; i++) {
		إذا (aplic->idc_map[i] == hart_index)
			أرجع i;
	}

	أرجع SBI_ENOENT;
}

ساكن فراغ aplic_init(بنية aplic_data *aplic)
{
	بنية aplic_delegate_data *deleg;
	u32 i, j, tmp, domaincfg;
	صحيح locked;

	/* Set domain configuration to 0 */
	writel(0, (فراغ *)(aplic->addr + APLIC_DOMAINCFG));

	/* Disable all interrupts */
	لكل (i = 0; i <= aplic->ژمارە_source; i += 32)
		writel(-1U, (فراغ *)(aplic->addr + APLIC_CLRIE_BASE +
				     (i / 32) * حجم(u32)));

	/* Set interrupt type and priority for all interrupts */
	لكل (i = 1; i <= aplic->ژمارە_source; i++) {
		/* Set IRQ source configuration to 0 */
		writel(0, (فراغ *)(aplic->addr + APLIC_SOURCECFG_BASE +
			  (i - 1) * حجم(u32)));
		/* Set IRQ target hart index and priority to 1 */
		writel(APLIC_DEFAULT_PRIORITY, (فراغ *)(aplic->addr +
						APLIC_TARGET_BASE +
						(i - 1) * حجم(u32)));
	}

	/* Configure IRQ delegation */
	لكل (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		إذا (!deleg->first_irq || !deleg->last_irq)
			استمر;
		إذا (aplic->ژمارە_source < deleg->first_irq ||
		    aplic->ژمارە_source < deleg->last_irq)
			استمر;
		إذا (deleg->child_index > APLIC_SOURCECFG_CHILDIDX_MASK)
			استمر;
		إذا (deleg->first_irq > deleg->last_irq) {
			tmp = deleg->first_irq;
			deleg->first_irq = deleg->last_irq;
			deleg->last_irq = tmp;
		}
		لكل (j = deleg->first_irq; j <= deleg->last_irq; j++)
			aplic_sourcecfg_write(aplic, j, APLIC_SOURCECFG_D | deleg->child_index);
	}

	/* Default initialization of IDC structures */
	إذا (aplic_is_direct_mode(aplic)) {
		لكل (i = 0; i < aplic->ژمارە_idc; i++) {
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
	إذا (!aplic_is_direct_mode(aplic))
		domaincfg |= APLIC_DOMAINCFG_DM;

	writel(domaincfg, (فراغ *)(aplic->addr + APLIC_DOMAINCFG));
}

فراغ aplic_reinit_all(فراغ)
{
	بنية aplic_data *aplic;

	sbi_list_for_each_entry(aplic, &aplic_list, گرێ)
		aplic_init(aplic);
}

ساكن فراغ aplic_writel_msicfg(بنية aplic_msicfg_data *msicfg,
				فراغ *msicfgaddr, فراغ *msicfgaddrH)
{
	u32 val;
	unsigned long base_ppn;

	/* Compute the MSI base PPN */
	base_ppn = msicfg->base_addr >> APLIC_xMSICFGADDR_PPN_SHIFT;
	base_ppn &= ~APLIC_xMSICFGADDR_PPN_HART(msicfg->lhxs);
	base_ppn &= ~APLIC_xMSICFGADDR_PPN_LHX(msicfg->lhxw, msicfg->lhxs);
	base_ppn &= ~APLIC_xMSICFGADDR_PPN_HHX(msicfg->hhxw, msicfg->hhxs);

	/* Write the lower MSI config register */
	writel((u32)base_ppn, msicfgaddr);

	/* Write the upper MSI config register */
	val = (((u64)base_ppn) >> 32) &
		APLIC_xMSICFGADDRH_BAPPN_MASK;
	val |= (msicfg->lhxw & APLIC_xMSICFGADDRH_LHXW_MASK)
		<< APLIC_xMSICFGADDRH_LHXW_SHIFT;
	val |= (msicfg->hhxw & APLIC_xMSICFGADDRH_HHXW_MASK)
		<< APLIC_xMSICFGADDRH_HHXW_SHIFT;
	val |= (msicfg->lhxs & APLIC_xMSICFGADDRH_LHXS_MASK)
		<< APLIC_xMSICFGADDRH_LHXS_SHIFT;
	val |= (msicfg->hhxs & APLIC_xMSICFGADDRH_HHXS_MASK)
		<< APLIC_xMSICFGADDRH_HHXS_SHIFT;
	writel(val, msicfgaddrH);
}

ساكن صحيح aplic_check_msicfg(بنية aplic_msicfg_data *msicfg)
{
	إذا (APLIC_xMSICFGADDRH_LHXS_MASK < msicfg->lhxs)
		أرجع SBI_EINVAL;

	إذا (APLIC_xMSICFGADDRH_LHXW_MASK < msicfg->lhxw)
		أرجع SBI_EINVAL;

	إذا (APLIC_xMSICFGADDRH_HHXS_MASK < msicfg->hhxs)
		أرجع SBI_EINVAL;

	إذا (APLIC_xMSICFGADDRH_HHXW_MASK < msicfg->hhxw)
		أرجع SBI_EINVAL;

	أرجع 0;
}

ساكن صحيح aplic_warm_init(بنية sbi_irqchip_device *chip)
{
	بنية aplic_data *aplic;
	u32 hart_index;
	صحيح idc_index;

	aplic = container_of(chip, بنية aplic_data, irqchip);
	إذا (!aplic_is_direct_mode(aplic))
		أرجع 0;

	hart_index = current_hartindex();
	idc_index  = aplic_find_idc_index(aplic, hart_index);
	إذا (idc_index < 0)
		أرجع 0;

	aplic_idc_write(aplic, idc_index, APLIC_IDC_ITHRESHOLD,
			APLIC_ENABLE_ITHRESHOLD);
	aplic_idc_write(aplic, idc_index, APLIC_IDC_IDELIVERY,
			APLIC_ENABLE_IDELIVERY);

	أرجع 0;
}

ساكن صحيح aplic_process_hwirqs(بنية sbi_irqchip_device *chip)
{
	بنية aplic_data *aplic;
	u32 hart_index, claimi, hwirq;
	صحيح idc_index, rc = 0, tmp;

	aplic = container_of(chip, بنية aplic_data, irqchip);

	إذا (!aplic_is_direct_mode(aplic))
		sbi_panic("aplic_process_hwirqs called in MSI mode.\n");

	hart_index = current_hartindex();
	idc_index  = aplic_find_idc_index(aplic, hart_index);
	إذا (idc_index < 0)
		أرجع 0;

	طالما ((claimi = aplic_idc_read(aplic, idc_index, APLIC_IDC_CLAIMI))) {
		hwirq = (claimi >> APLIC_IDC_TOPI_ID_SHIFT) &
			APLIC_IDC_TOPI_ID_MASK;

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
	بنية aplic_data *aplic;
	u32 sm;

	aplic = container_of(chip, بنية aplic_data, irqchip);
	إذا (aplic_is_direct_mode(aplic))
		أرجع;

	sm = aplic_sourcecfg_read(aplic, hwirq) & APLIC_SOURCECFG_SM_MASK;
	إذا (sm == APLIC_SOURCECFG_SM_LEVEL_بەرز ||
	    sm == APLIC_SOURCECFG_SM_LEVEL_نزم)
		aplic_msi_irq_retrigger_level(aplic, hwirq);
}

ساكن فراغ aplic_program_msi_target(بنية aplic_data *aplic,
				     u32 hwirq, u32 hart_index,
				     u32 guest_index, u32 eiid)
{
	u32 target;

	target = APLIC_TARGET_HART_IDX(hart_index) |
		 APLIC_TARGET_GUEST_IDX(guest_index) |
		 APLIC_TARGET_EIID(eiid);

	aplic_target_write(aplic, hwirq, target);
}

ساكن u32 derive_hart_index(بنية aplic_msicfg_data *msicfg,
			      ثابت بنية sbi_irqchip_msi_پەیام *پەیام)
{
	u64 addr = ((u64)پەیام->address_hi << 32) | پەیام->address_lo;
	u64 tppn = addr >> APLIC_xMSICFGADDR_PPN_SHIFT;
	u32 group_index, hart_index;

	group_index = (tppn >> APLIC_xMSICFGADDR_PPN_HHX_SHIFT(msicfg->hhxs)) &
		      APLIC_xMSICFGADDR_PPN_HHX_MASK(msicfg->hhxw);

	hart_index = (tppn >> msicfg->lhxs) &
		     ((1UL << msicfg->lhxw) - 1);

	hart_index |= (group_index << msicfg->lhxw);

	أرجع hart_index;
}

ساكن فراغ aplic_write_msi(u32 parent_hwirq,
			    ثابت بنية sbi_irqchip_msi_پەیام *پەیام,
			    فراغ *priv)
{
	بنية aplic_msi_data *msi_data = priv;
	u32 guest_index = 0;
	u32 eiid;

	eiid = پەیام->data;
	إذا (!eiid || eiid > APLIC_TARGET_EIID_MASK)
		أرجع;

	aplic_program_msi_target(msi_data->aplic, msi_data->hwirq,
				derive_hart_index(&msi_data->aplic->msicfg_mmode, پەیام),
				guest_index, eiid);
}

ساكن صحيح aplic_msi_callback(u32 parent_hwirq, فراغ *priv)
{
	بنية aplic_msi_data *msi_data = priv;
	صحيح rc;

	rc = sbi_irqchip_process_hwirq(&msi_data->aplic->irqchip, msi_data->hwirq);
	إذا (rc && rc != SBI_ENOENT)
		sbi_printf("%s: hwirq=%lu failed rc=%d\n",
			__func__,
			(unsigned long)parent_hwirq, rc);

	أرجع rc;
}

ساكن صحيح aplic_setup_msi(بنية aplic_data *aplic, u32 hwirq)
{
	بنية sbi_irqchip_device *parent;
	u32 first_hwirq;
	بنية aplic_msi_data *msi_data;
	صحيح rc;

	parent = sbi_irqchip_find_device(aplic->parent_unique_id);
	إذا (!parent) {
		sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
			__func__, (unsigned long)hwirq);
		أرجع SBI_EINVAL;
	}

	msi_data = sbi_zalloc(حجم(بنية aplic_msi_data));
	إذا (!msi_data)
		أرجع SBI_ENOMEM;

	msi_data->aplic = aplic;
	msi_data->hwirq = hwirq;

	rc = sbi_irqchip_register_msi(parent, 1,
					aplic_write_msi,
					aplic_msi_callback,
					msi_data,
					&first_hwirq);
	إذا (rc) {
		sbi_printf("%s: register_msi failed hwirq=%lu rc=%d\n",
			__func__, (unsigned long)hwirq, rc);
		sbi_free(msi_data);
		أرجع rc;
	}

	msi_data->parent_hwirq = first_hwirq;
	sbi_irqchip_set_hwirq_priv(&aplic->irqchip, hwirq, msi_data);

	أرجع 0;
}

ساكن صحيح aplic_hwirq_setup(بنية sbi_irqchip_device *chip,
			     u32 hwirq, u32 hwirq_flags)
{
	بنية aplic_data *aplic;
	u32 sourcecfg;

	aplic = container_of(chip, بنية aplic_data, irqchip);

	sourcecfg = aplic_hwirq_flags_to_sourcecfg(hwirq_flags);
	إذا (sourcecfg == APLIC_SOURCECFG_SM_INACTIVE) {
		sbi_printf("%s: unsupported flags=0x%lx hwirq=%lu\n",
			__func__,
			(unsigned long)hwirq_flags,
			(unsigned long)hwirq);
		أرجع SBI_EINVAL;
	}

	aplic_sourcecfg_write(aplic, hwirq, sourcecfg);

	إذا (aplic_hwirq_is_delegated(aplic, hwirq)) {
		sbi_printf("%s: hwirq=%lu is delegated\n",
			__func__, (unsigned long)hwirq);
		أرجع SBI_ENOTSUPP;
	}

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);

	إذا (!aplic_is_direct_mode(aplic))
		أرجع aplic_setup_msi(aplic, hwirq);

	أرجع 0;
}

ساكن فراغ aplic_hwirq_cleanup(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية aplic_data *aplic;
	بنية aplic_msi_data *msi_data;

	aplic = container_of(chip, بنية aplic_data, irqchip);
	إذا (aplic_hwirq_is_delegated(aplic, hwirq))
		أرجع;

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);
	aplic_sourcecfg_write(aplic, hwirq, APLIC_SOURCECFG_SM_INACTIVE);
	aplic_target_write(aplic, hwirq, APLIC_DEFAULT_PRIORITY);
	msi_data = (بنية aplic_msi_data *) sbi_irqchip_get_hwirq_priv(chip, hwirq);
	إذا (msi_data)
		sbi_free(msi_data);
}

ساكن صحيح aplic_hwirq_set_affinity(بنية sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_index)
{
	بنية aplic_data *aplic;
	بنية aplic_msi_data *msi_data;
	بنية sbi_irqchip_device *parent;
	صحيح idc_index;
	صحيح rc;

	aplic = container_of(chip, بنية aplic_data, irqchip);
	إذا (aplic_hwirq_is_delegated(aplic, hwirq))
		أرجع SBI_ENOTSUPP;

	إذا (!aplic_is_direct_mode(aplic)) {
		parent = sbi_irqchip_find_device(aplic->parent_unique_id);
		إذا (!parent) {
			sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
				__func__, (unsigned long)hwirq);
			أرجع SBI_EINVAL;
		}

		msi_data = sbi_irqchip_get_hwirq_priv(chip, hwirq);
		rc = sbi_irqchip_set_affinity(parent, msi_data->parent_hwirq, hart_index);
		إذا (rc) {
			sbi_printf("%s: sbi_irqchip_set_affinity failed hwirq=%lu rc=%d\n",
				__func__, (unsigned long)hwirq, rc);
			أرجع rc;
		}
		أرجع 0;
	}

	idc_index = aplic_find_idc_index(aplic, hart_index);
	إذا (idc_index < 0)
		أرجع SBI_EINVAL;

	aplic_target_write(aplic, hwirq,
			   APLIC_TARGET_HART_IDX(hart_index) |
			   APLIC_TARGET_IPRIO(APLIC_DEFAULT_PRIORITY));

	أرجع 0;
}

ساكن فراغ aplic_hwirq_mask(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية aplic_data *aplic;

	aplic = container_of(chip, بنية aplic_data, irqchip);
	إذا (aplic_hwirq_is_delegated(aplic, hwirq))
		أرجع;

	aplic_irq_clrie(aplic, hwirq);
}

ساكن فراغ aplic_hwirq_unmask(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية aplic_data *aplic;

	aplic = container_of(chip, بنية aplic_data, irqchip);
	إذا (aplic_hwirq_is_delegated(aplic, hwirq))
		أرجع;

	aplic_irq_setie(aplic, hwirq);
}

ساكن بنية sbi_irqchip_device aplic_irqchip_template = {
	.warm_init			= aplic_warm_init,
	.process_hwirqs		= aplic_process_hwirqs,
	.hwirq_eoi			= aplic_hwirq_eoi,
	.hwirq_setup		= aplic_hwirq_setup,
	.hwirq_cleanup		= aplic_hwirq_cleanup,
	.hwirq_set_affinity	= aplic_hwirq_set_affinity,
	.hwirq_mask			= aplic_hwirq_mask,
	.hwirq_unmask		= aplic_hwirq_unmask,
};

صحيح aplic_cold_irqchip_init(بنية aplic_data *aplic)
{
	صحيح rc;
	بنية aplic_delegate_data *deleg;
	u32 first_deleg_irq, last_deleg_irq, i;
	bool msi_mode;

	msi_mode = !aplic_is_direct_mode(aplic);
	إذا (!aplic->ژمارە_source) {
		sbi_printf("%s: num_source is zero\n", __func__);
		أرجع SBI_EINVAL;
	}

	إذا (APLIC_MAX_SOURCE <= aplic->ژمارە_source) {
		sbi_printf("%s: num_source=%lu exceeds max=%lu\n",
			__func__,
			(unsigned long)aplic->ژمارە_source,
			(unsigned long)APLIC_MAX_SOURCE);
		أرجع SBI_EINVAL;
	}

	إذا (!msi_mode && APLIC_MAX_IDC <= aplic->ژمارە_idc) {
		sbi_printf("%s: num_idc=%lu exceeds max=%lu\n",
			__func__,
			(unsigned long)aplic->ژمارە_idc,
			(unsigned long)APLIC_MAX_IDC);
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
	first_deleg_irq = -1U;
	last_deleg_irq = 0;
	لكل (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		إذا (deleg->first_irq < first_deleg_irq)
			first_deleg_irq = deleg->first_irq;
		إذا (last_deleg_irq < deleg->last_irq)
			last_deleg_irq = deleg->last_irq;
	}

	إذا (aplic->targets_mmode ||
	    ((first_deleg_irq < last_deleg_irq) &&
	    (last_deleg_irq == aplic->ژمارە_source) &&
	    (first_deleg_irq == 1))) {
		rc = sbi_domain_ڕەگ_add_memrange(aplic->addr, aplic->size, PAGE_SIZE,
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
		aplic->irqchip.ژمارە_hwirq = aplic->ژمارە_source + 1;

		إذا (msi_mode) {
			aplic->irqchip.warm_init      = عدم;
			aplic->irqchip.process_hwirqs = عدم;
			aplic->irqchip.hwirq_eoi      = عدم;
		}

		إذا (msi_mode)
			sbi_hartmask_set_all(&aplic->irqchip.target_harts);
		وإلا
			لكل (i = 0; i < aplic->ژمارە_idc; i++)
				sbi_hartmask_set_hartindex(aplic->idc_map[i],
							   &aplic->irqchip.target_harts);

		rc = sbi_irqchip_add_device(&aplic->irqchip);
		إذا (rc) {
			sbi_printf("%s: sbi_irqchip_add_device failed rc=%d id=%lu mode=%s target_weight=%lu\n",
				__func__, rc,
				(unsigned long)aplic->irqchip.id,
				msi_mode ? "msi" : "direct",
				(unsigned long)sbi_hartmask_weight(
					&aplic->irqchip.target_harts));
			أرجع rc;
		}
	}
	/* Attach to the aplic list */
	sbi_list_add_tail(&aplic->گرێ, &aplic_list);

	أرجع 0;
}
