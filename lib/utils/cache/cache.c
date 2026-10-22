/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi_utils/cache/cache.h>

સ્થિર SBI_LIST_HEAD(cache_list);

સંરચના cache_device *cache_find(u32 id)
{
	સંરચના cache_device *dev;

	sbi_list_for_each_entry(dev, &cache_list, ગાંઠ) {
		જો (dev->id == id)
			પાછા dev;
	}

	પાછા ખાલી;
}

પૂર્ણાંક cache_add(સંરચના cache_device *dev)
{
	જો (!dev)
		પાછા SBI_ENODEV;

	જો (cache_find(dev->id))
		પાછા SBI_EALREADY;

	sbi_list_add(&dev->ગાંઠ, &cache_list);

	પાછા SBI_OK;
}

પૂર્ણાંક cache_flush_all(સંરચના cache_device *dev)
{
	જો (!dev)
		પાછા SBI_ENODEV;

	જો (!dev->ops || !dev->ops->cache_flush_all)
		પાછા SBI_ENOTSUPP;

	પાછા dev->ops->cache_flush_all(dev);
}

પૂર્ણાંક cache_enable(સંરચના cache_device *dev, bool enable)
{
	જો (!dev)
		પાછા SBI_ENODEV;

	જો (!dev->ops || !dev->ops->cache_enable)
		પાછા SBI_ENOTSUPP;

	પાછા dev->ops->cache_enable(dev, enable);
}
