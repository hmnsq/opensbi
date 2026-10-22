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
	אם (rc == SBI_ENOENT)
		dev->next = عدم;
	אחרת אם (rc)
		החזר rc;

	החזר cache_add(dev);
}

ساكن صحيح fdt_cache_add_generic(ثابت فراغ *fdt, صحيح noff)
{
	بنية cache_device *dev;
	صحيح rc;

	dev = sbi_zalloc(حجم(*dev));
	אם (!dev)
		החזר SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	אם (rc) {
		sbi_free(dev);
		החזר rc;
	}

	החזר 0;
}

ساكن صحيح fdt_cache_find(ثابت فراغ *fdt, صحيح noff, بنية cache_device **out_dev)
{
	بنية cache_device *dev = cache_find(noff);
	صحيح rc;

	אם (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		אם (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		אם (rc)
			החזר rc;

		dev = cache_find(noff);
		אם (!dev)
			החזר SBI_EFAIL;
	}

	אם (out_dev)
		*out_dev = dev;

	החזר SBI_OK;
}

صحيح fdt_next_cache_get(ثابت فراغ *fdt, صحيح noff, بنية cache_device **out_dev)
{
	ثابت fdt32_t *val;
	صحيح len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	אם (!val || len < حجم(*val))
		החזר SBI_ENOENT;

	noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	אם (noff < 0)
		החזר noff;

	החזר fdt_cache_find(fdt, noff, out_dev);
}
