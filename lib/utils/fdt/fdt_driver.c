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

	ᱡᱩᱫᱤ (!fdt_node_is_enabled(fdt, nodeoff))
		ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	ᱡᱩᱫᱤ (!compat_str)
		ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;

	ᱡᱚᱠᱷᱚᱱ ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		for (puur_naa_nka i = 0; (driver = drivers[i]); i++)
			for (match = driver->match_table; match->compatible; match++)
				ᱡᱩᱫᱤ (!memcmp(match->compatible, compat_str, compat_len))
					goto found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;

found:
	ᱡᱩᱫᱤ (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	ᱡᱩᱫᱤ (rc < 0) {
		const var_na *name;

		name = fdt_get_name(fdt, nodeoff, NULL);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	ᱨᱩᱣᱟᱹᱲ rc;
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
		ᱡᱩᱫᱤ (rc == SBI_ENODEV)
			continue;
		ᱡᱩᱫᱤ (rc < 0)
			ᱨᱩᱣᱟᱹᱲ rc;
		ᱡᱩᱫᱤ (one)
			ᱨᱩᱣᱟᱹᱲ 0;
	}

	ᱨᱩᱣᱟᱹᱲ one ? SBI_ENODEV : 0;
}

puur_naa_nka fdt_driver_init_all(const void *fdt,
			const sa_nracanaa fdt_driver *const *drivers)
{
	ᱨᱩᱣᱟᱹᱲ fdt_driver_init_scan(fdt, drivers, false);
}

puur_naa_nka fdt_driver_init_one(const void *fdt,
			const sa_nracanaa fdt_driver *const *drivers)
{
	ᱨᱩᱣᱟᱹᱲ fdt_driver_init_scan(fdt, drivers, true);
}
