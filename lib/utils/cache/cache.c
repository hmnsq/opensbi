/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#شامل <sbi/sbi_error.h>
#شامل <sbi_utils/cache/cache.h>

ایستا SBI_LIST_HEAD(cache_list);

ساختار cache_device *cache_find(u32 id)
{
	ساختار cache_device *dev;

	sbi_list_for_each_entry(dev, &cache_list, گره) {
		اگر (dev->id == id)
			بازگشت dev;
	}

	بازگشت پوچ;
}

صحیح cache_add(ساختار cache_device *dev)
{
	اگر (!dev)
		بازگشت SBI_ENODEV;

	اگر (cache_find(dev->id))
		بازگشت SBI_EALREADY;

	sbi_list_add(&dev->گره, &cache_list);

	بازگشت SBI_OK;
}

صحیح cache_flush_all(ساختار cache_device *dev)
{
	اگر (!dev)
		بازگشت SBI_ENODEV;

	اگر (!dev->ops || !dev->ops->cache_flush_all)
		بازگشت SBI_ENOTSUPP;

	بازگشت dev->ops->cache_flush_all(dev);
}

صحیح cache_enable(ساختار cache_device *dev, bool enable)
{
	اگر (!dev)
		بازگشت SBI_ENODEV;

	اگر (!dev->ops || !dev->ops->cache_enable)
		بازگشت SBI_ENOTSUPP;

	بازگشت dev->ops->cache_enable(dev, enable);
}
