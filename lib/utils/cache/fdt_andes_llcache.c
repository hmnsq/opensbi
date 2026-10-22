/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_domain.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi_utils/cache/fdt_cache.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_driver.h>
#அந்தர்பாவ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#define LLCACHE_REG_CFG_OFFSET		0x0
#define LLCACHE_REG_CTRL_OFFSET		0x8
#define LLCACHE_REG_CCTL_CMD_OFFSET	0x40
#define LLCACHE_REG_CCTL_STATUS_OFFSET	0x80

#define LLCACHE_REG_CFG_MAP_MASK	BIT(20)
#define LLCACHE_REG_CTRL_EN_MASK	BIT(0)
#define LLCACHE_REG_CTRL_INIT_MASK	BIT(14)
#define LLCACHE_REG_CCTL_STATUS_MASK	GENMASK(3, 0)

#define LLCACHE_WBINVAL_ALL		0x12

கட்டமைப்பு andes_llcache {
	கட்டமைப்பு cache_device dev;
	வெறுமை *base;
	uint32_t cmd_stride;
	uint32_t status_stride;
	uint32_t status_core_stride;
};

#define to_llcache(_dev) container_of(_dev, கட்டமைப்பு andes_llcache, dev)

நிலையான bool andes_llcache_init_done(கட்டமைப்பு andes_llcache *llcache)
{
	uint32_t llcache_ctrl;
	வெறுமை *ctrl_addr = (எழுத்து *)llcache->base + LLCACHE_REG_CTRL_OFFSET;

	llcache_ctrl = readl_relaxed(ctrl_addr);
	திரும்பு !EXTRACT_FIELD(llcache_ctrl, LLCACHE_REG_CTRL_INIT_MASK);
}

நிலையான bool andes_llcache_cctl_done(கட்டமைப்பு andes_llcache *llcache, uint32_t hartid)
{
	uint32_t llcache_cctl_status;
	வெறுமை *cctl_status_addr = (எழுத்து *)llcache->base + LLCACHE_REG_CCTL_STATUS_OFFSET +
				 hartid * llcache->status_stride;

	llcache_cctl_status = readl_relaxed(cctl_status_addr);
	திரும்பு !EXTRACT_FIELD(llcache_cctl_status,
			      LLCACHE_REG_CCTL_STATUS_MASK <<
			      hartid * llcache->status_core_stride);
}

நிலையான முழு andes_llcache_flush_all(கட்டமைப்பு cache_device *dev)
{
	uint32_t hartid = current_hartid();
	கட்டமைப்பு andes_llcache *llcache = to_llcache(dev);
	வெறுமை *cctl_cmd_addr = (எழுத்து *)llcache->base + LLCACHE_REG_CCTL_CMD_OFFSET +
			      hartid * llcache->cmd_stride;

	/*
	 * Each command register corresponds to one CPU core, so each CPU core
	 * should only use its command registers to do the cache operation.
	 */
	writel(LLCACHE_WBINVAL_ALL, cctl_cmd_addr);

	/* Wait for the command completion */
	போது (!andes_llcache_cctl_done(llcache, hartid))
		;

	திரும்பு 0;
}

நிலையான முழு andes_llcache_enable(கட்டமைப்பு cache_device *dev, bool enable)
{
	கட்டமைப்பு andes_llcache *llcache = to_llcache(dev);
	u32 llcache_ctrl;
	வெறுமை *ctrl_addr = (எழுத்து *)llcache->base + LLCACHE_REG_CTRL_OFFSET;

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
	எனில் (enable) {
		llcache_ctrl = atcsmu_read_scratch();
		writel(llcache_ctrl, ctrl_addr);
		writel(llcache_ctrl | LLCACHE_REG_CTRL_EN_MASK, ctrl_addr);
	} இல்லையெனில் {
		llcache_ctrl = readl(ctrl_addr);
		atcsmu_write_scratch(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_MASK);
		writel(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_MASK, ctrl_addr);
	}

	llcache_ctrl = readl(ctrl_addr);
	எனில் (enable != EXTRACT_FIELD(llcache_ctrl, LLCACHE_REG_CTRL_EN_MASK))
		திரும்பு SBI_EFAIL;

	திரும்பு SBI_OK;
}

நிலையான கட்டமைப்பு cache_ops andes_llcache_ops = {
	.cache_flush_all = andes_llcache_flush_all,
	.cache_enable = andes_llcache_enable,
};

நிலையான முழு andes_llcache_probe(மாறிலி வெறுமை *fdt, முழு nodeoff, மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு rc;
	u64 llcache_base = 0, size;
	கட்டமைப்பு andes_llcache *llcache;
	கட்டமைப்பு cache_device *dev;
	uint32_t llcache_cfg;

	rc = fdt_get_முனை_addr_size(fdt, nodeoff, 0, &llcache_base, &size);
	எனில் (rc < 0 || !llcache_base || !size)
		திரும்பு SBI_ENODEV;

	llcache = sbi_zalloc(அளவு(*llcache));
	எனில் (!llcache)
		திரும்பு SBI_ENOMEM;

	dev = &llcache->dev;
	dev->ops = &andes_llcache_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	எனில் (rc) {
		sbi_free(llcache);
		திரும்பு rc;
	}

	llcache->base = (வெறுமை *)(ulong)llcache_base;
	llcache_cfg = readl_relaxed((எழுத்து *)llcache->base + LLCACHE_REG_CFG_OFFSET);

	/* Configurations for V1/V0 memory map */
	எனில் (EXTRACT_FIELD(llcache_cfg, LLCACHE_REG_CFG_MAP_MASK)) {
		llcache->cmd_stride = 0x1000;
		llcache->status_stride = 0x1000;
		llcache->status_core_stride = 0;
	} இல்லையெனில் {
		llcache->cmd_stride = 0x10;
		llcache->status_stride = 0x0;
		llcache->status_core_stride = 4;
	}

	எனில் (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_வேர்_add_memrange(
			(unsigned long)llcache->base, (unsigned long)size, PAGE_SIZE,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		எனில் (rc)
			திரும்பு rc;
	}

	/* Wait for the hardware initialization done */
	போது (!andes_llcache_init_done(llcache))
		;

	திரும்பு SBI_OK;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match andes_llcache_match[] = {
	{ .compatible = "andestech,llcache" },
	{},
};

மாறிலி கட்டமைப்பு fdt_driver fdt_andes_llcache = {
	.match_table = andes_llcache_match,
	.init = andes_llcache_probe,
};
