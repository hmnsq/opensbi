/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/sys/htif.h>

स्थिर पूर्णाङ्क htif_reset_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			   ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	यदि (!fdt_get_ग्रन्थिः_addr_आकार(fdt, nodeoff, 0, &fromhost_addr, रिक्त)) {
		custom = true;
		tohost_addr = fromhost_addr + परिमाण(uint64_t);
	}

	fdt_get_ग्रन्थिः_addr_आकार(fdt, nodeoff, 1, &tohost_addr, रिक्त);

	प्रत्यागम htif_system_reset_init(custom, fromhost_addr, tohost_addr);
}

स्थिर ध्रुव संरचना fdt_मेलनम् htif_reset_मेलनम्[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_reset_htif = {
	.मेलनम्_सारणी = htif_reset_मेलनम्,
	.init = htif_reset_init
};
