/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi_utils/cache/cache.h>

स्थिर SBI_LIST_HEAD(cache_list);

संरचना cache_device *cache_find(u32 id)
{
	संरचना cache_device *dev;

	sbi_list_for_each_entry(dev, &cache_list, गाठ) {
		जर (dev->id == id)
			परत dev;
	}

	परत रिक्त;
}

पूर्णांक cache_add(संरचना cache_device *dev)
{
	जर (!dev)
		परत SBI_ENODEV;

	जर (cache_find(dev->id))
		परत SBI_EALREADY;

	sbi_list_add(&dev->गाठ, &cache_list);

	परत SBI_OK;
}

पूर्णांक cache_flush_all(संरचना cache_device *dev)
{
	जर (!dev)
		परत SBI_ENODEV;

	जर (!dev->ops || !dev->ops->cache_flush_all)
		परत SBI_ENOTSUPP;

	परत dev->ops->cache_flush_all(dev);
}

पूर्णांक cache_enable(संरचना cache_device *dev, bool enable)
{
	जर (!dev)
		परत SBI_ENODEV;

	जर (!dev->ops || !dev->ops->cache_enable)
		परत SBI_ENOTSUPP;

	परत dev->ops->cache_enable(dev, enable);
}
