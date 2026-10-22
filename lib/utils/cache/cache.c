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
		නම් (dev->id == id)
			ආපසු dev;
	}

	ආපසු NULL;
}

පූර්ණ_සංඛ්‍යා cache_add(sa_nracanaa cache_device *dev)
{
	නම් (!dev)
		ආපසු SBI_ENODEV;

	නම් (cache_find(dev->id))
		ආපසු SBI_EALREADY;

	sbi_list_add(&dev->node, &cache_list);

	ආපසු SBI_OK;
}

පූර්ණ_සංඛ්‍යා cache_flush_all(sa_nracanaa cache_device *dev)
{
	නම් (!dev)
		ආපසු SBI_ENODEV;

	නම් (!dev->ops || !dev->ops->cache_flush_all)
		ආපසු SBI_ENOTSUPP;

	ආපසු dev->ops->cache_flush_all(dev);
}

පූර්ණ_සංඛ්‍යා cache_enable(sa_nracanaa cache_device *dev, bool enable)
{
	නම් (!dev)
		ආපසු SBI_ENODEV;

	නම් (!dev->ops || !dev->ops->cache_enable)
		ආපසු SBI_ENOTSUPP;

	ආපසු dev->ops->cache_enable(dev, enable);
}
