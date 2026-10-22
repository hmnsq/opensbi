/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 SiFive
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_driver.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>

முழு fdt_driver_init_by_offset(மாறிலி வெறுமை *fdt, முழு nodeoff,
			      மாறிலி கட்டமைப்பு fdt_driver *மாறிலி *drivers)
{
	மாறிலி கட்டமைப்பு fdt_driver *driver;
	மாறிலி கட்டமைப்பு fdt_match *match;
	முழு compat_len, prop_len, rc;
	மாறிலி எழுத்து *compat_str;

	எனில் (!fdt_முனை_is_enabled(fdt, nodeoff))
		திரும்பு SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	எனில் (!compat_str)
		திரும்பு SBI_ENODEV;

	போது ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		க்கு (முழு i = 0; (driver = drivers[i]); i++)
			க்கு (match = driver->match_table; match->compatible; match++)
				எனில் (!memcmp(match->compatible, compat_str, compat_len))
					செல் found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	திரும்பு SBI_ENODEV;

found:
	எனில் (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	எனில் (rc < 0) {
		மாறிலி எழுத்து *name;

		name = fdt_get_name(fdt, nodeoff, வெற்று);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	திரும்பு rc;
}

நிலையான முழு fdt_driver_init_scan(மாறிலி வெறுமை *fdt,
				மாறிலி கட்டமைப்பு fdt_driver *மாறிலி *drivers,
				bool one)
{
	முழு nodeoff, rc;

	க்கு (nodeoff = fdt_next_முனை(fdt, -1, வெற்று);
	     nodeoff >= 0;
	     nodeoff = fdt_next_முனை(fdt, nodeoff, வெற்று)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		எனில் (rc == SBI_ENODEV)
			தொடர்;
		எனில் (rc < 0)
			திரும்பு rc;
		எனில் (one)
			திரும்பு 0;
	}

	திரும்பு one ? SBI_ENODEV : 0;
}

முழு fdt_driver_init_all(மாறிலி வெறுமை *fdt,
			மாறிலி கட்டமைப்பு fdt_driver *மாறிலி *drivers)
{
	திரும்பு fdt_driver_init_scan(fdt, drivers, false);
}

முழு fdt_driver_init_one(மாறிலி வெறுமை *fdt,
			மாறிலி கட்டமைப்பு fdt_driver *மாறிலி *drivers)
{
	திரும்பு fdt_driver_init_scan(fdt, drivers, true);
}
