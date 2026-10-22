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

#تعریف_کرو LLCACHE_REG_CFG_وتھ		0x0
#تعریف_کرو LLCACHE_REG_CTRL_وتھ		0x8
#تعریف_کرو LLCACHE_REG_CCTL_CMD_وتھ	0x40
#تعریف_کرو LLCACHE_REG_CCTL_حالت_وتھ	0x80

#تعریف_کرو LLCACHE_REG_CFG_MAP_نقاب	ثنائی_ہندسہ(20)
#تعریف_کرو LLCACHE_REG_CTRL_EN_نقاب	ثنائی_ہندسہ(0)
#تعریف_کرو LLCACHE_REG_CTRL_INIT_نقاب	ثنائی_ہندسہ(14)
#تعریف_کرو LLCACHE_REG_CCTL_حالت_نقاب	GENMASK(3, 0)

#تعریف_کرو LLCACHE_WBINVAL_ALL		0x12

بنية andes_llcache {
	بنية cache_device dev;
	فراغ *ادھار_2;
	uint32_t cmd_stride;
	uint32_t حالت_stride;
	uint32_t حالت_core_stride;
};

#تعریف_کرو منزل_llcache(_dev) container_دا(_dev, بنية andes_llcache, dev)

ساكن bool andes_llcache_init_پورا(بنية andes_llcache *llcache)
{
	uint32_t llcache_ctrl;
	فراغ *ctrl_addr = (حرف *)llcache->ادھار_2 + LLCACHE_REG_CTRL_وتھ;

	llcache_ctrl = readl_relaxed(ctrl_addr);
	أرجع !کڈھو_میدان(llcache_ctrl, LLCACHE_REG_CTRL_INIT_نقاب);
}

ساكن bool andes_llcache_cctl_پورا(بنية andes_llcache *llcache, uint32_t hartid)
{
	uint32_t llcache_cctl_حالت;
	فراغ *cctl_حالت_addr = (حرف *)llcache->ادھار_2 + LLCACHE_REG_CCTL_حالت_وتھ +
				 hartid * llcache->حالت_stride;

	llcache_cctl_حالت = readl_relaxed(cctl_حالت_addr);
	أرجع !کڈھو_میدان(llcache_cctl_حالت,
			      LLCACHE_REG_CCTL_حالت_نقاب <<
			      hartid * llcache->حالت_core_stride);
}

ساكن صحيح andes_llcache_flush_all(بنية cache_device *dev)
{
	uint32_t hartid = current_hartid();
	بنية andes_llcache *llcache = منزل_llcache(dev);
	فراغ *cctl_cmd_addr = (حرف *)llcache->ادھار_2 + LLCACHE_REG_CCTL_CMD_وتھ +
			      hartid * llcache->cmd_stride;

	/*
	 * Each command register corresponds to one CPU core, so each CPU core
	 * should only use its command registers to do the cache operation.
	 */
	writel(LLCACHE_WBINVAL_ALL, cctl_cmd_addr);

	/* Wait for the command completion */
	طالما (!andes_llcache_cctl_پورا(llcache, hartid))
		;

	أرجع 0;
}

ساكن صحيح andes_llcache_enable(بنية cache_device *dev, bool enable)
{
	بنية andes_llcache *llcache = منزل_llcache(dev);
	u32 llcache_ctrl;
	فراغ *ctrl_addr = (حرف *)llcache->ادھار_2 + LLCACHE_REG_CTRL_وتھ;

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
		writel(llcache_ctrl | LLCACHE_REG_CTRL_EN_نقاب, ctrl_addr);
	} وإلا {
		llcache_ctrl = readl(ctrl_addr);
		atcsmu_write_scratch(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_نقاب);
		writel(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_نقاب, ctrl_addr);
	}

	llcache_ctrl = readl(ctrl_addr);
	إذا (enable != کڈھو_میدان(llcache_ctrl, LLCACHE_REG_CTRL_EN_نقاب))
		أرجع SBI_EFAIL;

	أرجع SBI_OK;
}

ساكن بنية cache_ops andes_llcache_ops = {
	.cache_flush_all = andes_llcache_flush_all,
	.cache_enable = andes_llcache_enable,
};

ساكن صحيح andes_llcache_probe(ثابت فراغ *fdt, صحيح nodeoff, ثابت بنية fdt_میل *میل)
{
	صحيح rc;
	u64 llcache_ادھار = 0, ناپ;
	بنية andes_llcache *llcache;
	بنية cache_device *dev;
	uint32_t llcache_cfg;

	rc = fdt_get_گرہ_addr_ناپ(fdt, nodeoff, 0, &llcache_ادھار, &ناپ);
	إذا (rc < 0 || !llcache_ادھار || !ناپ)
		أرجع SBI_ENODEV;

	llcache = sbi_zalloc(حجم(*llcache));
	إذا (!llcache)
		أرجع SBI_ENOMEM;

	dev = &llcache->dev;
	dev->ops = &andes_llcache_ops;
	rc = fdt_cache_جوڑو(fdt, nodeoff, dev);
	إذا (rc) {
		sbi_free(llcache);
		أرجع rc;
	}

	llcache->ادھار_2 = (فراغ *)(ulong)llcache_ادھار;
	llcache_cfg = readl_relaxed((حرف *)llcache->ادھار_2 + LLCACHE_REG_CFG_وتھ);

	/* Configurations for V1/V0 memory map */
	إذا (کڈھو_میدان(llcache_cfg, LLCACHE_REG_CFG_MAP_نقاب)) {
		llcache->cmd_stride = 0x1000;
		llcache->حالت_stride = 0x1000;
		llcache->حالت_core_stride = 0;
	} وإلا {
		llcache->cmd_stride = 0x10;
		llcache->حالت_stride = 0x0;
		llcache->حالت_core_stride = 4;
	}

	إذا (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_جڑ_جوڑو_memrange(
			(بنانشانی لما)llcache->ادھار_2, (بنانشانی لما)ناپ, ورقہ_ناپ,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		إذا (rc)
			أرجع rc;
	}

	/* Wait for the hardware initialization done */
	طالما (!andes_llcache_init_پورا(llcache))
		;

	أرجع SBI_OK;
}

ساكن ثابت بنية fdt_میل andes_llcache_میل[] = {
	{ .compatible = "andestech,llcache" },
	{},
};

ثابت بنية fdt_driver fdt_andes_llcache = {
	.میل_جدول = andes_llcache_میل,
	.init = andes_llcache_probe,
};
