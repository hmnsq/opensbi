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
			      ثابت بنية fdt_تطابق *تطابق)
{
	صحيح rc;
	بیعلامت دراز جابه_جایی;
	بنية aclint_mswi_داده *ms;

	ms = sbi_zalloc(حجم(*ms));
	إذا (!ms)
		أرجع SBI_ENOMEM;

	rc = fdt_parse_aclint_گره(fdt, nodeoff, false, false,
				   &ms->addr, &ms->اندازه, عدم, عدم,
				   &ms->اولی_hartid, &ms->hart_شمار);
	إذا (rc) {
		sbi_free(ms);
		أرجع rc;
	}

	إذا (تطابق->داده) {
		/* Adjust MSWI address and size for CLINT device */
		جابه_جایی = *((بیعلامت دراز *)تطابق->داده);
		ms->addr += جابه_جایی;
		إذا ((ms->اندازه - جابه_جایی) < ACLINT_MSWI_اندازه)
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

ساكن ثابت بیعلامت دراز clint_جابه_جایی = CLINT_MSWI_جابه_جایی;

ساكن ثابت بنية fdt_تطابق ipi_mswi_تطابق[] = {
	{ .compatible = "riscv,clint0", .داده = &clint_جابه_جایی },
	{ .compatible = "sifive,clint0", .داده = &clint_جابه_جایی },
	{ .compatible = "thead,c900-clint", .داده = &clint_جابه_جایی },
	{ .compatible = "thead,c900-aclint-mswi" },
	{ .compatible = "mips,p8700-aclint-mswi" },
	{ .compatible = "riscv,aclint-mswi" },
	{ },
};

ثابت بنية fdt_driver fdt_ipi_mswi = {
	.تطابق_جدول = ipi_mswi_تطابق,
	.init = ipi_mswi_cold_init,
};
