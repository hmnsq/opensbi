/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi_utils/cache/fdt_cache.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>

#परिभाषय FLUSH64_CMD_TARGET_ALL		(0x2 << 3)
#परिभाषय FLUSH64_CMD_TYPE_FLUSH		0x3ULL

#परिभाषय SIFIVE_PL2CACHE_CMD_QLEN	0xff

#परिभाषय SIFIVE_PL2CACHE_FLUSH64_OFF	0x200ULL
#परिभाषय SIFIVE_PL2_CACHE_स्थितिः_OFF	0x208ULL
#परिभाषय SIFIVE_PL2CACHE_CONFIG1_OFF	0x1000ULL
#परिभाषय SIFIVE_PL2CACHE_CONFIG0_OFF	0x1008ULL

#परिभाषय FLUSH64_CMD_POS			56
#परिभाषय REGIONCLOCKDISABLE_आवरणम्		BIT(3)

#परिभाषय CONFIG0_ACCEPT_DIRTY_दत्त_ENABLE	BIT(24)

संरचना sifive_pl2_quirks {
	bool no_dirty_fill;
};

संरचना sifive_pl2 {
	संरचना cache_device dev;
	शून्य *addr;
	bool no_dirty_fill;
};

#परिभाषय गन्तव्यम्_pl2(_dev) container_सम्बन्धि(_dev, संरचना sifive_pl2, dev)

स्थिर पूर्णाङ्क sifive_pl2_flush_all(संरचना cache_device *dev)
{
	संरचना sifive_pl2 *pl2_dev = गन्तव्यम्_pl2(dev);
	वर्ण *addr = pl2_dev->addr;
	u64 cmd = (FLUSH64_CMD_TARGET_ALL | FLUSH64_CMD_TYPE_FLUSH) << FLUSH64_CMD_POS;
	u32 config0;

	/*
	 * While flushing pl2 cache, a speculative load might causes a dirty line pull
	 * into PL2. It will cause the SiFive SMC0 refuse to enter the power gating.
	 * Disable the ACCEPT_DIRTY_DATA_ENABLE to avoid the issue.
	 */
	यदि (pl2_dev->no_dirty_fill) {
		config0 = readl((शून्य *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
		config0 &= ~CONFIG0_ACCEPT_DIRTY_दत्त_ENABLE;
		writel(config0, (शून्य *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
	}

#यदि __riscv_xlen != 32
	writeq(cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
#अन्यथा
	writel((u32)cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
	writel((u32)(cmd >> 32), addr + SIFIVE_PL2CACHE_FLUSH64_OFF + परिमाण(u32));
#यदि_अन्तः
	कुरु {} यावत् (readl(addr + SIFIVE_PL2_CACHE_स्थितिः_OFF) & SIFIVE_PL2CACHE_CMD_QLEN);

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क sifive_pl2_warm_init(संरचना cache_device *dev)
{
	संरचना sifive_pl2 *pl2_dev = गन्तव्यम्_pl2(dev);
	वर्ण *addr = pl2_dev->addr;
	u32 val;

	/* Enabling the clock gating */
	val = readl(addr + SIFIVE_PL2CACHE_CONFIG1_OFF);
	val &= (~REGIONCLOCKDISABLE_आवरणम्);
	writel(val, addr + SIFIVE_PL2CACHE_CONFIG1_OFF);

	प्रत्यागम 0;
}

स्थिर संरचना cache_ops sifive_pl2_ops = {
	.warm_init = sifive_pl2_warm_init,
	.cache_flush_all = sifive_pl2_flush_all,
};

स्थिर पूर्णाङ्क sifive_pl2_cold_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff, ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	ध्रुव संरचना sifive_pl2_quirks *quirk = मेलनम्->दत्त;
	संरचना sifive_pl2 *pl2_dev;
	संरचना cache_device *dev;
	u64 reg_addr;
	पूर्णाङ्क rc;

	/* find the pl2 control base address */
	rc = fdt_get_ग्रन्थिः_addr_आकार(fdt, nodeoff, 0, &reg_addr, रिक्त);
	यदि (rc < 0 && reg_addr)
		प्रत्यागम SBI_ENODEV;

	pl2_dev = sbi_zalloc(परिमाण(*pl2_dev));
	यदि (!pl2_dev)
		प्रत्यागम SBI_ENOMEM;

	dev = &pl2_dev->dev;
	dev->ops = &sifive_pl2_ops;
	dev->cpu_वैयक्तिकम् = true;

	rc = fdt_cache_सङ्कलय(fdt, nodeoff, dev);
	यदि (rc)
		प्रत्यागम rc;

	pl2_dev->addr = (शून्य *)(uintptr_t)reg_addr;
	यदि (quirk)
		pl2_dev->no_dirty_fill = quirk->no_dirty_fill;

	प्रत्यागम 0;
}

स्थिर ध्रुव संरचना sifive_pl2_quirks pl2cache2_quirks = {
	.no_dirty_fill = true,
};

स्थिर ध्रुव संरचना sifive_pl2_quirks pl2cache0_quirks = {
	.no_dirty_fill = false,
};

स्थिर ध्रुव संरचना fdt_मेलनम् sifive_pl2_मेलनम्[] = {
	{ .compatible = "sifive,pl2cache2", .दत्त = &pl2cache2_quirks },
	{ .compatible = "sifive,pl2cache1", .दत्त = &pl2cache0_quirks },
	{ .compatible = "sifive,pl2cache0", .दत्त = &pl2cache0_quirks },
	{},
};

संरचना fdt_driver fdt_sifive_pl2 = {
	.मेलनम्_सारणी = sifive_pl2_मेलनम्,
	.init = sifive_pl2_cold_init,
};
