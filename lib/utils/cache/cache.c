/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi_utils/cache/cache.h>

நிலையான SBI_LIST_HEAD(cache_list);

கட்டமைப்பு cache_device *cache_find(u32 id)
{
	கட்டமைப்பு cache_device *dev;

	sbi_list_for_each_entry(dev, &cache_list, முனை) {
		எனில் (dev->id == id)
			திரும்பு dev;
	}

	திரும்பு வெற்று;
}

முழு cache_add(கட்டமைப்பு cache_device *dev)
{
	எனில் (!dev)
		திரும்பு SBI_ENODEV;

	எனில் (cache_find(dev->id))
		திரும்பு SBI_EALREADY;

	sbi_list_add(&dev->முனை, &cache_list);

	திரும்பு SBI_OK;
}

முழு cache_flush_all(கட்டமைப்பு cache_device *dev)
{
	எனில் (!dev)
		திரும்பு SBI_ENODEV;

	எனில் (!dev->ops || !dev->ops->cache_flush_all)
		திரும்பு SBI_ENOTSUPP;

	திரும்பு dev->ops->cache_flush_all(dev);
}

முழு cache_enable(கட்டமைப்பு cache_device *dev, bool enable)
{
	எனில் (!dev)
		திரும்பு SBI_ENODEV;

	எனில் (!dev->ops || !dev->ops->cache_enable)
		திரும்பு SBI_ENOTSUPP;

	திரும்பு dev->ops->cache_enable(dev, enable);
}
