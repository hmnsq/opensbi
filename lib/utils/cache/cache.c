/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi_utils/cache/cache.h>

স্থির SBI_LIST_HEAD(cache_list);

সংরচনা cache_device *cache_find(u32 id)
{
	সংরচনা cache_device *dev;

	sbi_list_for_each_entry(dev, &cache_list, গ্ৰন্থি) {
		যদি (dev->id == id)
			ঘূৰাই dev;
	}

	ঘূৰাই রিক্ত;
}

পূর্ণাঙ্ক cache_add(সংরচনা cache_device *dev)
{
	যদি (!dev)
		ঘূৰাই SBI_ENODEV;

	যদি (cache_find(dev->id))
		ঘূৰাই SBI_EALREADY;

	sbi_list_add(&dev->গ্ৰন্থি, &cache_list);

	ঘূৰাই SBI_OK;
}

পূর্ণাঙ্ক cache_flush_all(সংরচনা cache_device *dev)
{
	যদি (!dev)
		ঘূৰাই SBI_ENODEV;

	যদি (!dev->ops || !dev->ops->cache_flush_all)
		ঘূৰাই SBI_ENOTSUPP;

	ঘূৰাই dev->ops->cache_flush_all(dev);
}

পূর্ণাঙ্ক cache_enable(সংরচনা cache_device *dev, bool enable)
{
	যদি (!dev)
		ঘূৰাই SBI_ENODEV;

	যদি (!dev->ops || !dev->ops->cache_enable)
		ঘূৰাই SBI_ENOTSUPP;

	ঘূৰাই dev->ops->cache_enable(dev, enable);
}
