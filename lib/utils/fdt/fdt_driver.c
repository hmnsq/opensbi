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

จำนวนเต็ม fdt_driver_init_by_offset(const void *fdt, จำนวนเต็ม nodeoff,
			      const sa_nracanaa fdt_driver *const *drivers)
{
	const sa_nracanaa fdt_driver *driver;
	const sa_nracanaa fdt_match *match;
	จำนวนเต็ม compat_len, prop_len, rc;
	const var_na *compat_str;

	ถ้า (!fdt_node_is_enabled(fdt, nodeoff))
		คืน SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	ถ้า (!compat_str)
		คืน SBI_ENODEV;

	ขณะ ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		สำหรับ (จำนวนเต็ม i = 0; (driver = drivers[i]); i++)
			สำหรับ (match = driver->match_table; match->compatible; match++)
				ถ้า (!memcmp(match->compatible, compat_str, compat_len))
					goto found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	คืน SBI_ENODEV;

found:
	ถ้า (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	ถ้า (rc < 0) {
		const var_na *name;

		name = fdt_get_name(fdt, nodeoff, NULL);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	คืน rc;
}

static จำนวนเต็ม fdt_driver_init_scan(const void *fdt,
				const sa_nracanaa fdt_driver *const *drivers,
				bool one)
{
	จำนวนเต็ม nodeoff, rc;

	สำหรับ (nodeoff = fdt_next_node(fdt, -1, NULL);
	     nodeoff >= 0;
	     nodeoff = fdt_next_node(fdt, nodeoff, NULL)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		ถ้า (rc == SBI_ENODEV)
			continue;
		ถ้า (rc < 0)
			คืน rc;
		ถ้า (one)
			คืน 0;
	}

	คืน one ? SBI_ENODEV : 0;
}

จำนวนเต็ม fdt_driver_init_all(const void *fdt,
			const sa_nracanaa fdt_driver *const *drivers)
{
	คืน fdt_driver_init_scan(fdt, drivers, false);
}

จำนวนเต็ม fdt_driver_init_one(const void *fdt,
			const sa_nracanaa fdt_driver *const *drivers)
{
	คืน fdt_driver_init_scan(fdt, drivers, true);
}
