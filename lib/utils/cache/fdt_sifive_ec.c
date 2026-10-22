/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi/sbi_platform.h>
#అన్తర్భావ <sbi_utils/cache/fdt_cache.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_driver.h>

#నిర్వచించు SIFIVE_EC_FEATURE_DISABLE_OFF	0x100UL
#నిర్వచించు SIFIVE_EC_FLUSH_CMD_OFF		0x800UL
#నిర్వచించు SIFIVE_EC_FLUSH_స్థితి_OFF	0x808UL
#నిర్వచించు SIFIVE_EC_FLUSH_ADDR_OFF	0x810UL
#నిర్వచించు SIFIVE_EC_బహుళకం_CTRL		0xa00UL

#నిర్వచించు SIFIVE_EC_FLUSH_COMPLETION_ముసుగు	బిట్టు(0)

#నిర్వచించు SIFIVE_EC_CLEANINV_ALL_CMD	0x3

#నిర్వచించు SIFIVE_EC_FEATURE_DISABLE_VAL	0

నిర్మాణం sifive_ec_quirks {
	bool two_బహుళకం;
	అక్షరం *reg_name;
};

నిర్మాణం sifive_ec_ముక్క {
	శూన్య *addr;
	bool చివరి_ముక్క;
};

నిర్మాణం sifive_ec {
	నిర్మాణం cache_device dev;
	నిర్మాణం sifive_ec_ముక్క *slices;
};

#నిర్వచించు గమ్యం_ec(_dev) container_యొక్క(_dev, నిర్మాణం sifive_ec, dev)

స్థిర పూర్ణాంకం sifive_ec_flush_all(నిర్మాణం cache_device *dev)
{
	నిర్మాణం sifive_ec *ec_dev = గమ్యం_ec(dev);
	నిర్మాణం sifive_ec_ముక్క *slices = ec_dev->slices;
	u32 cmd = SIFIVE_EC_CLEANINV_ALL_CMD, i = 0;
	శూన్య *addr;

	చేయి {
		addr = slices[i].addr;

		writel((పూర్ణాంకం)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF);
		writel((పూర్ణాంకం)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF + పరిమాణం(u32));
		writel(cmd, addr + SIFIVE_EC_FLUSH_CMD_OFF);
	} అయితే (!slices[i++].చివరి_ముక్క);

	i = 0;
	చేయి {
		addr = slices[i].addr;
		చేయి {} అయితే (!(readl(addr + SIFIVE_EC_FLUSH_స్థితి_OFF) &
				SIFIVE_EC_FLUSH_COMPLETION_ముసుగు));
	} అయితే (!slices[i++].చివరి_ముక్క);

	తిరిగి 0;
}

పూర్ణాంకం sifive_ec_warm_init(నిర్మాణం cache_device *dev)
{
	నిర్మాణం sifive_ec *ec_dev = గమ్యం_ec(dev);
	నిర్మాణం sifive_ec_ముక్క *slices = ec_dev->slices;
	నిర్మాణం sbi_domain *dom = sbi_domain_thishart_ptr();
	పూర్ణాంకం i = 0;

	ఒకవేళ (dom->boot_hartid == current_hartid()) {
		చేయి {
			writel(SIFIVE_EC_FEATURE_DISABLE_VAL,
			       slices[i].addr + SIFIVE_EC_FEATURE_DISABLE_OFF);
		} అయితే (!slices[i++].చివరి_ముక్క);
	}

	తిరిగి SBI_OK;
}

స్థిర నిర్మాణం cache_ops sifive_ec_ops = {
	.warm_init = sifive_ec_warm_init,
	.cache_flush_all = sifive_ec_flush_all,
};

స్థిర పూర్ణాంకం sifive_ec_slices_cold_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
				      నిర్మాణం sifive_ec_ముక్క *slices,
				      స్థిరరాశి నిర్మాణం sifive_ec_quirks *quirks)
{
	పూర్ణాంకం rc, subnode, ముక్క_idx = -1;
	u64 reg_addr, కొలత, ప్రారంభం_addr = -1, చివర_addr = 0;

	fdt_for_each_subnode(subnode, fdt, nodeoff) {
		rc = fdt_get_కణుపు_addr_కొలత_by_name(fdt, subnode, quirks->reg_name, &reg_addr,
						    &కొలత);
		ఒకవేళ (rc < 0)
			తిరిగి SBI_ENODEV;

		ఒకవేళ (reg_addr < ప్రారంభం_addr)
			ప్రారంభం_addr = reg_addr;

		ఒకవేళ (reg_addr + కొలత > చివర_addr)
			చివర_addr = reg_addr + కొలత;

		slices[++ముక్క_idx].addr = (శూన్య *)(uintptr_t)reg_addr;
	}
	slices[ముక్క_idx].చివరి_ముక్క = true;

	/* Only enable the pmp to protect the EC m-mode region when it support two mode */
	ఒకవేళ (quirks->two_బహుళకం) {
		rc = sbi_domain_మూలం_కూడు_memrange((గుర్తులేని దీర్ఘ)ప్రారంభం_addr,
						  (గుర్తులేని దీర్ఘ)(చివర_addr - ప్రారంభం_addr),
						  బిట్టు(12),
						  (SBI_DOMAIN_MEMREGION_MMIO |
						   SBI_DOMAIN_MEMREGION_M_READABLE |
						   SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ఒకవేళ (rc)
			తిరిగి rc;
	}

	తిరిగి SBI_OK;
}

స్థిర పూర్ణాంకం sifive_ec_cold_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff, స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	స్థిరరాశి నిర్మాణం sifive_ec_quirks *quirks = సరిపోలిక->దత్తాంశం;
	నిర్మాణం sifive_ec_ముక్క *slices;
	నిర్మాణం sifive_ec *ec_dev;
	నిర్మాణం cache_device *dev;
	పూర్ణాంకం subnode, rc = SBI_ENOMEM;
	u32 ముక్క_లెక్క = 0;

	/* Count the number of slices */
	fdt_for_each_subnode(subnode, fdt, nodeoff)
		ముక్క_లెక్క++;

	/* Need at least one slice */
	ఒకవేళ (!ముక్క_లెక్క)
		తిరిగి SBI_EINVAL;

	ec_dev = sbi_zalloc(పరిమాణం(*ec_dev));
	ఒకవేళ (!ec_dev)
		తిరిగి SBI_ENOMEM;

	slices = sbi_zalloc(ముక్క_లెక్క * పరిమాణం(*slices));
	ఒకవేళ (!slices)
		వెళ్ళు free_ec;

	rc = sifive_ec_slices_cold_init(fdt, nodeoff, slices, quirks);
	ఒకవేళ (rc)
		వెళ్ళు free_ముక్క;

	dev = &ec_dev->dev;
	dev->ops = &sifive_ec_ops;
	rc = fdt_cache_కూడు(fdt, nodeoff, dev);
	ఒకవేళ (rc)
		వెళ్ళు free_ముక్క;

	ec_dev->slices = slices;

	తిరిగి SBI_OK;

free_ముక్క:
	sbi_free(slices);
free_ec:
	sbi_free(ec_dev);
	తిరిగి rc;
}

స్థిర స్థిరరాశి నిర్మాణం sifive_ec_quirks sifive_extensiblecache0_quirks = {
	.two_బహుళకం = false,
	.reg_name = "control",
};

స్థిర స్థిరరాశి నిర్మాణం sifive_ec_quirks sifive_extensiblecache4_quirks = {
	.two_బహుళకం = true,
	.reg_name = "m_mode",
};

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక sifive_ec_సరిపోలిక[] = {
	{ .compatible = "sifive,extensiblecache4", .దత్తాంశం = &sifive_extensiblecache4_quirks },
	{ .compatible = "sifive,extensiblecache3", .దత్తాంశం = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache2", .దత్తాంశం = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache0", .దత్తాంశం = &sifive_extensiblecache0_quirks },
	{},
};

నిర్మాణం fdt_driver fdt_sifive_ec = {
	.సరిపోలిక_పట్టిక = sifive_ec_సరిపోలిక,
	.init = sifive_ec_cold_init,
};
