/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Samuel Holland <samuel@sholland.org>
 */

#অন্তর্ভাৱ <sbi/riscv_io.h>
#অন্তর্ভাৱ <sbi/riscv_encoding.h>
#অন্তর্ভাৱ <sbi/sbi_bitops.h>
#অন্তর্ভাৱ <sbi/sbi_console.h>
#অন্তর্ভাৱ <sbi/sbi_domain.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_heap.h>
#অন্তর্ভাৱ <sbi/sbi_string.h>
#অন্তর্ভাৱ <sbi_utils/irqchip/plic.h>

#define PLIC_PRIORITY_BASE 0x0
#define PLIC_PENDING_BASE 0x1000
#define PLIC_ENABLE_BASE 0x2000
#define PLIC_ENABLE_STRIDE 0x80
#define PLIC_CONTEXT_BASE 0x200000
#define PLIC_CONTEXT_STRIDE 0x1000

#define THEAD_PLIC_CTRL_REG 0x1ffffc

স্থির unsigned long plic_ptr_offset;

#define plic_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), শূন্য *, plic_ptr_offset)

#define plic_set_hart_data_ptr(__scratch, __plic)			\
	sbi_scratch_write_type((__scratch), শূন্য *, plic_ptr_offset, (__plic))

সংরচনা plic_data *plic_get(শূন্য)
{
	সংরচনা sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ঘূৰাই plic_get_hart_data_ptr(scratch);
}

স্থির u32 plic_get_priority(ধ্রুৱ সংরচনা plic_data *plic, u32 source)
{
	চঞ্চল শূন্য *plic_priority = (ৱর্ণ *)plic->addr +
			PLIC_PRIORITY_BASE + 4 * source;
	ঘূৰাই readl(plic_priority);
}

স্থির শূন্য plic_set_priority(ধ্রুৱ সংরচনা plic_data *plic, u32 source, u32 val)
{
	চঞ্চল শূন্য *plic_priority = (ৱর্ণ *)plic->addr +
			PLIC_PRIORITY_BASE + 4 * source;
	writel(val, plic_priority);
}

স্থির u32 plic_get_thresh(ধ্রুৱ সংরচনা plic_data *plic, u32 cntxid)
{
	চঞ্চল শূন্য *plic_thresh;

	plic_thresh = (ৱর্ণ *)plic->addr +
		      PLIC_CONTEXT_BASE + PLIC_CONTEXT_STRIDE * cntxid;

	ঘূৰাই readl(plic_thresh);
}

স্থির শূন্য plic_set_thresh(ধ্রুৱ সংরচনা plic_data *plic, u32 cntxid, u32 val)
{
	চঞ্চল শূন্য *plic_thresh;

	plic_thresh = (ৱর্ণ *)plic->addr +
		      PLIC_CONTEXT_BASE + PLIC_CONTEXT_STRIDE * cntxid;
	writel(val, plic_thresh);
}

স্থির u32 plic_get_ie(ধ্রুৱ সংরচনা plic_data *plic, u32 cntxid,
		       u32 word_index)
{
	চঞ্চল শূন্য *plic_ie;

	plic_ie = (ৱর্ণ *)plic->addr +
		   PLIC_ENABLE_BASE + PLIC_ENABLE_STRIDE * cntxid +
		   4 * word_index;

	ঘূৰাই readl(plic_ie);
}

স্থির শূন্য plic_set_ie(ধ্রুৱ সংরচনা plic_data *plic, u32 cntxid,
			u32 word_index, u32 val)
{
	চঞ্চল শূন্য *plic_ie;

	plic_ie = (ৱর্ণ *)plic->addr +
		   PLIC_ENABLE_BASE + PLIC_ENABLE_STRIDE * cntxid +
		   4 * word_index;
	writel(val, plic_ie);
}

স্থির শূন্য plic_delegate(ধ্রুৱ সংরচনা plic_data *plic)
{
	/* If this is a T-HEAD PLIC, delegate access to S-mode */
	যদি (plic->flags & PLIC_FLAG_THEAD_DELEGATION)
		writel_relaxed(BIT(0), (ৱর্ণ *)plic->addr + THEAD_PLIC_CTRL_REG);
}

স্থির পূর্ণাঙ্ক plic_context_init(ধ্রুৱ সংরচনা plic_data *plic, পূর্ণাঙ্ক context_id,
			     bool enable, u32 threshold)
{
	u32 ie_শব্দগুলি, ie_মান;

	যদি (!plic || context_id < 0)
		ঘূৰাই SBI_EINVAL;

	ie_শব্দগুলি = PLIC_IE_শব্দগুলি(plic);
	ie_মান = enable ? 0xffffffffU : 0U;

	বাবে (u32 i = 0; i < ie_শব্দগুলি; i++)
		plic_set_ie(plic, context_id, i, ie_মান);

	plic_set_thresh(plic, context_id, threshold);

	ঘূৰাই 0;
}

শূন্য plic_suspend(শূন্য)
{
	সংরচনা sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ধ্রুৱ সংরচনা plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 ie_শব্দগুলি = PLIC_IE_শব্দগুলি(plic);
	u32 *data_word = plic->pm_data;
	u8 *data_byte;

	যদি (!data_word)
		ঘূৰাই;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		যদি (context_id < 0)
			অনুৱর্ত;

		/* Save the enable bits */
		বাবে (u32 i = 0; i < ie_শব্দগুলি; i++)
			*data_word++ = plic_get_ie(plic, context_id, i);

		/* Save the context threshold */
		*data_word++ = plic_get_thresh(plic, context_id);
	}

	/* Restore the input priorities */
	data_byte = (u8 *)data_word;
	বাবে (u32 i = 1; i <= plic->অঙ্ক_src; i++)
		*data_byte++ = plic_get_priority(plic, i);
}

শূন্য plic_resume(শূন্য)
{
	সংরচনা sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ধ্রুৱ সংরচনা plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 ie_শব্দগুলি = PLIC_IE_শব্দগুলি(plic);
	u32 *data_word = plic->pm_data;
	u8 *data_byte;

	যদি (!data_word)
		ঘূৰাই;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		যদি (context_id < 0)
			অনুৱর্ত;

		/* Restore the enable bits */
		বাবে (u32 i = 0; i < ie_শব্দগুলি; i++)
			plic_set_ie(plic, context_id, i, *data_word++);

		/* Restore the context threshold */
		plic_set_thresh(plic, context_id, *data_word++);
	}

	/* Restore the input priorities */
	data_byte = (u8 *)data_word;
	বাবে (u32 i = 1; i <= plic->অঙ্ক_src; i++)
		plic_set_priority(plic, i, *data_byte++);

	/* Restore the delegation */
	plic_delegate(plic);
}

স্থির পূর্ণাঙ্ক plic_warm_irqchip_init(সংরচনা sbi_irqchip_device *dev)
{
	সংরচনা sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ধ্রুৱ সংরচনা plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 hartindex = current_hartindex();
	s16 m_cntx_id = plic->context_map[hartindex][PLIC_M_CONTEXT];
	s16 s_cntx_id = plic->context_map[hartindex][PLIC_S_CONTEXT];
	bool enable;
	পূর্ণাঙ্ক ret;

	/*
	 * By default, disable all IRQs for the target HART. Ariane
	 * has a bug which requires enabling all interrupts at boot.
	 */
	enable = plic->flags & PLIC_FLAG_ARIANE_BUG;

	যদি (m_cntx_id > -1) {
		ret = plic_context_init(plic, m_cntx_id, enable, 0x7);
		যদি (ret)
			ঘূৰাই ret;
	}

	যদি (s_cntx_id > -1) {
		ret = plic_context_init(plic, s_cntx_id, enable, 0x7);
		যদি (ret)
			ঘূৰাই ret;
	}

	ঘূৰাই 0;
}

পূর্ণাঙ্ক plic_cold_irqchip_init(সংরচনা plic_data *plic)
{
	পূর্ণাঙ্ক i, ret;

	যদি (!plic)
		ঘূৰাই SBI_EINVAL;

	যদি (!plic_ptr_offset) {
		plic_ptr_offset = sbi_scratch_alloc_type_offset(শূন্য *);
		যদি (!plic_ptr_offset)
			ঘূৰাই SBI_ENOMEM;
	}

	যদি (plic->flags & PLIC_FLAG_ENABLE_PM) {
		unsigned long data_size = 0;

		sbi_for_each_hartindex(i) {
			যদি (plic->context_map[i][PLIC_S_CONTEXT] < 0)
				অনুৱর্ত;

			/* Allocate space for enable bits */
			data_size += (plic->অঙ্ক_src / 32 + 1) * পরিমাণ(u32);

			/* Allocate space for the context threshold */
			data_size += পরিমাণ(u32);
		}

		/*
		 * Allocate space for the input priorities. So far,
		 * priorities on all known implementations fit in 8 bits.
		 */
		data_size += plic->অঙ্ক_src * পরিমাণ(u8);

		plic->pm_data = sbi_malloc(data_size);
		যদি (!plic->pm_data)
			ঘূৰাই SBI_ENOMEM;
	}

	/* Configure default priorities of all IRQs */
	বাবে (i = 1; i <= plic->অঙ্ক_src; i++)
		plic_set_priority(plic, i, 0);

	plic_delegate(plic);

	ret = sbi_domain_মূল_add_memrange(plic->addr, plic->size, BIT(20),
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	যদি (ret)
		ঘূৰাই ret;

	sbi_for_each_hartindex(i) {
		যদি (plic->context_map[i][PLIC_M_CONTEXT] < 0 &&
		    plic->context_map[i][PLIC_S_CONTEXT] < 0)
			অনুৱর্ত;

		plic_set_hart_data_ptr(sbi_hartindex_to_scratch(i), plic);
		sbi_hartmask_set_hartindex(i, &plic->irqchip.target_harts);
	}

	/* Register irqchip device */
	plic->irqchip.id = plic->unique_id;
	plic->irqchip.অঙ্ক_hwirq = plic->অঙ্ক_src + 1;
	plic->irqchip.warm_init = plic_warm_irqchip_init;
	ঘূৰাই sbi_irqchip_add_device(&plic->irqchip);
}
