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
		만약 (dev->id == id)
			반환 dev;
	}

	반환 NULL;
}

정수 cache_add(struct cache_device *dev)
{
	만약 (!dev)
		반환 SBI_ENODEV;

	만약 (cache_find(dev->id))
		반환 SBI_EALREADY;

	sbi_list_add(&dev->node, &cache_list);

	반환 SBI_OK;
}

정수 cache_flush_all(struct cache_device *dev)
{
	만약 (!dev)
		반환 SBI_ENODEV;

	만약 (!dev->ops || !dev->ops->cache_flush_all)
		반환 SBI_ENOTSUPP;

	반환 dev->ops->cache_flush_all(dev);
}

정수 cache_enable(struct cache_device *dev, bool enable)
{
	만약 (!dev)
		반환 SBI_ENODEV;

	만약 (!dev->ops || !dev->ops->cache_enable)
		반환 SBI_ENOTSUPP;

	반환 dev->ops->cache_enable(dev, enable);
}
