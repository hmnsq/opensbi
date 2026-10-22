/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ضمن <libfdt.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi_utils/cache/fdt_cache.h>
#ضمن <sbi_utils/fdt/fdt_driver.h>

/* List of FDT cache drivers generated at compile time */
خارجي ثابت بنية fdt_driver *ثابت fdt_cache_drivers[];

صحيح fdt_cache_add(ثابت فراغ *fdt, صحيح noff, بنية cache_device *dev)
{
	صحيح rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, عدم), حجم(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_next_cache_get(fdt, noff, &dev->next);
	ܐܢ (rc == SBI_ENOENT)
		dev->next = عدم;
	ܐܚܪܢܐ ܐܢ (rc)
		ܦܢܐ rc;

	ܦܢܐ cache_add(dev);
}

ساكن صحيح fdt_cache_add_generic(ثابت فراغ *fdt, صحيح noff)
{
	بنية cache_device *dev;
	صحيح rc;

	dev = sbi_zalloc(حجم(*dev));
	ܐܢ (!dev)
		ܦܢܐ SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	ܐܢ (rc) {
		sbi_free(dev);
		ܦܢܐ rc;
	}

	ܦܢܐ 0;
}

ساكن صحيح fdt_cache_find(ثابت فراغ *fdt, صحيح noff, بنية cache_device **out_dev)
{
	بنية cache_device *dev = cache_find(noff);
	صحيح rc;

	ܐܢ (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		ܐܢ (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		ܐܢ (rc)
			ܦܢܐ rc;

		dev = cache_find(noff);
		ܐܢ (!dev)
			ܦܢܐ SBI_EFAIL;
	}

	ܐܢ (out_dev)
		*out_dev = dev;

	ܦܢܐ SBI_OK;
}

صحيح fdt_next_cache_get(ثابت فراغ *fdt, صحيح noff, بنية cache_device **out_dev)
{
	ثابت fdt32_t *val;
	صحيح len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	ܐܢ (!val || len < حجم(*val))
		ܦܢܐ SBI_ENOENT;

	noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	ܐܢ (noff < 0)
		ܦܢܐ noff;

	ܦܢܐ fdt_cache_find(fdt, noff, out_dev);
}
