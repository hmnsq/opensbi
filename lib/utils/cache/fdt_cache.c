/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi_utils/cache/fdt_cache.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_driver.h>

/* List of FDT cache drivers generated at compile time */
বাহ্যিক ধ্রুবক কাঠামো fdt_driver *ধ্রুবক fdt_cache_drivers[];

পূর্ণসংখ্যা fdt_cache_add(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা noff, কাঠামো cache_device *dev)
{
	পূর্ণসংখ্যা rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, ফাঁকা), আকার(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_পরবর্তী_cache_get(fdt, noff, &dev->পরবর্তী);
	যদি (rc == SBI_ENOENT)
		dev->পরবর্তী = ফাঁকা;
	অন্যথা যদি (rc)
		ফেরত rc;

	ফেরত cache_add(dev);
}

স্থির পূর্ণসংখ্যা fdt_cache_add_generic(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা noff)
{
	কাঠামো cache_device *dev;
	পূর্ণসংখ্যা rc;

	dev = sbi_zalloc(আকার(*dev));
	যদি (!dev)
		ফেরত SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	যদি (rc) {
		sbi_free(dev);
		ফেরত rc;
	}

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা fdt_cache_find(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা noff, কাঠামো cache_device **out_dev)
{
	কাঠামো cache_device *dev = cache_find(noff);
	পূর্ণসংখ্যা rc;

	যদি (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		যদি (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		যদি (rc)
			ফেরত rc;

		dev = cache_find(noff);
		যদি (!dev)
			ফেরত SBI_EFAIL;
	}

	যদি (out_dev)
		*out_dev = dev;

	ফেরত SBI_OK;
}

পূর্ণসংখ্যা fdt_পরবর্তী_cache_get(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা noff, কাঠামো cache_device **out_dev)
{
	ধ্রুবক fdt32_t *val;
	পূর্ণসংখ্যা len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	যদি (!val || len < আকার(*val))
		ফেরত SBI_ENOENT;

	noff = fdt_গ্রন্থি_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	যদি (noff < 0)
		ফেরত noff;

	ফেরত fdt_cache_find(fdt, noff, out_dev);
}
