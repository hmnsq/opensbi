/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ضمن <sbi/sbi_error.h>
#ضمن <sbi_utils/cache/cache.h>

ساكن SBI_LIST_HEAD(cache_list);

بنية cache_device *cache_find(u32 id)
{
	بنية cache_device *dev;

	sbi_list_for_each_entry(dev, &cache_list, node) {
		ܐܢ (dev->id == id)
			ܦܢܐ dev;
	}

	ܦܢܐ عدم;
}

صحيح cache_add(بنية cache_device *dev)
{
	ܐܢ (!dev)
		ܦܢܐ SBI_ENODEV;

	ܐܢ (cache_find(dev->id))
		ܦܢܐ SBI_EALREADY;

	sbi_list_add(&dev->node, &cache_list);

	ܦܢܐ SBI_OK;
}

صحيح cache_flush_all(بنية cache_device *dev)
{
	ܐܢ (!dev)
		ܦܢܐ SBI_ENODEV;

	ܐܢ (!dev->ops || !dev->ops->cache_flush_all)
		ܦܢܐ SBI_ENOTSUPP;

	ܦܢܐ dev->ops->cache_flush_all(dev);
}

صحيح cache_enable(بنية cache_device *dev, bool enable)
{
	ܐܢ (!dev)
		ܦܢܐ SBI_ENODEV;

	ܐܢ (!dev->ops || !dev->ops->cache_enable)
		ܦܢܐ SBI_ENOTSUPP;

	ܦܢܐ dev->ops->cache_enable(dev, enable);
}
