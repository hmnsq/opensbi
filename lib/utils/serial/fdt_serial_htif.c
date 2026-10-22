/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/serial/fdt_serial.h>
#समावेश <sbi_utils/sys/htif.h>

अटल स्थिर काष्ठा fdt_match serial_htif_match[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

अटल पूर्णांक serial_htif_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			    स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक rc;
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	अगर (!fdt_get_गाँठ_addr_आकार(fdt, nodeoff, 0, &fromhost_addr, शून्य)) {
		custom = true;
		tohost_addr = fromhost_addr + माप(uint64_t);
	}

	fdt_get_गाँठ_addr_आकार(fdt, nodeoff, 1, &tohost_addr, शून्य);

	rc = sbi_domain_मूल_add_memrange(fromhost_addr, PAGE_आकार, PAGE_आकार,
					  (SBI_DOMAIN_MEMREGION_MMIO |
					   SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	अगर (rc)
		वापस rc;

	वापस htif_serial_init(custom, fromhost_addr, tohost_addr);
}

स्थिर काष्ठा fdt_driver fdt_serial_htif = {
	.match_table = serial_htif_match,
	.init = serial_htif_init
};
