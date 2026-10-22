/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi_utils/irqchip/aplic.h>

#घोषणा APLIC_MAX_IDC			(1UL << 14)
#घोषणा APLIC_MAX_SOURCE		1024

#घोषणा APLIC_DOMAINCFG		0x0000
#घोषणा APLIC_DOMAINCFG_IE		(1 << 8)
#घोषणा APLIC_DOMAINCFG_DM		(1 << 2)
#घोषणा APLIC_DOMAINCFG_BE		(1 << 0)

#घोषणा APLIC_SOURCECFG_BASE		0x0004
#घोषणा APLIC_SOURCECFG_D		(1 << 10)
#घोषणा APLIC_SOURCECFG_CHILDIDX_MASK	0x000003ff
#घोषणा APLIC_SOURCECFG_SM_MASK	0x00000007
#घोषणा APLIC_SOURCECFG_SM_INACTIVE	0x0
#घोषणा APLIC_SOURCECFG_SM_DETACH	0x1
#घोषणा APLIC_SOURCECFG_SM_EDGE_RISE	0x4
#घोषणा APLIC_SOURCECFG_SM_EDGE_FALL	0x5
#घोषणा APLIC_SOURCECFG_SM_LEVEL_ऊपरी	0x6
#घोषणा APLIC_SOURCECFG_SM_LEVEL_निचला	0x7

#घोषणा APLIC_MMSICFGADDR		0x1bc0
#घोषणा APLIC_MMSICFGADDRH		0x1bc4
#घोषणा APLIC_SMSICFGADDR		0x1bc8
#घोषणा APLIC_SMSICFGADDRH		0x1bcc

#घोषणा APLIC_xMSICFGADDRH_L		(1UL << 31)
#घोषणा APLIC_xMSICFGADDRH_HHXS_MASK	0x1f
#घोषणा APLIC_xMSICFGADDRH_HHXS_SHIFT	24
#घोषणा APLIC_xMSICFGADDRH_LHXS_MASK	0x7
#घोषणा APLIC_xMSICFGADDRH_LHXS_SHIFT	20
#घोषणा APLIC_xMSICFGADDRH_HHXW_MASK	0x7
#घोषणा APLIC_xMSICFGADDRH_HHXW_SHIFT	16
#घोषणा APLIC_xMSICFGADDRH_LHXW_MASK	0xf
#घोषणा APLIC_xMSICFGADDRH_LHXW_SHIFT	12
#घोषणा APLIC_xMSICFGADDRH_BAPPN_MASK	0xfff

#घोषणा APLIC_xMSICFGADDR_PPN_SHIFT	12

#घोषणा APLIC_xMSICFGADDR_PPN_HART(__lhxs) \
	((1UL << (__lhxs)) - 1)

#घोषणा APLIC_xMSICFGADDR_PPN_LHX_MASK(__lhxw) \
	((1UL << (__lhxw)) - 1)
#घोषणा APLIC_xMSICFGADDR_PPN_LHX_SHIFT(__lhxs) \
	((__lhxs))
#घोषणा APLIC_xMSICFGADDR_PPN_LHX(__lhxw, __lhxs) \
	(APLIC_xMSICFGADDR_PPN_LHX_MASK(__lhxw) << \
	 APLIC_xMSICFGADDR_PPN_LHX_SHIFT(__lhxs))

#घोषणा APLIC_xMSICFGADDR_PPN_HHX_MASK(__hhxw) \
	((1UL << (__hhxw)) - 1)
#घोषणा APLIC_xMSICFGADDR_PPN_HHX_SHIFT(__hhxs) \
	((__hhxs) + APLIC_xMSICFGADDR_PPN_SHIFT)
#घोषणा APLIC_xMSICFGADDR_PPN_HHX(__hhxw, __hhxs) \
	(APLIC_xMSICFGADDR_PPN_HHX_MASK(__hhxw) << \
	 APLIC_xMSICFGADDR_PPN_HHX_SHIFT(__hhxs))

#घोषणा APLIC_TARGET_GUEST_IDX(__gidx) \
	((((u32)(__gidx)) & APLIC_TARGET_GUEST_IDX_MASK) << \
	 APLIC_TARGET_GUEST_IDX_SHIFT)
#घोषणा APLIC_TARGET_EIID(__eiid) \
	(((u32)(__eiid)) & APLIC_TARGET_EIID_MASK)

#घोषणा APLIC_SETIP_BASE		0x1c00
#घोषणा APLIC_SETIPNUM			0x1cdc

#घोषणा APLIC_CLRIP_BASE		0x1d00
#घोषणा APLIC_CLRIPNUM			0x1ddc

#घोषणा APLIC_SETIE_BASE		0x1e00
#घोषणा APLIC_SETIENUM			0x1edc

#घोषणा APLIC_CLRIE_BASE		0x1f00
#घोषणा APLIC_CLRIENUM			0x1fdc

#घोषणा APLIC_SETIPNUM_LE		0x2000
#घोषणा APLIC_SETIPNUM_BE		0x2004

#घोषणा APLIC_TARGET_BASE		0x3004
#घोषणा APLIC_TARGET_HART_IDX_SHIFT	18
#घोषणा APLIC_TARGET_HART_IDX_MASK	0x3fff
#घोषणा APLIC_TARGET_GUEST_IDX_SHIFT	12
#घोषणा APLIC_TARGET_GUEST_IDX_MASK	0x3f
#घोषणा APLIC_TARGET_IPRIO_MASK	0xff
#घोषणा APLIC_TARGET_EIID_MASK	0x7ff

#घोषणा APLIC_TARGET_HART_IDX(__hidx) \
	((((u32)(__hidx)) & APLIC_TARGET_HART_IDX_MASK) << \
	 APLIC_TARGET_HART_IDX_SHIFT)
#घोषणा APLIC_TARGET_IPRIO(__prio) \
	(((u32)(__prio)) & APLIC_TARGET_IPRIO_MASK)

#घोषणा APLIC_IDC_BASE			0x4000
#घोषणा APLIC_IDC_आकार			32

#घोषणा APLIC_IDC_IDELIVERY		0x00

#घोषणा APLIC_IDC_IFORCE		0x04

#घोषणा APLIC_IDC_ITHRESHOLD		0x08

#घोषणा APLIC_IDC_TOPI			0x18
#घोषणा APLIC_IDC_TOPI_ID_SHIFT	16
#घोषणा APLIC_IDC_TOPI_ID_MASK	0x3ff
#घोषणा APLIC_IDC_TOPI_PRIO_MASK	0xff

#घोषणा APLIC_IDC_CLAIMI		0x1c

#घोषणा APLIC_DEFAULT_PRIORITY		1
#घोषणा APLIC_DISABLE_IDELIVERY		0
#घोषणा APLIC_ENABLE_IDELIVERY		1
#घोषणा APLIC_DISABLE_ITHRESHOLD	1
#घोषणा APLIC_ENABLE_ITHRESHOLD		0

काष्ठा aplic_msi_तथ्य {
	काष्ठा aplic_तथ्य *aplic;
	u32 hwirq;
	u32 parent_hwirq;
};

अटल SBI_LIST_शीर्ष(aplic_list);
अटल व्योम aplic_writel_msicfg(काष्ठा aplic_msicfg_तथ्य *msicfg,
				व्योम *msicfgaddr, व्योम *msicfgaddrH);

अटल अंतरभूत bool aplic_is_direct_mode(काष्ठा aplic_तथ्य *aplic)
{
	वापस aplic && aplic->अंक_idc;
}

अटल अंतरभूत व्योम aplic_sourcecfg_write(काष्ठा aplic_तथ्य *aplic,
					 u32 hwirq, u32 val)
{
	writel(val, (व्योम *)(aplic->addr + APLIC_SOURCECFG_BASE +
			     (hwirq - 1) * माप(u32)));
}

अटल अंतरभूत u32 aplic_sourcecfg_read(काष्ठा aplic_तथ्य *aplic, u32 hwirq)
{
	वापस readl((व्योम *)(aplic->addr + APLIC_SOURCECFG_BASE +
			      (hwirq - 1) * माप(u32)));
}

अटल अंतरभूत व्योम aplic_target_write(काष्ठा aplic_तथ्य *aplic,
				      u32 hwirq, u32 val)
{
	writel(val, (व्योम *)(aplic->addr + APLIC_TARGET_BASE +
			     (hwirq - 1) * माप(u32)));
}

अटल अंतरभूत व्योम aplic_irq_setie(काष्ठा aplic_तथ्य *aplic, u32 hwirq)
{
	writel(hwirq, (व्योम *)(aplic->addr + APLIC_SETIENUM));
}

अटल अंतरभूत व्योम aplic_irq_clrie(काष्ठा aplic_तथ्य *aplic, u32 hwirq)
{
	writel(hwirq, (व्योम *)(aplic->addr + APLIC_CLRIENUM));
}

अटल अंतरभूत व्योम aplic_irq_clrip(काष्ठा aplic_तथ्य *aplic, u32 hwirq)
{
	writel(hwirq, (व्योम *)(aplic->addr + APLIC_CLRIPNUM));
}

अटल अंतरभूत व्योम aplic_msi_irq_retrigger_level(काष्ठा aplic_तथ्य *aplic, u32 hwirq)
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
	writel(hwirq, (व्योम *)(aplic->addr + APLIC_SETIPNUM_LE));
}

अटल अंतरभूत व्योम *aplic_idc_base(काष्ठा aplic_तथ्य *aplic, u32 idc_सूचकांक)
{
	वापस (व्योम *)(aplic->addr + APLIC_IDC_BASE +
			idc_सूचकांक * APLIC_IDC_आकार);
}

अटल अंतरभूत u32 aplic_idc_read(काष्ठा aplic_तथ्य *aplic, u32 idc_सूचकांक,
				 u32 reg)
{
	वापस readl(aplic_idc_base(aplic, idc_सूचकांक) + reg);
}

अटल अंतरभूत व्योम aplic_idc_write(काष्ठा aplic_तथ्य *aplic, u32 idc_सूचकांक,
				   u32 reg, u32 val)
{
	writel(val, aplic_idc_base(aplic, idc_सूचकांक) + reg);
}

अटल u32 aplic_hwirq_flags_to_sourcecfg(u32 hwirq_flags)
{
	चयन (hwirq_flags & SBI_HWIRQ_FLAGS_LEVEL_SENSE_MASK) {
	हाल SBI_HWIRQ_FLAGS_EDGE_RISING:
		वापस APLIC_SOURCECFG_SM_EDGE_RISE;
	हाल SBI_HWIRQ_FLAGS_EDGE_FALLING:
		वापस APLIC_SOURCECFG_SM_EDGE_FALL;
	हाल SBI_HWIRQ_FLAGS_LEVEL_ऊपरी:
		वापस APLIC_SOURCECFG_SM_LEVEL_ऊपरी;
	हाल SBI_HWIRQ_FLAGS_LEVEL_निचला:
		वापस APLIC_SOURCECFG_SM_LEVEL_निचला;
	शेष:
		वापस APLIC_SOURCECFG_SM_INACTIVE;
	}
}

अटल bool aplic_hwirq_is_delegated(काष्ठा aplic_तथ्य *aplic, u32 hwirq)
{
	u32 sourcecfg;

	sourcecfg = aplic_sourcecfg_read(aplic, hwirq);
	वापस !!(sourcecfg & APLIC_SOURCECFG_D);
}

अटल पूर्णांक aplic_find_idc_सूचकांक(काष्ठा aplic_तथ्य *aplic, u32 hart_सूचकांक)
{
	u32 i;

	क्रम (i = 0; i < aplic->अंक_idc; i++) {
		अगर (aplic->idc_map[i] == hart_सूचकांक)
			वापस i;
	}

	वापस SBI_ENOENT;
}

अटल व्योम aplic_init(काष्ठा aplic_तथ्य *aplic)
{
	काष्ठा aplic_delegate_तथ्य *deleg;
	u32 i, j, tmp, domaincfg;
	पूर्णांक locked;

	/* Set domain configuration to 0 */
	writel(0, (व्योम *)(aplic->addr + APLIC_DOMAINCFG));

	/* Disable all interrupts */
	क्रम (i = 0; i <= aplic->अंक_source; i += 32)
		writel(-1U, (व्योम *)(aplic->addr + APLIC_CLRIE_BASE +
				     (i / 32) * माप(u32)));

	/* Set interrupt type and priority for all interrupts */
	क्रम (i = 1; i <= aplic->अंक_source; i++) {
		/* Set IRQ source configuration to 0 */
		writel(0, (व्योम *)(aplic->addr + APLIC_SOURCECFG_BASE +
			  (i - 1) * माप(u32)));
		/* Set IRQ target hart index and priority to 1 */
		writel(APLIC_DEFAULT_PRIORITY, (व्योम *)(aplic->addr +
						APLIC_TARGET_BASE +
						(i - 1) * माप(u32)));
	}

	/* Configure IRQ delegation */
	क्रम (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		अगर (!deleg->first_irq || !deleg->last_irq)
			जारी;
		अगर (aplic->अंक_source < deleg->first_irq ||
		    aplic->अंक_source < deleg->last_irq)
			जारी;
		अगर (deleg->child_सूचकांक > APLIC_SOURCECFG_CHILDIDX_MASK)
			जारी;
		अगर (deleg->first_irq > deleg->last_irq) {
			tmp = deleg->first_irq;
			deleg->first_irq = deleg->last_irq;
			deleg->last_irq = tmp;
		}
		क्रम (j = deleg->first_irq; j <= deleg->last_irq; j++)
			aplic_sourcecfg_write(aplic, j, APLIC_SOURCECFG_D | deleg->child_सूचकांक);
	}

	/* Default initialization of IDC structures */
	अगर (aplic_is_direct_mode(aplic)) {
		क्रम (i = 0; i < aplic->अंक_idc; i++) {
			aplic_idc_write(aplic, i, APLIC_IDC_IDELIVERY,
					APLIC_DISABLE_IDELIVERY);
			aplic_idc_write(aplic, i, APLIC_IDC_IFORCE, 0);
			aplic_idc_write(aplic, i, APLIC_IDC_ITHRESHOLD,
					APLIC_DISABLE_ITHRESHOLD);
		}
	} अन्यथा {
		/* MSI configuration */
		locked = readl((व्योम *)(aplic->addr + APLIC_MMSICFGADDRH)) & APLIC_xMSICFGADDRH_L;
		अगर (aplic->targets_mmode && aplic->has_msicfg_mmode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_mmode,
						(व्योम *)(aplic->addr + APLIC_MMSICFGADDR),
						(व्योम *)(aplic->addr + APLIC_MMSICFGADDRH));
		}
		अगर (aplic->targets_mmode && aplic->has_msicfg_smode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_smode,
						(व्योम *)(aplic->addr + APLIC_SMSICFGADDR),
						(व्योम *)(aplic->addr + APLIC_SMSICFGADDRH));
		}
	}

	domaincfg = APLIC_DOMAINCFG_IE;
	अगर (!aplic_is_direct_mode(aplic))
		domaincfg |= APLIC_DOMAINCFG_DM;

	writel(domaincfg, (व्योम *)(aplic->addr + APLIC_DOMAINCFG));
}

व्योम aplic_reinit_all(व्योम)
{
	काष्ठा aplic_तथ्य *aplic;

	sbi_list_for_each_entry(aplic, &aplic_list, गाँठ)
		aplic_init(aplic);
}

अटल व्योम aplic_writel_msicfg(काष्ठा aplic_msicfg_तथ्य *msicfg,
				व्योम *msicfgaddr, व्योम *msicfgaddrH)
{
	u32 val;
	अचिन्हित दीर्घ base_ppn;

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

अटल पूर्णांक aplic_check_msicfg(काष्ठा aplic_msicfg_तथ्य *msicfg)
{
	अगर (APLIC_xMSICFGADDRH_LHXS_MASK < msicfg->lhxs)
		वापस SBI_EINVAL;

	अगर (APLIC_xMSICFGADDRH_LHXW_MASK < msicfg->lhxw)
		वापस SBI_EINVAL;

	अगर (APLIC_xMSICFGADDRH_HHXS_MASK < msicfg->hhxs)
		वापस SBI_EINVAL;

	अगर (APLIC_xMSICFGADDRH_HHXW_MASK < msicfg->hhxw)
		वापस SBI_EINVAL;

	वापस 0;
}

अटल पूर्णांक aplic_warm_init(काष्ठा sbi_irqchip_device *chip)
{
	काष्ठा aplic_तथ्य *aplic;
	u32 hart_सूचकांक;
	पूर्णांक idc_सूचकांक;

	aplic = container_of(chip, काष्ठा aplic_तथ्य, irqchip);
	अगर (!aplic_is_direct_mode(aplic))
		वापस 0;

	hart_सूचकांक = current_hartindex();
	idc_सूचकांक  = aplic_find_idc_सूचकांक(aplic, hart_सूचकांक);
	अगर (idc_सूचकांक < 0)
		वापस 0;

	aplic_idc_write(aplic, idc_सूचकांक, APLIC_IDC_ITHRESHOLD,
			APLIC_ENABLE_ITHRESHOLD);
	aplic_idc_write(aplic, idc_सूचकांक, APLIC_IDC_IDELIVERY,
			APLIC_ENABLE_IDELIVERY);

	वापस 0;
}

अटल पूर्णांक aplic_process_hwirqs(काष्ठा sbi_irqchip_device *chip)
{
	काष्ठा aplic_तथ्य *aplic;
	u32 hart_सूचकांक, claimi, hwirq;
	पूर्णांक idc_सूचकांक, rc = 0, tmp;

	aplic = container_of(chip, काष्ठा aplic_तथ्य, irqchip);

	अगर (!aplic_is_direct_mode(aplic))
		sbi_panic("aplic_process_hwirqs called in MSI mode.\n");

	hart_सूचकांक = current_hartindex();
	idc_सूचकांक  = aplic_find_idc_सूचकांक(aplic, hart_सूचकांक);
	अगर (idc_सूचकांक < 0)
		वापस 0;

	जबतक ((claimi = aplic_idc_read(aplic, idc_सूचकांक, APLIC_IDC_CLAIMI))) {
		hwirq = (claimi >> APLIC_IDC_TOPI_ID_SHIFT) &
			APLIC_IDC_TOPI_ID_MASK;

		अगर (!hwirq)
			अवरोध;

		अगर (aplic_hwirq_is_delegated(aplic, hwirq))
			जारी;

		tmp = sbi_irqchip_process_hwirq(chip, hwirq);
		अगर (tmp)
			rc = tmp;
	}

	वापस rc;
}

अटल व्योम aplic_hwirq_eoi(काष्ठा sbi_irqchip_device *chip, u32 hwirq)
{
	काष्ठा aplic_तथ्य *aplic;
	u32 sm;

	aplic = container_of(chip, काष्ठा aplic_तथ्य, irqchip);
	अगर (aplic_is_direct_mode(aplic))
		वापस;

	sm = aplic_sourcecfg_read(aplic, hwirq) & APLIC_SOURCECFG_SM_MASK;
	अगर (sm == APLIC_SOURCECFG_SM_LEVEL_ऊपरी ||
	    sm == APLIC_SOURCECFG_SM_LEVEL_निचला)
		aplic_msi_irq_retrigger_level(aplic, hwirq);
}

अटल व्योम aplic_program_msi_target(काष्ठा aplic_तथ्य *aplic,
				     u32 hwirq, u32 hart_सूचकांक,
				     u32 guest_सूचकांक, u32 eiid)
{
	u32 target;

	target = APLIC_TARGET_HART_IDX(hart_सूचकांक) |
		 APLIC_TARGET_GUEST_IDX(guest_सूचकांक) |
		 APLIC_TARGET_EIID(eiid);

	aplic_target_write(aplic, hwirq, target);
}

अटल u32 derive_hart_सूचकांक(काष्ठा aplic_msicfg_तथ्य *msicfg,
			      स्थिर काष्ठा sbi_irqchip_msi_संदेश *संदेश)
{
	u64 addr = ((u64)संदेश->address_hi << 32) | संदेश->address_lo;
	u64 tppn = addr >> APLIC_xMSICFGADDR_PPN_SHIFT;
	u32 group_सूचकांक, hart_सूचकांक;

	group_सूचकांक = (tppn >> APLIC_xMSICFGADDR_PPN_HHX_SHIFT(msicfg->hhxs)) &
		      APLIC_xMSICFGADDR_PPN_HHX_MASK(msicfg->hhxw);

	hart_सूचकांक = (tppn >> msicfg->lhxs) &
		     ((1UL << msicfg->lhxw) - 1);

	hart_सूचकांक |= (group_सूचकांक << msicfg->lhxw);

	वापस hart_सूचकांक;
}

अटल व्योम aplic_write_msi(u32 parent_hwirq,
			    स्थिर काष्ठा sbi_irqchip_msi_संदेश *संदेश,
			    व्योम *priv)
{
	काष्ठा aplic_msi_तथ्य *msi_तथ्य = priv;
	u32 guest_सूचकांक = 0;
	u32 eiid;

	eiid = संदेश->तथ्य;
	अगर (!eiid || eiid > APLIC_TARGET_EIID_MASK)
		वापस;

	aplic_program_msi_target(msi_तथ्य->aplic, msi_तथ्य->hwirq,
				derive_hart_सूचकांक(&msi_तथ्य->aplic->msicfg_mmode, संदेश),
				guest_सूचकांक, eiid);
}

अटल पूर्णांक aplic_msi_callback(u32 parent_hwirq, व्योम *priv)
{
	काष्ठा aplic_msi_तथ्य *msi_तथ्य = priv;
	पूर्णांक rc;

	rc = sbi_irqchip_process_hwirq(&msi_तथ्य->aplic->irqchip, msi_तथ्य->hwirq);
	अगर (rc && rc != SBI_ENOENT)
		sbi_printf("%s: hwirq=%lu failed rc=%d\n",
			__func__,
			(अचिन्हित दीर्घ)parent_hwirq, rc);

	वापस rc;
}

अटल पूर्णांक aplic_setup_msi(काष्ठा aplic_तथ्य *aplic, u32 hwirq)
{
	काष्ठा sbi_irqchip_device *parent;
	u32 first_hwirq;
	काष्ठा aplic_msi_तथ्य *msi_तथ्य;
	पूर्णांक rc;

	parent = sbi_irqchip_find_device(aplic->parent_unique_id);
	अगर (!parent) {
		sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
			__func__, (अचिन्हित दीर्घ)hwirq);
		वापस SBI_EINVAL;
	}

	msi_तथ्य = sbi_zalloc(माप(काष्ठा aplic_msi_तथ्य));
	अगर (!msi_तथ्य)
		वापस SBI_ENOMEM;

	msi_तथ्य->aplic = aplic;
	msi_तथ्य->hwirq = hwirq;

	rc = sbi_irqchip_register_msi(parent, 1,
					aplic_write_msi,
					aplic_msi_callback,
					msi_तथ्य,
					&first_hwirq);
	अगर (rc) {
		sbi_printf("%s: register_msi failed hwirq=%lu rc=%d\n",
			__func__, (अचिन्हित दीर्घ)hwirq, rc);
		sbi_free(msi_तथ्य);
		वापस rc;
	}

	msi_तथ्य->parent_hwirq = first_hwirq;
	sbi_irqchip_set_hwirq_priv(&aplic->irqchip, hwirq, msi_तथ्य);

	वापस 0;
}

अटल पूर्णांक aplic_hwirq_setup(काष्ठा sbi_irqchip_device *chip,
			     u32 hwirq, u32 hwirq_flags)
{
	काष्ठा aplic_तथ्य *aplic;
	u32 sourcecfg;

	aplic = container_of(chip, काष्ठा aplic_तथ्य, irqchip);

	sourcecfg = aplic_hwirq_flags_to_sourcecfg(hwirq_flags);
	अगर (sourcecfg == APLIC_SOURCECFG_SM_INACTIVE) {
		sbi_printf("%s: unsupported flags=0x%lx hwirq=%lu\n",
			__func__,
			(अचिन्हित दीर्घ)hwirq_flags,
			(अचिन्हित दीर्घ)hwirq);
		वापस SBI_EINVAL;
	}

	aplic_sourcecfg_write(aplic, hwirq, sourcecfg);

	अगर (aplic_hwirq_is_delegated(aplic, hwirq)) {
		sbi_printf("%s: hwirq=%lu is delegated\n",
			__func__, (अचिन्हित दीर्घ)hwirq);
		वापस SBI_ENOTSUPP;
	}

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);

	अगर (!aplic_is_direct_mode(aplic))
		वापस aplic_setup_msi(aplic, hwirq);

	वापस 0;
}

अटल व्योम aplic_hwirq_cleanup(काष्ठा sbi_irqchip_device *chip, u32 hwirq)
{
	काष्ठा aplic_तथ्य *aplic;
	काष्ठा aplic_msi_तथ्य *msi_तथ्य;

	aplic = container_of(chip, काष्ठा aplic_तथ्य, irqchip);
	अगर (aplic_hwirq_is_delegated(aplic, hwirq))
		वापस;

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);
	aplic_sourcecfg_write(aplic, hwirq, APLIC_SOURCECFG_SM_INACTIVE);
	aplic_target_write(aplic, hwirq, APLIC_DEFAULT_PRIORITY);
	msi_तथ्य = (काष्ठा aplic_msi_तथ्य *) sbi_irqchip_get_hwirq_priv(chip, hwirq);
	अगर (msi_तथ्य)
		sbi_free(msi_तथ्य);
}

अटल पूर्णांक aplic_hwirq_set_affinity(काष्ठा sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_सूचकांक)
{
	काष्ठा aplic_तथ्य *aplic;
	काष्ठा aplic_msi_तथ्य *msi_तथ्य;
	काष्ठा sbi_irqchip_device *parent;
	पूर्णांक idc_सूचकांक;
	पूर्णांक rc;

	aplic = container_of(chip, काष्ठा aplic_तथ्य, irqchip);
	अगर (aplic_hwirq_is_delegated(aplic, hwirq))
		वापस SBI_ENOTSUPP;

	अगर (!aplic_is_direct_mode(aplic)) {
		parent = sbi_irqchip_find_device(aplic->parent_unique_id);
		अगर (!parent) {
			sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
				__func__, (अचिन्हित दीर्घ)hwirq);
			वापस SBI_EINVAL;
		}

		msi_तथ्य = sbi_irqchip_get_hwirq_priv(chip, hwirq);
		rc = sbi_irqchip_set_affinity(parent, msi_तथ्य->parent_hwirq, hart_सूचकांक);
		अगर (rc) {
			sbi_printf("%s: sbi_irqchip_set_affinity failed hwirq=%lu rc=%d\n",
				__func__, (अचिन्हित दीर्घ)hwirq, rc);
			वापस rc;
		}
		वापस 0;
	}

	idc_सूचकांक = aplic_find_idc_सूचकांक(aplic, hart_सूचकांक);
	अगर (idc_सूचकांक < 0)
		वापस SBI_EINVAL;

	aplic_target_write(aplic, hwirq,
			   APLIC_TARGET_HART_IDX(hart_सूचकांक) |
			   APLIC_TARGET_IPRIO(APLIC_DEFAULT_PRIORITY));

	वापस 0;
}

अटल व्योम aplic_hwirq_mask(काष्ठा sbi_irqchip_device *chip, u32 hwirq)
{
	काष्ठा aplic_तथ्य *aplic;

	aplic = container_of(chip, काष्ठा aplic_तथ्य, irqchip);
	अगर (aplic_hwirq_is_delegated(aplic, hwirq))
		वापस;

	aplic_irq_clrie(aplic, hwirq);
}

अटल व्योम aplic_hwirq_unmask(काष्ठा sbi_irqchip_device *chip, u32 hwirq)
{
	काष्ठा aplic_तथ्य *aplic;

	aplic = container_of(chip, काष्ठा aplic_तथ्य, irqchip);
	अगर (aplic_hwirq_is_delegated(aplic, hwirq))
		वापस;

	aplic_irq_setie(aplic, hwirq);
}

अटल काष्ठा sbi_irqchip_device aplic_irqchip_template = {
	.warm_init			= aplic_warm_init,
	.process_hwirqs		= aplic_process_hwirqs,
	.hwirq_eoi			= aplic_hwirq_eoi,
	.hwirq_setup		= aplic_hwirq_setup,
	.hwirq_cleanup		= aplic_hwirq_cleanup,
	.hwirq_set_affinity	= aplic_hwirq_set_affinity,
	.hwirq_mask			= aplic_hwirq_mask,
	.hwirq_unmask		= aplic_hwirq_unmask,
};

पूर्णांक aplic_cold_irqchip_init(काष्ठा aplic_तथ्य *aplic)
{
	पूर्णांक rc;
	काष्ठा aplic_delegate_तथ्य *deleg;
	u32 first_deleg_irq, last_deleg_irq, i;
	bool msi_mode;

	msi_mode = !aplic_is_direct_mode(aplic);
	अगर (!aplic->अंक_source) {
		sbi_printf("%s: num_source is zero\n", __func__);
		वापस SBI_EINVAL;
	}

	अगर (APLIC_MAX_SOURCE <= aplic->अंक_source) {
		sbi_printf("%s: num_source=%lu exceeds max=%lu\n",
			__func__,
			(अचिन्हित दीर्घ)aplic->अंक_source,
			(अचिन्हित दीर्घ)APLIC_MAX_SOURCE);
		वापस SBI_EINVAL;
	}

	अगर (!msi_mode && APLIC_MAX_IDC <= aplic->अंक_idc) {
		sbi_printf("%s: num_idc=%lu exceeds max=%lu\n",
			__func__,
			(अचिन्हित दीर्घ)aplic->अंक_idc,
			(अचिन्हित दीर्घ)APLIC_MAX_IDC);
		वापस SBI_EINVAL;
	}

	अगर (aplic->targets_mmode && aplic->has_msicfg_mmode) {
		rc = aplic_check_msicfg(&aplic->msicfg_mmode);
		अगर (rc) {
			sbi_printf("%s: invalid M-mode msicfg rc=%d\n",
				__func__, rc);
			वापस rc;
		}
	}

	अगर (aplic->targets_mmode && aplic->has_msicfg_smode) {
		rc = aplic_check_msicfg(&aplic->msicfg_smode);
		अगर (rc) {
			sbi_printf("%s: invalid S-mode msicfg rc=%d\n",
				__func__, rc);
			वापस rc;
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
	क्रम (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		अगर (deleg->first_irq < first_deleg_irq)
			first_deleg_irq = deleg->first_irq;
		अगर (last_deleg_irq < deleg->last_irq)
			last_deleg_irq = deleg->last_irq;
	}

	अगर (aplic->targets_mmode ||
	    ((first_deleg_irq < last_deleg_irq) &&
	    (last_deleg_irq == aplic->अंक_source) &&
	    (first_deleg_irq == 1))) {
		rc = sbi_domain_मूल_add_memrange(aplic->addr, aplic->आकार, PAGE_आकार,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		अगर (rc)
			वापस rc;
	}

	अगर ((aplic->targets_mmode)) {
		aplic->irqchip		 = aplic_irqchip_template;
		aplic->irqchip.id	 = aplic->unique_id;
		aplic->irqchip.caps = SBI_IRQCHIP_CAPS_WIRED;
		aplic->irqchip.अंक_hwirq = aplic->अंक_source + 1;

		अगर (msi_mode) {
			aplic->irqchip.warm_init      = शून्य;
			aplic->irqchip.process_hwirqs = शून्य;
			aplic->irqchip.hwirq_eoi      = शून्य;
		}

		अगर (msi_mode)
			sbi_hartmask_set_all(&aplic->irqchip.target_harts);
		अन्यथा
			क्रम (i = 0; i < aplic->अंक_idc; i++)
				sbi_hartmask_set_hartindex(aplic->idc_map[i],
							   &aplic->irqchip.target_harts);

		rc = sbi_irqchip_add_device(&aplic->irqchip);
		अगर (rc) {
			sbi_printf("%s: sbi_irqchip_add_device failed rc=%d id=%lu mode=%s target_weight=%lu\n",
				__func__, rc,
				(अचिन्हित दीर्घ)aplic->irqchip.id,
				msi_mode ? "msi" : "direct",
				(अचिन्हित दीर्घ)sbi_hartmask_weight(
					&aplic->irqchip.target_harts));
			वापस rc;
		}
	}
	/* Attach to the aplic list */
	sbi_list_add_पुच्छ(&aplic->गाँठ, &aplic_list);

	वापस 0;
}
