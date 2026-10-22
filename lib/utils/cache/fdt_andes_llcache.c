/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi_utils/cache/fdt_cache.h>
#ضمن <sbi_utils/fdt/fdt_driver.h>
#ضمن <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#تعریف_کړه LLCACHE_REG_CFG_دننه_واټن		0x0
#تعریف_کړه LLCACHE_REG_CTRL_دننه_واټن		0x8
#تعریف_کړه LLCACHE_REG_CCTL_CMD_دننه_واټن	0x40
#تعریف_کړه LLCACHE_REG_CCTL_حالت_دننه_واټن	0x80

#تعریف_کړه LLCACHE_REG_CFG_MAP_ماسک	بټ(20)
#تعریف_کړه LLCACHE_REG_CTRL_EN_ماسک	بټ(0)
#تعریف_کړه LLCACHE_REG_CTRL_INIT_ماسک	بټ(14)
#تعریف_کړه LLCACHE_REG_CCTL_حالت_ماسک	GENMASK(3, 0)

#تعریف_کړه LLCACHE_WBINVAL_ALL		0x12

بنية andes_llcache {
	بنية cache_device dev;
	فراغ *بنسټ_2;
	uint32_t cmd_stride;
	uint32_t حالت_stride;
	uint32_t حالت_core_stride;
};

#تعریف_کړه منزل_llcache(_dev) container_د(_dev, بنية andes_llcache, dev)

ساكن bool andes_llcache_init_بشپړ(بنية andes_llcache *llcache)
{
	uint32_t llcache_ctrl;
	فراغ *ctrl_addr = (حرف *)llcache->بنسټ_2 + LLCACHE_REG_CTRL_دننه_واټن;

	llcache_ctrl = readl_relaxed(ctrl_addr);
	أرجع !ایستل_ډګر(llcache_ctrl, LLCACHE_REG_CTRL_INIT_ماسک);
}

ساكن bool andes_llcache_cctl_بشپړ(بنية andes_llcache *llcache, uint32_t hartid)
{
	uint32_t llcache_cctl_حالت;
	فراغ *cctl_حالت_addr = (حرف *)llcache->بنسټ_2 + LLCACHE_REG_CCTL_حالت_دننه_واټن +
				 hartid * llcache->حالت_stride;

	llcache_cctl_حالت = readl_relaxed(cctl_حالت_addr);
	أرجع !ایستل_ډګر(llcache_cctl_حالت,
			      LLCACHE_REG_CCTL_حالت_ماسک <<
			      hartid * llcache->حالت_core_stride);
}

ساكن صحيح andes_llcache_flush_all(بنية cache_device *dev)
{
	uint32_t hartid = current_hartid();
	بنية andes_llcache *llcache = منزل_llcache(dev);
	فراغ *cctl_cmd_addr = (حرف *)llcache->بنسټ_2 + LLCACHE_REG_CCTL_CMD_دننه_واټن +
			      hartid * llcache->cmd_stride;

	/*
	 * Each command register corresponds to one CPU core, so each CPU core
	 * should only use its command registers to do the cache operation.
	 */
	writel(LLCACHE_WBINVAL_ALL, cctl_cmd_addr);

	/* Wait for the command completion */
	طالما (!andes_llcache_cctl_بشپړ(llcache, hartid))
		;

	أرجع 0;
}

ساكن صحيح andes_llcache_enable(بنية cache_device *dev, bool enable)
{
	بنية andes_llcache *llcache = منزل_llcache(dev);
	u32 llcache_ctrl;
	فراغ *ctrl_addr = (حرف *)llcache->بنسټ_2 + LLCACHE_REG_CTRL_دننه_واټن;

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
	إذا (enable) {
		llcache_ctrl = atcsmu_read_scratch();
		writel(llcache_ctrl, ctrl_addr);
		writel(llcache_ctrl | LLCACHE_REG_CTRL_EN_ماسک, ctrl_addr);
	} وإلا {
		llcache_ctrl = readl(ctrl_addr);
		atcsmu_write_scratch(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_ماسک);
		writel(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_ماسک, ctrl_addr);
	}

	llcache_ctrl = readl(ctrl_addr);
	إذا (enable != ایستل_ډګر(llcache_ctrl, LLCACHE_REG_CTRL_EN_ماسک))
		أرجع SBI_EFAIL;

	أرجع SBI_OK;
}

ساكن بنية cache_ops andes_llcache_ops = {
	.cache_flush_all = andes_llcache_flush_all,
	.cache_enable = andes_llcache_enable,
};

ساكن صحيح andes_llcache_probe(ثابت فراغ *fdt, صحيح nodeoff, ثابت بنية fdt_سمون *سمون)
{
	صحيح rc;
	u64 llcache_بنسټ = 0, اندازه;
	بنية andes_llcache *llcache;
	بنية cache_device *dev;
	uint32_t llcache_cfg;

	rc = fdt_get_غوټه_addr_اندازه(fdt, nodeoff, 0, &llcache_بنسټ, &اندازه);
	إذا (rc < 0 || !llcache_بنسټ || !اندازه)
		أرجع SBI_ENODEV;

	llcache = sbi_zalloc(حجم(*llcache));
	إذا (!llcache)
		أرجع SBI_ENOMEM;

	dev = &llcache->dev;
	dev->ops = &andes_llcache_ops;
	rc = fdt_cache_جمع_کړه(fdt, nodeoff, dev);
	إذا (rc) {
		sbi_free(llcache);
		أرجع rc;
	}

	llcache->بنسټ_2 = (فراغ *)(ulong)llcache_بنسټ;
	llcache_cfg = readl_relaxed((حرف *)llcache->بنسټ_2 + LLCACHE_REG_CFG_دننه_واټن);

	/* Configurations for V1/V0 memory map */
	إذا (ایستل_ډګر(llcache_cfg, LLCACHE_REG_CFG_MAP_ماسک)) {
		llcache->cmd_stride = 0x1000;
		llcache->حالت_stride = 0x1000;
		llcache->حالت_core_stride = 0;
	} وإلا {
		llcache->cmd_stride = 0x10;
		llcache->حالت_stride = 0x0;
		llcache->حالت_core_stride = 4;
	}

	إذا (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_ریښه_جمع_کړه_memrange(
			(بینښه اوږد)llcache->بنسټ_2, (بینښه اوږد)اندازه, پاڼه_اندازه,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		إذا (rc)
			أرجع rc;
	}

	/* Wait for the hardware initialization done */
	طالما (!andes_llcache_init_بشپړ(llcache))
		;

	أرجع SBI_OK;
}

ساكن ثابت بنية fdt_سمون andes_llcache_سمون[] = {
	{ .compatible = "andestech,llcache" },
	{},
};

ثابت بنية fdt_driver fdt_andes_llcache = {
	.سمون_جدول = andes_llcache_سمون,
	.init = andes_llcache_probe,
};
