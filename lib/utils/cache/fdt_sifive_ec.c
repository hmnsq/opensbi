/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#شامل <libfdt.h>
#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi_utils/cache/fdt_cache.h>
#شامل <sbi_utils/fdt/fdt_driver.h>

#تعریف_کرو SIFIVE_EC_FEATURE_DISABLE_OFF	0x100UL
#تعریف_کرو SIFIVE_EC_FLUSH_CMD_OFF		0x800UL
#تعریف_کرو SIFIVE_EC_FLUSH_حالت_OFF	0x808UL
#تعریف_کرو SIFIVE_EC_FLUSH_ADDR_OFF	0x810UL
#تعریف_کرو SIFIVE_EC_منوال_CTRL		0xa00UL

#تعریف_کرو SIFIVE_EC_FLUSH_COMPLETION_نقاب	ثنائی_ہندسہ(0)

#تعریف_کرو SIFIVE_EC_CLEANINV_ALL_CMD	0x3

#تعریف_کرو SIFIVE_EC_FEATURE_DISABLE_VAL	0

ساخت sifive_ec_quirks {
	bool two_منوال;
	حرف *reg_name;
};

ساخت sifive_ec_قاش {
	خالی *addr;
	bool آخری_قاش;
};

ساخت sifive_ec {
	ساخت cache_device dev;
	ساخت sifive_ec_قاش *slices;
};

#تعریف_کرو مقصد_ec(_dev) container_کا(_dev, ساخت sifive_ec, dev)

ساکن صحیح sifive_ec_flush_all(ساخت cache_device *dev)
{
	ساخت sifive_ec *ec_dev = مقصد_ec(dev);
	ساخت sifive_ec_قاش *slices = ec_dev->slices;
	u32 cmd = SIFIVE_EC_CLEANINV_ALL_CMD, i = 0;
	خالی *addr;

	کرو {
		addr = slices[i].addr;

		writel((صحیح)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF);
		writel((صحیح)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF + حجم(u32));
		writel(cmd, addr + SIFIVE_EC_FLUSH_CMD_OFF);
	} جب (!slices[i++].آخری_قاش);

	i = 0;
	کرو {
		addr = slices[i].addr;
		کرو {} جب (!(readl(addr + SIFIVE_EC_FLUSH_حالت_OFF) &
				SIFIVE_EC_FLUSH_COMPLETION_نقاب));
	} جب (!slices[i++].آخری_قاش);

	واپس 0;
}

صحیح sifive_ec_warm_init(ساخت cache_device *dev)
{
	ساخت sifive_ec *ec_dev = مقصد_ec(dev);
	ساخت sifive_ec_قاش *slices = ec_dev->slices;
	ساخت sbi_domain *dom = sbi_domain_thishart_ptr();
	صحیح i = 0;

	اگر (dom->boot_hartid == current_hartid()) {
		کرو {
			writel(SIFIVE_EC_FEATURE_DISABLE_VAL,
			       slices[i].addr + SIFIVE_EC_FEATURE_DISABLE_OFF);
		} جب (!slices[i++].آخری_قاش);
	}

	واپس SBI_OK;
}

ساکن ساخت cache_ops sifive_ec_ops = {
	.warm_init = sifive_ec_warm_init,
	.cache_flush_all = sifive_ec_flush_all,
};

ساکن صحیح sifive_ec_slices_cold_init(ثابت خالی *fdt, صحیح nodeoff,
				      ساخت sifive_ec_قاش *slices,
				      ثابت ساخت sifive_ec_quirks *quirks)
{
	صحیح rc, subnode, قاش_idx = -1;
	u64 reg_addr, سائز, آغاز_addr = -1, انتہا_addr = 0;

	fdt_for_each_subnode(subnode, fdt, nodeoff) {
		rc = fdt_get_گرہ_addr_سائز_by_name(fdt, subnode, quirks->reg_name, &reg_addr,
						    &سائز);
		اگر (rc < 0)
			واپس SBI_ENODEV;

		اگر (reg_addr < آغاز_addr)
			آغاز_addr = reg_addr;

		اگر (reg_addr + سائز > انتہا_addr)
			انتہا_addr = reg_addr + سائز;

		slices[++قاش_idx].addr = (خالی *)(uintptr_t)reg_addr;
	}
	slices[قاش_idx].آخری_قاش = true;

	/* Only enable the pmp to protect the EC m-mode region when it support two mode */
	اگر (quirks->two_منوال) {
		rc = sbi_domain_جڑ_جوڑو_memrange((غیرعلامتی طویل)آغاز_addr,
						  (غیرعلامتی طویل)(انتہا_addr - آغاز_addr),
						  ثنائی_ہندسہ(12),
						  (SBI_DOMAIN_MEMREGION_MMIO |
						   SBI_DOMAIN_MEMREGION_M_READABLE |
						   SBI_DOMAIN_MEMREGION_M_WRITABLE));
		اگر (rc)
			واپس rc;
	}

	واپس SBI_OK;
}

ساکن صحیح sifive_ec_cold_init(ثابت خالی *fdt, صحیح nodeoff, ثابت ساخت fdt_مطابقت *مطابقت)
{
	ثابت ساخت sifive_ec_quirks *quirks = مطابقت->ڈیٹا;
	ساخت sifive_ec_قاش *slices;
	ساخت sifive_ec *ec_dev;
	ساخت cache_device *dev;
	صحیح subnode, rc = SBI_ENOMEM;
	u32 قاش_گنتی = 0;

	/* Count the number of slices */
	fdt_for_each_subnode(subnode, fdt, nodeoff)
		قاش_گنتی++;

	/* Need at least one slice */
	اگر (!قاش_گنتی)
		واپس SBI_EINVAL;

	ec_dev = sbi_zalloc(حجم(*ec_dev));
	اگر (!ec_dev)
		واپس SBI_ENOMEM;

	slices = sbi_zalloc(قاش_گنتی * حجم(*slices));
	اگر (!slices)
		جاؤ free_ec;

	rc = sifive_ec_slices_cold_init(fdt, nodeoff, slices, quirks);
	اگر (rc)
		جاؤ free_قاش;

	dev = &ec_dev->dev;
	dev->ops = &sifive_ec_ops;
	rc = fdt_cache_جوڑو(fdt, nodeoff, dev);
	اگر (rc)
		جاؤ free_قاش;

	ec_dev->slices = slices;

	واپس SBI_OK;

free_قاش:
	sbi_free(slices);
free_ec:
	sbi_free(ec_dev);
	واپس rc;
}

ساکن ثابت ساخت sifive_ec_quirks sifive_extensiblecache0_quirks = {
	.two_منوال = false,
	.reg_name = "control",
};

ساکن ثابت ساخت sifive_ec_quirks sifive_extensiblecache4_quirks = {
	.two_منوال = true,
	.reg_name = "m_mode",
};

ساکن ثابت ساخت fdt_مطابقت sifive_ec_مطابقت[] = {
	{ .compatible = "sifive,extensiblecache4", .ڈیٹا = &sifive_extensiblecache4_quirks },
	{ .compatible = "sifive,extensiblecache3", .ڈیٹا = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache2", .ڈیٹا = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache0", .ڈیٹا = &sifive_extensiblecache0_quirks },
	{},
};

ساخت fdt_driver fdt_sifive_ec = {
	.مطابقت_جدول = sifive_ec_مطابقت,
	.init = sifive_ec_cold_init,
};
