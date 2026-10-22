/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi_utils/cache/fdt_cache.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_driver.h>
#അന്തര്ഭാവ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#define LLCACHE_REG_CFG_OFFSET		0x0
#define LLCACHE_REG_CTRL_OFFSET		0x8
#define LLCACHE_REG_CCTL_CMD_OFFSET	0x40
#define LLCACHE_REG_CCTL_STATUS_OFFSET	0x80

#define LLCACHE_REG_CFG_MAP_MASK	BIT(20)
#define LLCACHE_REG_CTRL_EN_MASK	BIT(0)
#define LLCACHE_REG_CTRL_INIT_MASK	BIT(14)
#define LLCACHE_REG_CCTL_STATUS_MASK	GENMASK(3, 0)

#define LLCACHE_WBINVAL_ALL		0x12

ഘടന andes_llcache {
	ഘടന cache_device dev;
	ശൂന്യം *base;
	uint32_t cmd_stride;
	uint32_t status_stride;
	uint32_t status_core_stride;
};

#define to_llcache(_dev) container_of(_dev, ഘടന andes_llcache, dev)

സ്ഥിതം bool andes_llcache_init_done(ഘടന andes_llcache *llcache)
{
	uint32_t llcache_ctrl;
	ശൂന്യം *ctrl_addr = (അക്ഷരം *)llcache->base + LLCACHE_REG_CTRL_OFFSET;

	llcache_ctrl = readl_relaxed(ctrl_addr);
	തിരികെ !EXTRACT_FIELD(llcache_ctrl, LLCACHE_REG_CTRL_INIT_MASK);
}

സ്ഥിതം bool andes_llcache_cctl_done(ഘടന andes_llcache *llcache, uint32_t hartid)
{
	uint32_t llcache_cctl_status;
	ശൂന്യം *cctl_status_addr = (അക്ഷരം *)llcache->base + LLCACHE_REG_CCTL_STATUS_OFFSET +
				 hartid * llcache->status_stride;

	llcache_cctl_status = readl_relaxed(cctl_status_addr);
	തിരികെ !EXTRACT_FIELD(llcache_cctl_status,
			      LLCACHE_REG_CCTL_STATUS_MASK <<
			      hartid * llcache->status_core_stride);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ andes_llcache_flush_all(ഘടന cache_device *dev)
{
	uint32_t hartid = current_hartid();
	ഘടന andes_llcache *llcache = to_llcache(dev);
	ശൂന്യം *cctl_cmd_addr = (അക്ഷരം *)llcache->base + LLCACHE_REG_CCTL_CMD_OFFSET +
			      hartid * llcache->cmd_stride;

	/*
	 * Each command register corresponds to one CPU core, so each CPU core
	 * should only use its command registers to do the cache operation.
	 */
	writel(LLCACHE_WBINVAL_ALL, cctl_cmd_addr);

	/* Wait for the command completion */
	ആകുമ്പോൾ (!andes_llcache_cctl_done(llcache, hartid))
		;

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ andes_llcache_enable(ഘടന cache_device *dev, bool enable)
{
	ഘടന andes_llcache *llcache = to_llcache(dev);
	u32 llcache_ctrl;
	ശൂന്യം *ctrl_addr = (അക്ഷരം *)llcache->base + LLCACHE_REG_CTRL_OFFSET;

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
	എങ്കിൽ (enable) {
		llcache_ctrl = atcsmu_read_scratch();
		writel(llcache_ctrl, ctrl_addr);
		writel(llcache_ctrl | LLCACHE_REG_CTRL_EN_MASK, ctrl_addr);
	} അല്ലെങ്കിൽ {
		llcache_ctrl = readl(ctrl_addr);
		atcsmu_write_scratch(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_MASK);
		writel(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_MASK, ctrl_addr);
	}

	llcache_ctrl = readl(ctrl_addr);
	എങ്കിൽ (enable != EXTRACT_FIELD(llcache_ctrl, LLCACHE_REG_CTRL_EN_MASK))
		തിരികെ SBI_EFAIL;

	തിരികെ SBI_OK;
}

സ്ഥിതം ഘടന cache_ops andes_llcache_ops = {
	.cache_flush_all = andes_llcache_flush_all,
	.cache_enable = andes_llcache_enable,
};

സ്ഥിതം പൂർണ്ണസംഖ്യ andes_llcache_probe(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff, സ്ഥിരം ഘടന fdt_match *match)
{
	പൂർണ്ണസംഖ്യ rc;
	u64 llcache_base = 0, size;
	ഘടന andes_llcache *llcache;
	ഘടന cache_device *dev;
	uint32_t llcache_cfg;

	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoff, 0, &llcache_base, &size);
	എങ്കിൽ (rc < 0 || !llcache_base || !size)
		തിരികെ SBI_ENODEV;

	llcache = sbi_zalloc(വലുപ്പം(*llcache));
	എങ്കിൽ (!llcache)
		തിരികെ SBI_ENOMEM;

	dev = &llcache->dev;
	dev->ops = &andes_llcache_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	എങ്കിൽ (rc) {
		sbi_free(llcache);
		തിരികെ rc;
	}

	llcache->base = (ശൂന്യം *)(ulong)llcache_base;
	llcache_cfg = readl_relaxed((അക്ഷരം *)llcache->base + LLCACHE_REG_CFG_OFFSET);

	/* Configurations for V1/V0 memory map */
	എങ്കിൽ (EXTRACT_FIELD(llcache_cfg, LLCACHE_REG_CFG_MAP_MASK)) {
		llcache->cmd_stride = 0x1000;
		llcache->status_stride = 0x1000;
		llcache->status_core_stride = 0;
	} അല്ലെങ്കിൽ {
		llcache->cmd_stride = 0x10;
		llcache->status_stride = 0x0;
		llcache->status_core_stride = 4;
	}

	എങ്കിൽ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_വേര്_add_memrange(
			(unsigned long)llcache->base, (unsigned long)size, PAGE_SIZE,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		എങ്കിൽ (rc)
			തിരികെ rc;
	}

	/* Wait for the hardware initialization done */
	ആകുമ്പോൾ (!andes_llcache_init_done(llcache))
		;

	തിരികെ SBI_OK;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match andes_llcache_match[] = {
	{ .compatible = "andestech,llcache" },
	{},
};

സ്ഥിരം ഘടന fdt_driver fdt_andes_llcache = {
	.match_table = andes_llcache_match,
	.init = andes_llcache_probe,
};
