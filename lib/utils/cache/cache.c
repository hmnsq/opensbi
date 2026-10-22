/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi_utils/cache/cache.h>

స్థిర SBI_LIST_తల(cache_list);

నిర్మాణం cache_device *cache_కనుగొను(u32 id)
{
	నిర్మాణం cache_device *dev;

	sbi_list_for_each_నమోదు(dev, &cache_list, కణుపు) {
		ఒకవేళ (dev->id == id)
			తిరిగి dev;
	}

	తిరిగి ఖాళీ;
}

పూర్ణాంకం cache_కూడు(నిర్మాణం cache_device *dev)
{
	ఒకవేళ (!dev)
		తిరిగి SBI_ENODEV;

	ఒకవేళ (cache_కనుగొను(dev->id))
		తిరిగి SBI_EALREADY;

	sbi_list_కూడు(&dev->కణుపు, &cache_list);

	తిరిగి SBI_OK;
}

పూర్ణాంకం cache_flush_all(నిర్మాణం cache_device *dev)
{
	ఒకవేళ (!dev)
		తిరిగి SBI_ENODEV;

	ఒకవేళ (!dev->ops || !dev->ops->cache_flush_all)
		తిరిగి SBI_ENOTSUPP;

	తిరిగి dev->ops->cache_flush_all(dev);
}

పూర్ణాంకం cache_enable(నిర్మాణం cache_device *dev, bool enable)
{
	ఒకవేళ (!dev)
		తిరిగి SBI_ENODEV;

	ఒకవేళ (!dev->ops || !dev->ops->cache_enable)
		తిరిగి SBI_ENOTSUPP;

	తిరిగి dev->ops->cache_enable(dev, enable);
}
