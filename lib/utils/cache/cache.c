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
		ꦪꦼꦤ꧀ (dev->id == id)
			ꦮꦭꦶ dev;
	}

	ꦮꦭꦶ NULL;
}

puur_naa_nka cache_add(sa_nracanaa cache_device *dev)
{
	ꦪꦼꦤ꧀ (!dev)
		ꦮꦭꦶ SBI_ENODEV;

	ꦪꦼꦤ꧀ (cache_find(dev->id))
		ꦮꦭꦶ SBI_EALREADY;

	sbi_list_add(&dev->node, &cache_list);

	ꦮꦭꦶ SBI_OK;
}

puur_naa_nka cache_flush_all(sa_nracanaa cache_device *dev)
{
	ꦪꦼꦤ꧀ (!dev)
		ꦮꦭꦶ SBI_ENODEV;

	ꦪꦼꦤ꧀ (!dev->ops || !dev->ops->cache_flush_all)
		ꦮꦭꦶ SBI_ENOTSUPP;

	ꦮꦭꦶ dev->ops->cache_flush_all(dev);
}

puur_naa_nka cache_enable(sa_nracanaa cache_device *dev, bool enable)
{
	ꦪꦼꦤ꧀ (!dev)
		ꦮꦭꦶ SBI_ENODEV;

	ꦪꦼꦤ꧀ (!dev->ops || !dev->ops->cache_enable)
		ꦮꦭꦶ SBI_ENOTSUPP;

	ꦮꦭꦶ dev->ops->cache_enable(dev, enable);
}
