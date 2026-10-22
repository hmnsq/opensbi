/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 SiFive
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_driver.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>

પૂર્ણાંક fdt_driver_init_by_offset(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			      અચલ સંરચના fdt_driver *અચલ *drivers)
{
	અચલ સંરચના fdt_driver *driver;
	અચલ સંરચના fdt_match *match;
	પૂર્ણાંક compat_len, prop_len, rc;
	અચલ અક્ષર *compat_str;

	જો (!fdt_ગાંઠ_is_enabled(fdt, nodeoff))
		પાછા SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	જો (!compat_str)
		પાછા SBI_ENODEV;

	જ્યારે ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		માટે (પૂર્ણાંક i = 0; (driver = drivers[i]); i++)
			માટે (match = driver->match_table; match->compatible; match++)
				જો (!memcmp(match->compatible, compat_str, compat_len))
					જાઓ found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	પાછા SBI_ENODEV;

found:
	જો (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	જો (rc < 0) {
		અચલ અક્ષર *name;

		name = fdt_get_name(fdt, nodeoff, ખાલી);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	પાછા rc;
}

સ્થિર પૂર્ણાંક fdt_driver_init_scan(અચલ શૂન્ય *fdt,
				અચલ સંરચના fdt_driver *અચલ *drivers,
				bool one)
{
	પૂર્ણાંક nodeoff, rc;

	માટે (nodeoff = fdt_next_ગાંઠ(fdt, -1, ખાલી);
	     nodeoff >= 0;
	     nodeoff = fdt_next_ગાંઠ(fdt, nodeoff, ખાલી)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		જો (rc == SBI_ENODEV)
			ચાલુ;
		જો (rc < 0)
			પાછા rc;
		જો (one)
			પાછા 0;
	}

	પાછા one ? SBI_ENODEV : 0;
}

પૂર્ણાંક fdt_driver_init_all(અચલ શૂન્ય *fdt,
			અચલ સંરચના fdt_driver *અચલ *drivers)
{
	પાછા fdt_driver_init_scan(fdt, drivers, false);
}

પૂર્ણાંક fdt_driver_init_one(અચલ શૂન્ય *fdt,
			અચલ સંરચના fdt_driver *અચલ *drivers)
{
	પાછા fdt_driver_init_scan(fdt, drivers, true);
}
