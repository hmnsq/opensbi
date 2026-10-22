/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#समावेश <libfdt.h>
#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi/sbi_platform.h>
#समावेश <sbi_utils/cache/fdt_cache.h>
#समावेश <sbi_utils/fdt/fdt_driver.h>

#घोषणा SIFIVE_EC_FEATURE_DISABLE_OFF	0x100UL
#घोषणा SIFIVE_EC_FLUSH_CMD_OFF		0x800UL
#घोषणा SIFIVE_EC_FLUSH_STATUS_OFF	0x808UL
#घोषणा SIFIVE_EC_FLUSH_ADDR_OFF	0x810UL
#घोषणा SIFIVE_EC_MODE_CTRL		0xa00UL

#घोषणा SIFIVE_EC_FLUSH_COMPLETION_MASK	BIT(0)

#घोषणा SIFIVE_EC_CLEANINV_ALL_CMD	0x3

#घोषणा SIFIVE_EC_FEATURE_DISABLE_VAL	0

काष्ठा sifive_ec_quirks {
	bool two_mode;
	अक्षर *reg_name;
};

काष्ठा sifive_ec_slice {
	व्योम *addr;
	bool last_slice;
};

काष्ठा sifive_ec {
	काष्ठा cache_device dev;
	काष्ठा sifive_ec_slice *slices;
};

#घोषणा to_ec(_dev) container_of(_dev, काष्ठा sifive_ec, dev)

अटल पूर्णांक sifive_ec_flush_all(काष्ठा cache_device *dev)
{
	काष्ठा sifive_ec *ec_dev = to_ec(dev);
	काष्ठा sifive_ec_slice *slices = ec_dev->slices;
	u32 cmd = SIFIVE_EC_CLEANINV_ALL_CMD, i = 0;
	व्योम *addr;

	करो {
		addr = slices[i].addr;

		writel((पूर्णांक)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF);
		writel((पूर्णांक)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF + माप(u32));
		writel(cmd, addr + SIFIVE_EC_FLUSH_CMD_OFF);
	} जबतक (!slices[i++].last_slice);

	i = 0;
	करो {
		addr = slices[i].addr;
		करो {} जबतक (!(readl(addr + SIFIVE_EC_FLUSH_STATUS_OFF) &
				SIFIVE_EC_FLUSH_COMPLETION_MASK));
	} जबतक (!slices[i++].last_slice);

	वापस 0;
}

पूर्णांक sifive_ec_warm_init(काष्ठा cache_device *dev)
{
	काष्ठा sifive_ec *ec_dev = to_ec(dev);
	काष्ठा sifive_ec_slice *slices = ec_dev->slices;
	काष्ठा sbi_domain *dom = sbi_domain_thishart_ptr();
	पूर्णांक i = 0;

	अगर (dom->boot_hartid == current_hartid()) {
		करो {
			writel(SIFIVE_EC_FEATURE_DISABLE_VAL,
			       slices[i].addr + SIFIVE_EC_FEATURE_DISABLE_OFF);
		} जबतक (!slices[i++].last_slice);
	}

	वापस SBI_OK;
}

अटल काष्ठा cache_ops sifive_ec_ops = {
	.warm_init = sifive_ec_warm_init,
	.cache_flush_all = sifive_ec_flush_all,
};

अटल पूर्णांक sifive_ec_slices_cold_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
				      काष्ठा sifive_ec_slice *slices,
				      स्थिर काष्ठा sifive_ec_quirks *quirks)
{
	पूर्णांक rc, subnode, slice_idx = -1;
	u64 reg_addr, आकार, start_addr = -1, end_addr = 0;

	fdt_for_each_subnode(subnode, fdt, nodeoff) {
		rc = fdt_get_गाँठ_addr_आकार_by_name(fdt, subnode, quirks->reg_name, &reg_addr,
						    &आकार);
		अगर (rc < 0)
			वापस SBI_ENODEV;

		अगर (reg_addr < start_addr)
			start_addr = reg_addr;

		अगर (reg_addr + आकार > end_addr)
			end_addr = reg_addr + आकार;

		slices[++slice_idx].addr = (व्योम *)(uintptr_t)reg_addr;
	}
	slices[slice_idx].last_slice = true;

	/* Only enable the pmp to protect the EC m-mode region when it support two mode */
	अगर (quirks->two_mode) {
		rc = sbi_domain_मूल_add_memrange((अचिन्हित दीर्घ)start_addr,
						  (अचिन्हित दीर्घ)(end_addr - start_addr),
						  BIT(12),
						  (SBI_DOMAIN_MEMREGION_MMIO |
						   SBI_DOMAIN_MEMREGION_M_READABLE |
						   SBI_DOMAIN_MEMREGION_M_WRITABLE));
		अगर (rc)
			वापस rc;
	}

	वापस SBI_OK;
}

अटल पूर्णांक sifive_ec_cold_init(स्थिर व्योम *fdt, पूर्णांक nodeoff, स्थिर काष्ठा fdt_match *match)
{
	स्थिर काष्ठा sifive_ec_quirks *quirks = match->तथ्य;
	काष्ठा sifive_ec_slice *slices;
	काष्ठा sifive_ec *ec_dev;
	काष्ठा cache_device *dev;
	पूर्णांक subnode, rc = SBI_ENOMEM;
	u32 slice_गिनती = 0;

	/* Count the number of slices */
	fdt_for_each_subnode(subnode, fdt, nodeoff)
		slice_गिनती++;

	/* Need at least one slice */
	अगर (!slice_गिनती)
		वापस SBI_EINVAL;

	ec_dev = sbi_zalloc(माप(*ec_dev));
	अगर (!ec_dev)
		वापस SBI_ENOMEM;

	slices = sbi_zalloc(slice_गिनती * माप(*slices));
	अगर (!slices)
		जाओ free_ec;

	rc = sifive_ec_slices_cold_init(fdt, nodeoff, slices, quirks);
	अगर (rc)
		जाओ free_slice;

	dev = &ec_dev->dev;
	dev->ops = &sifive_ec_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	अगर (rc)
		जाओ free_slice;

	ec_dev->slices = slices;

	वापस SBI_OK;

free_slice:
	sbi_free(slices);
free_ec:
	sbi_free(ec_dev);
	वापस rc;
}

अटल स्थिर काष्ठा sifive_ec_quirks sifive_extensiblecache0_quirks = {
	.two_mode = false,
	.reg_name = "control",
};

अटल स्थिर काष्ठा sifive_ec_quirks sifive_extensiblecache4_quirks = {
	.two_mode = true,
	.reg_name = "m_mode",
};

अटल स्थिर काष्ठा fdt_match sifive_ec_match[] = {
	{ .compatible = "sifive,extensiblecache4", .तथ्य = &sifive_extensiblecache4_quirks },
	{ .compatible = "sifive,extensiblecache3", .तथ्य = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache2", .तथ्य = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache0", .तथ्य = &sifive_extensiblecache0_quirks },
	{},
};

काष्ठा fdt_driver fdt_sifive_ec = {
	.match_table = sifive_ec_match,
	.init = sifive_ec_cold_init,
};
