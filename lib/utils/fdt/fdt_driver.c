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

ចំនួនគត់ fdt_driver_init_by_offset(const void *fdt, ចំនួនគត់ nodeoff,
			      const sa_nracanaa fdt_driver *const *drivers)
{
	const sa_nracanaa fdt_driver *driver;
	const sa_nracanaa fdt_match *match;
	ចំនួនគត់ compat_len, prop_len, rc;
	const var_na *compat_str;

	ប្រសិនបើ (!fdt_node_is_enabled(fdt, nodeoff))
		ត្រឡប់ SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	ប្រសិនបើ (!compat_str)
		ត្រឡប់ SBI_ENODEV;

	ខណៈពេល ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		សម្រាប់ (ចំនួនគត់ i = 0; (driver = drivers[i]); i++)
			សម្រាប់ (match = driver->match_table; match->compatible; match++)
				ប្រសិនបើ (!memcmp(match->compatible, compat_str, compat_len))
					goto found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	ត្រឡប់ SBI_ENODEV;

found:
	ប្រសិនបើ (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	ប្រសិនបើ (rc < 0) {
		const var_na *name;

		name = fdt_get_name(fdt, nodeoff, NULL);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	ត្រឡប់ rc;
}

static ចំនួនគត់ fdt_driver_init_scan(const void *fdt,
				const sa_nracanaa fdt_driver *const *drivers,
				bool one)
{
	ចំនួនគត់ nodeoff, rc;

	សម្រាប់ (nodeoff = fdt_next_node(fdt, -1, NULL);
	     nodeoff >= 0;
	     nodeoff = fdt_next_node(fdt, nodeoff, NULL)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		ប្រសិនបើ (rc == SBI_ENODEV)
			continue;
		ប្រសិនបើ (rc < 0)
			ត្រឡប់ rc;
		ប្រសិនបើ (one)
			ត្រឡប់ 0;
	}

	ត្រឡប់ one ? SBI_ENODEV : 0;
}

ចំនួនគត់ fdt_driver_init_all(const void *fdt,
			const sa_nracanaa fdt_driver *const *drivers)
{
	ត្រឡប់ fdt_driver_init_scan(fdt, drivers, false);
}

ចំនួនគត់ fdt_driver_init_one(const void *fdt,
			const sa_nracanaa fdt_driver *const *drivers)
{
	ត្រឡប់ fdt_driver_init_scan(fdt, drivers, true);
}
