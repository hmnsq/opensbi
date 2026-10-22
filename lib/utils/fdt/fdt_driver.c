/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 SiFive
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_driver.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>

పూర్ణాంకం fdt_driver_init_by_స్థానభ్రంశం(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			      స్థిరరాశి నిర్మాణం fdt_driver *స్థిరరాశి *drivers)
{
	స్థిరరాశి నిర్మాణం fdt_driver *driver;
	స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక;
	పూర్ణాంకం compat_len, prop_len, rc;
	స్థిరరాశి అక్షరం *compat_str;

	ఒకవేళ (!fdt_కణుపు_is_enabled(fdt, nodeoff))
		తిరిగి SBI_ENODEV;

	compat_str = fdt_getprop(fdt, nodeoff, "compatible", &prop_len);
	ఒకవేళ (!compat_str)
		తిరిగి SBI_ENODEV;

	అయితే ((compat_len = strnlen(compat_str, prop_len) + 1) <= prop_len) {
		కొరకు (పూర్ణాంకం i = 0; (driver = drivers[i]); i++)
			కొరకు (సరిపోలిక = driver->సరిపోలిక_పట్టిక; సరిపోలిక->compatible; సరిపోలిక++)
				ఒకవేళ (!memcmp(సరిపోలిక->compatible, compat_str, compat_len))
					వెళ్ళు దొరికింది;

		compat_str += compat_len;
		prop_len -= compat_len;
	}

	తిరిగి SBI_ENODEV;

దొరికింది:
	ఒకవేళ (driver->experimental)
		sbi_printf("WARNING: %s driver is experimental and may change\n",
			   సరిపోలిక->compatible);

	rc = driver->init(fdt, nodeoff, సరిపోలిక);
	ఒకవేళ (rc < 0) {
		స్థిరరాశి అక్షరం *name;

		name = fdt_get_name(fdt, nodeoff, ఖాళీ);
		sbi_printf("%s: %s (%s) init failed: %d\n",
			   __func__, name, సరిపోలిక->compatible, rc);
	}

	తిరిగి rc;
}

స్థిర పూర్ణాంకం fdt_driver_init_పరిశీలన(స్థిరరాశి శూన్య *fdt,
				స్థిరరాశి నిర్మాణం fdt_driver *స్థిరరాశి *drivers,
				bool one)
{
	పూర్ణాంకం nodeoff, rc;

	కొరకు (nodeoff = fdt_తదుపరి_కణుపు(fdt, -1, ఖాళీ);
	     nodeoff >= 0;
	     nodeoff = fdt_తదుపరి_కణుపు(fdt, nodeoff, ఖాళీ)) {
		rc = fdt_driver_init_by_స్థానభ్రంశం(fdt, nodeoff, drivers);
		ఒకవేళ (rc == SBI_ENODEV)
			కొనసాగు;
		ఒకవేళ (rc < 0)
			తిరిగి rc;
		ఒకవేళ (one)
			తిరిగి 0;
	}

	తిరిగి one ? SBI_ENODEV : 0;
}

పూర్ణాంకం fdt_driver_init_all(స్థిరరాశి శూన్య *fdt,
			స్థిరరాశి నిర్మాణం fdt_driver *స్థిరరాశి *drivers)
{
	తిరిగి fdt_driver_init_పరిశీలన(fdt, drivers, false);
}

పూర్ణాంకం fdt_driver_init_one(స్థిరరాశి శూన్య *fdt,
			స్థిరరాశి నిర్మాణం fdt_driver *స్థిరరాశి *drivers)
{
	తిరిగి fdt_driver_init_పరిశీలన(fdt, drivers, true);
}
