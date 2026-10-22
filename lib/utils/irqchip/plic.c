/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Samuel Holland <samuel@sholland.org>
 */

#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/riscv_encoding.h>
#અન્તર્ભાવ <sbi/sbi_bitops.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_domain.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi/sbi_string.h>
#અન્તર્ભાવ <sbi_utils/irqchip/plic.h>

#define PLIC_PRIORITY_BASE 0x0
#define PLIC_PENDING_BASE 0x1000
#define PLIC_ENABLE_BASE 0x2000
#define PLIC_ENABLE_STRIDE 0x80
#define PLIC_CONTEXT_BASE 0x200000
#define PLIC_CONTEXT_STRIDE 0x1000

#define THEAD_PLIC_CTRL_REG 0x1ffffc

સ્થિર unsigned long plic_ptr_offset;

#define plic_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), શૂન્ય *, plic_ptr_offset)

#define plic_set_hart_data_ptr(__scratch, __plic)			\
	sbi_scratch_write_type((__scratch), શૂન્ય *, plic_ptr_offset, (__plic))

સંરચના plic_data *plic_get(શૂન્ય)
{
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	પાછા plic_get_hart_data_ptr(scratch);
}

સ્થિર u32 plic_get_priority(અચલ સંરચના plic_data *plic, u32 source)
{
	અસ્થિર શૂન્ય *plic_priority = (અક્ષર *)plic->addr +
			PLIC_PRIORITY_BASE + 4 * source;
	પાછા readl(plic_priority);
}

સ્થિર શૂન્ય plic_set_priority(અચલ સંરચના plic_data *plic, u32 source, u32 val)
{
	અસ્થિર શૂન્ય *plic_priority = (અક્ષર *)plic->addr +
			PLIC_PRIORITY_BASE + 4 * source;
	writel(val, plic_priority);
}

સ્થિર u32 plic_get_thresh(અચલ સંરચના plic_data *plic, u32 cntxid)
{
	અસ્થિર શૂન્ય *plic_thresh;

	plic_thresh = (અક્ષર *)plic->addr +
		      PLIC_CONTEXT_BASE + PLIC_CONTEXT_STRIDE * cntxid;

	પાછા readl(plic_thresh);
}

સ્થિર શૂન્ય plic_set_thresh(અચલ સંરચના plic_data *plic, u32 cntxid, u32 val)
{
	અસ્થિર શૂન્ય *plic_thresh;

	plic_thresh = (અક્ષર *)plic->addr +
		      PLIC_CONTEXT_BASE + PLIC_CONTEXT_STRIDE * cntxid;
	writel(val, plic_thresh);
}

સ્થિર u32 plic_get_ie(અચલ સંરચના plic_data *plic, u32 cntxid,
		       u32 word_index)
{
	અસ્થિર શૂન્ય *plic_ie;

	plic_ie = (અક્ષર *)plic->addr +
		   PLIC_ENABLE_BASE + PLIC_ENABLE_STRIDE * cntxid +
		   4 * word_index;

	પાછા readl(plic_ie);
}

સ્થિર શૂન્ય plic_set_ie(અચલ સંરચના plic_data *plic, u32 cntxid,
			u32 word_index, u32 val)
{
	અસ્થિર શૂન્ય *plic_ie;

	plic_ie = (અક્ષર *)plic->addr +
		   PLIC_ENABLE_BASE + PLIC_ENABLE_STRIDE * cntxid +
		   4 * word_index;
	writel(val, plic_ie);
}

સ્થિર શૂન્ય plic_delegate(અચલ સંરચના plic_data *plic)
{
	/* If this is a T-HEAD PLIC, delegate access to S-mode */
	જો (plic->flags & PLIC_FLAG_THEAD_DELEGATION)
		writel_relaxed(BIT(0), (અક્ષર *)plic->addr + THEAD_PLIC_CTRL_REG);
}

સ્થિર પૂર્ણાંક plic_context_init(અચલ સંરચના plic_data *plic, પૂર્ણાંક context_id,
			     bool enable, u32 threshold)
{
	u32 ie_શબ્દો, ie_કિંમત;

	જો (!plic || context_id < 0)
		પાછા SBI_EINVAL;

	ie_શબ્દો = PLIC_IE_શબ્દો(plic);
	ie_કિંમત = enable ? 0xffffffffU : 0U;

	માટે (u32 i = 0; i < ie_શબ્દો; i++)
		plic_set_ie(plic, context_id, i, ie_કિંમત);

	plic_set_thresh(plic, context_id, threshold);

	પાછા 0;
}

શૂન્ય plic_suspend(શૂન્ય)
{
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	અચલ સંરચના plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 ie_શબ્દો = PLIC_IE_શબ્દો(plic);
	u32 *data_word = plic->pm_data;
	u8 *data_byte;

	જો (!data_word)
		પાછા;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		જો (context_id < 0)
			ચાલુ;

		/* Save the enable bits */
		માટે (u32 i = 0; i < ie_શબ્દો; i++)
			*data_word++ = plic_get_ie(plic, context_id, i);

		/* Save the context threshold */
		*data_word++ = plic_get_thresh(plic, context_id);
	}

	/* Restore the input priorities */
	data_byte = (u8 *)data_word;
	માટે (u32 i = 1; i <= plic->અંક_src; i++)
		*data_byte++ = plic_get_priority(plic, i);
}

શૂન્ય plic_resume(શૂન્ય)
{
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	અચલ સંરચના plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 ie_શબ્દો = PLIC_IE_શબ્દો(plic);
	u32 *data_word = plic->pm_data;
	u8 *data_byte;

	જો (!data_word)
		પાછા;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		જો (context_id < 0)
			ચાલુ;

		/* Restore the enable bits */
		માટે (u32 i = 0; i < ie_શબ્દો; i++)
			plic_set_ie(plic, context_id, i, *data_word++);

		/* Restore the context threshold */
		plic_set_thresh(plic, context_id, *data_word++);
	}

	/* Restore the input priorities */
	data_byte = (u8 *)data_word;
	માટે (u32 i = 1; i <= plic->અંક_src; i++)
		plic_set_priority(plic, i, *data_byte++);

	/* Restore the delegation */
	plic_delegate(plic);
}

સ્થિર પૂર્ણાંક plic_warm_irqchip_init(સંરચના sbi_irqchip_device *dev)
{
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	અચલ સંરચના plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 hartindex = current_hartindex();
	s16 m_cntx_id = plic->context_map[hartindex][PLIC_M_CONTEXT];
	s16 s_cntx_id = plic->context_map[hartindex][PLIC_S_CONTEXT];
	bool enable;
	પૂર્ણાંક ret;

	/*
	 * By default, disable all IRQs for the target HART. Ariane
	 * has a bug which requires enabling all interrupts at boot.
	 */
	enable = plic->flags & PLIC_FLAG_ARIANE_BUG;

	જો (m_cntx_id > -1) {
		ret = plic_context_init(plic, m_cntx_id, enable, 0x7);
		જો (ret)
			પાછા ret;
	}

	જો (s_cntx_id > -1) {
		ret = plic_context_init(plic, s_cntx_id, enable, 0x7);
		જો (ret)
			પાછા ret;
	}

	પાછા 0;
}

પૂર્ણાંક plic_cold_irqchip_init(સંરચના plic_data *plic)
{
	પૂર્ણાંક i, ret;

	જો (!plic)
		પાછા SBI_EINVAL;

	જો (!plic_ptr_offset) {
		plic_ptr_offset = sbi_scratch_alloc_type_offset(શૂન્ય *);
		જો (!plic_ptr_offset)
			પાછા SBI_ENOMEM;
	}

	જો (plic->flags & PLIC_FLAG_ENABLE_PM) {
		unsigned long data_size = 0;

		sbi_for_each_hartindex(i) {
			જો (plic->context_map[i][PLIC_S_CONTEXT] < 0)
				ચાલુ;

			/* Allocate space for enable bits */
			data_size += (plic->અંક_src / 32 + 1) * કદ(u32);

			/* Allocate space for the context threshold */
			data_size += કદ(u32);
		}

		/*
		 * Allocate space for the input priorities. So far,
		 * priorities on all known implementations fit in 8 bits.
		 */
		data_size += plic->અંક_src * કદ(u8);

		plic->pm_data = sbi_malloc(data_size);
		જો (!plic->pm_data)
			પાછા SBI_ENOMEM;
	}

	/* Configure default priorities of all IRQs */
	માટે (i = 1; i <= plic->અંક_src; i++)
		plic_set_priority(plic, i, 0);

	plic_delegate(plic);

	ret = sbi_domain_મૂળ_add_memrange(plic->addr, plic->size, BIT(20),
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	જો (ret)
		પાછા ret;

	sbi_for_each_hartindex(i) {
		જો (plic->context_map[i][PLIC_M_CONTEXT] < 0 &&
		    plic->context_map[i][PLIC_S_CONTEXT] < 0)
			ચાલુ;

		plic_set_hart_data_ptr(sbi_hartindex_to_scratch(i), plic);
		sbi_hartmask_set_hartindex(i, &plic->irqchip.target_harts);
	}

	/* Register irqchip device */
	plic->irqchip.id = plic->unique_id;
	plic->irqchip.અંક_hwirq = plic->અંક_src + 1;
	plic->irqchip.warm_init = plic_warm_irqchip_init;
	પાછા sbi_irqchip_add_device(&plic->irqchip);
}
