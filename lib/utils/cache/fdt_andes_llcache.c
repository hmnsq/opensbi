/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_domain.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi_utils/cache/fdt_cache.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_driver.h>
#અન્તર્ભાવ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#define LLCACHE_REG_CFG_OFFSET		0x0
#define LLCACHE_REG_CTRL_OFFSET		0x8
#define LLCACHE_REG_CCTL_CMD_OFFSET	0x40
#define LLCACHE_REG_CCTL_STATUS_OFFSET	0x80

#define LLCACHE_REG_CFG_MAP_MASK	BIT(20)
#define LLCACHE_REG_CTRL_EN_MASK	BIT(0)
#define LLCACHE_REG_CTRL_INIT_MASK	BIT(14)
#define LLCACHE_REG_CCTL_STATUS_MASK	GENMASK(3, 0)

#define LLCACHE_WBINVAL_ALL		0x12

સંરચના andes_llcache {
	સંરચના cache_device dev;
	શૂન્ય *base;
	uint32_t cmd_stride;
	uint32_t status_stride;
	uint32_t status_core_stride;
};

#define to_llcache(_dev) container_of(_dev, સંરચના andes_llcache, dev)

સ્થિર bool andes_llcache_init_done(સંરચના andes_llcache *llcache)
{
	uint32_t llcache_ctrl;
	શૂન્ય *ctrl_addr = (અક્ષર *)llcache->base + LLCACHE_REG_CTRL_OFFSET;

	llcache_ctrl = readl_relaxed(ctrl_addr);
	પાછા !EXTRACT_FIELD(llcache_ctrl, LLCACHE_REG_CTRL_INIT_MASK);
}

સ્થિર bool andes_llcache_cctl_done(સંરચના andes_llcache *llcache, uint32_t hartid)
{
	uint32_t llcache_cctl_status;
	શૂન્ય *cctl_status_addr = (અક્ષર *)llcache->base + LLCACHE_REG_CCTL_STATUS_OFFSET +
				 hartid * llcache->status_stride;

	llcache_cctl_status = readl_relaxed(cctl_status_addr);
	પાછા !EXTRACT_FIELD(llcache_cctl_status,
			      LLCACHE_REG_CCTL_STATUS_MASK <<
			      hartid * llcache->status_core_stride);
}

સ્થિર પૂર્ણાંક andes_llcache_flush_all(સંરચના cache_device *dev)
{
	uint32_t hartid = current_hartid();
	સંરચના andes_llcache *llcache = to_llcache(dev);
	શૂન્ય *cctl_cmd_addr = (અક્ષર *)llcache->base + LLCACHE_REG_CCTL_CMD_OFFSET +
			      hartid * llcache->cmd_stride;

	/*
	 * Each command register corresponds to one CPU core, so each CPU core
	 * should only use its command registers to do the cache operation.
	 */
	writel(LLCACHE_WBINVAL_ALL, cctl_cmd_addr);

	/* Wait for the command completion */
	જ્યારે (!andes_llcache_cctl_done(llcache, hartid))
		;

	પાછા 0;
}

સ્થિર પૂર્ણાંક andes_llcache_enable(સંરચના cache_device *dev, bool enable)
{
	સંરચના andes_llcache *llcache = to_llcache(dev);
	u32 llcache_ctrl;
	શૂન્ય *ctrl_addr = (અક્ષર *)llcache->base + LLCACHE_REG_CTRL_OFFSET;

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
	જો (enable) {
		llcache_ctrl = atcsmu_read_scratch();
		writel(llcache_ctrl, ctrl_addr);
		writel(llcache_ctrl | LLCACHE_REG_CTRL_EN_MASK, ctrl_addr);
	} નહીંતર {
		llcache_ctrl = readl(ctrl_addr);
		atcsmu_write_scratch(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_MASK);
		writel(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_MASK, ctrl_addr);
	}

	llcache_ctrl = readl(ctrl_addr);
	જો (enable != EXTRACT_FIELD(llcache_ctrl, LLCACHE_REG_CTRL_EN_MASK))
		પાછા SBI_EFAIL;

	પાછા SBI_OK;
}

સ્થિર સંરચના cache_ops andes_llcache_ops = {
	.cache_flush_all = andes_llcache_flush_all,
	.cache_enable = andes_llcache_enable,
};

સ્થિર પૂર્ણાંક andes_llcache_probe(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff, અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક rc;
	u64 llcache_base = 0, size;
	સંરચના andes_llcache *llcache;
	સંરચના cache_device *dev;
	uint32_t llcache_cfg;

	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoff, 0, &llcache_base, &size);
	જો (rc < 0 || !llcache_base || !size)
		પાછા SBI_ENODEV;

	llcache = sbi_zalloc(કદ(*llcache));
	જો (!llcache)
		પાછા SBI_ENOMEM;

	dev = &llcache->dev;
	dev->ops = &andes_llcache_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	જો (rc) {
		sbi_free(llcache);
		પાછા rc;
	}

	llcache->base = (શૂન્ય *)(ulong)llcache_base;
	llcache_cfg = readl_relaxed((અક્ષર *)llcache->base + LLCACHE_REG_CFG_OFFSET);

	/* Configurations for V1/V0 memory map */
	જો (EXTRACT_FIELD(llcache_cfg, LLCACHE_REG_CFG_MAP_MASK)) {
		llcache->cmd_stride = 0x1000;
		llcache->status_stride = 0x1000;
		llcache->status_core_stride = 0;
	} નહીંતર {
		llcache->cmd_stride = 0x10;
		llcache->status_stride = 0x0;
		llcache->status_core_stride = 4;
	}

	જો (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_મૂળ_add_memrange(
			(unsigned long)llcache->base, (unsigned long)size, PAGE_SIZE,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		જો (rc)
			પાછા rc;
	}

	/* Wait for the hardware initialization done */
	જ્યારે (!andes_llcache_init_done(llcache))
		;

	પાછા SBI_OK;
}

સ્થિર અચલ સંરચના fdt_match andes_llcache_match[] = {
	{ .compatible = "andestech,llcache" },
	{},
};

અચલ સંરચના fdt_driver fdt_andes_llcache = {
	.match_table = andes_llcache_match,
	.init = andes_llcache_probe,
};
