/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi_utils/cache/fdt_cache.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_driver.h>

/* List of FDT cache drivers generated at compile time */
ബാഹ്യ സ്ഥിരം ഘടന fdt_driver *സ്ഥിരം fdt_cache_drivers[];

പൂർണ്ണസംഖ്യ fdt_cache_add(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ noff, ഘടന cache_device *dev)
{
	പൂർണ്ണസംഖ്യ rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, ശൂന്യ), വലുപ്പം(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_next_cache_get(fdt, noff, &dev->next);
	എങ്കിൽ (rc == SBI_ENOENT)
		dev->next = ശൂന്യ;
	അല്ലെങ്കിൽ എങ്കിൽ (rc)
		തിരികെ rc;

	തിരികെ cache_add(dev);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_cache_add_generic(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ noff)
{
	ഘടന cache_device *dev;
	പൂർണ്ണസംഖ്യ rc;

	dev = sbi_zalloc(വലുപ്പം(*dev));
	എങ്കിൽ (!dev)
		തിരികെ SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	എങ്കിൽ (rc) {
		sbi_free(dev);
		തിരികെ rc;
	}

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_cache_find(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ noff, ഘടന cache_device **out_dev)
{
	ഘടന cache_device *dev = cache_find(noff);
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		എങ്കിൽ (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		എങ്കിൽ (rc)
			തിരികെ rc;

		dev = cache_find(noff);
		എങ്കിൽ (!dev)
			തിരികെ SBI_EFAIL;
	}

	എങ്കിൽ (out_dev)
		*out_dev = dev;

	തിരികെ SBI_OK;
}

പൂർണ്ണസംഖ്യ fdt_next_cache_get(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ noff, ഘടന cache_device **out_dev)
{
	സ്ഥിരം fdt32_t *val;
	പൂർണ്ണസംഖ്യ len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	എങ്കിൽ (!val || len < വലുപ്പം(*val))
		തിരികെ SBI_ENOENT;

	noff = fdt_കണ്ണി_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	എങ്കിൽ (noff < 0)
		തിരികെ noff;

	തിരികെ fdt_cache_find(fdt, noff, out_dev);
}
