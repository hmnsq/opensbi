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

#تعریف_کړه PLIC_لومړیتوب_بنسټ 0x0
#تعریف_کړه PLIC_PENDING_بنسټ 0x1000
#تعریف_کړه PLIC_ENABLE_بنسټ 0x2000
#تعریف_کړه PLIC_ENABLE_STRIDE 0x80
#تعریف_کړه PLIC_CONTEXT_بنسټ 0x200000
#تعریف_کړه PLIC_CONTEXT_STRIDE 0x1000

#تعریف_کړه THEAD_PLIC_CTRL_REG 0x1ffffc

ساكن بینښه اوږد plic_ptr_دننه_واټن;

#تعریف_کړه plic_get_hart_معلومات_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), فراغ *, plic_ptr_دننه_واټن)

#تعریف_کړه plic_سېټ_hart_معلومات_ptr(__scratch, __plic)			\
	sbi_scratch_write_type((__scratch), فراغ *, plic_ptr_دننه_واټن, (__plic))

بنية plic_معلومات *plic_get(فراغ)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	أرجع plic_get_hart_معلومات_ptr(scratch);
}

ساكن u32 plic_get_لومړیتوب(ثابت بنية plic_معلومات *plic, u32 سرچینه)
{
	متطاير فراغ *plic_لومړیتوب = (حرف *)plic->addr +
			PLIC_لومړیتوب_بنسټ + 4 * سرچینه;
	أرجع readl(plic_لومړیتوب);
}

ساكن فراغ plic_سېټ_لومړیتوب(ثابت بنية plic_معلومات *plic, u32 سرچینه, u32 val)
{
	متطاير فراغ *plic_لومړیتوب = (حرف *)plic->addr +
			PLIC_لومړیتوب_بنسټ + 4 * سرچینه;
	writel(val, plic_لومړیتوب);
}

ساكن u32 plic_get_thresh(ثابت بنية plic_معلومات *plic, u32 cntxid)
{
	متطاير فراغ *plic_thresh;

	plic_thresh = (حرف *)plic->addr +
		      PLIC_CONTEXT_بنسټ + PLIC_CONTEXT_STRIDE * cntxid;

	أرجع readl(plic_thresh);
}

ساكن فراغ plic_سېټ_thresh(ثابت بنية plic_معلومات *plic, u32 cntxid, u32 val)
{
	متطاير فراغ *plic_thresh;

	plic_thresh = (حرف *)plic->addr +
		      PLIC_CONTEXT_بنسټ + PLIC_CONTEXT_STRIDE * cntxid;
	writel(val, plic_thresh);
}

ساكن u32 plic_get_ie(ثابت بنية plic_معلومات *plic, u32 cntxid,
		       u32 کلمه_شاخص)
{
	متطاير فراغ *plic_ie;

	plic_ie = (حرف *)plic->addr +
		   PLIC_ENABLE_بنسټ + PLIC_ENABLE_STRIDE * cntxid +
		   4 * کلمه_شاخص;

	أرجع readl(plic_ie);
}

ساكن فراغ plic_سېټ_ie(ثابت بنية plic_معلومات *plic, u32 cntxid,
			u32 کلمه_شاخص, u32 val)
{
	متطاير فراغ *plic_ie;

	plic_ie = (حرف *)plic->addr +
		   PLIC_ENABLE_بنسټ + PLIC_ENABLE_STRIDE * cntxid +
		   4 * کلمه_شاخص;
	writel(val, plic_ie);
}

ساكن فراغ plic_delegate(ثابت بنية plic_معلومات *plic)
{
	/* If this is a T-HEAD PLIC, delegate access to S-mode */
	إذا (plic->flags & PLIC_FLAG_THEAD_DELEGATION)
		writel_relaxed(بټ(0), (حرف *)plic->addr + THEAD_PLIC_CTRL_REG);
}

ساكن صحيح plic_context_init(ثابت بنية plic_معلومات *plic, صحيح context_id,
			     bool enable, u32 پوله)
{
	u32 ie_ټکي, ie_ارزښت;

	إذا (!plic || context_id < 0)
		أرجع SBI_EINVAL;

	ie_ټکي = PLIC_IE_ټکي(plic);
	ie_ارزښت = enable ? 0xffffffffU : 0U;

	لكل (u32 i = 0; i < ie_ټکي; i++)
		plic_سېټ_ie(plic, context_id, i, ie_ارزښت);

	plic_سېټ_thresh(plic, context_id, پوله);

	أرجع 0;
}

فراغ plic_suspend(فراغ)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ثابت بنية plic_معلومات *plic = plic_get_hart_معلومات_ptr(scratch);
	u32 ie_ټکي = PLIC_IE_ټکي(plic);
	u32 *معلومات_کلمه = plic->pm_معلومات;
	u8 *معلومات_byte;

	إذا (!معلومات_کلمه)
		أرجع;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		إذا (context_id < 0)
			استمر;

		/* Save the enable bits */
		لكل (u32 i = 0; i < ie_ټکي; i++)
			*معلومات_کلمه++ = plic_get_ie(plic, context_id, i);

		/* Save the context threshold */
		*معلومات_کلمه++ = plic_get_thresh(plic, context_id);
	}

	/* Restore the input priorities */
	معلومات_byte = (u8 *)معلومات_کلمه;
	لكل (u32 i = 1; i <= plic->عدد_src; i++)
		*معلومات_byte++ = plic_get_لومړیتوب(plic, i);
}

فراغ plic_resume(فراغ)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ثابت بنية plic_معلومات *plic = plic_get_hart_معلومات_ptr(scratch);
	u32 ie_ټکي = PLIC_IE_ټکي(plic);
	u32 *معلومات_کلمه = plic->pm_معلومات;
	u8 *معلومات_byte;

	إذا (!معلومات_کلمه)
		أرجع;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		إذا (context_id < 0)
			استمر;

		/* Restore the enable bits */
		لكل (u32 i = 0; i < ie_ټکي; i++)
			plic_سېټ_ie(plic, context_id, i, *معلومات_کلمه++);

		/* Restore the context threshold */
		plic_سېټ_thresh(plic, context_id, *معلومات_کلمه++);
	}

	/* Restore the input priorities */
	معلومات_byte = (u8 *)معلومات_کلمه;
	لكل (u32 i = 1; i <= plic->عدد_src; i++)
		plic_سېټ_لومړیتوب(plic, i, *معلومات_byte++);

	/* Restore the delegation */
	plic_delegate(plic);
}

ساكن صحيح plic_warm_irqchip_init(بنية sbi_irqchip_device *dev)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ثابت بنية plic_معلومات *plic = plic_get_hart_معلومات_ptr(scratch);
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

	إذا (m_cntx_id > -1) {
		ret = plic_context_init(plic, m_cntx_id, enable, 0x7);
		إذا (ret)
			أرجع ret;
	}

	إذا (s_cntx_id > -1) {
		ret = plic_context_init(plic, s_cntx_id, enable, 0x7);
		إذا (ret)
			أرجع ret;
	}

	أرجع 0;
}

صحيح plic_cold_irqchip_init(بنية plic_معلومات *plic)
{
	صحيح i, ret;

	إذا (!plic)
		أرجع SBI_EINVAL;

	إذا (!plic_ptr_دننه_واټن) {
		plic_ptr_دننه_واټن = sbi_scratch_alloc_type_دننه_واټن(فراغ *);
		إذا (!plic_ptr_دننه_واټن)
			أرجع SBI_ENOMEM;
	}

	إذا (plic->flags & PLIC_FLAG_ENABLE_PM) {
		بینښه اوږد معلومات_اندازه_2 = 0;

		sbi_for_each_hartindex(i) {
			إذا (plic->context_map[i][PLIC_S_CONTEXT] < 0)
				استمر;

			/* Allocate space for enable bits */
			معلومات_اندازه_2 += (plic->عدد_src / 32 + 1) * حجم(u32);

			/* Allocate space for the context threshold */
			معلومات_اندازه_2 += حجم(u32);
		}

		/*
		 * Allocate space for the input priorities. So far,
		 * priorities on all known implementations fit in 8 bits.
		 */
		معلومات_اندازه_2 += plic->عدد_src * حجم(u8);

		plic->pm_معلومات = sbi_malloc(معلومات_اندازه_2);
		إذا (!plic->pm_معلومات)
			أرجع SBI_ENOMEM;
	}

	/* Configure default priorities of all IRQs */
	لكل (i = 1; i <= plic->عدد_src; i++)
		plic_سېټ_لومړیتوب(plic, i, 0);

	plic_delegate(plic);

	ret = sbi_domain_ریښه_جمع_کړه_memrange(plic->addr, plic->اندازه, بټ(20),
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	إذا (ret)
		أرجع ret;

	sbi_for_each_hartindex(i) {
		إذا (plic->context_map[i][PLIC_M_CONTEXT] < 0 &&
		    plic->context_map[i][PLIC_S_CONTEXT] < 0)
			استمر;

		plic_سېټ_hart_معلومات_ptr(sbi_hartindex_منزل_scratch(i), plic);
		sbi_hartmask_سېټ_hartindex(i, &plic->irqchip.موخه_harts);
	}

	/* Register irqchip device */
	plic->irqchip.id = plic->unique_id;
	plic->irqchip.عدد_hwirq = plic->عدد_src + 1;
	plic->irqchip.warm_init = plic_warm_irqchip_init;
	أرجع sbi_irqchip_جمع_کړه_device(&plic->irqchip);
}
