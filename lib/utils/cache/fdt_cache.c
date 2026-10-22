/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/cache/fdt_cache.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_driver.h>

/* List of FDT cache drivers generated at compile time */
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ fdt_driver *ਅਚਲ fdt_cache_drivers[];

ਪੂਰਨ fdt_cache_ਜੋੜੋ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ noff, ਬਣਤਰ cache_device *dev)
{
	ਪੂਰਨ rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, ਨਿਰਾ), ਆਕਾਰ(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_ਅਗਲਾ_cache_get(fdt, noff, &dev->ਅਗਲਾ);
	ਜੇ (rc == SBI_ENOENT)
		dev->ਅਗਲਾ = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ ਜੇ (rc)
		ਵਾਪਸ rc;

	ਵਾਪਸ cache_ਜੋੜੋ(dev);
}

ਸਥਿਰ ਪੂਰਨ fdt_cache_ਜੋੜੋ_generic(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ noff)
{
	ਬਣਤਰ cache_device *dev;
	ਪੂਰਨ rc;

	dev = sbi_zalloc(ਆਕਾਰ(*dev));
	ਜੇ (!dev)
		ਵਾਪਸ SBI_ENOMEM;

	rc = fdt_cache_ਜੋੜੋ(fdt, noff, dev);
	ਜੇ (rc) {
		sbi_free(dev);
		ਵਾਪਸ rc;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ fdt_cache_ਲੱਭੋ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ noff, ਬਣਤਰ cache_device **ਬਾਹਰਲਾ_dev)
{
	ਬਣਤਰ cache_device *dev = cache_ਲੱਭੋ(noff);
	ਪੂਰਨ rc;

	ਜੇ (!dev) {
		rc = fdt_driver_init_by_ਵਿੱਥ(fdt, noff, fdt_cache_drivers);
		ਜੇ (rc == SBI_ENODEV)
			rc = fdt_cache_ਜੋੜੋ_generic(fdt, noff);
		ਜੇ (rc)
			ਵਾਪਸ rc;

		dev = cache_ਲੱਭੋ(noff);
		ਜੇ (!dev)
			ਵਾਪਸ SBI_EFAIL;
	}

	ਜੇ (ਬਾਹਰਲਾ_dev)
		*ਬਾਹਰਲਾ_dev = dev;

	ਵਾਪਸ SBI_OK;
}

ਪੂਰਨ fdt_ਅਗਲਾ_cache_get(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ noff, ਬਣਤਰ cache_device **ਬਾਹਰਲਾ_dev)
{
	ਅਚਲ fdt32_t *val;
	ਪੂਰਨ len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	ਜੇ (!val || len < ਆਕਾਰ(*val))
		ਵਾਪਸ SBI_ENOENT;

	noff = fdt_ਗੰਢ_ਵਿੱਥ_by_phandle(fdt, fdt32_ਮੰਜ਼ਿਲ_cpu(val[0]));
	ਜੇ (noff < 0)
		ਵਾਪਸ noff;

	ਵਾਪਸ fdt_cache_ਲੱਭੋ(fdt, noff, ਬਾਹਰਲਾ_dev);
}
