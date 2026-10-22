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
		nếu (dev->id == id)
			trả_về dev;
	}

	trả_về NULL;
}

số_nguyên cache_add(struct cache_device *dev)
{
	nếu (!dev)
		trả_về SBI_ENODEV;

	nếu (cache_find(dev->id))
		trả_về SBI_EALREADY;

	sbi_list_add(&dev->node, &cache_list);

	trả_về SBI_OK;
}

số_nguyên cache_flush_all(struct cache_device *dev)
{
	nếu (!dev)
		trả_về SBI_ENODEV;

	nếu (!dev->ops || !dev->ops->cache_flush_all)
		trả_về SBI_ENOTSUPP;

	trả_về dev->ops->cache_flush_all(dev);
}

số_nguyên cache_enable(struct cache_device *dev, bool enable)
{
	nếu (!dev)
		trả_về SBI_ENODEV;

	nếu (!dev->ops || !dev->ops->cache_enable)
		trả_về SBI_ENOTSUPP;

	trả_về dev->ops->cache_enable(dev, enable);
}
