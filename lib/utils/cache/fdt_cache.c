/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi_utils/cache/fdt_cache.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>

/* List of FDT cache drivers generated at compile time */
बाह्य स्थिरांक संरचना fdt_driver *स्थिरांक fdt_cache_drivers[];

पूर्णांक fdt_cache_add(स्थिरांक शून्य *fdt, पूर्णांक noff, संरचना cache_device *dev)
{
	पूर्णांक rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, रिक्त), आकार(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_next_cache_get(fdt, noff, &dev->next);
	जर (rc == SBI_ENOENT)
		dev->next = रिक्त;
	अन्यथा जर (rc)
		परत rc;

	परत cache_add(dev);
}

स्थिर पूर्णांक fdt_cache_add_generic(स्थिरांक शून्य *fdt, पूर्णांक noff)
{
	संरचना cache_device *dev;
	पूर्णांक rc;

	dev = sbi_zalloc(आकार(*dev));
	जर (!dev)
		परत SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	जर (rc) {
		sbi_free(dev);
		परत rc;
	}

	परत 0;
}

स्थिर पूर्णांक fdt_cache_find(स्थिरांक शून्य *fdt, पूर्णांक noff, संरचना cache_device **out_dev)
{
	संरचना cache_device *dev = cache_find(noff);
	पूर्णांक rc;

	जर (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		जर (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		जर (rc)
			परत rc;

		dev = cache_find(noff);
		जर (!dev)
			परत SBI_EFAIL;
	}

	जर (out_dev)
		*out_dev = dev;

	परत SBI_OK;
}

पूर्णांक fdt_next_cache_get(स्थिरांक शून्य *fdt, पूर्णांक noff, संरचना cache_device **out_dev)
{
	स्थिरांक fdt32_t *val;
	पूर्णांक len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	जर (!val || len < आकार(*val))
		परत SBI_ENOENT;

	noff = fdt_गाठ_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	जर (noff < 0)
		परत noff;

	परत fdt_cache_find(fdt, noff, out_dev);
}
