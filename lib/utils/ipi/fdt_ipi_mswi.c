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
			      ثابت بنية fdt_میل *میل)
{
	صحيح rc;
	بنانشانی لما وتھ;
	بنية aclint_mswi_انکڑے *ms;

	ms = sbi_zalloc(حجم(*ms));
	إذا (!ms)
		أرجع SBI_ENOMEM;

	rc = fdt_parse_aclint_گرہ(fdt, nodeoff, false, false,
				   &ms->addr, &ms->ناپ, عدم, عدم,
				   &ms->پہلا_hartid, &ms->hart_گنتی);
	إذا (rc) {
		sbi_free(ms);
		أرجع rc;
	}

	إذا (میل->انکڑے) {
		/* Adjust MSWI address and size for CLINT device */
		وتھ = *((بنانشانی لما *)میل->انکڑے);
		ms->addr += وتھ;
		إذا ((ms->ناپ - وتھ) < ACLINT_MSWI_ناپ)
			أرجع SBI_EINVAL;
		ms->ناپ = ACLINT_MSWI_ناپ;
	}

	rc = aclint_mswi_cold_init(ms);
	إذا (rc) {
		sbi_free(ms);
		أرجع rc;
	}

	أرجع 0;
}

ساكن ثابت بنانشانی لما clint_وتھ = CLINT_MSWI_وتھ;

ساكن ثابت بنية fdt_میل ipi_mswi_میل[] = {
	{ .compatible = "riscv,clint0", .انکڑے = &clint_وتھ },
	{ .compatible = "sifive,clint0", .انکڑے = &clint_وتھ },
	{ .compatible = "thead,c900-clint", .انکڑے = &clint_وتھ },
	{ .compatible = "thead,c900-aclint-mswi" },
	{ .compatible = "mips,p8700-aclint-mswi" },
	{ .compatible = "riscv,aclint-mswi" },
	{ },
};

ثابت بنية fdt_driver fdt_ipi_mswi = {
	.میل_جدول = ipi_mswi_میل,
	.init = ipi_mswi_cold_init,
};
