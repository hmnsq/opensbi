/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 SiFive
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_driver.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>

പൂർണ്ണസംഖ്യ fdt_driver_init_by_offset(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			      സ്ഥിരം ഘടന fdt_driver *സ്ഥിരം *drivers)
{
	സ്ഥിരം ഘടന fdt_driver *driver;
	സ്ഥിരം ഘടന fdt_match *match;
	പൂർണ്ണസംഖ്യ compat_len, prop_len, rc;
	സ്ഥിരം അക്ഷരം *compat_str;

	എങ്കിൽ (!fdt_കണ്ണി_is_enabled(fdt, nodeoff))
		തിരികെ SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	എങ്കിൽ (!compat_str)
		തിരികെ SBI_ENODEV;

	ആകുമ്പോൾ ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; (driver = drivers[i]); i++)
			വേണ്ടി (match = driver->match_table; match->compatible; match++)
				എങ്കിൽ (!memcmp(match->compatible, compat_str, compat_len))
					പോകുക found;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	തിരികെ SBI_ENODEV;

found:
	എങ്കിൽ (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   match->compatible);

	rc = driver->init(fdt, nodeoff, match);
	എങ്കിൽ (rc < 0) {
		സ്ഥിരം അക്ഷരം *name;

		name = fdt_get_name(fdt, nodeoff, ശൂന്യ);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, match->compatible, rc);
	}

	തിരികെ rc;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_driver_init_scan(സ്ഥിരം ശൂന്യം *fdt,
				സ്ഥിരം ഘടന fdt_driver *സ്ഥിരം *drivers,
				bool one)
{
	പൂർണ്ണസംഖ്യ nodeoff, rc;

	വേണ്ടി (nodeoff = fdt_next_കണ്ണി(fdt, -1, ശൂന്യ);
	     nodeoff >= 0;
	     nodeoff = fdt_next_കണ്ണി(fdt, nodeoff, ശൂന്യ)) {
		rc = fdt_driver_init_by_offset(fdt, nodeoff, drivers);
		എങ്കിൽ (rc == SBI_ENODEV)
			തുടരുക;
		എങ്കിൽ (rc < 0)
			തിരികെ rc;
		എങ്കിൽ (one)
			തിരികെ 0;
	}

	തിരികെ one ? SBI_ENODEV : 0;
}

പൂർണ്ണസംഖ്യ fdt_driver_init_all(സ്ഥിരം ശൂന്യം *fdt,
			സ്ഥിരം ഘടന fdt_driver *സ്ഥിരം *drivers)
{
	തിരികെ fdt_driver_init_scan(fdt, drivers, false);
}

പൂർണ്ണസംഖ്യ fdt_driver_init_one(സ്ഥിരം ശൂന്യം *fdt,
			സ്ഥിരം ഘടന fdt_driver *സ്ഥിരം *drivers)
{
	തിരികെ fdt_driver_init_scan(fdt, drivers, true);
}
