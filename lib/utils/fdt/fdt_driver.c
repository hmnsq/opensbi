/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 SiFive
 */

#شامل <libfdt.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi_utils/fdt/fdt_driver.h>
#شامل <sbi_utils/fdt/fdt_helper.h>

صحیح fdt_driver_init_by_انحراف(ثابت خالی *fdt, صحیح nodeoff,
			      ثابت ساخت fdt_driver *ثابت *drivers)
{
	ثابت ساخت fdt_driver *driver;
	ثابت ساخت fdt_مطابقت *مطابقت;
	صحیح compat_len, prop_len, rc;
	ثابت حرف *compat_str;

	اگر (!fdt_گرہ_is_enabled(fdt, nodeoff))
		واپس SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	اگر (!compat_str)
		واپس SBI_ENODEV;

	جب ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		برائے (صحیح i = 0; (driver = drivers[i]); i++)
			برائے (مطابقت = driver->مطابقت_جدول; مطابقت->compatible; مطابقت++)
				اگر (!memcmp(مطابقت->compatible, compat_str, compat_len))
					جاؤ ملا;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	واپس SBI_ENODEV;

ملا:
	اگر (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   مطابقت->compatible);

	rc = driver->init(fdt, nodeoff, مطابقت);
	اگر (rc < 0) {
		ثابت حرف *name;

		name = fdt_get_name(fdt, nodeoff, تہی);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, مطابقت->compatible, rc);
	}

	واپس rc;
}

ساکن صحیح fdt_driver_init_جائزہ(ثابت خالی *fdt,
				ثابت ساخت fdt_driver *ثابت *drivers,
				bool one)
{
	صحیح nodeoff, rc;

	برائے (nodeoff = fdt_اگلا_گرہ(fdt, -1, تہی);
	     nodeoff >= 0;
	     nodeoff = fdt_اگلا_گرہ(fdt, nodeoff, تہی)) {
		rc = fdt_driver_init_by_انحراف(fdt, nodeoff, drivers);
		اگر (rc == SBI_ENODEV)
			جاری;
		اگر (rc < 0)
			واپس rc;
		اگر (one)
			واپس 0;
	}

	واپس one ? SBI_ENODEV : 0;
}

صحیح fdt_driver_init_all(ثابت خالی *fdt,
			ثابت ساخت fdt_driver *ثابت *drivers)
{
	واپس fdt_driver_init_جائزہ(fdt, drivers, false);
}

صحیح fdt_driver_init_one(ثابت خالی *fdt,
			ثابت ساخت fdt_driver *ثابت *drivers)
{
	واپس fdt_driver_init_جائزہ(fdt, drivers, true);
}
