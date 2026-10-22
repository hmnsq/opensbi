/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi_utils/fdt/fdt_driver.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/ipi/aclint_mswi.h>

ساکن صحیح ipi_mswi_cold_init(ثابت خالی *fdt, صحیح nodeoff,
			      ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح rc;
	غیرعلامتی طویل انحراف;
	ساخت aclint_mswi_ڈیٹا *ms;

	ms = sbi_zalloc(حجم(*ms));
	اگر (!ms)
		واپس SBI_ENOMEM;

	rc = fdt_parse_aclint_گرہ(fdt, nodeoff, false, false,
				   &ms->addr, &ms->سائز, تہی, تہی,
				   &ms->پہلا_hartid, &ms->hart_گنتی);
	اگر (rc) {
		sbi_free(ms);
		واپس rc;
	}

	اگر (مطابقت->ڈیٹا) {
		/* Adjust MSWI address and size for CLINT device */
		انحراف = *((غیرعلامتی طویل *)مطابقت->ڈیٹا);
		ms->addr += انحراف;
		اگر ((ms->سائز - انحراف) < ACLINT_MSWI_سائز)
			واپس SBI_EINVAL;
		ms->سائز = ACLINT_MSWI_سائز;
	}

	rc = aclint_mswi_cold_init(ms);
	اگر (rc) {
		sbi_free(ms);
		واپس rc;
	}

	واپس 0;
}

ساکن ثابت غیرعلامتی طویل clint_انحراف = CLINT_MSWI_انحراف;

ساکن ثابت ساخت fdt_مطابقت ipi_mswi_مطابقت[] = {
	{ .compatible = "riscv,clint0", .ڈیٹا = &clint_انحراف },
	{ .compatible = "sifive,clint0", .ڈیٹا = &clint_انحراف },
	{ .compatible = "thead,c900-clint", .ڈیٹا = &clint_انحراف },
	{ .compatible = "thead,c900-aclint-mswi" },
	{ .compatible = "mips,p8700-aclint-mswi" },
	{ .compatible = "riscv,aclint-mswi" },
	{ },
};

ثابت ساخت fdt_driver fdt_ipi_mswi = {
	.مطابقت_جدول = ipi_mswi_مطابقت,
	.init = ipi_mswi_cold_init,
};
