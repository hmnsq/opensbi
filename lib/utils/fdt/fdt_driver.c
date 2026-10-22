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

цяло fdt_driver_init_by_offset(const void *fdt, цяло nodeoff,
			      const struct fdt_driver *const *drivers)
{
	const struct fdt_driver *driver;
	const struct fdt_match *match;
	цяло compat_len, prop_len, rc;
	const char *compat_str;

	ако (!fdt_node_is_enabled(fdt, nodeoff))
		върни SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	ако (!compat_str)
		върни SBI_ENODEV;

	докато ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		за (цяло i = 0; (driver = drivers[i]); i++)
			за (match = driver->match_table; match->compatible; match++)
				ако (!memcmp(match->compatible, compat_str, compat_len))
					goto found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	върни SBI_ENODEV;

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

	върни rc;
}

static цяло fdt_driver_init_scan(const void *fdt,
				const struct fdt_driver *const *drivers,
				bool one)
{
	цяло nodeoff, rc;

	за (nodeoff = fdt_next_node(fdt, -1, NULL);
	     nodeoff >= 0;
	     nodeoff = fdt_next_node(fdt, nodeoff, NULL)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		ако (rc == SBI_ENODEV)
			continue;
		ако (rc < 0)
			върни rc;
		ако (one)
			върни 0;
	}

	върни one ? SBI_ENODEV : 0;
}

цяло fdt_driver_init_all(const void *fdt,
			const struct fdt_driver *const *drivers)
{
	върни fdt_driver_init_scan(fdt, drivers, false);
}

цяло fdt_driver_init_one(const void *fdt,
			const struct fdt_driver *const *drivers)
{
	върни fdt_driver_init_scan(fdt, drivers, true);
}
