/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/cache/fdt_cache.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_driver.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LLCACHE_REG_CFG_ਵਿੱਥ		0x0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LLCACHE_REG_CTRL_ਵਿੱਥ		0x8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LLCACHE_REG_CCTL_CMD_ਵਿੱਥ	0x40
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LLCACHE_REG_CCTL_ਸਥਿਤੀ_ਵਿੱਥ	0x80

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LLCACHE_REG_CFG_MAP_ਨਕਾਬ	ਦੋਆਧਾਰੀ_ਅੰਕ(20)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LLCACHE_REG_CTRL_EN_ਨਕਾਬ	ਦੋਆਧਾਰੀ_ਅੰਕ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LLCACHE_REG_CTRL_INIT_ਨਕਾਬ	ਦੋਆਧਾਰੀ_ਅੰਕ(14)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LLCACHE_REG_CCTL_ਸਥਿਤੀ_ਨਕਾਬ	GENMASK(3, 0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LLCACHE_WBINVAL_ALL		0x12

ਬਣਤਰ andes_llcache {
	ਬਣਤਰ cache_device dev;
	ਖਾਲੀ *ਅਧਾਰ_2;
	uint32_t cmd_stride;
	uint32_t ਸਥਿਤੀ_stride;
	uint32_t ਸਥਿਤੀ_core_stride;
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਮੰਜ਼ਿਲ_llcache(_dev) container_ਦਾ(_dev, ਬਣਤਰ andes_llcache, dev)

ਸਥਿਰ bool andes_llcache_init_ਪੂਰਾ(ਬਣਤਰ andes_llcache *llcache)
{
	uint32_t llcache_ctrl;
	ਖਾਲੀ *ctrl_addr = (ਅੱਖਰ *)llcache->ਅਧਾਰ_2 + LLCACHE_REG_CTRL_ਵਿੱਥ;

	llcache_ctrl = readl_relaxed(ctrl_addr);
	ਵਾਪਸ !ਕੱਢੋ_ਖੇਤਰ(llcache_ctrl, LLCACHE_REG_CTRL_INIT_ਨਕਾਬ);
}

ਸਥਿਰ bool andes_llcache_cctl_ਪੂਰਾ(ਬਣਤਰ andes_llcache *llcache, uint32_t hartid)
{
	uint32_t llcache_cctl_ਸਥਿਤੀ;
	ਖਾਲੀ *cctl_ਸਥਿਤੀ_addr = (ਅੱਖਰ *)llcache->ਅਧਾਰ_2 + LLCACHE_REG_CCTL_ਸਥਿਤੀ_ਵਿੱਥ +
				 hartid * llcache->ਸਥਿਤੀ_stride;

	llcache_cctl_ਸਥਿਤੀ = readl_relaxed(cctl_ਸਥਿਤੀ_addr);
	ਵਾਪਸ !ਕੱਢੋ_ਖੇਤਰ(llcache_cctl_ਸਥਿਤੀ,
			      LLCACHE_REG_CCTL_ਸਥਿਤੀ_ਨਕਾਬ <<
			      hartid * llcache->ਸਥਿਤੀ_core_stride);
}

ਸਥਿਰ ਪੂਰਨ andes_llcache_flush_all(ਬਣਤਰ cache_device *dev)
{
	uint32_t hartid = current_hartid();
	ਬਣਤਰ andes_llcache *llcache = ਮੰਜ਼ਿਲ_llcache(dev);
	ਖਾਲੀ *cctl_cmd_addr = (ਅੱਖਰ *)llcache->ਅਧਾਰ_2 + LLCACHE_REG_CCTL_CMD_ਵਿੱਥ +
			      hartid * llcache->cmd_stride;

	/*
	 * Each command register corresponds to one CPU core, so each CPU core
	 * should only use its command registers to do the cache operation.
	 */
	writel(LLCACHE_WBINVAL_ALL, cctl_cmd_addr);

	/* Wait for the command completion */
	ਜਦੋਂ (!andes_llcache_cctl_ਪੂਰਾ(llcache, hartid))
		;

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ andes_llcache_enable(ਬਣਤਰ cache_device *dev, bool enable)
{
	ਬਣਤਰ andes_llcache *llcache = ਮੰਜ਼ਿਲ_llcache(dev);
	u32 llcache_ctrl;
	ਖਾਲੀ *ctrl_addr = (ਅੱਖਰ *)llcache->ਅਧਾਰ_2 + LLCACHE_REG_CTRL_ਵਿੱਥ;

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
	ਜੇ (enable) {
		llcache_ctrl = atcsmu_read_scratch();
		writel(llcache_ctrl, ctrl_addr);
		writel(llcache_ctrl | LLCACHE_REG_CTRL_EN_ਨਕਾਬ, ctrl_addr);
	} ਨਹੀਂਤਾਂ {
		llcache_ctrl = readl(ctrl_addr);
		atcsmu_write_scratch(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_ਨਕਾਬ);
		writel(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_ਨਕਾਬ, ctrl_addr);
	}

	llcache_ctrl = readl(ctrl_addr);
	ਜੇ (enable != ਕੱਢੋ_ਖੇਤਰ(llcache_ctrl, LLCACHE_REG_CTRL_EN_ਨਕਾਬ))
		ਵਾਪਸ SBI_EFAIL;

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਬਣਤਰ cache_ops andes_llcache_ops = {
	.cache_flush_all = andes_llcache_flush_all,
	.cache_enable = andes_llcache_enable,
};

ਸਥਿਰ ਪੂਰਨ andes_llcache_probe(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff, ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ rc;
	u64 llcache_ਅਧਾਰ = 0, ਮਾਪ;
	ਬਣਤਰ andes_llcache *llcache;
	ਬਣਤਰ cache_device *dev;
	uint32_t llcache_cfg;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, 0, &llcache_ਅਧਾਰ, &ਮਾਪ);
	ਜੇ (rc < 0 || !llcache_ਅਧਾਰ || !ਮਾਪ)
		ਵਾਪਸ SBI_ENODEV;

	llcache = sbi_zalloc(ਆਕਾਰ(*llcache));
	ਜੇ (!llcache)
		ਵਾਪਸ SBI_ENOMEM;

	dev = &llcache->dev;
	dev->ops = &andes_llcache_ops;
	rc = fdt_cache_ਜੋੜੋ(fdt, nodeoff, dev);
	ਜੇ (rc) {
		sbi_free(llcache);
		ਵਾਪਸ rc;
	}

	llcache->ਅਧਾਰ_2 = (ਖਾਲੀ *)(ulong)llcache_ਅਧਾਰ;
	llcache_cfg = readl_relaxed((ਅੱਖਰ *)llcache->ਅਧਾਰ_2 + LLCACHE_REG_CFG_ਵਿੱਥ);

	/* Configurations for V1/V0 memory map */
	ਜੇ (ਕੱਢੋ_ਖੇਤਰ(llcache_cfg, LLCACHE_REG_CFG_MAP_ਨਕਾਬ)) {
		llcache->cmd_stride = 0x1000;
		llcache->ਸਥਿਤੀ_stride = 0x1000;
		llcache->ਸਥਿਤੀ_core_stride = 0;
	} ਨਹੀਂਤਾਂ {
		llcache->cmd_stride = 0x10;
		llcache->ਸਥਿਤੀ_stride = 0x0;
		llcache->ਸਥਿਤੀ_core_stride = 4;
	}

	ਜੇ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_ਜੜ੍ਹ_ਜੋੜੋ_memrange(
			(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)llcache->ਅਧਾਰ_2, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)ਮਾਪ, ਪੰਨਾ_ਮਾਪ,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		ਜੇ (rc)
			ਵਾਪਸ rc;
	}

	/* Wait for the hardware initialization done */
	ਜਦੋਂ (!andes_llcache_init_ਪੂਰਾ(llcache))
		;

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ andes_llcache_ਮੇਲ[] = {
	{ .compatible = "andestech,llcache" },
	{},
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_andes_llcache = {
	.ਮੇਲ_ਸਾਰਣੀ = andes_llcache_ਮੇਲ,
	.init = andes_llcache_probe,
};
