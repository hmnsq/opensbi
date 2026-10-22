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

#define SIFIVE_EC_FEATURE_DISABLE_OFF	0x100UL
#define SIFIVE_EC_FLUSH_CMD_OFF		0x800UL
#define SIFIVE_EC_FLUSH_STATUS_OFF	0x808UL
#define SIFIVE_EC_FLUSH_ADDR_OFF	0x810UL
#define SIFIVE_EC_MODE_CTRL		0xa00UL

#define SIFIVE_EC_FLUSH_COMPLETION_MASK	BIT(0)

#define SIFIVE_EC_CLEANINV_ALL_CMD	0x3

#define SIFIVE_EC_FEATURE_DISABLE_VAL	0

ಸಂರಚನಾ sifive_ec_quirks {
	bool two_mode;
	ವರ್ಣ *reg_name;
};

ಸಂರಚನಾ sifive_ec_slice {
	ಶೂನ್ಯ *addr;
	bool last_slice;
};

ಸಂರಚನಾ sifive_ec {
	ಸಂರಚನಾ cache_device dev;
	ಸಂರಚನಾ sifive_ec_slice *slices;
};

#define to_ec(_dev) container_of(_dev, ಸಂರಚನಾ sifive_ec, dev)

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_ec_flush_all(ಸಂರಚನಾ cache_device *dev)
{
	ಸಂರಚನಾ sifive_ec *ec_dev = to_ec(dev);
	ಸಂರಚನಾ sifive_ec_slice *slices = ec_dev->slices;
	u32 cmd = SIFIVE_EC_CLEANINV_ALL_CMD, i = 0;
	ಶೂನ್ಯ *addr;

	ಕುರು {
		addr = slices[i].addr;

		writel((ಪೂರ್ಣಾಙ್ಕ)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF);
		writel((ಪೂರ್ಣಾಙ್ಕ)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF + ಪರಿಮಾಣ(u32));
		writel(cmd, addr + SIFIVE_EC_FLUSH_CMD_OFF);
	} ಯಾವತ್ (!slices[i++].last_slice);

	i = 0;
	ಕುರು {
		addr = slices[i].addr;
		ಕುರು {} ಯಾವತ್ (!(readl(addr + SIFIVE_EC_FLUSH_STATUS_OFF) &
				SIFIVE_EC_FLUSH_COMPLETION_MASK));
	} ಯಾವತ್ (!slices[i++].last_slice);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ sifive_ec_warm_init(ಸಂರಚನಾ cache_device *dev)
{
	ಸಂರಚನಾ sifive_ec *ec_dev = to_ec(dev);
	ಸಂರಚನಾ sifive_ec_slice *slices = ec_dev->slices;
	ಸಂರಚನಾ sbi_domain *dom = sbi_domain_thishart_ptr();
	ಪೂರ್ಣಾಙ್ಕ i = 0;

	ಯದಿ (dom->boot_hartid == current_hartid()) {
		ಕುರು {
			writel(SIFIVE_EC_FEATURE_DISABLE_VAL,
			       slices[i].addr + SIFIVE_EC_FEATURE_DISABLE_OFF);
		} ಯಾವತ್ (!slices[i++].last_slice);
	}

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಸಂರಚನಾ cache_ops sifive_ec_ops = {
	.warm_init = sifive_ec_warm_init,
	.cache_flush_all = sifive_ec_flush_all,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_ec_slices_cold_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
				      ಸಂರಚನಾ sifive_ec_slice *slices,
				      ಧ್ರುವ ಸಂರಚನಾ sifive_ec_quirks *quirks)
{
	ಪೂರ್ಣಾಙ್ಕ rc, subnode, slice_idx = -1;
	u64 reg_addr, size, start_addr = -1, end_addr = 0;

	fdt_for_each_subnode(subnode, fdt, nodeoff) {
		rc = fdt_get_ಗಂಟು_addr_size_by_name(fdt, subnode, quirks->reg_name, &reg_addr,
						    &size);
		ಯದಿ (rc < 0)
			ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

		ಯದಿ (reg_addr < start_addr)
			start_addr = reg_addr;

		ಯದಿ (reg_addr + size > end_addr)
			end_addr = reg_addr + size;

		slices[++slice_idx].addr = (ಶೂನ್ಯ *)(uintptr_t)reg_addr;
	}
	slices[slice_idx].last_slice = true;

	/* Only enable the pmp to protect the EC m-mode region when it support two mode */
	ಯದಿ (quirks->two_mode) {
		rc = sbi_domain_ಬೇರು_add_memrange((unsigned long)start_addr,
						  (unsigned long)(end_addr - start_addr),
						  BIT(12),
						  (SBI_DOMAIN_MEMREGION_MMIO |
						   SBI_DOMAIN_MEMREGION_M_READABLE |
						   SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_ec_cold_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff, ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಧ್ರುವ ಸಂರಚನಾ sifive_ec_quirks *quirks = match->data;
	ಸಂರಚನಾ sifive_ec_slice *slices;
	ಸಂರಚನಾ sifive_ec *ec_dev;
	ಸಂರಚನಾ cache_device *dev;
	ಪೂರ್ಣಾಙ್ಕ subnode, rc = SBI_ENOMEM;
	u32 slice_ಎಣಿಕೆ = 0;

	/* Count the number of slices */
	fdt_for_each_subnode(subnode, fdt, nodeoff)
		slice_ಎಣಿಕೆ++;

	/* Need at least one slice */
	ಯದಿ (!slice_ಎಣಿಕೆ)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ec_dev = sbi_zalloc(ಪರಿಮಾಣ(*ec_dev));
	ಯದಿ (!ec_dev)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

	slices = sbi_zalloc(slice_ಎಣಿಕೆ * ಪರಿಮಾಣ(*slices));
	ಯದಿ (!slices)
		ಗಚ್ಛ free_ec;

	rc = sifive_ec_slices_cold_init(fdt, nodeoff, slices, quirks);
	ಯದಿ (rc)
		ಗಚ್ಛ free_slice;

	dev = &ec_dev->dev;
	dev->ops = &sifive_ec_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	ಯದಿ (rc)
		ಗಚ್ಛ free_slice;

	ec_dev->slices = slices;

	ಪ್ರತ್ಯಾಗಮ SBI_OK;

free_slice:
	sbi_free(slices);
free_ec:
	sbi_free(ec_dev);
	ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ sifive_ec_quirks sifive_extensiblecache0_quirks = {
	.two_mode = false,
	.reg_name = "control",
};

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ sifive_ec_quirks sifive_extensiblecache4_quirks = {
	.two_mode = true,
	.reg_name = "m_mode",
};

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match sifive_ec_match[] = {
	{ .compatible = "sifive,extensiblecache4", .data = &sifive_extensiblecache4_quirks },
	{ .compatible = "sifive,extensiblecache3", .data = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache2", .data = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache0", .data = &sifive_extensiblecache0_quirks },
	{},
};

ಸಂರಚನಾ fdt_driver fdt_sifive_ec = {
	.match_table = sifive_ec_match,
	.init = sifive_ec_cold_init,
};
