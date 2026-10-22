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

#وصف_ڪريو SIFIVE_EC_FEATURE_DISABLE_OFF	0x100UL
#وصف_ڪريو SIFIVE_EC_FLUSH_CMD_OFF		0x800UL
#وصف_ڪريو SIFIVE_EC_FLUSH_حالت_OFF	0x808UL
#وصف_ڪريو SIFIVE_EC_FLUSH_ADDR_OFF	0x810UL
#وصف_ڪريو SIFIVE_EC_گهڻائي_CTRL		0xa00UL

#وصف_ڪريو SIFIVE_EC_FLUSH_COMPLETION_نقاب	ٻٽو_انگ(0)

#وصف_ڪريو SIFIVE_EC_CLEANINV_ALL_CMD	0x3

#وصف_ڪريو SIFIVE_EC_FEATURE_DISABLE_VAL	0

بنية sifive_ec_quirks {
	bool two_گهڻائي;
	حرف *reg_name;
};

بنية sifive_ec_ٽڪرو {
	فراغ *addr;
	bool آخري_ٽڪرو;
};

بنية sifive_ec {
	بنية cache_device dev;
	بنية sifive_ec_ٽڪرو *slices;
};

#وصف_ڪريو منزل_ec(_dev) container_جو(_dev, بنية sifive_ec, dev)

ساكن صحيح sifive_ec_flush_all(بنية cache_device *dev)
{
	بنية sifive_ec *ec_dev = منزل_ec(dev);
	بنية sifive_ec_ٽڪرو *slices = ec_dev->slices;
	u32 cmd = SIFIVE_EC_CLEANINV_ALL_CMD, i = 0;
	فراغ *addr;

	افعل {
		addr = slices[i].addr;

		writel((صحيح)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF);
		writel((صحيح)-1, addr + SIFIVE_EC_FLUSH_ADDR_OFF + حجم(u32));
		writel(cmd, addr + SIFIVE_EC_FLUSH_CMD_OFF);
	} طالما (!slices[i++].آخري_ٽڪرو);

	i = 0;
	افعل {
		addr = slices[i].addr;
		افعل {} طالما (!(readl(addr + SIFIVE_EC_FLUSH_حالت_OFF) &
				SIFIVE_EC_FLUSH_COMPLETION_نقاب));
	} طالما (!slices[i++].آخري_ٽڪرو);

	أرجع 0;
}

صحيح sifive_ec_warm_init(بنية cache_device *dev)
{
	بنية sifive_ec *ec_dev = منزل_ec(dev);
	بنية sifive_ec_ٽڪرو *slices = ec_dev->slices;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();
	صحيح i = 0;

	إذا (dom->boot_hartid == current_hartid()) {
		افعل {
			writel(SIFIVE_EC_FEATURE_DISABLE_VAL,
			       slices[i].addr + SIFIVE_EC_FEATURE_DISABLE_OFF);
		} طالما (!slices[i++].آخري_ٽڪرو);
	}

	أرجع SBI_OK;
}

ساكن بنية cache_ops sifive_ec_ops = {
	.warm_init = sifive_ec_warm_init,
	.cache_flush_all = sifive_ec_flush_all,
};

ساكن صحيح sifive_ec_slices_cold_init(ثابت فراغ *fdt, صحيح nodeoff,
				      بنية sifive_ec_ٽڪرو *slices,
				      ثابت بنية sifive_ec_quirks *quirks)
{
	صحيح rc, subnode, ٽڪرو_idx = -1;
	u64 reg_addr, سائيز, شروعات_addr = -1, پڇاڙي_addr = 0;

	fdt_for_each_subnode(subnode, fdt, nodeoff) {
		rc = fdt_get_ڳنڍ_addr_سائيز_by_name(fdt, subnode, quirks->reg_name, &reg_addr,
						    &سائيز);
		إذا (rc < 0)
			أرجع SBI_ENODEV;

		إذا (reg_addr < شروعات_addr)
			شروعات_addr = reg_addr;

		إذا (reg_addr + سائيز > پڇاڙي_addr)
			پڇاڙي_addr = reg_addr + سائيز;

		slices[++ٽڪرو_idx].addr = (فراغ *)(uintptr_t)reg_addr;
	}
	slices[ٽڪرو_idx].آخري_ٽڪرو = true;

	/* Only enable the pmp to protect the EC m-mode region when it support two mode */
	إذا (quirks->two_گهڻائي) {
		rc = sbi_domain_پاڙ_جوڙيو_memrange((بنانشان ڊگهو)شروعات_addr,
						  (بنانشان ڊگهو)(پڇاڙي_addr - شروعات_addr),
						  ٻٽو_انگ(12),
						  (SBI_DOMAIN_MEMREGION_MMIO |
						   SBI_DOMAIN_MEMREGION_M_READABLE |
						   SBI_DOMAIN_MEMREGION_M_WRITABLE));
		إذا (rc)
			أرجع rc;
	}

	أرجع SBI_OK;
}

ساكن صحيح sifive_ec_cold_init(ثابت فراغ *fdt, صحيح nodeoff, ثابت بنية fdt_ميلاپ *ميلاپ)
{
	ثابت بنية sifive_ec_quirks *quirks = ميلاپ->معلومات;
	بنية sifive_ec_ٽڪرو *slices;
	بنية sifive_ec *ec_dev;
	بنية cache_device *dev;
	صحيح subnode, rc = SBI_ENOMEM;
	u32 ٽڪرو_ڳڻپ = 0;

	/* Count the number of slices */
	fdt_for_each_subnode(subnode, fdt, nodeoff)
		ٽڪرو_ڳڻپ++;

	/* Need at least one slice */
	إذا (!ٽڪرو_ڳڻپ)
		أرجع SBI_EINVAL;

	ec_dev = sbi_zalloc(حجم(*ec_dev));
	إذا (!ec_dev)
		أرجع SBI_ENOMEM;

	slices = sbi_zalloc(ٽڪرو_ڳڻپ * حجم(*slices));
	إذا (!slices)
		اذهب free_ec;

	rc = sifive_ec_slices_cold_init(fdt, nodeoff, slices, quirks);
	إذا (rc)
		اذهب free_ٽڪرو;

	dev = &ec_dev->dev;
	dev->ops = &sifive_ec_ops;
	rc = fdt_cache_جوڙيو(fdt, nodeoff, dev);
	إذا (rc)
		اذهب free_ٽڪرو;

	ec_dev->slices = slices;

	أرجع SBI_OK;

free_ٽڪرو:
	sbi_free(slices);
free_ec:
	sbi_free(ec_dev);
	أرجع rc;
}

ساكن ثابت بنية sifive_ec_quirks sifive_extensiblecache0_quirks = {
	.two_گهڻائي = false,
	.reg_name = "control",
};

ساكن ثابت بنية sifive_ec_quirks sifive_extensiblecache4_quirks = {
	.two_گهڻائي = true,
	.reg_name = "m_mode",
};

ساكن ثابت بنية fdt_ميلاپ sifive_ec_ميلاپ[] = {
	{ .compatible = "sifive,extensiblecache4", .معلومات = &sifive_extensiblecache4_quirks },
	{ .compatible = "sifive,extensiblecache3", .معلومات = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache2", .معلومات = &sifive_extensiblecache0_quirks },
	{ .compatible = "sifive,extensiblecache0", .معلومات = &sifive_extensiblecache0_quirks },
	{},
};

بنية fdt_driver fdt_sifive_ec = {
	.ميلاپ_جدول = sifive_ec_ميلاپ,
	.init = sifive_ec_cold_init,
};
