/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 starfivetech.com
 *
 * Authors:
 *   Minda Chen <minda.chen@starfivetech.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_string.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/i2c/dw_i2c.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/i2c/fdt_i2c.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_dw_i2c_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
			   ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା dw_i2c_adapter *adapter;
	u64 addr;

	adapter = sbi_zalloc(ଆକାର(*adapter));
	ଯଦି (!adapter)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, 0, &addr, ଖାଲି);
	ଯଦି (rc) {
		sbi_free(adapter);
		ଫେରନ୍ତୁ rc;
	}

	adapter->addr = addr;

	rc = dw_i2c_init(&adapter->adapter, nodeoff);
	ଯଦି (rc) {
		sbi_free(adapter);
		ଫେରନ୍ତୁ rc;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match fdt_dw_i2c_match[] = {
	{ .compatible = "snps,designware-i2c" },
	{ .compatible = "starfive,jh7110-i2c" },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_i2c_adapter_dw = {
	.match_table = fdt_dw_i2c_match,
	.init = fdt_dw_i2c_init,
};
