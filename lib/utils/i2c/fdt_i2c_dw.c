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

स्थिर पूर्णाङ्क fdt_dw_i2c_init(अचर शून्य *fdt, पूर्णाङ्क nodeoff,
			   अचर संरचना fdt_match *match)
{
	पूर्णाङ्क rc;
	संरचना dw_i2c_adapter *adapter;
	u64 addr;

	adapter = sbi_zalloc(परिमाण(*adapter));
	यदि (!adapter)
		फर्काऊ SBI_ENOMEM;

	rc = fdt_get_गाँठो_addr_size(fdt, nodeoff, 0, &addr, रिक्त);
	यदि (rc) {
		sbi_free(adapter);
		फर्काऊ rc;
	}

	adapter->addr = addr;

	rc = dw_i2c_init(&adapter->adapter, nodeoff);
	यदि (rc) {
		sbi_free(adapter);
		फर्काऊ rc;
	}

	फर्काऊ 0;
}

स्थिर अचर संरचना fdt_match fdt_dw_i2c_match[] = {
	{ .compatible = "snps,designware-i2c" },
	{ .compatible = "starfive,jh7110-i2c" },
	{ },
};

अचर संरचना fdt_driver fdt_i2c_adapter_dw = {
	.match_table = fdt_dw_i2c_match,
	.init = fdt_dw_i2c_init,
};
