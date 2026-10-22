/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/cache/cache.h>

ಸ್ಥಿರ SBI_LIST_HEAD(cache_list);

ಸಂರಚನಾ cache_device *cache_find(u32 id)
{
	ಸಂರಚನಾ cache_device *dev;

	sbi_list_for_each_entry(dev, &cache_list, ಗಂಟು) {
		ಯದಿ (dev->id == id)
			ಪ್ರತ್ಯಾಗಮ dev;
	}

	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಪೂರ್ಣಾಙ್ಕ cache_add(ಸಂರಚನಾ cache_device *dev)
{
	ಯದಿ (!dev)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	ಯದಿ (cache_find(dev->id))
		ಪ್ರತ್ಯಾಗಮ SBI_EALREADY;

	sbi_list_add(&dev->ಗಂಟು, &cache_list);

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಪೂರ್ಣಾಙ್ಕ cache_flush_all(ಸಂರಚನಾ cache_device *dev)
{
	ಯದಿ (!dev)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	ಯದಿ (!dev->ops || !dev->ops->cache_flush_all)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;

	ಪ್ರತ್ಯಾಗಮ dev->ops->cache_flush_all(dev);
}

ಪೂರ್ಣಾಙ್ಕ cache_enable(ಸಂರಚನಾ cache_device *dev, bool enable)
{
	ಯದಿ (!dev)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	ಯದಿ (!dev->ops || !dev->ops->cache_enable)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;

	ಪ್ರತ್ಯಾಗಮ dev->ops->cache_enable(dev, enable);
}
