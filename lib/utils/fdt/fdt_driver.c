/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 SiFive
 */

#समावेश <libfdt.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi_utils/fdt/fdt_driver.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>

पूर्णांक fdt_driver_init_by_offset(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			      स्थिर काष्ठा fdt_driver *स्थिर *drivers)
{
	स्थिर काष्ठा fdt_driver *driver;
	स्थिर काष्ठा fdt_match *match;
	पूर्णांक compat_len, prop_len, rc;
	स्थिर अक्षर *compat_str;

	अगर (!fdt_गाँठ_is_enabled(fdt, nodeoff))
		वापस SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	अगर (!compat_str)
		वापस SBI_ENODEV;

	जबतक ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		क्रम (पूर्णांक i = 0; (driver = drivers[i]); i++)
			क्रम (match = driver->match_table; match->compatible; match++)
				अगर (!स_भेद(match->compatible, compat_str, compat_len))
					जाओ found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	वापस SBI_ENODEV;

found:
	अगर (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	अगर (rc < 0) {
		स्थिर अक्षर *name;

		name = fdt_get_name(fdt, nodeoff, शून्य);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	वापस rc;
}

अटल पूर्णांक fdt_driver_init_scan(स्थिर व्योम *fdt,
				स्थिर काष्ठा fdt_driver *स्थिर *drivers,
				bool one)
{
	पूर्णांक nodeoff, rc;

	क्रम (nodeoff = fdt_अग्र_गाँठ(fdt, -1, शून्य);
	     nodeoff >= 0;
	     nodeoff = fdt_अग्र_गाँठ(fdt, nodeoff, शून्य)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		अगर (rc == SBI_ENODEV)
			जारी;
		अगर (rc < 0)
			वापस rc;
		अगर (one)
			वापस 0;
	}

	वापस one ? SBI_ENODEV : 0;
}

पूर्णांक fdt_driver_init_all(स्थिर व्योम *fdt,
			स्थिर काष्ठा fdt_driver *स्थिर *drivers)
{
	वापस fdt_driver_init_scan(fdt, drivers, false);
}

पूर्णांक fdt_driver_init_one(स्थिर व्योम *fdt,
			स्थिर काष्ठा fdt_driver *स्थिर *drivers)
{
	वापस fdt_driver_init_scan(fdt, drivers, true);
}
