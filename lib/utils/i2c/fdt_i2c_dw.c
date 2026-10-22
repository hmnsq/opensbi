/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 starfivetech.com
 *
 * Authors:
 *   Minda Chen <minda.chen@starfivetech.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/i2c/dw_i2c.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/i2c/fdt_i2c.h>

ਸਥਿਰ ਪੂਰਨ fdt_dw_i2c_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			   ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ dw_i2c_adapter *adapter;
	u64 addr;

	adapter = sbi_zalloc(ਆਕਾਰ(*adapter));
	ਜੇ (!adapter)
		ਵਾਪਸ SBI_ENOMEM;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, 0, &addr, ਨਿਰਾ);
	ਜੇ (rc) {
		sbi_free(adapter);
		ਵਾਪਸ rc;
	}

	adapter->addr = addr;

	rc = dw_i2c_init(&adapter->adapter, nodeoff);
	ਜੇ (rc) {
		sbi_free(adapter);
		ਵਾਪਸ rc;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ fdt_dw_i2c_ਮੇਲ[] = {
	{ .compatible = "snps,designware-i2c" },
	{ .compatible = "starfive,jh7110-i2c" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_i2c_adapter_dw = {
	.ਮੇਲ_ਸਾਰਣੀ = fdt_dw_i2c_ਮੇਲ,
	.init = fdt_dw_i2c_init,
};
