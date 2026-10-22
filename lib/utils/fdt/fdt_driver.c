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

صحيح fdt_driver_init_by_offset(ثابت فراغ *fdt, صحيح nodeoff,
			      ثابت بنية fdt_driver *ثابت *drivers)
{
	ثابت بنية fdt_driver *driver;
	ثابت بنية fdt_match *match;
	صحيح compat_len, prop_len, rc;
	ثابت حرف *compat_str;

	אם (!fdt_node_is_enabled(fdt, nodeoff))
		החזר SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	אם (!compat_str)
		החזר SBI_ENODEV;

	כלעוד ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		עבור (صحيح i = 0; (driver = drivers[i]); i++)
			עבור (match = driver->match_table; match->compatible; match++)
				אם (!memcmp(match->compatible, compat_str, compat_len))
					اذهب found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	החזר SBI_ENODEV;

found:
	אם (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	אם (rc < 0) {
		ثابت حرف *name;

		name = fdt_get_name(fdt, nodeoff, عدم);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	החזר rc;
}

ساكن صحيح fdt_driver_init_scan(ثابت فراغ *fdt,
				ثابت بنية fdt_driver *ثابت *drivers,
				bool one)
{
	صحيح nodeoff, rc;

	עבור (nodeoff = fdt_next_node(fdt, -1, عدم);
	     nodeoff >= 0;
	     nodeoff = fdt_next_node(fdt, nodeoff, عدم)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		אם (rc == SBI_ENODEV)
			استمر;
		אם (rc < 0)
			החזר rc;
		אם (one)
			החזר 0;
	}

	החזר one ? SBI_ENODEV : 0;
}

صحيح fdt_driver_init_all(ثابت فراغ *fdt,
			ثابت بنية fdt_driver *ثابت *drivers)
{
	החזר fdt_driver_init_scan(fdt, drivers, false);
}

صحيح fdt_driver_init_one(ثابت فراغ *fdt,
			ثابت بنية fdt_driver *ثابت *drivers)
{
	החזר fdt_driver_init_scan(fdt, drivers, true);
}
