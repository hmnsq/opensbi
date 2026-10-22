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

#ವ್ಯಾಖ್ಯಾನಿಸು LLCACHE_REG_CFG_ಸ್ಥಾನಾಂತರ		0x0
#ವ್ಯಾಖ್ಯಾನಿಸು LLCACHE_REG_CTRL_ಸ್ಥಾನಾಂತರ		0x8
#ವ್ಯಾಖ್ಯಾನಿಸು LLCACHE_REG_CCTL_CMD_ಸ್ಥಾನಾಂತರ	0x40
#ವ್ಯಾಖ್ಯಾನಿಸು LLCACHE_REG_CCTL_ಸ್ಥಿತಿ_ಸ್ಥಾನಾಂತರ	0x80

#ವ್ಯಾಖ್ಯಾನಿಸು LLCACHE_REG_CFG_MAP_ಮುಖವಾಡ	ಬಿಟ್(20)
#ವ್ಯಾಖ್ಯಾನಿಸು LLCACHE_REG_CTRL_EN_ಮುಖವಾಡ	ಬಿಟ್(0)
#ವ್ಯಾಖ್ಯಾನಿಸು LLCACHE_REG_CTRL_INIT_ಮುಖವಾಡ	ಬಿಟ್(14)
#ವ್ಯಾಖ್ಯಾನಿಸು LLCACHE_REG_CCTL_ಸ್ಥಿತಿ_ಮುಖವಾಡ	GENMASK(3, 0)

#ವ್ಯಾಖ್ಯಾನಿಸು LLCACHE_WBINVAL_ALL		0x12

ರಚನೆ andes_llcache {
	ರಚನೆ cache_device dev;
	ಶೂನ್ಯ *ತಳ_2;
	uint32_t cmd_stride;
	uint32_t ಸ್ಥಿತಿ_stride;
	uint32_t ಸ್ಥಿತಿ_core_stride;
};

#ವ್ಯಾಖ್ಯಾನಿಸು ಗಮ್ಯ_llcache(_dev) container_ದ(_dev, ರಚನೆ andes_llcache, dev)

ಸ್ಥಿರ bool andes_llcache_init_ಮುಗಿದ(ರಚನೆ andes_llcache *llcache)
{
	uint32_t llcache_ctrl;
	ಶೂನ್ಯ *ctrl_addr = (ಅಕ್ಷರ *)llcache->ತಳ_2 + LLCACHE_REG_CTRL_ಸ್ಥಾನಾಂತರ;

	llcache_ctrl = readl_relaxed(ctrl_addr);
	ಹಿಂತಿರುಗಿ !ಹೊರತೆಗೆ_ಕ್ಷೇತ್ರ(llcache_ctrl, LLCACHE_REG_CTRL_INIT_ಮುಖವಾಡ);
}

ಸ್ಥಿರ bool andes_llcache_cctl_ಮುಗಿದ(ರಚನೆ andes_llcache *llcache, uint32_t hartid)
{
	uint32_t llcache_cctl_ಸ್ಥಿತಿ;
	ಶೂನ್ಯ *cctl_ಸ್ಥಿತಿ_addr = (ಅಕ್ಷರ *)llcache->ತಳ_2 + LLCACHE_REG_CCTL_ಸ್ಥಿತಿ_ಸ್ಥಾನಾಂತರ +
				 hartid * llcache->ಸ್ಥಿತಿ_stride;

	llcache_cctl_ಸ್ಥಿತಿ = readl_relaxed(cctl_ಸ್ಥಿತಿ_addr);
	ಹಿಂತಿರುಗಿ !ಹೊರತೆಗೆ_ಕ್ಷೇತ್ರ(llcache_cctl_ಸ್ಥಿತಿ,
			      LLCACHE_REG_CCTL_ಸ್ಥಿತಿ_ಮುಖವಾಡ <<
			      hartid * llcache->ಸ್ಥಿತಿ_core_stride);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ andes_llcache_flush_all(ರಚನೆ cache_device *dev)
{
	uint32_t hartid = current_hartid();
	ರಚನೆ andes_llcache *llcache = ಗಮ್ಯ_llcache(dev);
	ಶೂನ್ಯ *cctl_cmd_addr = (ಅಕ್ಷರ *)llcache->ತಳ_2 + LLCACHE_REG_CCTL_CMD_ಸ್ಥಾನಾಂತರ +
			      hartid * llcache->cmd_stride;

	/*
	 * Each command register corresponds to one CPU core, so each CPU core
	 * should only use its command registers to do the cache operation.
	 */
	writel(LLCACHE_WBINVAL_ALL, cctl_cmd_addr);

	/* Wait for the command completion */
	ಆಗ (!andes_llcache_cctl_ಮುಗಿದ(llcache, hartid))
		;

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ andes_llcache_enable(ರಚನೆ cache_device *dev, bool enable)
{
	ರಚನೆ andes_llcache *llcache = ಗಮ್ಯ_llcache(dev);
	u32 llcache_ctrl;
	ಶೂನ್ಯ *ctrl_addr = (ಅಕ್ಷರ *)llcache->ತಳ_2 + LLCACHE_REG_CTRL_ಸ್ಥಾನಾಂತರ;

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
	ಆದರೆ (enable) {
		llcache_ctrl = atcsmu_read_scratch();
		writel(llcache_ctrl, ctrl_addr);
		writel(llcache_ctrl | LLCACHE_REG_CTRL_EN_ಮುಖವಾಡ, ctrl_addr);
	} ಇಲ್ಲದಿದ್ದರೆ {
		llcache_ctrl = readl(ctrl_addr);
		atcsmu_write_scratch(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_ಮುಖವಾಡ);
		writel(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_ಮುಖವಾಡ, ctrl_addr);
	}

	llcache_ctrl = readl(ctrl_addr);
	ಆದರೆ (enable != ಹೊರತೆಗೆ_ಕ್ಷೇತ್ರ(llcache_ctrl, LLCACHE_REG_CTRL_EN_ಮುಖವಾಡ))
		ಹಿಂತಿರುಗಿ SBI_EFAIL;

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ರಚನೆ cache_ops andes_llcache_ops = {
	.cache_flush_all = andes_llcache_flush_all,
	.cache_enable = andes_llcache_enable,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ andes_llcache_probe(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff, ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಪೂರ್ಣಾಂಕ rc;
	u64 llcache_ತಳ = 0, ಗಾತ್ರ_2;
	ರಚನೆ andes_llcache *llcache;
	ರಚನೆ cache_device *dev;
	uint32_t llcache_cfg;

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, 0, &llcache_ತಳ, &ಗಾತ್ರ_2);
	ಆದರೆ (rc < 0 || !llcache_ತಳ || !ಗಾತ್ರ_2)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	llcache = sbi_zalloc(ಗಾತ್ರ(*llcache));
	ಆದರೆ (!llcache)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	dev = &llcache->dev;
	dev->ops = &andes_llcache_ops;
	rc = fdt_cache_ಕೂಡಿಸು(fdt, nodeoff, dev);
	ಆದರೆ (rc) {
		sbi_free(llcache);
		ಹಿಂತಿರುಗಿ rc;
	}

	llcache->ತಳ_2 = (ಶೂನ್ಯ *)(ulong)llcache_ತಳ;
	llcache_cfg = readl_relaxed((ಅಕ್ಷರ *)llcache->ತಳ_2 + LLCACHE_REG_CFG_ಸ್ಥಾನಾಂತರ);

	/* Configurations for V1/V0 memory map */
	ಆದರೆ (ಹೊರತೆಗೆ_ಕ್ಷೇತ್ರ(llcache_cfg, LLCACHE_REG_CFG_MAP_ಮುಖವಾಡ)) {
		llcache->cmd_stride = 0x1000;
		llcache->ಸ್ಥಿತಿ_stride = 0x1000;
		llcache->ಸ್ಥಿತಿ_core_stride = 0;
	} ಇಲ್ಲದಿದ್ದರೆ {
		llcache->cmd_stride = 0x10;
		llcache->ಸ್ಥಿತಿ_stride = 0x0;
		llcache->ಸ್ಥಿತಿ_core_stride = 4;
	}

	ಆದರೆ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_ಬೇರು_ಕೂಡಿಸು_memrange(
			(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)llcache->ತಳ_2, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)ಗಾತ್ರ_2, ಪುಟ_ಗಾತ್ರ,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;
	}

	/* Wait for the hardware initialization done */
	ಆಗ (!andes_llcache_init_ಮುಗಿದ(llcache))
		;

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ andes_llcache_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "andestech,llcache" },
	{},
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_andes_llcache = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = andes_llcache_ಹೊಂದಿಕೆ,
	.init = andes_llcache_probe,
};
