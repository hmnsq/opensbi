/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi_utils/cache/fdt_cache.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_driver.h>

/* List of FDT cache drivers generated at compile time */
வெளி மாறிலி கட்டமைப்பு fdt_driver *மாறிலி fdt_cache_drivers[];

முழு fdt_cache_add(மாறிலி வெறுமை *fdt, முழு noff, கட்டமைப்பு cache_device *dev)
{
	முழு rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, வெற்று), அளவு(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_next_cache_get(fdt, noff, &dev->next);
	எனில் (rc == SBI_ENOENT)
		dev->next = வெற்று;
	இல்லையெனில் எனில் (rc)
		திரும்பு rc;

	திரும்பு cache_add(dev);
}

நிலையான முழு fdt_cache_add_generic(மாறிலி வெறுமை *fdt, முழு noff)
{
	கட்டமைப்பு cache_device *dev;
	முழு rc;

	dev = sbi_zalloc(அளவு(*dev));
	எனில் (!dev)
		திரும்பு SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	எனில் (rc) {
		sbi_free(dev);
		திரும்பு rc;
	}

	திரும்பு 0;
}

நிலையான முழு fdt_cache_find(மாறிலி வெறுமை *fdt, முழு noff, கட்டமைப்பு cache_device **out_dev)
{
	கட்டமைப்பு cache_device *dev = cache_find(noff);
	முழு rc;

	எனில் (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		எனில் (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		எனில் (rc)
			திரும்பு rc;

		dev = cache_find(noff);
		எனில் (!dev)
			திரும்பு SBI_EFAIL;
	}

	எனில் (out_dev)
		*out_dev = dev;

	திரும்பு SBI_OK;
}

முழு fdt_next_cache_get(மாறிலி வெறுமை *fdt, முழு noff, கட்டமைப்பு cache_device **out_dev)
{
	மாறிலி fdt32_t *val;
	முழு len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	எனில் (!val || len < அளவு(*val))
		திரும்பு SBI_ENOENT;

	noff = fdt_முனை_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	எனில் (noff < 0)
		திரும்பு noff;

	திரும்பு fdt_cache_find(fdt, noff, out_dev);
}
