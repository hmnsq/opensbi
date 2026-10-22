/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_driver.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/sys/htif.h>

ਸਥਿਰ ਪੂਰਨ htif_reset_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			   ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	ਜੇ (!fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, 0, &fromhost_addr, ਨਿਰਾ)) {
		custom = true;
		tohost_addr = fromhost_addr + ਆਕਾਰ(uint64_t);
	}

	fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, 1, &tohost_addr, ਨਿਰਾ);

	ਵਾਪਸ htif_system_reset_init(custom, fromhost_addr, tohost_addr);
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ htif_reset_ਮੇਲ[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_reset_htif = {
	.ਮੇਲ_ਸਾਰਣੀ = htif_reset_ਮੇਲ,
	.init = htif_reset_init
};
