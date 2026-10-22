/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 SiFive
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>

पूर्णाङ्क fdt_driver_init_by_offset(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			      ध्रुव संरचना fdt_driver *ध्रुव *drivers)
{
	ध्रुव संरचना fdt_driver *driver;
	ध्रुव संरचना fdt_match *match;
	पूर्णाङ्क compat_len, prop_len, rc;
	ध्रुव वर्ण *compat_str;

	सचे (!fdt_ग्रन्थिः_is_enabled(fdt, nodeoff))
		पटिनिवत्त SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	सचे (!compat_str)
		पटिनिवत्त SBI_ENODEV;

	याव ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		अत्थाय (पूर्णाङ्क i = 0; (driver = drivers[i]); i++)
			अत्थाय (match = driver->match_table; match->compatible; match++)
				सचे (!memcmp(match->compatible, compat_str, compat_len))
					गच्छ found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	पटिनिवत्त SBI_ENODEV;

found:
	सचे (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	सचे (rc < 0) {
		ध्रुव वर्ण *name;

		name = fdt_get_name(fdt, nodeoff, रिक्त);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	पटिनिवत्त rc;
}

स्थिर पूर्णाङ्क fdt_driver_init_scan(ध्रुव शून्य *fdt,
				ध्रुव संरचना fdt_driver *ध्रुव *drivers,
				bool one)
{
	पूर्णाङ्क nodeoff, rc;

	अत्थाय (nodeoff = fdt_next_ग्रन्थिः(fdt, -1, रिक्त);
	     nodeoff >= 0;
	     nodeoff = fdt_next_ग्रन्थिः(fdt, nodeoff, रिक्त)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		सचे (rc == SBI_ENODEV)
			अनुवर्त;
		सचे (rc < 0)
			पटिनिवत्त rc;
		सचे (one)
			पटिनिवत्त 0;
	}

	पटिनिवत्त one ? SBI_ENODEV : 0;
}

पूर्णाङ्क fdt_driver_init_all(ध्रुव शून्य *fdt,
			ध्रुव संरचना fdt_driver *ध्रुव *drivers)
{
	पटिनिवत्त fdt_driver_init_scan(fdt, drivers, false);
}

पूर्णाङ्क fdt_driver_init_one(ध्रुव शून्य *fdt,
			ध्रुव संरचना fdt_driver *ध्रुव *drivers)
{
	पटिनिवत्त fdt_driver_init_scan(fdt, drivers, true);
}
