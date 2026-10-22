/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ضمن <libfdt.h>
#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi_utils/cache/fdt_cache.h>
#ضمن <sbi_utils/fdt/fdt_driver.h>

#تعریف_کرو FLUSH64_CMD_نشانہ_ALL		(0x2 << 3)
#تعریف_کرو FLUSH64_CMD_TYPE_FLUSH		0x3ULL

#تعریف_کرو SIFIVE_PL2CACHE_CMD_QLEN	0xff

#تعریف_کرو SIFIVE_PL2CACHE_FLUSH64_OFF	0x200ULL
#تعریف_کرو SIFIVE_PL2_CACHE_حالت_OFF	0x208ULL
#تعریف_کرو SIFIVE_PL2CACHE_CONFIG1_OFF	0x1000ULL
#تعریف_کرو SIFIVE_PL2CACHE_CONFIG0_OFF	0x1008ULL

#تعریف_کرو FLUSH64_CMD_POS			56
#تعریف_کرو REGIONCLOCKDISABLE_نقاب		ثنائی_ہندسہ(3)

#تعریف_کرو CONFIG0_ACCEPT_DIRTY_انکڑے_ENABLE	ثنائی_ہندسہ(24)

بنية sifive_pl2_quirks {
	bool no_dirty_fill;
};

بنية sifive_pl2 {
	بنية cache_device dev;
	فراغ *addr;
	bool no_dirty_fill;
};

#تعریف_کرو منزل_pl2(_dev) container_دا(_dev, بنية sifive_pl2, dev)

ساكن صحيح sifive_pl2_flush_all(بنية cache_device *dev)
{
	بنية sifive_pl2 *pl2_dev = منزل_pl2(dev);
	حرف *addr = pl2_dev->addr;
	u64 cmd = (FLUSH64_CMD_نشانہ_ALL | FLUSH64_CMD_TYPE_FLUSH) << FLUSH64_CMD_POS;
	u32 config0;

	/*
	 * While flushing pl2 cache, a speculative load might causes a dirty line pull
	 * into PL2. It will cause the SiFive SMC0 refuse to enter the power gating.
	 * Disable the ACCEPT_DIRTY_DATA_ENABLE to avoid the issue.
	 */
	إذا (pl2_dev->no_dirty_fill) {
		config0 = readl((فراغ *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
		config0 &= ~CONFIG0_ACCEPT_DIRTY_انکڑے_ENABLE;
		writel(config0, (فراغ *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
	}

#إذا __riscv_xlen != 32
	writeq(cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
#وإلا
	writel((u32)cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
	writel((u32)(cmd >> 32), addr + SIFIVE_PL2CACHE_FLUSH64_OFF + حجم(u32));
#جے_انت
	افعل {} طالما (readl(addr + SIFIVE_PL2_CACHE_حالت_OFF) & SIFIVE_PL2CACHE_CMD_QLEN);

	أرجع 0;
}

ساكن صحيح sifive_pl2_warm_init(بنية cache_device *dev)
{
	بنية sifive_pl2 *pl2_dev = منزل_pl2(dev);
	حرف *addr = pl2_dev->addr;
	u32 val;

	/* Enabling the clock gating */
	val = readl(addr + SIFIVE_PL2CACHE_CONFIG1_OFF);
	val &= (~REGIONCLOCKDISABLE_نقاب);
	writel(val, addr + SIFIVE_PL2CACHE_CONFIG1_OFF);

	أرجع 0;
}

ساكن بنية cache_ops sifive_pl2_ops = {
	.warm_init = sifive_pl2_warm_init,
	.cache_flush_all = sifive_pl2_flush_all,
};

ساكن صحيح sifive_pl2_cold_init(ثابت فراغ *fdt, صحيح nodeoff, ثابت بنية fdt_میل *میل)
{
	ثابت بنية sifive_pl2_quirks *quirk = میل->انکڑے;
	بنية sifive_pl2 *pl2_dev;
	بنية cache_device *dev;
	u64 reg_addr;
	صحيح rc;

	/* find the pl2 control base address */
	rc = fdt_get_گرہ_addr_ناپ(fdt, nodeoff, 0, &reg_addr, عدم);
	إذا (rc < 0 && reg_addr)
		أرجع SBI_ENODEV;

	pl2_dev = sbi_zalloc(حجم(*pl2_dev));
	إذا (!pl2_dev)
		أرجع SBI_ENOMEM;

	dev = &pl2_dev->dev;
	dev->ops = &sifive_pl2_ops;
	dev->cpu_نجی = true;

	rc = fdt_cache_جوڑو(fdt, nodeoff, dev);
	إذا (rc)
		أرجع rc;

	pl2_dev->addr = (فراغ *)(uintptr_t)reg_addr;
	إذا (quirk)
		pl2_dev->no_dirty_fill = quirk->no_dirty_fill;

	أرجع 0;
}

ساكن ثابت بنية sifive_pl2_quirks pl2cache2_quirks = {
	.no_dirty_fill = true,
};

ساكن ثابت بنية sifive_pl2_quirks pl2cache0_quirks = {
	.no_dirty_fill = false,
};

ساكن ثابت بنية fdt_میل sifive_pl2_میل[] = {
	{ .compatible = "sifive,pl2cache2", .انکڑے = &pl2cache2_quirks },
	{ .compatible = "sifive,pl2cache1", .انکڑے = &pl2cache0_quirks },
	{ .compatible = "sifive,pl2cache0", .انکڑے = &pl2cache0_quirks },
	{},
};

بنية fdt_driver fdt_sifive_pl2 = {
	.میل_جدول = sifive_pl2_میل,
	.init = sifive_pl2_cold_init,
};
