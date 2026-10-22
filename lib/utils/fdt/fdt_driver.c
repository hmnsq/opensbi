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

	གལ་སྲིད (!fdt_node_is_enabled(fdt, nodeoff))
		ལོག SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	གལ་སྲིད (!compat_str)
		ལོག SBI_ENODEV;

	སྐབས ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		དོན་ལུ (ཧྲིལ i = 0; (driver = drivers[i]); i++)
			དོན་ལུ (match = driver->match_table; match->compatible; match++)
				གལ་སྲིད (!memcmp(match->compatible, compat_str, compat_len))
					goto found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	ལོག SBI_ENODEV;

found:
	གལ་སྲིད (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	གལ་སྲིད (rc < 0) {
		const var_na *name;

		name = fdt_get_name(fdt, nodeoff, NULL);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	ལོག rc;
}

static ཧྲིལ fdt_driver_init_scan(const void *fdt,
				const sa_nracanaa fdt_driver *const *drivers,
				bool one)
{
	ཧྲིལ nodeoff, rc;

	དོན་ལུ (nodeoff = fdt_next_node(fdt, -1, NULL);
	     nodeoff >= 0;
	     nodeoff = fdt_next_node(fdt, nodeoff, NULL)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		གལ་སྲིད (rc == SBI_ENODEV)
			continue;
		གལ་སྲིད (rc < 0)
			ལོག rc;
		གལ་སྲིད (one)
			ལོག 0;
	}

	ལོག one ? SBI_ENODEV : 0;
}

ཧྲིལ fdt_driver_init_all(const void *fdt,
			const sa_nracanaa fdt_driver *const *drivers)
{
	ལོག fdt_driver_init_scan(fdt, drivers, false);
}

ཧྲིལ fdt_driver_init_one(const void *fdt,
			const sa_nracanaa fdt_driver *const *drivers)
{
	ལོག fdt_driver_init_scan(fdt, drivers, true);
}
