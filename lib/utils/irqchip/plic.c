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

#define PLIC_PRIORITY_BASE 0x0
#define PLIC_PENDING_BASE 0x1000
#define PLIC_ENABLE_BASE 0x2000
#define PLIC_ENABLE_STRIDE 0x80
#define PLIC_CONTEXT_BASE 0x200000
#define PLIC_CONTEXT_STRIDE 0x1000

#define THEAD_PLIC_CTRL_REG 0x1ffffc

ಸ್ಥಿರ unsigned long plic_ptr_offset;

#define plic_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ಶೂನ್ಯ *, plic_ptr_offset)

#define plic_set_hart_data_ptr(__scratch, __plic)			\
	sbi_scratch_write_type((__scratch), ಶೂನ್ಯ *, plic_ptr_offset, (__plic))

ಸಂರಚನಾ plic_data *plic_get(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ಪ್ರತ್ಯಾಗಮ plic_get_hart_data_ptr(scratch);
}

ಸ್ಥಿರ u32 plic_get_priority(ಧ್ರುವ ಸಂರಚನಾ plic_data *plic, u32 source)
{
	ಚಞ್ಚಲ ಶೂನ್ಯ *plic_priority = (ವರ್ಣ *)plic->addr +
			PLIC_PRIORITY_BASE + 4 * source;
	ಪ್ರತ್ಯಾಗಮ readl(plic_priority);
}

ಸ್ಥಿರ ಶೂನ್ಯ plic_set_priority(ಧ್ರುವ ಸಂರಚನಾ plic_data *plic, u32 source, u32 val)
{
	ಚಞ್ಚಲ ಶೂನ್ಯ *plic_priority = (ವರ್ಣ *)plic->addr +
			PLIC_PRIORITY_BASE + 4 * source;
	writel(val, plic_priority);
}

ಸ್ಥಿರ u32 plic_get_thresh(ಧ್ರುವ ಸಂರಚನಾ plic_data *plic, u32 cntxid)
{
	ಚಞ್ಚಲ ಶೂನ್ಯ *plic_thresh;

	plic_thresh = (ವರ್ಣ *)plic->addr +
		      PLIC_CONTEXT_BASE + PLIC_CONTEXT_STRIDE * cntxid;

	ಪ್ರತ್ಯಾಗಮ readl(plic_thresh);
}

ಸ್ಥಿರ ಶೂನ್ಯ plic_set_thresh(ಧ್ರುವ ಸಂರಚನಾ plic_data *plic, u32 cntxid, u32 val)
{
	ಚಞ್ಚಲ ಶೂನ್ಯ *plic_thresh;

	plic_thresh = (ವರ್ಣ *)plic->addr +
		      PLIC_CONTEXT_BASE + PLIC_CONTEXT_STRIDE * cntxid;
	writel(val, plic_thresh);
}

ಸ್ಥಿರ u32 plic_get_ie(ಧ್ರುವ ಸಂರಚನಾ plic_data *plic, u32 cntxid,
		       u32 word_index)
{
	ಚಞ್ಚಲ ಶೂನ್ಯ *plic_ie;

	plic_ie = (ವರ್ಣ *)plic->addr +
		   PLIC_ENABLE_BASE + PLIC_ENABLE_STRIDE * cntxid +
		   4 * word_index;

	ಪ್ರತ್ಯಾಗಮ readl(plic_ie);
}

ಸ್ಥಿರ ಶೂನ್ಯ plic_set_ie(ಧ್ರುವ ಸಂರಚನಾ plic_data *plic, u32 cntxid,
			u32 word_index, u32 val)
{
	ಚಞ್ಚಲ ಶೂನ್ಯ *plic_ie;

	plic_ie = (ವರ್ಣ *)plic->addr +
		   PLIC_ENABLE_BASE + PLIC_ENABLE_STRIDE * cntxid +
		   4 * word_index;
	writel(val, plic_ie);
}

ಸ್ಥಿರ ಶೂನ್ಯ plic_delegate(ಧ್ರುವ ಸಂರಚನಾ plic_data *plic)
{
	/* If this is a T-HEAD PLIC, delegate access to S-mode */
	ಯದಿ (plic->flags & PLIC_FLAG_THEAD_DELEGATION)
		writel_relaxed(BIT(0), (ವರ್ಣ *)plic->addr + THEAD_PLIC_CTRL_REG);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ plic_context_init(ಧ್ರುವ ಸಂರಚನಾ plic_data *plic, ಪೂರ್ಣಾಙ್ಕ context_id,
			     bool enable, u32 threshold)
{
	u32 ie_ಪದಗಳು, ie_ಮೌಲ್ಯ;

	ಯದಿ (!plic || context_id < 0)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ie_ಪದಗಳು = PLIC_IE_ಪದಗಳು(plic);
	ie_ಮೌಲ್ಯ = enable ? 0xffffffffU : 0U;

	ಪ್ರತಿ (u32 i = 0; i < ie_ಪದಗಳು; i++)
		plic_set_ie(plic, context_id, i, ie_ಮೌಲ್ಯ);

	plic_set_thresh(plic, context_id, threshold);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ plic_suspend(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ಧ್ರುವ ಸಂರಚನಾ plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 ie_ಪದಗಳು = PLIC_IE_ಪದಗಳು(plic);
	u32 *data_word = plic->pm_data;
	u8 *data_byte;

	ಯದಿ (!data_word)
		ಪ್ರತ್ಯಾಗಮ;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		ಯದಿ (context_id < 0)
			ಅನುವರ್ತ;

		/* Save the enable bits */
		ಪ್ರತಿ (u32 i = 0; i < ie_ಪದಗಳು; i++)
			*data_word++ = plic_get_ie(plic, context_id, i);

		/* Save the context threshold */
		*data_word++ = plic_get_thresh(plic, context_id);
	}

	/* Restore the input priorities */
	data_byte = (u8 *)data_word;
	ಪ್ರತಿ (u32 i = 1; i <= plic->ಅಂಕೆ_src; i++)
		*data_byte++ = plic_get_priority(plic, i);
}

ಶೂನ್ಯ plic_resume(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ಧ್ರುವ ಸಂರಚನಾ plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 ie_ಪದಗಳು = PLIC_IE_ಪದಗಳು(plic);
	u32 *data_word = plic->pm_data;
	u8 *data_byte;

	ಯದಿ (!data_word)
		ಪ್ರತ್ಯಾಗಮ;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		ಯದಿ (context_id < 0)
			ಅನುವರ್ತ;

		/* Restore the enable bits */
		ಪ್ರತಿ (u32 i = 0; i < ie_ಪದಗಳು; i++)
			plic_set_ie(plic, context_id, i, *data_word++);

		/* Restore the context threshold */
		plic_set_thresh(plic, context_id, *data_word++);
	}

	/* Restore the input priorities */
	data_byte = (u8 *)data_word;
	ಪ್ರತಿ (u32 i = 1; i <= plic->ಅಂಕೆ_src; i++)
		plic_set_priority(plic, i, *data_byte++);

	/* Restore the delegation */
	plic_delegate(plic);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ plic_warm_irqchip_init(ಸಂರಚನಾ sbi_irqchip_device *dev)
{
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ಧ್ರುವ ಸಂರಚನಾ plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 hartindex = current_hartindex();
	s16 m_cntx_id = plic->context_map[hartindex][PLIC_M_CONTEXT];
	s16 s_cntx_id = plic->context_map[hartindex][PLIC_S_CONTEXT];
	bool enable;
	ಪೂರ್ಣಾಙ್ಕ ret;

	/*
	 * By default, disable all IRQs for the target HART. Ariane
	 * has a bug which requires enabling all interrupts at boot.
	 */
	enable = plic->flags & PLIC_FLAG_ARIANE_BUG;

	ಯದಿ (m_cntx_id > -1) {
		ret = plic_context_init(plic, m_cntx_id, enable, 0x7);
		ಯದಿ (ret)
			ಪ್ರತ್ಯಾಗಮ ret;
	}

	ಯದಿ (s_cntx_id > -1) {
		ret = plic_context_init(plic, s_cntx_id, enable, 0x7);
		ಯದಿ (ret)
			ಪ್ರತ್ಯಾಗಮ ret;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ plic_cold_irqchip_init(ಸಂರಚನಾ plic_data *plic)
{
	ಪೂರ್ಣಾಙ್ಕ i, ret;

	ಯದಿ (!plic)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (!plic_ptr_offset) {
		plic_ptr_offset = sbi_scratch_alloc_type_offset(ಶೂನ್ಯ *);
		ಯದಿ (!plic_ptr_offset)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;
	}

	ಯದಿ (plic->flags & PLIC_FLAG_ENABLE_PM) {
		unsigned long data_size = 0;

		sbi_for_each_hartindex(i) {
			ಯದಿ (plic->context_map[i][PLIC_S_CONTEXT] < 0)
				ಅನುವರ್ತ;

			/* Allocate space for enable bits */
			data_size += (plic->ಅಂಕೆ_src / 32 + 1) * ಪರಿಮಾಣ(u32);

			/* Allocate space for the context threshold */
			data_size += ಪರಿಮಾಣ(u32);
		}

		/*
		 * Allocate space for the input priorities. So far,
		 * priorities on all known implementations fit in 8 bits.
		 */
		data_size += plic->ಅಂಕೆ_src * ಪರಿಮಾಣ(u8);

		plic->pm_data = sbi_malloc(data_size);
		ಯದಿ (!plic->pm_data)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;
	}

	/* Configure default priorities of all IRQs */
	ಪ್ರತಿ (i = 1; i <= plic->ಅಂಕೆ_src; i++)
		plic_set_priority(plic, i, 0);

	plic_delegate(plic);

	ret = sbi_domain_ಬೇರು_add_memrange(plic->addr, plic->size, BIT(20),
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ ret;

	sbi_for_each_hartindex(i) {
		ಯದಿ (plic->context_map[i][PLIC_M_CONTEXT] < 0 &&
		    plic->context_map[i][PLIC_S_CONTEXT] < 0)
			ಅನುವರ್ತ;

		plic_set_hart_data_ptr(sbi_hartindex_to_scratch(i), plic);
		sbi_hartmask_set_hartindex(i, &plic->irqchip.target_harts);
	}

	/* Register irqchip device */
	plic->irqchip.id = plic->unique_id;
	plic->irqchip.ಅಂಕೆ_hwirq = plic->ಅಂಕೆ_src + 1;
	plic->irqchip.warm_init = plic_warm_irqchip_init;
	ಪ್ರತ್ಯಾಗಮ sbi_irqchip_add_device(&plic->irqchip);
}
