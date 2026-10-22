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

පූර්ණ_සංඛ්‍යා fdt_driver_init_by_offset(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoff,
			      const sa_nracanaa fdt_driver *const *drivers)
{
	const sa_nracanaa fdt_driver *driver;
	const sa_nracanaa fdt_match *match;
	පූර්ණ_සංඛ්‍යා compat_len, prop_len, rc;
	const var_na *compat_str;

	නම් (!fdt_node_is_enabled(fdt, nodeoff))
		ආපසු SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	නම් (!compat_str)
		ආපසු SBI_ENODEV;

	අතරතුර ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		සඳහා (පූර්ණ_සංඛ්‍යා i = 0; (driver = drivers[i]); i++)
			සඳහා (match = driver->match_table; match->compatible; match++)
				නම් (!memcmp(match->compatible, compat_str, compat_len))
					goto found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	ආපසු SBI_ENODEV;

found:
	නම් (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	නම් (rc < 0) {
		const var_na *name;

		name = fdt_get_name(fdt, nodeoff, NULL);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	ආපසු rc;
}

static පූර්ණ_සංඛ්‍යා fdt_driver_init_scan(const void *fdt,
				const sa_nracanaa fdt_driver *const *drivers,
				bool one)
{
	පූර්ණ_සංඛ්‍යා nodeoff, rc;

	සඳහා (nodeoff = fdt_next_node(fdt, -1, NULL);
	     nodeoff >= 0;
	     nodeoff = fdt_next_node(fdt, nodeoff, NULL)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		නම් (rc == SBI_ENODEV)
			continue;
		නම් (rc < 0)
			ආපසු rc;
		නම් (one)
			ආපසු 0;
	}

	ආපසු one ? SBI_ENODEV : 0;
}

පූර්ණ_සංඛ්‍යා fdt_driver_init_all(const void *fdt,
			const sa_nracanaa fdt_driver *const *drivers)
{
	ආපසු fdt_driver_init_scan(fdt, drivers, false);
}

පූර්ණ_සංඛ්‍යා fdt_driver_init_one(const void *fdt,
			const sa_nracanaa fdt_driver *const *drivers)
{
	ආපසු fdt_driver_init_scan(fdt, drivers, true);
}
