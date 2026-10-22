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
		ако (dev->id == id)
			врати dev;
	}

	врати NULL;
}

цео cache_add(struct cache_device *dev)
{
	ако (!dev)
		врати SBI_ENODEV;

	ако (cache_find(dev->id))
		врати SBI_EALREADY;

	sbi_list_add(&dev->node, &cache_list);

	врати SBI_OK;
}

цео cache_flush_all(struct cache_device *dev)
{
	ако (!dev)
		врати SBI_ENODEV;

	ако (!dev->ops || !dev->ops->cache_flush_all)
		врати SBI_ENOTSUPP;

	врати dev->ops->cache_flush_all(dev);
}

цео cache_enable(struct cache_device *dev, bool enable)
{
	ако (!dev)
		врати SBI_ENODEV;

	ако (!dev->ops || !dev->ops->cache_enable)
		врати SBI_ENOTSUPP;

	врати dev->ops->cache_enable(dev, enable);
}
