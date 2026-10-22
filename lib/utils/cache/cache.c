/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#include <sbi/sbi_error.h>
#include <sbi_utils/cache/cache.h>

static SBI_LIST_HEAD(cache_list);

sa_nracanaa cache_device *cache_find(u32 id)
{
	sa_nracanaa cache_device *dev;

	sbi_list_for_each_entry(dev, &cache_list, node) {
		ᱡᱩᱫᱤ (dev->id == id)
			ᱨᱩᱣᱟᱹᱲ dev;
	}

	ᱨᱩᱣᱟᱹᱲ NULL;
}

puur_naa_nka cache_add(sa_nracanaa cache_device *dev)
{
	ᱡᱩᱫᱤ (!dev)
		ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;

	ᱡᱩᱫᱤ (cache_find(dev->id))
		ᱨᱩᱣᱟᱹᱲ SBI_EALREADY;

	sbi_list_add(&dev->node, &cache_list);

	ᱨᱩᱣᱟᱹᱲ SBI_OK;
}

puur_naa_nka cache_flush_all(sa_nracanaa cache_device *dev)
{
	ᱡᱩᱫᱤ (!dev)
		ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;

	ᱡᱩᱫᱤ (!dev->ops || !dev->ops->cache_flush_all)
		ᱨᱩᱣᱟᱹᱲ SBI_ENOTSUPP;

	ᱨᱩᱣᱟᱹᱲ dev->ops->cache_flush_all(dev);
}

puur_naa_nka cache_enable(sa_nracanaa cache_device *dev, bool enable)
{
	ᱡᱩᱫᱤ (!dev)
		ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;

	ᱡᱩᱫᱤ (!dev->ops || !dev->ops->cache_enable)
		ᱨᱩᱣᱟᱹᱲ SBI_ENOTSUPP;

	ᱨᱩᱣᱟᱹᱲ dev->ops->cache_enable(dev, enable);
}
