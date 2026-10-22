/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Samuel Holland <samuel@sholland.org>
 */

#شامل <sbi/riscv_io.h>
#شامل <sbi/riscv_encoding.h>
#شامل <sbi/sbi_bitops.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_string.h>
#شامل <sbi_utils/irqchip/plic.h>

#define PLIC_PRIORITY_BASE 0x0
#define PLIC_PENDING_BASE 0x1000
#define PLIC_ENABLE_BASE 0x2000
#define PLIC_ENABLE_STRIDE 0x80
#define PLIC_CONTEXT_BASE 0x200000
#define PLIC_CONTEXT_STRIDE 0x1000

#define THEAD_PLIC_CTRL_REG 0x1ffffc

ایستا unsigned long plic_ptr_offset;

#define plic_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), تهی *, plic_ptr_offset)

#define plic_set_hart_data_ptr(__scratch, __plic)			\
	sbi_scratch_write_type((__scratch), تهی *, plic_ptr_offset, (__plic))

ساختار plic_data *plic_get(تهی)
{
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	بازگشت plic_get_hart_data_ptr(scratch);
}

ایستا u32 plic_get_priority(ثابت ساختار plic_data *plic, u32 source)
{
	فرار تهی *plic_priority = (نویسه *)plic->addr +
			PLIC_PRIORITY_BASE + 4 * source;
	بازگشت readl(plic_priority);
}

ایستا تهی plic_set_priority(ثابت ساختار plic_data *plic, u32 source, u32 val)
{
	فرار تهی *plic_priority = (نویسه *)plic->addr +
			PLIC_PRIORITY_BASE + 4 * source;
	writel(val, plic_priority);
}

ایستا u32 plic_get_thresh(ثابت ساختار plic_data *plic, u32 cntxid)
{
	فرار تهی *plic_thresh;

	plic_thresh = (نویسه *)plic->addr +
		      PLIC_CONTEXT_BASE + PLIC_CONTEXT_STRIDE * cntxid;

	بازگشت readl(plic_thresh);
}

ایستا تهی plic_set_thresh(ثابت ساختار plic_data *plic, u32 cntxid, u32 val)
{
	فرار تهی *plic_thresh;

	plic_thresh = (نویسه *)plic->addr +
		      PLIC_CONTEXT_BASE + PLIC_CONTEXT_STRIDE * cntxid;
	writel(val, plic_thresh);
}

ایستا u32 plic_get_ie(ثابت ساختار plic_data *plic, u32 cntxid,
		       u32 word_index)
{
	فرار تهی *plic_ie;

	plic_ie = (نویسه *)plic->addr +
		   PLIC_ENABLE_BASE + PLIC_ENABLE_STRIDE * cntxid +
		   4 * word_index;

	بازگشت readl(plic_ie);
}

ایستا تهی plic_set_ie(ثابت ساختار plic_data *plic, u32 cntxid,
			u32 word_index, u32 val)
{
	فرار تهی *plic_ie;

	plic_ie = (نویسه *)plic->addr +
		   PLIC_ENABLE_BASE + PLIC_ENABLE_STRIDE * cntxid +
		   4 * word_index;
	writel(val, plic_ie);
}

ایستا تهی plic_delegate(ثابت ساختار plic_data *plic)
{
	/* If this is a T-HEAD PLIC, delegate access to S-mode */
	اگر (plic->flags & PLIC_FLAG_THEAD_DELEGATION)
		writel_relaxed(BIT(0), (نویسه *)plic->addr + THEAD_PLIC_CTRL_REG);
}

ایستا صحیح plic_context_init(ثابت ساختار plic_data *plic, صحیح context_id,
			     bool enable, u32 threshold)
{
	u32 ie_واژگان, ie_مقدار;

	اگر (!plic || context_id < 0)
		بازگشت SBI_EINVAL;

	ie_واژگان = PLIC_IE_واژگان(plic);
	ie_مقدار = enable ? 0xffffffffU : 0U;

	برای (u32 i = 0; i < ie_واژگان; i++)
		plic_set_ie(plic, context_id, i, ie_مقدار);

	plic_set_thresh(plic, context_id, threshold);

	بازگشت 0;
}

تهی plic_suspend(تهی)
{
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ثابت ساختار plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 ie_واژگان = PLIC_IE_واژگان(plic);
	u32 *data_word = plic->pm_data;
	u8 *data_byte;

	اگر (!data_word)
		بازگشت;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		اگر (context_id < 0)
			ادامه;

		/* Save the enable bits */
		برای (u32 i = 0; i < ie_واژگان; i++)
			*data_word++ = plic_get_ie(plic, context_id, i);

		/* Save the context threshold */
		*data_word++ = plic_get_thresh(plic, context_id);
	}

	/* Restore the input priorities */
	data_byte = (u8 *)data_word;
	برای (u32 i = 1; i <= plic->رقم_src; i++)
		*data_byte++ = plic_get_priority(plic, i);
}

تهی plic_resume(تهی)
{
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ثابت ساختار plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 ie_واژگان = PLIC_IE_واژگان(plic);
	u32 *data_word = plic->pm_data;
	u8 *data_byte;

	اگر (!data_word)
		بازگشت;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		اگر (context_id < 0)
			ادامه;

		/* Restore the enable bits */
		برای (u32 i = 0; i < ie_واژگان; i++)
			plic_set_ie(plic, context_id, i, *data_word++);

		/* Restore the context threshold */
		plic_set_thresh(plic, context_id, *data_word++);
	}

	/* Restore the input priorities */
	data_byte = (u8 *)data_word;
	برای (u32 i = 1; i <= plic->رقم_src; i++)
		plic_set_priority(plic, i, *data_byte++);

	/* Restore the delegation */
	plic_delegate(plic);
}

ایستا صحیح plic_warm_irqchip_init(ساختار sbi_irqchip_device *dev)
{
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ثابت ساختار plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 hartindex = current_hartindex();
	s16 m_cntx_id = plic->context_map[hartindex][PLIC_M_CONTEXT];
	s16 s_cntx_id = plic->context_map[hartindex][PLIC_S_CONTEXT];
	bool enable;
	صحیح ret;

	/*
	 * By default, disable all IRQs for the target HART. Ariane
	 * has a bug which requires enabling all interrupts at boot.
	 */
	enable = plic->flags & PLIC_FLAG_ARIANE_BUG;

	اگر (m_cntx_id > -1) {
		ret = plic_context_init(plic, m_cntx_id, enable, 0x7);
		اگر (ret)
			بازگشت ret;
	}

	اگر (s_cntx_id > -1) {
		ret = plic_context_init(plic, s_cntx_id, enable, 0x7);
		اگر (ret)
			بازگشت ret;
	}

	بازگشت 0;
}

صحیح plic_cold_irqchip_init(ساختار plic_data *plic)
{
	صحیح i, ret;

	اگر (!plic)
		بازگشت SBI_EINVAL;

	اگر (!plic_ptr_offset) {
		plic_ptr_offset = sbi_scratch_alloc_type_offset(تهی *);
		اگر (!plic_ptr_offset)
			بازگشت SBI_ENOMEM;
	}

	اگر (plic->flags & PLIC_FLAG_ENABLE_PM) {
		unsigned long data_size = 0;

		sbi_for_each_hartindex(i) {
			اگر (plic->context_map[i][PLIC_S_CONTEXT] < 0)
				ادامه;

			/* Allocate space for enable bits */
			data_size += (plic->رقم_src / 32 + 1) * اندازه(u32);

			/* Allocate space for the context threshold */
			data_size += اندازه(u32);
		}

		/*
		 * Allocate space for the input priorities. So far,
		 * priorities on all known implementations fit in 8 bits.
		 */
		data_size += plic->رقم_src * اندازه(u8);

		plic->pm_data = sbi_malloc(data_size);
		اگر (!plic->pm_data)
			بازگشت SBI_ENOMEM;
	}

	/* Configure default priorities of all IRQs */
	برای (i = 1; i <= plic->رقم_src; i++)
		plic_set_priority(plic, i, 0);

	plic_delegate(plic);

	ret = sbi_domain_ریشه_add_memrange(plic->addr, plic->size, BIT(20),
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	اگر (ret)
		بازگشت ret;

	sbi_for_each_hartindex(i) {
		اگر (plic->context_map[i][PLIC_M_CONTEXT] < 0 &&
		    plic->context_map[i][PLIC_S_CONTEXT] < 0)
			ادامه;

		plic_set_hart_data_ptr(sbi_hartindex_to_scratch(i), plic);
		sbi_hartmask_set_hartindex(i, &plic->irqchip.target_harts);
	}

	/* Register irqchip device */
	plic->irqchip.id = plic->unique_id;
	plic->irqchip.رقم_hwirq = plic->رقم_src + 1;
	plic->irqchip.warm_init = plic_warm_irqchip_init;
	بازگشت sbi_irqchip_add_device(&plic->irqchip);
}
