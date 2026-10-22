/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 starfivetech.com
 *
 * Authors:
 *   Minda Chen <minda.chen@starfivetech.com>
 */

#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_string.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/i2c/dw_i2c.h>
#अन्तर्भाव <sbi_utils/i2c/fdt_i2c.h>

स्थिर पूर्णाङ्क fdt_dw_i2c_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			   ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	पूर्णाङ्क rc;
	संरचना dw_i2c_adapter *adapter;
	u64 addr;

	adapter = sbi_zalloc(परिमाण(*adapter));
	यदि (!adapter)
		प्रत्यागम SBI_ENOMEM;

	rc = fdt_get_ग्रन्थिः_addr_आकार(fdt, nodeoff, 0, &addr, रिक्त);
	यदि (rc) {
		sbi_free(adapter);
		प्रत्यागम rc;
	}

	adapter->addr = addr;

	rc = dw_i2c_init(&adapter->adapter, nodeoff);
	यदि (rc) {
		sbi_free(adapter);
		प्रत्यागम rc;
	}

	प्रत्यागम 0;
}

स्थिर ध्रुव संरचना fdt_मेलनम् fdt_dw_i2c_मेलनम्[] = {
	{ .compatible = "snps,designware-i2c" },
	{ .compatible = "starfive,jh7110-i2c" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_i2c_adapter_dw = {
	.मेलनम्_सारणी = fdt_dw_i2c_मेलनम्,
	.init = fdt_dw_i2c_init,
};
