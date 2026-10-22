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

цео fdt_driver_init_by_offset(const void *fdt, цео nodeoff,
			      const struct fdt_driver *const *drivers)
{
	const struct fdt_driver *driver;
	const struct fdt_match *match;
	цео compat_len, prop_len, rc;
	const char *compat_str;

	ако (!fdt_node_is_enabled(fdt, nodeoff))
		врати SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	ако (!compat_str)
		врати SBI_ENODEV;

	док ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		за (цео i = 0; (driver = drivers[i]); i++)
			за (match = driver->match_table; match->compatible; match++)
				ако (!memcmp(match->compatible, compat_str, compat_len))
					goto found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	врати SBI_ENODEV;

found:
	ако (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	ако (rc < 0) {
		const char *name;

		name = fdt_get_name(fdt, nodeoff, NULL);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	врати rc;
}

static цео fdt_driver_init_scan(const void *fdt,
				const struct fdt_driver *const *drivers,
				bool one)
{
	цео nodeoff, rc;

	за (nodeoff = fdt_next_node(fdt, -1, NULL);
	     nodeoff >= 0;
	     nodeoff = fdt_next_node(fdt, nodeoff, NULL)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		ако (rc == SBI_ENODEV)
			continue;
		ако (rc < 0)
			врати rc;
		ако (one)
			врати 0;
	}

	врати one ? SBI_ENODEV : 0;
}

цео fdt_driver_init_all(const void *fdt,
			const struct fdt_driver *const *drivers)
{
	врати fdt_driver_init_scan(fdt, drivers, false);
}

цео fdt_driver_init_one(const void *fdt,
			const struct fdt_driver *const *drivers)
{
	врати fdt_driver_init_scan(fdt, drivers, true);
}
