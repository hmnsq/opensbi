/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ضمن <libfdt.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi_utils/cache/fdt_cache.h>
#ضمن <sbi_utils/fdt/fdt_driver.h>

/* List of FDT cache drivers generated at compile time */
خارجي ثابت بنية fdt_driver *ثابت fdt_cache_drivers[];

صحيح fdt_cache_جمع_کن(ثابت فراغ *fdt, صحيح noff, بنية cache_device *dev)
{
	صحيح rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, عدم), حجم(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_بعدی_cache_get(fdt, noff, &dev->بعدی);
	إذا (rc == SBI_ENOENT)
		dev->بعدی = عدم;
	وإلا إذا (rc)
		أرجع rc;

	أرجع cache_جمع_کن(dev);
}

ساكن صحيح fdt_cache_جمع_کن_generic(ثابت فراغ *fdt, صحيح noff)
{
	بنية cache_device *dev;
	صحيح rc;

	dev = sbi_zalloc(حجم(*dev));
	إذا (!dev)
		أرجع SBI_ENOMEM;

	rc = fdt_cache_جمع_کن(fdt, noff, dev);
	إذا (rc) {
		sbi_free(dev);
		أرجع rc;
	}

	أرجع 0;
}

ساكن صحيح fdt_cache_پیدا_کن(ثابت فراغ *fdt, صحيح noff, بنية cache_device **بیرون_dev)
{
	بنية cache_device *dev = cache_پیدا_کن(noff);
	صحيح rc;

	إذا (!dev) {
		rc = fdt_driver_init_by_جابه_جایی(fdt, noff, fdt_cache_drivers);
		إذا (rc == SBI_ENODEV)
			rc = fdt_cache_جمع_کن_generic(fdt, noff);
		إذا (rc)
			أرجع rc;

		dev = cache_پیدا_کن(noff);
		إذا (!dev)
			أرجع SBI_EFAIL;
	}

	إذا (بیرون_dev)
		*بیرون_dev = dev;

	أرجع SBI_OK;
}

صحيح fdt_بعدی_cache_get(ثابت فراغ *fdt, صحيح noff, بنية cache_device **بیرون_dev)
{
	ثابت fdt32_t *val;
	صحيح len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	إذا (!val || len < حجم(*val))
		أرجع SBI_ENOENT;

	noff = fdt_گره_جابه_جایی_by_phandle(fdt, fdt32_مقصد_cpu(val[0]));
	إذا (noff < 0)
		أرجع noff;

	أرجع fdt_cache_پیدا_کن(fdt, noff, بیرون_dev);
}
