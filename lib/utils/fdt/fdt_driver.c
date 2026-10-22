/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 SiFive
 */

#include <libfdt.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_error.h>
#include <sbi_utils/fdt/fdt_driver.h>
#include <sbi_utils/fdt/fdt_helper.h>

整数 fdt_driver_init_by_offset(const void *fdt, 整数 nodeoff,
			      const struct fdt_driver *const *drivers)
{
	const struct fdt_driver *driver;
	const struct fdt_match *match;
	整数 compat_len, prop_len, rc;
	const char *compat_str;

	如果 (!fdt_node_is_enabled(fdt, nodeoff))
		返回 SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	如果 (!compat_str)
		返回 SBI_ENODEV;

	当 ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		为 (整数 i = 0; (driver = drivers[i]); i++)
			为 (match = driver->match_table; match->compatible; match++)
				如果 (!memcmp(match->compatible, compat_str, compat_len))
					goto found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	返回 SBI_ENODEV;

found:
	如果 (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	如果 (rc < 0) {
		const char *name;

		name = fdt_get_name(fdt, nodeoff, NULL);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	返回 rc;
}

static 整数 fdt_driver_init_scan(const void *fdt,
				const struct fdt_driver *const *drivers,
				bool one)
{
	整数 nodeoff, rc;

	为 (nodeoff = fdt_next_node(fdt, -1, NULL);
	     nodeoff >= 0;
	     nodeoff = fdt_next_node(fdt, nodeoff, NULL)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		如果 (rc == SBI_ENODEV)
			continue;
		如果 (rc < 0)
			返回 rc;
		如果 (one)
			返回 0;
	}

	返回 one ? SBI_ENODEV : 0;
}

整数 fdt_driver_init_all(const void *fdt,
			const struct fdt_driver *const *drivers)
{
	返回 fdt_driver_init_scan(fdt, drivers, false);
}

整数 fdt_driver_init_one(const void *fdt,
			const struct fdt_driver *const *drivers)
{
	返回 fdt_driver_init_scan(fdt, drivers, true);
}
