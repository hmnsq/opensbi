/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi_utils/cache/cache.h>

স্থির SBI_LIST_HEAD(cache_list);

সংরচনা cache_device *cache_find(u32 id)
{
	সংরচনা cache_device *dev;

	sbi_list_for_each_entry(dev, &cache_list, গ্রন্থি) {
		যদি (dev->id == id)
			প্রত্যাগম dev;
	}

	প্রত্যাগম রিক্ত;
}

পূর্ণাঙ্ক cache_add(সংরচনা cache_device *dev)
{
	যদি (!dev)
		প্রত্যাগম SBI_ENODEV;

	যদি (cache_find(dev->id))
		প্রত্যাগম SBI_EALREADY;

	sbi_list_add(&dev->গ্রন্থি, &cache_list);

	প্রত্যাগম SBI_OK;
}

পূর্ণাঙ্ক cache_flush_all(সংরচনা cache_device *dev)
{
	যদি (!dev)
		প্রত্যাগম SBI_ENODEV;

	যদি (!dev->ops || !dev->ops->cache_flush_all)
		প্রত্যাগম SBI_ENOTSUPP;

	প্রত্যাগম dev->ops->cache_flush_all(dev);
}

পূর্ণাঙ্ক cache_enable(সংরচনা cache_device *dev, bool enable)
{
	যদি (!dev)
		প্রত্যাগম SBI_ENODEV;

	যদি (!dev->ops || !dev->ops->cache_enable)
		প্রত্যাগম SBI_ENOTSUPP;

	প্রত্যাগম dev->ops->cache_enable(dev, enable);
}
