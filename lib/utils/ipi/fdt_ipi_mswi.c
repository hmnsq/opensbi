/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_driver.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/ipi/aclint_mswi.h>

ਸਥਿਰ ਪੂਰਨ ipi_mswi_cold_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			      ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ rc;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਵਿੱਥ;
	ਬਣਤਰ aclint_mswi_ਅੰਕੜੇ *ms;

	ms = sbi_zalloc(ਆਕਾਰ(*ms));
	ਜੇ (!ms)
		ਵਾਪਸ SBI_ENOMEM;

	rc = fdt_parse_aclint_ਗੰਢ(fdt, nodeoff, false, false,
				   &ms->addr, &ms->ਮਾਪ, ਨਿਰਾ, ਨਿਰਾ,
				   &ms->ਪਹਿਲਾ_hartid, &ms->hart_ਗਿਣਤੀ);
	ਜੇ (rc) {
		sbi_free(ms);
		ਵਾਪਸ rc;
	}

	ਜੇ (ਮੇਲ->ਅੰਕੜੇ) {
		/* Adjust MSWI address and size for CLINT device */
		ਵਿੱਥ = *((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *)ਮੇਲ->ਅੰਕੜੇ);
		ms->addr += ਵਿੱਥ;
		ਜੇ ((ms->ਮਾਪ - ਵਿੱਥ) < ACLINT_MSWI_ਮਾਪ)
			ਵਾਪਸ SBI_EINVAL;
		ms->ਮਾਪ = ACLINT_MSWI_ਮਾਪ;
	}

	rc = aclint_mswi_cold_init(ms);
	ਜੇ (rc) {
		sbi_free(ms);
		ਵਾਪਸ rc;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ clint_ਵਿੱਥ = CLINT_MSWI_ਵਿੱਥ;

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ ipi_mswi_ਮੇਲ[] = {
	{ .compatible = "riscv,clint0", .ਅੰਕੜੇ = &clint_ਵਿੱਥ },
	{ .compatible = "sifive,clint0", .ਅੰਕੜੇ = &clint_ਵਿੱਥ },
	{ .compatible = "thead,c900-clint", .ਅੰਕੜੇ = &clint_ਵਿੱਥ },
	{ .compatible = "thead,c900-aclint-mswi" },
	{ .compatible = "mips,p8700-aclint-mswi" },
	{ .compatible = "riscv,aclint-mswi" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_ipi_mswi = {
	.ਮੇਲ_ਸਾਰਣੀ = ipi_mswi_ਮੇਲ,
	.init = ipi_mswi_cold_init,
};
