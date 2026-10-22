/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/cache/cache.h>

ಸ್ಥಿರ SBI_LIST_ತಲೆ(cache_list);

ರಚನೆ cache_device *cache_ಕಂಡುಹಿಡಿ(u32 id)
{
	ರಚನೆ cache_device *dev;

	sbi_list_for_each_ನಮೂದು(dev, &cache_list, ಗಂಟು) {
		ಆದರೆ (dev->id == id)
			ಹಿಂತಿರುಗಿ dev;
	}

	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಪೂರ್ಣಾಂಕ cache_ಕೂಡಿಸು(ರಚನೆ cache_device *dev)
{
	ಆದರೆ (!dev)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	ಆದರೆ (cache_ಕಂಡುಹಿಡಿ(dev->id))
		ಹಿಂತಿರುಗಿ SBI_EALREADY;

	sbi_list_ಕೂಡಿಸು(&dev->ಗಂಟು, &cache_list);

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಪೂರ್ಣಾಂಕ cache_flush_all(ರಚನೆ cache_device *dev)
{
	ಆದರೆ (!dev)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	ಆದರೆ (!dev->ops || !dev->ops->cache_flush_all)
		ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;

	ಹಿಂತಿರುಗಿ dev->ops->cache_flush_all(dev);
}

ಪೂರ್ಣಾಂಕ cache_enable(ರಚನೆ cache_device *dev, bool enable)
{
	ಆದರೆ (!dev)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	ಆದರೆ (!dev->ops || !dev->ops->cache_enable)
		ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;

	ಹಿಂತಿರುಗಿ dev->ops->cache_enable(dev, enable);
}
