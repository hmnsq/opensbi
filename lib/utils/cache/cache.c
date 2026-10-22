/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ضمن <sbi/sbi_error.h>
#ضمن <sbi_utils/cache/cache.h>

ساكن SBI_LIST_سر(cache_list);

بنية cache_device *cache_پیدا_کړه(u32 id)
{
	بنية cache_device *dev;

	sbi_list_for_each_ثبت(dev, &cache_list, غوټه) {
		إذا (dev->id == id)
			أرجع dev;
	}

	أرجع عدم;
}

صحيح cache_جمع_کړه(بنية cache_device *dev)
{
	إذا (!dev)
		أرجع SBI_ENODEV;

	إذا (cache_پیدا_کړه(dev->id))
		أرجع SBI_EALREADY;

	sbi_list_جمع_کړه(&dev->غوټه, &cache_list);

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
