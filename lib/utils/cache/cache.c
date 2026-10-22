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
		ᰀᰧ (dev->id == id)
			ᰀᰧᰌ dev;
	}

	ᰀᰧᰌ NULL;
}

puur_naa_nka cache_add(sa_nracanaa cache_device *dev)
{
	ᰀᰧ (!dev)
		ᰀᰧᰌ SBI_ENODEV;

	ᰀᰧ (cache_find(dev->id))
		ᰀᰧᰌ SBI_EALREADY;

	sbi_list_add(&dev->node, &cache_list);

	ᰀᰧᰌ SBI_OK;
}

puur_naa_nka cache_flush_all(sa_nracanaa cache_device *dev)
{
	ᰀᰧ (!dev)
		ᰀᰧᰌ SBI_ENODEV;

	ᰀᰧ (!dev->ops || !dev->ops->cache_flush_all)
		ᰀᰧᰌ SBI_ENOTSUPP;

	ᰀᰧᰌ dev->ops->cache_flush_all(dev);
}

puur_naa_nka cache_enable(sa_nracanaa cache_device *dev, bool enable)
{
	ᰀᰧ (!dev)
		ᰀᰧᰌ SBI_ENODEV;

	ᰀᰧ (!dev->ops || !dev->ops->cache_enable)
		ᰀᰧᰌ SBI_ENOTSUPP;

	ᰀᰧᰌ dev->ops->cache_enable(dev, enable);
}
