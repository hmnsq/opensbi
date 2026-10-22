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

	sbi_list_for_each_entry(dev, &cache_list, ग्रन्थिः) {
		सचे (dev->id == id)
			पटिनिवत्त dev;
	}

	पटिनिवत्त रिक्त;
}

पूर्णाङ्क cache_add(संरचना cache_device *dev)
{
	सचे (!dev)
		पटिनिवत्त SBI_ENODEV;

	सचे (cache_find(dev->id))
		पटिनिवत्त SBI_EALREADY;

	sbi_list_add(&dev->ग्रन्थिः, &cache_list);

	पटिनिवत्त SBI_OK;
}

पूर्णाङ्क cache_flush_all(संरचना cache_device *dev)
{
	सचे (!dev)
		पटिनिवत्त SBI_ENODEV;

	सचे (!dev->ops || !dev->ops->cache_flush_all)
		पटिनिवत्त SBI_ENOTSUPP;

	पटिनिवत्त dev->ops->cache_flush_all(dev);
}

पूर्णाङ्क cache_enable(संरचना cache_device *dev, bool enable)
{
	सचे (!dev)
		पटिनिवत्त SBI_ENODEV;

	सचे (!dev->ops || !dev->ops->cache_enable)
		पटिनिवत्त SBI_ENOTSUPP;

	पटिनिवत्त dev->ops->cache_enable(dev, enable);
}
