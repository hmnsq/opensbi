/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 SiFive
 */

#অন্তর্ভাৱ <libfdt.h>
#অন্তর্ভাৱ <sbi/sbi_console.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>

পূর্ণাঙ্ক fdt_driver_init_by_offset(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			      ধ্রুৱ সংরচনা fdt_driver *ধ্রুৱ *drivers)
{
	ধ্রুৱ সংরচনা fdt_driver *driver;
	ধ্রুৱ সংরচনা fdt_match *match;
	পূর্ণাঙ্ক compat_len, prop_len, rc;
	ধ্রুৱ ৱর্ণ *compat_str;

	যদি (!fdt_গ্ৰন্থি_is_enabled(fdt, nodeoff))
		ঘূৰাই SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	যদি (!compat_str)
		ঘূৰাই SBI_ENODEV;

	যেতিয়ালৈ ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		বাবে (পূর্ণাঙ্ক i = 0; (driver = drivers[i]); i++)
			বাবে (match = driver->match_table; match->compatible; match++)
				যদি (!memcmp(match->compatible, compat_str, compat_len))
					গচ্ছ found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	ঘূৰাই SBI_ENODEV;

found:
	যদি (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	যদি (rc < 0) {
		ধ্রুৱ ৱর্ণ *name;

		name = fdt_get_name(fdt, nodeoff, রিক্ত);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	ঘূৰাই rc;
}

স্থির পূর্ণাঙ্ক fdt_driver_init_scan(ধ্রুৱ শূন্য *fdt,
				ধ্রুৱ সংরচনা fdt_driver *ধ্রুৱ *drivers,
				bool one)
{
	পূর্ণাঙ্ক nodeoff, rc;

	বাবে (nodeoff = fdt_next_গ্ৰন্থি(fdt, -1, রিক্ত);
	     nodeoff >= 0;
	     nodeoff = fdt_next_গ্ৰন্থি(fdt, nodeoff, রিক্ত)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		যদি (rc == SBI_ENODEV)
			অনুৱর্ত;
		যদি (rc < 0)
			ঘূৰাই rc;
		যদি (one)
			ঘূৰাই 0;
	}

	ঘূৰাই one ? SBI_ENODEV : 0;
}

পূর্ণাঙ্ক fdt_driver_init_all(ধ্রুৱ শূন্য *fdt,
			ধ্রুৱ সংরচনা fdt_driver *ধ্রুৱ *drivers)
{
	ঘূৰাই fdt_driver_init_scan(fdt, drivers, false);
}

পূর্ণাঙ্ক fdt_driver_init_one(ধ্রুৱ শূন্য *fdt,
			ধ্রুৱ সংরচনা fdt_driver *ধ্রুৱ *drivers)
{
	ঘূৰাই fdt_driver_init_scan(fdt, drivers, true);
}
