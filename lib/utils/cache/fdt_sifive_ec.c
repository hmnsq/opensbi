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

#परिभाषय SIFIVE_EC_FEATURE_DISABLE_OFF	0x100UL
#परिभाषय SIFIVE_EC_FLUSH_CMD_OFF		0x800UL
#परिभाषय SIFIVE_EC_FLUSH_स्थितिः_OFF	0x808UL
#परिभाषय SIFIVE_EC_FLUSH_ADDR_OFF	0x810UL
#परिभाषय SIFIVE_EC_बहुलकम्_CTRL		0xa00UL

#परिभाषय SIFIVE_EC_FLUSH_COMPLETION_आवरणम्	BIT(0)

#परिभाषय SIFIVE_EC_CLEANINV_ALL_CMD	0x3

#परिभाषय SIFIVE_EC_FEATURE_DISABLE_VAL	0

संरचना sifive_ec_quirks {
	bool two_बहुलकम्;
	वर्ण *reg_name;
};

संरचना sifive_ec_खण्डिका {
	शून्य *addr;
	bool चरमः_खण्डिका;
};

संरचना sifive_ec {
	संरचना cache_device dev;
	संरचना sifive_ec_खण्डिका *slices;
};

#परिभाषय गन्तव्यम्_ec(_dev) container_सम्बन्धि(_dev, संरचना sifive_ec, dev)

स्थिर पूर्णाङ्क sifive_ec_flush_all(संरचना cache_device *dev)
{
	संरचना sifive_ec *ec_dev = गन्तव्यम्_ec(dev);
	संरचना sifive_ec_खण्डिका *slices = ec_dev->slices;
	u32 cmd = SIFIVE_EC_CLEANINV_ALL_CMD, i = 0;
	शून्य *addr;

	कुरु {
		addr = slices[i].addr;

		writel((पूर्णाङ्क)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF);
		writel((पूर्णाङ्क)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF + परिमाण(u32));
		writel(cmd, addr + SIFIVE_EC_FLUSH_CMD_OFF);
	} यावत् (!slices[i++].चरमः_खण्डिका);

	i = 0;
	कुरु {
		addr = slices[i].addr;
		कुरु {} यावत् (!(readl(addr + SIFIVE_EC_FLUSH_स्थितिः_OFF) &
				SIFIVE_EC_FLUSH_COMPLETION_आवरणम्));
	} यावत् (!slices[i++].चरमः_खण्डिका);

	प्रत्यागम 0;
}

पूर्णाङ्क sifive_ec_warm_init(संरचना cache_device *dev)
{
	संरचना sifive_ec *ec_dev = गन्तव्यम्_ec(dev);
	संरचना sifive_ec_खण्डिका *slices = ec_dev->slices;
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();
	पूर्णाङ्क i = 0;

	यदि (dom->boot_hartid == current_hartid()) {
		कुरु {
			writel(SIFIVE_EC_FEATURE_DISABLE_VAL,
			       slices[i].addr + SIFIVE_EC_FEATURE_DISABLE_OFF);
		} यावत् (!slices[i++].चरमः_खण्डिका);
	}

	प्रत्यागम SBI_OK;
}

स्थिर संरचना cache_ops sifive_ec_ops = {
	.warm_init = sifive_ec_warm_init,
	.cache_flush_all = sifive_ec_flush_all,
};

स्थिर पूर्णाङ्क sifive_ec_slices_cold_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
				      संरचना sifive_ec_खण्डिका *slices,
				      ध्रुव संरचना sifive_ec_quirks *quirks)
{
	पूर्णाङ्क rc, subnode, खण्डिका_idx = -1;
	u64 reg_addr, आकार, start_addr = -1, अन्तः_addr = 0;

	fdt_for_each_subnode(subnode, fdt, nodeoff) {
		rc = fdt_get_ग्रन्थिः_addr_आकार_by_name(fdt, subnode, quirks->reg_name, &reg_addr,
						    &आकार);
		यदि (rc < 0)
			प्रत्यागम SBI_ENODEV;

		यदि (reg_addr < start_addr)
			start_addr = reg_addr;

		यदि (reg_addr + आकार > अन्तः_addr)
			अन्तः_addr = reg_addr + आकार;

		slices[++खण्डिका_idx].addr = (शून्य *)(uintptr_t)reg_addr;
	}
	slices[खण्डिका_idx].चरमः_खण्डिका = true;

	/* Only enable the pmp to protect the EC m-mode region when it support two mode */
	यदि (quirks->two_बहुलकम्) {
		rc = sbi_domain_मूलम्_सङ्कलय_memrange((अचिह्नित दीर्घ)start_addr,
						  (अचिह्नित दीर्घ)(अन्तः_addr - start_addr),
						  BIT(12),
						  (SBI_DOMAIN_MEMREGION_MMIO |
						   SBI_DOMAIN_MEMREGION_M_READABLE |
						   SBI_DOMAIN_MEMREGION_M_WRITABLE));
		यदि (rc)
			प्रत्यागम rc;
	}

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क sifive_ec_cold_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff, ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	ध्रुव संरचना sifive_ec_quirks *quirks = मेलनम्->दत्त;
	संरचना sifive_ec_खण्डिका *slices;
	संरचना sifive_ec *ec_dev;
	संरचना cache_device *dev;
	पूर्णाङ्क subnode, rc = SBI_ENOMEM;
	u32 खण्डिका_गणनम् = 0;

	/* Count the number of slices */
	fdt_for_each_subnode(subnode, fdt, nodeoff)
		खण्डिका_गणनम्++;

	/* Need at least one slice */
	यदि (!खण्डिका_गणनम्)
		प्रत्यागम SBI_EINVAL;

	ec_dev = sbi_zalloc(परिमाण(*ec_dev));
	यदि (!ec_dev)
		प्रत्यागम SBI_ENOMEM;

	slices = sbi_zalloc(खण्डिका_गणनम् * परिमाण(*slices));
	यदि (!slices)
		गच्छ free_ec;

	rc = sifive_ec_slices_cold_init(fdt, nodeoff, slices, quirks);
	यदि (rc)
		गच्छ free_खण्डिका;

	dev = &ec_dev->dev;
	dev->ops = &sifive_ec_ops;
	rc = fdt_cache_सङ्कलय(fdt, nodeoff, dev);
	यदि (rc)
		गच्छ free_खण्डिका;

	ec_dev->slices = slices;

	प्रत्यागम SBI_OK;

free_खण्डिका:
	sbi_free(slices);
free_ec:
	sbi_free(ec_dev);
	प्रत्यागम rc;
}

स्थिर ध्रुव संरचना sifive_ec_quirks sifive_extensiblecache0_quirks = {
	.two_बहुलकम् = false,
	.reg_name = "control",
};

स्थिर ध्रुव संरचना sifive_ec_quirks sifive_extensiblecache4_quirks = {
	.two_बहुलकम् = true,
	.reg_name = "m_mode",
};

स्थिर ध्रुव संरचना fdt_मेलनम् sifive_ec_मेलनम्[] = {
	{ .compatible = "sifive,extensiblecache4", .दत्त = &sifive_extensiblecache4_quirks },
	{ .compatible = "sifive,extensiblecache3", .दत्त = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache2", .दत्त = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache0", .दत्त = &sifive_extensiblecache0_quirks },
	{},
};

संरचना fdt_driver fdt_sifive_ec = {
	.मेलनम्_सारणी = sifive_ec_मेलनम्,
	.init = sifive_ec_cold_init,
};
