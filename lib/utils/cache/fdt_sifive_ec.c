/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi/sbi_platform.h>
#അന്തര്ഭാവ <sbi_utils/cache/fdt_cache.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_driver.h>

#define SIFIVE_EC_FEATURE_DISABLE_OFF	0x100UL
#define SIFIVE_EC_FLUSH_CMD_OFF		0x800UL
#define SIFIVE_EC_FLUSH_STATUS_OFF	0x808UL
#define SIFIVE_EC_FLUSH_ADDR_OFF	0x810UL
#define SIFIVE_EC_MODE_CTRL		0xa00UL

#define SIFIVE_EC_FLUSH_COMPLETION_MASK	BIT(0)

#define SIFIVE_EC_CLEANINV_ALL_CMD	0x3

#define SIFIVE_EC_FEATURE_DISABLE_VAL	0

ഘടന sifive_ec_quirks {
	bool two_mode;
	അക്ഷരം *reg_name;
};

ഘടന sifive_ec_slice {
	ശൂന്യം *addr;
	bool last_slice;
};

ഘടന sifive_ec {
	ഘടന cache_device dev;
	ഘടന sifive_ec_slice *slices;
};

#define to_ec(_dev) container_of(_dev, ഘടന sifive_ec, dev)

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_ec_flush_all(ഘടന cache_device *dev)
{
	ഘടന sifive_ec *ec_dev = to_ec(dev);
	ഘടന sifive_ec_slice *slices = ec_dev->slices;
	u32 cmd = SIFIVE_EC_CLEANINV_ALL_CMD, i = 0;
	ശൂന്യം *addr;

	ചെയ്യുക {
		addr = slices[i].addr;

		writel((പൂർണ്ണസംഖ്യ)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF);
		writel((പൂർണ്ണസംഖ്യ)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF + വലുപ്പം(u32));
		writel(cmd, addr + SIFIVE_EC_FLUSH_CMD_OFF);
	} ആകുമ്പോൾ (!slices[i++].last_slice);

	i = 0;
	ചെയ്യുക {
		addr = slices[i].addr;
		ചെയ്യുക {} ആകുമ്പോൾ (!(readl(addr + SIFIVE_EC_FLUSH_STATUS_OFF) &
				SIFIVE_EC_FLUSH_COMPLETION_MASK));
	} ആകുമ്പോൾ (!slices[i++].last_slice);

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ sifive_ec_warm_init(ഘടന cache_device *dev)
{
	ഘടന sifive_ec *ec_dev = to_ec(dev);
	ഘടന sifive_ec_slice *slices = ec_dev->slices;
	ഘടന sbi_domain *dom = sbi_domain_thishart_ptr();
	പൂർണ്ണസംഖ്യ i = 0;

	എങ്കിൽ (dom->boot_hartid == current_hartid()) {
		ചെയ്യുക {
			writel(SIFIVE_EC_FEATURE_DISABLE_VAL,
			       slices[i].addr + SIFIVE_EC_FEATURE_DISABLE_OFF);
		} ആകുമ്പോൾ (!slices[i++].last_slice);
	}

	തിരികെ SBI_OK;
}

സ്ഥിതം ഘടന cache_ops sifive_ec_ops = {
	.warm_init = sifive_ec_warm_init,
	.cache_flush_all = sifive_ec_flush_all,
};

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_ec_slices_cold_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
				      ഘടന sifive_ec_slice *slices,
				      സ്ഥിരം ഘടന sifive_ec_quirks *quirks)
{
	പൂർണ്ണസംഖ്യ rc, subnode, slice_idx = -1;
	u64 reg_addr, size, start_addr = -1, end_addr = 0;

	fdt_for_each_subnode(subnode, fdt, nodeoff) {
		rc = fdt_get_കണ്ണി_addr_size_by_name(fdt, subnode, quirks->reg_name, &reg_addr,
						    &size);
		എങ്കിൽ (rc < 0)
			തിരികെ SBI_ENODEV;

		എങ്കിൽ (reg_addr < start_addr)
			start_addr = reg_addr;

		എങ്കിൽ (reg_addr + size > end_addr)
			end_addr = reg_addr + size;

		slices[++slice_idx].addr = (ശൂന്യം *)(uintptr_t)reg_addr;
	}
	slices[slice_idx].last_slice = true;

	/* Only enable the pmp to protect the EC m-mode region when it support two mode */
	എങ്കിൽ (quirks->two_mode) {
		rc = sbi_domain_വേര്_add_memrange((unsigned long)start_addr,
						  (unsigned long)(end_addr - start_addr),
						  BIT(12),
						  (SBI_DOMAIN_MEMREGION_MMIO |
						   SBI_DOMAIN_MEMREGION_M_READABLE |
						   SBI_DOMAIN_MEMREGION_M_WRITABLE));
		എങ്കിൽ (rc)
			തിരികെ rc;
	}

	തിരികെ SBI_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_ec_cold_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff, സ്ഥിരം ഘടന fdt_match *match)
{
	സ്ഥിരം ഘടന sifive_ec_quirks *quirks = match->data;
	ഘടന sifive_ec_slice *slices;
	ഘടന sifive_ec *ec_dev;
	ഘടന cache_device *dev;
	പൂർണ്ണസംഖ്യ subnode, rc = SBI_ENOMEM;
	u32 slice_എണ്ണം = 0;

	/* Count the number of slices */
	fdt_for_each_subnode(subnode, fdt, nodeoff)
		slice_എണ്ണം++;

	/* Need at least one slice */
	എങ്കിൽ (!slice_എണ്ണം)
		തിരികെ SBI_EINVAL;

	ec_dev = sbi_zalloc(വലുപ്പം(*ec_dev));
	എങ്കിൽ (!ec_dev)
		തിരികെ SBI_ENOMEM;

	slices = sbi_zalloc(slice_എണ്ണം * വലുപ്പം(*slices));
	എങ്കിൽ (!slices)
		പോകുക free_ec;

	rc = sifive_ec_slices_cold_init(fdt, nodeoff, slices, quirks);
	എങ്കിൽ (rc)
		പോകുക free_slice;

	dev = &ec_dev->dev;
	dev->ops = &sifive_ec_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	എങ്കിൽ (rc)
		പോകുക free_slice;

	ec_dev->slices = slices;

	തിരികെ SBI_OK;

free_slice:
	sbi_free(slices);
free_ec:
	sbi_free(ec_dev);
	തിരികെ rc;
}

സ്ഥിതം സ്ഥിരം ഘടന sifive_ec_quirks sifive_extensiblecache0_quirks = {
	.two_mode = false,
	.reg_name = "control",
};

സ്ഥിതം സ്ഥിരം ഘടന sifive_ec_quirks sifive_extensiblecache4_quirks = {
	.two_mode = true,
	.reg_name = "m_mode",
};

സ്ഥിതം സ്ഥിരം ഘടന fdt_match sifive_ec_match[] = {
	{ .compatible = "sifive,extensiblecache4", .data = &sifive_extensiblecache4_quirks },
	{ .compatible = "sifive,extensiblecache3", .data = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache2", .data = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache0", .data = &sifive_extensiblecache0_quirks },
	{},
};

ഘടന fdt_driver fdt_sifive_ec = {
	.match_table = sifive_ec_match,
	.init = sifive_ec_cold_init,
};
