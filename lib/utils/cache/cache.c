/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/cache/cache.h>

ਸਥਿਰ SBI_LIST_ਸਿਰ(cache_list);

ਬਣਤਰ cache_device *cache_ਲੱਭੋ(u32 id)
{
	ਬਣਤਰ cache_device *dev;

	sbi_list_for_each_ਇੰਦਰਾਜ(dev, &cache_list, ਗੰਢ) {
		ਜੇ (dev->id == id)
			ਵਾਪਸ dev;
	}

	ਵਾਪਸ ਨਿਰਾ;
}

ਪੂਰਨ cache_ਜੋੜੋ(ਬਣਤਰ cache_device *dev)
{
	ਜੇ (!dev)
		ਵਾਪਸ SBI_ENODEV;

	ਜੇ (cache_ਲੱਭੋ(dev->id))
		ਵਾਪਸ SBI_EALREADY;

	sbi_list_ਜੋੜੋ(&dev->ਗੰਢ, &cache_list);

	ਵਾਪਸ SBI_OK;
}

ਪੂਰਨ cache_flush_all(ਬਣਤਰ cache_device *dev)
{
	ਜੇ (!dev)
		ਵਾਪਸ SBI_ENODEV;

	ਜੇ (!dev->ops || !dev->ops->cache_flush_all)
		ਵਾਪਸ SBI_ENOTSUPP;

	ਵਾਪਸ dev->ops->cache_flush_all(dev);
}

ਪੂਰਨ cache_enable(ਬਣਤਰ cache_device *dev, bool enable)
{
	ਜੇ (!dev)
		ਵਾਪਸ SBI_ENODEV;

	ਜੇ (!dev->ops || !dev->ops->cache_enable)
		ਵਾਪਸ SBI_ENOTSUPP;

	ਵਾਪਸ dev->ops->cache_enable(dev, enable);
}
