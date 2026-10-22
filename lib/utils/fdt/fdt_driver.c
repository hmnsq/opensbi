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

	यदि (!fdt_गाँठ_is_enabled(fdt, nodeoff))
		प्रत्यागम SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	यदि (!compat_str)
		प्रत्यागम SBI_ENODEV;

	यावत् ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		प्रति (पूर्णाङ्क i = 0; (driver = drivers[i]); i++)
			प्रति (match = driver->match_table; match->compatible; match++)
				यदि (!स_भेद(match->compatible, compat_str, compat_len))
					गच्छ found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	प्रत्यागम SBI_ENODEV;

found:
	यदि (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	यदि (rc < 0) {
		ध्रुव वर्ण *name;

		name = fdt_get_name(fdt, nodeoff, रिक्त);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	प्रत्यागम rc;
}

स्थिर पूर्णाङ्क fdt_driver_init_scan(ध्रुव शून्य *fdt,
				ध्रुव संरचना fdt_driver *ध्रुव *drivers,
				bool one)
{
	पूर्णाङ्क nodeoff, rc;

	प्रति (nodeoff = fdt_next_गाँठ(fdt, -1, रिक्त);
	     nodeoff >= 0;
	     nodeoff = fdt_next_गाँठ(fdt, nodeoff, रिक्त)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		यदि (rc == SBI_ENODEV)
			अनुवर्त;
		यदि (rc < 0)
			प्रत्यागम rc;
		यदि (one)
			प्रत्यागम 0;
	}

	प्रत्यागम one ? SBI_ENODEV : 0;
}

पूर्णाङ्क fdt_driver_init_all(ध्रुव शून्य *fdt,
			ध्रुव संरचना fdt_driver *ध्रुव *drivers)
{
	प्रत्यागम fdt_driver_init_scan(fdt, drivers, false);
}

पूर्णाङ्क fdt_driver_init_one(ध्रुव शून्य *fdt,
			ध्रुव संरचना fdt_driver *ध्रुव *drivers)
{
	प्रत्यागम fdt_driver_init_scan(fdt, drivers, true);
}
