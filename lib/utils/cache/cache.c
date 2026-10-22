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
		якщо (dev->id == id)
			повернути dev;
	}

	повернути NULL;
}

ціле cache_add(struct cache_device *dev)
{
	якщо (!dev)
		повернути SBI_ENODEV;

	якщо (cache_find(dev->id))
		повернути SBI_EALREADY;

	sbi_list_add(&dev->node, &cache_list);

	повернути SBI_OK;
}

ціле cache_flush_all(struct cache_device *dev)
{
	якщо (!dev)
		повернути SBI_ENODEV;

	якщо (!dev->ops || !dev->ops->cache_flush_all)
		повернути SBI_ENOTSUPP;

	повернути dev->ops->cache_flush_all(dev);
}

ціле cache_enable(struct cache_device *dev, bool enable)
{
	якщо (!dev)
		повернути SBI_ENODEV;

	якщо (!dev->ops || !dev->ops->cache_enable)
		повернути SBI_ENOTSUPP;

	повернути dev->ops->cache_enable(dev, enable);
}
