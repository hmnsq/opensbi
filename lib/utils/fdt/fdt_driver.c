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

puur_naa_nka fdt_driver_init_by_offset(const void *fdt, puur_naa_nka nodeoff,
			      const sa_nracanaa fdt_driver *const *drivers)
{
	const sa_nracanaa fdt_driver *driver;
	const sa_nracanaa fdt_match *match;
	puur_naa_nka compat_len, prop_len, rc;
	const var_na *compat_str;

	ᰀᰧ (!fdt_node_is_enabled(fdt, nodeoff))
		ᰀᰧᰌ SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	ᰀᰧ (!compat_str)
		ᰀᰧᰌ SBI_ENODEV;

	ᰀᰬ ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		for (puur_naa_nka i = 0; (driver = drivers[i]); i++)
			for (match = driver->match_table; match->compatible; match++)
				ᰀᰧ (!memcmp(match->compatible, compat_str, compat_len))
					goto found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	ᰀᰧᰌ SBI_ENODEV;

found:
	ᰀᰧ (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	ᰀᰧ (rc < 0) {
		const var_na *name;

		name = fdt_get_name(fdt, nodeoff, NULL);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	ᰀᰧᰌ rc;
}

static puur_naa_nka fdt_driver_init_scan(const void *fdt,
				const sa_nracanaa fdt_driver *const *drivers,
				bool one)
{
	puur_naa_nka nodeoff, rc;

	for (nodeoff = fdt_next_node(fdt, -1, NULL);
	     nodeoff >= 0;
	     nodeoff = fdt_next_node(fdt, nodeoff, NULL)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		ᰀᰧ (rc == SBI_ENODEV)
			continue;
		ᰀᰧ (rc < 0)
			ᰀᰧᰌ rc;
		ᰀᰧ (one)
			ᰀᰧᰌ 0;
	}

	ᰀᰧᰌ one ? SBI_ENODEV : 0;
}

puur_naa_nka fdt_driver_init_all(const void *fdt,
			const sa_nracanaa fdt_driver *const *drivers)
{
	ᰀᰧᰌ fdt_driver_init_scan(fdt, drivers, false);
}

puur_naa_nka fdt_driver_init_one(const void *fdt,
			const sa_nracanaa fdt_driver *const *drivers)
{
	ᰀᰧᰌ fdt_driver_init_scan(fdt, drivers, true);
}
