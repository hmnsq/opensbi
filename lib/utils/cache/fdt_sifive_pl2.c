/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#समावेश <libfdt.h>
#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi_utils/cache/fdt_cache.h>
#समावेश <sbi_utils/fdt/fdt_driver.h>

#घोषणा FLUSH64_CMD_TARGET_ALL		(0x2 << 3)
#घोषणा FLUSH64_CMD_TYPE_FLUSH		0x3ULL

#घोषणा SIFIVE_PL2CACHE_CMD_QLEN	0xff

#घोषणा SIFIVE_PL2CACHE_FLUSH64_OFF	0x200ULL
#घोषणा SIFIVE_PL2CACHE_STATUS_OFF	0x208ULL
#घोषणा SIFIVE_PL2CACHE_CONFIG1_OFF	0x1000ULL
#घोषणा SIFIVE_PL2CACHE_CONFIG0_OFF	0x1008ULL

#घोषणा FLUSH64_CMD_POS			56
#घोषणा REGIONCLOCKDISABLE_MASK		BIT(3)

#घोषणा CONFIG0_ACCEPT_DIRTY_तथ्य_ENABLE	BIT(24)

काष्ठा sifive_pl2_quirks {
	bool no_dirty_fill;
};

काष्ठा sifive_pl2 {
	काष्ठा cache_device dev;
	व्योम *addr;
	bool no_dirty_fill;
};

#घोषणा to_pl2(_dev) container_of(_dev, काष्ठा sifive_pl2, dev)

अटल पूर्णांक sifive_pl2_flush_all(काष्ठा cache_device *dev)
{
	काष्ठा sifive_pl2 *pl2_dev = to_pl2(dev);
	अक्षर *addr = pl2_dev->addr;
	u64 cmd = (FLUSH64_CMD_TARGET_ALL | FLUSH64_CMD_TYPE_FLUSH) << FLUSH64_CMD_POS;
	u32 config0;

	/*
	 * While flushing pl2 cache, a speculative load might causes a dirty line pull
	 * into PL2. It will cause the SiFive SMC0 refuse to enter the power gating.
	 * Disable the ACCEPT_DIRTY_DATA_ENABLE to avoid the issue.
	 */
	अगर (pl2_dev->no_dirty_fill) {
		config0 = readl((व्योम *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
		config0 &= ~CONFIG0_ACCEPT_DIRTY_तथ्य_ENABLE;
		writel(config0, (व्योम *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
	}

#अगर __riscv_xlen != 32
	writeq(cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
#अन्यथा
	writel((u32)cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
	writel((u32)(cmd >> 32), addr + SIFIVE_PL2CACHE_FLUSH64_OFF + माप(u32));
#पूर्ण_अगर
	करो {} जबतक (readl(addr + SIFIVE_PL2CACHE_STATUS_OFF) & SIFIVE_PL2CACHE_CMD_QLEN);

	वापस 0;
}

अटल पूर्णांक sifive_pl2_warm_init(काष्ठा cache_device *dev)
{
	काष्ठा sifive_pl2 *pl2_dev = to_pl2(dev);
	अक्षर *addr = pl2_dev->addr;
	u32 val;

	/* Enabling the clock gating */
	val = readl(addr + SIFIVE_PL2CACHE_CONFIG1_OFF);
	val &= (~REGIONCLOCKDISABLE_MASK);
	writel(val, addr + SIFIVE_PL2CACHE_CONFIG1_OFF);

	वापस 0;
}

अटल काष्ठा cache_ops sifive_pl2_ops = {
	.warm_init = sifive_pl2_warm_init,
	.cache_flush_all = sifive_pl2_flush_all,
};

अटल पूर्णांक sifive_pl2_cold_init(स्थिर व्योम *fdt, पूर्णांक nodeoff, स्थिर काष्ठा fdt_match *match)
{
	स्थिर काष्ठा sifive_pl2_quirks *quirk = match->तथ्य;
	काष्ठा sifive_pl2 *pl2_dev;
	काष्ठा cache_device *dev;
	u64 reg_addr;
	पूर्णांक rc;

	/* find the pl2 control base address */
	rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoff, 0, &reg_addr, शून्य);
	अगर (rc < 0 && reg_addr)
		वापस SBI_ENODEV;

	pl2_dev = sbi_zalloc(माप(*pl2_dev));
	अगर (!pl2_dev)
		वापस SBI_ENOMEM;

	dev = &pl2_dev->dev;
	dev->ops = &sifive_pl2_ops;
	dev->cpu_private = true;

	rc = fdt_cache_add(fdt, nodeoff, dev);
	अगर (rc)
		वापस rc;

	pl2_dev->addr = (व्योम *)(uintptr_t)reg_addr;
	अगर (quirk)
		pl2_dev->no_dirty_fill = quirk->no_dirty_fill;

	वापस 0;
}

अटल स्थिर काष्ठा sifive_pl2_quirks pl2cache2_quirks = {
	.no_dirty_fill = true,
};

अटल स्थिर काष्ठा sifive_pl2_quirks pl2cache0_quirks = {
	.no_dirty_fill = false,
};

अटल स्थिर काष्ठा fdt_match sifive_pl2_match[] = {
	{ .compatible = "sifive,pl2cache2", .तथ्य = &pl2cache2_quirks },
	{ .compatible = "sifive,pl2cache1", .तथ्य = &pl2cache0_quirks },
	{ .compatible = "sifive,pl2cache0", .तथ्य = &pl2cache0_quirks },
	{},
};

काष्ठा fdt_driver fdt_sifive_pl2 = {
	.match_table = sifive_pl2_match,
	.init = sifive_pl2_cold_init,
};
