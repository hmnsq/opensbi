/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/cache/fdt_cache.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_driver.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FLUSH64_CMD_ਨਿਸ਼ਾਨਾ_ALL		(0x2 << 3)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FLUSH64_CMD_TYPE_FLUSH		0x3ULL

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_PL2CACHE_CMD_QLEN	0xff

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_PL2CACHE_FLUSH64_OFF	0x200ULL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_PL2_CACHE_ਸਥਿਤੀ_OFF	0x208ULL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_PL2CACHE_CONFIG1_OFF	0x1000ULL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_PL2CACHE_CONFIG0_OFF	0x1008ULL

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FLUSH64_CMD_POS			56
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REGIONCLOCKDISABLE_ਨਕਾਬ		ਦੋਆਧਾਰੀ_ਅੰਕ(3)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONFIG0_ACCEPT_DIRTY_ਅੰਕੜੇ_ENABLE	ਦੋਆਧਾਰੀ_ਅੰਕ(24)

ਬਣਤਰ sifive_pl2_quirks {
	bool no_dirty_fill;
};

ਬਣਤਰ sifive_pl2 {
	ਬਣਤਰ cache_device dev;
	ਖਾਲੀ *addr;
	bool no_dirty_fill;
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਮੰਜ਼ਿਲ_pl2(_dev) container_ਦਾ(_dev, ਬਣਤਰ sifive_pl2, dev)

ਸਥਿਰ ਪੂਰਨ sifive_pl2_flush_all(ਬਣਤਰ cache_device *dev)
{
	ਬਣਤਰ sifive_pl2 *pl2_dev = ਮੰਜ਼ਿਲ_pl2(dev);
	ਅੱਖਰ *addr = pl2_dev->addr;
	u64 cmd = (FLUSH64_CMD_ਨਿਸ਼ਾਨਾ_ALL | FLUSH64_CMD_TYPE_FLUSH) << FLUSH64_CMD_POS;
	u32 config0;

	/*
	 * While flushing pl2 cache, a speculative load might causes a dirty line pull
	 * into PL2. It will cause the SiFive SMC0 refuse to enter the power gating.
	 * Disable the ACCEPT_DIRTY_DATA_ENABLE to avoid the issue.
	 */
	ਜੇ (pl2_dev->no_dirty_fill) {
		config0 = readl((ਖਾਲੀ *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
		config0 &= ~CONFIG0_ACCEPT_DIRTY_ਅੰਕੜੇ_ENABLE;
		writel(config0, (ਖਾਲੀ *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
	}

#ਜੇ __riscv_xlen != 32
	writeq(cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
#ਨਹੀਂਤਾਂ
	writel((u32)cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
	writel((u32)(cmd >> 32), addr + SIFIVE_PL2CACHE_FLUSH64_OFF + ਆਕਾਰ(u32));
#ਜੇ_ਅੰਤ
	ਕਰੋ {} ਜਦੋਂ (readl(addr + SIFIVE_PL2_CACHE_ਸਥਿਤੀ_OFF) & SIFIVE_PL2CACHE_CMD_QLEN);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ sifive_pl2_warm_init(ਬਣਤਰ cache_device *dev)
{
	ਬਣਤਰ sifive_pl2 *pl2_dev = ਮੰਜ਼ਿਲ_pl2(dev);
	ਅੱਖਰ *addr = pl2_dev->addr;
	u32 val;

	/* Enabling the clock gating */
	val = readl(addr + SIFIVE_PL2CACHE_CONFIG1_OFF);
	val &= (~REGIONCLOCKDISABLE_ਨਕਾਬ);
	writel(val, addr + SIFIVE_PL2CACHE_CONFIG1_OFF);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਬਣਤਰ cache_ops sifive_pl2_ops = {
	.warm_init = sifive_pl2_warm_init,
	.cache_flush_all = sifive_pl2_flush_all,
};

ਸਥਿਰ ਪੂਰਨ sifive_pl2_cold_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff, ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਅਚਲ ਬਣਤਰ sifive_pl2_quirks *quirk = ਮੇਲ->ਅੰਕੜੇ;
	ਬਣਤਰ sifive_pl2 *pl2_dev;
	ਬਣਤਰ cache_device *dev;
	u64 reg_addr;
	ਪੂਰਨ rc;

	/* find the pl2 control base address */
	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, 0, &reg_addr, ਨਿਰਾ);
	ਜੇ (rc < 0 && reg_addr)
		ਵਾਪਸ SBI_ENODEV;

	pl2_dev = sbi_zalloc(ਆਕਾਰ(*pl2_dev));
	ਜੇ (!pl2_dev)
		ਵਾਪਸ SBI_ENOMEM;

	dev = &pl2_dev->dev;
	dev->ops = &sifive_pl2_ops;
	dev->cpu_ਨਿੱਜੀ = true;

	rc = fdt_cache_ਜੋੜੋ(fdt, nodeoff, dev);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	pl2_dev->addr = (ਖਾਲੀ *)(uintptr_t)reg_addr;
	ਜੇ (quirk)
		pl2_dev->no_dirty_fill = quirk->no_dirty_fill;

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ sifive_pl2_quirks pl2cache2_quirks = {
	.no_dirty_fill = true,
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ sifive_pl2_quirks pl2cache0_quirks = {
	.no_dirty_fill = false,
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ sifive_pl2_ਮੇਲ[] = {
	{ .compatible = "sifive,pl2cache2", .ਅੰਕੜੇ = &pl2cache2_quirks },
	{ .compatible = "sifive,pl2cache1", .ਅੰਕੜੇ = &pl2cache0_quirks },
	{ .compatible = "sifive,pl2cache0", .ਅੰਕੜੇ = &pl2cache0_quirks },
	{},
};

ਬਣਤਰ fdt_driver fdt_sifive_pl2 = {
	.ਮੇਲ_ਸਾਰਣੀ = sifive_pl2_ਮੇਲ,
	.init = sifive_pl2_cold_init,
};
