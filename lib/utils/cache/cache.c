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
		အကယ်၍ (dev->id == id)
			ပြန်ပေး dev;
	}

	ပြန်ပေး NULL;
}

ကိန်းပြည့် cache_add(sa_nracanaa cache_device *dev)
{
	အကယ်၍ (!dev)
		ပြန်ပေး SBI_ENODEV;

	အကယ်၍ (cache_find(dev->id))
		ပြန်ပေး SBI_EALREADY;

	sbi_list_add(&dev->node, &cache_list);

	ပြန်ပေး SBI_OK;
}

ကိန်းပြည့် cache_flush_all(sa_nracanaa cache_device *dev)
{
	အကယ်၍ (!dev)
		ပြန်ပေး SBI_ENODEV;

	အကယ်၍ (!dev->ops || !dev->ops->cache_flush_all)
		ပြန်ပေး SBI_ENOTSUPP;

	ပြန်ပေး dev->ops->cache_flush_all(dev);
}

ကိန်းပြည့် cache_enable(sa_nracanaa cache_device *dev, bool enable)
{
	အကယ်၍ (!dev)
		ပြန်ပေး SBI_ENODEV;

	အကယ်၍ (!dev->ops || !dev->ops->cache_enable)
		ပြန်ပေး SBI_ENOTSUPP;

	ပြန်ပေး dev->ops->cache_enable(dev, enable);
}
