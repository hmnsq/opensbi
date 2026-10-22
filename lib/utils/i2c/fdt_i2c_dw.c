/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 starfivetech.com
 *
 * Authors:
 *   Minda Chen <minda.chen@starfivetech.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/i2c/dw_i2c.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/i2c/fdt_i2c.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_dw_i2c_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
			   ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ dw_i2c_adapter *adapter;
	u64 addr;

	adapter = sbi_zalloc(ಪರಿಮಾಣ(*adapter));
	ಯದಿ (!adapter)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, 0, &addr, ರಿಕ್ತ);
	ಯದಿ (rc) {
		sbi_free(adapter);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	adapter->addr = addr;

	rc = dw_i2c_init(&adapter->adapter, nodeoff);
	ಯದಿ (rc) {
		sbi_free(adapter);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match fdt_dw_i2c_match[] = {
	{ .compatible = "snps,designware-i2c" },
	{ .compatible = "starfive,jh7110-i2c" },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_i2c_adapter_dw = {
	.match_table = fdt_dw_i2c_match,
	.init = fdt_dw_i2c_init,
};
