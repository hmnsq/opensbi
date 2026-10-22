/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Common helper functions used across OpenSBI project.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

بنانشانی لما log2roundup(بنانشانی لما x)
{
	بنانشانی لما ret = 0;

	طالما (ret < __riscv_xlen) {
		إذا (x <= (1UL << ret))
			اكسر;
		ret++;
	}

	أرجع ret;
}
