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
		གལ་སྲིད (dev->id == id)
			ལོག dev;
	}

	ལོག NULL;
}

ཧྲིལ cache_add(sa_nracanaa cache_device *dev)
{
	གལ་སྲིད (!dev)
		ལོག SBI_ENODEV;

	གལ་སྲིད (cache_find(dev->id))
		ལོག SBI_EALREADY;

	sbi_list_add(&dev->node, &cache_list);

	ལོག SBI_OK;
}

ཧྲིལ cache_flush_all(sa_nracanaa cache_device *dev)
{
	གལ་སྲིད (!dev)
		ལོག SBI_ENODEV;

	གལ་སྲིད (!dev->ops || !dev->ops->cache_flush_all)
		ལོག SBI_ENOTSUPP;

	ལོག dev->ops->cache_flush_all(dev);
}

ཧྲིལ cache_enable(sa_nracanaa cache_device *dev, bool enable)
{
	གལ་སྲིད (!dev)
		ལོག SBI_ENODEV;

	གལ་སྲིད (!dev->ops || !dev->ops->cache_enable)
		ལོག SBI_ENOTSUPP;

	ལོག dev->ops->cache_enable(dev, enable);
}
