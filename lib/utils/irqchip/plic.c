/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Samuel Holland <samuel@sholland.org>
 */

#समावेश <sbi/riscv_io.h>
#समावेश <sbi/riscv_encoding.h>
#समावेश <sbi/sbi_bitops.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi/sbi_string.h>
#समावेश <sbi_utils/irqchip/plic.h>

#घोषणा PLIC_PRIORITY_BASE 0x0
#घोषणा PLIC_PENDING_BASE 0x1000
#घोषणा PLIC_ENABLE_BASE 0x2000
#घोषणा PLIC_ENABLE_STRIDE 0x80
#घोषणा PLIC_CONTEXT_BASE 0x200000
#घोषणा PLIC_CONTEXT_STRIDE 0x1000

#घोषणा THEAD_PLIC_CTRL_REG 0x1ffffc

अटल अचिन्हित दीर्घ plic_ptr_offset;

#घोषणा plic_get_hart_तथ्य_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), व्योम *, plic_ptr_offset)

#घोषणा plic_set_hart_तथ्य_ptr(__scratch, __plic)			\
	sbi_scratch_write_type((__scratch), व्योम *, plic_ptr_offset, (__plic))

काष्ठा plic_तथ्य *plic_get(व्योम)
{
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	वापस plic_get_hart_तथ्य_ptr(scratch);
}

अटल u32 plic_get_priority(स्थिर काष्ठा plic_तथ्य *plic, u32 source)
{
	अस्थिर व्योम *plic_priority = (अक्षर *)plic->addr +
			PLIC_PRIORITY_BASE + 4 * source;
	वापस readl(plic_priority);
}

अटल व्योम plic_set_priority(स्थिर काष्ठा plic_तथ्य *plic, u32 source, u32 val)
{
	अस्थिर व्योम *plic_priority = (अक्षर *)plic->addr +
			PLIC_PRIORITY_BASE + 4 * source;
	writel(val, plic_priority);
}

अटल u32 plic_get_thresh(स्थिर काष्ठा plic_तथ्य *plic, u32 cntxid)
{
	अस्थिर व्योम *plic_thresh;

	plic_thresh = (अक्षर *)plic->addr +
		      PLIC_CONTEXT_BASE + PLIC_CONTEXT_STRIDE * cntxid;

	वापस readl(plic_thresh);
}

अटल व्योम plic_set_thresh(स्थिर काष्ठा plic_तथ्य *plic, u32 cntxid, u32 val)
{
	अस्थिर व्योम *plic_thresh;

	plic_thresh = (अक्षर *)plic->addr +
		      PLIC_CONTEXT_BASE + PLIC_CONTEXT_STRIDE * cntxid;
	writel(val, plic_thresh);
}

अटल u32 plic_get_ie(स्थिर काष्ठा plic_तथ्य *plic, u32 cntxid,
		       u32 word_सूचकांक)
{
	अस्थिर व्योम *plic_ie;

	plic_ie = (अक्षर *)plic->addr +
		   PLIC_ENABLE_BASE + PLIC_ENABLE_STRIDE * cntxid +
		   4 * word_सूचकांक;

	वापस readl(plic_ie);
}

अटल व्योम plic_set_ie(स्थिर काष्ठा plic_तथ्य *plic, u32 cntxid,
			u32 word_सूचकांक, u32 val)
{
	अस्थिर व्योम *plic_ie;

	plic_ie = (अक्षर *)plic->addr +
		   PLIC_ENABLE_BASE + PLIC_ENABLE_STRIDE * cntxid +
		   4 * word_सूचकांक;
	writel(val, plic_ie);
}

अटल व्योम plic_delegate(स्थिर काष्ठा plic_तथ्य *plic)
{
	/* If this is a T-HEAD PLIC, delegate access to S-mode */
	अगर (plic->flags & PLIC_FLAG_THEAD_DELEGATION)
		writel_relaxed(BIT(0), (अक्षर *)plic->addr + THEAD_PLIC_CTRL_REG);
}

अटल पूर्णांक plic_context_init(स्थिर काष्ठा plic_तथ्य *plic, पूर्णांक context_id,
			     bool enable, u32 threshold)
{
	u32 ie_शब्दगण, ie_मूल्य;

	अगर (!plic || context_id < 0)
		वापस SBI_EINVAL;

	ie_शब्दगण = PLIC_IE_शब्दगण(plic);
	ie_मूल्य = enable ? 0xffffffffU : 0U;

	क्रम (u32 i = 0; i < ie_शब्दगण; i++)
		plic_set_ie(plic, context_id, i, ie_मूल्य);

	plic_set_thresh(plic, context_id, threshold);

	वापस 0;
}

व्योम plic_suspend(व्योम)
{
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	स्थिर काष्ठा plic_तथ्य *plic = plic_get_hart_तथ्य_ptr(scratch);
	u32 ie_शब्दगण = PLIC_IE_शब्दगण(plic);
	u32 *तथ्य_word = plic->pm_तथ्य;
	u8 *तथ्य_byte;

	अगर (!तथ्य_word)
		वापस;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		अगर (context_id < 0)
			जारी;

		/* Save the enable bits */
		क्रम (u32 i = 0; i < ie_शब्दगण; i++)
			*तथ्य_word++ = plic_get_ie(plic, context_id, i);

		/* Save the context threshold */
		*तथ्य_word++ = plic_get_thresh(plic, context_id);
	}

	/* Restore the input priorities */
	तथ्य_byte = (u8 *)तथ्य_word;
	क्रम (u32 i = 1; i <= plic->अंक_src; i++)
		*तथ्य_byte++ = plic_get_priority(plic, i);
}

व्योम plic_resume(व्योम)
{
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	स्थिर काष्ठा plic_तथ्य *plic = plic_get_hart_तथ्य_ptr(scratch);
	u32 ie_शब्दगण = PLIC_IE_शब्दगण(plic);
	u32 *तथ्य_word = plic->pm_तथ्य;
	u8 *तथ्य_byte;

	अगर (!तथ्य_word)
		वापस;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		अगर (context_id < 0)
			जारी;

		/* Restore the enable bits */
		क्रम (u32 i = 0; i < ie_शब्दगण; i++)
			plic_set_ie(plic, context_id, i, *तथ्य_word++);

		/* Restore the context threshold */
		plic_set_thresh(plic, context_id, *तथ्य_word++);
	}

	/* Restore the input priorities */
	तथ्य_byte = (u8 *)तथ्य_word;
	क्रम (u32 i = 1; i <= plic->अंक_src; i++)
		plic_set_priority(plic, i, *तथ्य_byte++);

	/* Restore the delegation */
	plic_delegate(plic);
}

अटल पूर्णांक plic_warm_irqchip_init(काष्ठा sbi_irqchip_device *dev)
{
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	स्थिर काष्ठा plic_तथ्य *plic = plic_get_hart_तथ्य_ptr(scratch);
	u32 hartindex = current_hartindex();
	s16 m_cntx_id = plic->context_map[hartindex][PLIC_M_CONTEXT];
	s16 s_cntx_id = plic->context_map[hartindex][PLIC_S_CONTEXT];
	bool enable;
	पूर्णांक ret;

	/*
	 * By default, disable all IRQs for the target HART. Ariane
	 * has a bug which requires enabling all interrupts at boot.
	 */
	enable = plic->flags & PLIC_FLAG_ARIANE_BUG;

	अगर (m_cntx_id > -1) {
		ret = plic_context_init(plic, m_cntx_id, enable, 0x7);
		अगर (ret)
			वापस ret;
	}

	अगर (s_cntx_id > -1) {
		ret = plic_context_init(plic, s_cntx_id, enable, 0x7);
		अगर (ret)
			वापस ret;
	}

	वापस 0;
}

पूर्णांक plic_cold_irqchip_init(काष्ठा plic_तथ्य *plic)
{
	पूर्णांक i, ret;

	अगर (!plic)
		वापस SBI_EINVAL;

	अगर (!plic_ptr_offset) {
		plic_ptr_offset = sbi_scratch_alloc_type_offset(व्योम *);
		अगर (!plic_ptr_offset)
			वापस SBI_ENOMEM;
	}

	अगर (plic->flags & PLIC_FLAG_ENABLE_PM) {
		अचिन्हित दीर्घ तथ्य_आकार_2 = 0;

		sbi_for_each_hartindex(i) {
			अगर (plic->context_map[i][PLIC_S_CONTEXT] < 0)
				जारी;

			/* Allocate space for enable bits */
			तथ्य_आकार_2 += (plic->अंक_src / 32 + 1) * माप(u32);

			/* Allocate space for the context threshold */
			तथ्य_आकार_2 += माप(u32);
		}

		/*
		 * Allocate space for the input priorities. So far,
		 * priorities on all known implementations fit in 8 bits.
		 */
		तथ्य_आकार_2 += plic->अंक_src * माप(u8);

		plic->pm_तथ्य = sbi_malloc(तथ्य_आकार_2);
		अगर (!plic->pm_तथ्य)
			वापस SBI_ENOMEM;
	}

	/* Configure default priorities of all IRQs */
	क्रम (i = 1; i <= plic->अंक_src; i++)
		plic_set_priority(plic, i, 0);

	plic_delegate(plic);

	ret = sbi_domain_मूल_add_memrange(plic->addr, plic->आकार, BIT(20),
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	अगर (ret)
		वापस ret;

	sbi_for_each_hartindex(i) {
		अगर (plic->context_map[i][PLIC_M_CONTEXT] < 0 &&
		    plic->context_map[i][PLIC_S_CONTEXT] < 0)
			जारी;

		plic_set_hart_तथ्य_ptr(sbi_hartindex_to_scratch(i), plic);
		sbi_hartmask_set_hartindex(i, &plic->irqchip.target_harts);
	}

	/* Register irqchip device */
	plic->irqchip.id = plic->unique_id;
	plic->irqchip.अंक_hwirq = plic->अंक_src + 1;
	plic->irqchip.warm_init = plic_warm_irqchip_init;
	वापस sbi_irqchip_add_device(&plic->irqchip);
}
