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
			върни dev;
	}

	върни NULL;
}

цяло cache_add(struct cache_device *dev)
{
	ако (!dev)
		върни SBI_ENODEV;

	ако (cache_find(dev->id))
		върни SBI_EALREADY;

	sbi_list_add(&dev->node, &cache_list);

	върни SBI_OK;
}

цяло cache_flush_all(struct cache_device *dev)
{
	ако (!dev)
		върни SBI_ENODEV;

	ако (!dev->ops || !dev->ops->cache_flush_all)
		върни SBI_ENOTSUPP;

	върни dev->ops->cache_flush_all(dev);
}

цяло cache_enable(struct cache_device *dev, bool enable)
{
	ако (!dev)
		върни SBI_ENODEV;

	ако (!dev->ops || !dev->ops->cache_enable)
		върни SBI_ENOTSUPP;

	върни dev->ops->cache_enable(dev, enable);
}
