/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi_utils/irqchip/aplic.h>

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
#define APLIC_SOURCECFG_SM_LEVEL_वरचा	0x6
#define APLIC_SOURCECFG_SM_LEVEL_खालचा	0x7

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

संरचना aplic_msi_data {
	संरचना aplic_data *aplic;
	u32 hwirq;
	u32 parent_hwirq;
};

स्थिर SBI_LIST_HEAD(aplic_list);
स्थिर शून्य aplic_writel_msicfg(संरचना aplic_msicfg_data *msicfg,
				शून्य *msicfgaddr, शून्य *msicfgaddrH);

स्थिर अन्तःस्थ bool aplic_is_direct_mode(संरचना aplic_data *aplic)
{
	प्रत्यागम aplic && aplic->अंक_idc;
}

स्थिर अन्तःस्थ शून्य aplic_sourcecfg_write(संरचना aplic_data *aplic,
					 u32 hwirq, u32 val)
{
	writel(val, (शून्य *)(aplic->addr + APLIC_SOURCECFG_BASE +
			     (hwirq - 1) * परिमाण(u32)));
}

स्थिर अन्तःस्थ u32 aplic_sourcecfg_read(संरचना aplic_data *aplic, u32 hwirq)
{
	प्रत्यागम readl((शून्य *)(aplic->addr + APLIC_SOURCECFG_BASE +
			      (hwirq - 1) * परिमाण(u32)));
}

स्थिर अन्तःस्थ शून्य aplic_target_write(संरचना aplic_data *aplic,
				      u32 hwirq, u32 val)
{
	writel(val, (शून्य *)(aplic->addr + APLIC_TARGET_BASE +
			     (hwirq - 1) * परिमाण(u32)));
}

स्थिर अन्तःस्थ शून्य aplic_irq_setie(संरचना aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (शून्य *)(aplic->addr + APLIC_SETIENUM));
}

स्थिर अन्तःस्थ शून्य aplic_irq_clrie(संरचना aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (शून्य *)(aplic->addr + APLIC_CLRIENUM));
}

स्थिर अन्तःस्थ शून्य aplic_irq_clrip(संरचना aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (शून्य *)(aplic->addr + APLIC_CLRIPNUM));
}

स्थिर अन्तःस्थ शून्य aplic_msi_irq_retrigger_level(संरचना aplic_data *aplic, u32 hwirq)
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
	writel(hwirq, (शून्य *)(aplic->addr + APLIC_SETIPNUM_LE));
}

स्थिर अन्तःस्थ शून्य *aplic_idc_base(संरचना aplic_data *aplic, u32 idc_index)
{
	प्रत्यागम (शून्य *)(aplic->addr + APLIC_IDC_BASE +
			idc_index * APLIC_IDC_SIZE);
}

स्थिर अन्तःस्थ u32 aplic_idc_read(संरचना aplic_data *aplic, u32 idc_index,
				 u32 reg)
{
	प्रत्यागम readl(aplic_idc_base(aplic, idc_index) + reg);
}

स्थिर अन्तःस्थ शून्य aplic_idc_write(संरचना aplic_data *aplic, u32 idc_index,
				   u32 reg, u32 val)
{
	writel(val, aplic_idc_base(aplic, idc_index) + reg);
}

स्थिर u32 aplic_hwirq_flags_to_sourcecfg(u32 hwirq_flags)
{
	विकल्प (hwirq_flags & SBI_HWIRQ_FLAGS_LEVEL_SENSE_MASK) {
	पक्ष SBI_HWIRQ_FLAGS_EDGE_RISING:
		प्रत्यागम APLIC_SOURCECFG_SM_EDGE_RISE;
	पक्ष SBI_HWIRQ_FLAGS_EDGE_FALLING:
		प्रत्यागम APLIC_SOURCECFG_SM_EDGE_FALL;
	पक्ष SBI_HWIRQ_FLAGS_LEVEL_वरचा:
		प्रत्यागम APLIC_SOURCECFG_SM_LEVEL_वरचा;
	पक्ष SBI_HWIRQ_FLAGS_LEVEL_खालचा:
		प्रत्यागम APLIC_SOURCECFG_SM_LEVEL_खालचा;
	स्वभाव:
		प्रत्यागम APLIC_SOURCECFG_SM_INACTIVE;
	}
}

स्थिर bool aplic_hwirq_is_delegated(संरचना aplic_data *aplic, u32 hwirq)
{
	u32 sourcecfg;

	sourcecfg = aplic_sourcecfg_read(aplic, hwirq);
	प्रत्यागम !!(sourcecfg & APLIC_SOURCECFG_D);
}

स्थिर पूर्णाङ्क aplic_find_idc_index(संरचना aplic_data *aplic, u32 hart_index)
{
	u32 i;

	प्रति (i = 0; i < aplic->अंक_idc; i++) {
		यदि (aplic->idc_map[i] == hart_index)
			प्रत्यागम i;
	}

	प्रत्यागम SBI_ENOENT;
}

स्थिर शून्य aplic_init(संरचना aplic_data *aplic)
{
	संरचना aplic_delegate_data *deleg;
	u32 i, j, tmp, domaincfg;
	पूर्णाङ्क locked;

	/* Set domain configuration to 0 */
	writel(0, (शून्य *)(aplic->addr + APLIC_DOMAINCFG));

	/* Disable all interrupts */
	प्रति (i = 0; i <= aplic->अंक_source; i += 32)
		writel(-1U, (शून्य *)(aplic->addr + APLIC_CLRIE_BASE +
				     (i / 32) * परिमाण(u32)));

	/* Set interrupt type and priority for all interrupts */
	प्रति (i = 1; i <= aplic->अंक_source; i++) {
		/* Set IRQ source configuration to 0 */
		writel(0, (शून्य *)(aplic->addr + APLIC_SOURCECFG_BASE +
			  (i - 1) * परिमाण(u32)));
		/* Set IRQ target hart index and priority to 1 */
		writel(APLIC_DEFAULT_PRIORITY, (शून्य *)(aplic->addr +
						APLIC_TARGET_BASE +
						(i - 1) * परिमाण(u32)));
	}

	/* Configure IRQ delegation */
	प्रति (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		यदि (!deleg->first_irq || !deleg->last_irq)
			अनुवर्त;
		यदि (aplic->अंक_source < deleg->first_irq ||
		    aplic->अंक_source < deleg->last_irq)
			अनुवर्त;
		यदि (deleg->child_index > APLIC_SOURCECFG_CHILDIDX_MASK)
			अनुवर्त;
		यदि (deleg->first_irq > deleg->last_irq) {
			tmp = deleg->first_irq;
			deleg->first_irq = deleg->last_irq;
			deleg->last_irq = tmp;
		}
		प्रति (j = deleg->first_irq; j <= deleg->last_irq; j++)
			aplic_sourcecfg_write(aplic, j, APLIC_SOURCECFG_D | deleg->child_index);
	}

	/* Default initialization of IDC structures */
	यदि (aplic_is_direct_mode(aplic)) {
		प्रति (i = 0; i < aplic->अंक_idc; i++) {
			aplic_idc_write(aplic, i, APLIC_IDC_IDELIVERY,
					APLIC_DISABLE_IDELIVERY);
			aplic_idc_write(aplic, i, APLIC_IDC_IFORCE, 0);
			aplic_idc_write(aplic, i, APLIC_IDC_ITHRESHOLD,
					APLIC_DISABLE_ITHRESHOLD);
		}
	} अन्यथा {
		/* MSI configuration */
		locked = readl((शून्य *)(aplic->addr + APLIC_MMSICFGADDRH)) & APLIC_xMSICFGADDRH_L;
		यदि (aplic->targets_mmode && aplic->has_msicfg_mmode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_mmode,
						(शून्य *)(aplic->addr + APLIC_MMSICFGADDR),
						(शून्य *)(aplic->addr + APLIC_MMSICFGADDRH));
		}
		यदि (aplic->targets_mmode && aplic->has_msicfg_smode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_smode,
						(शून्य *)(aplic->addr + APLIC_SMSICFGADDR),
						(शून्य *)(aplic->addr + APLIC_SMSICFGADDRH));
		}
	}

	domaincfg = APLIC_DOMAINCFG_IE;
	यदि (!aplic_is_direct_mode(aplic))
		domaincfg |= APLIC_DOMAINCFG_DM;

	writel(domaincfg, (शून्य *)(aplic->addr + APLIC_DOMAINCFG));
}

शून्य aplic_reinit_all(शून्य)
{
	संरचना aplic_data *aplic;

	sbi_list_for_each_entry(aplic, &aplic_list, गाठ)
		aplic_init(aplic);
}

स्थिर शून्य aplic_writel_msicfg(संरचना aplic_msicfg_data *msicfg,
				शून्य *msicfgaddr, शून्य *msicfgaddrH)
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

स्थिर पूर्णाङ्क aplic_check_msicfg(संरचना aplic_msicfg_data *msicfg)
{
	यदि (APLIC_xMSICFGADDRH_LHXS_MASK < msicfg->lhxs)
		प्रत्यागम SBI_EINVAL;

	यदि (APLIC_xMSICFGADDRH_LHXW_MASK < msicfg->lhxw)
		प्रत्यागम SBI_EINVAL;

	यदि (APLIC_xMSICFGADDRH_HHXS_MASK < msicfg->hhxs)
		प्रत्यागम SBI_EINVAL;

	यदि (APLIC_xMSICFGADDRH_HHXW_MASK < msicfg->hhxw)
		प्रत्यागम SBI_EINVAL;

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क aplic_warm_init(संरचना sbi_irqchip_device *chip)
{
	संरचना aplic_data *aplic;
	u32 hart_index;
	पूर्णाङ्क idc_index;

	aplic = container_of(chip, संरचना aplic_data, irqchip);
	यदि (!aplic_is_direct_mode(aplic))
		प्रत्यागम 0;

	hart_index = current_hartindex();
	idc_index  = aplic_find_idc_index(aplic, hart_index);
	यदि (idc_index < 0)
		प्रत्यागम 0;

	aplic_idc_write(aplic, idc_index, APLIC_IDC_ITHRESHOLD,
			APLIC_ENABLE_ITHRESHOLD);
	aplic_idc_write(aplic, idc_index, APLIC_IDC_IDELIVERY,
			APLIC_ENABLE_IDELIVERY);

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क aplic_process_hwirqs(संरचना sbi_irqchip_device *chip)
{
	संरचना aplic_data *aplic;
	u32 hart_index, claimi, hwirq;
	पूर्णाङ्क idc_index, rc = 0, tmp;

	aplic = container_of(chip, संरचना aplic_data, irqchip);

	यदि (!aplic_is_direct_mode(aplic))
		sbi_panic("aplic_process_hwirqs called in MSI mode.\n");

	hart_index = current_hartindex();
	idc_index  = aplic_find_idc_index(aplic, hart_index);
	यदि (idc_index < 0)
		प्रत्यागम 0;

	यावत् ((claimi = aplic_idc_read(aplic, idc_index, APLIC_IDC_CLAIMI))) {
		hwirq = (claimi >> APLIC_IDC_TOPI_ID_SHIFT) &
			APLIC_IDC_TOPI_ID_MASK;

		यदि (!hwirq)
			भङ्ग;

		यदि (aplic_hwirq_is_delegated(aplic, hwirq))
			अनुवर्त;

		tmp = sbi_irqchip_process_hwirq(chip, hwirq);
		यदि (tmp)
			rc = tmp;
	}

	प्रत्यागम rc;
}

स्थिर शून्य aplic_hwirq_eoi(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना aplic_data *aplic;
	u32 sm;

	aplic = container_of(chip, संरचना aplic_data, irqchip);
	यदि (aplic_is_direct_mode(aplic))
		प्रत्यागम;

	sm = aplic_sourcecfg_read(aplic, hwirq) & APLIC_SOURCECFG_SM_MASK;
	यदि (sm == APLIC_SOURCECFG_SM_LEVEL_वरचा ||
	    sm == APLIC_SOURCECFG_SM_LEVEL_खालचा)
		aplic_msi_irq_retrigger_level(aplic, hwirq);
}

स्थिर शून्य aplic_program_msi_target(संरचना aplic_data *aplic,
				     u32 hwirq, u32 hart_index,
				     u32 guest_index, u32 eiid)
{
	u32 target;

	target = APLIC_TARGET_HART_IDX(hart_index) |
		 APLIC_TARGET_GUEST_IDX(guest_index) |
		 APLIC_TARGET_EIID(eiid);

	aplic_target_write(aplic, hwirq, target);
}

स्थिर u32 derive_hart_index(संरचना aplic_msicfg_data *msicfg,
			      ध्रुव संरचना sbi_irqchip_msi_संदेश *संदेश)
{
	u64 addr = ((u64)संदेश->address_hi << 32) | संदेश->address_lo;
	u64 tppn = addr >> APLIC_xMSICFGADDR_PPN_SHIFT;
	u32 group_index, hart_index;

	group_index = (tppn >> APLIC_xMSICFGADDR_PPN_HHX_SHIFT(msicfg->hhxs)) &
		      APLIC_xMSICFGADDR_PPN_HHX_MASK(msicfg->hhxw);

	hart_index = (tppn >> msicfg->lhxs) &
		     ((1UL << msicfg->lhxw) - 1);

	hart_index |= (group_index << msicfg->lhxw);

	प्रत्यागम hart_index;
}

स्थिर शून्य aplic_write_msi(u32 parent_hwirq,
			    ध्रुव संरचना sbi_irqchip_msi_संदेश *संदेश,
			    शून्य *priv)
{
	संरचना aplic_msi_data *msi_data = priv;
	u32 guest_index = 0;
	u32 eiid;

	eiid = संदेश->data;
	यदि (!eiid || eiid > APLIC_TARGET_EIID_MASK)
		प्रत्यागम;

	aplic_program_msi_target(msi_data->aplic, msi_data->hwirq,
				derive_hart_index(&msi_data->aplic->msicfg_mmode, संदेश),
				guest_index, eiid);
}

स्थिर पूर्णाङ्क aplic_msi_callback(u32 parent_hwirq, शून्य *priv)
{
	संरचना aplic_msi_data *msi_data = priv;
	पूर्णाङ्क rc;

	rc = sbi_irqchip_process_hwirq(&msi_data->aplic->irqchip, msi_data->hwirq);
	यदि (rc && rc != SBI_ENOENT)
		sbi_printf("%s: hwirq=%lu failed rc=%d\n",
			__func__,
			(unsigned long)parent_hwirq, rc);

	प्रत्यागम rc;
}

स्थिर पूर्णाङ्क aplic_setup_msi(संरचना aplic_data *aplic, u32 hwirq)
{
	संरचना sbi_irqchip_device *parent;
	u32 first_hwirq;
	संरचना aplic_msi_data *msi_data;
	पूर्णाङ्क rc;

	parent = sbi_irqchip_find_device(aplic->parent_unique_id);
	यदि (!parent) {
		sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
			__func__, (unsigned long)hwirq);
		प्रत्यागम SBI_EINVAL;
	}

	msi_data = sbi_zalloc(परिमाण(संरचना aplic_msi_data));
	यदि (!msi_data)
		प्रत्यागम SBI_ENOMEM;

	msi_data->aplic = aplic;
	msi_data->hwirq = hwirq;

	rc = sbi_irqchip_register_msi(parent, 1,
					aplic_write_msi,
					aplic_msi_callback,
					msi_data,
					&first_hwirq);
	यदि (rc) {
		sbi_printf("%s: register_msi failed hwirq=%lu rc=%d\n",
			__func__, (unsigned long)hwirq, rc);
		sbi_free(msi_data);
		प्रत्यागम rc;
	}

	msi_data->parent_hwirq = first_hwirq;
	sbi_irqchip_set_hwirq_priv(&aplic->irqchip, hwirq, msi_data);

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क aplic_hwirq_setup(संरचना sbi_irqchip_device *chip,
			     u32 hwirq, u32 hwirq_flags)
{
	संरचना aplic_data *aplic;
	u32 sourcecfg;

	aplic = container_of(chip, संरचना aplic_data, irqchip);

	sourcecfg = aplic_hwirq_flags_to_sourcecfg(hwirq_flags);
	यदि (sourcecfg == APLIC_SOURCECFG_SM_INACTIVE) {
		sbi_printf("%s: unsupported flags=0x%lx hwirq=%lu\n",
			__func__,
			(unsigned long)hwirq_flags,
			(unsigned long)hwirq);
		प्रत्यागम SBI_EINVAL;
	}

	aplic_sourcecfg_write(aplic, hwirq, sourcecfg);

	यदि (aplic_hwirq_is_delegated(aplic, hwirq)) {
		sbi_printf("%s: hwirq=%lu is delegated\n",
			__func__, (unsigned long)hwirq);
		प्रत्यागम SBI_ENOTSUPP;
	}

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);

	यदि (!aplic_is_direct_mode(aplic))
		प्रत्यागम aplic_setup_msi(aplic, hwirq);

	प्रत्यागम 0;
}

स्थिर शून्य aplic_hwirq_cleanup(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना aplic_data *aplic;
	संरचना aplic_msi_data *msi_data;

	aplic = container_of(chip, संरचना aplic_data, irqchip);
	यदि (aplic_hwirq_is_delegated(aplic, hwirq))
		प्रत्यागम;

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);
	aplic_sourcecfg_write(aplic, hwirq, APLIC_SOURCECFG_SM_INACTIVE);
	aplic_target_write(aplic, hwirq, APLIC_DEFAULT_PRIORITY);
	msi_data = (संरचना aplic_msi_data *) sbi_irqchip_get_hwirq_priv(chip, hwirq);
	यदि (msi_data)
		sbi_free(msi_data);
}

स्थिर पूर्णाङ्क aplic_hwirq_set_affinity(संरचना sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_index)
{
	संरचना aplic_data *aplic;
	संरचना aplic_msi_data *msi_data;
	संरचना sbi_irqchip_device *parent;
	पूर्णाङ्क idc_index;
	पूर्णाङ्क rc;

	aplic = container_of(chip, संरचना aplic_data, irqchip);
	यदि (aplic_hwirq_is_delegated(aplic, hwirq))
		प्रत्यागम SBI_ENOTSUPP;

	यदि (!aplic_is_direct_mode(aplic)) {
		parent = sbi_irqchip_find_device(aplic->parent_unique_id);
		यदि (!parent) {
			sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
				__func__, (unsigned long)hwirq);
			प्रत्यागम SBI_EINVAL;
		}

		msi_data = sbi_irqchip_get_hwirq_priv(chip, hwirq);
		rc = sbi_irqchip_set_affinity(parent, msi_data->parent_hwirq, hart_index);
		यदि (rc) {
			sbi_printf("%s: sbi_irqchip_set_affinity failed hwirq=%lu rc=%d\n",
				__func__, (unsigned long)hwirq, rc);
			प्रत्यागम rc;
		}
		प्रत्यागम 0;
	}

	idc_index = aplic_find_idc_index(aplic, hart_index);
	यदि (idc_index < 0)
		प्रत्यागम SBI_EINVAL;

	aplic_target_write(aplic, hwirq,
			   APLIC_TARGET_HART_IDX(hart_index) |
			   APLIC_TARGET_IPRIO(APLIC_DEFAULT_PRIORITY));

	प्रत्यागम 0;
}

स्थिर शून्य aplic_hwirq_mask(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना aplic_data *aplic;

	aplic = container_of(chip, संरचना aplic_data, irqchip);
	यदि (aplic_hwirq_is_delegated(aplic, hwirq))
		प्रत्यागम;

	aplic_irq_clrie(aplic, hwirq);
}

स्थिर शून्य aplic_hwirq_unmask(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना aplic_data *aplic;

	aplic = container_of(chip, संरचना aplic_data, irqchip);
	यदि (aplic_hwirq_is_delegated(aplic, hwirq))
		प्रत्यागम;

	aplic_irq_setie(aplic, hwirq);
}

स्थिर संरचना sbi_irqchip_device aplic_irqchip_template = {
	.warm_init			= aplic_warm_init,
	.process_hwirqs		= aplic_process_hwirqs,
	.hwirq_eoi			= aplic_hwirq_eoi,
	.hwirq_setup		= aplic_hwirq_setup,
	.hwirq_cleanup		= aplic_hwirq_cleanup,
	.hwirq_set_affinity	= aplic_hwirq_set_affinity,
	.hwirq_mask			= aplic_hwirq_mask,
	.hwirq_unmask		= aplic_hwirq_unmask,
};

पूर्णाङ्क aplic_cold_irqchip_init(संरचना aplic_data *aplic)
{
	पूर्णाङ्क rc;
	संरचना aplic_delegate_data *deleg;
	u32 first_deleg_irq, last_deleg_irq, i;
	bool msi_mode;

	msi_mode = !aplic_is_direct_mode(aplic);
	यदि (!aplic->अंक_source) {
		sbi_printf("%s: num_source is zero\n", __func__);
		प्रत्यागम SBI_EINVAL;
	}

	यदि (APLIC_MAX_SOURCE <= aplic->अंक_source) {
		sbi_printf("%s: num_source=%lu exceeds max=%lu\n",
			__func__,
			(unsigned long)aplic->अंक_source,
			(unsigned long)APLIC_MAX_SOURCE);
		प्रत्यागम SBI_EINVAL;
	}

	यदि (!msi_mode && APLIC_MAX_IDC <= aplic->अंक_idc) {
		sbi_printf("%s: num_idc=%lu exceeds max=%lu\n",
			__func__,
			(unsigned long)aplic->अंक_idc,
			(unsigned long)APLIC_MAX_IDC);
		प्रत्यागम SBI_EINVAL;
	}

	यदि (aplic->targets_mmode && aplic->has_msicfg_mmode) {
		rc = aplic_check_msicfg(&aplic->msicfg_mmode);
		यदि (rc) {
			sbi_printf("%s: invalid M-mode msicfg rc=%d\n",
				__func__, rc);
			प्रत्यागम rc;
		}
	}

	यदि (aplic->targets_mmode && aplic->has_msicfg_smode) {
		rc = aplic_check_msicfg(&aplic->msicfg_smode);
		यदि (rc) {
			sbi_printf("%s: invalid S-mode msicfg rc=%d\n",
				__func__, rc);
			प्रत्यागम rc;
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
	प्रति (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		यदि (deleg->first_irq < first_deleg_irq)
			first_deleg_irq = deleg->first_irq;
		यदि (last_deleg_irq < deleg->last_irq)
			last_deleg_irq = deleg->last_irq;
	}

	यदि (aplic->targets_mmode ||
	    ((first_deleg_irq < last_deleg_irq) &&
	    (last_deleg_irq == aplic->अंक_source) &&
	    (first_deleg_irq == 1))) {
		rc = sbi_domain_मूळ_add_memrange(aplic->addr, aplic->size, PAGE_SIZE,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		यदि (rc)
			प्रत्यागम rc;
	}

	यदि ((aplic->targets_mmode)) {
		aplic->irqchip		 = aplic_irqchip_template;
		aplic->irqchip.id	 = aplic->unique_id;
		aplic->irqchip.caps = SBI_IRQCHIP_CAPS_WIRED;
		aplic->irqchip.अंक_hwirq = aplic->अंक_source + 1;

		यदि (msi_mode) {
			aplic->irqchip.warm_init      = रिक्त;
			aplic->irqchip.process_hwirqs = रिक्त;
			aplic->irqchip.hwirq_eoi      = रिक्त;
		}

		यदि (msi_mode)
			sbi_hartmask_set_all(&aplic->irqchip.target_harts);
		अन्यथा
			प्रति (i = 0; i < aplic->अंक_idc; i++)
				sbi_hartmask_set_hartindex(aplic->idc_map[i],
							   &aplic->irqchip.target_harts);

		rc = sbi_irqchip_add_device(&aplic->irqchip);
		यदि (rc) {
			sbi_printf("%s: sbi_irqchip_add_device failed rc=%d id=%lu mode=%s target_weight=%lu\n",
				__func__, rc,
				(unsigned long)aplic->irqchip.id,
				msi_mode ? "msi" : "direct",
				(unsigned long)sbi_hartmask_weight(
					&aplic->irqchip.target_harts));
			प्रत्यागम rc;
		}
	}
	/* Attach to the aplic list */
	sbi_list_add_tail(&aplic->गाठ, &aplic_list);

	प्रत्यागम 0;
}
