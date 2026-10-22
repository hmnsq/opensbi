/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Samuel Holland <samuel@sholland.org>
 */

#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/riscv_encoding.h>
#অন্তর্ভাব <sbi/sbi_bitops.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi/sbi_string.h>
#অন্তর্ভাব <sbi_utils/irqchip/plic.h>

#define PLIC_PRIORITY_BASE 0x0
#define PLIC_PENDING_BASE 0x1000
#define PLIC_ENABLE_BASE 0x2000
#define PLIC_ENABLE_STRIDE 0x80
#define PLIC_CONTEXT_BASE 0x200000
#define PLIC_CONTEXT_STRIDE 0x1000

#define THEAD_PLIC_CTRL_REG 0x1ffffc

স্থির unsigned long plic_ptr_offset;

#define plic_get_hart_তথ্য_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), শূন্য *, plic_ptr_offset)

#define plic_set_hart_তথ্য_ptr(__scratch, __plic)			\
	sbi_scratch_write_type((__scratch), শূন্য *, plic_ptr_offset, (__plic))

কাঠামো plic_তথ্য *plic_get(শূন্য)
{
	কাঠামো sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ফেরত plic_get_hart_তথ্য_ptr(scratch);
}

স্থির u32 plic_get_priority(ধ্রুবক কাঠামো plic_তথ্য *plic, u32 source)
{
	অস্থির শূন্য *plic_priority = (অক্ষর *)plic->addr +
			PLIC_PRIORITY_BASE + 4 * source;
	ফেরত readl(plic_priority);
}

স্থির শূন্য plic_set_priority(ধ্রুবক কাঠামো plic_তথ্য *plic, u32 source, u32 val)
{
	অস্থির শূন্য *plic_priority = (অক্ষর *)plic->addr +
			PLIC_PRIORITY_BASE + 4 * source;
	writel(val, plic_priority);
}

স্থির u32 plic_get_thresh(ধ্রুবক কাঠামো plic_তথ্য *plic, u32 cntxid)
{
	অস্থির শূন্য *plic_thresh;

	plic_thresh = (অক্ষর *)plic->addr +
		      PLIC_CONTEXT_BASE + PLIC_CONTEXT_STRIDE * cntxid;

	ফেরত readl(plic_thresh);
}

স্থির শূন্য plic_set_thresh(ধ্রুবক কাঠামো plic_তথ্য *plic, u32 cntxid, u32 val)
{
	অস্থির শূন্য *plic_thresh;

	plic_thresh = (অক্ষর *)plic->addr +
		      PLIC_CONTEXT_BASE + PLIC_CONTEXT_STRIDE * cntxid;
	writel(val, plic_thresh);
}

স্থির u32 plic_get_ie(ধ্রুবক কাঠামো plic_তথ্য *plic, u32 cntxid,
		       u32 word_সূচক)
{
	অস্থির শূন্য *plic_ie;

	plic_ie = (অক্ষর *)plic->addr +
		   PLIC_ENABLE_BASE + PLIC_ENABLE_STRIDE * cntxid +
		   4 * word_সূচক;

	ফেরত readl(plic_ie);
}

স্থির শূন্য plic_set_ie(ধ্রুবক কাঠামো plic_তথ্য *plic, u32 cntxid,
			u32 word_সূচক, u32 val)
{
	অস্থির শূন্য *plic_ie;

	plic_ie = (অক্ষর *)plic->addr +
		   PLIC_ENABLE_BASE + PLIC_ENABLE_STRIDE * cntxid +
		   4 * word_সূচক;
	writel(val, plic_ie);
}

স্থির শূন্য plic_delegate(ধ্রুবক কাঠামো plic_তথ্য *plic)
{
	/* If this is a T-HEAD PLIC, delegate access to S-mode */
	যদি (plic->flags & PLIC_FLAG_THEAD_DELEGATION)
		writel_relaxed(BIT(0), (অক্ষর *)plic->addr + THEAD_PLIC_CTRL_REG);
}

স্থির পূর্ণসংখ্যা plic_context_init(ধ্রুবক কাঠামো plic_তথ্য *plic, পূর্ণসংখ্যা context_id,
			     bool enable, u32 threshold)
{
	u32 ie_শব্দগুলি, ie_মান;

	যদি (!plic || context_id < 0)
		ফেরত SBI_EINVAL;

	ie_শব্দগুলি = PLIC_IE_শব্দগুলি(plic);
	ie_মান = enable ? 0xffffffffU : 0U;

	জন্য (u32 i = 0; i < ie_শব্দগুলি; i++)
		plic_set_ie(plic, context_id, i, ie_মান);

	plic_set_thresh(plic, context_id, threshold);

	ফেরত 0;
}

শূন্য plic_suspend(শূন্য)
{
	কাঠামো sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ধ্রুবক কাঠামো plic_তথ্য *plic = plic_get_hart_তথ্য_ptr(scratch);
	u32 ie_শব্দগুলি = PLIC_IE_শব্দগুলি(plic);
	u32 *তথ্য_word = plic->pm_তথ্য;
	u8 *তথ্য_byte;

	যদি (!তথ্য_word)
		ফেরত;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		যদি (context_id < 0)
			চালু;

		/* Save the enable bits */
		জন্য (u32 i = 0; i < ie_শব্দগুলি; i++)
			*তথ্য_word++ = plic_get_ie(plic, context_id, i);

		/* Save the context threshold */
		*তথ্য_word++ = plic_get_thresh(plic, context_id);
	}

	/* Restore the input priorities */
	তথ্য_byte = (u8 *)তথ্য_word;
	জন্য (u32 i = 1; i <= plic->অঙ্ক_src; i++)
		*তথ্য_byte++ = plic_get_priority(plic, i);
}

শূন্য plic_resume(শূন্য)
{
	কাঠামো sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ধ্রুবক কাঠামো plic_তথ্য *plic = plic_get_hart_তথ্য_ptr(scratch);
	u32 ie_শব্দগুলি = PLIC_IE_শব্দগুলি(plic);
	u32 *তথ্য_word = plic->pm_তথ্য;
	u8 *তথ্য_byte;

	যদি (!তথ্য_word)
		ফেরত;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		যদি (context_id < 0)
			চালু;

		/* Restore the enable bits */
		জন্য (u32 i = 0; i < ie_শব্দগুলি; i++)
			plic_set_ie(plic, context_id, i, *তথ্য_word++);

		/* Restore the context threshold */
		plic_set_thresh(plic, context_id, *তথ্য_word++);
	}

	/* Restore the input priorities */
	তথ্য_byte = (u8 *)তথ্য_word;
	জন্য (u32 i = 1; i <= plic->অঙ্ক_src; i++)
		plic_set_priority(plic, i, *তথ্য_byte++);

	/* Restore the delegation */
	plic_delegate(plic);
}

স্থির পূর্ণসংখ্যা plic_warm_irqchip_init(কাঠামো sbi_irqchip_device *dev)
{
	কাঠামো sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ধ্রুবক কাঠামো plic_তথ্য *plic = plic_get_hart_তথ্য_ptr(scratch);
	u32 hartindex = current_hartindex();
	s16 m_cntx_id = plic->context_map[hartindex][PLIC_M_CONTEXT];
	s16 s_cntx_id = plic->context_map[hartindex][PLIC_S_CONTEXT];
	bool enable;
	পূর্ণসংখ্যা ret;

	/*
	 * By default, disable all IRQs for the target HART. Ariane
	 * has a bug which requires enabling all interrupts at boot.
	 */
	enable = plic->flags & PLIC_FLAG_ARIANE_BUG;

	যদি (m_cntx_id > -1) {
		ret = plic_context_init(plic, m_cntx_id, enable, 0x7);
		যদি (ret)
			ফেরত ret;
	}

	যদি (s_cntx_id > -1) {
		ret = plic_context_init(plic, s_cntx_id, enable, 0x7);
		যদি (ret)
			ফেরত ret;
	}

	ফেরত 0;
}

পূর্ণসংখ্যা plic_cold_irqchip_init(কাঠামো plic_তথ্য *plic)
{
	পূর্ণসংখ্যা i, ret;

	যদি (!plic)
		ফেরত SBI_EINVAL;

	যদি (!plic_ptr_offset) {
		plic_ptr_offset = sbi_scratch_alloc_type_offset(শূন্য *);
		যদি (!plic_ptr_offset)
			ফেরত SBI_ENOMEM;
	}

	যদি (plic->flags & PLIC_FLAG_ENABLE_PM) {
		unsigned long তথ্য_আকার_2 = 0;

		sbi_for_each_hartindex(i) {
			যদি (plic->context_map[i][PLIC_S_CONTEXT] < 0)
				চালু;

			/* Allocate space for enable bits */
			তথ্য_আকার_2 += (plic->অঙ্ক_src / 32 + 1) * আকার(u32);

			/* Allocate space for the context threshold */
			তথ্য_আকার_2 += আকার(u32);
		}

		/*
		 * Allocate space for the input priorities. So far,
		 * priorities on all known implementations fit in 8 bits.
		 */
		তথ্য_আকার_2 += plic->অঙ্ক_src * আকার(u8);

		plic->pm_তথ্য = sbi_malloc(তথ্য_আকার_2);
		যদি (!plic->pm_তথ্য)
			ফেরত SBI_ENOMEM;
	}

	/* Configure default priorities of all IRQs */
	জন্য (i = 1; i <= plic->অঙ্ক_src; i++)
		plic_set_priority(plic, i, 0);

	plic_delegate(plic);

	ret = sbi_domain_মূল_add_memrange(plic->addr, plic->আকার_2, BIT(20),
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	যদি (ret)
		ফেরত ret;

	sbi_for_each_hartindex(i) {
		যদি (plic->context_map[i][PLIC_M_CONTEXT] < 0 &&
		    plic->context_map[i][PLIC_S_CONTEXT] < 0)
			চালু;

		plic_set_hart_তথ্য_ptr(sbi_hartindex_to_scratch(i), plic);
		sbi_hartmask_set_hartindex(i, &plic->irqchip.target_harts);
	}

	/* Register irqchip device */
	plic->irqchip.id = plic->unique_id;
	plic->irqchip.অঙ্ক_hwirq = plic->অঙ্ক_src + 1;
	plic->irqchip.warm_init = plic_warm_irqchip_init;
	ফেরত sbi_irqchip_add_device(&plic->irqchip);
}
