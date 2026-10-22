/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ضمن <libfdt.h>
#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_platform.h>
#ضمن <sbi_utils/cache/fdt_cache.h>
#ضمن <sbi_utils/fdt/fdt_driver.h>

#تعریف_کرو SIFIVE_EC_FEATURE_DISABLE_OFF	0x100UL
#تعریف_کرو SIFIVE_EC_FLUSH_CMD_OFF		0x800UL
#تعریف_کرو SIFIVE_EC_FLUSH_حالت_OFF	0x808UL
#تعریف_کرو SIFIVE_EC_FLUSH_ADDR_OFF	0x810UL
#تعریف_کرو SIFIVE_EC_منوال_CTRL		0xa00UL

#تعریف_کرو SIFIVE_EC_FLUSH_COMPLETION_نقاب	ثنائی_ہندسہ(0)

#تعریف_کرو SIFIVE_EC_CLEANINV_ALL_CMD	0x3

#تعریف_کرو SIFIVE_EC_FEATURE_DISABLE_VAL	0

بنية sifive_ec_quirks {
	bool two_منوال;
	حرف *reg_name;
};

بنية sifive_ec_ٹکڑا {
	فراغ *addr;
	bool آخری_ٹکڑا;
};

بنية sifive_ec {
	بنية cache_device dev;
	بنية sifive_ec_ٹکڑا *slices;
};

#تعریف_کرو منزل_ec(_dev) container_دا(_dev, بنية sifive_ec, dev)

ساكن صحيح sifive_ec_flush_all(بنية cache_device *dev)
{
	بنية sifive_ec *ec_dev = منزل_ec(dev);
	بنية sifive_ec_ٹکڑا *slices = ec_dev->slices;
	u32 cmd = SIFIVE_EC_CLEANINV_ALL_CMD, i = 0;
	فراغ *addr;

	افعل {
		addr = slices[i].addr;

		writel((صحيح)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF);
		writel((صحيح)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF + حجم(u32));
		writel(cmd, addr + SIFIVE_EC_FLUSH_CMD_OFF);
	} طالما (!slices[i++].آخری_ٹکڑا);

	i = 0;
	افعل {
		addr = slices[i].addr;
		افعل {} طالما (!(readl(addr + SIFIVE_EC_FLUSH_حالت_OFF) &
				SIFIVE_EC_FLUSH_COMPLETION_نقاب));
	} طالما (!slices[i++].آخری_ٹکڑا);

	أرجع 0;
}

صحيح sifive_ec_warm_init(بنية cache_device *dev)
{
	بنية sifive_ec *ec_dev = منزل_ec(dev);
	بنية sifive_ec_ٹکڑا *slices = ec_dev->slices;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();
	صحيح i = 0;

	إذا (dom->boot_hartid == current_hartid()) {
		افعل {
			writel(SIFIVE_EC_FEATURE_DISABLE_VAL,
			       slices[i].addr + SIFIVE_EC_FEATURE_DISABLE_OFF);
		} طالما (!slices[i++].آخری_ٹکڑا);
	}

	أرجع SBI_OK;
}

ساكن بنية cache_ops sifive_ec_ops = {
	.warm_init = sifive_ec_warm_init,
	.cache_flush_all = sifive_ec_flush_all,
};

ساكن صحيح sifive_ec_slices_cold_init(ثابت فراغ *fdt, صحيح nodeoff,
				      بنية sifive_ec_ٹکڑا *slices,
				      ثابت بنية sifive_ec_quirks *quirks)
{
	صحيح rc, subnode, ٹکڑا_idx = -1;
	u64 reg_addr, ناپ, شروع_addr = -1, سرا_addr = 0;

	fdt_for_each_subnode(subnode, fdt, nodeoff) {
		rc = fdt_get_گرہ_addr_ناپ_by_name(fdt, subnode, quirks->reg_name, &reg_addr,
						    &ناپ);
		إذا (rc < 0)
			أرجع SBI_ENODEV;

		إذا (reg_addr < شروع_addr)
			شروع_addr = reg_addr;

		إذا (reg_addr + ناپ > سرا_addr)
			سرا_addr = reg_addr + ناپ;

		slices[++ٹکڑا_idx].addr = (فراغ *)(uintptr_t)reg_addr;
	}
	slices[ٹکڑا_idx].آخری_ٹکڑا = true;

	/* Only enable the pmp to protect the EC m-mode region when it support two mode */
	إذا (quirks->two_منوال) {
		rc = sbi_domain_جڑ_جوڑو_memrange((بنانشانی لما)شروع_addr,
						  (بنانشانی لما)(سرا_addr - شروع_addr),
						  ثنائی_ہندسہ(12),
						  (SBI_DOMAIN_MEMREGION_MMIO |
						   SBI_DOMAIN_MEMREGION_M_READABLE |
						   SBI_DOMAIN_MEMREGION_M_WRITABLE));
		إذا (rc)
			أرجع rc;
	}

	أرجع SBI_OK;
}

ساكن صحيح sifive_ec_cold_init(ثابت فراغ *fdt, صحيح nodeoff, ثابت بنية fdt_میل *میل)
{
	ثابت بنية sifive_ec_quirks *quirks = میل->انکڑے;
	بنية sifive_ec_ٹکڑا *slices;
	بنية sifive_ec *ec_dev;
	بنية cache_device *dev;
	صحيح subnode, rc = SBI_ENOMEM;
	u32 ٹکڑا_گنتی = 0;

	/* Count the number of slices */
	fdt_for_each_subnode(subnode, fdt, nodeoff)
		ٹکڑا_گنتی++;

	/* Need at least one slice */
	إذا (!ٹکڑا_گنتی)
		أرجع SBI_EINVAL;

	ec_dev = sbi_zalloc(حجم(*ec_dev));
	إذا (!ec_dev)
		أرجع SBI_ENOMEM;

	slices = sbi_zalloc(ٹکڑا_گنتی * حجم(*slices));
	إذا (!slices)
		اذهب free_ec;

	rc = sifive_ec_slices_cold_init(fdt, nodeoff, slices, quirks);
	إذا (rc)
		اذهب free_ٹکڑا;

	dev = &ec_dev->dev;
	dev->ops = &sifive_ec_ops;
	rc = fdt_cache_جوڑو(fdt, nodeoff, dev);
	إذا (rc)
		اذهب free_ٹکڑا;

	ec_dev->slices = slices;

	أرجع SBI_OK;

free_ٹکڑا:
	sbi_free(slices);
free_ec:
	sbi_free(ec_dev);
	أرجع rc;
}

ساكن ثابت بنية sifive_ec_quirks sifive_extensiblecache0_quirks = {
	.two_منوال = false,
	.reg_name = "control",
};

ساكن ثابت بنية sifive_ec_quirks sifive_extensiblecache4_quirks = {
	.two_منوال = true,
	.reg_name = "m_mode",
};

ساكن ثابت بنية fdt_میل sifive_ec_میل[] = {
	{ .compatible = "sifive,extensiblecache4", .انکڑے = &sifive_extensiblecache4_quirks },
	{ .compatible = "sifive,extensiblecache3", .انکڑے = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache2", .انکڑے = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache0", .انکڑے = &sifive_extensiblecache0_quirks },
	{},
};

بنية fdt_driver fdt_sifive_ec = {
	.میل_جدول = sifive_ec_میل,
	.init = sifive_ec_cold_init,
};
