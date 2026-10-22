/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_platform.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/cache/fdt_cache.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_driver.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_EC_FEATURE_DISABLE_OFF	0x100UL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_EC_FLUSH_CMD_OFF		0x800UL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_EC_FLUSH_ਸਥਿਤੀ_OFF	0x808UL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_EC_FLUSH_ADDR_OFF	0x810UL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_EC_ਬਹੁਲਕ_CTRL		0xa00UL

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_EC_FLUSH_COMPLETION_ਨਕਾਬ	ਦੋਆਧਾਰੀ_ਅੰਕ(0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_EC_CLEANINV_ALL_CMD	0x3

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_EC_FEATURE_DISABLE_VAL	0

ਬਣਤਰ sifive_ec_quirks {
	bool two_ਬਹੁਲਕ;
	ਅੱਖਰ *reg_name;
};

ਬਣਤਰ sifive_ec_ਟੁਕੜਾ {
	ਖਾਲੀ *addr;
	bool ਆਖਰੀ_ਟੁਕੜਾ;
};

ਬਣਤਰ sifive_ec {
	ਬਣਤਰ cache_device dev;
	ਬਣਤਰ sifive_ec_ਟੁਕੜਾ *slices;
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਮੰਜ਼ਿਲ_ec(_dev) container_ਦਾ(_dev, ਬਣਤਰ sifive_ec, dev)

ਸਥਿਰ ਪੂਰਨ sifive_ec_flush_all(ਬਣਤਰ cache_device *dev)
{
	ਬਣਤਰ sifive_ec *ec_dev = ਮੰਜ਼ਿਲ_ec(dev);
	ਬਣਤਰ sifive_ec_ਟੁਕੜਾ *slices = ec_dev->slices;
	u32 cmd = SIFIVE_EC_CLEANINV_ALL_CMD, i = 0;
	ਖਾਲੀ *addr;

	ਕਰੋ {
		addr = slices[i].addr;

		writel((ਪੂਰਨ)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF);
		writel((ਪੂਰਨ)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF + ਆਕਾਰ(u32));
		writel(cmd, addr + SIFIVE_EC_FLUSH_CMD_OFF);
	} ਜਦੋਂ (!slices[i++].ਆਖਰੀ_ਟੁਕੜਾ);

	i = 0;
	ਕਰੋ {
		addr = slices[i].addr;
		ਕਰੋ {} ਜਦੋਂ (!(readl(addr + SIFIVE_EC_FLUSH_ਸਥਿਤੀ_OFF) &
				SIFIVE_EC_FLUSH_COMPLETION_ਨਕਾਬ));
	} ਜਦੋਂ (!slices[i++].ਆਖਰੀ_ਟੁਕੜਾ);

	ਵਾਪਸ 0;
}

ਪੂਰਨ sifive_ec_warm_init(ਬਣਤਰ cache_device *dev)
{
	ਬਣਤਰ sifive_ec *ec_dev = ਮੰਜ਼ਿਲ_ec(dev);
	ਬਣਤਰ sifive_ec_ਟੁਕੜਾ *slices = ec_dev->slices;
	ਬਣਤਰ sbi_domain *dom = sbi_domain_thishart_ptr();
	ਪੂਰਨ i = 0;

	ਜੇ (dom->boot_hartid == current_hartid()) {
		ਕਰੋ {
			writel(SIFIVE_EC_FEATURE_DISABLE_VAL,
			       slices[i].addr + SIFIVE_EC_FEATURE_DISABLE_OFF);
		} ਜਦੋਂ (!slices[i++].ਆਖਰੀ_ਟੁਕੜਾ);
	}

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਬਣਤਰ cache_ops sifive_ec_ops = {
	.warm_init = sifive_ec_warm_init,
	.cache_flush_all = sifive_ec_flush_all,
};

ਸਥਿਰ ਪੂਰਨ sifive_ec_slices_cold_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
				      ਬਣਤਰ sifive_ec_ਟੁਕੜਾ *slices,
				      ਅਚਲ ਬਣਤਰ sifive_ec_quirks *quirks)
{
	ਪੂਰਨ rc, subnode, ਟੁਕੜਾ_idx = -1;
	u64 reg_addr, ਮਾਪ, ਸ਼ੁਰੂ_addr = -1, ਸਿਰਾ_addr = 0;

	fdt_for_each_subnode(subnode, fdt, nodeoff) {
		rc = fdt_get_ਗੰਢ_addr_ਮਾਪ_by_name(fdt, subnode, quirks->reg_name, &reg_addr,
						    &ਮਾਪ);
		ਜੇ (rc < 0)
			ਵਾਪਸ SBI_ENODEV;

		ਜੇ (reg_addr < ਸ਼ੁਰੂ_addr)
			ਸ਼ੁਰੂ_addr = reg_addr;

		ਜੇ (reg_addr + ਮਾਪ > ਸਿਰਾ_addr)
			ਸਿਰਾ_addr = reg_addr + ਮਾਪ;

		slices[++ਟੁਕੜਾ_idx].addr = (ਖਾਲੀ *)(uintptr_t)reg_addr;
	}
	slices[ਟੁਕੜਾ_idx].ਆਖਰੀ_ਟੁਕੜਾ = true;

	/* Only enable the pmp to protect the EC m-mode region when it support two mode */
	ਜੇ (quirks->two_ਬਹੁਲਕ) {
		rc = sbi_domain_ਜੜ੍ਹ_ਜੋੜੋ_memrange((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)ਸ਼ੁਰੂ_addr,
						  (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)(ਸਿਰਾ_addr - ਸ਼ੁਰੂ_addr),
						  ਦੋਆਧਾਰੀ_ਅੰਕ(12),
						  (SBI_DOMAIN_MEMREGION_MMIO |
						   SBI_DOMAIN_MEMREGION_M_READABLE |
						   SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ਜੇ (rc)
			ਵਾਪਸ rc;
	}

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਪੂਰਨ sifive_ec_cold_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff, ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਅਚਲ ਬਣਤਰ sifive_ec_quirks *quirks = ਮੇਲ->ਅੰਕੜੇ;
	ਬਣਤਰ sifive_ec_ਟੁਕੜਾ *slices;
	ਬਣਤਰ sifive_ec *ec_dev;
	ਬਣਤਰ cache_device *dev;
	ਪੂਰਨ subnode, rc = SBI_ENOMEM;
	u32 ਟੁਕੜਾ_ਗਿਣਤੀ = 0;

	/* Count the number of slices */
	fdt_for_each_subnode(subnode, fdt, nodeoff)
		ਟੁਕੜਾ_ਗਿਣਤੀ++;

	/* Need at least one slice */
	ਜੇ (!ਟੁਕੜਾ_ਗਿਣਤੀ)
		ਵਾਪਸ SBI_EINVAL;

	ec_dev = sbi_zalloc(ਆਕਾਰ(*ec_dev));
	ਜੇ (!ec_dev)
		ਵਾਪਸ SBI_ENOMEM;

	slices = sbi_zalloc(ਟੁਕੜਾ_ਗਿਣਤੀ * ਆਕਾਰ(*slices));
	ਜੇ (!slices)
		ਜਾਓ free_ec;

	rc = sifive_ec_slices_cold_init(fdt, nodeoff, slices, quirks);
	ਜੇ (rc)
		ਜਾਓ free_ਟੁਕੜਾ;

	dev = &ec_dev->dev;
	dev->ops = &sifive_ec_ops;
	rc = fdt_cache_ਜੋੜੋ(fdt, nodeoff, dev);
	ਜੇ (rc)
		ਜਾਓ free_ਟੁਕੜਾ;

	ec_dev->slices = slices;

	ਵਾਪਸ SBI_OK;

free_ਟੁਕੜਾ:
	sbi_free(slices);
free_ec:
	sbi_free(ec_dev);
	ਵਾਪਸ rc;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ sifive_ec_quirks sifive_extensiblecache0_quirks = {
	.two_ਬਹੁਲਕ = false,
	.reg_name = "control",
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ sifive_ec_quirks sifive_extensiblecache4_quirks = {
	.two_ਬਹੁਲਕ = true,
	.reg_name = "m_mode",
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ sifive_ec_ਮੇਲ[] = {
	{ .compatible = "sifive,extensiblecache4", .ਅੰਕੜੇ = &sifive_extensiblecache4_quirks },
	{ .compatible = "sifive,extensiblecache3", .ਅੰਕੜੇ = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache2", .ਅੰਕੜੇ = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache0", .ਅੰਕੜੇ = &sifive_extensiblecache0_quirks },
	{},
};

ਬਣਤਰ fdt_driver fdt_sifive_ec = {
	.ਮੇਲ_ਸਾਰਣੀ = sifive_ec_ਮੇਲ,
	.init = sifive_ec_cold_init,
};
