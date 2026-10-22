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

ཧྲིལ fdt_driver_init_by_offset(const void *fdt, ཧྲིལ nodeoff,
			      const sa_nracanaa fdt_driver *const *drivers)
{
	const sa_nracanaa fdt_driver *driver;
	const sa_nracanaa fdt_match *match;
	ཧྲིལ compat_len, prop_len, rc;
	const var_na *compat_str;

	གལ་ཏེ (!fdt_node_is_enabled(fdt, nodeoff))
		ཕྱིར SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	གལ་ཏེ (!compat_str)
		ཕྱིར SBI_ENODEV;

	སྐབས ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		ཆེད་དུ (ཧྲིལ i = 0; (driver = drivers[i]); i++)
			ཆེད་དུ (match = driver->match_table; match->compatible; match++)
				གལ་ཏེ (!memcmp(match->compatible, compat_str, compat_len))
					goto found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	ཕྱིར SBI_ENODEV;

found:
	གལ་ཏེ (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	གལ་ཏེ (rc < 0) {
		const var_na *name;

		name = fdt_get_name(fdt, nodeoff, NULL);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	ཕྱིར rc;
}

static ཧྲིལ fdt_driver_init_scan(const void *fdt,
				const sa_nracanaa fdt_driver *const *drivers,
				bool one)
{
	ཧྲིལ nodeoff, rc;

	ཆེད་དུ (nodeoff = fdt_next_node(fdt, -1, NULL);
	     nodeoff >= 0;
	     nodeoff = fdt_next_node(fdt, nodeoff, NULL)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		གལ་ཏེ (rc == SBI_ENODEV)
			continue;
		གལ་ཏེ (rc < 0)
			ཕྱིར rc;
		གལ་ཏེ (one)
			ཕྱིར 0;
	}

	ཕྱིར one ? SBI_ENODEV : 0;
}

ཧྲིལ fdt_driver_init_all(const void *fdt,
			const sa_nracanaa fdt_driver *const *drivers)
{
	ཕྱིར fdt_driver_init_scan(fdt, drivers, false);
}

ཧྲིལ fdt_driver_init_one(const void *fdt,
			const sa_nracanaa fdt_driver *const *drivers)
{
	ཕྱིར fdt_driver_init_scan(fdt, drivers, true);
}
