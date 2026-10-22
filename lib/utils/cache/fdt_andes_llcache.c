/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi_utils/cache/fdt_cache.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>
#अन्तर्भाव <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#परिभाषय LLCACHE_REG_CFG_विस्थापनम्		0x0
#परिभाषय LLCACHE_REG_CTRL_विस्थापनम्		0x8
#परिभाषय LLCACHE_REG_CCTL_CMD_विस्थापनम्	0x40
#परिभाषय LLCACHE_REG_CCTL_स्थितिः_विस्थापनम्	0x80

#परिभाषय LLCACHE_REG_CFG_MAP_आवरणम्	BIT(20)
#परिभाषय LLCACHE_REG_CTRL_EN_आवरणम्	BIT(0)
#परिभाषय LLCACHE_REG_CTRL_INIT_आवरणम्	BIT(14)
#परिभाषय LLCACHE_REG_CCTL_स्थितिः_आवरणम्	GENMASK(3, 0)

#परिभाषय LLCACHE_WBINVAL_ALL		0x12

संरचना andes_llcache {
	संरचना cache_device dev;
	शून्य *आधारः_2;
	uint32_t cmd_stride;
	uint32_t स्थितिः_stride;
	uint32_t स्थितिः_core_stride;
};

#परिभाषय गन्तव्यम्_llcache(_dev) container_सम्बन्धि(_dev, संरचना andes_llcache, dev)

स्थिर bool andes_llcache_init_समाप्तम्(संरचना andes_llcache *llcache)
{
	uint32_t llcache_ctrl;
	शून्य *ctrl_addr = (वर्ण *)llcache->आधारः_2 + LLCACHE_REG_CTRL_विस्थापनम्;

	llcache_ctrl = readl_relaxed(ctrl_addr);
	प्रत्यागम !EXTRACT_क्षेत्रम्(llcache_ctrl, LLCACHE_REG_CTRL_INIT_आवरणम्);
}

स्थिर bool andes_llcache_cctl_समाप्तम्(संरचना andes_llcache *llcache, uint32_t hartid)
{
	uint32_t llcache_cctl_स्थितिः;
	शून्य *cctl_स्थितिः_addr = (वर्ण *)llcache->आधारः_2 + LLCACHE_REG_CCTL_स्थितिः_विस्थापनम् +
				 hartid * llcache->स्थितिः_stride;

	llcache_cctl_स्थितिः = readl_relaxed(cctl_स्थितिः_addr);
	प्रत्यागम !EXTRACT_क्षेत्रम्(llcache_cctl_स्थितिः,
			      LLCACHE_REG_CCTL_स्थितिः_आवरणम् <<
			      hartid * llcache->स्थितिः_core_stride);
}

स्थिर पूर्णाङ्क andes_llcache_flush_all(संरचना cache_device *dev)
{
	uint32_t hartid = current_hartid();
	संरचना andes_llcache *llcache = गन्तव्यम्_llcache(dev);
	शून्य *cctl_cmd_addr = (वर्ण *)llcache->आधारः_2 + LLCACHE_REG_CCTL_CMD_विस्थापनम् +
			      hartid * llcache->cmd_stride;

	/*
	 * Each command register corresponds to one CPU core, so each CPU core
	 * should only use its command registers to do the cache operation.
	 */
	writel(LLCACHE_WBINVAL_ALL, cctl_cmd_addr);

	/* Wait for the command completion */
	यावत् (!andes_llcache_cctl_समाप्तम्(llcache, hartid))
		;

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क andes_llcache_enable(संरचना cache_device *dev, bool enable)
{
	संरचना andes_llcache *llcache = गन्तव्यम्_llcache(dev);
	u32 llcache_ctrl;
	शून्य *ctrl_addr = (वर्ण *)llcache->आधारः_2 + LLCACHE_REG_CTRL_विस्थापनम्;

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
	यदि (enable) {
		llcache_ctrl = atcsmu_read_scratch();
		writel(llcache_ctrl, ctrl_addr);
		writel(llcache_ctrl | LLCACHE_REG_CTRL_EN_आवरणम्, ctrl_addr);
	} अन्यथा {
		llcache_ctrl = readl(ctrl_addr);
		atcsmu_write_scratch(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_आवरणम्);
		writel(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_आवरणम्, ctrl_addr);
	}

	llcache_ctrl = readl(ctrl_addr);
	यदि (enable != EXTRACT_क्षेत्रम्(llcache_ctrl, LLCACHE_REG_CTRL_EN_आवरणम्))
		प्रत्यागम SBI_EFAIL;

	प्रत्यागम SBI_OK;
}

स्थिर संरचना cache_ops andes_llcache_ops = {
	.cache_flush_all = andes_llcache_flush_all,
	.cache_enable = andes_llcache_enable,
};

स्थिर पूर्णाङ्क andes_llcache_probe(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff, ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	पूर्णाङ्क rc;
	u64 llcache_आधारः = 0, आकार;
	संरचना andes_llcache *llcache;
	संरचना cache_device *dev;
	uint32_t llcache_cfg;

	rc = fdt_get_ग्रन्थिः_addr_आकार(fdt, nodeoff, 0, &llcache_आधारः, &आकार);
	यदि (rc < 0 || !llcache_आधारः || !आकार)
		प्रत्यागम SBI_ENODEV;

	llcache = sbi_zalloc(परिमाण(*llcache));
	यदि (!llcache)
		प्रत्यागम SBI_ENOMEM;

	dev = &llcache->dev;
	dev->ops = &andes_llcache_ops;
	rc = fdt_cache_सङ्कलय(fdt, nodeoff, dev);
	यदि (rc) {
		sbi_free(llcache);
		प्रत्यागम rc;
	}

	llcache->आधारः_2 = (शून्य *)(ulong)llcache_आधारः;
	llcache_cfg = readl_relaxed((वर्ण *)llcache->आधारः_2 + LLCACHE_REG_CFG_विस्थापनम्);

	/* Configurations for V1/V0 memory map */
	यदि (EXTRACT_क्षेत्रम्(llcache_cfg, LLCACHE_REG_CFG_MAP_आवरणम्)) {
		llcache->cmd_stride = 0x1000;
		llcache->स्थितिः_stride = 0x1000;
		llcache->स्थितिः_core_stride = 0;
	} अन्यथा {
		llcache->cmd_stride = 0x10;
		llcache->स्थितिः_stride = 0x0;
		llcache->स्थितिः_core_stride = 4;
	}

	यदि (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_मूलम्_सङ्कलय_memrange(
			(अचिह्नित दीर्घ)llcache->आधारः_2, (अचिह्नित दीर्घ)आकार, PAGE_आकार,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		यदि (rc)
			प्रत्यागम rc;
	}

	/* Wait for the hardware initialization done */
	यावत् (!andes_llcache_init_समाप्तम्(llcache))
		;

	प्रत्यागम SBI_OK;
}

स्थिर ध्रुव संरचना fdt_मेलनम् andes_llcache_मेलनम्[] = {
	{ .compatible = "andestech,llcache" },
	{},
};

ध्रुव संरचना fdt_driver fdt_andes_llcache = {
	.मेलनम्_सारणी = andes_llcache_मेलनम्,
	.init = andes_llcache_probe,
};
