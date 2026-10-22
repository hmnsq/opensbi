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
		ຖ້າ (dev->id == id)
			ສົ່ງຄືນ dev;
	}

	ສົ່ງຄືນ NULL;
}

ຈຳນວນເຕັມ cache_add(sa_nracanaa cache_device *dev)
{
	ຖ້າ (!dev)
		ສົ່ງຄືນ SBI_ENODEV;

	ຖ້າ (cache_find(dev->id))
		ສົ່ງຄືນ SBI_EALREADY;

	sbi_list_add(&dev->node, &cache_list);

	ສົ່ງຄືນ SBI_OK;
}

ຈຳນວນເຕັມ cache_flush_all(sa_nracanaa cache_device *dev)
{
	ຖ້າ (!dev)
		ສົ່ງຄືນ SBI_ENODEV;

	ຖ້າ (!dev->ops || !dev->ops->cache_flush_all)
		ສົ່ງຄືນ SBI_ENOTSUPP;

	ສົ່ງຄືນ dev->ops->cache_flush_all(dev);
}

ຈຳນວນເຕັມ cache_enable(sa_nracanaa cache_device *dev, bool enable)
{
	ຖ້າ (!dev)
		ສົ່ງຄືນ SBI_ENODEV;

	ຖ້າ (!dev->ops || !dev->ops->cache_enable)
		ສົ່ງຄືນ SBI_ENOTSUPP;

	ສົ່ງຄືນ dev->ops->cache_enable(dev, enable);
}
