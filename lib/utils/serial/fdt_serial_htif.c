/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/serial/fdt_serial.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/sys/htif.h>

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match serial_htif_match[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା serial_htif_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
			    ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	ଯଦି (!fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, 0, &fromhost_addr, ଖାଲି)) {
		custom = true;
		tohost_addr = fromhost_addr + ଆକାର(uint64_t);
	}

	fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, 1, &tohost_addr, ଖାଲି);

	rc = sbi_domain_ମୂଳ_add_memrange(fromhost_addr, PAGE_SIZE, PAGE_SIZE,
					  (SBI_DOMAIN_MEMREGION_MMIO |
					   SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	ଫେରନ୍ତୁ htif_serial_init(custom, fromhost_addr, tohost_addr);
}

ଅଚଳ ସଂରଚନା fdt_driver fdt_serial_htif = {
	.match_table = serial_htif_match,
	.init = serial_htif_init
};
