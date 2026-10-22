/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 starfivetech.com
 *
 * Authors:
 *   Minda Chen <minda.chen@starfivetech.com>
 */

#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi/sbi_string.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/i2c/dw_i2c.h>
#समावेश <sbi_utils/i2c/fdt_i2c.h>

अटल पूर्णांक fdt_dw_i2c_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			   स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक rc;
	काष्ठा dw_i2c_adapter *adapter;
	u64 addr;

	adapter = sbi_zalloc(माप(*adapter));
	अगर (!adapter)
		वापस SBI_ENOMEM;

	rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoff, 0, &addr, शून्य);
	अगर (rc) {
		sbi_free(adapter);
		वापस rc;
	}

	adapter->addr = addr;

	rc = dw_i2c_init(&adapter->adapter, nodeoff);
	अगर (rc) {
		sbi_free(adapter);
		वापस rc;
	}

	वापस 0;
}

अटल स्थिर काष्ठा fdt_match fdt_dw_i2c_match[] = {
	{ .compatible = "snps,designware-i2c" },
	{ .compatible = "starfive,jh7110-i2c" },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_i2c_adapter_dw = {
	.match_table = fdt_dw_i2c_match,
	.init = fdt_dw_i2c_init,
};
