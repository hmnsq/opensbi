/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#অন্তর্ভাৱ <libfdt.h>
#অন্তর্ভাৱ <sbi/sbi_console.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_heap.h>
#অন্তর্ভাৱ <sbi_utils/cache/fdt_cache.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_driver.h>

/* List of FDT cache drivers generated at compile time */
বাহ্য ধ্রুৱ সংরচনা fdt_driver *ধ্রুৱ fdt_cache_drivers[];

পূর্ণাঙ্ক fdt_cache_add(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক noff, সংরচনা cache_device *dev)
{
	পূর্ণাঙ্ক rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, রিক্ত), পরিমাণ(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_next_cache_get(fdt, noff, &dev->next);
	যদি (rc == SBI_ENOENT)
		dev->next = রিক্ত;
	অন্যথা যদি (rc)
		ঘূৰাই rc;

	ঘূৰাই cache_add(dev);
}

স্থির পূর্ণাঙ্ক fdt_cache_add_generic(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক noff)
{
	সংরচনা cache_device *dev;
	পূর্ণাঙ্ক rc;

	dev = sbi_zalloc(পরিমাণ(*dev));
	যদি (!dev)
		ঘূৰাই SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	যদি (rc) {
		sbi_free(dev);
		ঘূৰাই rc;
	}

	ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক fdt_cache_find(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক noff, সংরচনা cache_device **out_dev)
{
	সংরচনা cache_device *dev = cache_find(noff);
	পূর্ণাঙ্ক rc;

	যদি (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		যদি (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		যদি (rc)
			ঘূৰাই rc;

		dev = cache_find(noff);
		যদি (!dev)
			ঘূৰাই SBI_EFAIL;
	}

	যদি (out_dev)
		*out_dev = dev;

	ঘূৰাই SBI_OK;
}

পূর্ণাঙ্ক fdt_next_cache_get(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক noff, সংরচনা cache_device **out_dev)
{
	ধ্রুৱ fdt32_t *val;
	পূর্ণাঙ্ক len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	যদি (!val || len < পরিমাণ(*val))
		ঘূৰাই SBI_ENOENT;

	noff = fdt_গ্ৰন্থি_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	যদি (noff < 0)
		ঘূৰাই noff;

	ঘূৰাই fdt_cache_find(fdt, noff, out_dev);
}
