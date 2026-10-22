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

পূর্ণাঙ্ক fdt_driver_init_by_offset(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			      ধ্রুব সংরচনা fdt_driver *ধ্রুব *drivers)
{
	ধ্রুব সংরচনা fdt_driver *driver;
	ধ্রুব সংরচনা fdt_match *match;
	পূর্ণাঙ্ক compat_len, prop_len, rc;
	ধ্রুব বর্ণ *compat_str;

	যদি (!fdt_গ্রন্থি_is_enabled(fdt, nodeoff))
		প্রত্যাগম SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	যদি (!compat_str)
		প্রত্যাগম SBI_ENODEV;

	যাবত্ ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		প্রতি (পূর্ণাঙ্ক i = 0; (driver = drivers[i]); i++)
			প্রতি (match = driver->match_table; match->compatible; match++)
				যদি (!memcmp(match->compatible, compat_str, compat_len))
					গচ্ছ found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	প্রত্যাগম SBI_ENODEV;

found:
	যদি (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	যদি (rc < 0) {
		ধ্রুব বর্ণ *name;

		name = fdt_get_name(fdt, nodeoff, রিক্ত);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	প্রত্যাগম rc;
}

স্থির পূর্ণাঙ্ক fdt_driver_init_scan(ধ্রুব শূন্য *fdt,
				ধ্রুব সংরচনা fdt_driver *ধ্রুব *drivers,
				bool one)
{
	পূর্ণাঙ্ক nodeoff, rc;

	প্রতি (nodeoff = fdt_next_গ্রন্থি(fdt, -1, রিক্ত);
	     nodeoff >= 0;
	     nodeoff = fdt_next_গ্রন্থি(fdt, nodeoff, রিক্ত)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		যদি (rc == SBI_ENODEV)
			অনুবর্ত;
		যদি (rc < 0)
			প্রত্যাগম rc;
		যদি (one)
			প্রত্যাগম 0;
	}

	প্রত্যাগম one ? SBI_ENODEV : 0;
}

পূর্ণাঙ্ক fdt_driver_init_all(ধ্রুব শূন্য *fdt,
			ধ্রুব সংরচনা fdt_driver *ধ্রুব *drivers)
{
	প্রত্যাগম fdt_driver_init_scan(fdt, drivers, false);
}

পূর্ণাঙ্ক fdt_driver_init_one(ধ্রুব শূন্য *fdt,
			ধ্রুব সংরচনা fdt_driver *ধ্রুব *drivers)
{
	প্রত্যাগম fdt_driver_init_scan(fdt, drivers, true);
}
