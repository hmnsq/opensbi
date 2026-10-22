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

số_nguyên fdt_driver_init_by_offset(const void *fdt, số_nguyên nodeoff,
			      const struct fdt_driver *const *drivers)
{
	const struct fdt_driver *driver;
	const struct fdt_match *match;
	số_nguyên compat_len, prop_len, rc;
	const char *compat_str;

	nếu (!fdt_node_is_enabled(fdt, nodeoff))
		trả_về SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	nếu (!compat_str)
		trả_về SBI_ENODEV;

	trong_khi ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		cho (số_nguyên i = 0; (driver = drivers[i]); i++)
			cho (match = driver->match_table; match->compatible; match++)
				nếu (!memcmp(match->compatible, compat_str, compat_len))
					goto found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	trả_về SBI_ENODEV;

found:
	nếu (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	nếu (rc < 0) {
		const char *name;

		name = fdt_get_name(fdt, nodeoff, NULL);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	trả_về rc;
}

static số_nguyên fdt_driver_init_scan(const void *fdt,
				const struct fdt_driver *const *drivers,
				bool one)
{
	số_nguyên nodeoff, rc;

	cho (nodeoff = fdt_next_node(fdt, -1, NULL);
	     nodeoff >= 0;
	     nodeoff = fdt_next_node(fdt, nodeoff, NULL)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		nếu (rc == SBI_ENODEV)
			continue;
		nếu (rc < 0)
			trả_về rc;
		nếu (one)
			trả_về 0;
	}

	trả_về one ? SBI_ENODEV : 0;
}

số_nguyên fdt_driver_init_all(const void *fdt,
			const struct fdt_driver *const *drivers)
{
	trả_về fdt_driver_init_scan(fdt, drivers, false);
}

số_nguyên fdt_driver_init_one(const void *fdt,
			const struct fdt_driver *const *drivers)
{
	trả_về fdt_driver_init_scan(fdt, drivers, true);
}
