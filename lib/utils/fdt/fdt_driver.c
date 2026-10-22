/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 SiFive
 */

#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>

পূর্ণসংখ্যা fdt_driver_init_by_offset(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
			      ধ্রুবক কাঠামো fdt_driver *ধ্রুবক *drivers)
{
	ধ্রুবক কাঠামো fdt_driver *driver;
	ধ্রুবক কাঠামো fdt_match *match;
	পূর্ণসংখ্যা compat_len, prop_len, rc;
	ধ্রুবক অক্ষর *compat_str;

	যদি (!fdt_গ্রন্থি_is_enabled(fdt, nodeoff))
		ফেরত SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	যদি (!compat_str)
		ফেরত SBI_ENODEV;

	যখন ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		জন্য (পূর্ণসংখ্যা i = 0; (driver = drivers[i]); i++)
			জন্য (match = driver->match_table; match->compatible; match++)
				যদি (!memcmp(match->compatible, compat_str, compat_len))
					যাও found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	ফেরত SBI_ENODEV;

found:
	যদি (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	যদি (rc < 0) {
		ধ্রুবক অক্ষর *name;

		name = fdt_get_name(fdt, nodeoff, ফাঁকা);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	ফেরত rc;
}

স্থির পূর্ণসংখ্যা fdt_driver_init_scan(ধ্রুবক শূন্য *fdt,
				ধ্রুবক কাঠামো fdt_driver *ধ্রুবক *drivers,
				bool one)
{
	পূর্ণসংখ্যা nodeoff, rc;

	জন্য (nodeoff = fdt_পরবর্তী_গ্রন্থি(fdt, -1, ফাঁকা);
	     nodeoff >= 0;
	     nodeoff = fdt_পরবর্তী_গ্রন্থি(fdt, nodeoff, ফাঁকা)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		যদি (rc == SBI_ENODEV)
			চালু;
		যদি (rc < 0)
			ফেরত rc;
		যদি (one)
			ফেরত 0;
	}

	ফেরত one ? SBI_ENODEV : 0;
}

পূর্ণসংখ্যা fdt_driver_init_all(ধ্রুবক শূন্য *fdt,
			ধ্রুবক কাঠামো fdt_driver *ধ্রুবক *drivers)
{
	ফেরত fdt_driver_init_scan(fdt, drivers, false);
}

পূর্ণসংখ্যা fdt_driver_init_one(ধ্রুবক শূন্য *fdt,
			ধ্রুবক কাঠামো fdt_driver *ধ্রুবক *drivers)
{
	ফেরত fdt_driver_init_scan(fdt, drivers, true);
}
