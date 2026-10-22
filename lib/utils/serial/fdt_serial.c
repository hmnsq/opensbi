/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/serial/fdt_serial.h>

/* List of FDT serial drivers generated at compile time */
బాహ్య స్థిరరాశి నిర్మాణం fdt_driver *స్థిరరాశి fdt_serial_drivers[];

పూర్ణాంకం fdt_serial_init(స్థిరరాశి శూన్య *fdt)
{
	స్థిరరాశి శూన్య *prop;
	పూర్ణాంకం noff = -1, len, coff, rc;

	/* Find offset of node pointed to by stdout-path */
	coff = fdt_మార్గం_స్థానభ్రంశం(fdt, "/chosen");
	ఒకవేళ (-1 < coff) {
		prop = fdt_getprop(fdt, coff, "stdout-path", &len);
		ఒకవేళ (prop && len) {
			స్థిరరాశి అక్షరం *sep, *ప్రారంభం = prop;

			/* The device path may be followed by ':' */
			sep = strchr(ప్రారంభం, ':');
			ఒకవేళ (sep)
				noff = fdt_మార్గం_స్థానభ్రంశం_namelen(fdt, prop,
							       sep - ప్రారంభం);
			లేకపోతే
				noff = fdt_మార్గం_స్థానభ్రంశం(fdt, prop);
		}
	}

	/* First check DT node pointed by stdout-path */
	ఒకవేళ (-1 < noff) {
		rc = fdt_driver_init_by_స్థానభ్రంశం(fdt, noff, fdt_serial_drivers);
		ఒకవేళ (rc != SBI_ENODEV)
			తిరిగి rc;
	}

	/* Lastly check all DT nodes */
	తిరిగి fdt_driver_init_one(fdt, fdt_serial_drivers);
}
