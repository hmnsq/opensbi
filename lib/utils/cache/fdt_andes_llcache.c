/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>
#অন্তর্ভাব <sbi_utils/cache/fdt_cache.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাব <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#define LLCACHE_REG_CFG_OFFSET		0x0
#define LLCACHE_REG_CTRL_OFFSET		0x8
#define LLCACHE_REG_CCTL_CMD_OFFSET	0x40
#define LLCACHE_REG_CCTL_STATUS_OFFSET	0x80

#define LLCACHE_REG_CFG_MAP_MASK	BIT(20)
#define LLCACHE_REG_CTRL_EN_MASK	BIT(0)
#define LLCACHE_REG_CTRL_INIT_MASK	BIT(14)
#define LLCACHE_REG_CCTL_STATUS_MASK	GENMASK(3, 0)

#define LLCACHE_WBINVAL_ALL		0x12

কাঠামো andes_llcache {
	কাঠামো cache_device dev;
	শূন্য *base;
	uint32_t cmd_stride;
	uint32_t status_stride;
	uint32_t status_core_stride;
};

#define to_llcache(_dev) container_of(_dev, কাঠামো andes_llcache, dev)

স্থির bool andes_llcache_init_done(কাঠামো andes_llcache *llcache)
{
	uint32_t llcache_ctrl;
	শূন্য *ctrl_addr = (অক্ষর *)llcache->base + LLCACHE_REG_CTRL_OFFSET;

	llcache_ctrl = readl_relaxed(ctrl_addr);
	ফেরত !EXTRACT_FIELD(llcache_ctrl, LLCACHE_REG_CTRL_INIT_MASK);
}

স্থির bool andes_llcache_cctl_done(কাঠামো andes_llcache *llcache, uint32_t hartid)
{
	uint32_t llcache_cctl_status;
	শূন্য *cctl_status_addr = (অক্ষর *)llcache->base + LLCACHE_REG_CCTL_STATUS_OFFSET +
				 hartid * llcache->status_stride;

	llcache_cctl_status = readl_relaxed(cctl_status_addr);
	ফেরত !EXTRACT_FIELD(llcache_cctl_status,
			      LLCACHE_REG_CCTL_STATUS_MASK <<
			      hartid * llcache->status_core_stride);
}

স্থির পূর্ণসংখ্যা andes_llcache_flush_all(কাঠামো cache_device *dev)
{
	uint32_t hartid = current_hartid();
	কাঠামো andes_llcache *llcache = to_llcache(dev);
	শূন্য *cctl_cmd_addr = (অক্ষর *)llcache->base + LLCACHE_REG_CCTL_CMD_OFFSET +
			      hartid * llcache->cmd_stride;

	/*
	 * Each command register corresponds to one CPU core, so each CPU core
	 * should only use its command registers to do the cache operation.
	 */
	writel(LLCACHE_WBINVAL_ALL, cctl_cmd_addr);

	/* Wait for the command completion */
	যখন (!andes_llcache_cctl_done(llcache, hartid))
		;

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা andes_llcache_enable(কাঠামো cache_device *dev, bool enable)
{
	কাঠামো andes_llcache *llcache = to_llcache(dev);
	u32 llcache_ctrl;
	শূন্য *ctrl_addr = (অক্ষর *)llcache->base + LLCACHE_REG_CTRL_OFFSET;

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
	যদি (enable) {
		llcache_ctrl = atcsmu_read_scratch();
		writel(llcache_ctrl, ctrl_addr);
		writel(llcache_ctrl | LLCACHE_REG_CTRL_EN_MASK, ctrl_addr);
	} অন্যথা {
		llcache_ctrl = readl(ctrl_addr);
		atcsmu_write_scratch(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_MASK);
		writel(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_MASK, ctrl_addr);
	}

	llcache_ctrl = readl(ctrl_addr);
	যদি (enable != EXTRACT_FIELD(llcache_ctrl, LLCACHE_REG_CTRL_EN_MASK))
		ফেরত SBI_EFAIL;

	ফেরত SBI_OK;
}

স্থির কাঠামো cache_ops andes_llcache_ops = {
	.cache_flush_all = andes_llcache_flush_all,
	.cache_enable = andes_llcache_enable,
};

স্থির পূর্ণসংখ্যা andes_llcache_probe(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff, ধ্রুবক কাঠামো fdt_match *match)
{
	পূর্ণসংখ্যা rc;
	u64 llcache_base = 0, আকার_2;
	কাঠামো andes_llcache *llcache;
	কাঠামো cache_device *dev;
	uint32_t llcache_cfg;

	rc = fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoff, 0, &llcache_base, &আকার_2);
	যদি (rc < 0 || !llcache_base || !আকার_2)
		ফেরত SBI_ENODEV;

	llcache = sbi_zalloc(আকার(*llcache));
	যদি (!llcache)
		ফেরত SBI_ENOMEM;

	dev = &llcache->dev;
	dev->ops = &andes_llcache_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	যদি (rc) {
		sbi_free(llcache);
		ফেরত rc;
	}

	llcache->base = (শূন্য *)(ulong)llcache_base;
	llcache_cfg = readl_relaxed((অক্ষর *)llcache->base + LLCACHE_REG_CFG_OFFSET);

	/* Configurations for V1/V0 memory map */
	যদি (EXTRACT_FIELD(llcache_cfg, LLCACHE_REG_CFG_MAP_MASK)) {
		llcache->cmd_stride = 0x1000;
		llcache->status_stride = 0x1000;
		llcache->status_core_stride = 0;
	} অন্যথা {
		llcache->cmd_stride = 0x10;
		llcache->status_stride = 0x0;
		llcache->status_core_stride = 4;
	}

	যদি (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_মূল_add_memrange(
			(unsigned long)llcache->base, (unsigned long)আকার_2, PAGE_আকার,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		যদি (rc)
			ফেরত rc;
	}

	/* Wait for the hardware initialization done */
	যখন (!andes_llcache_init_done(llcache))
		;

	ফেরত SBI_OK;
}

স্থির ধ্রুবক কাঠামো fdt_match andes_llcache_match[] = {
	{ .compatible = "andestech,llcache" },
	{},
};

ধ্রুবক কাঠামো fdt_driver fdt_andes_llcache = {
	.match_table = andes_llcache_match,
	.init = andes_llcache_probe,
};
