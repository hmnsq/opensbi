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
	ສົ່ງຄືນ !EXTRACT_FIELD(llcache_ctrl, LLCACHE_REG_CTRL_INIT_MASK);
}

static bool andes_llcache_cctl_done(sa_nracanaa andes_llcache *llcache, uint32_t hartid)
{
	uint32_t llcache_cctl_status;
	void *cctl_status_addr = (var_na *)llcache->base + LLCACHE_REG_CCTL_STATUS_OFFSET +
				 hartid * llcache->status_stride;

	llcache_cctl_status = readl_relaxed(cctl_status_addr);
	ສົ່ງຄືນ !EXTRACT_FIELD(llcache_cctl_status,
			      LLCACHE_REG_CCTL_STATUS_MASK <<
			      hartid * llcache->status_core_stride);
}

static ຈຳນວນເຕັມ andes_llcache_flush_all(sa_nracanaa cache_device *dev)
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
	ໃນຂະນະທີ່ (!andes_llcache_cctl_done(llcache, hartid))
		;

	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ andes_llcache_enable(sa_nracanaa cache_device *dev, bool enable)
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
	ຖ້າ (enable) {
		llcache_ctrl = atcsmu_read_scratch();
		writel(llcache_ctrl, ctrl_addr);
		writel(llcache_ctrl | LLCACHE_REG_CTRL_EN_MASK, ctrl_addr);
	} ບໍ່ດັ່ງນັ້ນ {
		llcache_ctrl = readl(ctrl_addr);
		atcsmu_write_scratch(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_MASK);
		writel(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_MASK, ctrl_addr);
	}

	llcache_ctrl = readl(ctrl_addr);
	ຖ້າ (enable != EXTRACT_FIELD(llcache_ctrl, LLCACHE_REG_CTRL_EN_MASK))
		ສົ່ງຄືນ SBI_EFAIL;

	ສົ່ງຄືນ SBI_OK;
}

static sa_nracanaa cache_ops andes_llcache_ops = {
	.cache_flush_all = andes_llcache_flush_all,
	.cache_enable = andes_llcache_enable,
};

static ຈຳນວນເຕັມ andes_llcache_probe(const void *fdt, ຈຳນວນເຕັມ nodeoff, const sa_nracanaa fdt_match *match)
{
	ຈຳນວນເຕັມ rc;
	u64 llcache_base = 0, size;
	sa_nracanaa andes_llcache *llcache;
	sa_nracanaa cache_device *dev;
	uint32_t llcache_cfg;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &llcache_base, &size);
	ຖ້າ (rc < 0 || !llcache_base || !size)
		ສົ່ງຄືນ SBI_ENODEV;

	llcache = sbi_zalloc(parimaa_na(*llcache));
	ຖ້າ (!llcache)
		ສົ່ງຄືນ SBI_ENOMEM;

	dev = &llcache->dev;
	dev->ops = &andes_llcache_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	ຖ້າ (rc) {
		sbi_free(llcache);
		ສົ່ງຄືນ rc;
	}

	llcache->base = (void *)(ulong)llcache_base;
	llcache_cfg = readl_relaxed((var_na *)llcache->base + LLCACHE_REG_CFG_OFFSET);

	/* Configurations for V1/V0 memory map */
	ຖ້າ (EXTRACT_FIELD(llcache_cfg, LLCACHE_REG_CFG_MAP_MASK)) {
		llcache->cmd_stride = 0x1000;
		llcache->status_stride = 0x1000;
		llcache->status_core_stride = 0;
	} ບໍ່ດັ່ງນັ້ນ {
		llcache->cmd_stride = 0x10;
		llcache->status_stride = 0x0;
		llcache->status_core_stride = 4;
	}

	ຖ້າ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_root_add_memrange(
			(unsigned long)llcache->base, (unsigned long)size, PAGE_SIZE,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		ຖ້າ (rc)
			ສົ່ງຄືນ rc;
	}

	/* Wait for the hardware initialization done */
	ໃນຂະນະທີ່ (!andes_llcache_init_done(llcache))
		;

	ສົ່ງຄືນ SBI_OK;
}

static const sa_nracanaa fdt_match andes_llcache_match[] = {
	{ .compatible = "andestech,llcache" },
	{},
};

const sa_nracanaa fdt_driver fdt_andes_llcache = {
	.match_table = andes_llcache_match,
	.init = andes_llcache_probe,
};
