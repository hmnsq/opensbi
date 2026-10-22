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
		ᬬᬾᬦ᭄ (dev->id == id)
			ᬯᬮᬶ dev;
	}

	ᬯᬮᬶ NULL;
}

puur_naa_nka cache_add(sa_nracanaa cache_device *dev)
{
	ᬬᬾᬦ᭄ (!dev)
		ᬯᬮᬶ SBI_ENODEV;

	ᬬᬾᬦ᭄ (cache_find(dev->id))
		ᬯᬮᬶ SBI_EALREADY;

	sbi_list_add(&dev->node, &cache_list);

	ᬯᬮᬶ SBI_OK;
}

puur_naa_nka cache_flush_all(sa_nracanaa cache_device *dev)
{
	ᬬᬾᬦ᭄ (!dev)
		ᬯᬮᬶ SBI_ENODEV;

	ᬬᬾᬦ᭄ (!dev->ops || !dev->ops->cache_flush_all)
		ᬯᬮᬶ SBI_ENOTSUPP;

	ᬯᬮᬶ dev->ops->cache_flush_all(dev);
}

puur_naa_nka cache_enable(sa_nracanaa cache_device *dev, bool enable)
{
	ᬬᬾᬦ᭄ (!dev)
		ᬯᬮᬶ SBI_ENODEV;

	ᬬᬾᬦ᭄ (!dev->ops || !dev->ops->cache_enable)
		ᬯᬮᬶ SBI_ENOTSUPP;

	ᬯᬮᬶ dev->ops->cache_enable(dev, enable);
}
