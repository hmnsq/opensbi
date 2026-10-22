/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/fdt_serial.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/sys/htif.h>

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ serial_htif_ਮੇਲ[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

ਸਥਿਰ ਪੂਰਨ serial_htif_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			    ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ rc;
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	ਜੇ (!fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, 0, &fromhost_addr, ਨਿਰਾ)) {
		custom = true;
		tohost_addr = fromhost_addr + ਆਕਾਰ(uint64_t);
	}

	fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, 1, &tohost_addr, ਨਿਰਾ);

	rc = sbi_domain_ਜੜ੍ਹ_ਜੋੜੋ_memrange(fromhost_addr, ਪੰਨਾ_ਮਾਪ, ਪੰਨਾ_ਮਾਪ,
					  (SBI_DOMAIN_MEMREGION_MMIO |
					   SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	ਜੇ (rc)
		ਵਾਪਸ rc;

	ਵਾਪਸ htif_serial_init(custom, fromhost_addr, tohost_addr);
}

ਅਚਲ ਬਣਤਰ fdt_driver fdt_serial_htif = {
	.ਮੇਲ_ਸਾਰਣੀ = serial_htif_ਮੇਲ,
	.init = serial_htif_init
};
