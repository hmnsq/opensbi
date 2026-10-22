/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi_utils/cache/cache.h>

സ്ഥിതം SBI_LIST_HEAD(cache_list);

ഘടന cache_device *cache_find(u32 id)
{
	ഘടന cache_device *dev;

	sbi_list_for_each_entry(dev, &cache_list, കണ്ണി) {
		എങ്കിൽ (dev->id == id)
			തിരികെ dev;
	}

	തിരികെ ശൂന്യ;
}

പൂർണ്ണസംഖ്യ cache_add(ഘടന cache_device *dev)
{
	എങ്കിൽ (!dev)
		തിരികെ SBI_ENODEV;

	എങ്കിൽ (cache_find(dev->id))
		തിരികെ SBI_EALREADY;

	sbi_list_add(&dev->കണ്ണി, &cache_list);

	തിരികെ SBI_OK;
}

പൂർണ്ണസംഖ്യ cache_flush_all(ഘടന cache_device *dev)
{
	എങ്കിൽ (!dev)
		തിരികെ SBI_ENODEV;

	എങ്കിൽ (!dev->ops || !dev->ops->cache_flush_all)
		തിരികെ SBI_ENOTSUPP;

	തിരികെ dev->ops->cache_flush_all(dev);
}

പൂർണ്ണസംഖ്യ cache_enable(ഘടന cache_device *dev, bool enable)
{
	എങ്കിൽ (!dev)
		തിരികെ SBI_ENODEV;

	എങ്കിൽ (!dev->ops || !dev->ops->cache_enable)
		തിരികെ SBI_ENOTSUPP;

	തിരികെ dev->ops->cache_enable(dev, enable);
}
