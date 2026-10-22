/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/ipi/aclint_mswi.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ipi_mswi_cold_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
			      ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	unsigned long offset;
	ಸಂರಚನಾ aclint_mswi_data *ms;

	ms = sbi_zalloc(ಪರಿಮಾಣ(*ms));
	ಯದಿ (!ms)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

	rc = fdt_parse_aclint_ಗಂಟು(fdt, nodeoff, false, false,
				   &ms->addr, &ms->size, ರಿಕ್ತ, ರಿಕ್ತ,
				   &ms->first_hartid, &ms->hart_ಎಣಿಕೆ);
	ಯದಿ (rc) {
		sbi_free(ms);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಯದಿ (match->data) {
		/* Adjust MSWI address and size for CLINT device */
		offset = *((unsigned long *)match->data);
		ms->addr += offset;
		ಯದಿ ((ms->size - offset) < ACLINT_MSWI_SIZE)
			ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
		ms->size = ACLINT_MSWI_SIZE;
	}

	rc = aclint_mswi_cold_init(ms);
	ಯದಿ (rc) {
		sbi_free(ms);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ unsigned long clint_offset = CLINT_MSWI_OFFSET;

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match ipi_mswi_match[] = {
	{ .compatible = "riscv,clint0", .data = &clint_offset },
	{ .compatible = "sifive,clint0", .data = &clint_offset },
	{ .compatible = "thead,c900-clint", .data = &clint_offset },
	{ .compatible = "thead,c900-aclint-mswi" },
	{ .compatible = "mips,p8700-aclint-mswi" },
	{ .compatible = "riscv,aclint-mswi" },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_ipi_mswi = {
	.match_table = ipi_mswi_match,
	.init = ipi_mswi_cold_init,
};
