/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Samuel Holland <samuel@sholland.org>
 */

#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/riscv_encoding.h>
#அந்தர்பாவ <sbi/sbi_bitops.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_domain.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi/sbi_string.h>
#அந்தர்பாவ <sbi_utils/irqchip/plic.h>

#define PLIC_PRIORITY_BASE 0x0
#define PLIC_PENDING_BASE 0x1000
#define PLIC_ENABLE_BASE 0x2000
#define PLIC_ENABLE_STRIDE 0x80
#define PLIC_CONTEXT_BASE 0x200000
#define PLIC_CONTEXT_STRIDE 0x1000

#define THEAD_PLIC_CTRL_REG 0x1ffffc

நிலையான unsigned long plic_ptr_offset;

#define plic_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), வெறுமை *, plic_ptr_offset)

#define plic_set_hart_data_ptr(__scratch, __plic)			\
	sbi_scratch_write_type((__scratch), வெறுமை *, plic_ptr_offset, (__plic))

கட்டமைப்பு plic_data *plic_get(வெறுமை)
{
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	திரும்பு plic_get_hart_data_ptr(scratch);
}

நிலையான u32 plic_get_priority(மாறிலி கட்டமைப்பு plic_data *plic, u32 source)
{
	நிலையற்ற வெறுமை *plic_priority = (எழுத்து *)plic->addr +
			PLIC_PRIORITY_BASE + 4 * source;
	திரும்பு readl(plic_priority);
}

நிலையான வெறுமை plic_set_priority(மாறிலி கட்டமைப்பு plic_data *plic, u32 source, u32 val)
{
	நிலையற்ற வெறுமை *plic_priority = (எழுத்து *)plic->addr +
			PLIC_PRIORITY_BASE + 4 * source;
	writel(val, plic_priority);
}

நிலையான u32 plic_get_thresh(மாறிலி கட்டமைப்பு plic_data *plic, u32 cntxid)
{
	நிலையற்ற வெறுமை *plic_thresh;

	plic_thresh = (எழுத்து *)plic->addr +
		      PLIC_CONTEXT_BASE + PLIC_CONTEXT_STRIDE * cntxid;

	திரும்பு readl(plic_thresh);
}

நிலையான வெறுமை plic_set_thresh(மாறிலி கட்டமைப்பு plic_data *plic, u32 cntxid, u32 val)
{
	நிலையற்ற வெறுமை *plic_thresh;

	plic_thresh = (எழுத்து *)plic->addr +
		      PLIC_CONTEXT_BASE + PLIC_CONTEXT_STRIDE * cntxid;
	writel(val, plic_thresh);
}

நிலையான u32 plic_get_ie(மாறிலி கட்டமைப்பு plic_data *plic, u32 cntxid,
		       u32 word_index)
{
	நிலையற்ற வெறுமை *plic_ie;

	plic_ie = (எழுத்து *)plic->addr +
		   PLIC_ENABLE_BASE + PLIC_ENABLE_STRIDE * cntxid +
		   4 * word_index;

	திரும்பு readl(plic_ie);
}

நிலையான வெறுமை plic_set_ie(மாறிலி கட்டமைப்பு plic_data *plic, u32 cntxid,
			u32 word_index, u32 val)
{
	நிலையற்ற வெறுமை *plic_ie;

	plic_ie = (எழுத்து *)plic->addr +
		   PLIC_ENABLE_BASE + PLIC_ENABLE_STRIDE * cntxid +
		   4 * word_index;
	writel(val, plic_ie);
}

நிலையான வெறுமை plic_delegate(மாறிலி கட்டமைப்பு plic_data *plic)
{
	/* If this is a T-HEAD PLIC, delegate access to S-mode */
	எனில் (plic->flags & PLIC_FLAG_THEAD_DELEGATION)
		writel_relaxed(BIT(0), (எழுத்து *)plic->addr + THEAD_PLIC_CTRL_REG);
}

நிலையான முழு plic_context_init(மாறிலி கட்டமைப்பு plic_data *plic, முழு context_id,
			     bool enable, u32 threshold)
{
	u32 ie_சொற்கள், ie_மதிப்பு;

	எனில் (!plic || context_id < 0)
		திரும்பு SBI_EINVAL;

	ie_சொற்கள் = PLIC_IE_சொற்கள்(plic);
	ie_மதிப்பு = enable ? 0xffffffffU : 0U;

	க்கு (u32 i = 0; i < ie_சொற்கள்; i++)
		plic_set_ie(plic, context_id, i, ie_மதிப்பு);

	plic_set_thresh(plic, context_id, threshold);

	திரும்பு 0;
}

வெறுமை plic_suspend(வெறுமை)
{
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	மாறிலி கட்டமைப்பு plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 ie_சொற்கள் = PLIC_IE_சொற்கள்(plic);
	u32 *data_word = plic->pm_data;
	u8 *data_byte;

	எனில் (!data_word)
		திரும்பு;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		எனில் (context_id < 0)
			தொடர்;

		/* Save the enable bits */
		க்கு (u32 i = 0; i < ie_சொற்கள்; i++)
			*data_word++ = plic_get_ie(plic, context_id, i);

		/* Save the context threshold */
		*data_word++ = plic_get_thresh(plic, context_id);
	}

	/* Restore the input priorities */
	data_byte = (u8 *)data_word;
	க்கு (u32 i = 1; i <= plic->இலக்கம்_src; i++)
		*data_byte++ = plic_get_priority(plic, i);
}

வெறுமை plic_resume(வெறுமை)
{
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	மாறிலி கட்டமைப்பு plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 ie_சொற்கள் = PLIC_IE_சொற்கள்(plic);
	u32 *data_word = plic->pm_data;
	u8 *data_byte;

	எனில் (!data_word)
		திரும்பு;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		எனில் (context_id < 0)
			தொடர்;

		/* Restore the enable bits */
		க்கு (u32 i = 0; i < ie_சொற்கள்; i++)
			plic_set_ie(plic, context_id, i, *data_word++);

		/* Restore the context threshold */
		plic_set_thresh(plic, context_id, *data_word++);
	}

	/* Restore the input priorities */
	data_byte = (u8 *)data_word;
	க்கு (u32 i = 1; i <= plic->இலக்கம்_src; i++)
		plic_set_priority(plic, i, *data_byte++);

	/* Restore the delegation */
	plic_delegate(plic);
}

நிலையான முழு plic_warm_irqchip_init(கட்டமைப்பு sbi_irqchip_device *dev)
{
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	மாறிலி கட்டமைப்பு plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 hartindex = current_hartindex();
	s16 m_cntx_id = plic->context_map[hartindex][PLIC_M_CONTEXT];
	s16 s_cntx_id = plic->context_map[hartindex][PLIC_S_CONTEXT];
	bool enable;
	முழு ret;

	/*
	 * By default, disable all IRQs for the target HART. Ariane
	 * has a bug which requires enabling all interrupts at boot.
	 */
	enable = plic->flags & PLIC_FLAG_ARIANE_BUG;

	எனில் (m_cntx_id > -1) {
		ret = plic_context_init(plic, m_cntx_id, enable, 0x7);
		எனில் (ret)
			திரும்பு ret;
	}

	எனில் (s_cntx_id > -1) {
		ret = plic_context_init(plic, s_cntx_id, enable, 0x7);
		எனில் (ret)
			திரும்பு ret;
	}

	திரும்பு 0;
}

முழு plic_cold_irqchip_init(கட்டமைப்பு plic_data *plic)
{
	முழு i, ret;

	எனில் (!plic)
		திரும்பு SBI_EINVAL;

	எனில் (!plic_ptr_offset) {
		plic_ptr_offset = sbi_scratch_alloc_type_offset(வெறுமை *);
		எனில் (!plic_ptr_offset)
			திரும்பு SBI_ENOMEM;
	}

	எனில் (plic->flags & PLIC_FLAG_ENABLE_PM) {
		unsigned long data_size = 0;

		sbi_for_each_hartindex(i) {
			எனில் (plic->context_map[i][PLIC_S_CONTEXT] < 0)
				தொடர்;

			/* Allocate space for enable bits */
			data_size += (plic->இலக்கம்_src / 32 + 1) * அளவு(u32);

			/* Allocate space for the context threshold */
			data_size += அளவு(u32);
		}

		/*
		 * Allocate space for the input priorities. So far,
		 * priorities on all known implementations fit in 8 bits.
		 */
		data_size += plic->இலக்கம்_src * அளவு(u8);

		plic->pm_data = sbi_malloc(data_size);
		எனில் (!plic->pm_data)
			திரும்பு SBI_ENOMEM;
	}

	/* Configure default priorities of all IRQs */
	க்கு (i = 1; i <= plic->இலக்கம்_src; i++)
		plic_set_priority(plic, i, 0);

	plic_delegate(plic);

	ret = sbi_domain_வேர்_add_memrange(plic->addr, plic->size, BIT(20),
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	எனில் (ret)
		திரும்பு ret;

	sbi_for_each_hartindex(i) {
		எனில் (plic->context_map[i][PLIC_M_CONTEXT] < 0 &&
		    plic->context_map[i][PLIC_S_CONTEXT] < 0)
			தொடர்;

		plic_set_hart_data_ptr(sbi_hartindex_to_scratch(i), plic);
		sbi_hartmask_set_hartindex(i, &plic->irqchip.target_harts);
	}

	/* Register irqchip device */
	plic->irqchip.id = plic->unique_id;
	plic->irqchip.இலக்கம்_hwirq = plic->இலக்கம்_src + 1;
	plic->irqchip.warm_init = plic_warm_irqchip_init;
	திரும்பு sbi_irqchip_add_device(&plic->irqchip);
}
