/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/cache/fdt_cache.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>

#define FLUSH64_CMD_TARGET_ALL		(0x2 << 3)
#define FLUSH64_CMD_TYPE_FLUSH		0x3ULL

#define SIFIVE_PL2CACHE_CMD_QLEN	0xff

#define SIFIVE_PL2CACHE_FLUSH64_OFF	0x200ULL
#define SIFIVE_PL2CACHE_STATUS_OFF	0x208ULL
#define SIFIVE_PL2CACHE_CONFIG1_OFF	0x1000ULL
#define SIFIVE_PL2CACHE_CONFIG0_OFF	0x1008ULL

#define FLUSH64_CMD_POS			56
#define REGIONCLOCKDISABLE_MASK		BIT(3)

#define CONFIG0_ACCEPT_DIRTY_DATA_ENABLE	BIT(24)

ಸಂರಚನಾ sifive_pl2_quirks {
	bool no_dirty_fill;
};

ಸಂರಚನಾ sifive_pl2 {
	ಸಂರಚನಾ cache_device dev;
	ಶೂನ್ಯ *addr;
	bool no_dirty_fill;
};

#define to_pl2(_dev) container_of(_dev, ಸಂರಚನಾ sifive_pl2, dev)

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_pl2_flush_all(ಸಂರಚನಾ cache_device *dev)
{
	ಸಂರಚನಾ sifive_pl2 *pl2_dev = to_pl2(dev);
	ವರ್ಣ *addr = pl2_dev->addr;
	u64 cmd = (FLUSH64_CMD_TARGET_ALL | FLUSH64_CMD_TYPE_FLUSH) << FLUSH64_CMD_POS;
	u32 config0;

	/*
	 * While flushing pl2 cache, a speculative load might causes a dirty line pull
	 * into PL2. It will cause the SiFive SMC0 refuse to enter the power gating.
	 * Disable the ACCEPT_DIRTY_DATA_ENABLE to avoid the issue.
	 */
	ಯದಿ (pl2_dev->no_dirty_fill) {
		config0 = readl((ಶೂನ್ಯ *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
		config0 &= ~CONFIG0_ACCEPT_DIRTY_DATA_ENABLE;
		writel(config0, (ಶೂನ್ಯ *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
	}

#ಯದಿ __riscv_xlen != 32
	writeq(cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
#ಅನ್ಯಥಾ
	writel((u32)cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
	writel((u32)(cmd >> 32), addr + SIFIVE_PL2CACHE_FLUSH64_OFF + ಪರಿಮಾಣ(u32));
#endif
	ಕುರು {} ಯಾವತ್ (readl(addr + SIFIVE_PL2CACHE_STATUS_OFF) & SIFIVE_PL2CACHE_CMD_QLEN);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_pl2_warm_init(ಸಂರಚನಾ cache_device *dev)
{
	ಸಂರಚನಾ sifive_pl2 *pl2_dev = to_pl2(dev);
	ವರ್ಣ *addr = pl2_dev->addr;
	u32 val;

	/* Enabling the clock gating */
	val = readl(addr + SIFIVE_PL2CACHE_CONFIG1_OFF);
	val &= (~REGIONCLOCKDISABLE_MASK);
	writel(val, addr + SIFIVE_PL2CACHE_CONFIG1_OFF);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಸಂರಚನಾ cache_ops sifive_pl2_ops = {
	.warm_init = sifive_pl2_warm_init,
	.cache_flush_all = sifive_pl2_flush_all,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_pl2_cold_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff, ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಧ್ರುವ ಸಂರಚನಾ sifive_pl2_quirks *quirk = match->data;
	ಸಂರಚನಾ sifive_pl2 *pl2_dev;
	ಸಂರಚನಾ cache_device *dev;
	u64 reg_addr;
	ಪೂರ್ಣಾಙ್ಕ rc;

	/* find the pl2 control base address */
	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, 0, &reg_addr, ರಿಕ್ತ);
	ಯದಿ (rc < 0 && reg_addr)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	pl2_dev = sbi_zalloc(ಪರಿಮಾಣ(*pl2_dev));
	ಯದಿ (!pl2_dev)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

	dev = &pl2_dev->dev;
	dev->ops = &sifive_pl2_ops;
	dev->cpu_private = true;

	rc = fdt_cache_add(fdt, nodeoff, dev);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	pl2_dev->addr = (ಶೂನ್ಯ *)(uintptr_t)reg_addr;
	ಯದಿ (quirk)
		pl2_dev->no_dirty_fill = quirk->no_dirty_fill;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ sifive_pl2_quirks pl2cache2_quirks = {
	.no_dirty_fill = true,
};

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ sifive_pl2_quirks pl2cache0_quirks = {
	.no_dirty_fill = false,
};

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match sifive_pl2_match[] = {
	{ .compatible = "sifive,pl2cache2", .data = &pl2cache2_quirks },
	{ .compatible = "sifive,pl2cache1", .data = &pl2cache0_quirks },
	{ .compatible = "sifive,pl2cache0", .data = &pl2cache0_quirks },
	{},
};

ಸಂರಚನಾ fdt_driver fdt_sifive_pl2 = {
	.match_table = sifive_pl2_match,
	.init = sifive_pl2_cold_init,
};
