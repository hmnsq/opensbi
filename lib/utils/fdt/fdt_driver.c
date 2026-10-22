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

ціле fdt_driver_init_by_offset(const void *fdt, ціле nodeoff,
			      const struct fdt_driver *const *drivers)
{
	const struct fdt_driver *driver;
	const struct fdt_match *match;
	ціле compat_len, prop_len, rc;
	const char *compat_str;

	якщо (!fdt_node_is_enabled(fdt, nodeoff))
		повернути SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	якщо (!compat_str)
		повернути SBI_ENODEV;

	поки ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		для (ціле i = 0; (driver = drivers[i]); i++)
			для (match = driver->match_table; match->compatible; match++)
				якщо (!memcmp(match->compatible, compat_str, compat_len))
					goto found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	повернути SBI_ENODEV;

found:
	якщо (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	якщо (rc < 0) {
		const char *name;

		name = fdt_get_name(fdt, nodeoff, NULL);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	повернути rc;
}

static ціле fdt_driver_init_scan(const void *fdt,
				const struct fdt_driver *const *drivers,
				bool one)
{
	ціле nodeoff, rc;

	для (nodeoff = fdt_next_node(fdt, -1, NULL);
	     nodeoff >= 0;
	     nodeoff = fdt_next_node(fdt, nodeoff, NULL)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		якщо (rc == SBI_ENODEV)
			continue;
		якщо (rc < 0)
			повернути rc;
		якщо (one)
			повернути 0;
	}

	повернути one ? SBI_ENODEV : 0;
}

ціле fdt_driver_init_all(const void *fdt,
			const struct fdt_driver *const *drivers)
{
	повернути fdt_driver_init_scan(fdt, drivers, false);
}

ціле fdt_driver_init_one(const void *fdt,
			const struct fdt_driver *const *drivers)
{
	повернути fdt_driver_init_scan(fdt, drivers, true);
}
