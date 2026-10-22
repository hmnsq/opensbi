/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_driver.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/ipi/aclint_mswi.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ ipi_mswi_cold_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			      സ്ഥിരം ഘടന fdt_match *match)
{
	പൂർണ്ണസംഖ്യ rc;
	unsigned long offset;
	ഘടന aclint_mswi_data *ms;

	ms = sbi_zalloc(വലുപ്പം(*ms));
	എങ്കിൽ (!ms)
		തിരികെ SBI_ENOMEM;

	rc = fdt_parse_aclint_കണ്ണി(fdt, nodeoff, false, false,
				   &ms->addr, &ms->size, ശൂന്യ, ശൂന്യ,
				   &ms->first_hartid, &ms->hart_എണ്ണം);
	എങ്കിൽ (rc) {
		sbi_free(ms);
		തിരികെ rc;
	}

	എങ്കിൽ (match->data) {
		/* Adjust MSWI address and size for CLINT device */
		offset = *((unsigned long *)match->data);
		ms->addr += offset;
		എങ്കിൽ ((ms->size - offset) < ACLINT_MSWI_SIZE)
			തിരികെ SBI_EINVAL;
		ms->size = ACLINT_MSWI_SIZE;
	}

	rc = aclint_mswi_cold_init(ms);
	എങ്കിൽ (rc) {
		sbi_free(ms);
		തിരികെ rc;
	}

	തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം unsigned long clint_offset = CLINT_MSWI_OFFSET;

സ്ഥിതം സ്ഥിരം ഘടന fdt_match ipi_mswi_match[] = {
	{ .compatible = "riscv,clint0", .data = &clint_offset },
	{ .compatible = "sifive,clint0", .data = &clint_offset },
	{ .compatible = "thead,c900-clint", .data = &clint_offset },
	{ .compatible = "thead,c900-aclint-mswi" },
	{ .compatible = "mips,p8700-aclint-mswi" },
	{ .compatible = "riscv,aclint-mswi" },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_ipi_mswi = {
	.match_table = ipi_mswi_match,
	.init = ipi_mswi_cold_init,
};
