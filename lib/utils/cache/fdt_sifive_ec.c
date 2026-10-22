/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi/sbi_platform.h>
#અન્તર્ભાવ <sbi_utils/cache/fdt_cache.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_driver.h>

#define SIFIVE_EC_FEATURE_DISABLE_OFF	0x100UL
#define SIFIVE_EC_FLUSH_CMD_OFF		0x800UL
#define SIFIVE_EC_FLUSH_STATUS_OFF	0x808UL
#define SIFIVE_EC_FLUSH_ADDR_OFF	0x810UL
#define SIFIVE_EC_MODE_CTRL		0xa00UL

#define SIFIVE_EC_FLUSH_COMPLETION_MASK	BIT(0)

#define SIFIVE_EC_CLEANINV_ALL_CMD	0x3

#define SIFIVE_EC_FEATURE_DISABLE_VAL	0

સંરચના sifive_ec_quirks {
	bool two_mode;
	અક્ષર *reg_name;
};

સંરચના sifive_ec_slice {
	શૂન્ય *addr;
	bool last_slice;
};

સંરચના sifive_ec {
	સંરચના cache_device dev;
	સંરચના sifive_ec_slice *slices;
};

#define to_ec(_dev) container_of(_dev, સંરચના sifive_ec, dev)

સ્થિર પૂર્ણાંક sifive_ec_flush_all(સંરચના cache_device *dev)
{
	સંરચના sifive_ec *ec_dev = to_ec(dev);
	સંરચના sifive_ec_slice *slices = ec_dev->slices;
	u32 cmd = SIFIVE_EC_CLEANINV_ALL_CMD, i = 0;
	શૂન્ય *addr;

	કરો {
		addr = slices[i].addr;

		writel((પૂર્ણાંક)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF);
		writel((પૂર્ણાંક)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF + કદ(u32));
		writel(cmd, addr + SIFIVE_EC_FLUSH_CMD_OFF);
	} જ્યારે (!slices[i++].last_slice);

	i = 0;
	કરો {
		addr = slices[i].addr;
		કરો {} જ્યારે (!(readl(addr + SIFIVE_EC_FLUSH_STATUS_OFF) &
				SIFIVE_EC_FLUSH_COMPLETION_MASK));
	} જ્યારે (!slices[i++].last_slice);

	પાછા 0;
}

પૂર્ણાંક sifive_ec_warm_init(સંરચના cache_device *dev)
{
	સંરચના sifive_ec *ec_dev = to_ec(dev);
	સંરચના sifive_ec_slice *slices = ec_dev->slices;
	સંરચના sbi_domain *dom = sbi_domain_thishart_ptr();
	પૂર્ણાંક i = 0;

	જો (dom->boot_hartid == current_hartid()) {
		કરો {
			writel(SIFIVE_EC_FEATURE_DISABLE_VAL,
			       slices[i].addr + SIFIVE_EC_FEATURE_DISABLE_OFF);
		} જ્યારે (!slices[i++].last_slice);
	}

	પાછા SBI_OK;
}

સ્થિર સંરચના cache_ops sifive_ec_ops = {
	.warm_init = sifive_ec_warm_init,
	.cache_flush_all = sifive_ec_flush_all,
};

સ્થિર પૂર્ણાંક sifive_ec_slices_cold_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
				      સંરચના sifive_ec_slice *slices,
				      અચલ સંરચના sifive_ec_quirks *quirks)
{
	પૂર્ણાંક rc, subnode, slice_idx = -1;
	u64 reg_addr, size, start_addr = -1, end_addr = 0;

	fdt_for_each_subnode(subnode, fdt, nodeoff) {
		rc = fdt_get_ગાંઠ_addr_size_by_name(fdt, subnode, quirks->reg_name, &reg_addr,
						    &size);
		જો (rc < 0)
			પાછા SBI_ENODEV;

		જો (reg_addr < start_addr)
			start_addr = reg_addr;

		જો (reg_addr + size > end_addr)
			end_addr = reg_addr + size;

		slices[++slice_idx].addr = (શૂન્ય *)(uintptr_t)reg_addr;
	}
	slices[slice_idx].last_slice = true;

	/* Only enable the pmp to protect the EC m-mode region when it support two mode */
	જો (quirks->two_mode) {
		rc = sbi_domain_મૂળ_add_memrange((unsigned long)start_addr,
						  (unsigned long)(end_addr - start_addr),
						  BIT(12),
						  (SBI_DOMAIN_MEMREGION_MMIO |
						   SBI_DOMAIN_MEMREGION_M_READABLE |
						   SBI_DOMAIN_MEMREGION_M_WRITABLE));
		જો (rc)
			પાછા rc;
	}

	પાછા SBI_OK;
}

સ્થિર પૂર્ણાંક sifive_ec_cold_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff, અચલ સંરચના fdt_match *match)
{
	અચલ સંરચના sifive_ec_quirks *quirks = match->data;
	સંરચના sifive_ec_slice *slices;
	સંરચના sifive_ec *ec_dev;
	સંરચના cache_device *dev;
	પૂર્ણાંક subnode, rc = SBI_ENOMEM;
	u32 slice_ગણતરી = 0;

	/* Count the number of slices */
	fdt_for_each_subnode(subnode, fdt, nodeoff)
		slice_ગણતરી++;

	/* Need at least one slice */
	જો (!slice_ગણતરી)
		પાછા SBI_EINVAL;

	ec_dev = sbi_zalloc(કદ(*ec_dev));
	જો (!ec_dev)
		પાછા SBI_ENOMEM;

	slices = sbi_zalloc(slice_ગણતરી * કદ(*slices));
	જો (!slices)
		જાઓ free_ec;

	rc = sifive_ec_slices_cold_init(fdt, nodeoff, slices, quirks);
	જો (rc)
		જાઓ free_slice;

	dev = &ec_dev->dev;
	dev->ops = &sifive_ec_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	જો (rc)
		જાઓ free_slice;

	ec_dev->slices = slices;

	પાછા SBI_OK;

free_slice:
	sbi_free(slices);
free_ec:
	sbi_free(ec_dev);
	પાછા rc;
}

સ્થિર અચલ સંરચના sifive_ec_quirks sifive_extensiblecache0_quirks = {
	.two_mode = false,
	.reg_name = "control",
};

સ્થિર અચલ સંરચના sifive_ec_quirks sifive_extensiblecache4_quirks = {
	.two_mode = true,
	.reg_name = "m_mode",
};

સ્થિર અચલ સંરચના fdt_match sifive_ec_match[] = {
	{ .compatible = "sifive,extensiblecache4", .data = &sifive_extensiblecache4_quirks },
	{ .compatible = "sifive,extensiblecache3", .data = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache2", .data = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache0", .data = &sifive_extensiblecache0_quirks },
	{},
};

સંરચના fdt_driver fdt_sifive_ec = {
	.match_table = sifive_ec_match,
	.init = sifive_ec_cold_init,
};
