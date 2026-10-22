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
			      ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	पूर्णाङ्क rc;
	अचिह्नित दीर्घ विस्थापनम्;
	संरचना aclint_mswi_दत्त *ms;

	ms = sbi_zalloc(परिमाण(*ms));
	यदि (!ms)
		प्रत्यागम SBI_ENOMEM;

	rc = fdt_parse_aclint_ग्रन्थिः(fdt, nodeoff, false, false,
				   &ms->addr, &ms->आकार, रिक्त, रिक्त,
				   &ms->first_hartid, &ms->hart_गणनम्);
	यदि (rc) {
		sbi_free(ms);
		प्रत्यागम rc;
	}

	यदि (मेलनम्->दत्त) {
		/* Adjust MSWI address and size for CLINT device */
		विस्थापनम् = *((अचिह्नित दीर्घ *)मेलनम्->दत्त);
		ms->addr += विस्थापनम्;
		यदि ((ms->आकार - विस्थापनम्) < ACLINT_MSWI_आकार)
			प्रत्यागम SBI_EINVAL;
		ms->आकार = ACLINT_MSWI_आकार;
	}

	rc = aclint_mswi_cold_init(ms);
	यदि (rc) {
		sbi_free(ms);
		प्रत्यागम rc;
	}

	प्रत्यागम 0;
}

स्थिर ध्रुव अचिह्नित दीर्घ clint_विस्थापनम् = CLINT_MSWI_विस्थापनम्;

स्थिर ध्रुव संरचना fdt_मेलनम् ipi_mswi_मेलनम्[] = {
	{ .compatible = "riscv,clint0", .दत्त = &clint_विस्थापनम् },
	{ .compatible = "sifive,clint0", .दत्त = &clint_विस्थापनम् },
	{ .compatible = "thead,c900-clint", .दत्त = &clint_विस्थापनम् },
	{ .compatible = "thead,c900-aclint-mswi" },
	{ .compatible = "mips,p8700-aclint-mswi" },
	{ .compatible = "riscv,aclint-mswi" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_ipi_mswi = {
	.मेलनम्_सारणी = ipi_mswi_मेलनम्,
	.init = ipi_mswi_cold_init,
};
