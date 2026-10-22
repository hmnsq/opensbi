/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi_utils/cache/fdt_cache.h>
#شامل <sbi_utils/fdt/fdt_driver.h>
#شامل <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#تعریف_کرو LLCACHE_REG_CFG_انحراف		0x0
#تعریف_کرو LLCACHE_REG_CTRL_انحراف		0x8
#تعریف_کرو LLCACHE_REG_CCTL_CMD_انحراف	0x40
#تعریف_کرو LLCACHE_REG_CCTL_حالت_انحراف	0x80

#تعریف_کرو LLCACHE_REG_CFG_MAP_نقاب	ثنائی_ہندسہ(20)
#تعریف_کرو LLCACHE_REG_CTRL_EN_نقاب	ثنائی_ہندسہ(0)
#تعریف_کرو LLCACHE_REG_CTRL_INIT_نقاب	ثنائی_ہندسہ(14)
#تعریف_کرو LLCACHE_REG_CCTL_حالت_نقاب	GENMASK(3, 0)

#تعریف_کرو LLCACHE_WBINVAL_ALL		0x12

ساخت andes_llcache {
	ساخت cache_device dev;
	خالی *بنیاد_2;
	uint32_t cmd_stride;
	uint32_t حالت_stride;
	uint32_t حالت_core_stride;
};

#تعریف_کرو مقصد_llcache(_dev) container_کا(_dev, ساخت andes_llcache, dev)

ساکن bool andes_llcache_init_مکمل(ساخت andes_llcache *llcache)
{
	uint32_t llcache_ctrl;
	خالی *ctrl_addr = (حرف *)llcache->بنیاد_2 + LLCACHE_REG_CTRL_انحراف;

	llcache_ctrl = readl_relaxed(ctrl_addr);
	واپس !نکالو_میدان(llcache_ctrl, LLCACHE_REG_CTRL_INIT_نقاب);
}

ساکن bool andes_llcache_cctl_مکمل(ساخت andes_llcache *llcache, uint32_t hartid)
{
	uint32_t llcache_cctl_حالت;
	خالی *cctl_حالت_addr = (حرف *)llcache->بنیاد_2 + LLCACHE_REG_CCTL_حالت_انحراف +
				 hartid * llcache->حالت_stride;

	llcache_cctl_حالت = readl_relaxed(cctl_حالت_addr);
	واپس !نکالو_میدان(llcache_cctl_حالت,
			      LLCACHE_REG_CCTL_حالت_نقاب <<
			      hartid * llcache->حالت_core_stride);
}

ساکن صحیح andes_llcache_flush_all(ساخت cache_device *dev)
{
	uint32_t hartid = current_hartid();
	ساخت andes_llcache *llcache = مقصد_llcache(dev);
	خالی *cctl_cmd_addr = (حرف *)llcache->بنیاد_2 + LLCACHE_REG_CCTL_CMD_انحراف +
			      hartid * llcache->cmd_stride;

	/*
	 * Each command register corresponds to one CPU core, so each CPU core
	 * should only use its command registers to do the cache operation.
	 */
	writel(LLCACHE_WBINVAL_ALL, cctl_cmd_addr);

	/* Wait for the command completion */
	جب (!andes_llcache_cctl_مکمل(llcache, hartid))
		;

	واپس 0;
}

ساکن صحیح andes_llcache_enable(ساخت cache_device *dev, bool enable)
{
	ساخت andes_llcache *llcache = مقصد_llcache(dev);
	u32 llcache_ctrl;
	خالی *ctrl_addr = (حرف *)llcache->بنیاد_2 + LLCACHE_REG_CTRL_انحراف;

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
	اگر (enable) {
		llcache_ctrl = atcsmu_read_scratch();
		writel(llcache_ctrl, ctrl_addr);
		writel(llcache_ctrl | LLCACHE_REG_CTRL_EN_نقاب, ctrl_addr);
	} ورنہ {
		llcache_ctrl = readl(ctrl_addr);
		atcsmu_write_scratch(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_نقاب);
		writel(llcache_ctrl & ~LLCACHE_REG_CTRL_EN_نقاب, ctrl_addr);
	}

	llcache_ctrl = readl(ctrl_addr);
	اگر (enable != نکالو_میدان(llcache_ctrl, LLCACHE_REG_CTRL_EN_نقاب))
		واپس SBI_EFAIL;

	واپس SBI_OK;
}

ساکن ساخت cache_ops andes_llcache_ops = {
	.cache_flush_all = andes_llcache_flush_all,
	.cache_enable = andes_llcache_enable,
};

ساکن صحیح andes_llcache_probe(ثابت خالی *fdt, صحیح nodeoff, ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح rc;
	u64 llcache_بنیاد = 0, سائز;
	ساخت andes_llcache *llcache;
	ساخت cache_device *dev;
	uint32_t llcache_cfg;

	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoff, 0, &llcache_بنیاد, &سائز);
	اگر (rc < 0 || !llcache_بنیاد || !سائز)
		واپس SBI_ENODEV;

	llcache = sbi_zalloc(حجم(*llcache));
	اگر (!llcache)
		واپس SBI_ENOMEM;

	dev = &llcache->dev;
	dev->ops = &andes_llcache_ops;
	rc = fdt_cache_جوڑو(fdt, nodeoff, dev);
	اگر (rc) {
		sbi_free(llcache);
		واپس rc;
	}

	llcache->بنیاد_2 = (خالی *)(ulong)llcache_بنیاد;
	llcache_cfg = readl_relaxed((حرف *)llcache->بنیاد_2 + LLCACHE_REG_CFG_انحراف);

	/* Configurations for V1/V0 memory map */
	اگر (نکالو_میدان(llcache_cfg, LLCACHE_REG_CFG_MAP_نقاب)) {
		llcache->cmd_stride = 0x1000;
		llcache->حالت_stride = 0x1000;
		llcache->حالت_core_stride = 0;
	} ورنہ {
		llcache->cmd_stride = 0x10;
		llcache->حالت_stride = 0x0;
		llcache->حالت_core_stride = 4;
	}

	اگر (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_جڑ_جوڑو_memrange(
			(غیرعلامتی طویل)llcache->بنیاد_2, (غیرعلامتی طویل)سائز, صفحہ_سائز,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		اگر (rc)
			واپس rc;
	}

	/* Wait for the hardware initialization done */
	جب (!andes_llcache_init_مکمل(llcache))
		;

	واپس SBI_OK;
}

ساکن ثابت ساخت fdt_مطابقت andes_llcache_مطابقت[] = {
	{ .compatible = "andestech,llcache" },
	{},
};

ثابت ساخت fdt_driver fdt_andes_llcache = {
	.مطابقت_جدول = andes_llcache_مطابقت,
	.init = andes_llcache_probe,
};
