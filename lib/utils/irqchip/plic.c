/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Samuel Holland <samuel@sholland.org>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_encoding.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitops.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/irqchip/plic.h>

#ವ್ಯಾಖ್ಯಾನಿಸು PLIC_ಆದ್ಯತೆ_ತಳ 0x0
#ವ್ಯಾಖ್ಯಾನಿಸು PLIC_PENDING_ತಳ 0x1000
#ವ್ಯಾಖ್ಯಾನಿಸು PLIC_ENABLE_ತಳ 0x2000
#ವ್ಯಾಖ್ಯಾನಿಸು PLIC_ENABLE_STRIDE 0x80
#ವ್ಯಾಖ್ಯಾನಿಸು PLIC_CONTEXT_ತಳ 0x200000
#ವ್ಯಾಖ್ಯಾನಿಸು PLIC_CONTEXT_STRIDE 0x1000

#ವ್ಯಾಖ್ಯಾನಿಸು THEAD_PLIC_CTRL_REG 0x1ffffc

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ plic_ptr_ಸ್ಥಾನಾಂತರ;

#ವ್ಯಾಖ್ಯಾನಿಸು plic_get_hart_ದತ್ತಾಂಶ_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ಶೂನ್ಯ *, plic_ptr_ಸ್ಥಾನಾಂತರ)

#ವ್ಯಾಖ್ಯಾನಿಸು plic_ಗಣ_hart_ದತ್ತಾಂಶ_ptr(__scratch, __plic)			\
	sbi_scratch_write_type((__scratch), ಶೂನ್ಯ *, plic_ptr_ಸ್ಥಾನಾಂತರ, (__plic))

ರಚನೆ plic_ದತ್ತಾಂಶ *plic_get(ಶೂನ್ಯ)
{
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ಹಿಂತಿರುಗಿ plic_get_hart_ದತ್ತಾಂಶ_ptr(scratch);
}

ಸ್ಥಿರ u32 plic_get_ಆದ್ಯತೆ(ಸ್ಥಿರಾಂಕ ರಚನೆ plic_ದತ್ತಾಂಶ *plic, u32 ಮೂಲ)
{
	ಅಸ್ಥಿರ ಶೂನ್ಯ *plic_ಆದ್ಯತೆ = (ಅಕ್ಷರ *)plic->addr +
			PLIC_ಆದ್ಯತೆ_ತಳ + 4 * ಮೂಲ;
	ಹಿಂತಿರುಗಿ readl(plic_ಆದ್ಯತೆ);
}

ಸ್ಥಿರ ಶೂನ್ಯ plic_ಗಣ_ಆದ್ಯತೆ(ಸ್ಥಿರಾಂಕ ರಚನೆ plic_ದತ್ತಾಂಶ *plic, u32 ಮೂಲ, u32 val)
{
	ಅಸ್ಥಿರ ಶೂನ್ಯ *plic_ಆದ್ಯತೆ = (ಅಕ್ಷರ *)plic->addr +
			PLIC_ಆದ್ಯತೆ_ತಳ + 4 * ಮೂಲ;
	writel(val, plic_ಆದ್ಯತೆ);
}

ಸ್ಥಿರ u32 plic_get_thresh(ಸ್ಥಿರಾಂಕ ರಚನೆ plic_ದತ್ತಾಂಶ *plic, u32 cntxid)
{
	ಅಸ್ಥಿರ ಶೂನ್ಯ *plic_thresh;

	plic_thresh = (ಅಕ್ಷರ *)plic->addr +
		      PLIC_CONTEXT_ತಳ + PLIC_CONTEXT_STRIDE * cntxid;

	ಹಿಂತಿರುಗಿ readl(plic_thresh);
}

ಸ್ಥಿರ ಶೂನ್ಯ plic_ಗಣ_thresh(ಸ್ಥಿರಾಂಕ ರಚನೆ plic_ದತ್ತಾಂಶ *plic, u32 cntxid, u32 val)
{
	ಅಸ್ಥಿರ ಶೂನ್ಯ *plic_thresh;

	plic_thresh = (ಅಕ್ಷರ *)plic->addr +
		      PLIC_CONTEXT_ತಳ + PLIC_CONTEXT_STRIDE * cntxid;
	writel(val, plic_thresh);
}

ಸ್ಥಿರ u32 plic_get_ie(ಸ್ಥಿರಾಂಕ ರಚನೆ plic_ದತ್ತಾಂಶ *plic, u32 cntxid,
		       u32 ಶಬ್ದ_ಸೂಚ್ಯಂಕ)
{
	ಅಸ್ಥಿರ ಶೂನ್ಯ *plic_ie;

	plic_ie = (ಅಕ್ಷರ *)plic->addr +
		   PLIC_ENABLE_ತಳ + PLIC_ENABLE_STRIDE * cntxid +
		   4 * ಶಬ್ದ_ಸೂಚ್ಯಂಕ;

	ಹಿಂತಿರುಗಿ readl(plic_ie);
}

ಸ್ಥಿರ ಶೂನ್ಯ plic_ಗಣ_ie(ಸ್ಥಿರಾಂಕ ರಚನೆ plic_ದತ್ತಾಂಶ *plic, u32 cntxid,
			u32 ಶಬ್ದ_ಸೂಚ್ಯಂಕ, u32 val)
{
	ಅಸ್ಥಿರ ಶೂನ್ಯ *plic_ie;

	plic_ie = (ಅಕ್ಷರ *)plic->addr +
		   PLIC_ENABLE_ತಳ + PLIC_ENABLE_STRIDE * cntxid +
		   4 * ಶಬ್ದ_ಸೂಚ್ಯಂಕ;
	writel(val, plic_ie);
}

ಸ್ಥಿರ ಶೂನ್ಯ plic_delegate(ಸ್ಥಿರಾಂಕ ರಚನೆ plic_ದತ್ತಾಂಶ *plic)
{
	/* If this is a T-HEAD PLIC, delegate access to S-mode */
	ಆದರೆ (plic->flags & PLIC_FLAG_THEAD_DELEGATION)
		writel_relaxed(ಬಿಟ್(0), (ಅಕ್ಷರ *)plic->addr + THEAD_PLIC_CTRL_REG);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ plic_context_init(ಸ್ಥಿರಾಂಕ ರಚನೆ plic_ದತ್ತಾಂಶ *plic, ಪೂರ್ಣಾಂಕ context_id,
			     bool enable, u32 ಮಿತಿ)
{
	u32 ie_ಪದಗಳು, ie_ಮೌಲ್ಯ;

	ಆದರೆ (!plic || context_id < 0)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ie_ಪದಗಳು = PLIC_IE_ಪದಗಳು(plic);
	ie_ಮೌಲ್ಯ = enable ? 0xffffffffU : 0U;

	ಗಾಗಿ (u32 i = 0; i < ie_ಪದಗಳು; i++)
		plic_ಗಣ_ie(plic, context_id, i, ie_ಮೌಲ್ಯ);

	plic_ಗಣ_thresh(plic, context_id, ಮಿತಿ);

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ plic_suspend(ಶೂನ್ಯ)
{
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ಸ್ಥಿರಾಂಕ ರಚನೆ plic_ದತ್ತಾಂಶ *plic = plic_get_hart_ದತ್ತಾಂಶ_ptr(scratch);
	u32 ie_ಪದಗಳು = PLIC_IE_ಪದಗಳು(plic);
	u32 *ದತ್ತಾಂಶ_ಶಬ್ದ = plic->pm_ದತ್ತಾಂಶ;
	u8 *ದತ್ತಾಂಶ_byte;

	ಆದರೆ (!ದತ್ತಾಂಶ_ಶಬ್ದ)
		ಹಿಂತಿರುಗಿ;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		ಆದರೆ (context_id < 0)
			ಮುಂದುವರಿ;

		/* Save the enable bits */
		ಗಾಗಿ (u32 i = 0; i < ie_ಪದಗಳು; i++)
			*ದತ್ತಾಂಶ_ಶಬ್ದ++ = plic_get_ie(plic, context_id, i);

		/* Save the context threshold */
		*ದತ್ತಾಂಶ_ಶಬ್ದ++ = plic_get_thresh(plic, context_id);
	}

	/* Restore the input priorities */
	ದತ್ತಾಂಶ_byte = (u8 *)ದತ್ತಾಂಶ_ಶಬ್ದ;
	ಗಾಗಿ (u32 i = 1; i <= plic->ಅಂಕೆ_src; i++)
		*ದತ್ತಾಂಶ_byte++ = plic_get_ಆದ್ಯತೆ(plic, i);
}

ಶೂನ್ಯ plic_resume(ಶೂನ್ಯ)
{
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ಸ್ಥಿರಾಂಕ ರಚನೆ plic_ದತ್ತಾಂಶ *plic = plic_get_hart_ದತ್ತಾಂಶ_ptr(scratch);
	u32 ie_ಪದಗಳು = PLIC_IE_ಪದಗಳು(plic);
	u32 *ದತ್ತಾಂಶ_ಶಬ್ದ = plic->pm_ದತ್ತಾಂಶ;
	u8 *ದತ್ತಾಂಶ_byte;

	ಆದರೆ (!ದತ್ತಾಂಶ_ಶಬ್ದ)
		ಹಿಂತಿರುಗಿ;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		ಆದರೆ (context_id < 0)
			ಮುಂದುವರಿ;

		/* Restore the enable bits */
		ಗಾಗಿ (u32 i = 0; i < ie_ಪದಗಳು; i++)
			plic_ಗಣ_ie(plic, context_id, i, *ದತ್ತಾಂಶ_ಶಬ್ದ++);

		/* Restore the context threshold */
		plic_ಗಣ_thresh(plic, context_id, *ದತ್ತಾಂಶ_ಶಬ್ದ++);
	}

	/* Restore the input priorities */
	ದತ್ತಾಂಶ_byte = (u8 *)ದತ್ತಾಂಶ_ಶಬ್ದ;
	ಗಾಗಿ (u32 i = 1; i <= plic->ಅಂಕೆ_src; i++)
		plic_ಗಣ_ಆದ್ಯತೆ(plic, i, *ದತ್ತಾಂಶ_byte++);

	/* Restore the delegation */
	plic_delegate(plic);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ plic_warm_irqchip_init(ರಚನೆ sbi_irqchip_device *dev)
{
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ಸ್ಥಿರಾಂಕ ರಚನೆ plic_ದತ್ತಾಂಶ *plic = plic_get_hart_ದತ್ತಾಂಶ_ptr(scratch);
	u32 hartindex = current_hartindex();
	s16 m_cntx_id = plic->context_map[hartindex][PLIC_M_CONTEXT];
	s16 s_cntx_id = plic->context_map[hartindex][PLIC_S_CONTEXT];
	bool enable;
	ಪೂರ್ಣಾಂಕ ret;

	/*
	 * By default, disable all IRQs for the target HART. Ariane
	 * has a bug which requires enabling all interrupts at boot.
	 */
	enable = plic->flags & PLIC_FLAG_ARIANE_BUG;

	ಆದರೆ (m_cntx_id > -1) {
		ret = plic_context_init(plic, m_cntx_id, enable, 0x7);
		ಆದರೆ (ret)
			ಹಿಂತಿರುಗಿ ret;
	}

	ಆದರೆ (s_cntx_id > -1) {
		ret = plic_context_init(plic, s_cntx_id, enable, 0x7);
		ಆದರೆ (ret)
			ಹಿಂತಿರುಗಿ ret;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ plic_cold_irqchip_init(ರಚನೆ plic_ದತ್ತಾಂಶ *plic)
{
	ಪೂರ್ಣಾಂಕ i, ret;

	ಆದರೆ (!plic)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (!plic_ptr_ಸ್ಥಾನಾಂತರ) {
		plic_ptr_ಸ್ಥಾನಾಂತರ = sbi_scratch_alloc_type_ಸ್ಥಾನಾಂತರ(ಶೂನ್ಯ *);
		ಆದರೆ (!plic_ptr_ಸ್ಥಾನಾಂತರ)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;
	}

	ಆದರೆ (plic->flags & PLIC_FLAG_ENABLE_PM) {
		ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದತ್ತಾಂಶ_ಗಾತ್ರ_2 = 0;

		sbi_for_each_hartindex(i) {
			ಆದರೆ (plic->context_map[i][PLIC_S_CONTEXT] < 0)
				ಮುಂದುವರಿ;

			/* Allocate space for enable bits */
			ದತ್ತಾಂಶ_ಗಾತ್ರ_2 += (plic->ಅಂಕೆ_src / 32 + 1) * ಗಾತ್ರ(u32);

			/* Allocate space for the context threshold */
			ದತ್ತಾಂಶ_ಗಾತ್ರ_2 += ಗಾತ್ರ(u32);
		}

		/*
		 * Allocate space for the input priorities. So far,
		 * priorities on all known implementations fit in 8 bits.
		 */
		ದತ್ತಾಂಶ_ಗಾತ್ರ_2 += plic->ಅಂಕೆ_src * ಗಾತ್ರ(u8);

		plic->pm_ದತ್ತಾಂಶ = sbi_malloc(ದತ್ತಾಂಶ_ಗಾತ್ರ_2);
		ಆದರೆ (!plic->pm_ದತ್ತಾಂಶ)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;
	}

	/* Configure default priorities of all IRQs */
	ಗಾಗಿ (i = 1; i <= plic->ಅಂಕೆ_src; i++)
		plic_ಗಣ_ಆದ್ಯತೆ(plic, i, 0);

	plic_delegate(plic);

	ret = sbi_domain_ಬೇರು_ಕೂಡಿಸು_memrange(plic->addr, plic->ಗಾತ್ರ_2, ಬಿಟ್(20),
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ ret;

	sbi_for_each_hartindex(i) {
		ಆದರೆ (plic->context_map[i][PLIC_M_CONTEXT] < 0 &&
		    plic->context_map[i][PLIC_S_CONTEXT] < 0)
			ಮುಂದುವರಿ;

		plic_ಗಣ_hart_ದತ್ತಾಂಶ_ptr(sbi_hartindex_ಗಮ್ಯ_scratch(i), plic);
		sbi_hartmask_ಗಣ_hartindex(i, &plic->irqchip.ಗುರಿ_harts);
	}

	/* Register irqchip device */
	plic->irqchip.id = plic->unique_id;
	plic->irqchip.ಅಂಕೆ_hwirq = plic->ಅಂಕೆ_src + 1;
	plic->irqchip.warm_init = plic_warm_irqchip_init;
	ಹಿಂತಿರುಗಿ sbi_irqchip_ಕೂಡಿಸು_device(&plic->irqchip);
}
