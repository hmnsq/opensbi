/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Samuel Holland <samuel@sholland.org>
 */

#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/riscv_encoding.h>
#అన్తర్భావ <sbi/sbi_bitops.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi/sbi_string.h>
#అన్తర్భావ <sbi_utils/irqchip/plic.h>

#నిర్వచించు PLIC_ప్రాధాన్యత_పునాది 0x0
#నిర్వచించు PLIC_PENDING_పునాది 0x1000
#నిర్వచించు PLIC_ENABLE_పునాది 0x2000
#నిర్వచించు PLIC_ENABLE_STRIDE 0x80
#నిర్వచించు PLIC_CONTEXT_పునాది 0x200000
#నిర్వచించు PLIC_CONTEXT_STRIDE 0x1000

#నిర్వచించు THEAD_PLIC_CTRL_REG 0x1ffffc

స్థిర గుర్తులేని దీర్ఘ plic_ptr_స్థానభ్రంశం;

#నిర్వచించు plic_get_hart_దత్తాంశం_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), శూన్య *, plic_ptr_స్థానభ్రంశం)

#నిర్వచించు plic_సమితి_hart_దత్తాంశం_ptr(__scratch, __plic)			\
	sbi_scratch_write_type((__scratch), శూన్య *, plic_ptr_స్థానభ్రంశం, (__plic))

నిర్మాణం plic_దత్తాంశం *plic_get(శూన్య)
{
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	తిరిగి plic_get_hart_దత్తాంశం_ptr(scratch);
}

స్థిర u32 plic_get_ప్రాధాన్యత(స్థిరరాశి నిర్మాణం plic_దత్తాంశం *plic, u32 ఆధారం)
{
	అస్థిర శూన్య *plic_ప్రాధాన్యత = (అక్షరం *)plic->addr +
			PLIC_ప్రాధాన్యత_పునాది + 4 * ఆధారం;
	తిరిగి readl(plic_ప్రాధాన్యత);
}

స్థిర శూన్య plic_సమితి_ప్రాధాన్యత(స్థిరరాశి నిర్మాణం plic_దత్తాంశం *plic, u32 ఆధారం, u32 val)
{
	అస్థిర శూన్య *plic_ప్రాధాన్యత = (అక్షరం *)plic->addr +
			PLIC_ప్రాధాన్యత_పునాది + 4 * ఆధారం;
	writel(val, plic_ప్రాధాన్యత);
}

స్థిర u32 plic_get_thresh(స్థిరరాశి నిర్మాణం plic_దత్తాంశం *plic, u32 cntxid)
{
	అస్థిర శూన్య *plic_thresh;

	plic_thresh = (అక్షరం *)plic->addr +
		      PLIC_CONTEXT_పునాది + PLIC_CONTEXT_STRIDE * cntxid;

	తిరిగి readl(plic_thresh);
}

స్థిర శూన్య plic_సమితి_thresh(స్థిరరాశి నిర్మాణం plic_దత్తాంశం *plic, u32 cntxid, u32 val)
{
	అస్థిర శూన్య *plic_thresh;

	plic_thresh = (అక్షరం *)plic->addr +
		      PLIC_CONTEXT_పునాది + PLIC_CONTEXT_STRIDE * cntxid;
	writel(val, plic_thresh);
}

స్థిర u32 plic_get_ie(స్థిరరాశి నిర్మాణం plic_దత్తాంశం *plic, u32 cntxid,
		       u32 మాట_సూచీ)
{
	అస్థిర శూన్య *plic_ie;

	plic_ie = (అక్షరం *)plic->addr +
		   PLIC_ENABLE_పునాది + PLIC_ENABLE_STRIDE * cntxid +
		   4 * మాట_సూచీ;

	తిరిగి readl(plic_ie);
}

స్థిర శూన్య plic_సమితి_ie(స్థిరరాశి నిర్మాణం plic_దత్తాంశం *plic, u32 cntxid,
			u32 మాట_సూచీ, u32 val)
{
	అస్థిర శూన్య *plic_ie;

	plic_ie = (అక్షరం *)plic->addr +
		   PLIC_ENABLE_పునాది + PLIC_ENABLE_STRIDE * cntxid +
		   4 * మాట_సూచీ;
	writel(val, plic_ie);
}

స్థిర శూన్య plic_delegate(స్థిరరాశి నిర్మాణం plic_దత్తాంశం *plic)
{
	/* If this is a T-HEAD PLIC, delegate access to S-mode */
	ఒకవేళ (plic->flags & PLIC_FLAG_THEAD_DELEGATION)
		writel_relaxed(బిట్టు(0), (అక్షరం *)plic->addr + THEAD_PLIC_CTRL_REG);
}

స్థిర పూర్ణాంకం plic_context_init(స్థిరరాశి నిర్మాణం plic_దత్తాంశం *plic, పూర్ణాంకం context_id,
			     bool enable, u32 పరిమితి)
{
	u32 ie_పదాలు, ie_విలువ;

	ఒకవేళ (!plic || context_id < 0)
		తిరిగి SBI_EINVAL;

	ie_పదాలు = PLIC_IE_పదాలు(plic);
	ie_విలువ = enable ? 0xffffffffU : 0U;

	కొరకు (u32 i = 0; i < ie_పదాలు; i++)
		plic_సమితి_ie(plic, context_id, i, ie_విలువ);

	plic_సమితి_thresh(plic, context_id, పరిమితి);

	తిరిగి 0;
}

శూన్య plic_suspend(శూన్య)
{
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	స్థిరరాశి నిర్మాణం plic_దత్తాంశం *plic = plic_get_hart_దత్తాంశం_ptr(scratch);
	u32 ie_పదాలు = PLIC_IE_పదాలు(plic);
	u32 *దత్తాంశం_మాట = plic->pm_దత్తాంశం;
	u8 *దత్తాంశం_byte;

	ఒకవేళ (!దత్తాంశం_మాట)
		తిరిగి;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		ఒకవేళ (context_id < 0)
			కొనసాగు;

		/* Save the enable bits */
		కొరకు (u32 i = 0; i < ie_పదాలు; i++)
			*దత్తాంశం_మాట++ = plic_get_ie(plic, context_id, i);

		/* Save the context threshold */
		*దత్తాంశం_మాట++ = plic_get_thresh(plic, context_id);
	}

	/* Restore the input priorities */
	దత్తాంశం_byte = (u8 *)దత్తాంశం_మాట;
	కొరకు (u32 i = 1; i <= plic->అంకె_src; i++)
		*దత్తాంశం_byte++ = plic_get_ప్రాధాన్యత(plic, i);
}

శూన్య plic_resume(శూన్య)
{
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	స్థిరరాశి నిర్మాణం plic_దత్తాంశం *plic = plic_get_hart_దత్తాంశం_ptr(scratch);
	u32 ie_పదాలు = PLIC_IE_పదాలు(plic);
	u32 *దత్తాంశం_మాట = plic->pm_దత్తాంశం;
	u8 *దత్తాంశం_byte;

	ఒకవేళ (!దత్తాంశం_మాట)
		తిరిగి;

	sbi_for_each_hartindex(h) {
		s16 context_id = plic->context_map[h][PLIC_S_CONTEXT];

		ఒకవేళ (context_id < 0)
			కొనసాగు;

		/* Restore the enable bits */
		కొరకు (u32 i = 0; i < ie_పదాలు; i++)
			plic_సమితి_ie(plic, context_id, i, *దత్తాంశం_మాట++);

		/* Restore the context threshold */
		plic_సమితి_thresh(plic, context_id, *దత్తాంశం_మాట++);
	}

	/* Restore the input priorities */
	దత్తాంశం_byte = (u8 *)దత్తాంశం_మాట;
	కొరకు (u32 i = 1; i <= plic->అంకె_src; i++)
		plic_సమితి_ప్రాధాన్యత(plic, i, *దత్తాంశం_byte++);

	/* Restore the delegation */
	plic_delegate(plic);
}

స్థిర పూర్ణాంకం plic_warm_irqchip_init(నిర్మాణం sbi_irqchip_device *dev)
{
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	స్థిరరాశి నిర్మాణం plic_దత్తాంశం *plic = plic_get_hart_దత్తాంశం_ptr(scratch);
	u32 hartindex = current_hartindex();
	s16 m_cntx_id = plic->context_map[hartindex][PLIC_M_CONTEXT];
	s16 s_cntx_id = plic->context_map[hartindex][PLIC_S_CONTEXT];
	bool enable;
	పూర్ణాంకం ret;

	/*
	 * By default, disable all IRQs for the target HART. Ariane
	 * has a bug which requires enabling all interrupts at boot.
	 */
	enable = plic->flags & PLIC_FLAG_ARIANE_BUG;

	ఒకవేళ (m_cntx_id > -1) {
		ret = plic_context_init(plic, m_cntx_id, enable, 0x7);
		ఒకవేళ (ret)
			తిరిగి ret;
	}

	ఒకవేళ (s_cntx_id > -1) {
		ret = plic_context_init(plic, s_cntx_id, enable, 0x7);
		ఒకవేళ (ret)
			తిరిగి ret;
	}

	తిరిగి 0;
}

పూర్ణాంకం plic_cold_irqchip_init(నిర్మాణం plic_దత్తాంశం *plic)
{
	పూర్ణాంకం i, ret;

	ఒకవేళ (!plic)
		తిరిగి SBI_EINVAL;

	ఒకవేళ (!plic_ptr_స్థానభ్రంశం) {
		plic_ptr_స్థానభ్రంశం = sbi_scratch_alloc_type_స్థానభ్రంశం(శూన్య *);
		ఒకవేళ (!plic_ptr_స్థానభ్రంశం)
			తిరిగి SBI_ENOMEM;
	}

	ఒకవేళ (plic->flags & PLIC_FLAG_ENABLE_PM) {
		గుర్తులేని దీర్ఘ దత్తాంశం_కొలత_2 = 0;

		sbi_for_each_hartindex(i) {
			ఒకవేళ (plic->context_map[i][PLIC_S_CONTEXT] < 0)
				కొనసాగు;

			/* Allocate space for enable bits */
			దత్తాంశం_కొలత_2 += (plic->అంకె_src / 32 + 1) * పరిమాణం(u32);

			/* Allocate space for the context threshold */
			దత్తాంశం_కొలత_2 += పరిమాణం(u32);
		}

		/*
		 * Allocate space for the input priorities. So far,
		 * priorities on all known implementations fit in 8 bits.
		 */
		దత్తాంశం_కొలత_2 += plic->అంకె_src * పరిమాణం(u8);

		plic->pm_దత్తాంశం = sbi_malloc(దత్తాంశం_కొలత_2);
		ఒకవేళ (!plic->pm_దత్తాంశం)
			తిరిగి SBI_ENOMEM;
	}

	/* Configure default priorities of all IRQs */
	కొరకు (i = 1; i <= plic->అంకె_src; i++)
		plic_సమితి_ప్రాధాన్యత(plic, i, 0);

	plic_delegate(plic);

	ret = sbi_domain_మూలం_కూడు_memrange(plic->addr, plic->కొలత, బిట్టు(20),
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	ఒకవేళ (ret)
		తిరిగి ret;

	sbi_for_each_hartindex(i) {
		ఒకవేళ (plic->context_map[i][PLIC_M_CONTEXT] < 0 &&
		    plic->context_map[i][PLIC_S_CONTEXT] < 0)
			కొనసాగు;

		plic_సమితి_hart_దత్తాంశం_ptr(sbi_hartindex_గమ్యం_scratch(i), plic);
		sbi_hartmask_సమితి_hartindex(i, &plic->irqchip.లక్ష్యం_harts);
	}

	/* Register irqchip device */
	plic->irqchip.id = plic->unique_id;
	plic->irqchip.అంకె_hwirq = plic->అంకె_src + 1;
	plic->irqchip.warm_init = plic_warm_irqchip_init;
	తిరిగి sbi_irqchip_కూడు_device(&plic->irqchip);
}
