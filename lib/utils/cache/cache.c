/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi_utils/cache/cache.h>

স্থির SBI_LIST_শীর্ষ(cache_list);

কাঠামো cache_device *cache_find(u32 id)
{
	কাঠামো cache_device *dev;

	sbi_list_for_each_entry(dev, &cache_list, গ্রন্থি) {
		যদি (dev->id == id)
			ফেরত dev;
	}

	ফেরত ফাঁকা;
}

পূর্ণসংখ্যা cache_add(কাঠামো cache_device *dev)
{
	যদি (!dev)
		ফেরত SBI_ENODEV;

	যদি (cache_find(dev->id))
		ফেরত SBI_EALREADY;

	sbi_list_add(&dev->গ্রন্থি, &cache_list);

	ফেরত SBI_OK;
}

পূর্ণসংখ্যা cache_flush_all(কাঠামো cache_device *dev)
{
	যদি (!dev)
		ফেরত SBI_ENODEV;

	যদি (!dev->ops || !dev->ops->cache_flush_all)
		ফেরত SBI_ENOTSUPP;

	ফেরত dev->ops->cache_flush_all(dev);
}

পূর্ণসংখ্যা cache_enable(কাঠামো cache_device *dev, bool enable)
{
	যদি (!dev)
		ফেরত SBI_ENODEV;

	যদি (!dev->ops || !dev->ops->cache_enable)
		ফেরত SBI_ENOTSUPP;

	ফেরত dev->ops->cache_enable(dev, enable);
}
