/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi_utils/fdt/fdt_driver.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/ipi/aclint_mswi.h>

अटल पूर्णांक ipi_mswi_cold_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			      स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक rc;
	अचिन्हित दीर्घ offset;
	काष्ठा aclint_mswi_तथ्य *ms;

	ms = sbi_zalloc(माप(*ms));
	अगर (!ms)
		वापस SBI_ENOMEM;

	rc = fdt_parse_aclint_गाँठ(fdt, nodeoff, false, false,
				   &ms->addr, &ms->आकार, शून्य, शून्य,
				   &ms->first_hartid, &ms->hart_गिनती);
	अगर (rc) {
		sbi_free(ms);
		वापस rc;
	}

	अगर (match->तथ्य) {
		/* Adjust MSWI address and size for CLINT device */
		offset = *((अचिन्हित दीर्घ *)match->तथ्य);
		ms->addr += offset;
		अगर ((ms->आकार - offset) < ACLINT_MSWI_आकार)
			वापस SBI_EINVAL;
		ms->आकार = ACLINT_MSWI_आकार;
	}

	rc = aclint_mswi_cold_init(ms);
	अगर (rc) {
		sbi_free(ms);
		वापस rc;
	}

	वापस 0;
}

अटल स्थिर अचिन्हित दीर्घ clint_offset = CLINT_MSWI_OFFSET;

अटल स्थिर काष्ठा fdt_match ipi_mswi_match[] = {
	{ .compatible = "riscv,clint0", .तथ्य = &clint_offset },
	{ .compatible = "sifive,clint0", .तथ्य = &clint_offset },
	{ .compatible = "thead,c900-clint", .तथ्य = &clint_offset },
	{ .compatible = "thead,c900-aclint-mswi" },
	{ .compatible = "mips,p8700-aclint-mswi" },
	{ .compatible = "riscv,aclint-mswi" },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_ipi_mswi = {
	.match_table = ipi_mswi_match,
	.init = ipi_mswi_cold_init,
};
