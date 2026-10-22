/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Samuel Holland <samuel@sholland.org>
 */

#ضمن <sbi/riscv_io.h>
#ضمن <sbi/riscv_encoding.h>
#ضمن <sbi/sbi_bitops.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_string.h>
#ضمن <sbi_utils/irqchip/plic.h>

#define PLIC_PRIORITY_BASE 0x0
#define PLIC_PENDING_BASE 0x1000
#define PLIC_ENABLE_BASE 0x2000
#define PLIC_ENABLE_STRIDE 0x80
#define PLIC_CONTEXT_BASE 0x200000
#define PLIC_CONTEXT_STRIDE 0x1000

#define THEAD_PLIC_CTRL_REG 0x1ffffc

ساكن unsigned long plic_ptr_offset;

#define plic_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), فراغ *, plic_ptr_offset)

#define plic_set_hart_data_ptr(__scratch, __plic)			\
	sbi_scratch_write_type((__scratch), فراغ *, plic_ptr_offset, (__plic))

بنية plic_data *plic_get(فراغ)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ܦܢܐ plic_get_hart_data_ptr(scratch);
}

ساكن u32 plic_get_priority(ثابت بنية plic_data *plic, u32 source)
{
	متطاير فراغ *plic_priority = (حرف *)plic->addr +
			PLIC_PRIORITY_BASE + 4 * source;
	ܦܢܐ readl(plic_priority);
}

ساكن فراغ plic_set_priority(ثابت بنية plic_data *plic, u32 source, u32 val)
{
	متطاير فراغ *plic_priority = (حرف *)plic->addr +
			PLIC_PRIORITY_BASE + 4 * source;
	writel(val, plic_priority);
}

ساكن u32 plic_get_thresh(ثابت بنية plic_data *plic, u32 cntxid)
{
	متطاير فراغ *plic_thresh;

	plic_thresh = (حرف *)plic->addr +
		      PLIC_CONTEXT_BASE + PLIC_CONTEXT_STRIDE * cntxid;

	ܦܢܐ readl(plic_thresh);
}

ساكن فراغ plic_set_thresh(ثابت بنية plic_data *plic, u32 cntxid, u32 val)
{
	متطاير فراغ *plic_thresh;

	plic_thresh = (حرف *)plic->addr +
		      PLIC_CONTEXT_BASE + PLIC_CONTEXT_STRIDE * cntxid;
	writel(val, plic_thresh);
}

ساكن u32 plic_get_ie(ثابت بنية plic_data *plic, u32 cntxid,
		       u32 word_index)
{
	متطاير فراغ *plic_ie;

	plic_ie = (حرف *)plic->addr +
		   PLIC_ENABLE_BASE + PLIC_ENABLE_STRIDE * cntxid +
		   4 * word_index;

	ܦܢܐ readl(plic_ie);
}

ساكن فراغ plic_set_ie(ثابت بنية plic_data *plic, u32 cntxid,
			u32 word_index, u32 val)
{
	متطاير فراغ *plic_ie;

	plic_ie = (حرف *)plic->addr +
		   PLIC_ENABLE_BASE + PLIC_ENABLE_STRIDE * cntxid +
		   4 * word_index;
	writel(val, plic_ie);
}

ساكن فراغ plic_delegate(ثابت بنية plic_data *plic)
{
	/* If this is a T-HEAD PLIC, delegate access to S-mode */
	ܐܢ (plic->flags & PLIC_FLAG_THEAD_DELEGATION)
		writel_relaxed(BIT(0), (حرف *)plic->addr + THEAD_PLIC_CTRL_REG);
}

ساكن صحيح plic_context_init(ثابت بنية plic_data *plic, صحيح context_id,
			     bool enable, u32 threshold)
{
	u32 ie_words, ie_value;

	ܐܢ (!plic || context_id < 0)
		ܦܢܐ SBI_EINVAL;

	ie_words = PLIC_IE_WORDS(plic);
	ie_value = enable ? 0xffffffffU : 0U;

	لكل (u32 i = 0; i < ie_words; i++)
		plic_set_ie(plic, context_id, i, ie_value);

	plic_set_thresh(plic, context_id, threshold);

	ܦܢܐ 0;
}

فراغ plic_suspend(فراغ)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ثابت بنية plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 ie_words = PLIC_IE_WORDS(plic);
	u32 *data_word = plic->pm_data;
	u8 *data_byte;

	ܐܢ (!data_word)
		ܦܢܐ;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		ܐܢ (context_id < 0)
			استمر;

		/* Save the enable bits */
		لكل (u32 i = 0; i < ie_words; i++)
			*data_word++ = plic_get_ie(plic, context_id, i);

		/* Save the context threshold */
		*data_word++ = plic_get_thresh(plic, context_id);
	}

	/* Restore the input priorities */
	data_byte = (u8 *)data_word;
	لكل (u32 i = 1; i <= plic->num_src; i++)
		*data_byte++ = plic_get_priority(plic, i);
}

فراغ plic_resume(فراغ)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ثابت بنية plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 ie_words = PLIC_IE_WORDS(plic);
	u32 *data_word = plic->pm_data;
	u8 *data_byte;

	ܐܢ (!data_word)
		ܦܢܐ;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		ܐܢ (context_id < 0)
			استمر;

		/* Restore the enable bits */
		لكل (u32 i = 0; i < ie_words; i++)
			plic_set_ie(plic, context_id, i, *data_word++);

		/* Restore the context threshold */
		plic_set_thresh(plic, context_id, *data_word++);
	}

	/* Restore the input priorities */
	data_byte = (u8 *)data_word;
	لكل (u32 i = 1; i <= plic->num_src; i++)
		plic_set_priority(plic, i, *data_byte++);

	/* Restore the delegation */
	plic_delegate(plic);
}

ساكن صحيح plic_warm_irqchip_init(بنية sbi_irqchip_device *dev)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ثابت بنية plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 hartindex = current_hartindex();
	s16 m_cntx_id = plic->context_map[hartindex][PLIC_M_CONTEXT];
	s16 s_cntx_id = plic->context_map[hartindex][PLIC_S_CONTEXT];
	bool enable;
	صحيح ret;

	/*
	 * By default, disable all IRQs for the target HART. Ariane
	 * has a bug which requires enabling all interrupts at boot.
	 */
	enable = plic->flags & PLIC_FLAG_ARIANE_BUG;

	ܐܢ (m_cntx_id > -1) {
		ret = plic_context_init(plic, m_cntx_id, enable, 0x7);
		ܐܢ (ret)
			ܦܢܐ ret;
	}

	ܐܢ (s_cntx_id > -1) {
		ret = plic_context_init(plic, s_cntx_id, enable, 0x7);
		ܐܢ (ret)
			ܦܢܐ ret;
	}

	ܦܢܐ 0;
}

صحيح plic_cold_irqchip_init(بنية plic_data *plic)
{
	صحيح i, ret;

	ܐܢ (!plic)
		ܦܢܐ SBI_EINVAL;

	ܐܢ (!plic_ptr_offset) {
		plic_ptr_offset = sbi_scratch_alloc_type_offset(فراغ *);
		ܐܢ (!plic_ptr_offset)
			ܦܢܐ SBI_ENOMEM;
	}

	ܐܢ (plic->flags & PLIC_FLAG_ENABLE_PM) {
		unsigned long data_size = 0;

		sbi_for_each_hartindex(i) {
			ܐܢ (plic->context_map[i][PLIC_S_CONTEXT] < 0)
				استمر;

			/* Allocate space for enable bits */
			data_size += (plic->num_src / 32 + 1) * حجم(u32);

			/* Allocate space for the context threshold */
			data_size += حجم(u32);
		}

		/*
		 * Allocate space for the input priorities. So far,
		 * priorities on all known implementations fit in 8 bits.
		 */
		data_size += plic->num_src * حجم(u8);

		plic->pm_data = sbi_malloc(data_size);
		ܐܢ (!plic->pm_data)
			ܦܢܐ SBI_ENOMEM;
	}

	/* Configure default priorities of all IRQs */
	لكل (i = 1; i <= plic->num_src; i++)
		plic_set_priority(plic, i, 0);

	plic_delegate(plic);

	ret = sbi_domain_root_add_memrange(plic->addr, plic->size, BIT(20),
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	ܐܢ (ret)
		ܦܢܐ ret;

	sbi_for_each_hartindex(i) {
		ܐܢ (plic->context_map[i][PLIC_M_CONTEXT] < 0 &&
		    plic->context_map[i][PLIC_S_CONTEXT] < 0)
			استمر;

		plic_set_hart_data_ptr(sbi_hartindex_to_scratch(i), plic);
		sbi_hartmask_set_hartindex(i, &plic->irqchip.target_harts);
	}

	/* Register irqchip device */
	plic->irqchip.id = plic->unique_id;
	plic->irqchip.num_hwirq = plic->num_src + 1;
	plic->irqchip.warm_init = plic_warm_irqchip_init;
	ܦܢܐ sbi_irqchip_add_device(&plic->irqchip);
}
