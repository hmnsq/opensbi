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
		אם (dev->id == id)
			החזר dev;
	}

	החזר عدم;
}

صحيح cache_add(بنية cache_device *dev)
{
	אם (!dev)
		החזר SBI_ENODEV;

	אם (cache_find(dev->id))
		החזר SBI_EALREADY;

	sbi_list_add(&dev->node, &cache_list);

	החזר SBI_OK;
}

صحيح cache_flush_all(بنية cache_device *dev)
{
	אם (!dev)
		החזר SBI_ENODEV;

	אם (!dev->ops || !dev->ops->cache_flush_all)
		החזר SBI_ENOTSUPP;

	החזר dev->ops->cache_flush_all(dev);
}

صحيح cache_enable(بنية cache_device *dev, bool enable)
{
	אם (!dev)
		החזר SBI_ENODEV;

	אם (!dev->ops || !dev->ops->cache_enable)
		החזר SBI_ENOTSUPP;

	החזר dev->ops->cache_enable(dev, enable);
}
