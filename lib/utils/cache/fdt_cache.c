/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#شامل <libfdt.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi_utils/cache/fdt_cache.h>
#شامل <sbi_utils/fdt/fdt_driver.h>

/* List of FDT cache drivers generated at compile time */
خارجی ثابت ساخت fdt_driver *ثابت fdt_cache_drivers[];

صحیح fdt_cache_جوڑو(ثابت خالی *fdt, صحیح noff, ساخت cache_device *dev)
{
	صحیح rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, تہی), حجم(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_اگلا_cache_get(fdt, noff, &dev->اگلا);
	اگر (rc == SBI_ENOENT)
		dev->اگلا = تہی;
	ورنہ اگر (rc)
		واپس rc;

	واپس cache_جوڑو(dev);
}

ساکن صحیح fdt_cache_جوڑو_generic(ثابت خالی *fdt, صحیح noff)
{
	ساخت cache_device *dev;
	صحیح rc;

	dev = sbi_zalloc(حجم(*dev));
	اگر (!dev)
		واپس SBI_ENOMEM;

	rc = fdt_cache_جوڑو(fdt, noff, dev);
	اگر (rc) {
		sbi_free(dev);
		واپس rc;
	}

	واپس 0;
}

ساکن صحیح fdt_cache_ڈھونڈو(ثابت خالی *fdt, صحیح noff, ساخت cache_device **برآمد_dev)
{
	ساخت cache_device *dev = cache_ڈھونڈو(noff);
	صحیح rc;

	اگر (!dev) {
		rc = fdt_driver_init_by_انحراف(fdt, noff, fdt_cache_drivers);
		اگر (rc == SBI_ENODEV)
			rc = fdt_cache_جوڑو_generic(fdt, noff);
		اگر (rc)
			واپس rc;

		dev = cache_ڈھونڈو(noff);
		اگر (!dev)
			واپس SBI_EFAIL;
	}

	اگر (برآمد_dev)
		*برآمد_dev = dev;

	واپس SBI_OK;
}

صحیح fdt_اگلا_cache_get(ثابت خالی *fdt, صحیح noff, ساخت cache_device **برآمد_dev)
{
	ثابت fdt32_t *val;
	صحیح len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	اگر (!val || len < حجم(*val))
		واپس SBI_ENOENT;

	noff = fdt_گرہ_انحراف_by_phandle(fdt, fdt32_مقصد_cpu(val[0]));
	اگر (noff < 0)
		واپس noff;

	واپس fdt_cache_ڈھونڈو(fdt, noff, برآمد_dev);
}
