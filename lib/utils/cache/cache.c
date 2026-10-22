/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/cache/cache.h>

ସ୍ଥିର SBI_LIST_HEAD(cache_list);

ସଂରଚନା cache_device *cache_find(u32 id)
{
	ସଂରଚନା cache_device *dev;

	sbi_list_for_each_entry(dev, &cache_list, ଗଣ୍ଠି) {
		ଯଦି (dev->id == id)
			ଫେରନ୍ତୁ dev;
	}

	ଫେରନ୍ତୁ ଖାଲି;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cache_add(ସଂରଚନା cache_device *dev)
{
	ଯଦି (!dev)
		ଫେରନ୍ତୁ SBI_ENODEV;

	ଯଦି (cache_find(dev->id))
		ଫେରନ୍ତୁ SBI_EALREADY;

	sbi_list_add(&dev->ଗଣ୍ଠି, &cache_list);

	ଫେରନ୍ତୁ SBI_OK;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cache_flush_all(ସଂରଚନା cache_device *dev)
{
	ଯଦି (!dev)
		ଫେରନ୍ତୁ SBI_ENODEV;

	ଯଦି (!dev->ops || !dev->ops->cache_flush_all)
		ଫେରନ୍ତୁ SBI_ENOTSUPP;

	ଫେରନ୍ତୁ dev->ops->cache_flush_all(dev);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cache_enable(ସଂରଚନା cache_device *dev, bool enable)
{
	ଯଦି (!dev)
		ଫେରନ୍ତୁ SBI_ENODEV;

	ଯଦି (!dev->ops || !dev->ops->cache_enable)
		ଫେରନ୍ତୁ SBI_ENOTSUPP;

	ଫେରନ୍ତୁ dev->ops->cache_enable(dev, enable);
}
