/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Samuel Holland <samuel@sholland.org>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_encoding.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_bitops.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_string.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/irqchip/plic.h>

#define PLIC_PRIORITY_BASE 0x0
#define PLIC_PENDING_BASE 0x1000
#define PLIC_ENABLE_BASE 0x2000
#define PLIC_ENABLE_STRIDE 0x80
#define PLIC_CONTEXT_BASE 0x200000
#define PLIC_CONTEXT_STRIDE 0x1000

#define THEAD_PLIC_CTRL_REG 0x1ffffc

ସ୍ଥିର unsigned long plic_ptr_offset;

#define plic_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ଶୂନ୍ୟ *, plic_ptr_offset)

#define plic_set_hart_data_ptr(__scratch, __plic)			\
	sbi_scratch_write_type((__scratch), ଶୂନ୍ୟ *, plic_ptr_offset, (__plic))

ସଂରଚନା plic_data *plic_get(ଶୂନ୍ୟ)
{
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ଫେରନ୍ତୁ plic_get_hart_data_ptr(scratch);
}

ସ୍ଥିର u32 plic_get_priority(ଅଚଳ ସଂରଚନା plic_data *plic, u32 source)
{
	ଅସ୍ଥିର ଶୂନ୍ୟ *plic_priority = (ଅକ୍ଷର *)plic->addr +
			PLIC_PRIORITY_BASE + 4 * source;
	ଫେରନ୍ତୁ readl(plic_priority);
}

ସ୍ଥିର ଶୂନ୍ୟ plic_set_priority(ଅଚଳ ସଂରଚନା plic_data *plic, u32 source, u32 val)
{
	ଅସ୍ଥିର ଶୂନ୍ୟ *plic_priority = (ଅକ୍ଷର *)plic->addr +
			PLIC_PRIORITY_BASE + 4 * source;
	writel(val, plic_priority);
}

ସ୍ଥିର u32 plic_get_thresh(ଅଚଳ ସଂରଚନା plic_data *plic, u32 cntxid)
{
	ଅସ୍ଥିର ଶୂନ୍ୟ *plic_thresh;

	plic_thresh = (ଅକ୍ଷର *)plic->addr +
		      PLIC_CONTEXT_BASE + PLIC_CONTEXT_STRIDE * cntxid;

	ଫେରନ୍ତୁ readl(plic_thresh);
}

ସ୍ଥିର ଶୂନ୍ୟ plic_set_thresh(ଅଚଳ ସଂରଚନା plic_data *plic, u32 cntxid, u32 val)
{
	ଅସ୍ଥିର ଶୂନ୍ୟ *plic_thresh;

	plic_thresh = (ଅକ୍ଷର *)plic->addr +
		      PLIC_CONTEXT_BASE + PLIC_CONTEXT_STRIDE * cntxid;
	writel(val, plic_thresh);
}

ସ୍ଥିର u32 plic_get_ie(ଅଚଳ ସଂରଚନା plic_data *plic, u32 cntxid,
		       u32 word_index)
{
	ଅସ୍ଥିର ଶୂନ୍ୟ *plic_ie;

	plic_ie = (ଅକ୍ଷର *)plic->addr +
		   PLIC_ENABLE_BASE + PLIC_ENABLE_STRIDE * cntxid +
		   4 * word_index;

	ଫେରନ୍ତୁ readl(plic_ie);
}

ସ୍ଥିର ଶୂନ୍ୟ plic_set_ie(ଅଚଳ ସଂରଚନା plic_data *plic, u32 cntxid,
			u32 word_index, u32 val)
{
	ଅସ୍ଥିର ଶୂନ୍ୟ *plic_ie;

	plic_ie = (ଅକ୍ଷର *)plic->addr +
		   PLIC_ENABLE_BASE + PLIC_ENABLE_STRIDE * cntxid +
		   4 * word_index;
	writel(val, plic_ie);
}

ସ୍ଥିର ଶୂନ୍ୟ plic_delegate(ଅଚଳ ସଂରଚନା plic_data *plic)
{
	/* If this is a T-HEAD PLIC, delegate access to S-mode */
	ଯଦି (plic->flags & PLIC_FLAG_THEAD_DELEGATION)
		writel_relaxed(BIT(0), (ଅକ୍ଷର *)plic->addr + THEAD_PLIC_CTRL_REG);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା plic_context_init(ଅଚଳ ସଂରଚନା plic_data *plic, ପୂର୍ଣ୍ଣସଂଖ୍ୟା context_id,
			     bool enable, u32 threshold)
{
	u32 ie_ଶବ୍ଦଗୁଡ଼ିକ, ie_ମୂଲ୍ୟ;

	ଯଦି (!plic || context_id < 0)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ie_ଶବ୍ଦଗୁଡ଼ିକ = PLIC_IE_ଶବ୍ଦଗୁଡ଼ିକ(plic);
	ie_ମୂଲ୍ୟ = enable ? 0xffffffffU : 0U;

	ପାଇଁ (u32 i = 0; i < ie_ଶବ୍ଦଗୁଡ଼ିକ; i++)
		plic_set_ie(plic, context_id, i, ie_ମୂଲ୍ୟ);

	plic_set_thresh(plic, context_id, threshold);

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ plic_suspend(ଶୂନ୍ୟ)
{
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ଅଚଳ ସଂରଚନା plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 ie_ଶବ୍ଦଗୁଡ଼ିକ = PLIC_IE_ଶବ୍ଦଗୁଡ଼ିକ(plic);
	u32 *data_word = plic->pm_data;
	u8 *data_byte;

	ଯଦି (!data_word)
		ଫେରନ୍ତୁ;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		ଯଦି (context_id < 0)
			ଜାରି;

		/* Save the enable bits */
		ପାଇଁ (u32 i = 0; i < ie_ଶବ୍ଦଗୁଡ଼ିକ; i++)
			*data_word++ = plic_get_ie(plic, context_id, i);

		/* Save the context threshold */
		*data_word++ = plic_get_thresh(plic, context_id);
	}

	/* Restore the input priorities */
	data_byte = (u8 *)data_word;
	ପାଇଁ (u32 i = 1; i <= plic->ଅଙ୍କ_src; i++)
		*data_byte++ = plic_get_priority(plic, i);
}

ଶୂନ୍ୟ plic_resume(ଶୂନ୍ୟ)
{
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ଅଚଳ ସଂରଚନା plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 ie_ଶବ୍ଦଗୁଡ଼ିକ = PLIC_IE_ଶବ୍ଦଗୁଡ଼ିକ(plic);
	u32 *data_word = plic->pm_data;
	u8 *data_byte;

	ଯଦି (!data_word)
		ଫେରନ୍ତୁ;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		ଯଦି (context_id < 0)
			ଜାରି;

		/* Restore the enable bits */
		ପାଇଁ (u32 i = 0; i < ie_ଶବ୍ଦଗୁଡ଼ିକ; i++)
			plic_set_ie(plic, context_id, i, *data_word++);

		/* Restore the context threshold */
		plic_set_thresh(plic, context_id, *data_word++);
	}

	/* Restore the input priorities */
	data_byte = (u8 *)data_word;
	ପାଇଁ (u32 i = 1; i <= plic->ଅଙ୍କ_src; i++)
		plic_set_priority(plic, i, *data_byte++);

	/* Restore the delegation */
	plic_delegate(plic);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା plic_warm_irqchip_init(ସଂରଚନା sbi_irqchip_device *dev)
{
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ଅଚଳ ସଂରଚନା plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 hartindex = current_hartindex();
	s16 m_cntx_id = plic->context_map[hartindex][PLIC_M_CONTEXT];
	s16 s_cntx_id = plic->context_map[hartindex][PLIC_S_CONTEXT];
	bool enable;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

	/*
	 * By default, disable all IRQs for the target HART. Ariane
	 * has a bug which requires enabling all interrupts at boot.
	 */
	enable = plic->flags & PLIC_FLAG_ARIANE_BUG;

	ଯଦି (m_cntx_id > -1) {
		ret = plic_context_init(plic, m_cntx_id, enable, 0x7);
		ଯଦି (ret)
			ଫେରନ୍ତୁ ret;
	}

	ଯଦି (s_cntx_id > -1) {
		ret = plic_context_init(plic, s_cntx_id, enable, 0x7);
		ଯଦି (ret)
			ଫେରନ୍ତୁ ret;
	}

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା plic_cold_irqchip_init(ସଂରଚନା plic_data *plic)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, ret;

	ଯଦି (!plic)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (!plic_ptr_offset) {
		plic_ptr_offset = sbi_scratch_alloc_type_offset(ଶୂନ୍ୟ *);
		ଯଦି (!plic_ptr_offset)
			ଫେରନ୍ତୁ SBI_ENOMEM;
	}

	ଯଦି (plic->flags & PLIC_FLAG_ENABLE_PM) {
		unsigned long data_size = 0;

		sbi_for_each_hartindex(i) {
			ଯଦି (plic->context_map[i][PLIC_S_CONTEXT] < 0)
				ଜାରି;

			/* Allocate space for enable bits */
			data_size += (plic->ଅଙ୍କ_src / 32 + 1) * ଆକାର(u32);

			/* Allocate space for the context threshold */
			data_size += ଆକାର(u32);
		}

		/*
		 * Allocate space for the input priorities. So far,
		 * priorities on all known implementations fit in 8 bits.
		 */
		data_size += plic->ଅଙ୍କ_src * ଆକାର(u8);

		plic->pm_data = sbi_malloc(data_size);
		ଯଦି (!plic->pm_data)
			ଫେରନ୍ତୁ SBI_ENOMEM;
	}

	/* Configure default priorities of all IRQs */
	ପାଇଁ (i = 1; i <= plic->ଅଙ୍କ_src; i++)
		plic_set_priority(plic, i, 0);

	plic_delegate(plic);

	ret = sbi_domain_ମୂଳ_add_memrange(plic->addr, plic->size, BIT(20),
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	ଯଦି (ret)
		ଫେରନ୍ତୁ ret;

	sbi_for_each_hartindex(i) {
		ଯଦି (plic->context_map[i][PLIC_M_CONTEXT] < 0 &&
		    plic->context_map[i][PLIC_S_CONTEXT] < 0)
			ଜାରି;

		plic_set_hart_data_ptr(sbi_hartindex_to_scratch(i), plic);
		sbi_hartmask_set_hartindex(i, &plic->irqchip.target_harts);
	}

	/* Register irqchip device */
	plic->irqchip.id = plic->unique_id;
	plic->irqchip.ଅଙ୍କ_hwirq = plic->ଅଙ୍କ_src + 1;
	plic->irqchip.warm_init = plic_warm_irqchip_init;
	ଫେରନ୍ତୁ sbi_irqchip_add_device(&plic->irqchip);
}
