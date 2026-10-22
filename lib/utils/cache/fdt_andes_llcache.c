/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/cache/fdt_cache.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_driver.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#define LLCACHE_REG_CFG_OFFSET		0x0
#define LLCACHE_REG_CTRL_OFFSET		0x8
#define LLCACHE_REG_CCTL_CMD_OFFSET	0x40
#define LLCACHE_REG_CCTL_STATUS_OFFSET	0x80

#define LLCACHE_REG_CFG_MAP_MASK	BIT(20)
#define LLCACHE_REG_CTRL_EN_MASK	BIT(0)
#define LLCACHE_REG_CTRL_INIT_MASK	BIT(14)
#define LLCACHE_REG_CCTL_STATUS_MASK	GENMASK(3, 0)

#define LLCACHE_WBINVAL_ALL		0x12

ସଂରଚନା andes_llcache {
	ସଂରଚନା cache_device dev;
	ଶୂନ୍ୟ *base;
	uint32_t cmd_stride;
	uint32_t status_stride;
	uint32_t status_core_stride;
};

#define to_llcache(_dev) container_of(_dev, ସଂରଚନା andes_llcache, dev)

ସ୍ଥିର bool andes_llcache_init_done(ସଂରଚନା andes_llcache *llcache)
{
	uint32_t llcache_ctrl;
	ଶୂନ୍ୟ *ctrl_addr = (ଅକ୍ଷର *)llcache->base + LLCACHE_REG_CTRL_OFFSET;

	llcache_ctrl = readl_relaxed(ctrl_addr);
	ଫେରନ୍ତୁ !EXTRACT_FIELD(llcache_ctrl, LLCACHE_REG_CTRL_INIT_MASK);
}

ସ୍ଥିର bool andes_llcache_cctl_done(ସଂରଚନା andes_llcache *llcache, uint32_t hartid)
{
	uint32_t llcache_cctl_status;
	ଶୂନ୍ୟ *cctl_status_addr = (ଅକ୍ଷର *)llcache->base + LLCACHE_REG_CCTL_STATUS_OFFSET +
				 hartid * llcache->status_stride;

	llcache_cctl_status = readl_relaxed(cctl_status_addr);
	ଫେରନ୍ତୁ !EXTRACT_FIELD(llcache_cctl_status,
			      LLCACHE_REG_CCTL_STATUS_MASK <<
			      hartid * llcache->status_core_stride);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା andes_llcache_flush_all(ସଂରଚନା cache_device *dev)
{
	uint32_t hartid = current_hartid();
	ସଂରଚନା andes_llcache *llcache = to_llcache(dev);
	ଶୂନ୍ୟ *cctl_cmd_addr = (ଅକ୍ଷର *)llcache->base + LLCACHE_REG_CCTL_CMD_OFFSET +
			      hartid * llcache->cmd_stride;

	/*
	 * Each command register corresponds to one CPU core, so each CPU core
	 * should only use its command registers to do the cache operation.
	 */
	writel(LLCACHE_WBINVAL_ALL, cctl_cmd_addr);

	/* Wait for the command completion */
	ଯେତେବେଳେ (!andes_llcache_cctl_done(llcache, hartid))
		;

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା andes_llcache_enable(ସଂରଚନା cache_device *dev, bool enable)
{
	ସଂରଚନା andes_llcache *llcache = to_llcache(dev);
	u32 llcache_ctrl;
	ଶୂନ୍ୟ *ctrl_addr = (ଅକ୍ଷର *)llcache->base + LLCACHE_REG_CTRL_OFFSET;

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
	ଯଦି (enable) {
		llcache_ctrl = atcsmu_read_scratch();
		writel(llcache_ctrl, ctrl_addr);
		writel(llcache_ctrl | LLCACHE_REG_CTRL_EN_MASK, ctrl_addr);
	} ଅନ୍ୟଥା {
		llcache_ctrl = readl(ctrl_addr);
		atcsmu_write_scratch(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_MASK);
		writel(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_MASK, ctrl_addr);
	}

	llcache_ctrl = readl(ctrl_addr);
	ଯଦି (enable != EXTRACT_FIELD(llcache_ctrl, LLCACHE_REG_CTRL_EN_MASK))
		ଫେରନ୍ତୁ SBI_EFAIL;

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ସଂରଚନା cache_ops andes_llcache_ops = {
	.cache_flush_all = andes_llcache_flush_all,
	.cache_enable = andes_llcache_enable,
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା andes_llcache_probe(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff, ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	u64 llcache_base = 0, size;
	ସଂରଚନା andes_llcache *llcache;
	ସଂରଚନା cache_device *dev;
	uint32_t llcache_cfg;

	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, 0, &llcache_base, &size);
	ଯଦି (rc < 0 || !llcache_base || !size)
		ଫେରନ୍ତୁ SBI_ENODEV;

	llcache = sbi_zalloc(ଆକାର(*llcache));
	ଯଦି (!llcache)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	dev = &llcache->dev;
	dev->ops = &andes_llcache_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	ଯଦି (rc) {
		sbi_free(llcache);
		ଫେରନ୍ତୁ rc;
	}

	llcache->base = (ଶୂନ୍ୟ *)(ulong)llcache_base;
	llcache_cfg = readl_relaxed((ଅକ୍ଷର *)llcache->base + LLCACHE_REG_CFG_OFFSET);

	/* Configurations for V1/V0 memory map */
	ଯଦି (EXTRACT_FIELD(llcache_cfg, LLCACHE_REG_CFG_MAP_MASK)) {
		llcache->cmd_stride = 0x1000;
		llcache->status_stride = 0x1000;
		llcache->status_core_stride = 0;
	} ଅନ୍ୟଥା {
		llcache->cmd_stride = 0x10;
		llcache->status_stride = 0x0;
		llcache->status_core_stride = 4;
	}

	ଯଦି (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_ମୂଳ_add_memrange(
			(unsigned long)llcache->base, (unsigned long)size, PAGE_SIZE,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;
	}

	/* Wait for the hardware initialization done */
	ଯେତେବେଳେ (!andes_llcache_init_done(llcache))
		;

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match andes_llcache_match[] = {
	{ .compatible = "andestech,llcache" },
	{},
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_andes_llcache = {
	.match_table = andes_llcache_match,
	.init = andes_llcache_probe,
};
