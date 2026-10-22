/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 SiFive
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_driver.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>

ਪੂਰਨ fdt_driver_init_by_ਵਿੱਥ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			      ਅਚਲ ਬਣਤਰ fdt_driver *ਅਚਲ *drivers)
{
	ਅਚਲ ਬਣਤਰ fdt_driver *driver;
	ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ;
	ਪੂਰਨ compat_len, prop_len, rc;
	ਅਚਲ ਅੱਖਰ *compat_str;

	ਜੇ (!fdt_ਗੰਢ_is_enabled(fdt, nodeoff))
		ਵਾਪਸ SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	ਜੇ (!compat_str)
		ਵਾਪਸ SBI_ENODEV;

	ਜਦੋਂ ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		ਲਈ (ਪੂਰਨ i = 0; (driver = drivers[i]); i++)
			ਲਈ (ਮੇਲ = driver->ਮੇਲ_ਸਾਰਣੀ; ਮੇਲ->compatible; ਮੇਲ++)
				ਜੇ (!memcmp(ਮੇਲ->compatible, compat_str, compat_len))
					ਜਾਓ ਮਿਲਿਆ;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	ਵਾਪਸ SBI_ENODEV;

ਮਿਲਿਆ:
	ਜੇ (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   ਮੇਲ->compatible);

	rc = driver->init(fdt, nodeoff, ਮੇਲ);
	ਜੇ (rc < 0) {
		ਅਚਲ ਅੱਖਰ *name;

		name = fdt_get_name(fdt, nodeoff, ਨਿਰਾ);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, ਮੇਲ->compatible, rc);
	}

	ਵਾਪਸ rc;
}

ਸਥਿਰ ਪੂਰਨ fdt_driver_init_ਜਾਂਚ(ਅਚਲ ਖਾਲੀ *fdt,
				ਅਚਲ ਬਣਤਰ fdt_driver *ਅਚਲ *drivers,
				bool one)
{
	ਪੂਰਨ nodeoff, rc;

	ਲਈ (nodeoff = fdt_ਅਗਲਾ_ਗੰਢ(fdt, -1, ਨਿਰਾ);
	     nodeoff >= 0;
	     nodeoff = fdt_ਅਗਲਾ_ਗੰਢ(fdt, nodeoff, ਨਿਰਾ)) {
		rc = fdt_driver_init_by_ਵਿੱਥ(fdt, nodeoff, drivers);
		ਜੇ (rc == SBI_ENODEV)
			ਜਾਰੀ;
		ਜੇ (rc < 0)
			ਵਾਪਸ rc;
		ਜੇ (one)
			ਵਾਪਸ 0;
	}

	ਵਾਪਸ one ? SBI_ENODEV : 0;
}

ਪੂਰਨ fdt_driver_init_all(ਅਚਲ ਖਾਲੀ *fdt,
			ਅਚਲ ਬਣਤਰ fdt_driver *ਅਚਲ *drivers)
{
	ਵਾਪਸ fdt_driver_init_ਜਾਂਚ(fdt, drivers, false);
}

ਪੂਰਨ fdt_driver_init_one(ਅਚਲ ਖਾਲੀ *fdt,
			ਅਚਲ ਬਣਤਰ fdt_driver *ਅਚਲ *drivers)
{
	ਵਾਪਸ fdt_driver_init_ਜਾਂਚ(fdt, drivers, true);
}
