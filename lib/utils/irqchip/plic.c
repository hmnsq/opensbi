/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Samuel Holland <samuel@sholland.org>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_encoding.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitops.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/irqchip/plic.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLIC_ਤਰਜੀਹ_ਅਧਾਰ 0x0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLIC_PENDING_ਅਧਾਰ 0x1000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLIC_ENABLE_ਅਧਾਰ 0x2000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLIC_ENABLE_STRIDE 0x80
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLIC_CONTEXT_ਅਧਾਰ 0x200000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLIC_CONTEXT_STRIDE 0x1000

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ THEAD_PLIC_CTRL_REG 0x1ffffc

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ plic_ptr_ਵਿੱਥ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ plic_get_hart_ਅੰਕੜੇ_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ਖਾਲੀ *, plic_ptr_ਵਿੱਥ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ plic_ਸੈੱਟ_hart_ਅੰਕੜੇ_ptr(__scratch, __plic)			\
	sbi_scratch_write_type((__scratch), ਖਾਲੀ *, plic_ptr_ਵਿੱਥ, (__plic))

ਬਣਤਰ plic_ਅੰਕੜੇ *plic_get(ਖਾਲੀ)
{
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ਵਾਪਸ plic_get_hart_ਅੰਕੜੇ_ptr(scratch);
}

ਸਥਿਰ u32 plic_get_ਤਰਜੀਹ(ਅਚਲ ਬਣਤਰ plic_ਅੰਕੜੇ *plic, u32 ਸਰੋਤ)
{
	ਅਸਥਿਰ ਖਾਲੀ *plic_ਤਰਜੀਹ = (ਅੱਖਰ *)plic->addr +
			PLIC_ਤਰਜੀਹ_ਅਧਾਰ + 4 * ਸਰੋਤ;
	ਵਾਪਸ readl(plic_ਤਰਜੀਹ);
}

ਸਥਿਰ ਖਾਲੀ plic_ਸੈੱਟ_ਤਰਜੀਹ(ਅਚਲ ਬਣਤਰ plic_ਅੰਕੜੇ *plic, u32 ਸਰੋਤ, u32 val)
{
	ਅਸਥਿਰ ਖਾਲੀ *plic_ਤਰਜੀਹ = (ਅੱਖਰ *)plic->addr +
			PLIC_ਤਰਜੀਹ_ਅਧਾਰ + 4 * ਸਰੋਤ;
	writel(val, plic_ਤਰਜੀਹ);
}

ਸਥਿਰ u32 plic_get_thresh(ਅਚਲ ਬਣਤਰ plic_ਅੰਕੜੇ *plic, u32 cntxid)
{
	ਅਸਥਿਰ ਖਾਲੀ *plic_thresh;

	plic_thresh = (ਅੱਖਰ *)plic->addr +
		      PLIC_CONTEXT_ਅਧਾਰ + PLIC_CONTEXT_STRIDE * cntxid;

	ਵਾਪਸ readl(plic_thresh);
}

ਸਥਿਰ ਖਾਲੀ plic_ਸੈੱਟ_thresh(ਅਚਲ ਬਣਤਰ plic_ਅੰਕੜੇ *plic, u32 cntxid, u32 val)
{
	ਅਸਥਿਰ ਖਾਲੀ *plic_thresh;

	plic_thresh = (ਅੱਖਰ *)plic->addr +
		      PLIC_CONTEXT_ਅਧਾਰ + PLIC_CONTEXT_STRIDE * cntxid;
	writel(val, plic_thresh);
}

ਸਥਿਰ u32 plic_get_ie(ਅਚਲ ਬਣਤਰ plic_ਅੰਕੜੇ *plic, u32 cntxid,
		       u32 ਸ਼ਬਦ_ਸੂਚਕਾਂਕ)
{
	ਅਸਥਿਰ ਖਾਲੀ *plic_ie;

	plic_ie = (ਅੱਖਰ *)plic->addr +
		   PLIC_ENABLE_ਅਧਾਰ + PLIC_ENABLE_STRIDE * cntxid +
		   4 * ਸ਼ਬਦ_ਸੂਚਕਾਂਕ;

	ਵਾਪਸ readl(plic_ie);
}

ਸਥਿਰ ਖਾਲੀ plic_ਸੈੱਟ_ie(ਅਚਲ ਬਣਤਰ plic_ਅੰਕੜੇ *plic, u32 cntxid,
			u32 ਸ਼ਬਦ_ਸੂਚਕਾਂਕ, u32 val)
{
	ਅਸਥਿਰ ਖਾਲੀ *plic_ie;

	plic_ie = (ਅੱਖਰ *)plic->addr +
		   PLIC_ENABLE_ਅਧਾਰ + PLIC_ENABLE_STRIDE * cntxid +
		   4 * ਸ਼ਬਦ_ਸੂਚਕਾਂਕ;
	writel(val, plic_ie);
}

ਸਥਿਰ ਖਾਲੀ plic_delegate(ਅਚਲ ਬਣਤਰ plic_ਅੰਕੜੇ *plic)
{
	/* If this is a T-HEAD PLIC, delegate access to S-mode */
	ਜੇ (plic->flags & PLIC_FLAG_THEAD_DELEGATION)
		writel_relaxed(ਦੋਆਧਾਰੀ_ਅੰਕ(0), (ਅੱਖਰ *)plic->addr + THEAD_PLIC_CTRL_REG);
}

ਸਥਿਰ ਪੂਰਨ plic_context_init(ਅਚਲ ਬਣਤਰ plic_ਅੰਕੜੇ *plic, ਪੂਰਨ context_id,
			     bool enable, u32 ਹੱਦ)
{
	u32 ie_ਸ਼ਬਦ, ie_ਮੁੱਲ;

	ਜੇ (!plic || context_id < 0)
		ਵਾਪਸ SBI_EINVAL;

	ie_ਸ਼ਬਦ = PLIC_IE_ਸ਼ਬਦ(plic);
	ie_ਮੁੱਲ = enable ? 0xffffffffU : 0U;

	ਲਈ (u32 i = 0; i < ie_ਸ਼ਬਦ; i++)
		plic_ਸੈੱਟ_ie(plic, context_id, i, ie_ਮੁੱਲ);

	plic_ਸੈੱਟ_thresh(plic, context_id, ਹੱਦ);

	ਵਾਪਸ 0;
}

ਖਾਲੀ plic_suspend(ਖਾਲੀ)
{
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ਅਚਲ ਬਣਤਰ plic_ਅੰਕੜੇ *plic = plic_get_hart_ਅੰਕੜੇ_ptr(scratch);
	u32 ie_ਸ਼ਬਦ = PLIC_IE_ਸ਼ਬਦ(plic);
	u32 *ਅੰਕੜੇ_ਸ਼ਬਦ = plic->pm_ਅੰਕੜੇ;
	u8 *ਅੰਕੜੇ_byte;

	ਜੇ (!ਅੰਕੜੇ_ਸ਼ਬਦ)
		ਵਾਪਸ;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		ਜੇ (context_id < 0)
			ਜਾਰੀ;

		/* Save the enable bits */
		ਲਈ (u32 i = 0; i < ie_ਸ਼ਬਦ; i++)
			*ਅੰਕੜੇ_ਸ਼ਬਦ++ = plic_get_ie(plic, context_id, i);

		/* Save the context threshold */
		*ਅੰਕੜੇ_ਸ਼ਬਦ++ = plic_get_thresh(plic, context_id);
	}

	/* Restore the input priorities */
	ਅੰਕੜੇ_byte = (u8 *)ਅੰਕੜੇ_ਸ਼ਬਦ;
	ਲਈ (u32 i = 1; i <= plic->ਅੰਕ_src; i++)
		*ਅੰਕੜੇ_byte++ = plic_get_ਤਰਜੀਹ(plic, i);
}

ਖਾਲੀ plic_resume(ਖਾਲੀ)
{
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ਅਚਲ ਬਣਤਰ plic_ਅੰਕੜੇ *plic = plic_get_hart_ਅੰਕੜੇ_ptr(scratch);
	u32 ie_ਸ਼ਬਦ = PLIC_IE_ਸ਼ਬਦ(plic);
	u32 *ਅੰਕੜੇ_ਸ਼ਬਦ = plic->pm_ਅੰਕੜੇ;
	u8 *ਅੰਕੜੇ_byte;

	ਜੇ (!ਅੰਕੜੇ_ਸ਼ਬਦ)
		ਵਾਪਸ;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		ਜੇ (context_id < 0)
			ਜਾਰੀ;

		/* Restore the enable bits */
		ਲਈ (u32 i = 0; i < ie_ਸ਼ਬਦ; i++)
			plic_ਸੈੱਟ_ie(plic, context_id, i, *ਅੰਕੜੇ_ਸ਼ਬਦ++);

		/* Restore the context threshold */
		plic_ਸੈੱਟ_thresh(plic, context_id, *ਅੰਕੜੇ_ਸ਼ਬਦ++);
	}

	/* Restore the input priorities */
	ਅੰਕੜੇ_byte = (u8 *)ਅੰਕੜੇ_ਸ਼ਬਦ;
	ਲਈ (u32 i = 1; i <= plic->ਅੰਕ_src; i++)
		plic_ਸੈੱਟ_ਤਰਜੀਹ(plic, i, *ਅੰਕੜੇ_byte++);

	/* Restore the delegation */
	plic_delegate(plic);
}

ਸਥਿਰ ਪੂਰਨ plic_warm_irqchip_init(ਬਣਤਰ sbi_irqchip_device *dev)
{
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ਅਚਲ ਬਣਤਰ plic_ਅੰਕੜੇ *plic = plic_get_hart_ਅੰਕੜੇ_ptr(scratch);
	u32 hartindex = current_hartindex();
	s16 m_cntx_id = plic->context_map[hartindex][PLIC_M_CONTEXT];
	s16 s_cntx_id = plic->context_map[hartindex][PLIC_S_CONTEXT];
	bool enable;
	ਪੂਰਨ ret;

	/*
	 * By default, disable all IRQs for the target HART. Ariane
	 * has a bug which requires enabling all interrupts at boot.
	 */
	enable = plic->flags & PLIC_FLAG_ARIANE_BUG;

	ਜੇ (m_cntx_id > -1) {
		ret = plic_context_init(plic, m_cntx_id, enable, 0x7);
		ਜੇ (ret)
			ਵਾਪਸ ret;
	}

	ਜੇ (s_cntx_id > -1) {
		ret = plic_context_init(plic, s_cntx_id, enable, 0x7);
		ਜੇ (ret)
			ਵਾਪਸ ret;
	}

	ਵਾਪਸ 0;
}

ਪੂਰਨ plic_cold_irqchip_init(ਬਣਤਰ plic_ਅੰਕੜੇ *plic)
{
	ਪੂਰਨ i, ret;

	ਜੇ (!plic)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (!plic_ptr_ਵਿੱਥ) {
		plic_ptr_ਵਿੱਥ = sbi_scratch_alloc_type_ਵਿੱਥ(ਖਾਲੀ *);
		ਜੇ (!plic_ptr_ਵਿੱਥ)
			ਵਾਪਸ SBI_ENOMEM;
	}

	ਜੇ (plic->flags & PLIC_FLAG_ENABLE_PM) {
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅੰਕੜੇ_ਮਾਪ_2 = 0;

		sbi_for_each_hartindex(i) {
			ਜੇ (plic->context_map[i][PLIC_S_CONTEXT] < 0)
				ਜਾਰੀ;

			/* Allocate space for enable bits */
			ਅੰਕੜੇ_ਮਾਪ_2 += (plic->ਅੰਕ_src / 32 + 1) * ਆਕਾਰ(u32);

			/* Allocate space for the context threshold */
			ਅੰਕੜੇ_ਮਾਪ_2 += ਆਕਾਰ(u32);
		}

		/*
		 * Allocate space for the input priorities. So far,
		 * priorities on all known implementations fit in 8 bits.
		 */
		ਅੰਕੜੇ_ਮਾਪ_2 += plic->ਅੰਕ_src * ਆਕਾਰ(u8);

		plic->pm_ਅੰਕੜੇ = sbi_malloc(ਅੰਕੜੇ_ਮਾਪ_2);
		ਜੇ (!plic->pm_ਅੰਕੜੇ)
			ਵਾਪਸ SBI_ENOMEM;
	}

	/* Configure default priorities of all IRQs */
	ਲਈ (i = 1; i <= plic->ਅੰਕ_src; i++)
		plic_ਸੈੱਟ_ਤਰਜੀਹ(plic, i, 0);

	plic_delegate(plic);

	ret = sbi_domain_ਜੜ੍ਹ_ਜੋੜੋ_memrange(plic->addr, plic->ਮਾਪ, ਦੋਆਧਾਰੀ_ਅੰਕ(20),
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	ਜੇ (ret)
		ਵਾਪਸ ret;

	sbi_for_each_hartindex(i) {
		ਜੇ (plic->context_map[i][PLIC_M_CONTEXT] < 0 &&
		    plic->context_map[i][PLIC_S_CONTEXT] < 0)
			ਜਾਰੀ;

		plic_ਸੈੱਟ_hart_ਅੰਕੜੇ_ptr(sbi_hartindex_ਮੰਜ਼ਿਲ_scratch(i), plic);
		sbi_hartmask_ਸੈੱਟ_hartindex(i, &plic->irqchip.ਨਿਸ਼ਾਨਾ_harts);
	}

	/* Register irqchip device */
	plic->irqchip.id = plic->unique_id;
	plic->irqchip.ਅੰਕ_hwirq = plic->ਅੰਕ_src + 1;
	plic->irqchip.warm_init = plic_warm_irqchip_init;
	ਵਾਪਸ sbi_irqchip_ਜੋੜੋ_device(&plic->irqchip);
}
