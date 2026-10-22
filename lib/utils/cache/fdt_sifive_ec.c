/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi/sbi_platform.h>
#அந்தர்பாவ <sbi_utils/cache/fdt_cache.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_driver.h>

#define SIFIVE_EC_FEATURE_DISABLE_OFF	0x100UL
#define SIFIVE_EC_FLUSH_CMD_OFF		0x800UL
#define SIFIVE_EC_FLUSH_STATUS_OFF	0x808UL
#define SIFIVE_EC_FLUSH_ADDR_OFF	0x810UL
#define SIFIVE_EC_MODE_CTRL		0xa00UL

#define SIFIVE_EC_FLUSH_COMPLETION_MASK	BIT(0)

#define SIFIVE_EC_CLEANINV_ALL_CMD	0x3

#define SIFIVE_EC_FEATURE_DISABLE_VAL	0

கட்டமைப்பு sifive_ec_quirks {
	bool two_mode;
	எழுத்து *reg_name;
};

கட்டமைப்பு sifive_ec_slice {
	வெறுமை *addr;
	bool last_slice;
};

கட்டமைப்பு sifive_ec {
	கட்டமைப்பு cache_device dev;
	கட்டமைப்பு sifive_ec_slice *slices;
};

#define to_ec(_dev) container_of(_dev, கட்டமைப்பு sifive_ec, dev)

நிலையான முழு sifive_ec_flush_all(கட்டமைப்பு cache_device *dev)
{
	கட்டமைப்பு sifive_ec *ec_dev = to_ec(dev);
	கட்டமைப்பு sifive_ec_slice *slices = ec_dev->slices;
	u32 cmd = SIFIVE_EC_CLEANINV_ALL_CMD, i = 0;
	வெறுமை *addr;

	செய் {
		addr = slices[i].addr;

		writel((முழு)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF);
		writel((முழு)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF + அளவு(u32));
		writel(cmd, addr + SIFIVE_EC_FLUSH_CMD_OFF);
	} போது (!slices[i++].last_slice);

	i = 0;
	செய் {
		addr = slices[i].addr;
		செய் {} போது (!(readl(addr + SIFIVE_EC_FLUSH_STATUS_OFF) &
				SIFIVE_EC_FLUSH_COMPLETION_MASK));
	} போது (!slices[i++].last_slice);

	திரும்பு 0;
}

முழு sifive_ec_warm_init(கட்டமைப்பு cache_device *dev)
{
	கட்டமைப்பு sifive_ec *ec_dev = to_ec(dev);
	கட்டமைப்பு sifive_ec_slice *slices = ec_dev->slices;
	கட்டமைப்பு sbi_domain *dom = sbi_domain_thishart_ptr();
	முழு i = 0;

	எனில் (dom->boot_hartid == current_hartid()) {
		செய் {
			writel(SIFIVE_EC_FEATURE_DISABLE_VAL,
			       slices[i].addr + SIFIVE_EC_FEATURE_DISABLE_OFF);
		} போது (!slices[i++].last_slice);
	}

	திரும்பு SBI_OK;
}

நிலையான கட்டமைப்பு cache_ops sifive_ec_ops = {
	.warm_init = sifive_ec_warm_init,
	.cache_flush_all = sifive_ec_flush_all,
};

நிலையான முழு sifive_ec_slices_cold_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
				      கட்டமைப்பு sifive_ec_slice *slices,
				      மாறிலி கட்டமைப்பு sifive_ec_quirks *quirks)
{
	முழு rc, subnode, slice_idx = -1;
	u64 reg_addr, size, start_addr = -1, end_addr = 0;

	fdt_for_each_subnode(subnode, fdt, nodeoff) {
		rc = fdt_get_முனை_addr_size_by_name(fdt, subnode, quirks->reg_name, &reg_addr,
						    &size);
		எனில் (rc < 0)
			திரும்பு SBI_ENODEV;

		எனில் (reg_addr < start_addr)
			start_addr = reg_addr;

		எனில் (reg_addr + size > end_addr)
			end_addr = reg_addr + size;

		slices[++slice_idx].addr = (வெறுமை *)(uintptr_t)reg_addr;
	}
	slices[slice_idx].last_slice = true;

	/* Only enable the pmp to protect the EC m-mode region when it support two mode */
	எனில் (quirks->two_mode) {
		rc = sbi_domain_வேர்_add_memrange((unsigned long)start_addr,
						  (unsigned long)(end_addr - start_addr),
						  BIT(12),
						  (SBI_DOMAIN_MEMREGION_MMIO |
						   SBI_DOMAIN_MEMREGION_M_READABLE |
						   SBI_DOMAIN_MEMREGION_M_WRITABLE));
		எனில் (rc)
			திரும்பு rc;
	}

	திரும்பு SBI_OK;
}

நிலையான முழு sifive_ec_cold_init(மாறிலி வெறுமை *fdt, முழு nodeoff, மாறிலி கட்டமைப்பு fdt_match *match)
{
	மாறிலி கட்டமைப்பு sifive_ec_quirks *quirks = match->data;
	கட்டமைப்பு sifive_ec_slice *slices;
	கட்டமைப்பு sifive_ec *ec_dev;
	கட்டமைப்பு cache_device *dev;
	முழு subnode, rc = SBI_ENOMEM;
	u32 slice_மொத்தம் = 0;

	/* Count the number of slices */
	fdt_for_each_subnode(subnode, fdt, nodeoff)
		slice_மொத்தம்++;

	/* Need at least one slice */
	எனில் (!slice_மொத்தம்)
		திரும்பு SBI_EINVAL;

	ec_dev = sbi_zalloc(அளவு(*ec_dev));
	எனில் (!ec_dev)
		திரும்பு SBI_ENOMEM;

	slices = sbi_zalloc(slice_மொத்தம் * அளவு(*slices));
	எனில் (!slices)
		செல் free_ec;

	rc = sifive_ec_slices_cold_init(fdt, nodeoff, slices, quirks);
	எனில் (rc)
		செல் free_slice;

	dev = &ec_dev->dev;
	dev->ops = &sifive_ec_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	எனில் (rc)
		செல் free_slice;

	ec_dev->slices = slices;

	திரும்பு SBI_OK;

free_slice:
	sbi_free(slices);
free_ec:
	sbi_free(ec_dev);
	திரும்பு rc;
}

நிலையான மாறிலி கட்டமைப்பு sifive_ec_quirks sifive_extensiblecache0_quirks = {
	.two_mode = false,
	.reg_name = "control",
};

நிலையான மாறிலி கட்டமைப்பு sifive_ec_quirks sifive_extensiblecache4_quirks = {
	.two_mode = true,
	.reg_name = "m_mode",
};

நிலையான மாறிலி கட்டமைப்பு fdt_match sifive_ec_match[] = {
	{ .compatible = "sifive,extensiblecache4", .data = &sifive_extensiblecache4_quirks },
	{ .compatible = "sifive,extensiblecache3", .data = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache2", .data = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache0", .data = &sifive_extensiblecache0_quirks },
	{},
};

கட்டமைப்பு fdt_driver fdt_sifive_ec = {
	.match_table = sifive_ec_match,
	.init = sifive_ec_cold_init,
};
