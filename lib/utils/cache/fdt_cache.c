/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#समावेश <libfdt.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi_utils/cache/fdt_cache.h>
#समावेश <sbi_utils/fdt/fdt_driver.h>

/* List of FDT cache drivers generated at compile time */
बाह्य स्थिर काष्ठा fdt_driver *स्थिर fdt_cache_drivers[];

पूर्णांक fdt_cache_add(स्थिर व्योम *fdt, पूर्णांक noff, काष्ठा cache_device *dev)
{
	पूर्णांक rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, शून्य), माप(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_अग्र_cache_get(fdt, noff, &dev->अग्र);
	अगर (rc == SBI_ENOENT)
		dev->अग्र = शून्य;
	अन्यथा अगर (rc)
		वापस rc;

	वापस cache_add(dev);
}

अटल पूर्णांक fdt_cache_add_generic(स्थिर व्योम *fdt, पूर्णांक noff)
{
	काष्ठा cache_device *dev;
	पूर्णांक rc;

	dev = sbi_zalloc(माप(*dev));
	अगर (!dev)
		वापस SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	अगर (rc) {
		sbi_free(dev);
		वापस rc;
	}

	वापस 0;
}

अटल पूर्णांक fdt_cache_find(स्थिर व्योम *fdt, पूर्णांक noff, काष्ठा cache_device **out_dev)
{
	काष्ठा cache_device *dev = cache_find(noff);
	पूर्णांक rc;

	अगर (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		अगर (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		अगर (rc)
			वापस rc;

		dev = cache_find(noff);
		अगर (!dev)
			वापस SBI_EFAIL;
	}

	अगर (out_dev)
		*out_dev = dev;

	वापस SBI_OK;
}

पूर्णांक fdt_अग्र_cache_get(स्थिर व्योम *fdt, पूर्णांक noff, काष्ठा cache_device **out_dev)
{
	स्थिर fdt32_t *val;
	पूर्णांक len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	अगर (!val || len < माप(*val))
		वापस SBI_ENOENT;

	noff = fdt_गाँठ_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	अगर (noff < 0)
		वापस noff;

	वापस fdt_cache_find(fdt, noff, out_dev);
}
