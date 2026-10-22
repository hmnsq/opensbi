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

#define LLCACHE_REG_CFG_OFFSET		0x0
#define LLCACHE_REG_CTRL_OFFSET		0x8
#define LLCACHE_REG_CCTL_CMD_OFFSET	0x40
#define LLCACHE_REG_CCTL_STATUS_OFFSET	0x80

#define LLCACHE_REG_CFG_MAP_MASK	BIT(20)
#define LLCACHE_REG_CTRL_EN_MASK	BIT(0)
#define LLCACHE_REG_CTRL_INIT_MASK	BIT(14)
#define LLCACHE_REG_CCTL_STATUS_MASK	GENMASK(3, 0)

#define LLCACHE_WBINVAL_ALL		0x12

संरचना andes_llcache {
	संरचना cache_device dev;
	शून्य *base;
	uint32_t cmd_stride;
	uint32_t status_stride;
	uint32_t status_core_stride;
};

#define to_llcache(_dev) container_of(_dev, संरचना andes_llcache, dev)

स्थिर bool andes_llcache_init_done(संरचना andes_llcache *llcache)
{
	uint32_t llcache_ctrl;
	शून्य *ctrl_addr = (वर्ण *)llcache->base + LLCACHE_REG_CTRL_OFFSET;

	llcache_ctrl = readl_relaxed(ctrl_addr);
	प्रत्यागम !EXTRACT_FIELD(llcache_ctrl, LLCACHE_REG_CTRL_INIT_MASK);
}

स्थिर bool andes_llcache_cctl_done(संरचना andes_llcache *llcache, uint32_t hartid)
{
	uint32_t llcache_cctl_status;
	शून्य *cctl_status_addr = (वर्ण *)llcache->base + LLCACHE_REG_CCTL_STATUS_OFFSET +
				 hartid * llcache->status_stride;

	llcache_cctl_status = readl_relaxed(cctl_status_addr);
	प्रत्यागम !EXTRACT_FIELD(llcache_cctl_status,
			      LLCACHE_REG_CCTL_STATUS_MASK <<
			      hartid * llcache->status_core_stride);
}

स्थिर पूर्णाङ्क andes_llcache_flush_all(संरचना cache_device *dev)
{
	uint32_t hartid = current_hartid();
	संरचना andes_llcache *llcache = to_llcache(dev);
	शून्य *cctl_cmd_addr = (वर्ण *)llcache->base + LLCACHE_REG_CCTL_CMD_OFFSET +
			      hartid * llcache->cmd_stride;

	/*
	 * Each command register corresponds to one CPU core, so each CPU core
	 * should only use its command registers to do the cache operation.
	 */
	writel(LLCACHE_WBINVAL_ALL, cctl_cmd_addr);

	/* Wait for the command completion */
	यावत् (!andes_llcache_cctl_done(llcache, hartid))
		;

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क andes_llcache_enable(संरचना cache_device *dev, bool enable)
{
	संरचना andes_llcache *llcache = to_llcache(dev);
	u32 llcache_ctrl;
	शून्य *ctrl_addr = (वर्ण *)llcache->base + LLCACHE_REG_CTRL_OFFSET;

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
		writel(llcache_ctrl | LLCACHE_REG_CTRL_EN_MASK, ctrl_addr);
	} अन्यथा {
		llcache_ctrl = readl(ctrl_addr);
		atcsmu_write_scratch(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_MASK);
		writel(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_MASK, ctrl_addr);
	}

	llcache_ctrl = readl(ctrl_addr);
	यदि (enable != EXTRACT_FIELD(llcache_ctrl, LLCACHE_REG_CTRL_EN_MASK))
		प्रत्यागम SBI_EFAIL;

	प्रत्यागम SBI_OK;
}

स्थिर संरचना cache_ops andes_llcache_ops = {
	.cache_flush_all = andes_llcache_flush_all,
	.cache_enable = andes_llcache_enable,
};

स्थिर पूर्णाङ्क andes_llcache_probe(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff, ध्रुव संरचना fdt_match *match)
{
	पूर्णाङ्क rc;
	u64 llcache_base = 0, size;
	संरचना andes_llcache *llcache;
	संरचना cache_device *dev;
	uint32_t llcache_cfg;

	rc = fdt_get_गाँठ_addr_size(fdt, nodeoff, 0, &llcache_base, &size);
	यदि (rc < 0 || !llcache_base || !size)
		प्रत्यागम SBI_ENODEV;

	llcache = sbi_zalloc(परिमाण(*llcache));
	यदि (!llcache)
		प्रत्यागम SBI_ENOMEM;

	dev = &llcache->dev;
	dev->ops = &andes_llcache_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	यदि (rc) {
		sbi_free(llcache);
		प्रत्यागम rc;
	}

	llcache->base = (शून्य *)(ulong)llcache_base;
	llcache_cfg = readl_relaxed((वर्ण *)llcache->base + LLCACHE_REG_CFG_OFFSET);

	/* Configurations for V1/V0 memory map */
	यदि (EXTRACT_FIELD(llcache_cfg, LLCACHE_REG_CFG_MAP_MASK)) {
		llcache->cmd_stride = 0x1000;
		llcache->status_stride = 0x1000;
		llcache->status_core_stride = 0;
	} अन्यथा {
		llcache->cmd_stride = 0x10;
		llcache->status_stride = 0x0;
		llcache->status_core_stride = 4;
	}

	यदि (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_मूल_add_memrange(
			(unsigned long)llcache->base, (unsigned long)size, PAGE_SIZE,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		यदि (rc)
			प्रत्यागम rc;
	}

	/* Wait for the hardware initialization done */
	यावत् (!andes_llcache_init_done(llcache))
		;

	प्रत्यागम SBI_OK;
}

स्थिर ध्रुव संरचना fdt_match andes_llcache_match[] = {
	{ .compatible = "andestech,llcache" },
	{},
};

ध्रुव संरचना fdt_driver fdt_andes_llcache = {
	.match_table = andes_llcache_match,
	.init = andes_llcache_probe,
};
