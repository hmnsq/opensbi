/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#include <sbi/sbi_error.h>
#include <sbi_utils/cache/cache.h>

static SBI_LIST_HEAD(cache_list);

struct cache_device *cache_find(u32 id)
{
	struct cache_device *dev;

	sbi_list_for_each_entry(dev, &cache_list, node) {
		如果 (dev->id == id)
			返回 dev;
	}

	返回 NULL;
}

整数 cache_add(struct cache_device *dev)
{
	如果 (!dev)
		返回 SBI_ENODEV;

	如果 (cache_find(dev->id))
		返回 SBI_EALREADY;

	sbi_list_add(&dev->node, &cache_list);

	返回 SBI_OK;
}

整数 cache_flush_all(struct cache_device *dev)
{
	如果 (!dev)
		返回 SBI_ENODEV;

	如果 (!dev->ops || !dev->ops->cache_flush_all)
		返回 SBI_ENOTSUPP;

	返回 dev->ops->cache_flush_all(dev);
}

整数 cache_enable(struct cache_device *dev, bool enable)
{
	如果 (!dev)
		返回 SBI_ENODEV;

	如果 (!dev->ops || !dev->ops->cache_enable)
		返回 SBI_ENOTSUPP;

	返回 dev->ops->cache_enable(dev, enable);
}
