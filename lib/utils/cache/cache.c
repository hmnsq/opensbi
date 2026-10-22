/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ضمن <sbi/sbi_error.h>
#ضمن <sbi_utils/cache/cache.h>

ساكن SBI_LIST_سر(cache_list);

بنية cache_device *cache_ڳوليو(u32 id)
{
	بنية cache_device *dev;

	sbi_list_for_each_داخلا(dev, &cache_list, ڳنڍ) {
		إذا (dev->id == id)
			أرجع dev;
	}

	أرجع عدم;
}

صحيح cache_جوڙيو(بنية cache_device *dev)
{
	إذا (!dev)
		أرجع SBI_ENODEV;

	إذا (cache_ڳوليو(dev->id))
		أرجع SBI_EALREADY;

	sbi_list_جوڙيو(&dev->ڳنڍ, &cache_list);

	أرجع SBI_OK;
}

صحيح cache_flush_all(بنية cache_device *dev)
{
	إذا (!dev)
		أرجع SBI_ENODEV;

	إذا (!dev->ops || !dev->ops->cache_flush_all)
		أرجع SBI_ENOTSUPP;

	أرجع dev->ops->cache_flush_all(dev);
}

صحيح cache_enable(بنية cache_device *dev, bool enable)
{
	إذا (!dev)
		أرجع SBI_ENODEV;

	إذا (!dev->ops || !dev->ops->cache_enable)
		أرجع SBI_ENOTSUPP;

	أرجع dev->ops->cache_enable(dev, enable);
}
