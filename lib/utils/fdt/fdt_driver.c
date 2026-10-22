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

पूर्णाङ्क fdt_driver_init_by_offset(अचर शून्य *fdt, पूर्णाङ्क nodeoff,
			      अचर संरचना fdt_driver *अचर *drivers)
{
	अचर संरचना fdt_driver *driver;
	अचर संरचना fdt_match *match;
	पूर्णाङ्क compat_len, prop_len, rc;
	अचर वर्ण *compat_str;

	यदि (!fdt_गाँठो_is_enabled(fdt, nodeoff))
		फर्काऊ SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	यदि (!compat_str)
		फर्काऊ SBI_ENODEV;

	जबसम्म ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		लागि (पूर्णाङ्क i = 0; (driver = drivers[i]); i++)
			लागि (match = driver->match_table; match->compatible; match++)
				यदि (!स_भेद(match->compatible, compat_str, compat_len))
					गच्छ found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	फर्काऊ SBI_ENODEV;

found:
	यदि (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	यदि (rc < 0) {
		अचर वर्ण *name;

		name = fdt_get_name(fdt, nodeoff, रिक्त);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	फर्काऊ rc;
}

स्थिर पूर्णाङ्क fdt_driver_init_scan(अचर शून्य *fdt,
				अचर संरचना fdt_driver *अचर *drivers,
				bool one)
{
	पूर्णाङ्क nodeoff, rc;

	लागि (nodeoff = fdt_next_गाँठो(fdt, -1, रिक्त);
	     nodeoff >= 0;
	     nodeoff = fdt_next_गाँठो(fdt, nodeoff, रिक्त)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		यदि (rc == SBI_ENODEV)
			जारी;
		यदि (rc < 0)
			फर्काऊ rc;
		यदि (one)
			फर्काऊ 0;
	}

	फर्काऊ one ? SBI_ENODEV : 0;
}

पूर्णाङ्क fdt_driver_init_all(अचर शून्य *fdt,
			अचर संरचना fdt_driver *अचर *drivers)
{
	फर्काऊ fdt_driver_init_scan(fdt, drivers, false);
}

पूर्णाङ्क fdt_driver_init_one(अचर शून्य *fdt,
			अचर संरचना fdt_driver *अचर *drivers)
{
	फर्काऊ fdt_driver_init_scan(fdt, drivers, true);
}
