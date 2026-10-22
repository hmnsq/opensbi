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

#परिभाषय APLIC_MAX_IDC			(1UL << 14)
#परिभाषय APLIC_MAX_SOURCE		1024

#परिभाषय APLIC_DOMAINCFG		0x0000
#परिभाषय APLIC_DOMAINCFG_IE		(1 << 8)
#परिभाषय APLIC_DOMAINCFG_DM		(1 << 2)
#परिभाषय APLIC_DOMAINCFG_BE		(1 << 0)

#परिभाषय APLIC_SOURCECFG_आधारः		0x0004
#परिभाषय APLIC_SOURCECFG_D		(1 << 10)
#परिभाषय APLIC_SOURCECFG_CHILDIDX_आवरणम्	0x000003ff
#परिभाषय APLIC_SOURCECFG_SM_आवरणम्	0x00000007
#परिभाषय APLIC_SOURCECFG_SM_INACTIVE	0x0
#परिभाषय APLIC_SOURCECFG_SM_DETACH	0x1
#परिभाषय APLIC_SOURCECFG_SM_EDGE_RISE	0x4
#परिभाषय APLIC_SOURCECFG_SM_EDGE_FALL	0x5
#परिभाषय APLIC_SOURCECFG_SM_LEVEL_उच्चः	0x6
#परिभाषय APLIC_SOURCECFG_SM_LEVEL_नीचः	0x7

#परिभाषय APLIC_MMSICFGADDR		0x1bc0
#परिभाषय APLIC_MMSICFGADDRH		0x1bc4
#परिभाषय APLIC_SMSICFGADDR		0x1bc8
#परिभाषय APLIC_SMSICFGADDRH		0x1bcc

#परिभाषय APLIC_xMSICFGADDRH_L		(1UL << 31)
#परिभाषय APLIC_x_MSICFGADDRH_HHXS_आवरणम्	0x1f
#परिभाषय APLIC_x_MSICFGADDRH_HHXS_सरणम्	24
#परिभाषय APLIC_x_MSICFGADDRH_LHXS_आवरणम्	0x7
#परिभाषय APLIC_x_MSICFGADDRH_LHXS_सरणम्	20
#परिभाषय APLIC_x_MSICFGADDRH_HHXW_आवरणम्	0x7
#परिभाषय APLIC_x_MSICFGADDRH_HHXW_सरणम्	16
#परिभाषय APLIC_x_MSICFGADDRH_LHXW_आवरणम्	0xf
#परिभाषय APLIC_x_MSICFGADDRH_LHXW_सरणम्	12
#परिभाषय APLIC_x_MSICFGADDRH_BAPPN_आवरणम्	0xfff

#परिभाषय APLIC_x_MSICFGADDR_PPN_सरणम्	12

#परिभाषय APLIC_xMSICFGADDR_PPN_HART(__lhxs) \
	((1UL << (__lhxs)) - 1)

#परिभाषय APLIC_x_MSICFGADDR_PPN_LHX_आवरणम्(__lhxw) \
	((1UL << (__lhxw)) - 1)
#परिभाषय APLIC_x_MSICFGADDR_PPN_LHX_सरणम्(__lhxs) \
	((__lhxs))
#परिभाषय APLIC_xMSICFGADDR_PPN_LHX(__lhxw, __lhxs) \
	(APLIC_x_MSICFGADDR_PPN_LHX_आवरणम्(__lhxw) << \
	 APLIC_x_MSICFGADDR_PPN_LHX_सरणम्(__lhxs))

#परिभाषय APLIC_x_MSICFGADDR_PPN_HHX_आवरणम्(__hhxw) \
	((1UL << (__hhxw)) - 1)
#परिभाषय APLIC_x_MSICFGADDR_PPN_HHX_सरणम्(__hhxs) \
	((__hhxs) + APLIC_x_MSICFGADDR_PPN_सरणम्)
#परिभाषय APLIC_xMSICFGADDR_PPN_HHX(__hhxw, __hhxs) \
	(APLIC_x_MSICFGADDR_PPN_HHX_आवरणम्(__hhxw) << \
	 APLIC_x_MSICFGADDR_PPN_HHX_सरणम्(__hhxs))

#परिभाषय APLIC_TARGET_GUEST_IDX(__gidx) \
	((((u32)(__gidx)) & APLIC_TARGET_GUEST_IDX_आवरणम्) << \
	 APLIC_TARGET_GUEST_IDX_सरणम्)
#परिभाषय APLIC_TARGET_EIID(__eiid) \
	(((u32)(__eiid)) & APLIC_TARGET_EIID_आवरणम्)

#परिभाषय APLIC_SETIP_आधारः		0x1c00
#परिभाषय APLIC_SETIPNUM			0x1cdc

#परिभाषय APLIC_CLRIP_आधारः		0x1d00
#परिभाषय APLIC_CLRIPNUM			0x1ddc

#परिभाषय APLIC_SETIE_आधारः		0x1e00
#परिभाषय APLIC_SETIENUM			0x1edc

#परिभाषय APLIC_CLRIE_आधारः		0x1f00
#परिभाषय APLIC_CLRIENUM			0x1fdc

#परिभाषय APLIC_SETIPNUM_LE		0x2000
#परिभाषय APLIC_SETIPNUM_BE		0x2004

#परिभाषय APLIC_TARGET_आधारः		0x3004
#परिभाषय APLIC_TARGET_HART_IDX_सरणम्	18
#परिभाषय APLIC_TARGET_HART_IDX_आवरणम्	0x3fff
#परिभाषय APLIC_TARGET_GUEST_IDX_सरणम्	12
#परिभाषय APLIC_TARGET_GUEST_IDX_आवरणम्	0x3f
#परिभाषय APLIC_TARGET_IPRIO_आवरणम्	0xff
#परिभाषय APLIC_TARGET_EIID_आवरणम्	0x7ff

#परिभाषय APLIC_TARGET_HART_IDX(__hidx) \
	((((u32)(__hidx)) & APLIC_TARGET_HART_IDX_आवरणम्) << \
	 APLIC_TARGET_HART_IDX_सरणम्)
#परिभाषय APLIC_TARGET_IPRIO(__prio) \
	(((u32)(__prio)) & APLIC_TARGET_IPRIO_आवरणम्)

#परिभाषय APLIC_IDC_आधारः			0x4000
#परिभाषय APLIC_IDC_आकार			32

#परिभाषय APLIC_IDC_IDELIVERY		0x00

#परिभाषय APLIC_IDC_IFORCE		0x04

#परिभाषय APLIC_IDC_ITHRESHOLD		0x08

#परिभाषय APLIC_IDC_TOPI			0x18
#परिभाषय APLIC_IDC_TOPI_ID_सरणम्	16
#परिभाषय APLIC_IDC_TOPI_ID_आवरणम्	0x3ff
#परिभाषय APLIC_IDC_TOPI_PRIO_आवरणम्	0xff

#परिभाषय APLIC_IDC_CLAIMI		0x1c

#परिभाषय APLIC_DEFAULT_प्राथमिकता		1
#परिभाषय APLIC_DISABLE_IDELIVERY		0
#परिभाषय APLIC_ENABLE_IDELIVERY		1
#परिभाषय APLIC_DISABLE_ITHRESHOLD	1
#परिभाषय APLIC_ENABLE_ITHRESHOLD		0

संरचना aplic_msi_दत्त {
	संरचना aplic_दत्त *aplic;
	u32 hwirq;
	u32 parent_hwirq;
};

स्थिर SBI_LIST_शीर्ष(aplic_list);
स्थिर शून्य aplic_writel_msicfg(संरचना aplic_msicfg_दत्त *msicfg,
				शून्य *msicfgaddr, शून्य *msicfgaddrH);

स्थिर अन्तःस्थ bool aplic_is_प्रत्यक्षम्_बहुलकम्(संरचना aplic_दत्त *aplic)
{
	प्रत्यागम aplic && aplic->अङ्कः_idc;
}

स्थिर अन्तःस्थ शून्य aplic_sourcecfg_write(संरचना aplic_दत्त *aplic,
					 u32 hwirq, u32 val)
{
	writel(val, (शून्य *)(aplic->addr + APLIC_SOURCECFG_आधारः +
			     (hwirq - 1) * परिमाण(u32)));
}

स्थिर अन्तःस्थ u32 aplic_sourcecfg_read(संरचना aplic_दत्त *aplic, u32 hwirq)
{
	प्रत्यागम readl((शून्य *)(aplic->addr + APLIC_SOURCECFG_आधारः +
			      (hwirq - 1) * परिमाण(u32)));
}

स्थिर अन्तःस्थ शून्य aplic_target_write(संरचना aplic_दत्त *aplic,
				      u32 hwirq, u32 val)
{
	writel(val, (शून्य *)(aplic->addr + APLIC_TARGET_आधारः +
			     (hwirq - 1) * परिमाण(u32)));
}

स्थिर अन्तःस्थ शून्य aplic_irq_setie(संरचना aplic_दत्त *aplic, u32 hwirq)
{
	writel(hwirq, (शून्य *)(aplic->addr + APLIC_SETIENUM));
}

स्थिर अन्तःस्थ शून्य aplic_irq_clrie(संरचना aplic_दत्त *aplic, u32 hwirq)
{
	writel(hwirq, (शून्य *)(aplic->addr + APLIC_CLRIENUM));
}

स्थिर अन्तःस्थ शून्य aplic_irq_clrip(संरचना aplic_दत्त *aplic, u32 hwirq)
{
	writel(hwirq, (शून्य *)(aplic->addr + APLIC_CLRIPNUM));
}

स्थिर अन्तःस्थ शून्य aplic_msi_irq_retrigger_level(संरचना aplic_दत्त *aplic, u32 hwirq)
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

स्थिर अन्तःस्थ शून्य *aplic_idc_आधारः(संरचना aplic_दत्त *aplic, u32 idc_सूचकांक)
{
	प्रत्यागम (शून्य *)(aplic->addr + APLIC_IDC_आधारः +
			idc_सूचकांक * APLIC_IDC_आकार);
}

स्थिर अन्तःस्थ u32 aplic_idc_read(संरचना aplic_दत्त *aplic, u32 idc_सूचकांक,
				 u32 reg)
{
	प्रत्यागम readl(aplic_idc_आधारः(aplic, idc_सूचकांक) + reg);
}

स्थिर अन्तःस्थ शून्य aplic_idc_write(संरचना aplic_दत्त *aplic, u32 idc_सूचकांक,
				   u32 reg, u32 val)
{
	writel(val, aplic_idc_आधारः(aplic, idc_सूचकांक) + reg);
}

स्थिर u32 aplic_hwirq_flags_गन्तव्यम्_sourcecfg(u32 hwirq_flags)
{
	विकल्प (hwirq_flags & SBI_HWIRQ_FLAGS_LEVEL_SENSE_आवरणम्) {
	पक्ष SBI_HWIRQ_FLAGS_EDGE_RISING:
		प्रत्यागम APLIC_SOURCECFG_SM_EDGE_RISE;
	पक्ष SBI_HWIRQ_FLAGS_EDGE_FALLING:
		प्रत्यागम APLIC_SOURCECFG_SM_EDGE_FALL;
	पक्ष SBI_HWIRQ_FLAGS_LEVEL_उच्चः:
		प्रत्यागम APLIC_SOURCECFG_SM_LEVEL_उच्चः;
	पक्ष SBI_HWIRQ_FLAGS_LEVEL_नीचः:
		प्रत्यागम APLIC_SOURCECFG_SM_LEVEL_नीचः;
	स्वभाव:
		प्रत्यागम APLIC_SOURCECFG_SM_INACTIVE;
	}
}

स्थिर bool aplic_hwirq_is_delegated(संरचना aplic_दत्त *aplic, u32 hwirq)
{
	u32 sourcecfg;

	sourcecfg = aplic_sourcecfg_read(aplic, hwirq);
	प्रत्यागम !!(sourcecfg & APLIC_SOURCECFG_D);
}

स्थिर पूर्णाङ्क aplic_find_idc_सूचकांक(संरचना aplic_दत्त *aplic, u32 hart_सूचकांक)
{
	u32 i;

	प्रति (i = 0; i < aplic->अङ्कः_idc; i++) {
		यदि (aplic->idc_map[i] == hart_सूचकांक)
			प्रत्यागम i;
	}

	प्रत्यागम SBI_ENOENT;
}

स्थिर शून्य aplic_init(संरचना aplic_दत्त *aplic)
{
	संरचना aplic_delegate_दत्त *deleg;
	u32 i, j, tmp, domaincfg;
	पूर्णाङ्क locked;

	/* Set domain configuration to 0 */
	writel(0, (शून्य *)(aplic->addr + APLIC_DOMAINCFG));

	/* Disable all interrupts */
	प्रति (i = 0; i <= aplic->अङ्कः_source; i += 32)
		writel(-1U, (शून्य *)(aplic->addr + APLIC_CLRIE_आधारः +
				     (i / 32) * परिमाण(u32)));

	/* Set interrupt type and priority for all interrupts */
	प्रति (i = 1; i <= aplic->अङ्कः_source; i++) {
		/* Set IRQ source configuration to 0 */
		writel(0, (शून्य *)(aplic->addr + APLIC_SOURCECFG_आधारः +
			  (i - 1) * परिमाण(u32)));
		/* Set IRQ target hart index and priority to 1 */
		writel(APLIC_DEFAULT_प्राथमिकता, (शून्य *)(aplic->addr +
						APLIC_TARGET_आधारः +
						(i - 1) * परिमाण(u32)));
	}

	/* Configure IRQ delegation */
	प्रति (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		यदि (!deleg->first_irq || !deleg->चरमः_irq)
			अनुवर्त;
		यदि (aplic->अङ्कः_source < deleg->first_irq ||
		    aplic->अङ्कः_source < deleg->चरमः_irq)
			अनुवर्त;
		यदि (deleg->child_सूचकांक > APLIC_SOURCECFG_CHILDIDX_आवरणम्)
			अनुवर्त;
		यदि (deleg->first_irq > deleg->चरमः_irq) {
			tmp = deleg->first_irq;
			deleg->first_irq = deleg->चरमः_irq;
			deleg->चरमः_irq = tmp;
		}
		प्रति (j = deleg->first_irq; j <= deleg->चरमः_irq; j++)
			aplic_sourcecfg_write(aplic, j, APLIC_SOURCECFG_D | deleg->child_सूचकांक);
	}

	/* Default initialization of IDC structures */
	यदि (aplic_is_प्रत्यक्षम्_बहुलकम्(aplic)) {
		प्रति (i = 0; i < aplic->अङ्कः_idc; i++) {
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
	यदि (!aplic_is_प्रत्यक्षम्_बहुलकम्(aplic))
		domaincfg |= APLIC_DOMAINCFG_DM;

	writel(domaincfg, (शून्य *)(aplic->addr + APLIC_DOMAINCFG));
}

शून्य aplic_reinit_all(शून्य)
{
	संरचना aplic_दत्त *aplic;

	sbi_list_for_each_प्रविष्टिः(aplic, &aplic_list, ग्रन्थिः)
		aplic_init(aplic);
}

स्थिर शून्य aplic_writel_msicfg(संरचना aplic_msicfg_दत्त *msicfg,
				शून्य *msicfgaddr, शून्य *msicfgaddrH)
{
	u32 val;
	अचिह्नित दीर्घ आधारः_ppn;

	/* Compute the MSI base PPN */
	आधारः_ppn = msicfg->आधारः_addr >> APLIC_x_MSICFGADDR_PPN_सरणम्;
	आधारः_ppn &= ~APLIC_xMSICFGADDR_PPN_HART(msicfg->lhxs);
	आधारः_ppn &= ~APLIC_xMSICFGADDR_PPN_LHX(msicfg->lhxw, msicfg->lhxs);
	आधारः_ppn &= ~APLIC_xMSICFGADDR_PPN_HHX(msicfg->hhxw, msicfg->hhxs);

	/* Write the lower MSI config register */
	writel((u32)आधारः_ppn, msicfgaddr);

	/* Write the upper MSI config register */
	val = (((u64)आधारः_ppn) >> 32) &
		APLIC_x_MSICFGADDRH_BAPPN_आवरणम्;
	val |= (msicfg->lhxw & APLIC_x_MSICFGADDRH_LHXW_आवरणम्)
		<< APLIC_x_MSICFGADDRH_LHXW_सरणम्;
	val |= (msicfg->hhxw & APLIC_x_MSICFGADDRH_HHXW_आवरणम्)
		<< APLIC_x_MSICFGADDRH_HHXW_सरणम्;
	val |= (msicfg->lhxs & APLIC_x_MSICFGADDRH_LHXS_आवरणम्)
		<< APLIC_x_MSICFGADDRH_LHXS_सरणम्;
	val |= (msicfg->hhxs & APLIC_x_MSICFGADDRH_HHXS_आवरणम्)
		<< APLIC_x_MSICFGADDRH_HHXS_सरणम्;
	writel(val, msicfgaddrH);
}

स्थिर पूर्णाङ्क aplic_check_msicfg(संरचना aplic_msicfg_दत्त *msicfg)
{
	यदि (APLIC_x_MSICFGADDRH_LHXS_आवरणम् < msicfg->lhxs)
		प्रत्यागम SBI_EINVAL;

	यदि (APLIC_x_MSICFGADDRH_LHXW_आवरणम् < msicfg->lhxw)
		प्रत्यागम SBI_EINVAL;

	यदि (APLIC_x_MSICFGADDRH_HHXS_आवरणम् < msicfg->hhxs)
		प्रत्यागम SBI_EINVAL;

	यदि (APLIC_x_MSICFGADDRH_HHXW_आवरणम् < msicfg->hhxw)
		प्रत्यागम SBI_EINVAL;

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क aplic_warm_init(संरचना sbi_irqchip_device *chip)
{
	संरचना aplic_दत्त *aplic;
	u32 hart_सूचकांक;
	पूर्णाङ्क idc_सूचकांक;

	aplic = container_सम्बन्धि(chip, संरचना aplic_दत्त, irqchip);
	यदि (!aplic_is_प्रत्यक्षम्_बहुलकम्(aplic))
		प्रत्यागम 0;

	hart_सूचकांक = current_hartindex();
	idc_सूचकांक  = aplic_find_idc_सूचकांक(aplic, hart_सूचकांक);
	यदि (idc_सूचकांक < 0)
		प्रत्यागम 0;

	aplic_idc_write(aplic, idc_सूचकांक, APLIC_IDC_ITHRESHOLD,
			APLIC_ENABLE_ITHRESHOLD);
	aplic_idc_write(aplic, idc_सूचकांक, APLIC_IDC_IDELIVERY,
			APLIC_ENABLE_IDELIVERY);

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क aplic_process_hwirqs(संरचना sbi_irqchip_device *chip)
{
	संरचना aplic_दत्त *aplic;
	u32 hart_सूचकांक, claimi, hwirq;
	पूर्णाङ्क idc_सूचकांक, rc = 0, tmp;

	aplic = container_सम्बन्धि(chip, संरचना aplic_दत्त, irqchip);

	यदि (!aplic_is_प्रत्यक्षम्_बहुलकम्(aplic))
		sbi_panic("aplic_process_hwirqs called in MSI mode.\n");

	hart_सूचकांक = current_hartindex();
	idc_सूचकांक  = aplic_find_idc_सूचकांक(aplic, hart_सूचकांक);
	यदि (idc_सूचकांक < 0)
		प्रत्यागम 0;

	यावत् ((claimi = aplic_idc_read(aplic, idc_सूचकांक, APLIC_IDC_CLAIMI))) {
		hwirq = (claimi >> APLIC_IDC_TOPI_ID_सरणम्) &
			APLIC_IDC_TOPI_ID_आवरणम्;

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
	संरचना aplic_दत्त *aplic;
	u32 sm;

	aplic = container_सम्बन्धि(chip, संरचना aplic_दत्त, irqchip);
	यदि (aplic_is_प्रत्यक्षम्_बहुलकम्(aplic))
		प्रत्यागम;

	sm = aplic_sourcecfg_read(aplic, hwirq) & APLIC_SOURCECFG_SM_आवरणम्;
	यदि (sm == APLIC_SOURCECFG_SM_LEVEL_उच्चः ||
	    sm == APLIC_SOURCECFG_SM_LEVEL_नीचः)
		aplic_msi_irq_retrigger_level(aplic, hwirq);
}

स्थिर शून्य aplic_क्रमादेशः_msi_target(संरचना aplic_दत्त *aplic,
				     u32 hwirq, u32 hart_सूचकांक,
				     u32 guest_सूचकांक, u32 eiid)
{
	u32 target;

	target = APLIC_TARGET_HART_IDX(hart_सूचकांक) |
		 APLIC_TARGET_GUEST_IDX(guest_सूचकांक) |
		 APLIC_TARGET_EIID(eiid);

	aplic_target_write(aplic, hwirq, target);
}

स्थिर u32 derive_hart_सूचकांक(संरचना aplic_msicfg_दत्त *msicfg,
			      ध्रुव संरचना sbi_irqchip_msi_सन्देशः *सन्देशः)
{
	u64 addr = ((u64)सन्देशः->स्थानाङ्कः_hi << 32) | सन्देशः->स्थानाङ्कः_lo;
	u64 tppn = addr >> APLIC_x_MSICFGADDR_PPN_सरणम्;
	u32 गणः_सूचकांक, hart_सूचकांक;

	गणः_सूचकांक = (tppn >> APLIC_x_MSICFGADDR_PPN_HHX_सरणम्(msicfg->hhxs)) &
		      APLIC_x_MSICFGADDR_PPN_HHX_आवरणम्(msicfg->hhxw);

	hart_सूचकांक = (tppn >> msicfg->lhxs) &
		     ((1UL << msicfg->lhxw) - 1);

	hart_सूचकांक |= (गणः_सूचकांक << msicfg->lhxw);

	प्रत्यागम hart_सूचकांक;
}

स्थिर शून्य aplic_write_msi(u32 parent_hwirq,
			    ध्रुव संरचना sbi_irqchip_msi_सन्देशः *सन्देशः,
			    शून्य *priv)
{
	संरचना aplic_msi_दत्त *msi_दत्त = priv;
	u32 guest_सूचकांक = 0;
	u32 eiid;

	eiid = सन्देशः->दत्त;
	यदि (!eiid || eiid > APLIC_TARGET_EIID_आवरणम्)
		प्रत्यागम;

	aplic_क्रमादेशः_msi_target(msi_दत्त->aplic, msi_दत्त->hwirq,
				derive_hart_सूचकांक(&msi_दत्त->aplic->msicfg_mmode, सन्देशः),
				guest_सूचकांक, eiid);
}

स्थिर पूर्णाङ्क aplic_msi_callback(u32 parent_hwirq, शून्य *priv)
{
	संरचना aplic_msi_दत्त *msi_दत्त = priv;
	पूर्णाङ्क rc;

	rc = sbi_irqchip_process_hwirq(&msi_दत्त->aplic->irqchip, msi_दत्त->hwirq);
	यदि (rc && rc != SBI_ENOENT)
		sbi_printf("%s: hwirq=%lu failed rc=%d\n",
			__func__,
			(अचिह्नित दीर्घ)parent_hwirq, rc);

	प्रत्यागम rc;
}

स्थिर पूर्णाङ्क aplic_सज्जता_msi(संरचना aplic_दत्त *aplic, u32 hwirq)
{
	संरचना sbi_irqchip_device *parent;
	u32 first_hwirq;
	संरचना aplic_msi_दत्त *msi_दत्त;
	पूर्णाङ्क rc;

	parent = sbi_irqchip_find_device(aplic->parent_unique_id);
	यदि (!parent) {
		sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
			__func__, (अचिह्नित दीर्घ)hwirq);
		प्रत्यागम SBI_EINVAL;
	}

	msi_दत्त = sbi_zalloc(परिमाण(संरचना aplic_msi_दत्त));
	यदि (!msi_दत्त)
		प्रत्यागम SBI_ENOMEM;

	msi_दत्त->aplic = aplic;
	msi_दत्त->hwirq = hwirq;

	rc = sbi_irqchip_पञ्जिकाकोशः_msi(parent, 1,
					aplic_write_msi,
					aplic_msi_callback,
					msi_दत्त,
					&first_hwirq);
	यदि (rc) {
		sbi_printf("%s: register_msi failed hwirq=%lu rc=%d\n",
			__func__, (अचिह्नित दीर्घ)hwirq, rc);
		sbi_free(msi_दत्त);
		प्रत्यागम rc;
	}

	msi_दत्त->parent_hwirq = first_hwirq;
	sbi_irqchip_समुच्चयः_hwirq_priv(&aplic->irqchip, hwirq, msi_दत्त);

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क aplic_hwirq_सज्जता(संरचना sbi_irqchip_device *chip,
			     u32 hwirq, u32 hwirq_flags)
{
	संरचना aplic_दत्त *aplic;
	u32 sourcecfg;

	aplic = container_सम्बन्धि(chip, संरचना aplic_दत्त, irqchip);

	sourcecfg = aplic_hwirq_flags_गन्तव्यम्_sourcecfg(hwirq_flags);
	यदि (sourcecfg == APLIC_SOURCECFG_SM_INACTIVE) {
		sbi_printf("%s: unsupported flags=0x%lx hwirq=%lu\n",
			__func__,
			(अचिह्नित दीर्घ)hwirq_flags,
			(अचिह्नित दीर्घ)hwirq);
		प्रत्यागम SBI_EINVAL;
	}

	aplic_sourcecfg_write(aplic, hwirq, sourcecfg);

	यदि (aplic_hwirq_is_delegated(aplic, hwirq)) {
		sbi_printf("%s: hwirq=%lu is delegated\n",
			__func__, (अचिह्नित दीर्घ)hwirq);
		प्रत्यागम SBI_ENOTSUPP;
	}

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);

	यदि (!aplic_is_प्रत्यक्षम्_बहुलकम्(aplic))
		प्रत्यागम aplic_सज्जता_msi(aplic, hwirq);

	प्रत्यागम 0;
}

स्थिर शून्य aplic_hwirq_cleanup(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना aplic_दत्त *aplic;
	संरचना aplic_msi_दत्त *msi_दत्त;

	aplic = container_सम्बन्धि(chip, संरचना aplic_दत्त, irqchip);
	यदि (aplic_hwirq_is_delegated(aplic, hwirq))
		प्रत्यागम;

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);
	aplic_sourcecfg_write(aplic, hwirq, APLIC_SOURCECFG_SM_INACTIVE);
	aplic_target_write(aplic, hwirq, APLIC_DEFAULT_प्राथमिकता);
	msi_दत्त = (संरचना aplic_msi_दत्त *) sbi_irqchip_get_hwirq_priv(chip, hwirq);
	यदि (msi_दत्त)
		sbi_free(msi_दत्त);
}

स्थिर पूर्णाङ्क aplic_hwirq_समुच्चयः_affinity(संरचना sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_सूचकांक)
{
	संरचना aplic_दत्त *aplic;
	संरचना aplic_msi_दत्त *msi_दत्त;
	संरचना sbi_irqchip_device *parent;
	पूर्णाङ्क idc_सूचकांक;
	पूर्णाङ्क rc;

	aplic = container_सम्बन्धि(chip, संरचना aplic_दत्त, irqchip);
	यदि (aplic_hwirq_is_delegated(aplic, hwirq))
		प्रत्यागम SBI_ENOTSUPP;

	यदि (!aplic_is_प्रत्यक्षम्_बहुलकम्(aplic)) {
		parent = sbi_irqchip_find_device(aplic->parent_unique_id);
		यदि (!parent) {
			sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
				__func__, (अचिह्नित दीर्घ)hwirq);
			प्रत्यागम SBI_EINVAL;
		}

		msi_दत्त = sbi_irqchip_get_hwirq_priv(chip, hwirq);
		rc = sbi_irqchip_समुच्चयः_affinity(parent, msi_दत्त->parent_hwirq, hart_सूचकांक);
		यदि (rc) {
			sbi_printf("%s: sbi_irqchip_set_affinity failed hwirq=%lu rc=%d\n",
				__func__, (अचिह्नित दीर्घ)hwirq, rc);
			प्रत्यागम rc;
		}
		प्रत्यागम 0;
	}

	idc_सूचकांक = aplic_find_idc_सूचकांक(aplic, hart_सूचकांक);
	यदि (idc_सूचकांक < 0)
		प्रत्यागम SBI_EINVAL;

	aplic_target_write(aplic, hwirq,
			   APLIC_TARGET_HART_IDX(hart_सूचकांक) |
			   APLIC_TARGET_IPRIO(APLIC_DEFAULT_प्राथमिकता));

	प्रत्यागम 0;
}

स्थिर शून्य aplic_hwirq_आवरणम्(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना aplic_दत्त *aplic;

	aplic = container_सम्बन्धि(chip, संरचना aplic_दत्त, irqchip);
	यदि (aplic_hwirq_is_delegated(aplic, hwirq))
		प्रत्यागम;

	aplic_irq_clrie(aplic, hwirq);
}

स्थिर शून्य aplic_hwirq_unmask(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना aplic_दत्त *aplic;

	aplic = container_सम्बन्धि(chip, संरचना aplic_दत्त, irqchip);
	यदि (aplic_hwirq_is_delegated(aplic, hwirq))
		प्रत्यागम;

	aplic_irq_setie(aplic, hwirq);
}

स्थिर संरचना sbi_irqchip_device aplic_irqchip_template = {
	.warm_init			= aplic_warm_init,
	.process_hwirqs		= aplic_process_hwirqs,
	.hwirq_eoi			= aplic_hwirq_eoi,
	.hwirq_सज्जता		= aplic_hwirq_सज्जता,
	.hwirq_cleanup		= aplic_hwirq_cleanup,
	.hwirq_समुच्चयः_affinity	= aplic_hwirq_समुच्चयः_affinity,
	.hwirq_आवरणम्			= aplic_hwirq_आवरणम्,
	.hwirq_unmask		= aplic_hwirq_unmask,
};

पूर्णाङ्क aplic_cold_irqchip_init(संरचना aplic_दत्त *aplic)
{
	पूर्णाङ्क rc;
	संरचना aplic_delegate_दत्त *deleg;
	u32 first_deleg_irq, चरमः_deleg_irq, i;
	bool msi_बहुलकम्;

	msi_बहुलकम् = !aplic_is_प्रत्यक्षम्_बहुलकम्(aplic);
	यदि (!aplic->अङ्कः_source) {
		sbi_printf("%s: num_source is zero\n", __func__);
		प्रत्यागम SBI_EINVAL;
	}

	यदि (APLIC_MAX_SOURCE <= aplic->अङ्कः_source) {
		sbi_printf("%s: num_source=%lu exceeds max=%lu\n",
			__func__,
			(अचिह्नित दीर्घ)aplic->अङ्कः_source,
			(अचिह्नित दीर्घ)APLIC_MAX_SOURCE);
		प्रत्यागम SBI_EINVAL;
	}

	यदि (!msi_बहुलकम् && APLIC_MAX_IDC <= aplic->अङ्कः_idc) {
		sbi_printf("%s: num_idc=%lu exceeds max=%lu\n",
			__func__,
			(अचिह्नित दीर्घ)aplic->अङ्कः_idc,
			(अचिह्नित दीर्घ)APLIC_MAX_IDC);
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
	चरमः_deleg_irq = 0;
	प्रति (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		यदि (deleg->first_irq < first_deleg_irq)
			first_deleg_irq = deleg->first_irq;
		यदि (चरमः_deleg_irq < deleg->चरमः_irq)
			चरमः_deleg_irq = deleg->चरमः_irq;
	}

	यदि (aplic->targets_mmode ||
	    ((first_deleg_irq < चरमः_deleg_irq) &&
	    (चरमः_deleg_irq == aplic->अङ्कः_source) &&
	    (first_deleg_irq == 1))) {
		rc = sbi_domain_मूलम्_सङ्कलय_memrange(aplic->addr, aplic->आकार, PAGE_आकार,
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
		aplic->irqchip.अङ्कः_hwirq = aplic->अङ्कः_source + 1;

		यदि (msi_बहुलकम्) {
			aplic->irqchip.warm_init      = रिक्त;
			aplic->irqchip.process_hwirqs = रिक्त;
			aplic->irqchip.hwirq_eoi      = रिक्त;
		}

		यदि (msi_बहुलकम्)
			sbi_hartmask_समुच्चयः_all(&aplic->irqchip.target_harts);
		अन्यथा
			प्रति (i = 0; i < aplic->अङ्कः_idc; i++)
				sbi_hartmask_समुच्चयः_hartindex(aplic->idc_map[i],
							   &aplic->irqchip.target_harts);

		rc = sbi_irqchip_सङ्कलय_device(&aplic->irqchip);
		यदि (rc) {
			sbi_printf("%s: sbi_irqchip_add_device failed rc=%d id=%lu mode=%s target_weight=%lu\n",
				__func__, rc,
				(अचिह्नित दीर्घ)aplic->irqchip.id,
				msi_बहुलकम् ? "msi" : "direct",
				(अचिह्नित दीर्घ)sbi_hartmask_weight(
					&aplic->irqchip.target_harts));
			प्रत्यागम rc;
		}
	}
	/* Attach to the aplic list */
	sbi_list_सङ्कलय_पुच्छ(&aplic->ग्रन्थिः, &aplic_list);

	प्रत्यागम 0;
}
