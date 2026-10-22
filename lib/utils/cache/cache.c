/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ضمن <sbi/sbi_error.h>
#ضمن <sbi_utils/cache/cache.h>

ساكن SBI_LIST_HEAD(cache_list);

بنية cache_device *cache_find(u32 id)
{
	بنية cache_device *dev;

	sbi_list_for_each_entry(dev, &cache_list, node) {
		𐡄𐡍 (dev->id == id)
			أرجع dev;
	}

	أرجع عدم;
}

صحيح cache_add(بنية cache_device *dev)
{
	𐡄𐡍 (!dev)
		أرجع SBI_ENODEV;

	𐡄𐡍 (cache_find(dev->id))
		أرجع SBI_EALREADY;

	sbi_list_add(&dev->node, &cache_list);

	أرجع SBI_OK;
}

صحيح cache_flush_all(بنية cache_device *dev)
{
	𐡄𐡍 (!dev)
		أرجع SBI_ENODEV;

	𐡄𐡍 (!dev->ops || !dev->ops->cache_flush_all)
		أرجع SBI_ENOTSUPP;

	أرجع dev->ops->cache_flush_all(dev);
}

صحيح cache_enable(بنية cache_device *dev, bool enable)
{
	𐡄𐡍 (!dev)
		أرجع SBI_ENODEV;

	𐡄𐡍 (!dev->ops || !dev->ops->cache_enable)
		أرجع SBI_ENOTSUPP;

	أرجع dev->ops->cache_enable(dev, enable);
}
