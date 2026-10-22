/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#অন্তর্ভাৱ <libfdt.h>
#অন্তর্ভাৱ <sbi/riscv_io.h>
#অন্তর্ভাৱ <sbi/sbi_heap.h>
#অন্তর্ভাৱ <sbi/sbi_platform.h>
#অন্তর্ভাৱ <sbi_utils/cache/fdt_cache.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_driver.h>

#define SIFIVE_EC_FEATURE_DISABLE_OFF	0x100UL
#define SIFIVE_EC_FLUSH_CMD_OFF		0x800UL
#define SIFIVE_EC_FLUSH_STATUS_OFF	0x808UL
#define SIFIVE_EC_FLUSH_ADDR_OFF	0x810UL
#define SIFIVE_EC_MODE_CTRL		0xa00UL

#define SIFIVE_EC_FLUSH_COMPLETION_MASK	BIT(0)

#define SIFIVE_EC_CLEANINV_ALL_CMD	0x3

#define SIFIVE_EC_FEATURE_DISABLE_VAL	0

সংরচনা sifive_ec_quirks {
	bool two_mode;
	ৱর্ণ *reg_name;
};

সংরচনা sifive_ec_slice {
	শূন্য *addr;
	bool last_slice;
};

সংরচনা sifive_ec {
	সংরচনা cache_device dev;
	সংরচনা sifive_ec_slice *slices;
};

#define to_ec(_dev) container_of(_dev, সংরচনা sifive_ec, dev)

স্থির পূর্ণাঙ্ক sifive_ec_flush_all(সংরচনা cache_device *dev)
{
	সংরচনা sifive_ec *ec_dev = to_ec(dev);
	সংরচনা sifive_ec_slice *slices = ec_dev->slices;
	u32 cmd = SIFIVE_EC_CLEANINV_ALL_CMD, i = 0;
	শূন্য *addr;

	কৰা {
		addr = slices[i].addr;

		writel((পূর্ণাঙ্ক)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF);
		writel((পূর্ণাঙ্ক)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF + পরিমাণ(u32));
		writel(cmd, addr + SIFIVE_EC_FLUSH_CMD_OFF);
	} যেতিয়ালৈ (!slices[i++].last_slice);

	i = 0;
	কৰা {
		addr = slices[i].addr;
		কৰা {} যেতিয়ালৈ (!(readl(addr + SIFIVE_EC_FLUSH_STATUS_OFF) &
				SIFIVE_EC_FLUSH_COMPLETION_MASK));
	} যেতিয়ালৈ (!slices[i++].last_slice);

	ঘূৰাই 0;
}

পূর্ণাঙ্ক sifive_ec_warm_init(সংরচনা cache_device *dev)
{
	সংরচনা sifive_ec *ec_dev = to_ec(dev);
	সংরচনা sifive_ec_slice *slices = ec_dev->slices;
	সংরচনা sbi_domain *dom = sbi_domain_thishart_ptr();
	পূর্ণাঙ্ক i = 0;

	যদি (dom->boot_hartid == current_hartid()) {
		কৰা {
			writel(SIFIVE_EC_FEATURE_DISABLE_VAL,
			       slices[i].addr + SIFIVE_EC_FEATURE_DISABLE_OFF);
		} যেতিয়ালৈ (!slices[i++].last_slice);
	}

	ঘূৰাই SBI_OK;
}

স্থির সংরচনা cache_ops sifive_ec_ops = {
	.warm_init = sifive_ec_warm_init,
	.cache_flush_all = sifive_ec_flush_all,
};

স্থির পূর্ণাঙ্ক sifive_ec_slices_cold_init(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
				      সংরচনা sifive_ec_slice *slices,
				      ধ্রুৱ সংরচনা sifive_ec_quirks *quirks)
{
	পূর্ণাঙ্ক rc, subnode, slice_idx = -1;
	u64 reg_addr, size, start_addr = -1, end_addr = 0;

	fdt_for_each_subnode(subnode, fdt, nodeoff) {
		rc = fdt_get_গ্ৰন্থি_addr_size_by_name(fdt, subnode, quirks->reg_name, &reg_addr,
						    &size);
		যদি (rc < 0)
			ঘূৰাই SBI_ENODEV;

		যদি (reg_addr < start_addr)
			start_addr = reg_addr;

		যদি (reg_addr + size > end_addr)
			end_addr = reg_addr + size;

		slices[++slice_idx].addr = (শূন্য *)(uintptr_t)reg_addr;
	}
	slices[slice_idx].last_slice = true;

	/* Only enable the pmp to protect the EC m-mode region when it support two mode */
	যদি (quirks->two_mode) {
		rc = sbi_domain_মূল_add_memrange((unsigned long)start_addr,
						  (unsigned long)(end_addr - start_addr),
						  BIT(12),
						  (SBI_DOMAIN_MEMREGION_MMIO |
						   SBI_DOMAIN_MEMREGION_M_READABLE |
						   SBI_DOMAIN_MEMREGION_M_WRITABLE));
		যদি (rc)
			ঘূৰাই rc;
	}

	ঘূৰাই SBI_OK;
}

স্থির পূর্ণাঙ্ক sifive_ec_cold_init(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff, ধ্রুৱ সংরচনা fdt_match *match)
{
	ধ্রুৱ সংরচনা sifive_ec_quirks *quirks = match->data;
	সংরচনা sifive_ec_slice *slices;
	সংরচনা sifive_ec *ec_dev;
	সংরচনা cache_device *dev;
	পূর্ণাঙ্ক subnode, rc = SBI_ENOMEM;
	u32 slice_গুনতি = 0;

	/* Count the number of slices */
	fdt_for_each_subnode(subnode, fdt, nodeoff)
		slice_গুনতি++;

	/* Need at least one slice */
	যদি (!slice_গুনতি)
		ঘূৰাই SBI_EINVAL;

	ec_dev = sbi_zalloc(পরিমাণ(*ec_dev));
	যদি (!ec_dev)
		ঘূৰাই SBI_ENOMEM;

	slices = sbi_zalloc(slice_গুনতি * পরিমাণ(*slices));
	যদি (!slices)
		গচ্ছ free_ec;

	rc = sifive_ec_slices_cold_init(fdt, nodeoff, slices, quirks);
	যদি (rc)
		গচ্ছ free_slice;

	dev = &ec_dev->dev;
	dev->ops = &sifive_ec_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	যদি (rc)
		গচ্ছ free_slice;

	ec_dev->slices = slices;

	ঘূৰাই SBI_OK;

free_slice:
	sbi_free(slices);
free_ec:
	sbi_free(ec_dev);
	ঘূৰাই rc;
}

স্থির ধ্রুৱ সংরচনা sifive_ec_quirks sifive_extensiblecache0_quirks = {
	.two_mode = false,
	.reg_name = "control",
};

স্থির ধ্রুৱ সংরচনা sifive_ec_quirks sifive_extensiblecache4_quirks = {
	.two_mode = true,
	.reg_name = "m_mode",
};

স্থির ধ্রুৱ সংরচনা fdt_match sifive_ec_match[] = {
	{ .compatible = "sifive,extensiblecache4", .data = &sifive_extensiblecache4_quirks },
	{ .compatible = "sifive,extensiblecache3", .data = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache2", .data = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache0", .data = &sifive_extensiblecache0_quirks },
	{},
};

সংরচনা fdt_driver fdt_sifive_ec = {
	.match_table = sifive_ec_match,
	.init = sifive_ec_cold_init,
};
