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

ຈຳນວນເຕັມ fdt_driver_init_by_offset(const void *fdt, ຈຳນວນເຕັມ nodeoff,
			      const sa_nracanaa fdt_driver *const *drivers)
{
	const sa_nracanaa fdt_driver *driver;
	const sa_nracanaa fdt_match *match;
	ຈຳນວນເຕັມ compat_len, prop_len, rc;
	const var_na *compat_str;

	ຖ້າ (!fdt_node_is_enabled(fdt, nodeoff))
		ສົ່ງຄືນ SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	ຖ້າ (!compat_str)
		ສົ່ງຄືນ SBI_ENODEV;

	ໃນຂະນະທີ່ ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		ສຳລັບ (ຈຳນວນເຕັມ i = 0; (driver = drivers[i]); i++)
			ສຳລັບ (match = driver->match_table; match->compatible; match++)
				ຖ້າ (!memcmp(match->compatible, compat_str, compat_len))
					goto found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	ສົ່ງຄືນ SBI_ENODEV;

found:
	ຖ້າ (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	ຖ້າ (rc < 0) {
		const var_na *name;

		name = fdt_get_name(fdt, nodeoff, NULL);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	ສົ່ງຄືນ rc;
}

static ຈຳນວນເຕັມ fdt_driver_init_scan(const void *fdt,
				const sa_nracanaa fdt_driver *const *drivers,
				bool one)
{
	ຈຳນວນເຕັມ nodeoff, rc;

	ສຳລັບ (nodeoff = fdt_next_node(fdt, -1, NULL);
	     nodeoff >= 0;
	     nodeoff = fdt_next_node(fdt, nodeoff, NULL)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		ຖ້າ (rc == SBI_ENODEV)
			continue;
		ຖ້າ (rc < 0)
			ສົ່ງຄືນ rc;
		ຖ້າ (one)
			ສົ່ງຄືນ 0;
	}

	ສົ່ງຄືນ one ? SBI_ENODEV : 0;
}

ຈຳນວນເຕັມ fdt_driver_init_all(const void *fdt,
			const sa_nracanaa fdt_driver *const *drivers)
{
	ສົ່ງຄືນ fdt_driver_init_scan(fdt, drivers, false);
}

ຈຳນວນເຕັມ fdt_driver_init_one(const void *fdt,
			const sa_nracanaa fdt_driver *const *drivers)
{
	ສົ່ງຄືນ fdt_driver_init_scan(fdt, drivers, true);
}
