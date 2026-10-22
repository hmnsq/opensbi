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

정수 fdt_driver_init_by_offset(const void *fdt, 정수 nodeoff,
			      const struct fdt_driver *const *drivers)
{
	const struct fdt_driver *driver;
	const struct fdt_match *match;
	정수 compat_len, prop_len, rc;
	const char *compat_str;

	만약 (!fdt_node_is_enabled(fdt, nodeoff))
		반환 SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	만약 (!compat_str)
		반환 SBI_ENODEV;

	동안 ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		위해 (정수 i = 0; (driver = drivers[i]); i++)
			위해 (match = driver->match_table; match->compatible; match++)
				만약 (!memcmp(match->compatible, compat_str, compat_len))
					goto found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	반환 SBI_ENODEV;

found:
	만약 (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	만약 (rc < 0) {
		const char *name;

		name = fdt_get_name(fdt, nodeoff, NULL);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	반환 rc;
}

static 정수 fdt_driver_init_scan(const void *fdt,
				const struct fdt_driver *const *drivers,
				bool one)
{
	정수 nodeoff, rc;

	위해 (nodeoff = fdt_next_node(fdt, -1, NULL);
	     nodeoff >= 0;
	     nodeoff = fdt_next_node(fdt, nodeoff, NULL)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		만약 (rc == SBI_ENODEV)
			continue;
		만약 (rc < 0)
			반환 rc;
		만약 (one)
			반환 0;
	}

	반환 one ? SBI_ENODEV : 0;
}

정수 fdt_driver_init_all(const void *fdt,
			const struct fdt_driver *const *drivers)
{
	반환 fdt_driver_init_scan(fdt, drivers, false);
}

정수 fdt_driver_init_one(const void *fdt,
			const struct fdt_driver *const *drivers)
{
	반환 fdt_driver_init_scan(fdt, drivers, true);
}
