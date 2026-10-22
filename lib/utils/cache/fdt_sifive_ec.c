/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#شامل <libfdt.h>
#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi_utils/cache/fdt_cache.h>
#شامل <sbi_utils/fdt/fdt_driver.h>

#define SIFIVE_EC_FEATURE_DISABLE_OFF	0x100UL
#define SIFIVE_EC_FLUSH_CMD_OFF		0x800UL
#define SIFIVE_EC_FLUSH_STATUS_OFF	0x808UL
#define SIFIVE_EC_FLUSH_ADDR_OFF	0x810UL
#define SIFIVE_EC_MODE_CTRL		0xa00UL

#define SIFIVE_EC_FLUSH_COMPLETION_MASK	BIT(0)

#define SIFIVE_EC_CLEANINV_ALL_CMD	0x3

#define SIFIVE_EC_FEATURE_DISABLE_VAL	0

ساختار sifive_ec_quirks {
	bool two_mode;
	نویسه *reg_name;
};

ساختار sifive_ec_slice {
	تهی *addr;
	bool last_slice;
};

ساختار sifive_ec {
	ساختار cache_device dev;
	ساختار sifive_ec_slice *slices;
};

#define to_ec(_dev) container_of(_dev, ساختار sifive_ec, dev)

ایستا صحیح sifive_ec_flush_all(ساختار cache_device *dev)
{
	ساختار sifive_ec *ec_dev = to_ec(dev);
	ساختار sifive_ec_slice *slices = ec_dev->slices;
	u32 cmd = SIFIVE_EC_CLEANINV_ALL_CMD, i = 0;
	تهی *addr;

	انجامبده {
		addr = slices[i].addr;

		writel((صحیح)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF);
		writel((صحیح)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF + اندازه(u32));
		writel(cmd, addr + SIFIVE_EC_FLUSH_CMD_OFF);
	} وقتی (!slices[i++].last_slice);

	i = 0;
	انجامبده {
		addr = slices[i].addr;
		انجامبده {} وقتی (!(readl(addr + SIFIVE_EC_FLUSH_STATUS_OFF) &
				SIFIVE_EC_FLUSH_COMPLETION_MASK));
	} وقتی (!slices[i++].last_slice);

	بازگشت 0;
}

صحیح sifive_ec_warm_init(ساختار cache_device *dev)
{
	ساختار sifive_ec *ec_dev = to_ec(dev);
	ساختار sifive_ec_slice *slices = ec_dev->slices;
	ساختار sbi_domain *dom = sbi_domain_thishart_ptr();
	صحیح i = 0;

	اگر (dom->boot_hartid == current_hartid()) {
		انجامبده {
			writel(SIFIVE_EC_FEATURE_DISABLE_VAL,
			       slices[i].addr + SIFIVE_EC_FEATURE_DISABLE_OFF);
		} وقتی (!slices[i++].last_slice);
	}

	بازگشت SBI_OK;
}

ایستا ساختار cache_ops sifive_ec_ops = {
	.warm_init = sifive_ec_warm_init,
	.cache_flush_all = sifive_ec_flush_all,
};

ایستا صحیح sifive_ec_slices_cold_init(ثابت تهی *fdt, صحیح nodeoff,
				      ساختار sifive_ec_slice *slices,
				      ثابت ساختار sifive_ec_quirks *quirks)
{
	صحیح rc, subnode, slice_idx = -1;
	u64 reg_addr, size, start_addr = -1, end_addr = 0;

	fdt_for_each_subnode(subnode, fdt, nodeoff) {
		rc = fdt_get_گره_addr_size_by_name(fdt, subnode, quirks->reg_name, &reg_addr,
						    &size);
		اگر (rc < 0)
			بازگشت SBI_ENODEV;

		اگر (reg_addr < start_addr)
			start_addr = reg_addr;

		اگر (reg_addr + size > end_addr)
			end_addr = reg_addr + size;

		slices[++slice_idx].addr = (تهی *)(uintptr_t)reg_addr;
	}
	slices[slice_idx].last_slice = true;

	/* Only enable the pmp to protect the EC m-mode region when it support two mode */
	اگر (quirks->two_mode) {
		rc = sbi_domain_ریشه_add_memrange((unsigned long)start_addr,
						  (unsigned long)(end_addr - start_addr),
						  BIT(12),
						  (SBI_DOMAIN_MEMREGION_MMIO |
						   SBI_DOMAIN_MEMREGION_M_READABLE |
						   SBI_DOMAIN_MEMREGION_M_WRITABLE));
		اگر (rc)
			بازگشت rc;
	}

	بازگشت SBI_OK;
}

ایستا صحیح sifive_ec_cold_init(ثابت تهی *fdt, صحیح nodeoff, ثابت ساختار fdt_match *match)
{
	ثابت ساختار sifive_ec_quirks *quirks = match->data;
	ساختار sifive_ec_slice *slices;
	ساختار sifive_ec *ec_dev;
	ساختار cache_device *dev;
	صحیح subnode, rc = SBI_ENOMEM;
	u32 slice_شمار = 0;

	/* Count the number of slices */
	fdt_for_each_subnode(subnode, fdt, nodeoff)
		slice_شمار++;

	/* Need at least one slice */
	اگر (!slice_شمار)
		بازگشت SBI_EINVAL;

	ec_dev = sbi_zalloc(اندازه(*ec_dev));
	اگر (!ec_dev)
		بازگشت SBI_ENOMEM;

	slices = sbi_zalloc(slice_شمار * اندازه(*slices));
	اگر (!slices)
		برو free_ec;

	rc = sifive_ec_slices_cold_init(fdt, nodeoff, slices, quirks);
	اگر (rc)
		برو free_slice;

	dev = &ec_dev->dev;
	dev->ops = &sifive_ec_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	اگر (rc)
		برو free_slice;

	ec_dev->slices = slices;

	بازگشت SBI_OK;

free_slice:
	sbi_free(slices);
free_ec:
	sbi_free(ec_dev);
	بازگشت rc;
}

ایستا ثابت ساختار sifive_ec_quirks sifive_extensiblecache0_quirks = {
	.two_mode = false,
	.reg_name = "control",
};

ایستا ثابت ساختار sifive_ec_quirks sifive_extensiblecache4_quirks = {
	.two_mode = true,
	.reg_name = "m_mode",
};

ایستا ثابت ساختار fdt_match sifive_ec_match[] = {
	{ .compatible = "sifive,extensiblecache4", .data = &sifive_extensiblecache4_quirks },
	{ .compatible = "sifive,extensiblecache3", .data = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache2", .data = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache0", .data = &sifive_extensiblecache0_quirks },
	{},
};

ساختار fdt_driver fdt_sifive_ec = {
	.match_table = sifive_ec_match,
	.init = sifive_ec_cold_init,
};
