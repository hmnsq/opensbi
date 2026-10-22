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

целое fdt_driver_init_by_offset(const void *fdt, целое nodeoff,
			      const struct fdt_driver *const *drivers)
{
	const struct fdt_driver *driver;
	const struct fdt_match *match;
	целое compat_len, prop_len, rc;
	const char *compat_str;

	если (!fdt_node_is_enabled(fdt, nodeoff))
		вернуть SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	если (!compat_str)
		вернуть SBI_ENODEV;

	пока ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		для (целое i = 0; (driver = drivers[i]); i++)
			для (match = driver->match_table; match->compatible; match++)
				если (!memcmp(match->compatible, compat_str, compat_len))
					goto found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	вернуть SBI_ENODEV;

found:
	если (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	если (rc < 0) {
		const char *name;

		name = fdt_get_name(fdt, nodeoff, NULL);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	вернуть rc;
}

static целое fdt_driver_init_scan(const void *fdt,
				const struct fdt_driver *const *drivers,
				bool one)
{
	целое nodeoff, rc;

	для (nodeoff = fdt_next_node(fdt, -1, NULL);
	     nodeoff >= 0;
	     nodeoff = fdt_next_node(fdt, nodeoff, NULL)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		если (rc == SBI_ENODEV)
			continue;
		если (rc < 0)
			вернуть rc;
		если (one)
			вернуть 0;
	}

	вернуть one ? SBI_ENODEV : 0;
}

целое fdt_driver_init_all(const void *fdt,
			const struct fdt_driver *const *drivers)
{
	вернуть fdt_driver_init_scan(fdt, drivers, false);
}

целое fdt_driver_init_one(const void *fdt,
			const struct fdt_driver *const *drivers)
{
	вернуть fdt_driver_init_scan(fdt, drivers, true);
}
