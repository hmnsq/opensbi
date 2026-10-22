/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 starfivetech.com
 *
 * Authors:
 *   Minda Chen <minda.chen@starfivetech.com>
 */

#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi/sbi_string.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/i2c/dw_i2c.h>
#அந்தர்பாவ <sbi_utils/i2c/fdt_i2c.h>

நிலையான முழு fdt_dw_i2c_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
			   மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு rc;
	கட்டமைப்பு dw_i2c_adapter *adapter;
	u64 addr;

	adapter = sbi_zalloc(அளவு(*adapter));
	எனில் (!adapter)
		திரும்பு SBI_ENOMEM;

	rc = fdt_get_முனை_addr_size(fdt, nodeoff, 0, &addr, வெற்று);
	எனில் (rc) {
		sbi_free(adapter);
		திரும்பு rc;
	}

	adapter->addr = addr;

	rc = dw_i2c_init(&adapter->adapter, nodeoff);
	எனில் (rc) {
		sbi_free(adapter);
		திரும்பு rc;
	}

	திரும்பு 0;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match fdt_dw_i2c_match[] = {
	{ .compatible = "snps,designware-i2c" },
	{ .compatible = "starfive,jh7110-i2c" },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_i2c_adapter_dw = {
	.match_table = fdt_dw_i2c_match,
	.init = fdt_dw_i2c_init,
};
