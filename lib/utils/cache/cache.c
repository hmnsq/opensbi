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
		ᤀᤡ᤺ (dev->id == id)
			ᤀᤡᤴ dev;
	}

	ᤀᤡᤴ NULL;
}

puur_naa_nka cache_add(sa_nracanaa cache_device *dev)
{
	ᤀᤡ᤺ (!dev)
		ᤀᤡᤴ SBI_ENODEV;

	ᤀᤡ᤺ (cache_find(dev->id))
		ᤀᤡᤴ SBI_EALREADY;

	sbi_list_add(&dev->node, &cache_list);

	ᤀᤡᤴ SBI_OK;
}

puur_naa_nka cache_flush_all(sa_nracanaa cache_device *dev)
{
	ᤀᤡ᤺ (!dev)
		ᤀᤡᤴ SBI_ENODEV;

	ᤀᤡ᤺ (!dev->ops || !dev->ops->cache_flush_all)
		ᤀᤡᤴ SBI_ENOTSUPP;

	ᤀᤡᤴ dev->ops->cache_flush_all(dev);
}

puur_naa_nka cache_enable(sa_nracanaa cache_device *dev, bool enable)
{
	ᤀᤡ᤺ (!dev)
		ᤀᤡᤴ SBI_ENODEV;

	ᤀᤡ᤺ (!dev->ops || !dev->ops->cache_enable)
		ᤀᤡᤴ SBI_ENOTSUPP;

	ᤀᤡᤴ dev->ops->cache_enable(dev, enable);
}
