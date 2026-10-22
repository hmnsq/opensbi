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

अचिन्हित दीर्घ log2roundup(अचिन्हित दीर्घ x)
{
	अचिन्हित दीर्घ ret = 0;

	यावत् (ret < __riscv_xlen) {
		यदि (x <= (1UL << ret))
			भङ्ग;
		ret++;
	}

	प्रत्यागम ret;
}
