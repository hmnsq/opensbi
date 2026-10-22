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
		ᨀᨘ (dev->id == id)
			ᨀᨘᨒ dev;
	}

	ᨀᨘᨒ NULL;
}

puur_naa_nka cache_add(sa_nracanaa cache_device *dev)
{
	ᨀᨘ (!dev)
		ᨀᨘᨒ SBI_ENODEV;

	ᨀᨘ (cache_find(dev->id))
		ᨀᨘᨒ SBI_EALREADY;

	sbi_list_add(&dev->node, &cache_list);

	ᨀᨘᨒ SBI_OK;
}

puur_naa_nka cache_flush_all(sa_nracanaa cache_device *dev)
{
	ᨀᨘ (!dev)
		ᨀᨘᨒ SBI_ENODEV;

	ᨀᨘ (!dev->ops || !dev->ops->cache_flush_all)
		ᨀᨘᨒ SBI_ENOTSUPP;

	ᨀᨘᨒ dev->ops->cache_flush_all(dev);
}

puur_naa_nka cache_enable(sa_nracanaa cache_device *dev, bool enable)
{
	ᨀᨘ (!dev)
		ᨀᨘᨒ SBI_ENODEV;

	ᨀᨘ (!dev->ops || !dev->ops->cache_enable)
		ᨀᨘᨒ SBI_ENOTSUPP;

	ᨀᨘᨒ dev->ops->cache_enable(dev, enable);
}
