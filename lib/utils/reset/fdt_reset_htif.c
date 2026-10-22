/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi_utils/fdt/fdt_driver.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/sys/htif.h>

अटल पूर्णांक htif_reset_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			   स्थिर काष्ठा fdt_match *match)
{
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	अगर (!fdt_get_गाँठ_addr_आकार(fdt, nodeoff, 0, &fromhost_addr, शून्य)) {
		custom = true;
		tohost_addr = fromhost_addr + माप(uint64_t);
	}

	fdt_get_गाँठ_addr_आकार(fdt, nodeoff, 1, &tohost_addr, शून्य);

	वापस htif_system_reset_init(custom, fromhost_addr, tohost_addr);
}

अटल स्थिर काष्ठा fdt_match htif_reset_match[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_reset_htif = {
	.match_table = htif_reset_match,
	.init = htif_reset_init
};
