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
	𐤀𐤌 (rc == SBI_ENOENT)
		dev->next = عدم;
	وإلا 𐤀𐤌 (rc)
		أرجع rc;

	أرجع cache_add(dev);
}

ساكن صحيح fdt_cache_add_generic(ثابت فراغ *fdt, صحيح noff)
{
	بنية cache_device *dev;
	صحيح rc;

	dev = sbi_zalloc(حجم(*dev));
	𐤀𐤌 (!dev)
		أرجع SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	𐤀𐤌 (rc) {
		sbi_free(dev);
		أرجع rc;
	}

	أرجع 0;
}

ساكن صحيح fdt_cache_find(ثابت فراغ *fdt, صحيح noff, بنية cache_device **out_dev)
{
	بنية cache_device *dev = cache_find(noff);
	صحيح rc;

	𐤀𐤌 (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		𐤀𐤌 (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		𐤀𐤌 (rc)
			أرجع rc;

		dev = cache_find(noff);
		𐤀𐤌 (!dev)
			أرجع SBI_EFAIL;
	}

	𐤀𐤌 (out_dev)
		*out_dev = dev;

	أرجع SBI_OK;
}

صحيح fdt_next_cache_get(ثابت فراغ *fdt, صحيح noff, بنية cache_device **out_dev)
{
	ثابت fdt32_t *val;
	صحيح len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	𐤀𐤌 (!val || len < حجم(*val))
		أرجع SBI_ENOENT;

	noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	𐤀𐤌 (noff < 0)
		أرجع noff;

	أرجع fdt_cache_find(fdt, noff, out_dev);
}
