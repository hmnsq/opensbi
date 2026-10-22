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
		ប្រសិនបើ (dev->id == id)
			ត្រឡប់ dev;
	}

	ត្រឡប់ NULL;
}

ចំនួនគត់ cache_add(sa_nracanaa cache_device *dev)
{
	ប្រសិនបើ (!dev)
		ត្រឡប់ SBI_ENODEV;

	ប្រសិនបើ (cache_find(dev->id))
		ត្រឡប់ SBI_EALREADY;

	sbi_list_add(&dev->node, &cache_list);

	ត្រឡប់ SBI_OK;
}

ចំនួនគត់ cache_flush_all(sa_nracanaa cache_device *dev)
{
	ប្រសិនបើ (!dev)
		ត្រឡប់ SBI_ENODEV;

	ប្រសិនបើ (!dev->ops || !dev->ops->cache_flush_all)
		ត្រឡប់ SBI_ENOTSUPP;

	ត្រឡប់ dev->ops->cache_flush_all(dev);
}

ចំនួនគត់ cache_enable(sa_nracanaa cache_device *dev, bool enable)
{
	ប្រសិនបើ (!dev)
		ត្រឡប់ SBI_ENODEV;

	ប្រសិនបើ (!dev->ops || !dev->ops->cache_enable)
		ត្រឡប់ SBI_ENOTSUPP;

	ត្រឡប់ dev->ops->cache_enable(dev, enable);
}
