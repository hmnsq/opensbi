/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_platform.h>
#अन्तर्भाव <sbi_utils/cache/fdt_cache.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>

#define SIFIVE_EC_FEATURE_DISABLE_OFF	0x100UL
#define SIFIVE_EC_FLUSH_CMD_OFF		0x800UL
#define SIFIVE_EC_FLUSH_STATUS_OFF	0x808UL
#define SIFIVE_EC_FLUSH_ADDR_OFF	0x810UL
#define SIFIVE_EC_MODE_CTRL		0xa00UL

#define SIFIVE_EC_FLUSH_COMPLETION_MASK	BIT(0)

#define SIFIVE_EC_CLEANINV_ALL_CMD	0x3

#define SIFIVE_EC_FEATURE_DISABLE_VAL	0

संरचना sifive_ec_quirks {
	bool two_mode;
	अक्षर *reg_name;
};

संरचना sifive_ec_slice {
	शून्य *addr;
	bool last_slice;
};

संरचना sifive_ec {
	संरचना cache_device dev;
	संरचना sifive_ec_slice *slices;
};

#define to_ec(_dev) container_of(_dev, संरचना sifive_ec, dev)

स्थिर पूर्णांक sifive_ec_flush_all(संरचना cache_device *dev)
{
	संरचना sifive_ec *ec_dev = to_ec(dev);
	संरचना sifive_ec_slice *slices = ec_dev->slices;
	u32 cmd = SIFIVE_EC_CLEANINV_ALL_CMD, i = 0;
	शून्य *addr;

	करा {
		addr = slices[i].addr;

		writel((पूर्णांक)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF);
		writel((पूर्णांक)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF + आकार(u32));
		writel(cmd, addr + SIFIVE_EC_FLUSH_CMD_OFF);
	} पर्यंत (!slices[i++].last_slice);

	i = 0;
	करा {
		addr = slices[i].addr;
		करा {} पर्यंत (!(readl(addr + SIFIVE_EC_FLUSH_STATUS_OFF) &
				SIFIVE_EC_FLUSH_COMPLETION_MASK));
	} पर्यंत (!slices[i++].last_slice);

	परत 0;
}

पूर्णांक sifive_ec_warm_init(संरचना cache_device *dev)
{
	संरचना sifive_ec *ec_dev = to_ec(dev);
	संरचना sifive_ec_slice *slices = ec_dev->slices;
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();
	पूर्णांक i = 0;

	जर (dom->boot_hartid == current_hartid()) {
		करा {
			writel(SIFIVE_EC_FEATURE_DISABLE_VAL,
			       slices[i].addr + SIFIVE_EC_FEATURE_DISABLE_OFF);
		} पर्यंत (!slices[i++].last_slice);
	}

	परत SBI_OK;
}

स्थिर संरचना cache_ops sifive_ec_ops = {
	.warm_init = sifive_ec_warm_init,
	.cache_flush_all = sifive_ec_flush_all,
};

स्थिर पूर्णांक sifive_ec_slices_cold_init(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
				      संरचना sifive_ec_slice *slices,
				      स्थिरांक संरचना sifive_ec_quirks *quirks)
{
	पूर्णांक rc, subnode, slice_idx = -1;
	u64 reg_addr, size, start_addr = -1, end_addr = 0;

	fdt_for_each_subnode(subnode, fdt, nodeoff) {
		rc = fdt_get_गाठ_addr_size_by_name(fdt, subnode, quirks->reg_name, &reg_addr,
						    &size);
		जर (rc < 0)
			परत SBI_ENODEV;

		जर (reg_addr < start_addr)
			start_addr = reg_addr;

		जर (reg_addr + size > end_addr)
			end_addr = reg_addr + size;

		slices[++slice_idx].addr = (शून्य *)(uintptr_t)reg_addr;
	}
	slices[slice_idx].last_slice = true;

	/* Only enable the pmp to protect the EC m-mode region when it support two mode */
	जर (quirks->two_mode) {
		rc = sbi_domain_मूळ_add_memrange((अचिन्हित दीर्घ)start_addr,
						  (अचिन्हित दीर्घ)(end_addr - start_addr),
						  BIT(12),
						  (SBI_DOMAIN_MEMREGION_MMIO |
						   SBI_DOMAIN_MEMREGION_M_READABLE |
						   SBI_DOMAIN_MEMREGION_M_WRITABLE));
		जर (rc)
			परत rc;
	}

	परत SBI_OK;
}

स्थिर पूर्णांक sifive_ec_cold_init(स्थिरांक शून्य *fdt, पूर्णांक nodeoff, स्थिरांक संरचना fdt_match *match)
{
	स्थिरांक संरचना sifive_ec_quirks *quirks = match->data;
	संरचना sifive_ec_slice *slices;
	संरचना sifive_ec *ec_dev;
	संरचना cache_device *dev;
	पूर्णांक subnode, rc = SBI_ENOMEM;
	u32 slice_मोजणी = 0;

	/* Count the number of slices */
	fdt_for_each_subnode(subnode, fdt, nodeoff)
		slice_मोजणी++;

	/* Need at least one slice */
	जर (!slice_मोजणी)
		परत SBI_EINVAL;

	ec_dev = sbi_zalloc(आकार(*ec_dev));
	जर (!ec_dev)
		परत SBI_ENOMEM;

	slices = sbi_zalloc(slice_मोजणी * आकार(*slices));
	जर (!slices)
		जा free_ec;

	rc = sifive_ec_slices_cold_init(fdt, nodeoff, slices, quirks);
	जर (rc)
		जा free_slice;

	dev = &ec_dev->dev;
	dev->ops = &sifive_ec_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	जर (rc)
		जा free_slice;

	ec_dev->slices = slices;

	परत SBI_OK;

free_slice:
	sbi_free(slices);
free_ec:
	sbi_free(ec_dev);
	परत rc;
}

स्थिर स्थिरांक संरचना sifive_ec_quirks sifive_extensiblecache0_quirks = {
	.two_mode = false,
	.reg_name = "control",
};

स्थिर स्थिरांक संरचना sifive_ec_quirks sifive_extensiblecache4_quirks = {
	.two_mode = true,
	.reg_name = "m_mode",
};

स्थिर स्थिरांक संरचना fdt_match sifive_ec_match[] = {
	{ .compatible = "sifive,extensiblecache4", .data = &sifive_extensiblecache4_quirks },
	{ .compatible = "sifive,extensiblecache3", .data = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache2", .data = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache0", .data = &sifive_extensiblecache0_quirks },
	{},
};

संरचना fdt_driver fdt_sifive_ec = {
	.match_table = sifive_ec_match,
	.init = sifive_ec_cold_init,
};
