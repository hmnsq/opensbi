/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#include <libfdt.h>
#include <sbi/riscv_io.h>
#include <sbi/sbi_heap.h>
#include <sbi/sbi_platform.h>
#include <sbi_utils/cache/fdt_cache.h>
#include <sbi_utils/fdt/fdt_driver.h>

#define SIFIVE_EC_FEATURE_DISABLE_OFF	0x100UL
#define SIFIVE_EC_FLUSH_CMD_OFF		0x800UL
#define SIFIVE_EC_FLUSH_STATUS_OFF	0x808UL
#define SIFIVE_EC_FLUSH_ADDR_OFF	0x810UL
#define SIFIVE_EC_MODE_CTRL		0xa00UL

#define SIFIVE_EC_FLUSH_COMPLETION_MASK	BIT(0)

#define SIFIVE_EC_CLEANINV_ALL_CMD	0x3

#define SIFIVE_EC_FEATURE_DISABLE_VAL	0

sa_nracanaa sifive_ec_quirks {
	bool two_mode;
	var_na *reg_name;
};

sa_nracanaa sifive_ec_slice {
	void *addr;
	bool last_slice;
};

sa_nracanaa sifive_ec {
	sa_nracanaa cache_device dev;
	sa_nracanaa sifive_ec_slice *slices;
};

#define to_ec(_dev) container_of(_dev, sa_nracanaa sifive_ec, dev)

static puur_naa_nka sifive_ec_flush_all(sa_nracanaa cache_device *dev)
{
	sa_nracanaa sifive_ec *ec_dev = to_ec(dev);
	sa_nracanaa sifive_ec_slice *slices = ec_dev->slices;
	u32 cmd = SIFIVE_EC_CLEANINV_ALL_CMD, i = 0;
	void *addr;

	do {
		addr = slices[i].addr;

		writel((puur_naa_nka)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF);
		writel((puur_naa_nka)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF + parimaa_na(u32));
		writel(cmd, addr + SIFIVE_EC_FLUSH_CMD_OFF);
	} ၶၢဝ်း (!slices[i++].last_slice);

	i = 0;
	do {
		addr = slices[i].addr;
		do {} ၶၢဝ်း (!(readl(addr + SIFIVE_EC_FLUSH_STATUS_OFF) &
				SIFIVE_EC_FLUSH_COMPLETION_MASK));
	} ၶၢဝ်း (!slices[i++].last_slice);

	ပွၵ်ႈ 0;
}

puur_naa_nka sifive_ec_warm_init(sa_nracanaa cache_device *dev)
{
	sa_nracanaa sifive_ec *ec_dev = to_ec(dev);
	sa_nracanaa sifive_ec_slice *slices = ec_dev->slices;
	sa_nracanaa sbi_domain *dom = sbi_domain_thishart_ptr();
	puur_naa_nka i = 0;

	ပေႃး (dom->boot_hartid == current_hartid()) {
		do {
			writel(SIFIVE_EC_FEATURE_DISABLE_VAL,
			       slices[i].addr + SIFIVE_EC_FEATURE_DISABLE_OFF);
		} ၶၢဝ်း (!slices[i++].last_slice);
	}

	ပွၵ်ႈ SBI_OK;
}

static sa_nracanaa cache_ops sifive_ec_ops = {
	.warm_init = sifive_ec_warm_init,
	.cache_flush_all = sifive_ec_flush_all,
};

static puur_naa_nka sifive_ec_slices_cold_init(const void *fdt, puur_naa_nka nodeoff,
				      sa_nracanaa sifive_ec_slice *slices,
				      const sa_nracanaa sifive_ec_quirks *quirks)
{
	puur_naa_nka rc, subnode, slice_idx = -1;
	u64 reg_addr, size, start_addr = -1, end_addr = 0;

	fdt_for_each_subnode(subnode, fdt, nodeoff) {
		rc = fdt_get_node_addr_size_by_name(fdt, subnode, quirks->reg_name, &reg_addr,
						    &size);
		ပေႃး (rc < 0)
			ပွၵ်ႈ SBI_ENODEV;

		ပေႃး (reg_addr < start_addr)
			start_addr = reg_addr;

		ပေႃး (reg_addr + size > end_addr)
			end_addr = reg_addr + size;

		slices[++slice_idx].addr = (void *)(uintptr_t)reg_addr;
	}
	slices[slice_idx].last_slice = true;

	/* Only enable the pmp to protect the EC m-mode region when it support two mode */
	ပေႃး (quirks->two_mode) {
		rc = sbi_domain_root_add_memrange((unsigned long)start_addr,
						  (unsigned long)(end_addr - start_addr),
						  BIT(12),
						  (SBI_DOMAIN_MEMREGION_MMIO |
						   SBI_DOMAIN_MEMREGION_M_READABLE |
						   SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ပေႃး (rc)
			ပွၵ်ႈ rc;
	}

	ပွၵ်ႈ SBI_OK;
}

static puur_naa_nka sifive_ec_cold_init(const void *fdt, puur_naa_nka nodeoff, const sa_nracanaa fdt_match *match)
{
	const sa_nracanaa sifive_ec_quirks *quirks = match->data;
	sa_nracanaa sifive_ec_slice *slices;
	sa_nracanaa sifive_ec *ec_dev;
	sa_nracanaa cache_device *dev;
	puur_naa_nka subnode, rc = SBI_ENOMEM;
	u32 slice_count = 0;

	/* Count the number of slices */
	fdt_for_each_subnode(subnode, fdt, nodeoff)
		slice_count++;

	/* Need at least one slice */
	ပေႃး (!slice_count)
		ပွၵ်ႈ SBI_EINVAL;

	ec_dev = sbi_zalloc(parimaa_na(*ec_dev));
	ပေႃး (!ec_dev)
		ပွၵ်ႈ SBI_ENOMEM;

	slices = sbi_zalloc(slice_count * parimaa_na(*slices));
	ပေႃး (!slices)
		goto free_ec;

	rc = sifive_ec_slices_cold_init(fdt, nodeoff, slices, quirks);
	ပေႃး (rc)
		goto free_slice;

	dev = &ec_dev->dev;
	dev->ops = &sifive_ec_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	ပေႃး (rc)
		goto free_slice;

	ec_dev->slices = slices;

	ပွၵ်ႈ SBI_OK;

free_slice:
	sbi_free(slices);
free_ec:
	sbi_free(ec_dev);
	ပွၵ်ႈ rc;
}

static const sa_nracanaa sifive_ec_quirks sifive_extensiblecache0_quirks = {
	.two_mode = false,
	.reg_name = "control",
};

static const sa_nracanaa sifive_ec_quirks sifive_extensiblecache4_quirks = {
	.two_mode = true,
	.reg_name = "m_mode",
};

static const sa_nracanaa fdt_match sifive_ec_match[] = {
	{ .compatible = "sifive,extensiblecache4", .data = &sifive_extensiblecache4_quirks },
	{ .compatible = "sifive,extensiblecache3", .data = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache2", .data = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache0", .data = &sifive_extensiblecache0_quirks },
	{},
};

sa_nracanaa fdt_driver fdt_sifive_ec = {
	.match_table = sifive_ec_match,
	.init = sifive_ec_cold_init,
};
