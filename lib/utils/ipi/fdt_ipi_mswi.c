/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/ipi/aclint_mswi.h>

स्थिर पूर्णाङ्क ipi_mswi_cold_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			      ध्रुव संरचना fdt_match *match)
{
	पूर्णाङ्क rc;
	unsigned long offset;
	संरचना aclint_mswi_data *ms;

	ms = sbi_zalloc(परिमाण(*ms));
	जइ (!ms)
		प्रत्यागम SBI_ENOMEM;

	rc = fdt_parse_aclint_ग्रन्थिः(fdt, nodeoff, false, false,
				   &ms->addr, &ms->size, रिक्त, रिक्त,
				   &ms->first_hartid, &ms->hart_गणनम्);
	जइ (rc) {
		sbi_free(ms);
		प्रत्यागम rc;
	}

	जइ (match->data) {
		/* Adjust MSWI address and size for CLINT device */
		offset = *((unsigned long *)match->data);
		ms->addr += offset;
		जइ ((ms->size - offset) < ACLINT_MSWI_SIZE)
			प्रत्यागम SBI_EINVAL;
		ms->size = ACLINT_MSWI_SIZE;
	}

	rc = aclint_mswi_cold_init(ms);
	जइ (rc) {
		sbi_free(ms);
		प्रत्यागम rc;
	}

	प्रत्यागम 0;
}

स्थिर ध्रुव unsigned long clint_offset = CLINT_MSWI_OFFSET;

स्थिर ध्रुव संरचना fdt_match ipi_mswi_match[] = {
	{ .compatible = "riscv,clint0", .data = &clint_offset },
	{ .compatible = "sifive,clint0", .data = &clint_offset },
	{ .compatible = "thead,c900-clint", .data = &clint_offset },
	{ .compatible = "thead,c900-aclint-mswi" },
	{ .compatible = "mips,p8700-aclint-mswi" },
	{ .compatible = "riscv,aclint-mswi" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_ipi_mswi = {
	.match_table = ipi_mswi_match,
	.init = ipi_mswi_cold_init,
};
