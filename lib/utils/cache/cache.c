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
		もし (dev->id == id)
			戻る dev;
	}

	戻る NULL;
}

整数 cache_add(struct cache_device *dev)
{
	もし (!dev)
		戻る SBI_ENODEV;

	もし (cache_find(dev->id))
		戻る SBI_EALREADY;

	sbi_list_add(&dev->node, &cache_list);

	戻る SBI_OK;
}

整数 cache_flush_all(struct cache_device *dev)
{
	もし (!dev)
		戻る SBI_ENODEV;

	もし (!dev->ops || !dev->ops->cache_flush_all)
		戻る SBI_ENOTSUPP;

	戻る dev->ops->cache_flush_all(dev);
}

整数 cache_enable(struct cache_device *dev, bool enable)
{
	もし (!dev)
		戻る SBI_ENODEV;

	もし (!dev->ops || !dev->ops->cache_enable)
		戻る SBI_ENOTSUPP;

	戻る dev->ops->cache_enable(dev, enable);
}
