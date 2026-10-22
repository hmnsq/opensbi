/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#include <sbi/sbi_error.h>
#include <sbi_utils/cache/cache.h>

static SBI_LIST_HEAD(cache_list);

sa_nracanaa cache_device *cache_find(u32 id)
{
	sa_nracanaa cache_device *dev;

	sbi_list_for_each_entry(dev, &cache_list, node) {
		གལ་ཏེ (dev->id == id)
			ཕྱིར dev;
	}

	ཕྱིར NULL;
}

ཧྲིལ cache_add(sa_nracanaa cache_device *dev)
{
	གལ་ཏེ (!dev)
		ཕྱིར SBI_ENODEV;

	གལ་ཏེ (cache_find(dev->id))
		ཕྱིར SBI_EALREADY;

	sbi_list_add(&dev->node, &cache_list);

	ཕྱིར SBI_OK;
}

ཧྲིལ cache_flush_all(sa_nracanaa cache_device *dev)
{
	གལ་ཏེ (!dev)
		ཕྱིར SBI_ENODEV;

	གལ་ཏེ (!dev->ops || !dev->ops->cache_flush_all)
		ཕྱིར SBI_ENOTSUPP;

	ཕྱིར dev->ops->cache_flush_all(dev);
}

ཧྲིལ cache_enable(sa_nracanaa cache_device *dev, bool enable)
{
	གལ་ཏེ (!dev)
		ཕྱིར SBI_ENODEV;

	གལ་ཏེ (!dev->ops || !dev->ops->cache_enable)
		ཕྱིར SBI_ENOTSUPP;

	ཕྱིར dev->ops->cache_enable(dev, enable);
}
