/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi_utils/cache/fdt_cache.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_driver.h>
#అన్తర్భావ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#నిర్వచించు LLCACHE_REG_CFG_స్థానభ్రంశం		0x0
#నిర్వచించు LLCACHE_REG_CTRL_స్థానభ్రంశం		0x8
#నిర్వచించు LLCACHE_REG_CCTL_CMD_స్థానభ్రంశం	0x40
#నిర్వచించు LLCACHE_REG_CCTL_స్థితి_స్థానభ్రంశం	0x80

#నిర్వచించు LLCACHE_REG_CFG_MAP_ముసుగు	బిట్టు(20)
#నిర్వచించు LLCACHE_REG_CTRL_EN_ముసుగు	బిట్టు(0)
#నిర్వచించు LLCACHE_REG_CTRL_INIT_ముసుగు	బిట్టు(14)
#నిర్వచించు LLCACHE_REG_CCTL_స్థితి_ముసుగు	GENMASK(3, 0)

#నిర్వచించు LLCACHE_WBINVAL_ALL		0x12

నిర్మాణం andes_llcache {
	నిర్మాణం cache_device dev;
	శూన్య *పునాది_2;
	uint32_t cmd_stride;
	uint32_t స్థితి_stride;
	uint32_t స్థితి_core_stride;
};

#నిర్వచించు గమ్యం_llcache(_dev) container_యొక్క(_dev, నిర్మాణం andes_llcache, dev)

స్థిర bool andes_llcache_init_పూర్తి(నిర్మాణం andes_llcache *llcache)
{
	uint32_t llcache_ctrl;
	శూన్య *ctrl_addr = (అక్షరం *)llcache->పునాది_2 + LLCACHE_REG_CTRL_స్థానభ్రంశం;

	llcache_ctrl = readl_relaxed(ctrl_addr);
	తిరిగి !వెలికితీయి_క్షేత్రం(llcache_ctrl, LLCACHE_REG_CTRL_INIT_ముసుగు);
}

స్థిర bool andes_llcache_cctl_పూర్తి(నిర్మాణం andes_llcache *llcache, uint32_t hartid)
{
	uint32_t llcache_cctl_స్థితి;
	శూన్య *cctl_స్థితి_addr = (అక్షరం *)llcache->పునాది_2 + LLCACHE_REG_CCTL_స్థితి_స్థానభ్రంశం +
				 hartid * llcache->స్థితి_stride;

	llcache_cctl_స్థితి = readl_relaxed(cctl_స్థితి_addr);
	తిరిగి !వెలికితీయి_క్షేత్రం(llcache_cctl_స్థితి,
			      LLCACHE_REG_CCTL_స్థితి_ముసుగు <<
			      hartid * llcache->స్థితి_core_stride);
}

స్థిర పూర్ణాంకం andes_llcache_flush_all(నిర్మాణం cache_device *dev)
{
	uint32_t hartid = current_hartid();
	నిర్మాణం andes_llcache *llcache = గమ్యం_llcache(dev);
	శూన్య *cctl_cmd_addr = (అక్షరం *)llcache->పునాది_2 + LLCACHE_REG_CCTL_CMD_స్థానభ్రంశం +
			      hartid * llcache->cmd_stride;

	/*
	 * Each command register corresponds to one CPU core, so each CPU core
	 * should only use its command registers to do the cache operation.
	 */
	writel(LLCACHE_WBINVAL_ALL, cctl_cmd_addr);

	/* Wait for the command completion */
	అయితే (!andes_llcache_cctl_పూర్తి(llcache, hartid))
		;

	తిరిగి 0;
}

స్థిర పూర్ణాంకం andes_llcache_enable(నిర్మాణం cache_device *dev, bool enable)
{
	నిర్మాణం andes_llcache *llcache = గమ్యం_llcache(dev);
	u32 llcache_ctrl;
	శూన్య *ctrl_addr = (అక్షరం *)llcache->పునాది_2 + LLCACHE_REG_CTRL_స్థానభ్రంశం;

	/*
	 * To properly enable the last level cache to cache both instructions
	 * and data, apply the following sequence:
	 *
	 * - Write the control register with the desired value, except the
	 *   CEN field should be set to zero. Thus, store the control register
	 *   value with the CEN field being 0 when disabling the last level
	 *   cache.
	 * - Write the control register again using the same value of step 1
	 *   with the CEN field being 1.
	 */
	ఒకవేళ (enable) {
		llcache_ctrl = atcsmu_read_scratch();
		writel(llcache_ctrl, ctrl_addr);
		writel(llcache_ctrl | LLCACHE_REG_CTRL_EN_ముసుగు, ctrl_addr);
	} లేకపోతే {
		llcache_ctrl = readl(ctrl_addr);
		atcsmu_write_scratch(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_ముసుగు);
		writel(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_ముసుగు, ctrl_addr);
	}

	llcache_ctrl = readl(ctrl_addr);
	ఒకవేళ (enable != వెలికితీయి_క్షేత్రం(llcache_ctrl, LLCACHE_REG_CTRL_EN_ముసుగు))
		తిరిగి SBI_EFAIL;

	తిరిగి SBI_OK;
}

స్థిర నిర్మాణం cache_ops andes_llcache_ops = {
	.cache_flush_all = andes_llcache_flush_all,
	.cache_enable = andes_llcache_enable,
};

స్థిర పూర్ణాంకం andes_llcache_probe(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff, స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం rc;
	u64 llcache_పునాది = 0, కొలత;
	నిర్మాణం andes_llcache *llcache;
	నిర్మాణం cache_device *dev;
	uint32_t llcache_cfg;

	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, 0, &llcache_పునాది, &కొలత);
	ఒకవేళ (rc < 0 || !llcache_పునాది || !కొలత)
		తిరిగి SBI_ENODEV;

	llcache = sbi_zalloc(పరిమాణం(*llcache));
	ఒకవేళ (!llcache)
		తిరిగి SBI_ENOMEM;

	dev = &llcache->dev;
	dev->ops = &andes_llcache_ops;
	rc = fdt_cache_కూడు(fdt, nodeoff, dev);
	ఒకవేళ (rc) {
		sbi_free(llcache);
		తిరిగి rc;
	}

	llcache->పునాది_2 = (శూన్య *)(ulong)llcache_పునాది;
	llcache_cfg = readl_relaxed((అక్షరం *)llcache->పునాది_2 + LLCACHE_REG_CFG_స్థానభ్రంశం);

	/* Configurations for V1/V0 memory map */
	ఒకవేళ (వెలికితీయి_క్షేత్రం(llcache_cfg, LLCACHE_REG_CFG_MAP_ముసుగు)) {
		llcache->cmd_stride = 0x1000;
		llcache->స్థితి_stride = 0x1000;
		llcache->స్థితి_core_stride = 0;
	} లేకపోతే {
		llcache->cmd_stride = 0x10;
		llcache->స్థితి_stride = 0x0;
		llcache->స్థితి_core_stride = 4;
	}

	ఒకవేళ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_మూలం_కూడు_memrange(
			(గుర్తులేని దీర్ఘ)llcache->పునాది_2, (గుర్తులేని దీర్ఘ)కొలత, పుట_కొలత,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		ఒకవేళ (rc)
			తిరిగి rc;
	}

	/* Wait for the hardware initialization done */
	అయితే (!andes_llcache_init_పూర్తి(llcache))
		;

	తిరిగి SBI_OK;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక andes_llcache_సరిపోలిక[] = {
	{ .compatible = "andestech,llcache" },
	{},
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_andes_llcache = {
	.సరిపోలిక_పట్టిక = andes_llcache_సరిపోలిక,
	.init = andes_llcache_probe,
};
