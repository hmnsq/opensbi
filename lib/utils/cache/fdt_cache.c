/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#شامل <libfdt.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi_utils/cache/fdt_cache.h>
#شامل <sbi_utils/fdt/fdt_driver.h>

/* List of FDT cache drivers generated at compile time */
بیرونی ثابت ساختار fdt_driver *ثابت fdt_cache_drivers[];

صحیح fdt_cache_add(ثابت تهی *fdt, صحیح noff, ساختار cache_device *dev)
{
	صحیح rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, پوچ), اندازه(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_next_cache_get(fdt, noff, &dev->next);
	اگر (rc == SBI_ENOENT)
		dev->next = پوچ;
	وگرنه اگر (rc)
		بازگشت rc;

	بازگشت cache_add(dev);
}

ایستا صحیح fdt_cache_add_generic(ثابت تهی *fdt, صحیح noff)
{
	ساختار cache_device *dev;
	صحیح rc;

	dev = sbi_zalloc(اندازه(*dev));
	اگر (!dev)
		بازگشت SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	اگر (rc) {
		sbi_free(dev);
		بازگشت rc;
	}

	بازگشت 0;
}

ایستا صحیح fdt_cache_find(ثابت تهی *fdt, صحیح noff, ساختار cache_device **out_dev)
{
	ساختار cache_device *dev = cache_find(noff);
	صحیح rc;

	اگر (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		اگر (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		اگر (rc)
			بازگشت rc;

		dev = cache_find(noff);
		اگر (!dev)
			بازگشت SBI_EFAIL;
	}

	اگر (out_dev)
		*out_dev = dev;

	بازگشت SBI_OK;
}

صحیح fdt_next_cache_get(ثابت تهی *fdt, صحیح noff, ساختار cache_device **out_dev)
{
	ثابت fdt32_t *val;
	صحیح len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	اگر (!val || len < اندازه(*val))
		بازگشت SBI_ENOENT;

	noff = fdt_گره_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	اگر (noff < 0)
		بازگشت noff;

	بازگشت fdt_cache_find(fdt, noff, out_dev);
}
