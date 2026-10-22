/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Samuel Holland <samuel@sholland.org>
 */

#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/riscv_encoding.h>
#अन्तर्भाव <sbi/sbi_bitops.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_string.h>
#अन्तर्भाव <sbi_utils/irqchip/plic.h>

#define PLIC_PRIORITY_BASE 0x0
#define PLIC_PENDING_BASE 0x1000
#define PLIC_ENABLE_BASE 0x2000
#define PLIC_ENABLE_STRIDE 0x80
#define PLIC_CONTEXT_BASE 0x200000
#define PLIC_CONTEXT_STRIDE 0x1000

#define THEAD_PLIC_CTRL_REG 0x1ffffc

स्थिर अचिन्हित दीर्घ plic_ptr_offset;

#define plic_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), शून्य *, plic_ptr_offset)

#define plic_set_hart_data_ptr(__scratch, __plic)			\
	sbi_scratch_write_type((__scratch), शून्य *, plic_ptr_offset, (__plic))

संरचना plic_data *plic_get(शून्य)
{
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	परत plic_get_hart_data_ptr(scratch);
}

स्थिर u32 plic_get_priority(स्थिरांक संरचना plic_data *plic, u32 source)
{
	अस्थिर शून्य *plic_priority = (अक्षर *)plic->addr +
			PLIC_PRIORITY_BASE + 4 * source;
	परत readl(plic_priority);
}

स्थिर शून्य plic_set_priority(स्थिरांक संरचना plic_data *plic, u32 source, u32 val)
{
	अस्थिर शून्य *plic_priority = (अक्षर *)plic->addr +
			PLIC_PRIORITY_BASE + 4 * source;
	writel(val, plic_priority);
}

स्थिर u32 plic_get_thresh(स्थिरांक संरचना plic_data *plic, u32 cntxid)
{
	अस्थिर शून्य *plic_thresh;

	plic_thresh = (अक्षर *)plic->addr +
		      PLIC_CONTEXT_BASE + PLIC_CONTEXT_STRIDE * cntxid;

	परत readl(plic_thresh);
}

स्थिर शून्य plic_set_thresh(स्थिरांक संरचना plic_data *plic, u32 cntxid, u32 val)
{
	अस्थिर शून्य *plic_thresh;

	plic_thresh = (अक्षर *)plic->addr +
		      PLIC_CONTEXT_BASE + PLIC_CONTEXT_STRIDE * cntxid;
	writel(val, plic_thresh);
}

स्थिर u32 plic_get_ie(स्थिरांक संरचना plic_data *plic, u32 cntxid,
		       u32 word_index)
{
	अस्थिर शून्य *plic_ie;

	plic_ie = (अक्षर *)plic->addr +
		   PLIC_ENABLE_BASE + PLIC_ENABLE_STRIDE * cntxid +
		   4 * word_index;

	परत readl(plic_ie);
}

स्थिर शून्य plic_set_ie(स्थिरांक संरचना plic_data *plic, u32 cntxid,
			u32 word_index, u32 val)
{
	अस्थिर शून्य *plic_ie;

	plic_ie = (अक्षर *)plic->addr +
		   PLIC_ENABLE_BASE + PLIC_ENABLE_STRIDE * cntxid +
		   4 * word_index;
	writel(val, plic_ie);
}

स्थिर शून्य plic_delegate(स्थिरांक संरचना plic_data *plic)
{
	/* If this is a T-HEAD PLIC, delegate access to S-mode */
	जर (plic->flags & PLIC_FLAG_THEAD_DELEGATION)
		writel_relaxed(BIT(0), (अक्षर *)plic->addr + THEAD_PLIC_CTRL_REG);
}

स्थिर पूर्णांक plic_context_init(स्थिरांक संरचना plic_data *plic, पूर्णांक context_id,
			     bool enable, u32 threshold)
{
	u32 ie_शब्द, ie_मूल्य;

	जर (!plic || context_id < 0)
		परत SBI_EINVAL;

	ie_शब्द = PLIC_IE_शब्द(plic);
	ie_मूल्य = enable ? 0xffffffffU : 0U;

	साठी (u32 i = 0; i < ie_शब्द; i++)
		plic_set_ie(plic, context_id, i, ie_मूल्य);

	plic_set_thresh(plic, context_id, threshold);

	परत 0;
}

शून्य plic_suspend(शून्य)
{
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	स्थिरांक संरचना plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 ie_शब्द = PLIC_IE_शब्द(plic);
	u32 *data_word = plic->pm_data;
	u8 *data_byte;

	जर (!data_word)
		परत;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		जर (context_id < 0)
			सुरूठेवा;

		/* Save the enable bits */
		साठी (u32 i = 0; i < ie_शब्द; i++)
			*data_word++ = plic_get_ie(plic, context_id, i);

		/* Save the context threshold */
		*data_word++ = plic_get_thresh(plic, context_id);
	}

	/* Restore the input priorities */
	data_byte = (u8 *)data_word;
	साठी (u32 i = 1; i <= plic->अंक_src; i++)
		*data_byte++ = plic_get_priority(plic, i);
}

शून्य plic_resume(शून्य)
{
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	स्थिरांक संरचना plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 ie_शब्द = PLIC_IE_शब्द(plic);
	u32 *data_word = plic->pm_data;
	u8 *data_byte;

	जर (!data_word)
		परत;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		जर (context_id < 0)
			सुरूठेवा;

		/* Restore the enable bits */
		साठी (u32 i = 0; i < ie_शब्द; i++)
			plic_set_ie(plic, context_id, i, *data_word++);

		/* Restore the context threshold */
		plic_set_thresh(plic, context_id, *data_word++);
	}

	/* Restore the input priorities */
	data_byte = (u8 *)data_word;
	साठी (u32 i = 1; i <= plic->अंक_src; i++)
		plic_set_priority(plic, i, *data_byte++);

	/* Restore the delegation */
	plic_delegate(plic);
}

स्थिर पूर्णांक plic_warm_irqchip_init(संरचना sbi_irqchip_device *dev)
{
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	स्थिरांक संरचना plic_data *plic = plic_get_hart_data_ptr(scratch);
	u32 hartindex = current_hartindex();
	s16 m_cntx_id = plic->context_map[hartindex][PLIC_M_CONTEXT];
	s16 s_cntx_id = plic->context_map[hartindex][PLIC_S_CONTEXT];
	bool enable;
	पूर्णांक ret;

	/*
	 * By default, disable all IRQs for the target HART. Ariane
	 * has a bug which requires enabling all interrupts at boot.
	 */
	enable = plic->flags & PLIC_FLAG_ARIANE_BUG;

	जर (m_cntx_id > -1) {
		ret = plic_context_init(plic, m_cntx_id, enable, 0x7);
		जर (ret)
			परत ret;
	}

	जर (s_cntx_id > -1) {
		ret = plic_context_init(plic, s_cntx_id, enable, 0x7);
		जर (ret)
			परत ret;
	}

	परत 0;
}

पूर्णांक plic_cold_irqchip_init(संरचना plic_data *plic)
{
	पूर्णांक i, ret;

	जर (!plic)
		परत SBI_EINVAL;

	जर (!plic_ptr_offset) {
		plic_ptr_offset = sbi_scratch_alloc_type_offset(शून्य *);
		जर (!plic_ptr_offset)
			परत SBI_ENOMEM;
	}

	जर (plic->flags & PLIC_FLAG_ENABLE_PM) {
		अचिन्हित दीर्घ data_size = 0;

		sbi_for_each_hartindex(i) {
			जर (plic->context_map[i][PLIC_S_CONTEXT] < 0)
				सुरूठेवा;

			/* Allocate space for enable bits */
			data_size += (plic->अंक_src / 32 + 1) * आकार(u32);

			/* Allocate space for the context threshold */
			data_size += आकार(u32);
		}

		/*
		 * Allocate space for the input priorities. So far,
		 * priorities on all known implementations fit in 8 bits.
		 */
		data_size += plic->अंक_src * आकार(u8);

		plic->pm_data = sbi_malloc(data_size);
		जर (!plic->pm_data)
			परत SBI_ENOMEM;
	}

	/* Configure default priorities of all IRQs */
	साठी (i = 1; i <= plic->अंक_src; i++)
		plic_set_priority(plic, i, 0);

	plic_delegate(plic);

	ret = sbi_domain_मूळ_add_memrange(plic->addr, plic->size, BIT(20),
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	जर (ret)
		परत ret;

	sbi_for_each_hartindex(i) {
		जर (plic->context_map[i][PLIC_M_CONTEXT] < 0 &&
		    plic->context_map[i][PLIC_S_CONTEXT] < 0)
			सुरूठेवा;

		plic_set_hart_data_ptr(sbi_hartindex_to_scratch(i), plic);
		sbi_hartmask_set_hartindex(i, &plic->irqchip.target_harts);
	}

	/* Register irqchip device */
	plic->irqchip.id = plic->unique_id;
	plic->irqchip.अंक_hwirq = plic->अंक_src + 1;
	plic->irqchip.warm_init = plic_warm_irqchip_init;
	परत sbi_irqchip_add_device(&plic->irqchip);
}
