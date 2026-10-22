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
बाह्य ध्रुव संरचना fdt_driver *ध्रुव fdt_cache_drivers[];

पूर्णाङ्क fdt_cache_सङ्कलय(ध्रुव शून्य *fdt, पूर्णाङ्क noff, संरचना cache_device *dev)
{
	पूर्णाङ्क rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, रिक्त), परिमाण(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_अग्र_cache_get(fdt, noff, &dev->अग्र);
	यदि (rc == SBI_ENOENT)
		dev->अग्र = रिक्त;
	अन्यथा यदि (rc)
		प्रत्यागम rc;

	प्रत्यागम cache_सङ्कलय(dev);
}

स्थिर पूर्णाङ्क fdt_cache_सङ्कलय_generic(ध्रुव शून्य *fdt, पूर्णाङ्क noff)
{
	संरचना cache_device *dev;
	पूर्णाङ्क rc;

	dev = sbi_zalloc(परिमाण(*dev));
	यदि (!dev)
		प्रत्यागम SBI_ENOMEM;

	rc = fdt_cache_सङ्कलय(fdt, noff, dev);
	यदि (rc) {
		sbi_free(dev);
		प्रत्यागम rc;
	}

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क fdt_cache_find(ध्रुव शून्य *fdt, पूर्णाङ्क noff, संरचना cache_device **बहिर्गमनम्_dev)
{
	संरचना cache_device *dev = cache_find(noff);
	पूर्णाङ्क rc;

	यदि (!dev) {
		rc = fdt_driver_init_by_विस्थापनम्(fdt, noff, fdt_cache_drivers);
		यदि (rc == SBI_ENODEV)
			rc = fdt_cache_सङ्कलय_generic(fdt, noff);
		यदि (rc)
			प्रत्यागम rc;

		dev = cache_find(noff);
		यदि (!dev)
			प्रत्यागम SBI_EFAIL;
	}

	यदि (बहिर्गमनम्_dev)
		*बहिर्गमनम्_dev = dev;

	प्रत्यागम SBI_OK;
}

पूर्णाङ्क fdt_अग्र_cache_get(ध्रुव शून्य *fdt, पूर्णाङ्क noff, संरचना cache_device **बहिर्गमनम्_dev)
{
	ध्रुव fdt32_t *val;
	पूर्णाङ्क len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	यदि (!val || len < परिमाण(*val))
		प्रत्यागम SBI_ENOENT;

	noff = fdt_ग्रन्थिः_विस्थापनम्_by_phandle(fdt, fdt32_गन्तव्यम्_cpu(val[0]));
	यदि (noff < 0)
		प्रत्यागम noff;

	प्रत्यागम fdt_cache_find(fdt, noff, बहिर्गमनम्_dev);
}
