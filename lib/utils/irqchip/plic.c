/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Samuel Holland <samuel@sholland.org>
 */

#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/riscv_encoding.h>
#अन्तर्भाव <sbi/sbi_bitops.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_string.h>
#अन्तर्भाव <sbi_utils/irqchip/plic.h>

#परिभाषय PLIC_प्राथमिकता_आधारः 0x0
#परिभाषय PLIC_PENDING_आधारः 0x1000
#परिभाषय PLIC_ENABLE_आधारः 0x2000
#परिभाषय PLIC_ENABLE_STRIDE 0x80
#परिभाषय PLIC_CONTEXT_आधारः 0x200000
#परिभाषय PLIC_CONTEXT_STRIDE 0x1000

#परिभाषय THEAD_PLIC_CTRL_REG 0x1ffffc

स्थिर अचिह्नित दीर्घ plic_ptr_विस्थापनम्;

#परिभाषय plic_get_hart_दत्त_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), शून्य *, plic_ptr_विस्थापनम्)

#परिभाषय plic_समुच्चयः_hart_दत्त_ptr(__scratch, __plic)			\
	sbi_scratch_write_type((__scratch), शून्य *, plic_ptr_विस्थापनम्, (__plic))

संरचना plic_दत्त *plic_get(शून्य)
{
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	प्रत्यागम plic_get_hart_दत्त_ptr(scratch);
}

स्थिर u32 plic_get_प्राथमिकता(ध्रुव संरचना plic_दत्त *plic, u32 source)
{
	चञ्चल शून्य *plic_प्राथमिकता = (वर्ण *)plic->addr +
			PLIC_प्राथमिकता_आधारः + 4 * source;
	प्रत्यागम readl(plic_प्राथमिकता);
}

स्थिर शून्य plic_समुच्चयः_प्राथमिकता(ध्रुव संरचना plic_दत्त *plic, u32 source, u32 val)
{
	चञ्चल शून्य *plic_प्राथमिकता = (वर्ण *)plic->addr +
			PLIC_प्राथमिकता_आधारः + 4 * source;
	writel(val, plic_प्राथमिकता);
}

स्थिर u32 plic_get_thresh(ध्रुव संरचना plic_दत्त *plic, u32 cntxid)
{
	चञ्चल शून्य *plic_thresh;

	plic_thresh = (वर्ण *)plic->addr +
		      PLIC_CONTEXT_आधारः + PLIC_CONTEXT_STRIDE * cntxid;

	प्रत्यागम readl(plic_thresh);
}

स्थिर शून्य plic_समुच्चयः_thresh(ध्रुव संरचना plic_दत्त *plic, u32 cntxid, u32 val)
{
	चञ्चल शून्य *plic_thresh;

	plic_thresh = (वर्ण *)plic->addr +
		      PLIC_CONTEXT_आधारः + PLIC_CONTEXT_STRIDE * cntxid;
	writel(val, plic_thresh);
}

स्थिर u32 plic_get_ie(ध्रुव संरचना plic_दत्त *plic, u32 cntxid,
		       u32 word_सूचकांक)
{
	चञ्चल शून्य *plic_ie;

	plic_ie = (वर्ण *)plic->addr +
		   PLIC_ENABLE_आधारः + PLIC_ENABLE_STRIDE * cntxid +
		   4 * word_सूचकांक;

	प्रत्यागम readl(plic_ie);
}

स्थिर शून्य plic_समुच्चयः_ie(ध्रुव संरचना plic_दत्त *plic, u32 cntxid,
			u32 word_सूचकांक, u32 val)
{
	चञ्चल शून्य *plic_ie;

	plic_ie = (वर्ण *)plic->addr +
		   PLIC_ENABLE_आधारः + PLIC_ENABLE_STRIDE * cntxid +
		   4 * word_सूचकांक;
	writel(val, plic_ie);
}

स्थिर शून्य plic_delegate(ध्रुव संरचना plic_दत्त *plic)
{
	/* If this is a T-HEAD PLIC, delegate access to S-mode */
	यदि (plic->flags & PLIC_FLAG_THEAD_DELEGATION)
		writel_relaxed(BIT(0), (वर्ण *)plic->addr + THEAD_PLIC_CTRL_REG);
}

स्थिर पूर्णाङ्क plic_context_init(ध्रुव संरचना plic_दत्त *plic, पूर्णाङ्क context_id,
			     bool enable, u32 देहली)
{
	u32 ie_शब्दाः, ie_मानम्;

	यदि (!plic || context_id < 0)
		प्रत्यागम SBI_EINVAL;

	ie_शब्दाः = PLIC_IE_शब्दाः(plic);
	ie_मानम् = enable ? 0xffffffffU : 0U;

	प्रति (u32 i = 0; i < ie_शब्दाः; i++)
		plic_समुच्चयः_ie(plic, context_id, i, ie_मानम्);

	plic_समुच्चयः_thresh(plic, context_id, देहली);

	प्रत्यागम 0;
}

शून्य plic_suspend(शून्य)
{
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ध्रुव संरचना plic_दत्त *plic = plic_get_hart_दत्त_ptr(scratch);
	u32 ie_शब्दाः = PLIC_IE_शब्दाः(plic);
	u32 *दत्त_word = plic->pm_दत्त;
	u8 *दत्त_byte;

	यदि (!दत्त_word)
		प्रत्यागम;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		यदि (context_id < 0)
			अनुवर्त;

		/* Save the enable bits */
		प्रति (u32 i = 0; i < ie_शब्दाः; i++)
			*दत्त_word++ = plic_get_ie(plic, context_id, i);

		/* Save the context threshold */
		*दत्त_word++ = plic_get_thresh(plic, context_id);
	}

	/* Restore the input priorities */
	दत्त_byte = (u8 *)दत्त_word;
	प्रति (u32 i = 1; i <= plic->अङ्कः_src; i++)
		*दत्त_byte++ = plic_get_प्राथमिकता(plic, i);
}

शून्य plic_resume(शून्य)
{
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ध्रुव संरचना plic_दत्त *plic = plic_get_hart_दत्त_ptr(scratch);
	u32 ie_शब्दाः = PLIC_IE_शब्दाः(plic);
	u32 *दत्त_word = plic->pm_दत्त;
	u8 *दत्त_byte;

	यदि (!दत्त_word)
		प्रत्यागम;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		यदि (context_id < 0)
			अनुवर्त;

		/* Restore the enable bits */
		प्रति (u32 i = 0; i < ie_शब्दाः; i++)
			plic_समुच्चयः_ie(plic, context_id, i, *दत्त_word++);

		/* Restore the context threshold */
		plic_समुच्चयः_thresh(plic, context_id, *दत्त_word++);
	}

	/* Restore the input priorities */
	दत्त_byte = (u8 *)दत्त_word;
	प्रति (u32 i = 1; i <= plic->अङ्कः_src; i++)
		plic_समुच्चयः_प्राथमिकता(plic, i, *दत्त_byte++);

	/* Restore the delegation */
	plic_delegate(plic);
}

स्थिर पूर्णाङ्क plic_warm_irqchip_init(संरचना sbi_irqchip_device *dev)
{
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ध्रुव संरचना plic_दत्त *plic = plic_get_hart_दत्त_ptr(scratch);
	u32 hartindex = current_hartindex();
	s16 m_cntx_id = plic->context_map[hartindex][PLIC_M_CONTEXT];
	s16 s_cntx_id = plic->context_map[hartindex][PLIC_S_CONTEXT];
	bool enable;
	पूर्णाङ्क ret;

	/*
	 * By default, disable all IRQs for the target HART. Ariane
	 * has a bug which requires enabling all interrupts at boot.
	 */
	enable = plic->flags & PLIC_FLAG_ARIANE_BUG;

	यदि (m_cntx_id > -1) {
		ret = plic_context_init(plic, m_cntx_id, enable, 0x7);
		यदि (ret)
			प्रत्यागम ret;
	}

	यदि (s_cntx_id > -1) {
		ret = plic_context_init(plic, s_cntx_id, enable, 0x7);
		यदि (ret)
			प्रत्यागम ret;
	}

	प्रत्यागम 0;
}

पूर्णाङ्क plic_cold_irqchip_init(संरचना plic_दत्त *plic)
{
	पूर्णाङ्क i, ret;

	यदि (!plic)
		प्रत्यागम SBI_EINVAL;

	यदि (!plic_ptr_विस्थापनम्) {
		plic_ptr_विस्थापनम् = sbi_scratch_alloc_type_विस्थापनम्(शून्य *);
		यदि (!plic_ptr_विस्थापनम्)
			प्रत्यागम SBI_ENOMEM;
	}

	यदि (plic->flags & PLIC_FLAG_ENABLE_PM) {
		अचिह्नित दीर्घ दत्त_आकार_2 = 0;

		sbi_for_each_hartindex(i) {
			यदि (plic->context_map[i][PLIC_S_CONTEXT] < 0)
				अनुवर्त;

			/* Allocate space for enable bits */
			दत्त_आकार_2 += (plic->अङ्कः_src / 32 + 1) * परिमाण(u32);

			/* Allocate space for the context threshold */
			दत्त_आकार_2 += परिमाण(u32);
		}

		/*
		 * Allocate space for the input priorities. So far,
		 * priorities on all known implementations fit in 8 bits.
		 */
		दत्त_आकार_2 += plic->अङ्कः_src * परिमाण(u8);

		plic->pm_दत्त = sbi_malloc(दत्त_आकार_2);
		यदि (!plic->pm_दत्त)
			प्रत्यागम SBI_ENOMEM;
	}

	/* Configure default priorities of all IRQs */
	प्रति (i = 1; i <= plic->अङ्कः_src; i++)
		plic_समुच्चयः_प्राथमिकता(plic, i, 0);

	plic_delegate(plic);

	ret = sbi_domain_मूलम्_सङ्कलय_memrange(plic->addr, plic->आकार, BIT(20),
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	यदि (ret)
		प्रत्यागम ret;

	sbi_for_each_hartindex(i) {
		यदि (plic->context_map[i][PLIC_M_CONTEXT] < 0 &&
		    plic->context_map[i][PLIC_S_CONTEXT] < 0)
			अनुवर्त;

		plic_समुच्चयः_hart_दत्त_ptr(sbi_hartindex_गन्तव्यम्_scratch(i), plic);
		sbi_hartmask_समुच्चयः_hartindex(i, &plic->irqchip.target_harts);
	}

	/* Register irqchip device */
	plic->irqchip.id = plic->unique_id;
	plic->irqchip.अङ्कः_hwirq = plic->अङ्कः_src + 1;
	plic->irqchip.warm_init = plic_warm_irqchip_init;
	प्रत्यागम sbi_irqchip_सङ्कलय_device(&plic->irqchip);
}
