/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_domain.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi_utils/irqchip/aplic.h>

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
#define APLIC_SOURCECFG_SM_LEVEL_ઉપલું	0x6
#define APLIC_SOURCECFG_SM_LEVEL_નીચલું	0x7

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

સંરચના aplic_msi_data {
	સંરચના aplic_data *aplic;
	u32 hwirq;
	u32 parent_hwirq;
};

સ્થિર SBI_LIST_HEAD(aplic_list);
સ્થિર શૂન્ય aplic_writel_msicfg(સંરચના aplic_msicfg_data *msicfg,
				શૂન્ય *msicfgaddr, શૂન્ય *msicfgaddrH);

સ્થિર અંતઃરેખ bool aplic_is_direct_mode(સંરચના aplic_data *aplic)
{
	પાછા aplic && aplic->અંક_idc;
}

સ્થિર અંતઃરેખ શૂન્ય aplic_sourcecfg_write(સંરચના aplic_data *aplic,
					 u32 hwirq, u32 val)
{
	writel(val, (શૂન્ય *)(aplic->addr + APLIC_SOURCECFG_BASE +
			     (hwirq - 1) * કદ(u32)));
}

સ્થિર અંતઃરેખ u32 aplic_sourcecfg_read(સંરચના aplic_data *aplic, u32 hwirq)
{
	પાછા readl((શૂન્ય *)(aplic->addr + APLIC_SOURCECFG_BASE +
			      (hwirq - 1) * કદ(u32)));
}

સ્થિર અંતઃરેખ શૂન્ય aplic_target_write(સંરચના aplic_data *aplic,
				      u32 hwirq, u32 val)
{
	writel(val, (શૂન્ય *)(aplic->addr + APLIC_TARGET_BASE +
			     (hwirq - 1) * કદ(u32)));
}

સ્થિર અંતઃરેખ શૂન્ય aplic_irq_setie(સંરચના aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (શૂન્ય *)(aplic->addr + APLIC_SETIENUM));
}

સ્થિર અંતઃરેખ શૂન્ય aplic_irq_clrie(સંરચના aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (શૂન્ય *)(aplic->addr + APLIC_CLRIENUM));
}

સ્થિર અંતઃરેખ શૂન્ય aplic_irq_clrip(સંરચના aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (શૂન્ય *)(aplic->addr + APLIC_CLRIPNUM));
}

સ્થિર અંતઃરેખ શૂન્ય aplic_msi_irq_retrigger_level(સંરચના aplic_data *aplic, u32 hwirq)
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
	writel(hwirq, (શૂન્ય *)(aplic->addr + APLIC_SETIPNUM_LE));
}

સ્થિર અંતઃરેખ શૂન્ય *aplic_idc_base(સંરચના aplic_data *aplic, u32 idc_index)
{
	પાછા (શૂન્ય *)(aplic->addr + APLIC_IDC_BASE +
			idc_index * APLIC_IDC_SIZE);
}

સ્થિર અંતઃરેખ u32 aplic_idc_read(સંરચના aplic_data *aplic, u32 idc_index,
				 u32 reg)
{
	પાછા readl(aplic_idc_base(aplic, idc_index) + reg);
}

સ્થિર અંતઃરેખ શૂન્ય aplic_idc_write(સંરચના aplic_data *aplic, u32 idc_index,
				   u32 reg, u32 val)
{
	writel(val, aplic_idc_base(aplic, idc_index) + reg);
}

સ્થિર u32 aplic_hwirq_flags_to_sourcecfg(u32 hwirq_flags)
{
	પસંદગી (hwirq_flags & SBI_HWIRQ_FLAGS_LEVEL_SENSE_MASK) {
	સ્થિતિ SBI_HWIRQ_FLAGS_EDGE_RISING:
		પાછા APLIC_SOURCECFG_SM_EDGE_RISE;
	સ્થિતિ SBI_HWIRQ_FLAGS_EDGE_FALLING:
		પાછા APLIC_SOURCECFG_SM_EDGE_FALL;
	સ્થિતિ SBI_HWIRQ_FLAGS_LEVEL_ઉપલું:
		પાછા APLIC_SOURCECFG_SM_LEVEL_ઉપલું;
	સ્થિતિ SBI_HWIRQ_FLAGS_LEVEL_નીચલું:
		પાછા APLIC_SOURCECFG_SM_LEVEL_નીચલું;
	મૂળભૂત:
		પાછા APLIC_SOURCECFG_SM_INACTIVE;
	}
}

સ્થિર bool aplic_hwirq_is_delegated(સંરચના aplic_data *aplic, u32 hwirq)
{
	u32 sourcecfg;

	sourcecfg = aplic_sourcecfg_read(aplic, hwirq);
	પાછા !!(sourcecfg & APLIC_SOURCECFG_D);
}

સ્થિર પૂર્ણાંક aplic_find_idc_index(સંરચના aplic_data *aplic, u32 hart_index)
{
	u32 i;

	માટે (i = 0; i < aplic->અંક_idc; i++) {
		જો (aplic->idc_map[i] == hart_index)
			પાછા i;
	}

	પાછા SBI_ENOENT;
}

સ્થિર શૂન્ય aplic_init(સંરચના aplic_data *aplic)
{
	સંરચના aplic_delegate_data *deleg;
	u32 i, j, tmp, domaincfg;
	પૂર્ણાંક locked;

	/* Set domain configuration to 0 */
	writel(0, (શૂન્ય *)(aplic->addr + APLIC_DOMAINCFG));

	/* Disable all interrupts */
	માટે (i = 0; i <= aplic->અંક_source; i += 32)
		writel(-1U, (શૂન્ય *)(aplic->addr + APLIC_CLRIE_BASE +
				     (i / 32) * કદ(u32)));

	/* Set interrupt type and priority for all interrupts */
	માટે (i = 1; i <= aplic->અંક_source; i++) {
		/* Set IRQ source configuration to 0 */
		writel(0, (શૂન્ય *)(aplic->addr + APLIC_SOURCECFG_BASE +
			  (i - 1) * કદ(u32)));
		/* Set IRQ target hart index and priority to 1 */
		writel(APLIC_DEFAULT_PRIORITY, (શૂન્ય *)(aplic->addr +
						APLIC_TARGET_BASE +
						(i - 1) * કદ(u32)));
	}

	/* Configure IRQ delegation */
	માટે (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		જો (!deleg->first_irq || !deleg->last_irq)
			ચાલુ;
		જો (aplic->અંક_source < deleg->first_irq ||
		    aplic->અંક_source < deleg->last_irq)
			ચાલુ;
		જો (deleg->child_index > APLIC_SOURCECFG_CHILDIDX_MASK)
			ચાલુ;
		જો (deleg->first_irq > deleg->last_irq) {
			tmp = deleg->first_irq;
			deleg->first_irq = deleg->last_irq;
			deleg->last_irq = tmp;
		}
		માટે (j = deleg->first_irq; j <= deleg->last_irq; j++)
			aplic_sourcecfg_write(aplic, j, APLIC_SOURCECFG_D | deleg->child_index);
	}

	/* Default initialization of IDC structures */
	જો (aplic_is_direct_mode(aplic)) {
		માટે (i = 0; i < aplic->અંક_idc; i++) {
			aplic_idc_write(aplic, i, APLIC_IDC_IDELIVERY,
					APLIC_DISABLE_IDELIVERY);
			aplic_idc_write(aplic, i, APLIC_IDC_IFORCE, 0);
			aplic_idc_write(aplic, i, APLIC_IDC_ITHRESHOLD,
					APLIC_DISABLE_ITHRESHOLD);
		}
	} નહીંતર {
		/* MSI configuration */
		locked = readl((શૂન્ય *)(aplic->addr + APLIC_MMSICFGADDRH)) & APLIC_xMSICFGADDRH_L;
		જો (aplic->targets_mmode && aplic->has_msicfg_mmode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_mmode,
						(શૂન્ય *)(aplic->addr + APLIC_MMSICFGADDR),
						(શૂન્ય *)(aplic->addr + APLIC_MMSICFGADDRH));
		}
		જો (aplic->targets_mmode && aplic->has_msicfg_smode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_smode,
						(શૂન્ય *)(aplic->addr + APLIC_SMSICFGADDR),
						(શૂન્ય *)(aplic->addr + APLIC_SMSICFGADDRH));
		}
	}

	domaincfg = APLIC_DOMAINCFG_IE;
	જો (!aplic_is_direct_mode(aplic))
		domaincfg |= APLIC_DOMAINCFG_DM;

	writel(domaincfg, (શૂન્ય *)(aplic->addr + APLIC_DOMAINCFG));
}

શૂન્ય aplic_reinit_all(શૂન્ય)
{
	સંરચના aplic_data *aplic;

	sbi_list_for_each_entry(aplic, &aplic_list, ગાંઠ)
		aplic_init(aplic);
}

સ્થિર શૂન્ય aplic_writel_msicfg(સંરચના aplic_msicfg_data *msicfg,
				શૂન્ય *msicfgaddr, શૂન્ય *msicfgaddrH)
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

સ્થિર પૂર્ણાંક aplic_check_msicfg(સંરચના aplic_msicfg_data *msicfg)
{
	જો (APLIC_xMSICFGADDRH_LHXS_MASK < msicfg->lhxs)
		પાછા SBI_EINVAL;

	જો (APLIC_xMSICFGADDRH_LHXW_MASK < msicfg->lhxw)
		પાછા SBI_EINVAL;

	જો (APLIC_xMSICFGADDRH_HHXS_MASK < msicfg->hhxs)
		પાછા SBI_EINVAL;

	જો (APLIC_xMSICFGADDRH_HHXW_MASK < msicfg->hhxw)
		પાછા SBI_EINVAL;

	પાછા 0;
}

સ્થિર પૂર્ણાંક aplic_warm_init(સંરચના sbi_irqchip_device *chip)
{
	સંરચના aplic_data *aplic;
	u32 hart_index;
	પૂર્ણાંક idc_index;

	aplic = container_of(chip, સંરચના aplic_data, irqchip);
	જો (!aplic_is_direct_mode(aplic))
		પાછા 0;

	hart_index = current_hartindex();
	idc_index  = aplic_find_idc_index(aplic, hart_index);
	જો (idc_index < 0)
		પાછા 0;

	aplic_idc_write(aplic, idc_index, APLIC_IDC_ITHRESHOLD,
			APLIC_ENABLE_ITHRESHOLD);
	aplic_idc_write(aplic, idc_index, APLIC_IDC_IDELIVERY,
			APLIC_ENABLE_IDELIVERY);

	પાછા 0;
}

સ્થિર પૂર્ણાંક aplic_process_hwirqs(સંરચના sbi_irqchip_device *chip)
{
	સંરચના aplic_data *aplic;
	u32 hart_index, claimi, hwirq;
	પૂર્ણાંક idc_index, rc = 0, tmp;

	aplic = container_of(chip, સંરચના aplic_data, irqchip);

	જો (!aplic_is_direct_mode(aplic))
		sbi_panic("aplic_process_hwirqs called in MSI mode.\n");

	hart_index = current_hartindex();
	idc_index  = aplic_find_idc_index(aplic, hart_index);
	જો (idc_index < 0)
		પાછા 0;

	જ્યારે ((claimi = aplic_idc_read(aplic, idc_index, APLIC_IDC_CLAIMI))) {
		hwirq = (claimi >> APLIC_IDC_TOPI_ID_SHIFT) &
			APLIC_IDC_TOPI_ID_MASK;

		જો (!hwirq)
			તોડો;

		જો (aplic_hwirq_is_delegated(aplic, hwirq))
			ચાલુ;

		tmp = sbi_irqchip_process_hwirq(chip, hwirq);
		જો (tmp)
			rc = tmp;
	}

	પાછા rc;
}

સ્થિર શૂન્ય aplic_hwirq_eoi(સંરચના sbi_irqchip_device *chip, u32 hwirq)
{
	સંરચના aplic_data *aplic;
	u32 sm;

	aplic = container_of(chip, સંરચના aplic_data, irqchip);
	જો (aplic_is_direct_mode(aplic))
		પાછા;

	sm = aplic_sourcecfg_read(aplic, hwirq) & APLIC_SOURCECFG_SM_MASK;
	જો (sm == APLIC_SOURCECFG_SM_LEVEL_ઉપલું ||
	    sm == APLIC_SOURCECFG_SM_LEVEL_નીચલું)
		aplic_msi_irq_retrigger_level(aplic, hwirq);
}

સ્થિર શૂન્ય aplic_program_msi_target(સંરચના aplic_data *aplic,
				     u32 hwirq, u32 hart_index,
				     u32 guest_index, u32 eiid)
{
	u32 target;

	target = APLIC_TARGET_HART_IDX(hart_index) |
		 APLIC_TARGET_GUEST_IDX(guest_index) |
		 APLIC_TARGET_EIID(eiid);

	aplic_target_write(aplic, hwirq, target);
}

સ્થિર u32 derive_hart_index(સંરચના aplic_msicfg_data *msicfg,
			      અચલ સંરચના sbi_irqchip_msi_સંદેશ *સંદેશ)
{
	u64 addr = ((u64)સંદેશ->address_hi << 32) | સંદેશ->address_lo;
	u64 tppn = addr >> APLIC_xMSICFGADDR_PPN_SHIFT;
	u32 group_index, hart_index;

	group_index = (tppn >> APLIC_xMSICFGADDR_PPN_HHX_SHIFT(msicfg->hhxs)) &
		      APLIC_xMSICFGADDR_PPN_HHX_MASK(msicfg->hhxw);

	hart_index = (tppn >> msicfg->lhxs) &
		     ((1UL << msicfg->lhxw) - 1);

	hart_index |= (group_index << msicfg->lhxw);

	પાછા hart_index;
}

સ્થિર શૂન્ય aplic_write_msi(u32 parent_hwirq,
			    અચલ સંરચના sbi_irqchip_msi_સંદેશ *સંદેશ,
			    શૂન્ય *priv)
{
	સંરચના aplic_msi_data *msi_data = priv;
	u32 guest_index = 0;
	u32 eiid;

	eiid = સંદેશ->data;
	જો (!eiid || eiid > APLIC_TARGET_EIID_MASK)
		પાછા;

	aplic_program_msi_target(msi_data->aplic, msi_data->hwirq,
				derive_hart_index(&msi_data->aplic->msicfg_mmode, સંદેશ),
				guest_index, eiid);
}

સ્થિર પૂર્ણાંક aplic_msi_callback(u32 parent_hwirq, શૂન્ય *priv)
{
	સંરચના aplic_msi_data *msi_data = priv;
	પૂર્ણાંક rc;

	rc = sbi_irqchip_process_hwirq(&msi_data->aplic->irqchip, msi_data->hwirq);
	જો (rc && rc != SBI_ENOENT)
		sbi_printf("%s: hwirq=%lu failed rc=%d\n",
			__func__,
			(unsigned long)parent_hwirq, rc);

	પાછા rc;
}

સ્થિર પૂર્ણાંક aplic_setup_msi(સંરચના aplic_data *aplic, u32 hwirq)
{
	સંરચના sbi_irqchip_device *parent;
	u32 first_hwirq;
	સંરચના aplic_msi_data *msi_data;
	પૂર્ણાંક rc;

	parent = sbi_irqchip_find_device(aplic->parent_unique_id);
	જો (!parent) {
		sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
			__func__, (unsigned long)hwirq);
		પાછા SBI_EINVAL;
	}

	msi_data = sbi_zalloc(કદ(સંરચના aplic_msi_data));
	જો (!msi_data)
		પાછા SBI_ENOMEM;

	msi_data->aplic = aplic;
	msi_data->hwirq = hwirq;

	rc = sbi_irqchip_register_msi(parent, 1,
					aplic_write_msi,
					aplic_msi_callback,
					msi_data,
					&first_hwirq);
	જો (rc) {
		sbi_printf("%s: register_msi failed hwirq=%lu rc=%d\n",
			__func__, (unsigned long)hwirq, rc);
		sbi_free(msi_data);
		પાછા rc;
	}

	msi_data->parent_hwirq = first_hwirq;
	sbi_irqchip_set_hwirq_priv(&aplic->irqchip, hwirq, msi_data);

	પાછા 0;
}

સ્થિર પૂર્ણાંક aplic_hwirq_setup(સંરચના sbi_irqchip_device *chip,
			     u32 hwirq, u32 hwirq_flags)
{
	સંરચના aplic_data *aplic;
	u32 sourcecfg;

	aplic = container_of(chip, સંરચના aplic_data, irqchip);

	sourcecfg = aplic_hwirq_flags_to_sourcecfg(hwirq_flags);
	જો (sourcecfg == APLIC_SOURCECFG_SM_INACTIVE) {
		sbi_printf("%s: unsupported flags=0x%lx hwirq=%lu\n",
			__func__,
			(unsigned long)hwirq_flags,
			(unsigned long)hwirq);
		પાછા SBI_EINVAL;
	}

	aplic_sourcecfg_write(aplic, hwirq, sourcecfg);

	જો (aplic_hwirq_is_delegated(aplic, hwirq)) {
		sbi_printf("%s: hwirq=%lu is delegated\n",
			__func__, (unsigned long)hwirq);
		પાછા SBI_ENOTSUPP;
	}

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);

	જો (!aplic_is_direct_mode(aplic))
		પાછા aplic_setup_msi(aplic, hwirq);

	પાછા 0;
}

સ્થિર શૂન્ય aplic_hwirq_cleanup(સંરચના sbi_irqchip_device *chip, u32 hwirq)
{
	સંરચના aplic_data *aplic;
	સંરચના aplic_msi_data *msi_data;

	aplic = container_of(chip, સંરચના aplic_data, irqchip);
	જો (aplic_hwirq_is_delegated(aplic, hwirq))
		પાછા;

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);
	aplic_sourcecfg_write(aplic, hwirq, APLIC_SOURCECFG_SM_INACTIVE);
	aplic_target_write(aplic, hwirq, APLIC_DEFAULT_PRIORITY);
	msi_data = (સંરચના aplic_msi_data *) sbi_irqchip_get_hwirq_priv(chip, hwirq);
	જો (msi_data)
		sbi_free(msi_data);
}

સ્થિર પૂર્ણાંક aplic_hwirq_set_affinity(સંરચના sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_index)
{
	સંરચના aplic_data *aplic;
	સંરચના aplic_msi_data *msi_data;
	સંરચના sbi_irqchip_device *parent;
	પૂર્ણાંક idc_index;
	પૂર્ણાંક rc;

	aplic = container_of(chip, સંરચના aplic_data, irqchip);
	જો (aplic_hwirq_is_delegated(aplic, hwirq))
		પાછા SBI_ENOTSUPP;

	જો (!aplic_is_direct_mode(aplic)) {
		parent = sbi_irqchip_find_device(aplic->parent_unique_id);
		જો (!parent) {
			sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
				__func__, (unsigned long)hwirq);
			પાછા SBI_EINVAL;
		}

		msi_data = sbi_irqchip_get_hwirq_priv(chip, hwirq);
		rc = sbi_irqchip_set_affinity(parent, msi_data->parent_hwirq, hart_index);
		જો (rc) {
			sbi_printf("%s: sbi_irqchip_set_affinity failed hwirq=%lu rc=%d\n",
				__func__, (unsigned long)hwirq, rc);
			પાછા rc;
		}
		પાછા 0;
	}

	idc_index = aplic_find_idc_index(aplic, hart_index);
	જો (idc_index < 0)
		પાછા SBI_EINVAL;

	aplic_target_write(aplic, hwirq,
			   APLIC_TARGET_HART_IDX(hart_index) |
			   APLIC_TARGET_IPRIO(APLIC_DEFAULT_PRIORITY));

	પાછા 0;
}

સ્થિર શૂન્ય aplic_hwirq_mask(સંરચના sbi_irqchip_device *chip, u32 hwirq)
{
	સંરચના aplic_data *aplic;

	aplic = container_of(chip, સંરચના aplic_data, irqchip);
	જો (aplic_hwirq_is_delegated(aplic, hwirq))
		પાછા;

	aplic_irq_clrie(aplic, hwirq);
}

સ્થિર શૂન્ય aplic_hwirq_unmask(સંરચના sbi_irqchip_device *chip, u32 hwirq)
{
	સંરચના aplic_data *aplic;

	aplic = container_of(chip, સંરચના aplic_data, irqchip);
	જો (aplic_hwirq_is_delegated(aplic, hwirq))
		પાછા;

	aplic_irq_setie(aplic, hwirq);
}

સ્થિર સંરચના sbi_irqchip_device aplic_irqchip_template = {
	.warm_init			= aplic_warm_init,
	.process_hwirqs		= aplic_process_hwirqs,
	.hwirq_eoi			= aplic_hwirq_eoi,
	.hwirq_setup		= aplic_hwirq_setup,
	.hwirq_cleanup		= aplic_hwirq_cleanup,
	.hwirq_set_affinity	= aplic_hwirq_set_affinity,
	.hwirq_mask			= aplic_hwirq_mask,
	.hwirq_unmask		= aplic_hwirq_unmask,
};

પૂર્ણાંક aplic_cold_irqchip_init(સંરચના aplic_data *aplic)
{
	પૂર્ણાંક rc;
	સંરચના aplic_delegate_data *deleg;
	u32 first_deleg_irq, last_deleg_irq, i;
	bool msi_mode;

	msi_mode = !aplic_is_direct_mode(aplic);
	જો (!aplic->અંક_source) {
		sbi_printf("%s: num_source is zero\n", __func__);
		પાછા SBI_EINVAL;
	}

	જો (APLIC_MAX_SOURCE <= aplic->અંક_source) {
		sbi_printf("%s: num_source=%lu exceeds max=%lu\n",
			__func__,
			(unsigned long)aplic->અંક_source,
			(unsigned long)APLIC_MAX_SOURCE);
		પાછા SBI_EINVAL;
	}

	જો (!msi_mode && APLIC_MAX_IDC <= aplic->અંક_idc) {
		sbi_printf("%s: num_idc=%lu exceeds max=%lu\n",
			__func__,
			(unsigned long)aplic->અંક_idc,
			(unsigned long)APLIC_MAX_IDC);
		પાછા SBI_EINVAL;
	}

	જો (aplic->targets_mmode && aplic->has_msicfg_mmode) {
		rc = aplic_check_msicfg(&aplic->msicfg_mmode);
		જો (rc) {
			sbi_printf("%s: invalid M-mode msicfg rc=%d\n",
				__func__, rc);
			પાછા rc;
		}
	}

	જો (aplic->targets_mmode && aplic->has_msicfg_smode) {
		rc = aplic_check_msicfg(&aplic->msicfg_smode);
		જો (rc) {
			sbi_printf("%s: invalid S-mode msicfg rc=%d\n",
				__func__, rc);
			પાછા rc;
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
	માટે (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		જો (deleg->first_irq < first_deleg_irq)
			first_deleg_irq = deleg->first_irq;
		જો (last_deleg_irq < deleg->last_irq)
			last_deleg_irq = deleg->last_irq;
	}

	જો (aplic->targets_mmode ||
	    ((first_deleg_irq < last_deleg_irq) &&
	    (last_deleg_irq == aplic->અંક_source) &&
	    (first_deleg_irq == 1))) {
		rc = sbi_domain_મૂળ_add_memrange(aplic->addr, aplic->size, PAGE_SIZE,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		જો (rc)
			પાછા rc;
	}

	જો ((aplic->targets_mmode)) {
		aplic->irqchip		 = aplic_irqchip_template;
		aplic->irqchip.id	 = aplic->unique_id;
		aplic->irqchip.caps = SBI_IRQCHIP_CAPS_WIRED;
		aplic->irqchip.અંક_hwirq = aplic->અંક_source + 1;

		જો (msi_mode) {
			aplic->irqchip.warm_init      = ખાલી;
			aplic->irqchip.process_hwirqs = ખાલી;
			aplic->irqchip.hwirq_eoi      = ખાલી;
		}

		જો (msi_mode)
			sbi_hartmask_set_all(&aplic->irqchip.target_harts);
		નહીંતર
			માટે (i = 0; i < aplic->અંક_idc; i++)
				sbi_hartmask_set_hartindex(aplic->idc_map[i],
							   &aplic->irqchip.target_harts);

		rc = sbi_irqchip_add_device(&aplic->irqchip);
		જો (rc) {
			sbi_printf("%s: sbi_irqchip_add_device failed rc=%d id=%lu mode=%s target_weight=%lu\n",
				__func__, rc,
				(unsigned long)aplic->irqchip.id,
				msi_mode ? "msi" : "direct",
				(unsigned long)sbi_hartmask_weight(
					&aplic->irqchip.target_harts));
			પાછા rc;
		}
	}
	/* Attach to the aplic list */
	sbi_list_add_tail(&aplic->ગાંઠ, &aplic_list);

	પાછા 0;
}
