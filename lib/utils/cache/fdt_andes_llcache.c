/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi_utils/cache/fdt_cache.h>
#شامل <sbi_utils/fdt/fdt_driver.h>
#شامل <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#define LLCACHE_REG_CFG_OFFSET		0x0
#define LLCACHE_REG_CTRL_OFFSET		0x8
#define LLCACHE_REG_CCTL_CMD_OFFSET	0x40
#define LLCACHE_REG_CCTL_STATUS_OFFSET	0x80

#define LLCACHE_REG_CFG_MAP_MASK	BIT(20)
#define LLCACHE_REG_CTRL_EN_MASK	BIT(0)
#define LLCACHE_REG_CTRL_INIT_MASK	BIT(14)
#define LLCACHE_REG_CCTL_STATUS_MASK	GENMASK(3, 0)

#define LLCACHE_WBINVAL_ALL		0x12

ساختار andes_llcache {
	ساختار cache_device dev;
	تهی *base;
	uint32_t cmd_stride;
	uint32_t status_stride;
	uint32_t status_core_stride;
};

#define to_llcache(_dev) container_of(_dev, ساختار andes_llcache, dev)

ایستا bool andes_llcache_init_done(ساختار andes_llcache *llcache)
{
	uint32_t llcache_ctrl;
	تهی *ctrl_addr = (نویسه *)llcache->base + LLCACHE_REG_CTRL_OFFSET;

	llcache_ctrl = readl_relaxed(ctrl_addr);
	بازگشت !EXTRACT_FIELD(llcache_ctrl, LLCACHE_REG_CTRL_INIT_MASK);
}

ایستا bool andes_llcache_cctl_done(ساختار andes_llcache *llcache, uint32_t hartid)
{
	uint32_t llcache_cctl_status;
	تهی *cctl_status_addr = (نویسه *)llcache->base + LLCACHE_REG_CCTL_STATUS_OFFSET +
				 hartid * llcache->status_stride;

	llcache_cctl_status = readl_relaxed(cctl_status_addr);
	بازگشت !EXTRACT_FIELD(llcache_cctl_status,
			      LLCACHE_REG_CCTL_STATUS_MASK <<
			      hartid * llcache->status_core_stride);
}

ایستا صحیح andes_llcache_flush_all(ساختار cache_device *dev)
{
	uint32_t hartid = current_hartid();
	ساختار andes_llcache *llcache = to_llcache(dev);
	تهی *cctl_cmd_addr = (نویسه *)llcache->base + LLCACHE_REG_CCTL_CMD_OFFSET +
			      hartid * llcache->cmd_stride;

	/*
	 * Each command register corresponds to one CPU core, so each CPU core
	 * should only use its command registers to do the cache operation.
	 */
	writel(LLCACHE_WBINVAL_ALL, cctl_cmd_addr);

	/* Wait for the command completion */
	وقتی (!andes_llcache_cctl_done(llcache, hartid))
		;

	بازگشت 0;
}

ایستا صحیح andes_llcache_enable(ساختار cache_device *dev, bool enable)
{
	ساختار andes_llcache *llcache = to_llcache(dev);
	u32 llcache_ctrl;
	تهی *ctrl_addr = (نویسه *)llcache->base + LLCACHE_REG_CTRL_OFFSET;

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
	اگر (enable) {
		llcache_ctrl = atcsmu_read_scratch();
		writel(llcache_ctrl, ctrl_addr);
		writel(llcache_ctrl | LLCACHE_REG_CTRL_EN_MASK, ctrl_addr);
	} وگرنه {
		llcache_ctrl = readl(ctrl_addr);
		atcsmu_write_scratch(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_MASK);
		writel(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_MASK, ctrl_addr);
	}

	llcache_ctrl = readl(ctrl_addr);
	اگر (enable != EXTRACT_FIELD(llcache_ctrl, LLCACHE_REG_CTRL_EN_MASK))
		بازگشت SBI_EFAIL;

	بازگشت SBI_OK;
}

ایستا ساختار cache_ops andes_llcache_ops = {
	.cache_flush_all = andes_llcache_flush_all,
	.cache_enable = andes_llcache_enable,
};

ایستا صحیح andes_llcache_probe(ثابت تهی *fdt, صحیح nodeoff, ثابت ساختار fdt_match *match)
{
	صحیح rc;
	u64 llcache_base = 0, size;
	ساختار andes_llcache *llcache;
	ساختار cache_device *dev;
	uint32_t llcache_cfg;

	rc = fdt_get_گره_addr_size(fdt, nodeoff, 0, &llcache_base, &size);
	اگر (rc < 0 || !llcache_base || !size)
		بازگشت SBI_ENODEV;

	llcache = sbi_zalloc(اندازه(*llcache));
	اگر (!llcache)
		بازگشت SBI_ENOMEM;

	dev = &llcache->dev;
	dev->ops = &andes_llcache_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	اگر (rc) {
		sbi_free(llcache);
		بازگشت rc;
	}

	llcache->base = (تهی *)(ulong)llcache_base;
	llcache_cfg = readl_relaxed((نویسه *)llcache->base + LLCACHE_REG_CFG_OFFSET);

	/* Configurations for V1/V0 memory map */
	اگر (EXTRACT_FIELD(llcache_cfg, LLCACHE_REG_CFG_MAP_MASK)) {
		llcache->cmd_stride = 0x1000;
		llcache->status_stride = 0x1000;
		llcache->status_core_stride = 0;
	} وگرنه {
		llcache->cmd_stride = 0x10;
		llcache->status_stride = 0x0;
		llcache->status_core_stride = 4;
	}

	اگر (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_ریشه_add_memrange(
			(unsigned long)llcache->base, (unsigned long)size, PAGE_SIZE,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		اگر (rc)
			بازگشت rc;
	}

	/* Wait for the hardware initialization done */
	وقتی (!andes_llcache_init_done(llcache))
		;

	بازگشت SBI_OK;
}

ایستا ثابت ساختار fdt_match andes_llcache_match[] = {
	{ .compatible = "andestech,llcache" },
	{},
};

ثابت ساختار fdt_driver fdt_andes_llcache = {
	.match_table = andes_llcache_match,
	.init = andes_llcache_probe,
};
