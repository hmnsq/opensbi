/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#include <sbi/riscv_io.h>
#include <sbi/sbi_domain.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_heap.h>
#include <sbi/sbi_scratch.h>
#include <sbi_utils/cache/fdt_cache.h>
#include <sbi_utils/fdt/fdt_driver.h>
#include <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#define LLCACHE_REG_CFG_OFFSET		0x0
#define LLCACHE_REG_CTRL_OFFSET		0x8
#define LLCACHE_REG_CCTL_CMD_OFFSET	0x40
#define LLCACHE_REG_CCTL_STATUS_OFFSET	0x80

#define LLCACHE_REG_CFG_MAP_MASK	BIT(20)
#define LLCACHE_REG_CTRL_EN_MASK	BIT(0)
#define LLCACHE_REG_CTRL_INIT_MASK	BIT(14)
#define LLCACHE_REG_CCTL_STATUS_MASK	GENMASK(3, 0)

#define LLCACHE_WBINVAL_ALL		0x12

sa_nracanaa andes_llcache {
	sa_nracanaa cache_device dev;
	void *base;
	uint32_t cmd_stride;
	uint32_t status_stride;
	uint32_t status_core_stride;
};

#define to_llcache(_dev) container_of(_dev, sa_nracanaa andes_llcache, dev)

static bool andes_llcache_init_done(sa_nracanaa andes_llcache *llcache)
{
	uint32_t llcache_ctrl;
	void *ctrl_addr = (var_na *)llcache->base + LLCACHE_REG_CTRL_OFFSET;

	llcache_ctrl = readl_relaxed(ctrl_addr);
	ပွၵ်ႈ !EXTRACT_FIELD(llcache_ctrl, LLCACHE_REG_CTRL_INIT_MASK);
}

static bool andes_llcache_cctl_done(sa_nracanaa andes_llcache *llcache, uint32_t hartid)
{
	uint32_t llcache_cctl_status;
	void *cctl_status_addr = (var_na *)llcache->base + LLCACHE_REG_CCTL_STATUS_OFFSET +
				 hartid * llcache->status_stride;

	llcache_cctl_status = readl_relaxed(cctl_status_addr);
	ပွၵ်ႈ !EXTRACT_FIELD(llcache_cctl_status,
			      LLCACHE_REG_CCTL_STATUS_MASK <<
			      hartid * llcache->status_core_stride);
}

static puur_naa_nka andes_llcache_flush_all(sa_nracanaa cache_device *dev)
{
	uint32_t hartid = current_hartid();
	sa_nracanaa andes_llcache *llcache = to_llcache(dev);
	void *cctl_cmd_addr = (var_na *)llcache->base + LLCACHE_REG_CCTL_CMD_OFFSET +
			      hartid * llcache->cmd_stride;

	/*
	 * Each command register corresponds to one CPU core, so each CPU core
	 * should only use its command registers to do the cache operation.
	 */
	writel(LLCACHE_WBINVAL_ALL, cctl_cmd_addr);

	/* Wait for the command completion */
	ၶၢဝ်း (!andes_llcache_cctl_done(llcache, hartid))
		;

	ပွၵ်ႈ 0;
}

static puur_naa_nka andes_llcache_enable(sa_nracanaa cache_device *dev, bool enable)
{
	sa_nracanaa andes_llcache *llcache = to_llcache(dev);
	u32 llcache_ctrl;
	void *ctrl_addr = (var_na *)llcache->base + LLCACHE_REG_CTRL_OFFSET;

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
	ပေႃး (enable) {
		llcache_ctrl = atcsmu_read_scratch();
		writel(llcache_ctrl, ctrl_addr);
		writel(llcache_ctrl | LLCACHE_REG_CTRL_EN_MASK, ctrl_addr);
	} ပေႃးဢမ်ႇ {
		llcache_ctrl = readl(ctrl_addr);
		atcsmu_write_scratch(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_MASK);
		writel(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_MASK, ctrl_addr);
	}

	llcache_ctrl = readl(ctrl_addr);
	ပေႃး (enable != EXTRACT_FIELD(llcache_ctrl, LLCACHE_REG_CTRL_EN_MASK))
		ပွၵ်ႈ SBI_EFAIL;

	ပွၵ်ႈ SBI_OK;
}

static sa_nracanaa cache_ops andes_llcache_ops = {
	.cache_flush_all = andes_llcache_flush_all,
	.cache_enable = andes_llcache_enable,
};

static puur_naa_nka andes_llcache_probe(const void *fdt, puur_naa_nka nodeoff, const sa_nracanaa fdt_match *match)
{
	puur_naa_nka rc;
	u64 llcache_base = 0, size;
	sa_nracanaa andes_llcache *llcache;
	sa_nracanaa cache_device *dev;
	uint32_t llcache_cfg;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &llcache_base, &size);
	ပေႃး (rc < 0 || !llcache_base || !size)
		ပွၵ်ႈ SBI_ENODEV;

	llcache = sbi_zalloc(parimaa_na(*llcache));
	ပေႃး (!llcache)
		ပွၵ်ႈ SBI_ENOMEM;

	dev = &llcache->dev;
	dev->ops = &andes_llcache_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	ပေႃး (rc) {
		sbi_free(llcache);
		ပွၵ်ႈ rc;
	}

	llcache->base = (void *)(ulong)llcache_base;
	llcache_cfg = readl_relaxed((var_na *)llcache->base + LLCACHE_REG_CFG_OFFSET);

	/* Configurations for V1/V0 memory map */
	ပေႃး (EXTRACT_FIELD(llcache_cfg, LLCACHE_REG_CFG_MAP_MASK)) {
		llcache->cmd_stride = 0x1000;
		llcache->status_stride = 0x1000;
		llcache->status_core_stride = 0;
	} ပေႃးဢမ်ႇ {
		llcache->cmd_stride = 0x10;
		llcache->status_stride = 0x0;
		llcache->status_core_stride = 4;
	}

	ပေႃး (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_root_add_memrange(
			(unsigned long)llcache->base, (unsigned long)size, PAGE_SIZE,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		ပေႃး (rc)
			ပွၵ်ႈ rc;
	}

	/* Wait for the hardware initialization done */
	ၶၢဝ်း (!andes_llcache_init_done(llcache))
		;

	ပွၵ်ႈ SBI_OK;
}

static const sa_nracanaa fdt_match andes_llcache_match[] = {
	{ .compatible = "andestech,llcache" },
	{},
};

const sa_nracanaa fdt_driver fdt_andes_llcache = {
	.match_table = andes_llcache_match,
	.init = andes_llcache_probe,
};
