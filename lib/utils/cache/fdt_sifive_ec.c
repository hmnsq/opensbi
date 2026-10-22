/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_platform.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/cache/fdt_cache.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>

#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_EC_FEATURE_DISABLE_OFF	0x100UL
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_EC_FLUSH_CMD_OFF		0x800UL
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_EC_FLUSH_ಸ್ಥಿತಿ_OFF	0x808UL
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_EC_FLUSH_ADDR_OFF	0x810UL
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_EC_ಬಹುಲಕ_CTRL		0xa00UL

#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_EC_FLUSH_COMPLETION_ಮುಖವಾಡ	ಬಿಟ್(0)

#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_EC_CLEANINV_ALL_CMD	0x3

#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_EC_FEATURE_DISABLE_VAL	0

ರಚನೆ sifive_ec_quirks {
	bool two_ಬಹುಲಕ;
	ಅಕ್ಷರ *reg_name;
};

ರಚನೆ sifive_ec_ತುಂಡು {
	ಶೂನ್ಯ *addr;
	bool ಕೊನೆಯ_ತುಂಡು;
};

ರಚನೆ sifive_ec {
	ರಚನೆ cache_device dev;
	ರಚನೆ sifive_ec_ತುಂಡು *slices;
};

#ವ್ಯಾಖ್ಯಾನಿಸು ಗಮ್ಯ_ec(_dev) container_ದ(_dev, ರಚನೆ sifive_ec, dev)

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_ec_flush_all(ರಚನೆ cache_device *dev)
{
	ರಚನೆ sifive_ec *ec_dev = ಗಮ್ಯ_ec(dev);
	ರಚನೆ sifive_ec_ತುಂಡು *slices = ec_dev->slices;
	u32 cmd = SIFIVE_EC_CLEANINV_ALL_CMD, i = 0;
	ಶೂನ್ಯ *addr;

	ಮಾಡು {
		addr = slices[i].addr;

		writel((ಪೂರ್ಣಾಂಕ)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF);
		writel((ಪೂರ್ಣಾಂಕ)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF + ಗಾತ್ರ(u32));
		writel(cmd, addr + SIFIVE_EC_FLUSH_CMD_OFF);
	} ಆಗ (!slices[i++].ಕೊನೆಯ_ತುಂಡು);

	i = 0;
	ಮಾಡು {
		addr = slices[i].addr;
		ಮಾಡು {} ಆಗ (!(readl(addr + SIFIVE_EC_FLUSH_ಸ್ಥಿತಿ_OFF) &
				SIFIVE_EC_FLUSH_COMPLETION_ಮುಖವಾಡ));
	} ಆಗ (!slices[i++].ಕೊನೆಯ_ತುಂಡು);

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ sifive_ec_warm_init(ರಚನೆ cache_device *dev)
{
	ರಚನೆ sifive_ec *ec_dev = ಗಮ್ಯ_ec(dev);
	ರಚನೆ sifive_ec_ತುಂಡು *slices = ec_dev->slices;
	ರಚನೆ sbi_domain *dom = sbi_domain_thishart_ptr();
	ಪೂರ್ಣಾಂಕ i = 0;

	ಆದರೆ (dom->boot_hartid == current_hartid()) {
		ಮಾಡು {
			writel(SIFIVE_EC_FEATURE_DISABLE_VAL,
			       slices[i].addr + SIFIVE_EC_FEATURE_DISABLE_OFF);
		} ಆಗ (!slices[i++].ಕೊನೆಯ_ತುಂಡು);
	}

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ರಚನೆ cache_ops sifive_ec_ops = {
	.warm_init = sifive_ec_warm_init,
	.cache_flush_all = sifive_ec_flush_all,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_ec_slices_cold_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
				      ರಚನೆ sifive_ec_ತುಂಡು *slices,
				      ಸ್ಥಿರಾಂಕ ರಚನೆ sifive_ec_quirks *quirks)
{
	ಪೂರ್ಣಾಂಕ rc, subnode, ತುಂಡು_idx = -1;
	u64 reg_addr, ಗಾತ್ರ_2, ಆರಂಭ_addr = -1, ಕೊನೆ_addr = 0;

	fdt_for_each_subnode(subnode, fdt, nodeoff) {
		rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ_by_name(fdt, subnode, quirks->reg_name, &reg_addr,
						    &ಗಾತ್ರ_2);
		ಆದರೆ (rc < 0)
			ಹಿಂತಿರುಗಿ SBI_ENODEV;

		ಆದರೆ (reg_addr < ಆರಂಭ_addr)
			ಆರಂಭ_addr = reg_addr;

		ಆದರೆ (reg_addr + ಗಾತ್ರ_2 > ಕೊನೆ_addr)
			ಕೊನೆ_addr = reg_addr + ಗಾತ್ರ_2;

		slices[++ತುಂಡು_idx].addr = (ಶೂನ್ಯ *)(uintptr_t)reg_addr;
	}
	slices[ತುಂಡು_idx].ಕೊನೆಯ_ತುಂಡು = true;

	/* Only enable the pmp to protect the EC m-mode region when it support two mode */
	ಆದರೆ (quirks->two_ಬಹುಲಕ) {
		rc = sbi_domain_ಬೇರು_ಕೂಡಿಸು_memrange((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)ಆರಂಭ_addr,
						  (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)(ಕೊನೆ_addr - ಆರಂಭ_addr),
						  ಬಿಟ್(12),
						  (SBI_DOMAIN_MEMREGION_MMIO |
						   SBI_DOMAIN_MEMREGION_M_READABLE |
						   SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;
	}

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_ec_cold_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff, ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಸ್ಥಿರಾಂಕ ರಚನೆ sifive_ec_quirks *quirks = ಹೊಂದಿಕೆ->ದತ್ತಾಂಶ;
	ರಚನೆ sifive_ec_ತುಂಡು *slices;
	ರಚನೆ sifive_ec *ec_dev;
	ರಚನೆ cache_device *dev;
	ಪೂರ್ಣಾಂಕ subnode, rc = SBI_ENOMEM;
	u32 ತುಂಡು_ಎಣಿಕೆ = 0;

	/* Count the number of slices */
	fdt_for_each_subnode(subnode, fdt, nodeoff)
		ತುಂಡು_ಎಣಿಕೆ++;

	/* Need at least one slice */
	ಆದರೆ (!ತುಂಡು_ಎಣಿಕೆ)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ec_dev = sbi_zalloc(ಗಾತ್ರ(*ec_dev));
	ಆದರೆ (!ec_dev)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	slices = sbi_zalloc(ತುಂಡು_ಎಣಿಕೆ * ಗಾತ್ರ(*slices));
	ಆದರೆ (!slices)
		ಹೋಗು free_ec;

	rc = sifive_ec_slices_cold_init(fdt, nodeoff, slices, quirks);
	ಆದರೆ (rc)
		ಹೋಗು free_ತುಂಡು;

	dev = &ec_dev->dev;
	dev->ops = &sifive_ec_ops;
	rc = fdt_cache_ಕೂಡಿಸು(fdt, nodeoff, dev);
	ಆದರೆ (rc)
		ಹೋಗು free_ತುಂಡು;

	ec_dev->slices = slices;

	ಹಿಂತಿರುಗಿ SBI_OK;

free_ತುಂಡು:
	sbi_free(slices);
free_ec:
	sbi_free(ec_dev);
	ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ sifive_ec_quirks sifive_extensiblecache0_quirks = {
	.two_ಬಹುಲಕ = false,
	.reg_name = "control",
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ sifive_ec_quirks sifive_extensiblecache4_quirks = {
	.two_ಬಹುಲಕ = true,
	.reg_name = "m_mode",
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ sifive_ec_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "sifive,extensiblecache4", .ದತ್ತಾಂಶ = &sifive_extensiblecache4_quirks },
	{ .compatible = "sifive,extensiblecache3", .ದತ್ತಾಂಶ = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache2", .ದತ್ತಾಂಶ = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache0", .ದತ್ತಾಂಶ = &sifive_extensiblecache0_quirks },
	{},
};

ರಚನೆ fdt_driver fdt_sifive_ec = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = sifive_ec_ಹೊಂದಿಕೆ,
	.init = sifive_ec_cold_init,
};
