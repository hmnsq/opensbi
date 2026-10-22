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

#ವ್ಯಾಖ್ಯಾನಿಸು FLUSH64_CMD_ಗುರಿ_ALL		(0x2 << 3)
#ವ್ಯಾಖ್ಯಾನಿಸು FLUSH64_CMD_TYPE_FLUSH		0x3ULL

#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_PL2CACHE_CMD_QLEN	0xff

#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_PL2CACHE_FLUSH64_OFF	0x200ULL
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_PL2_CACHE_ಸ್ಥಿತಿ_OFF	0x208ULL
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_PL2CACHE_CONFIG1_OFF	0x1000ULL
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_PL2CACHE_CONFIG0_OFF	0x1008ULL

#ವ್ಯಾಖ್ಯಾನಿಸು FLUSH64_CMD_POS			56
#ವ್ಯಾಖ್ಯಾನಿಸು REGIONCLOCKDISABLE_ಮುಖವಾಡ		ಬಿಟ್(3)

#ವ್ಯಾಖ್ಯಾನಿಸು CONFIG0_ACCEPT_DIRTY_ದತ್ತಾಂಶ_ENABLE	ಬಿಟ್(24)

ರಚನೆ sifive_pl2_quirks {
	bool no_dirty_fill;
};

ರಚನೆ sifive_pl2 {
	ರಚನೆ cache_device dev;
	ಶೂನ್ಯ *addr;
	bool no_dirty_fill;
};

#ವ್ಯಾಖ್ಯಾನಿಸು ಗಮ್ಯ_pl2(_dev) container_ದ(_dev, ರಚನೆ sifive_pl2, dev)

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_pl2_flush_all(ರಚನೆ cache_device *dev)
{
	ರಚನೆ sifive_pl2 *pl2_dev = ಗಮ್ಯ_pl2(dev);
	ಅಕ್ಷರ *addr = pl2_dev->addr;
	u64 cmd = (FLUSH64_CMD_ಗುರಿ_ALL | FLUSH64_CMD_TYPE_FLUSH) << FLUSH64_CMD_POS;
	u32 config0;

	/*
	 * While flushing pl2 cache, a speculative load might causes a dirty line pull
	 * into PL2. It will cause the SiFive SMC0 refuse to enter the power gating.
	 * Disable the ACCEPT_DIRTY_DATA_ENABLE to avoid the issue.
	 */
	ಆದರೆ (pl2_dev->no_dirty_fill) {
		config0 = readl((ಶೂನ್ಯ *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
		config0 &= ~CONFIG0_ACCEPT_DIRTY_ದತ್ತಾಂಶ_ENABLE;
		writel(config0, (ಶೂನ್ಯ *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
	}

#ಆದರೆ __riscv_xlen != 32
	writeq(cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
#ಇಲ್ಲದಿದ್ದರೆ
	writel((u32)cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
	writel((u32)(cmd >> 32), addr + SIFIVE_PL2CACHE_FLUSH64_OFF + ಗಾತ್ರ(u32));
#ಆದರೆ_ಅಂತ್ಯ
	ಮಾಡು {} ಆಗ (readl(addr + SIFIVE_PL2_CACHE_ಸ್ಥಿತಿ_OFF) & SIFIVE_PL2CACHE_CMD_QLEN);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_pl2_warm_init(ರಚನೆ cache_device *dev)
{
	ರಚನೆ sifive_pl2 *pl2_dev = ಗಮ್ಯ_pl2(dev);
	ಅಕ್ಷರ *addr = pl2_dev->addr;
	u32 val;

	/* Enabling the clock gating */
	val = readl(addr + SIFIVE_PL2CACHE_CONFIG1_OFF);
	val &= (~REGIONCLOCKDISABLE_ಮುಖವಾಡ);
	writel(val, addr + SIFIVE_PL2CACHE_CONFIG1_OFF);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ರಚನೆ cache_ops sifive_pl2_ops = {
	.warm_init = sifive_pl2_warm_init,
	.cache_flush_all = sifive_pl2_flush_all,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_pl2_cold_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff, ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಸ್ಥಿರಾಂಕ ರಚನೆ sifive_pl2_quirks *quirk = ಹೊಂದಿಕೆ->ದತ್ತಾಂಶ;
	ರಚನೆ sifive_pl2 *pl2_dev;
	ರಚನೆ cache_device *dev;
	u64 reg_addr;
	ಪೂರ್ಣಾಂಕ rc;

	/* find the pl2 control base address */
	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, 0, &reg_addr, ಖಾಲಿ);
	ಆದರೆ (rc < 0 && reg_addr)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	pl2_dev = sbi_zalloc(ಗಾತ್ರ(*pl2_dev));
	ಆದರೆ (!pl2_dev)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	dev = &pl2_dev->dev;
	dev->ops = &sifive_pl2_ops;
	dev->cpu_ಖಾಸಗಿ = true;

	rc = fdt_cache_ಕೂಡಿಸು(fdt, nodeoff, dev);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	pl2_dev->addr = (ಶೂನ್ಯ *)(uintptr_t)reg_addr;
	ಆದರೆ (quirk)
		pl2_dev->no_dirty_fill = quirk->no_dirty_fill;

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ sifive_pl2_quirks pl2cache2_quirks = {
	.no_dirty_fill = true,
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ sifive_pl2_quirks pl2cache0_quirks = {
	.no_dirty_fill = false,
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ sifive_pl2_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "sifive,pl2cache2", .ದತ್ತಾಂಶ = &pl2cache2_quirks },
	{ .compatible = "sifive,pl2cache1", .ದತ್ತಾಂಶ = &pl2cache0_quirks },
	{ .compatible = "sifive,pl2cache0", .ದತ್ತಾಂಶ = &pl2cache0_quirks },
	{},
};

ರಚನೆ fdt_driver fdt_sifive_pl2 = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = sifive_pl2_ಹೊಂದಿಕೆ,
	.init = sifive_pl2_cold_init,
};
