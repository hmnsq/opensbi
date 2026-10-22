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

#تعریف_کرو PLIC_ترجیح_بنیاد 0x0
#تعریف_کرو PLIC_PENDING_بنیاد 0x1000
#تعریف_کرو PLIC_ENABLE_بنیاد 0x2000
#تعریف_کرو PLIC_ENABLE_STRIDE 0x80
#تعریف_کرو PLIC_CONTEXT_بنیاد 0x200000
#تعریف_کرو PLIC_CONTEXT_STRIDE 0x1000

#تعریف_کرو THEAD_PLIC_CTRL_REG 0x1ffffc

ساکن غیرعلامتی طویل plic_ptr_انحراف;

#تعریف_کرو plic_get_hart_ڈیٹا_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), خالی *, plic_ptr_انحراف)

#تعریف_کرو plic_سیٹ_hart_ڈیٹا_ptr(__scratch, __plic)			\
	sbi_scratch_write_type((__scratch), خالی *, plic_ptr_انحراف, (__plic))

ساخت plic_ڈیٹا *plic_get(خالی)
{
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	واپس plic_get_hart_ڈیٹا_ptr(scratch);
}

ساکن u32 plic_get_ترجیح(ثابت ساخت plic_ڈیٹا *plic, u32 ماخذ)
{
	متلون خالی *plic_ترجیح = (حرف *)plic->addr +
			PLIC_ترجیح_بنیاد + 4 * ماخذ;
	واپس readl(plic_ترجیح);
}

ساکن خالی plic_سیٹ_ترجیح(ثابت ساخت plic_ڈیٹا *plic, u32 ماخذ, u32 val)
{
	متلون خالی *plic_ترجیح = (حرف *)plic->addr +
			PLIC_ترجیح_بنیاد + 4 * ماخذ;
	writel(val, plic_ترجیح);
}

ساکن u32 plic_get_thresh(ثابت ساخت plic_ڈیٹا *plic, u32 cntxid)
{
	متلون خالی *plic_thresh;

	plic_thresh = (حرف *)plic->addr +
		      PLIC_CONTEXT_بنیاد + PLIC_CONTEXT_STRIDE * cntxid;

	واپس readl(plic_thresh);
}

ساکن خالی plic_سیٹ_thresh(ثابت ساخت plic_ڈیٹا *plic, u32 cntxid, u32 val)
{
	متلون خالی *plic_thresh;

	plic_thresh = (حرف *)plic->addr +
		      PLIC_CONTEXT_بنیاد + PLIC_CONTEXT_STRIDE * cntxid;
	writel(val, plic_thresh);
}

ساکن u32 plic_get_ie(ثابت ساخت plic_ڈیٹا *plic, u32 cntxid,
		       u32 لفظ_اشاریہ)
{
	متلون خالی *plic_ie;

	plic_ie = (حرف *)plic->addr +
		   PLIC_ENABLE_بنیاد + PLIC_ENABLE_STRIDE * cntxid +
		   4 * لفظ_اشاریہ;

	واپس readl(plic_ie);
}

ساکن خالی plic_سیٹ_ie(ثابت ساخت plic_ڈیٹا *plic, u32 cntxid,
			u32 لفظ_اشاریہ, u32 val)
{
	متلون خالی *plic_ie;

	plic_ie = (حرف *)plic->addr +
		   PLIC_ENABLE_بنیاد + PLIC_ENABLE_STRIDE * cntxid +
		   4 * لفظ_اشاریہ;
	writel(val, plic_ie);
}

ساکن خالی plic_delegate(ثابت ساخت plic_ڈیٹا *plic)
{
	/* If this is a T-HEAD PLIC, delegate access to S-mode */
	اگر (plic->flags & PLIC_FLAG_THEAD_DELEGATION)
		writel_relaxed(ثنائی_ہندسہ(0), (حرف *)plic->addr + THEAD_PLIC_CTRL_REG);
}

ساکن صحیح plic_context_init(ثابت ساخت plic_ڈیٹا *plic, صحیح context_id,
			     bool enable, u32 حد)
{
	u32 ie_الفاظ, ie_قدر;

	اگر (!plic || context_id < 0)
		واپس SBI_EINVAL;

	ie_الفاظ = PLIC_IE_الفاظ(plic);
	ie_قدر = enable ? 0xffffffffU : 0U;

	برائے (u32 i = 0; i < ie_الفاظ; i++)
		plic_سیٹ_ie(plic, context_id, i, ie_قدر);

	plic_سیٹ_thresh(plic, context_id, حد);

	واپس 0;
}

خالی plic_suspend(خالی)
{
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ثابت ساخت plic_ڈیٹا *plic = plic_get_hart_ڈیٹا_ptr(scratch);
	u32 ie_الفاظ = PLIC_IE_الفاظ(plic);
	u32 *ڈیٹا_لفظ = plic->pm_ڈیٹا;
	u8 *ڈیٹا_byte;

	اگر (!ڈیٹا_لفظ)
		واپس;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		اگر (context_id < 0)
			جاری;

		/* Save the enable bits */
		برائے (u32 i = 0; i < ie_الفاظ; i++)
			*ڈیٹا_لفظ++ = plic_get_ie(plic, context_id, i);

		/* Save the context threshold */
		*ڈیٹا_لفظ++ = plic_get_thresh(plic, context_id);
	}

	/* Restore the input priorities */
	ڈیٹا_byte = (u8 *)ڈیٹا_لفظ;
	برائے (u32 i = 1; i <= plic->عدد_src; i++)
		*ڈیٹا_byte++ = plic_get_ترجیح(plic, i);
}

خالی plic_resume(خالی)
{
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ثابت ساخت plic_ڈیٹا *plic = plic_get_hart_ڈیٹا_ptr(scratch);
	u32 ie_الفاظ = PLIC_IE_الفاظ(plic);
	u32 *ڈیٹا_لفظ = plic->pm_ڈیٹا;
	u8 *ڈیٹا_byte;

	اگر (!ڈیٹا_لفظ)
		واپس;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		اگر (context_id < 0)
			جاری;

		/* Restore the enable bits */
		برائے (u32 i = 0; i < ie_الفاظ; i++)
			plic_سیٹ_ie(plic, context_id, i, *ڈیٹا_لفظ++);

		/* Restore the context threshold */
		plic_سیٹ_thresh(plic, context_id, *ڈیٹا_لفظ++);
	}

	/* Restore the input priorities */
	ڈیٹا_byte = (u8 *)ڈیٹا_لفظ;
	برائے (u32 i = 1; i <= plic->عدد_src; i++)
		plic_سیٹ_ترجیح(plic, i, *ڈیٹا_byte++);

	/* Restore the delegation */
	plic_delegate(plic);
}

ساکن صحیح plic_warm_irqchip_init(ساخت sbi_irqchip_device *dev)
{
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ثابت ساخت plic_ڈیٹا *plic = plic_get_hart_ڈیٹا_ptr(scratch);
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
			واپس ret;
	}

	اگر (s_cntx_id > -1) {
		ret = plic_context_init(plic, s_cntx_id, enable, 0x7);
		اگر (ret)
			واپس ret;
	}

	واپس 0;
}

صحیح plic_cold_irqchip_init(ساخت plic_ڈیٹا *plic)
{
	صحیح i, ret;

	اگر (!plic)
		واپس SBI_EINVAL;

	اگر (!plic_ptr_انحراف) {
		plic_ptr_انحراف = sbi_scratch_alloc_type_انحراف(خالی *);
		اگر (!plic_ptr_انحراف)
			واپس SBI_ENOMEM;
	}

	اگر (plic->flags & PLIC_FLAG_ENABLE_PM) {
		غیرعلامتی طویل ڈیٹا_سائز_2 = 0;

		sbi_for_each_hartindex(i) {
			اگر (plic->context_map[i][PLIC_S_CONTEXT] < 0)
				جاری;

			/* Allocate space for enable bits */
			ڈیٹا_سائز_2 += (plic->عدد_src / 32 + 1) * حجم(u32);

			/* Allocate space for the context threshold */
			ڈیٹا_سائز_2 += حجم(u32);
		}

		/*
		 * Allocate space for the input priorities. So far,
		 * priorities on all known implementations fit in 8 bits.
		 */
		ڈیٹا_سائز_2 += plic->عدد_src * حجم(u8);

		plic->pm_ڈیٹا = sbi_malloc(ڈیٹا_سائز_2);
		اگر (!plic->pm_ڈیٹا)
			واپس SBI_ENOMEM;
	}

	/* Configure default priorities of all IRQs */
	برائے (i = 1; i <= plic->عدد_src; i++)
		plic_سیٹ_ترجیح(plic, i, 0);

	plic_delegate(plic);

	ret = sbi_domain_جڑ_جوڑو_memrange(plic->addr, plic->سائز, ثنائی_ہندسہ(20),
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	اگر (ret)
		واپس ret;

	sbi_for_each_hartindex(i) {
		اگر (plic->context_map[i][PLIC_M_CONTEXT] < 0 &&
		    plic->context_map[i][PLIC_S_CONTEXT] < 0)
			جاری;

		plic_سیٹ_hart_ڈیٹا_ptr(sbi_hartindex_مقصد_scratch(i), plic);
		sbi_hartmask_سیٹ_hartindex(i, &plic->irqchip.ہدف_harts);
	}

	/* Register irqchip device */
	plic->irqchip.id = plic->unique_id;
	plic->irqchip.عدد_hwirq = plic->عدد_src + 1;
	plic->irqchip.warm_init = plic_warm_irqchip_init;
	واپس sbi_irqchip_جوڑو_device(&plic->irqchip);
}
