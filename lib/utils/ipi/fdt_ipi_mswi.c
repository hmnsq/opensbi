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

स्थिर पूर्णाङ्क ipi_mswi_cold_init(अचर शून्य *fdt, पूर्णाङ्क nodeoff,
			      अचर संरचना fdt_match *match)
{
	पूर्णाङ्क rc;
	अचिन्हित दीर्घ offset;
	संरचना aclint_mswi_data *ms;

	ms = sbi_zalloc(परिमाण(*ms));
	यदि (!ms)
		फर्काऊ SBI_ENOMEM;

	rc = fdt_parse_aclint_गाँठो(fdt, nodeoff, false, false,
				   &ms->addr, &ms->size, रिक्त, रिक्त,
				   &ms->first_hartid, &ms->hart_गन्ती);
	यदि (rc) {
		sbi_free(ms);
		फर्काऊ rc;
	}

	यदि (match->data) {
		/* Adjust MSWI address and size for CLINT device */
		offset = *((अचिन्हित दीर्घ *)match->data);
		ms->addr += offset;
		यदि ((ms->size - offset) < ACLINT_MSWI_SIZE)
			फर्काऊ SBI_EINVAL;
		ms->size = ACLINT_MSWI_SIZE;
	}

	rc = aclint_mswi_cold_init(ms);
	यदि (rc) {
		sbi_free(ms);
		फर्काऊ rc;
	}

	फर्काऊ 0;
}

स्थिर अचर अचिन्हित दीर्घ clint_offset = CLINT_MSWI_OFFSET;

स्थिर अचर संरचना fdt_match ipi_mswi_match[] = {
	{ .compatible = "riscv,clint0", .data = &clint_offset },
	{ .compatible = "sifive,clint0", .data = &clint_offset },
	{ .compatible = "thead,c900-clint", .data = &clint_offset },
	{ .compatible = "thead,c900-aclint-mswi" },
	{ .compatible = "mips,p8700-aclint-mswi" },
	{ .compatible = "riscv,aclint-mswi" },
	{ },
};

अचर संरचना fdt_driver fdt_ipi_mswi = {
	.match_table = ipi_mswi_match,
	.init = ipi_mswi_cold_init,
};
