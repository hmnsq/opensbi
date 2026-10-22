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
		если (dev->id == id)
			вернуть dev;
	}

	вернуть NULL;
}

целое cache_add(struct cache_device *dev)
{
	если (!dev)
		вернуть SBI_ENODEV;

	если (cache_find(dev->id))
		вернуть SBI_EALREADY;

	sbi_list_add(&dev->node, &cache_list);

	вернуть SBI_OK;
}

целое cache_flush_all(struct cache_device *dev)
{
	если (!dev)
		вернуть SBI_ENODEV;

	если (!dev->ops || !dev->ops->cache_flush_all)
		вернуть SBI_ENOTSUPP;

	вернуть dev->ops->cache_flush_all(dev);
}

целое cache_enable(struct cache_device *dev, bool enable)
{
	если (!dev)
		вернуть SBI_ENODEV;

	если (!dev->ops || !dev->ops->cache_enable)
		вернуть SBI_ENOTSUPP;

	вернуть dev->ops->cache_enable(dev, enable);
}
