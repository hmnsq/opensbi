/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#شامل <sbi/sbi_error.h>
#شامل <sbi_utils/cache/cache.h>

ساکن SBI_LIST_سر(cache_list);

ساخت cache_device *cache_ڈھونڈو(u32 id)
{
	ساخت cache_device *dev;

	sbi_list_for_each_اندراج(dev, &cache_list, گرہ) {
		اگر (dev->id == id)
			واپس dev;
	}

	واپس تہی;
}

صحیح cache_جوڑو(ساخت cache_device *dev)
{
	اگر (!dev)
		واپس SBI_ENODEV;

	اگر (cache_ڈھونڈو(dev->id))
		واپس SBI_EALREADY;

	sbi_list_جوڑو(&dev->گرہ, &cache_list);

	واپس SBI_OK;
}

صحیح cache_flush_all(ساخت cache_device *dev)
{
	اگر (!dev)
		واپس SBI_ENODEV;

	اگر (!dev->ops || !dev->ops->cache_flush_all)
		واپس SBI_ENOTSUPP;

	واپس dev->ops->cache_flush_all(dev);
}

صحیح cache_enable(ساخت cache_device *dev, bool enable)
{
	اگر (!dev)
		واپس SBI_ENODEV;

	اگر (!dev->ops || !dev->ops->cache_enable)
		واپس SBI_ENOTSUPP;

	واپس dev->ops->cache_enable(dev, enable);
}
