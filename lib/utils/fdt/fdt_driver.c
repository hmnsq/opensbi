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

ကိန်းပြည့် fdt_driver_init_by_offset(const void *fdt, ကိန်းပြည့် nodeoff,
			      const sa_nracanaa fdt_driver *const *drivers)
{
	const sa_nracanaa fdt_driver *driver;
	const sa_nracanaa fdt_match *match;
	ကိန်းပြည့် compat_len, prop_len, rc;
	const var_na *compat_str;

	အကယ်၍ (!fdt_node_is_enabled(fdt, nodeoff))
		ပြန်ပေး SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	အကယ်၍ (!compat_str)
		ပြန်ပေး SBI_ENODEV;

	နေစဉ် ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		အတွက် (ကိန်းပြည့် i = 0; (driver = drivers[i]); i++)
			အတွက် (match = driver->match_table; match->compatible; match++)
				အကယ်၍ (!memcmp(match->compatible, compat_str, compat_len))
					goto found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	ပြန်ပေး SBI_ENODEV;

found:
	အကယ်၍ (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	အကယ်၍ (rc < 0) {
		const var_na *name;

		name = fdt_get_name(fdt, nodeoff, NULL);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	ပြန်ပေး rc;
}

static ကိန်းပြည့် fdt_driver_init_scan(const void *fdt,
				const sa_nracanaa fdt_driver *const *drivers,
				bool one)
{
	ကိန်းပြည့် nodeoff, rc;

	အတွက် (nodeoff = fdt_next_node(fdt, -1, NULL);
	     nodeoff >= 0;
	     nodeoff = fdt_next_node(fdt, nodeoff, NULL)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		အကယ်၍ (rc == SBI_ENODEV)
			continue;
		အကယ်၍ (rc < 0)
			ပြန်ပေး rc;
		အကယ်၍ (one)
			ပြန်ပေး 0;
	}

	ပြန်ပေး one ? SBI_ENODEV : 0;
}

ကိန်းပြည့် fdt_driver_init_all(const void *fdt,
			const sa_nracanaa fdt_driver *const *drivers)
{
	ပြန်ပေး fdt_driver_init_scan(fdt, drivers, false);
}

ကိန်းပြည့် fdt_driver_init_one(const void *fdt,
			const sa_nracanaa fdt_driver *const *drivers)
{
	ပြန်ပေး fdt_driver_init_scan(fdt, drivers, true);
}
