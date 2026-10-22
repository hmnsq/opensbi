/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi_utils/cache/fdt_cache.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_driver.h>

/* List of FDT cache drivers generated at compile time */
బాహ్య స్థిరరాశి నిర్మాణం fdt_driver *స్థిరరాశి fdt_cache_drivers[];

పూర్ణాంకం fdt_cache_కూడు(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం noff, నిర్మాణం cache_device *dev)
{
	పూర్ణాంకం rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, ఖాళీ), పరిమాణం(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_తదుపరి_cache_get(fdt, noff, &dev->తదుపరి);
	ఒకవేళ (rc == SBI_ENOENT)
		dev->తదుపరి = ఖాళీ;
	లేకపోతే ఒకవేళ (rc)
		తిరిగి rc;

	తిరిగి cache_కూడు(dev);
}

స్థిర పూర్ణాంకం fdt_cache_కూడు_generic(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం noff)
{
	నిర్మాణం cache_device *dev;
	పూర్ణాంకం rc;

	dev = sbi_zalloc(పరిమాణం(*dev));
	ఒకవేళ (!dev)
		తిరిగి SBI_ENOMEM;

	rc = fdt_cache_కూడు(fdt, noff, dev);
	ఒకవేళ (rc) {
		sbi_free(dev);
		తిరిగి rc;
	}

	తిరిగి 0;
}

స్థిర పూర్ణాంకం fdt_cache_కనుగొను(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం noff, నిర్మాణం cache_device **బహిర్గతం_dev)
{
	నిర్మాణం cache_device *dev = cache_కనుగొను(noff);
	పూర్ణాంకం rc;

	ఒకవేళ (!dev) {
		rc = fdt_driver_init_by_స్థానభ్రంశం(fdt, noff, fdt_cache_drivers);
		ఒకవేళ (rc == SBI_ENODEV)
			rc = fdt_cache_కూడు_generic(fdt, noff);
		ఒకవేళ (rc)
			తిరిగి rc;

		dev = cache_కనుగొను(noff);
		ఒకవేళ (!dev)
			తిరిగి SBI_EFAIL;
	}

	ఒకవేళ (బహిర్గతం_dev)
		*బహిర్గతం_dev = dev;

	తిరిగి SBI_OK;
}

పూర్ణాంకం fdt_తదుపరి_cache_get(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం noff, నిర్మాణం cache_device **బహిర్గతం_dev)
{
	స్థిరరాశి fdt32_t *val;
	పూర్ణాంకం len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	ఒకవేళ (!val || len < పరిమాణం(*val))
		తిరిగి SBI_ENOENT;

	noff = fdt_కణుపు_స్థానభ్రంశం_by_phandle(fdt, fdt32_గమ్యం_cpu(val[0]));
	ఒకవేళ (noff < 0)
		తిరిగి noff;

	తిరిగి fdt_cache_కనుగొను(fdt, noff, బహిర్గతం_dev);
}
