/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/cache/fdt_cache.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_driver.h>

/* List of FDT cache drivers generated at compile time */
ବାହ୍ୟ ଅଚଳ ସଂରଚନା fdt_driver *ଅଚଳ fdt_cache_drivers[];

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_cache_add(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା noff, ସଂରଚନା cache_device *dev)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, ଖାଲି), ଆକାର(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_next_cache_get(fdt, noff, &dev->next);
	ଯଦି (rc == SBI_ENOENT)
		dev->next = ଖାଲି;
	ଅନ୍ୟଥା ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	ଫେରନ୍ତୁ cache_add(dev);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_cache_add_generic(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା noff)
{
	ସଂରଚନା cache_device *dev;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	dev = sbi_zalloc(ଆକାର(*dev));
	ଯଦି (!dev)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	ଯଦି (rc) {
		sbi_free(dev);
		ଫେରନ୍ତୁ rc;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_cache_find(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା noff, ସଂରଚନା cache_device **out_dev)
{
	ସଂରଚନା cache_device *dev = cache_find(noff);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		ଯଦି (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;

		dev = cache_find(noff);
		ଯଦି (!dev)
			ଫେରନ୍ତୁ SBI_EFAIL;
	}

	ଯଦି (out_dev)
		*out_dev = dev;

	ଫେରନ୍ତୁ SBI_OK;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_next_cache_get(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା noff, ସଂରଚନା cache_device **out_dev)
{
	ଅଚଳ fdt32_t *val;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	ଯଦି (!val || len < ଆକାର(*val))
		ଫେରନ୍ତୁ SBI_ENOENT;

	noff = fdt_ଗଣ୍ଠି_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	ଯଦି (noff < 0)
		ଫେରନ୍ତୁ noff;

	ଫେରନ୍ତୁ fdt_cache_find(fdt, noff, out_dev);
}
