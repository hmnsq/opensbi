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

غیرعلامتی طویل log2roundup(غیرعلامتی طویل x)
{
	غیرعلامتی طویل ret = 0;

	جب (ret < __riscv_xlen) {
		اگر (x <= (1UL << ret))
			توڑو;
		ret++;
	}

	واپس ret;
}
