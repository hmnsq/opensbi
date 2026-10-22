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

	trong_khi (ret < __riscv_xlen) {
		nếu (x <= (1UL << ret))
			break;
		ret++;
	}

	trả_về ret;
}
