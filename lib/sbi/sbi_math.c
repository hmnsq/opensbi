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

గుర్తులేని దీర్ఘ log2roundup(గుర్తులేని దీర్ఘ x)
{
	గుర్తులేని దీర్ఘ ret = 0;

	అయితే (ret < __riscv_xlen) {
		ఒకవేళ (x <= (1UL << ret))
			ఆపు;
		ret++;
	}

	తిరిగి ret;
}
