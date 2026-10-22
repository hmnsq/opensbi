/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/cache/fdt_cache.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#define LLCACHE_REG_CFG_OFFSET		0x0
#define LLCACHE_REG_CTRL_OFFSET		0x8
#define LLCACHE_REG_CCTL_CMD_OFFSET	0x40
#define LLCACHE_REG_CCTL_STATUS_OFFSET	0x80

#define LLCACHE_REG_CFG_MAP_MASK	BIT(20)
#define LLCACHE_REG_CTRL_EN_MASK	BIT(0)
#define LLCACHE_REG_CTRL_INIT_MASK	BIT(14)
#define LLCACHE_REG_CCTL_STATUS_MASK	GENMASK(3, 0)

#define LLCACHE_WBINVAL_ALL		0x12

ಸಂರಚನಾ andes_llcache {
	ಸಂರಚನಾ cache_device dev;
	ಶೂನ್ಯ *base;
	uint32_t cmd_stride;
	uint32_t status_stride;
	uint32_t status_core_stride;
};

#define to_llcache(_dev) container_of(_dev, ಸಂರಚನಾ andes_llcache, dev)

ಸ್ಥಿರ bool andes_llcache_init_done(ಸಂರಚನಾ andes_llcache *llcache)
{
	uint32_t llcache_ctrl;
	ಶೂನ್ಯ *ctrl_addr = (ವರ್ಣ *)llcache->base + LLCACHE_REG_CTRL_OFFSET;

	llcache_ctrl = readl_relaxed(ctrl_addr);
	ಪ್ರತ್ಯಾಗಮ !EXTRACT_FIELD(llcache_ctrl, LLCACHE_REG_CTRL_INIT_MASK);
}

ಸ್ಥಿರ bool andes_llcache_cctl_done(ಸಂರಚನಾ andes_llcache *llcache, uint32_t hartid)
{
	uint32_t llcache_cctl_status;
	ಶೂನ್ಯ *cctl_status_addr = (ವರ್ಣ *)llcache->base + LLCACHE_REG_CCTL_STATUS_OFFSET +
				 hartid * llcache->status_stride;

	llcache_cctl_status = readl_relaxed(cctl_status_addr);
	ಪ್ರತ್ಯಾಗಮ !EXTRACT_FIELD(llcache_cctl_status,
			      LLCACHE_REG_CCTL_STATUS_MASK <<
			      hartid * llcache->status_core_stride);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ andes_llcache_flush_all(ಸಂರಚನಾ cache_device *dev)
{
	uint32_t hartid = current_hartid();
	ಸಂರಚನಾ andes_llcache *llcache = to_llcache(dev);
	ಶೂನ್ಯ *cctl_cmd_addr = (ವರ್ಣ *)llcache->base + LLCACHE_REG_CCTL_CMD_OFFSET +
			      hartid * llcache->cmd_stride;

	/*
	 * Each command register corresponds to one CPU core, so each CPU core
	 * should only use its command registers to do the cache operation.
	 */
	writel(LLCACHE_WBINVAL_ALL, cctl_cmd_addr);

	/* Wait for the command completion */
	ಯಾವತ್ (!andes_llcache_cctl_done(llcache, hartid))
		;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ andes_llcache_enable(ಸಂರಚನಾ cache_device *dev, bool enable)
{
	ಸಂರಚನಾ andes_llcache *llcache = to_llcache(dev);
	u32 llcache_ctrl;
	ಶೂನ್ಯ *ctrl_addr = (ವರ್ಣ *)llcache->base + LLCACHE_REG_CTRL_OFFSET;

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
	ಯದಿ (enable) {
		llcache_ctrl = atcsmu_read_scratch();
		writel(llcache_ctrl, ctrl_addr);
		writel(llcache_ctrl | LLCACHE_REG_CTRL_EN_MASK, ctrl_addr);
	} ಅನ್ಯಥಾ {
		llcache_ctrl = readl(ctrl_addr);
		atcsmu_write_scratch(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_MASK);
		writel(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_MASK, ctrl_addr);
	}

	llcache_ctrl = readl(ctrl_addr);
	ಯದಿ (enable != EXTRACT_FIELD(llcache_ctrl, LLCACHE_REG_CTRL_EN_MASK))
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಸಂರಚನಾ cache_ops andes_llcache_ops = {
	.cache_flush_all = andes_llcache_flush_all,
	.cache_enable = andes_llcache_enable,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ andes_llcache_probe(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff, ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	u64 llcache_base = 0, size;
	ಸಂರಚನಾ andes_llcache *llcache;
	ಸಂರಚನಾ cache_device *dev;
	uint32_t llcache_cfg;

	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, 0, &llcache_base, &size);
	ಯದಿ (rc < 0 || !llcache_base || !size)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	llcache = sbi_zalloc(ಪರಿಮಾಣ(*llcache));
	ಯದಿ (!llcache)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

	dev = &llcache->dev;
	dev->ops = &andes_llcache_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	ಯದಿ (rc) {
		sbi_free(llcache);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	llcache->base = (ಶೂನ್ಯ *)(ulong)llcache_base;
	llcache_cfg = readl_relaxed((ವರ್ಣ *)llcache->base + LLCACHE_REG_CFG_OFFSET);

	/* Configurations for V1/V0 memory map */
	ಯದಿ (EXTRACT_FIELD(llcache_cfg, LLCACHE_REG_CFG_MAP_MASK)) {
		llcache->cmd_stride = 0x1000;
		llcache->status_stride = 0x1000;
		llcache->status_core_stride = 0;
	} ಅನ್ಯಥಾ {
		llcache->cmd_stride = 0x10;
		llcache->status_stride = 0x0;
		llcache->status_core_stride = 4;
	}

	ಯದಿ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_ಬೇರು_add_memrange(
			(unsigned long)llcache->base, (unsigned long)size, PAGE_SIZE,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;
	}

	/* Wait for the hardware initialization done */
	ಯಾವತ್ (!andes_llcache_init_done(llcache))
		;

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match andes_llcache_match[] = {
	{ .compatible = "andestech,llcache" },
	{},
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_andes_llcache = {
	.match_table = andes_llcache_match,
	.init = andes_llcache_probe,
};
