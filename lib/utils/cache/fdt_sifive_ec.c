/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_platform.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/cache/fdt_cache.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_driver.h>

#define SIFIVE_EC_FEATURE_DISABLE_OFF	0x100UL
#define SIFIVE_EC_FLUSH_CMD_OFF		0x800UL
#define SIFIVE_EC_FLUSH_STATUS_OFF	0x808UL
#define SIFIVE_EC_FLUSH_ADDR_OFF	0x810UL
#define SIFIVE_EC_MODE_CTRL		0xa00UL

#define SIFIVE_EC_FLUSH_COMPLETION_MASK	BIT(0)

#define SIFIVE_EC_CLEANINV_ALL_CMD	0x3

#define SIFIVE_EC_FEATURE_DISABLE_VAL	0

ସଂରଚନା sifive_ec_quirks {
	bool two_mode;
	ଅକ୍ଷର *reg_name;
};

ସଂରଚନା sifive_ec_slice {
	ଶୂନ୍ୟ *addr;
	bool last_slice;
};

ସଂରଚନା sifive_ec {
	ସଂରଚନା cache_device dev;
	ସଂରଚନା sifive_ec_slice *slices;
};

#define to_ec(_dev) container_of(_dev, ସଂରଚନା sifive_ec, dev)

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_ec_flush_all(ସଂରଚନା cache_device *dev)
{
	ସଂରଚନା sifive_ec *ec_dev = to_ec(dev);
	ସଂରଚନା sifive_ec_slice *slices = ec_dev->slices;
	u32 cmd = SIFIVE_EC_CLEANINV_ALL_CMD, i = 0;
	ଶୂନ୍ୟ *addr;

	କର {
		addr = slices[i].addr;

		writel((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF);
		writel((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF + ଆକାର(u32));
		writel(cmd, addr + SIFIVE_EC_FLUSH_CMD_OFF);
	} ଯେତେବେଳେ (!slices[i++].last_slice);

	i = 0;
	କର {
		addr = slices[i].addr;
		କର {} ଯେତେବେଳେ (!(readl(addr + SIFIVE_EC_FLUSH_STATUS_OFF) &
				SIFIVE_EC_FLUSH_COMPLETION_MASK));
	} ଯେତେବେଳେ (!slices[i++].last_slice);

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_ec_warm_init(ସଂରଚନା cache_device *dev)
{
	ସଂରଚନା sifive_ec *ec_dev = to_ec(dev);
	ସଂରଚନା sifive_ec_slice *slices = ec_dev->slices;
	ସଂରଚନା sbi_domain *dom = sbi_domain_thishart_ptr();
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;

	ଯଦି (dom->boot_hartid == current_hartid()) {
		କର {
			writel(SIFIVE_EC_FEATURE_DISABLE_VAL,
			       slices[i].addr + SIFIVE_EC_FEATURE_DISABLE_OFF);
		} ଯେତେବେଳେ (!slices[i++].last_slice);
	}

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ସଂରଚନା cache_ops sifive_ec_ops = {
	.warm_init = sifive_ec_warm_init,
	.cache_flush_all = sifive_ec_flush_all,
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_ec_slices_cold_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
				      ସଂରଚନା sifive_ec_slice *slices,
				      ଅଚଳ ସଂରଚନା sifive_ec_quirks *quirks)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc, subnode, slice_idx = -1;
	u64 reg_addr, size, start_addr = -1, end_addr = 0;

	fdt_for_each_subnode(subnode, fdt, nodeoff) {
		rc = fdt_get_ଗଣ୍ଠି_addr_size_by_name(fdt, subnode, quirks->reg_name, &reg_addr,
						    &size);
		ଯଦି (rc < 0)
			ଫେରନ୍ତୁ SBI_ENODEV;

		ଯଦି (reg_addr < start_addr)
			start_addr = reg_addr;

		ଯଦି (reg_addr + size > end_addr)
			end_addr = reg_addr + size;

		slices[++slice_idx].addr = (ଶୂନ୍ୟ *)(uintptr_t)reg_addr;
	}
	slices[slice_idx].last_slice = true;

	/* Only enable the pmp to protect the EC m-mode region when it support two mode */
	ଯଦି (quirks->two_mode) {
		rc = sbi_domain_ମୂଳ_add_memrange((unsigned long)start_addr,
						  (unsigned long)(end_addr - start_addr),
						  BIT(12),
						  (SBI_DOMAIN_MEMREGION_MMIO |
						   SBI_DOMAIN_MEMREGION_M_READABLE |
						   SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;
	}

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_ec_cold_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff, ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ଅଚଳ ସଂରଚନା sifive_ec_quirks *quirks = match->data;
	ସଂରଚନା sifive_ec_slice *slices;
	ସଂରଚନା sifive_ec *ec_dev;
	ସଂରଚନା cache_device *dev;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା subnode, rc = SBI_ENOMEM;
	u32 slice_ଗଣନା = 0;

	/* Count the number of slices */
	fdt_for_each_subnode(subnode, fdt, nodeoff)
		slice_ଗଣନା++;

	/* Need at least one slice */
	ଯଦି (!slice_ଗଣନା)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ec_dev = sbi_zalloc(ଆକାର(*ec_dev));
	ଯଦି (!ec_dev)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	slices = sbi_zalloc(slice_ଗଣନା * ଆକାର(*slices));
	ଯଦି (!slices)
		ଯାଅ free_ec;

	rc = sifive_ec_slices_cold_init(fdt, nodeoff, slices, quirks);
	ଯଦି (rc)
		ଯାଅ free_slice;

	dev = &ec_dev->dev;
	dev->ops = &sifive_ec_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	ଯଦି (rc)
		ଯାଅ free_slice;

	ec_dev->slices = slices;

	ଫେରନ୍ତୁ SBI_OK;

free_slice:
	sbi_free(slices);
free_ec:
	sbi_free(ec_dev);
	ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା sifive_ec_quirks sifive_extensiblecache0_quirks = {
	.two_mode = false,
	.reg_name = "control",
};

ସ୍ଥିର ଅଚଳ ସଂରଚନା sifive_ec_quirks sifive_extensiblecache4_quirks = {
	.two_mode = true,
	.reg_name = "m_mode",
};

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match sifive_ec_match[] = {
	{ .compatible = "sifive,extensiblecache4", .data = &sifive_extensiblecache4_quirks },
	{ .compatible = "sifive,extensiblecache3", .data = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache2", .data = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache0", .data = &sifive_extensiblecache0_quirks },
	{},
};

ସଂରଚନା fdt_driver fdt_sifive_ec = {
	.match_table = sifive_ec_match,
	.init = sifive_ec_cold_init,
};
