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

unsigned long log2roundup(unsigned long x)
{
	unsigned long ret = 0;

	동안 (ret < __riscv_xlen) {
		만약 (x <= (1UL << ret))
			break;
		ret++;
	}

	반환 ret;
}
