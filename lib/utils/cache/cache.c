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
		ถ้า (dev->id == id)
			คืน dev;
	}

	คืน NULL;
}

จำนวนเต็ม cache_add(sa_nracanaa cache_device *dev)
{
	ถ้า (!dev)
		คืน SBI_ENODEV;

	ถ้า (cache_find(dev->id))
		คืน SBI_EALREADY;

	sbi_list_add(&dev->node, &cache_list);

	คืน SBI_OK;
}

จำนวนเต็ม cache_flush_all(sa_nracanaa cache_device *dev)
{
	ถ้า (!dev)
		คืน SBI_ENODEV;

	ถ้า (!dev->ops || !dev->ops->cache_flush_all)
		คืน SBI_ENOTSUPP;

	คืน dev->ops->cache_flush_all(dev);
}

จำนวนเต็ม cache_enable(sa_nracanaa cache_device *dev, bool enable)
{
	ถ้า (!dev)
		คืน SBI_ENODEV;

	ถ้า (!dev->ops || !dev->ops->cache_enable)
		คืน SBI_ENOTSUPP;

	คืน dev->ops->cache_enable(dev, enable);
}
