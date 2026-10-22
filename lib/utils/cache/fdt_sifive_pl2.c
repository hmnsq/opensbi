/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#شامل <libfdt.h>
#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi_utils/cache/fdt_cache.h>
#شامل <sbi_utils/fdt/fdt_driver.h>

#تعریف_کرو FLUSH64_CMD_ہدف_ALL		(0x2 << 3)
#تعریف_کرو FLUSH64_CMD_TYPE_FLUSH		0x3ULL

#تعریف_کرو SIFIVE_PL2CACHE_CMD_QLEN	0xff

#تعریف_کرو SIFIVE_PL2CACHE_FLUSH64_OFF	0x200ULL
#تعریف_کرو SIFIVE_PL2_CACHE_حالت_OFF	0x208ULL
#تعریف_کرو SIFIVE_PL2CACHE_CONFIG1_OFF	0x1000ULL
#تعریف_کرو SIFIVE_PL2CACHE_CONFIG0_OFF	0x1008ULL

#تعریف_کرو FLUSH64_CMD_POS			56
#تعریف_کرو REGIONCLOCKDISABLE_نقاب		ثنائی_ہندسہ(3)

#تعریف_کرو CONFIG0_ACCEPT_DIRTY_ڈیٹا_ENABLE	ثنائی_ہندسہ(24)

ساخت sifive_pl2_quirks {
	bool no_dirty_fill;
};

ساخت sifive_pl2 {
	ساخت cache_device dev;
	خالی *addr;
	bool no_dirty_fill;
};

#تعریف_کرو مقصد_pl2(_dev) container_کا(_dev, ساخت sifive_pl2, dev)

ساکن صحیح sifive_pl2_flush_all(ساخت cache_device *dev)
{
	ساخت sifive_pl2 *pl2_dev = مقصد_pl2(dev);
	حرف *addr = pl2_dev->addr;
	u64 cmd = (FLUSH64_CMD_ہدف_ALL | FLUSH64_CMD_TYPE_FLUSH) << FLUSH64_CMD_POS;
	u32 config0;

	/*
	 * While flushing pl2 cache, a speculative load might causes a dirty line pull
	 * into PL2. It will cause the SiFive SMC0 refuse to enter the power gating.
	 * Disable the ACCEPT_DIRTY_DATA_ENABLE to avoid the issue.
	 */
	اگر (pl2_dev->no_dirty_fill) {
		config0 = readl((خالی *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
		config0 &= ~CONFIG0_ACCEPT_DIRTY_ڈیٹا_ENABLE;
		writel(config0, (خالی *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
	}

#اگر __riscv_xlen != 32
	writeq(cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
#ورنہ
	writel((u32)cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
	writel((u32)(cmd >> 32), addr + SIFIVE_PL2CACHE_FLUSH64_OFF + حجم(u32));
#اگر_ختم
	کرو {} جب (readl(addr + SIFIVE_PL2_CACHE_حالت_OFF) & SIFIVE_PL2CACHE_CMD_QLEN);

	واپس 0;
}

ساکن صحیح sifive_pl2_warm_init(ساخت cache_device *dev)
{
	ساخت sifive_pl2 *pl2_dev = مقصد_pl2(dev);
	حرف *addr = pl2_dev->addr;
	u32 val;

	/* Enabling the clock gating */
	val = readl(addr + SIFIVE_PL2CACHE_CONFIG1_OFF);
	val &= (~REGIONCLOCKDISABLE_نقاب);
	writel(val, addr + SIFIVE_PL2CACHE_CONFIG1_OFF);

	واپس 0;
}

ساکن ساخت cache_ops sifive_pl2_ops = {
	.warm_init = sifive_pl2_warm_init,
	.cache_flush_all = sifive_pl2_flush_all,
};

ساکن صحیح sifive_pl2_cold_init(ثابت خالی *fdt, صحیح nodeoff, ثابت ساخت fdt_مطابقت *مطابقت)
{
	ثابت ساخت sifive_pl2_quirks *quirk = مطابقت->ڈیٹا;
	ساخت sifive_pl2 *pl2_dev;
	ساخت cache_device *dev;
	u64 reg_addr;
	صحیح rc;

	/* find the pl2 control base address */
	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoff, 0, &reg_addr, تہی);
	اگر (rc < 0 && reg_addr)
		واپس SBI_ENODEV;

	pl2_dev = sbi_zalloc(حجم(*pl2_dev));
	اگر (!pl2_dev)
		واپس SBI_ENOMEM;

	dev = &pl2_dev->dev;
	dev->ops = &sifive_pl2_ops;
	dev->cpu_نجی = true;

	rc = fdt_cache_جوڑو(fdt, nodeoff, dev);
	اگر (rc)
		واپس rc;

	pl2_dev->addr = (خالی *)(uintptr_t)reg_addr;
	اگر (quirk)
		pl2_dev->no_dirty_fill = quirk->no_dirty_fill;

	واپس 0;
}

ساکن ثابت ساخت sifive_pl2_quirks pl2cache2_quirks = {
	.no_dirty_fill = true,
};

ساکن ثابت ساخت sifive_pl2_quirks pl2cache0_quirks = {
	.no_dirty_fill = false,
};

ساکن ثابت ساخت fdt_مطابقت sifive_pl2_مطابقت[] = {
	{ .compatible = "sifive,pl2cache2", .ڈیٹا = &pl2cache2_quirks },
	{ .compatible = "sifive,pl2cache1", .ڈیٹا = &pl2cache0_quirks },
	{ .compatible = "sifive,pl2cache0", .ڈیٹا = &pl2cache0_quirks },
	{},
};

ساخت fdt_driver fdt_sifive_pl2 = {
	.مطابقت_جدول = sifive_pl2_مطابقت,
	.init = sifive_pl2_cold_init,
};
