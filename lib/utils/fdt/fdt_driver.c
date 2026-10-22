/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 SiFive
 */

#ضمن <libfdt.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi_utils/fdt/fdt_driver.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>

صحيح fdt_driver_init_by_جابه_جایی(ثابت فراغ *fdt, صحيح nodeoff,
			      ثابت بنية fdt_driver *ثابت *drivers)
{
	ثابت بنية fdt_driver *driver;
	ثابت بنية fdt_تطابق *تطابق;
	صحيح compat_len, prop_len, rc;
	ثابت حرف *compat_str;

	إذا (!fdt_گره_is_enabled(fdt, nodeoff))
		أرجع SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	إذا (!compat_str)
		أرجع SBI_ENODEV;

	طالما ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		لكل (صحيح i = 0; (driver = drivers[i]); i++)
			لكل (تطابق = driver->تطابق_جدول; تطابق->compatible; تطابق++)
				إذا (!memcmp(تطابق->compatible, compat_str, compat_len))
					اذهب یافت;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	أرجع SBI_ENODEV;

یافت:
	إذا (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   تطابق->compatible);

	rc = driver->init(fdt, nodeoff, تطابق);
	إذا (rc < 0) {
		ثابت حرف *name;

		name = fdt_get_name(fdt, nodeoff, عدم);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, تطابق->compatible, rc);
	}

	أرجع rc;
}

ساكن صحيح fdt_driver_init_پیمایش(ثابت فراغ *fdt,
				ثابت بنية fdt_driver *ثابت *drivers,
				bool one)
{
	صحيح nodeoff, rc;

	لكل (nodeoff = fdt_بعدی_گره(fdt, -1, عدم);
	     nodeoff >= 0;
	     nodeoff = fdt_بعدی_گره(fdt, nodeoff, عدم)) {
		rc = fdt_driver_init_by_جابه_جایی(fdt, nodeoff, drivers);
		إذا (rc == SBI_ENODEV)
			استمر;
		إذا (rc < 0)
			أرجع rc;
		إذا (one)
			أرجع 0;
	}

	أرجع one ? SBI_ENODEV : 0;
}

صحيح fdt_driver_init_all(ثابت فراغ *fdt,
			ثابت بنية fdt_driver *ثابت *drivers)
{
	أرجع fdt_driver_init_پیمایش(fdt, drivers, false);
}

صحيح fdt_driver_init_one(ثابت فراغ *fdt,
			ثابت بنية fdt_driver *ثابت *drivers)
{
	أرجع fdt_driver_init_پیمایش(fdt, drivers, true);
}
