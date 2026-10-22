/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 SiFive
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_driver.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_driver_init_by_offset(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
			      ଅଚଳ ସଂରଚନା fdt_driver *ଅଚଳ *drivers)
{
	ଅଚଳ ସଂରଚନା fdt_driver *driver;
	ଅଚଳ ସଂରଚନା fdt_match *match;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା compat_len, prop_len, rc;
	ଅଚଳ ଅକ୍ଷର *compat_str;

	ଯଦି (!fdt_ଗଣ୍ଠି_is_enabled(fdt, nodeoff))
		ଫେରନ୍ତୁ SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	ଯଦି (!compat_str)
		ଫେରନ୍ତୁ SBI_ENODEV;

	ଯେତେବେଳେ ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; (driver = drivers[i]); i++)
			ପାଇଁ (match = driver->match_table; match->compatible; match++)
				ଯଦି (!memcmp(match->compatible, compat_str, compat_len))
					ଯାଅ found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	ଫେରନ୍ତୁ SBI_ENODEV;

found:
	ଯଦି (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	ଯଦି (rc < 0) {
		ଅଚଳ ଅକ୍ଷର *name;

		name = fdt_get_name(fdt, nodeoff, ଖାଲି);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_driver_init_scan(ଅଚଳ ଶୂନ୍ୟ *fdt,
				ଅଚଳ ସଂରଚନା fdt_driver *ଅଚଳ *drivers,
				bool one)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff, rc;

	ପାଇଁ (nodeoff = fdt_next_ଗଣ୍ଠି(fdt, -1, ଖାଲି);
	     nodeoff >= 0;
	     nodeoff = fdt_next_ଗଣ୍ଠି(fdt, nodeoff, ଖାଲି)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		ଯଦି (rc == SBI_ENODEV)
			ଜାରି;
		ଯଦି (rc < 0)
			ଫେରନ୍ତୁ rc;
		ଯଦି (one)
			ଫେରନ୍ତୁ 0;
	}

	ଫେରନ୍ତୁ one ? SBI_ENODEV : 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_driver_init_all(ଅଚଳ ଶୂନ୍ୟ *fdt,
			ଅଚଳ ସଂରଚନା fdt_driver *ଅଚଳ *drivers)
{
	ଫେରନ୍ତୁ fdt_driver_init_scan(fdt, drivers, false);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_driver_init_one(ଅଚଳ ଶୂନ୍ୟ *fdt,
			ଅଚଳ ସଂରଚନା fdt_driver *ଅଚଳ *drivers)
{
	ଫେରନ୍ତୁ fdt_driver_init_scan(fdt, drivers, true);
}
