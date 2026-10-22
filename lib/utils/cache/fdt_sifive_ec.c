/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ضمن <libfdt.h>
#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_platform.h>
#ضمن <sbi_utils/cache/fdt_cache.h>
#ضمن <sbi_utils/fdt/fdt_driver.h>

#define SIFIVE_EC_FEATURE_DISABLE_OFF	0x100UL
#define SIFIVE_EC_FLUSH_CMD_OFF		0x800UL
#define SIFIVE_EC_FLUSH_STATUS_OFF	0x808UL
#define SIFIVE_EC_FLUSH_ADDR_OFF	0x810UL
#define SIFIVE_EC_MODE_CTRL		0xa00UL

#define SIFIVE_EC_FLUSH_COMPLETION_MASK	BIT(0)

#define SIFIVE_EC_CLEANINV_ALL_CMD	0x3

#define SIFIVE_EC_FEATURE_DISABLE_VAL	0

بنية sifive_ec_quirks {
	bool two_mode;
	حرف *reg_name;
};

بنية sifive_ec_slice {
	فراغ *addr;
	bool last_slice;
};

بنية sifive_ec {
	بنية cache_device dev;
	بنية sifive_ec_slice *slices;
};

#define to_ec(_dev) container_of(_dev, بنية sifive_ec, dev)

ساكن صحيح sifive_ec_flush_all(بنية cache_device *dev)
{
	بنية sifive_ec *ec_dev = to_ec(dev);
	بنية sifive_ec_slice *slices = ec_dev->slices;
	u32 cmd = SIFIVE_EC_CLEANINV_ALL_CMD, i = 0;
	فراغ *addr;

	افعل {
		addr = slices[i].addr;

		writel((صحيح)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF);
		writel((صحيح)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF + حجم(u32));
		writel(cmd, addr + SIFIVE_EC_FLUSH_CMD_OFF);
	} طالما (!slices[i++].last_slice);

	i = 0;
	افعل {
		addr = slices[i].addr;
		افعل {} طالما (!(readl(addr + SIFIVE_EC_FLUSH_STATUS_OFF) &
				SIFIVE_EC_FLUSH_COMPLETION_MASK));
	} طالما (!slices[i++].last_slice);

	أرجع 0;
}

صحيح sifive_ec_warm_init(بنية cache_device *dev)
{
	بنية sifive_ec *ec_dev = to_ec(dev);
	بنية sifive_ec_slice *slices = ec_dev->slices;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();
	صحيح i = 0;

	إذا (dom->boot_hartid == current_hartid()) {
		افعل {
			writel(SIFIVE_EC_FEATURE_DISABLE_VAL,
			       slices[i].addr + SIFIVE_EC_FEATURE_DISABLE_OFF);
		} طالما (!slices[i++].last_slice);
	}

	أرجع SBI_OK;
}

ساكن بنية cache_ops sifive_ec_ops = {
	.warm_init = sifive_ec_warm_init,
	.cache_flush_all = sifive_ec_flush_all,
};

ساكن صحيح sifive_ec_slices_cold_init(ثابت فراغ *fdt, صحيح nodeoff,
				      بنية sifive_ec_slice *slices,
				      ثابت بنية sifive_ec_quirks *quirks)
{
	صحيح rc, subnode, slice_idx = -1;
	u64 reg_addr, size, start_addr = -1, end_addr = 0;

	fdt_for_each_subnode(subnode, fdt, nodeoff) {
		rc = fdt_get_كلي_addr_size_by_name(fdt, subnode, quirks->reg_name, &reg_addr,
						    &size);
		إذا (rc < 0)
			أرجع SBI_ENODEV;

		إذا (reg_addr < start_addr)
			start_addr = reg_addr;

		إذا (reg_addr + size > end_addr)
			end_addr = reg_addr + size;

		slices[++slice_idx].addr = (فراغ *)(uintptr_t)reg_addr;
	}
	slices[slice_idx].last_slice = true;

	/* Only enable the pmp to protect the EC m-mode region when it support two mode */
	إذا (quirks->two_mode) {
		rc = sbi_domain_سيو_add_memrange((unsigned long)start_addr,
						  (unsigned long)(end_addr - start_addr),
						  BIT(12),
						  (SBI_DOMAIN_MEMREGION_MMIO |
						   SBI_DOMAIN_MEMREGION_M_READABLE |
						   SBI_DOMAIN_MEMREGION_M_WRITABLE));
		إذا (rc)
			أرجع rc;
	}

	أرجع SBI_OK;
}

ساكن صحيح sifive_ec_cold_init(ثابت فراغ *fdt, صحيح nodeoff, ثابت بنية fdt_match *match)
{
	ثابت بنية sifive_ec_quirks *quirks = match->data;
	بنية sifive_ec_slice *slices;
	بنية sifive_ec *ec_dev;
	بنية cache_device *dev;
	صحيح subnode, rc = SBI_ENOMEM;
	u32 slice_كدي = 0;

	/* Count the number of slices */
	fdt_for_each_subnode(subnode, fdt, nodeoff)
		slice_كدي++;

	/* Need at least one slice */
	إذا (!slice_كدي)
		أرجع SBI_EINVAL;

	ec_dev = sbi_zalloc(حجم(*ec_dev));
	إذا (!ec_dev)
		أرجع SBI_ENOMEM;

	slices = sbi_zalloc(slice_كدي * حجم(*slices));
	إذا (!slices)
		اذهب free_ec;

	rc = sifive_ec_slices_cold_init(fdt, nodeoff, slices, quirks);
	إذا (rc)
		اذهب free_slice;

	dev = &ec_dev->dev;
	dev->ops = &sifive_ec_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	إذا (rc)
		اذهب free_slice;

	ec_dev->slices = slices;

	أرجع SBI_OK;

free_slice:
	sbi_free(slices);
free_ec:
	sbi_free(ec_dev);
	أرجع rc;
}

ساكن ثابت بنية sifive_ec_quirks sifive_extensiblecache0_quirks = {
	.two_mode = false,
	.reg_name = "control",
};

ساكن ثابت بنية sifive_ec_quirks sifive_extensiblecache4_quirks = {
	.two_mode = true,
	.reg_name = "m_mode",
};

ساكن ثابت بنية fdt_match sifive_ec_match[] = {
	{ .compatible = "sifive,extensiblecache4", .data = &sifive_extensiblecache4_quirks },
	{ .compatible = "sifive,extensiblecache3", .data = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache2", .data = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache0", .data = &sifive_extensiblecache0_quirks },
	{},
};

بنية fdt_driver fdt_sifive_ec = {
	.match_table = sifive_ec_match,
	.init = sifive_ec_cold_init,
};
