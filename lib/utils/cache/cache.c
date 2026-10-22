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
		ပေႃး (dev->id == id)
			ပွၵ်ႈ dev;
	}

	ပွၵ်ႈ NULL;
}

puur_naa_nka cache_add(sa_nracanaa cache_device *dev)
{
	ပေႃး (!dev)
		ပွၵ်ႈ SBI_ENODEV;

	ပေႃး (cache_find(dev->id))
		ပွၵ်ႈ SBI_EALREADY;

	sbi_list_add(&dev->node, &cache_list);

	ပွၵ်ႈ SBI_OK;
}

puur_naa_nka cache_flush_all(sa_nracanaa cache_device *dev)
{
	ပေႃး (!dev)
		ပွၵ်ႈ SBI_ENODEV;

	ပေႃး (!dev->ops || !dev->ops->cache_flush_all)
		ပွၵ်ႈ SBI_ENOTSUPP;

	ပွၵ်ႈ dev->ops->cache_flush_all(dev);
}

puur_naa_nka cache_enable(sa_nracanaa cache_device *dev, bool enable)
{
	ပေႃး (!dev)
		ပွၵ်ႈ SBI_ENODEV;

	ပေႃး (!dev->ops || !dev->ops->cache_enable)
		ပွၵ်ႈ SBI_ENOTSUPP;

	ပွၵ်ႈ dev->ops->cache_enable(dev, enable);
}
