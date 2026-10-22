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

#تعریف_کن PLIC_اولویت_پایه 0x0
#تعریف_کن PLIC_PENDING_پایه 0x1000
#تعریف_کن PLIC_ENABLE_پایه 0x2000
#تعریف_کن PLIC_ENABLE_STRIDE 0x80
#تعریف_کن PLIC_CONTEXT_پایه 0x200000
#تعریف_کن PLIC_CONTEXT_STRIDE 0x1000

#تعریف_کن THEAD_PLIC_CTRL_REG 0x1ffffc

ساكن بیعلامت دراز plic_ptr_جابه_جایی;

#تعریف_کن plic_get_hart_داده_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), فراغ *, plic_ptr_جابه_جایی)

#تعریف_کن plic_مجموعه_hart_داده_ptr(__scratch, __plic)			\
	sbi_scratch_write_type((__scratch), فراغ *, plic_ptr_جابه_جایی, (__plic))

بنية plic_داده *plic_get(فراغ)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	أرجع plic_get_hart_داده_ptr(scratch);
}

ساكن u32 plic_get_اولویت(ثابت بنية plic_داده *plic, u32 منبع)
{
	متطاير فراغ *plic_اولویت = (حرف *)plic->addr +
			PLIC_اولویت_پایه + 4 * منبع;
	أرجع readl(plic_اولویت);
}

ساكن فراغ plic_مجموعه_اولویت(ثابت بنية plic_داده *plic, u32 منبع, u32 val)
{
	متطاير فراغ *plic_اولویت = (حرف *)plic->addr +
			PLIC_اولویت_پایه + 4 * منبع;
	writel(val, plic_اولویت);
}

ساكن u32 plic_get_thresh(ثابت بنية plic_داده *plic, u32 cntxid)
{
	متطاير فراغ *plic_thresh;

	plic_thresh = (حرف *)plic->addr +
		      PLIC_CONTEXT_پایه + PLIC_CONTEXT_STRIDE * cntxid;

	أرجع readl(plic_thresh);
}

ساكن فراغ plic_مجموعه_thresh(ثابت بنية plic_داده *plic, u32 cntxid, u32 val)
{
	متطاير فراغ *plic_thresh;

	plic_thresh = (حرف *)plic->addr +
		      PLIC_CONTEXT_پایه + PLIC_CONTEXT_STRIDE * cntxid;
	writel(val, plic_thresh);
}

ساكن u32 plic_get_ie(ثابت بنية plic_داده *plic, u32 cntxid,
		       u32 کلمه_شاخص)
{
	متطاير فراغ *plic_ie;

	plic_ie = (حرف *)plic->addr +
		   PLIC_ENABLE_پایه + PLIC_ENABLE_STRIDE * cntxid +
		   4 * کلمه_شاخص;

	أرجع readl(plic_ie);
}

ساكن فراغ plic_مجموعه_ie(ثابت بنية plic_داده *plic, u32 cntxid,
			u32 کلمه_شاخص, u32 val)
{
	متطاير فراغ *plic_ie;

	plic_ie = (حرف *)plic->addr +
		   PLIC_ENABLE_پایه + PLIC_ENABLE_STRIDE * cntxid +
		   4 * کلمه_شاخص;
	writel(val, plic_ie);
}

ساكن فراغ plic_delegate(ثابت بنية plic_داده *plic)
{
	/* If this is a T-HEAD PLIC, delegate access to S-mode */
	إذا (plic->flags & PLIC_FLAG_THEAD_DELEGATION)
		writel_relaxed(بیت(0), (حرف *)plic->addr + THEAD_PLIC_CTRL_REG);
}

ساكن صحيح plic_context_init(ثابت بنية plic_داده *plic, صحيح context_id,
			     bool enable, u32 آستانه)
{
	u32 ie_واژگان, ie_مقدار;

	إذا (!plic || context_id < 0)
		أرجع SBI_EINVAL;

	ie_واژگان = PLIC_IE_واژگان(plic);
	ie_مقدار = enable ? 0xffffffffU : 0U;

	لكل (u32 i = 0; i < ie_واژگان; i++)
		plic_مجموعه_ie(plic, context_id, i, ie_مقدار);

	plic_مجموعه_thresh(plic, context_id, آستانه);

	أرجع 0;
}

فراغ plic_suspend(فراغ)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ثابت بنية plic_داده *plic = plic_get_hart_داده_ptr(scratch);
	u32 ie_واژگان = PLIC_IE_واژگان(plic);
	u32 *داده_کلمه = plic->pm_داده;
	u8 *داده_byte;

	إذا (!داده_کلمه)
		أرجع;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		إذا (context_id < 0)
			استمر;

		/* Save the enable bits */
		لكل (u32 i = 0; i < ie_واژگان; i++)
			*داده_کلمه++ = plic_get_ie(plic, context_id, i);

		/* Save the context threshold */
		*داده_کلمه++ = plic_get_thresh(plic, context_id);
	}

	/* Restore the input priorities */
	داده_byte = (u8 *)داده_کلمه;
	لكل (u32 i = 1; i <= plic->رقم_src; i++)
		*داده_byte++ = plic_get_اولویت(plic, i);
}

فراغ plic_resume(فراغ)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ثابت بنية plic_داده *plic = plic_get_hart_داده_ptr(scratch);
	u32 ie_واژگان = PLIC_IE_واژگان(plic);
	u32 *داده_کلمه = plic->pm_داده;
	u8 *داده_byte;

	إذا (!داده_کلمه)
		أرجع;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		إذا (context_id < 0)
			استمر;

		/* Restore the enable bits */
		لكل (u32 i = 0; i < ie_واژگان; i++)
			plic_مجموعه_ie(plic, context_id, i, *داده_کلمه++);

		/* Restore the context threshold */
		plic_مجموعه_thresh(plic, context_id, *داده_کلمه++);
	}

	/* Restore the input priorities */
	داده_byte = (u8 *)داده_کلمه;
	لكل (u32 i = 1; i <= plic->رقم_src; i++)
		plic_مجموعه_اولویت(plic, i, *داده_byte++);

	/* Restore the delegation */
	plic_delegate(plic);
}

ساكن صحيح plic_warm_irqchip_init(بنية sbi_irqchip_device *dev)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ثابت بنية plic_داده *plic = plic_get_hart_داده_ptr(scratch);
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

صحيح plic_cold_irqchip_init(بنية plic_داده *plic)
{
	صحيح i, ret;

	إذا (!plic)
		أرجع SBI_EINVAL;

	إذا (!plic_ptr_جابه_جایی) {
		plic_ptr_جابه_جایی = sbi_scratch_alloc_type_جابه_جایی(فراغ *);
		إذا (!plic_ptr_جابه_جایی)
			أرجع SBI_ENOMEM;
	}

	إذا (plic->flags & PLIC_FLAG_ENABLE_PM) {
		بیعلامت دراز داده_اندازه_2 = 0;

		sbi_for_each_hartindex(i) {
			إذا (plic->context_map[i][PLIC_S_CONTEXT] < 0)
				استمر;

			/* Allocate space for enable bits */
			داده_اندازه_2 += (plic->رقم_src / 32 + 1) * حجم(u32);

			/* Allocate space for the context threshold */
			داده_اندازه_2 += حجم(u32);
		}

		/*
		 * Allocate space for the input priorities. So far,
		 * priorities on all known implementations fit in 8 bits.
		 */
		داده_اندازه_2 += plic->رقم_src * حجم(u8);

		plic->pm_داده = sbi_malloc(داده_اندازه_2);
		إذا (!plic->pm_داده)
			أرجع SBI_ENOMEM;
	}

	/* Configure default priorities of all IRQs */
	لكل (i = 1; i <= plic->رقم_src; i++)
		plic_مجموعه_اولویت(plic, i, 0);

	plic_delegate(plic);

	ret = sbi_domain_ریشه_جمع_کن_memrange(plic->addr, plic->اندازه, بیت(20),
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	إذا (ret)
		أرجع ret;

	sbi_for_each_hartindex(i) {
		إذا (plic->context_map[i][PLIC_M_CONTEXT] < 0 &&
		    plic->context_map[i][PLIC_S_CONTEXT] < 0)
			استمر;

		plic_مجموعه_hart_داده_ptr(sbi_hartindex_مقصد_scratch(i), plic);
		sbi_hartmask_مجموعه_hartindex(i, &plic->irqchip.هدف_harts);
	}

	/* Register irqchip device */
	plic->irqchip.id = plic->unique_id;
	plic->irqchip.رقم_hwirq = plic->رقم_src + 1;
	plic->irqchip.warm_init = plic_warm_irqchip_init;
	أرجع sbi_irqchip_جمع_کن_device(&plic->irqchip);
}
