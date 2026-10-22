/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#समावेश <sbi/sbi_error.h>
#समावेश <sbi_utils/cache/cache.h>

अटल SBI_LIST_शीर्ष(cache_list);

काष्ठा cache_device *cache_find(u32 id)
{
	काष्ठा cache_device *dev;

	sbi_list_for_each_entry(dev, &cache_list, गाँठ) {
		अगर (dev->id == id)
			वापस dev;
	}

	वापस शून्य;
}

पूर्णांक cache_add(काष्ठा cache_device *dev)
{
	अगर (!dev)
		वापस SBI_ENODEV;

	अगर (cache_find(dev->id))
		वापस SBI_EALREADY;

	sbi_list_add(&dev->गाँठ, &cache_list);

	वापस SBI_OK;
}

पूर्णांक cache_flush_all(काष्ठा cache_device *dev)
{
	अगर (!dev)
		वापस SBI_ENODEV;

	अगर (!dev->ops || !dev->ops->cache_flush_all)
		वापस SBI_ENOTSUPP;

	वापस dev->ops->cache_flush_all(dev);
}

पूर्णांक cache_enable(काष्ठा cache_device *dev, bool enable)
{
	अगर (!dev)
		वापस SBI_ENODEV;

	अगर (!dev->ops || !dev->ops->cache_enable)
		वापस SBI_ENOTSUPP;

	वापस dev->ops->cache_enable(dev, enable);
}
