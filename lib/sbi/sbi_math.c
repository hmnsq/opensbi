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

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ log2roundup(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ x)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ret = 0;

	ਜਦੋਂ (ret < __riscv_xlen) {
		ਜੇ (x <= (1UL << ret))
			ਰੋਕੋ;
		ret++;
	}

	ਵਾਪਸ ret;
}
