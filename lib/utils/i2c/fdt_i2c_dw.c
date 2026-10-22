/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 starfivetech.com
 *
 * Authors:
 *   Minda Chen <minda.chen@starfivetech.com>
 */

#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi/sbi_string.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/i2c/dw_i2c.h>
#అన్తర్భావ <sbi_utils/i2c/fdt_i2c.h>

స్థిర పూర్ణాంకం fdt_dw_i2c_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			   స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం rc;
	నిర్మాణం dw_i2c_adapter *adapter;
	u64 addr;

	adapter = sbi_zalloc(పరిమాణం(*adapter));
	ఒకవేళ (!adapter)
		తిరిగి SBI_ENOMEM;

	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, 0, &addr, ఖాళీ);
	ఒకవేళ (rc) {
		sbi_free(adapter);
		తిరిగి rc;
	}

	adapter->addr = addr;

	rc = dw_i2c_init(&adapter->adapter, nodeoff);
	ఒకవేళ (rc) {
		sbi_free(adapter);
		తిరిగి rc;
	}

	తిరిగి 0;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక fdt_dw_i2c_సరిపోలిక[] = {
	{ .compatible = "snps,designware-i2c" },
	{ .compatible = "starfive,jh7110-i2c" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_i2c_adapter_dw = {
	.సరిపోలిక_పట్టిక = fdt_dw_i2c_సరిపోలిక,
	.init = fdt_dw_i2c_init,
};
