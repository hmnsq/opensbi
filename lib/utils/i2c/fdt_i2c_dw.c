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

स्थिर पूर्णांक fdt_dw_i2c_init(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
			   स्थिरांक संरचना fdt_match *match)
{
	पूर्णांक rc;
	संरचना dw_i2c_adapter *adapter;
	u64 addr;

	adapter = sbi_zalloc(आकार(*adapter));
	जर (!adapter)
		परत SBI_ENOMEM;

	rc = fdt_get_गाठ_addr_size(fdt, nodeoff, 0, &addr, रिक्त);
	जर (rc) {
		sbi_free(adapter);
		परत rc;
	}

	adapter->addr = addr;

	rc = dw_i2c_init(&adapter->adapter, nodeoff);
	जर (rc) {
		sbi_free(adapter);
		परत rc;
	}

	परत 0;
}

स्थिर स्थिरांक संरचना fdt_match fdt_dw_i2c_match[] = {
	{ .compatible = "snps,designware-i2c" },
	{ .compatible = "starfive,jh7110-i2c" },
	{ },
};

स्थिरांक संरचना fdt_driver fdt_i2c_adapter_dw = {
	.match_table = fdt_dw_i2c_match,
	.init = fdt_dw_i2c_init,
};
