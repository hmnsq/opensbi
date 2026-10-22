/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi_utils/fdt/fdt_driver.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/ipi/aclint_mswi.h>

ساكن صحيح ipi_mswi_cold_init(ثابت فراغ *fdt, صحيح nodeoff,
			      ثابت بنية fdt_سمون *سمون)
{
	صحيح rc;
	بینښه اوږد دننه_واټن;
	بنية aclint_mswi_معلومات *ms;

	ms = sbi_zalloc(حجم(*ms));
	إذا (!ms)
		أرجع SBI_ENOMEM;

	rc = fdt_parse_aclint_غوټه(fdt, nodeoff, false, false,
				   &ms->addr, &ms->اندازه, عدم, عدم,
				   &ms->لومړی_hartid, &ms->hart_شمېر);
	إذا (rc) {
		sbi_free(ms);
		أرجع rc;
	}

	إذا (سمون->معلومات) {
		/* Adjust MSWI address and size for CLINT device */
		دننه_واټن = *((بینښه اوږد *)سمون->معلومات);
		ms->addr += دننه_واټن;
		إذا ((ms->اندازه - دننه_واټن) < ACLINT_MSWI_اندازه)
			أرجع SBI_EINVAL;
		ms->اندازه = ACLINT_MSWI_اندازه;
	}

	rc = aclint_mswi_cold_init(ms);
	إذا (rc) {
		sbi_free(ms);
		أرجع rc;
	}

	أرجع 0;
}

ساكن ثابت بینښه اوږد clint_دننه_واټن = CLINT_MSWI_دننه_واټن;

ساكن ثابت بنية fdt_سمون ipi_mswi_سمون[] = {
	{ .compatible = "riscv,clint0", .معلومات = &clint_دننه_واټن },
	{ .compatible = "sifive,clint0", .معلومات = &clint_دننه_واټن },
	{ .compatible = "thead,c900-clint", .معلومات = &clint_دننه_واټن },
	{ .compatible = "thead,c900-aclint-mswi" },
	{ .compatible = "mips,p8700-aclint-mswi" },
	{ .compatible = "riscv,aclint-mswi" },
	{ },
};

ثابت بنية fdt_driver fdt_ipi_mswi = {
	.سمون_جدول = ipi_mswi_سمون,
	.init = ipi_mswi_cold_init,
};
