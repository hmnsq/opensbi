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

صحیح fdt_driver_init_by_offset(ثابت تهی *fdt, صحیح nodeoff,
			      ثابت ساختار fdt_driver *ثابت *drivers)
{
	ثابت ساختار fdt_driver *driver;
	ثابت ساختار fdt_match *match;
	صحیح compat_len, prop_len, rc;
	ثابت نویسه *compat_str;

	اگر (!fdt_گره_is_enabled(fdt, nodeoff))
		بازگشت SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	اگر (!compat_str)
		بازگشت SBI_ENODEV;

	وقتی ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		برای (صحیح i = 0; (driver = drivers[i]); i++)
			برای (match = driver->match_table; match->compatible; match++)
				اگر (!memcmp(match->compatible, compat_str, compat_len))
					برو found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	بازگشت SBI_ENODEV;

found:
	اگر (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	اگر (rc < 0) {
		ثابت نویسه *name;

		name = fdt_get_name(fdt, nodeoff, پوچ);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	بازگشت rc;
}

ایستا صحیح fdt_driver_init_scan(ثابت تهی *fdt,
				ثابت ساختار fdt_driver *ثابت *drivers,
				bool one)
{
	صحیح nodeoff, rc;

	برای (nodeoff = fdt_next_گره(fdt, -1, پوچ);
	     nodeoff >= 0;
	     nodeoff = fdt_next_گره(fdt, nodeoff, پوچ)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		اگر (rc == SBI_ENODEV)
			ادامه;
		اگر (rc < 0)
			بازگشت rc;
		اگر (one)
			بازگشت 0;
	}

	بازگشت one ? SBI_ENODEV : 0;
}

صحیح fdt_driver_init_all(ثابت تهی *fdt,
			ثابت ساختار fdt_driver *ثابت *drivers)
{
	بازگشت fdt_driver_init_scan(fdt, drivers, false);
}

صحیح fdt_driver_init_one(ثابت تهی *fdt,
			ثابت ساختار fdt_driver *ثابت *drivers)
{
	بازگشت fdt_driver_init_scan(fdt, drivers, true);
}
