/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Samuel Holland <samuel@sholland.org>
 */

#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/riscv_encoding.h>
#അന്തര്ഭാവ <sbi/sbi_bitops.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi/sbi_string.h>
#അന്തര്ഭാവ <sbi_utils/irqchip/plic.h>

#define PLIC_PRIORITY_BASE 0x0
#define PLIC_PENDING_BASE 0x1000
#define PLIC_ENABLE_BASE 0x2000
#define PLIC_ENABLE_STRIDE 0x80
#define PLIC_CONTEXT_BASE 0x200000
#define PLIC_CONTEXT_STRIDE 0x1000

#define THEAD_PLIC_CTRL_REG 0x1ffffc

സ്ഥിതം unsigned long plic_ptr_offset;

#define plic_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ശൂന്യം *, plic_ptr_offset)

#define plic_set_hart_data_ptr(__scratch, __plic)			\
	sbi_scratch_write_type((__scratch), ശൂന്യം *, plic_ptr_offset, (__plic))

ഘടന plic_data *plic_get(ശൂന്യം)
{
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	തിരികെ plic_get_hart_data_ptr(scratch);
}

സ്ഥിതം u32 plic_get_priority(സ്ഥിരം ഘടന plic_data *plic, u32 source)
{
	അസ്ഥിര ശൂന്യം *plic_priority = (അക്ഷരം *)plic->addr +
			PLIC_PRIORITY_BASE + 4 * source;
	തിരികെ readl(plic_priority);
}

സ്ഥിതം ശൂന്യം plic_set_priority(സ്ഥിരം ഘടന plic_data *plic, u32 source, u32 val)
{
	അസ്ഥിര ശൂന്യം *plic_priority = (അക്ഷരം *)plic->addr +
			PLIC_PRIORITY_BASE + 4 * source;
	writel(val, plic_priority);
}

സ്ഥിതം u32 plic_get_thresh(സ്ഥിരം ഘടന plic_data *plic, u32 cntxid)
{
	അസ്ഥിര ശൂന്യം *plic_thresh;

	plic_thresh = (അക്ഷരം *)plic->addr +
		      PLIC_CONTEXT_BASE + PLIC_CONTEXT_STRIDE * cntxid;

	തിരികെ readl(plic_thresh);
}

സ്ഥിതം ശൂന്യം plic_set_thresh(സ്ഥിരം ഘടന plic_data *plic, u32 cntxid, u32 val)
{
	അസ്ഥിര ശൂന്യം *plic_thresh;

	plic_thresh = (അക്ഷരം *)plic->addr +
		      PLIC_CONTEXT_BASE + PLIC_CONTEXT_STRIDE * cntxid;
	writel(val, plic_thresh);
}

സ്ഥിതം u32 plic_get_ie(സ്ഥിരം ഘടന plic_data *plic, u32 cntxid,
		       u32 word_index)
{
	അസ്ഥിര ശൂന്യം *plic_ie;

	plic_ie = (അക്ഷരം *)plic->addr +
		   PLIC_ENABLE_BASE + PLIC_ENABLE_STRIDE * cntxid +
		   4 * word_index;

	തിരികെ readl(plic_ie);
}

സ്ഥിതം ശൂന്യം plic_set_ie(സ്ഥിരം ഘടന plic_data *plic, u32 cntxid,
			u32 word_index, u32 val)
{
	അസ്ഥിര ശൂന്യം *plic_ie;

	plic_ie = (അക്ഷരം *)plic->addr +
		   PLIC_ENABLE_BASE + PLIC_ENABLE_STRIDE * cntxid +
		   4 * word_index;
	writel(val, plic_ie);
}

സ്ഥിതം ശൂന്യം plic_delegate(സ്ഥിരം ഘടന plic_data *plic)
{
	/* If this is a T-HEAD PLIC, delegate access to S-mode */
	എങ്കിൽ (plic->flags & PLIC_FLAG_THEAD_DELEGATION)
		writel_relaxed(BIT(0), (അക്ഷരം *)plic->addr + THEAD_PLIC_CTRL_REG);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ plic_context_init(സ്ഥിരം ഘടന plic_data *plic, പൂർണ്ണസംഖ്യ context_id,
			     bool enable, u32 threshold)
{
	u32 ie_വാക്കുകൾ, ie_മൂല്യം;

	എങ്കിൽ (!plic || context_id < 0)
		തിരികെ SBI_EINVAL;

	ie_വാക്കുകൾ = PLIC_IE_വാക്കുകൾ(plic);
	ie_മൂല്യം = enable ? 0xffffffffU : 0U;

	വേണ്ടി (u32 i = 0; i < ie_വാക്കുകൾ; i++)
		plic_set_ie(plic, context_id, i, ie_മൂല്യം);

	plic_set_thresh(plic, context_id, threshold);

	തിരികെ 0;
}

ശൂന്യം plic_suspend(ശൂന്യം)
{
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	സ്ഥിരം ഘടന plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 ie_വാക്കുകൾ = PLIC_IE_വാക്കുകൾ(plic);
	u32 *data_word = plic->pm_data;
	u8 *data_byte;

	എങ്കിൽ (!data_word)
		തിരികെ;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		എങ്കിൽ (context_id < 0)
			തുടരുക;

		/* Save the enable bits */
		വേണ്ടി (u32 i = 0; i < ie_വാക്കുകൾ; i++)
			*data_word++ = plic_get_ie(plic, context_id, i);

		/* Save the context threshold */
		*data_word++ = plic_get_thresh(plic, context_id);
	}

	/* Restore the input priorities */
	data_byte = (u8 *)data_word;
	വേണ്ടി (u32 i = 1; i <= plic->അക്കം_src; i++)
		*data_byte++ = plic_get_priority(plic, i);
}

ശൂന്യം plic_resume(ശൂന്യം)
{
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	സ്ഥിരം ഘടന plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 ie_വാക്കുകൾ = PLIC_IE_വാക്കുകൾ(plic);
	u32 *data_word = plic->pm_data;
	u8 *data_byte;

	എങ്കിൽ (!data_word)
		തിരികെ;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		എങ്കിൽ (context_id < 0)
			തുടരുക;

		/* Restore the enable bits */
		വേണ്ടി (u32 i = 0; i < ie_വാക്കുകൾ; i++)
			plic_set_ie(plic, context_id, i, *data_word++);

		/* Restore the context threshold */
		plic_set_thresh(plic, context_id, *data_word++);
	}

	/* Restore the input priorities */
	data_byte = (u8 *)data_word;
	വേണ്ടി (u32 i = 1; i <= plic->അക്കം_src; i++)
		plic_set_priority(plic, i, *data_byte++);

	/* Restore the delegation */
	plic_delegate(plic);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ plic_warm_irqchip_init(ഘടന sbi_irqchip_device *dev)
{
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	സ്ഥിരം ഘടന plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 hartindex = current_hartindex();
	s16 m_cntx_id = plic->context_map[hartindex][PLIC_M_CONTEXT];
	s16 s_cntx_id = plic->context_map[hartindex][PLIC_S_CONTEXT];
	bool enable;
	പൂർണ്ണസംഖ്യ ret;

	/*
	 * By default, disable all IRQs for the target HART. Ariane
	 * has a bug which requires enabling all interrupts at boot.
	 */
	enable = plic->flags & PLIC_FLAG_ARIANE_BUG;

	എങ്കിൽ (m_cntx_id > -1) {
		ret = plic_context_init(plic, m_cntx_id, enable, 0x7);
		എങ്കിൽ (ret)
			തിരികെ ret;
	}

	എങ്കിൽ (s_cntx_id > -1) {
		ret = plic_context_init(plic, s_cntx_id, enable, 0x7);
		എങ്കിൽ (ret)
			തിരികെ ret;
	}

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ plic_cold_irqchip_init(ഘടന plic_data *plic)
{
	പൂർണ്ണസംഖ്യ i, ret;

	എങ്കിൽ (!plic)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (!plic_ptr_offset) {
		plic_ptr_offset = sbi_scratch_alloc_type_offset(ശൂന്യം *);
		എങ്കിൽ (!plic_ptr_offset)
			തിരികെ SBI_ENOMEM;
	}

	എങ്കിൽ (plic->flags & PLIC_FLAG_ENABLE_PM) {
		unsigned long data_size = 0;

		sbi_for_each_hartindex(i) {
			എങ്കിൽ (plic->context_map[i][PLIC_S_CONTEXT] < 0)
				തുടരുക;

			/* Allocate space for enable bits */
			data_size += (plic->അക്കം_src / 32 + 1) * വലുപ്പം(u32);

			/* Allocate space for the context threshold */
			data_size += വലുപ്പം(u32);
		}

		/*
		 * Allocate space for the input priorities. So far,
		 * priorities on all known implementations fit in 8 bits.
		 */
		data_size += plic->അക്കം_src * വലുപ്പം(u8);

		plic->pm_data = sbi_malloc(data_size);
		എങ്കിൽ (!plic->pm_data)
			തിരികെ SBI_ENOMEM;
	}

	/* Configure default priorities of all IRQs */
	വേണ്ടി (i = 1; i <= plic->അക്കം_src; i++)
		plic_set_priority(plic, i, 0);

	plic_delegate(plic);

	ret = sbi_domain_വേര്_add_memrange(plic->addr, plic->size, BIT(20),
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	എങ്കിൽ (ret)
		തിരികെ ret;

	sbi_for_each_hartindex(i) {
		എങ്കിൽ (plic->context_map[i][PLIC_M_CONTEXT] < 0 &&
		    plic->context_map[i][PLIC_S_CONTEXT] < 0)
			തുടരുക;

		plic_set_hart_data_ptr(sbi_hartindex_to_scratch(i), plic);
		sbi_hartmask_set_hartindex(i, &plic->irqchip.target_harts);
	}

	/* Register irqchip device */
	plic->irqchip.id = plic->unique_id;
	plic->irqchip.അക്കം_hwirq = plic->അക്കം_src + 1;
	plic->irqchip.warm_init = plic_warm_irqchip_init;
	തിരികെ sbi_irqchip_add_device(&plic->irqchip);
}
