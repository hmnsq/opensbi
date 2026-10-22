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

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ log2roundup(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ x)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ret = 0;

	ಆಗ (ret < __riscv_xlen) {
		ಆದರೆ (x <= (1UL << ret))
			ನಿಲ್ಲಿಸು;
		ret++;
	}

	ಹಿಂತಿರುಗಿ ret;
}
