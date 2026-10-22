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

पूर्णांक fdt_driver_init_by_offset(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
			      स्थिरांक संरचना fdt_driver *स्थिरांक *drivers)
{
	स्थिरांक संरचना fdt_driver *driver;
	स्थिरांक संरचना fdt_match *match;
	पूर्णांक compat_len, prop_len, rc;
	स्थिरांक अक्षर *compat_str;

	जर (!fdt_गाठ_is_enabled(fdt, nodeoff))
		परत SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	जर (!compat_str)
		परत SBI_ENODEV;

	पर्यंत ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		साठी (पूर्णांक i = 0; (driver = drivers[i]); i++)
			साठी (match = driver->match_table; match->compatible; match++)
				जर (!स_भेद(match->compatible, compat_str, compat_len))
					जा found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	परत SBI_ENODEV;

found:
	जर (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	जर (rc < 0) {
		स्थिरांक अक्षर *name;

		name = fdt_get_name(fdt, nodeoff, रिक्त);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	परत rc;
}

स्थिर पूर्णांक fdt_driver_init_scan(स्थिरांक शून्य *fdt,
				स्थिरांक संरचना fdt_driver *स्थिरांक *drivers,
				bool one)
{
	पूर्णांक nodeoff, rc;

	साठी (nodeoff = fdt_next_गाठ(fdt, -1, रिक्त);
	     nodeoff >= 0;
	     nodeoff = fdt_next_गाठ(fdt, nodeoff, रिक्त)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		जर (rc == SBI_ENODEV)
			सुरूठेवा;
		जर (rc < 0)
			परत rc;
		जर (one)
			परत 0;
	}

	परत one ? SBI_ENODEV : 0;
}

पूर्णांक fdt_driver_init_all(स्थिरांक शून्य *fdt,
			स्थिरांक संरचना fdt_driver *स्थिरांक *drivers)
{
	परत fdt_driver_init_scan(fdt, drivers, false);
}

पूर्णांक fdt_driver_init_one(स्थिरांक शून्य *fdt,
			स्थिरांक संरचना fdt_driver *स्थिरांक *drivers)
{
	परत fdt_driver_init_scan(fdt, drivers, true);
}
