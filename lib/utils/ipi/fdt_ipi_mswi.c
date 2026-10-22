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
			      ثابت بنية fdt_ميلاپ *ميلاپ)
{
	صحيح rc;
	بنانشان ڊگهو وٿي;
	بنية aclint_mswi_معلومات *ms;

	ms = sbi_zalloc(حجم(*ms));
	إذا (!ms)
		أرجع SBI_ENOMEM;

	rc = fdt_parse_aclint_ڳنڍ(fdt, nodeoff, false, false,
				   &ms->addr, &ms->سائيز, عدم, عدم,
				   &ms->پهريون_hartid, &ms->hart_ڳڻپ);
	إذا (rc) {
		sbi_free(ms);
		أرجع rc;
	}

	إذا (ميلاپ->معلومات) {
		/* Adjust MSWI address and size for CLINT device */
		وٿي = *((بنانشان ڊگهو *)ميلاپ->معلومات);
		ms->addr += وٿي;
		إذا ((ms->سائيز - وٿي) < ACLINT_MSWI_سائيز)
			أرجع SBI_EINVAL;
		ms->سائيز = ACLINT_MSWI_سائيز;
	}

	rc = aclint_mswi_cold_init(ms);
	إذا (rc) {
		sbi_free(ms);
		أرجع rc;
	}

	أرجع 0;
}

ساكن ثابت بنانشان ڊگهو clint_وٿي = CLINT_MSWI_وٿي;

ساكن ثابت بنية fdt_ميلاپ ipi_mswi_ميلاپ[] = {
	{ .compatible = "riscv,clint0", .معلومات = &clint_وٿي },
	{ .compatible = "sifive,clint0", .معلومات = &clint_وٿي },
	{ .compatible = "thead,c900-clint", .معلومات = &clint_وٿي },
	{ .compatible = "thead,c900-aclint-mswi" },
	{ .compatible = "mips,p8700-aclint-mswi" },
	{ .compatible = "riscv,aclint-mswi" },
	{ },
};

ثابت بنية fdt_driver fdt_ipi_mswi = {
	.ميلاپ_جدول = ipi_mswi_ميلاپ,
	.init = ipi_mswi_cold_init,
};
