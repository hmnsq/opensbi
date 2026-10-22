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
		यदि (dev->id == id)
			प्रत्यागम dev;
	}

	प्रत्यागम रिक्त;
}

पूर्णाङ्क cache_add(संरचना cache_device *dev)
{
	यदि (!dev)
		प्रत्यागम SBI_ENODEV;

	यदि (cache_find(dev->id))
		प्रत्यागम SBI_EALREADY;

	sbi_list_add(&dev->गाठ, &cache_list);

	प्रत्यागम SBI_OK;
}

पूर्णाङ्क cache_flush_all(संरचना cache_device *dev)
{
	यदि (!dev)
		प्रत्यागम SBI_ENODEV;

	यदि (!dev->ops || !dev->ops->cache_flush_all)
		प्रत्यागम SBI_ENOTSUPP;

	प्रत्यागम dev->ops->cache_flush_all(dev);
}

पूर्णाङ्क cache_enable(संरचना cache_device *dev, bool enable)
{
	यदि (!dev)
		प्रत्यागम SBI_ENODEV;

	यदि (!dev->ops || !dev->ops->cache_enable)
		प्रत्यागम SBI_ENOTSUPP;

	प्रत्यागम dev->ops->cache_enable(dev, enable);
}
