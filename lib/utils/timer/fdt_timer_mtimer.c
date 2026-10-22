/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi/sbi_list.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/timer/fdt_timer.h>
#അന്തര്ഭാവ <sbi_utils/timer/aclint_mtimer.h>

ഘടന timer_mtimer_quirks {
	bool		is_clint;
	unsigned പൂർണ്ണസംഖ്യ	clint_mtime_offset;
	bool		clint_without_mtime;
	bool		has_64bit_mmio;
};

ഘടന timer_mtimer_കണ്ണി {
	ഘടന sbi_dlist head;
	ഘടന aclint_mtimer_data data;
};
സ്ഥിതം SBI_LIST_HEAD(mtn_list);

സ്ഥിതം ഘടന aclint_mtimer_data *mt_reference = ശൂന്യ;

സ്ഥിതം പൂർണ്ണസംഖ്യ timer_mtimer_cold_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
				  സ്ഥിരം ഘടന fdt_match *match)
{
	പൂർണ്ണസംഖ്യ rc;
	unsigned long addr[2], size[2];
	ഘടന timer_mtimer_കണ്ണി *mtn, *n;
	ഘടന aclint_mtimer_data *mt;
	സ്ഥിരം ഘടന timer_mtimer_quirks *quirks = match->data;
	bool is_clint = quirks && quirks->is_clint;
	bool is_ref = false;

	mtn = sbi_zalloc(വലുപ്പം(*mtn));
	എങ്കിൽ (!mtn)
		തിരികെ SBI_ENOMEM;
	mt = &mtn->data;

	rc = fdt_parse_aclint_കണ്ണി(fdt, nodeoff, true, !is_clint,
				   &addr[0], &size[0], &addr[1], &size[1],
				   &mt->first_hartid, &mt->hart_എണ്ണം);
	എങ്കിൽ (rc) {
		sbi_free(mtn);
		തിരികെ rc;
	}
	mt->has_64bit_mmio = true;
	mt->has_shared_mtime = false;

	rc = fdt_parse_timebase_frequency(fdt, &mt->mtime_freq);
	എങ്കിൽ (rc) {
		sbi_free(mtn);
		തിരികെ rc;
	}

	എങ്കിൽ (is_clint) { /* SiFive CLINT */
		/* Set CLINT addresses */
		mt->mtimecmp_addr = addr[0] + ACLINT_DEFAULT_MTIMECMP_OFFSET;
		mt->mtimecmp_size = ACLINT_DEFAULT_MTIMECMP_SIZE;
		എങ്കിൽ (!quirks->clint_without_mtime) {
			mt->mtime_addr = addr[0] + ACLINT_DEFAULT_MTIME_OFFSET;
			mt->mtime_size = size[0] - mt->mtimecmp_size;
			/* Adjust MTIMER address and size for CLINT device */
			mt->mtime_addr += quirks->clint_mtime_offset;
			mt->mtime_size -= quirks->clint_mtime_offset;
		} അല്ലെങ്കിൽ {
			mt->mtime_addr = mt->mtime_size = 0;
		}
		mt->mtimecmp_addr += quirks->clint_mtime_offset;
	} അല്ലെങ്കിൽ { /* RISC-V ACLINT MTIMER */
		/* Set ACLINT MTIMER addresses */
		mt->mtime_addr = addr[0];
		mt->mtime_size = size[0];
		mt->mtimecmp_addr = addr[1];
		mt->mtimecmp_size = size[1];
	}

	/* Apply additional quirks */
	എങ്കിൽ (quirks) {
		mt->has_64bit_mmio = quirks->has_64bit_mmio;
	}

	/* Check if MTIMER device has shared MTIME address */
	എങ്കിൽ (mt->mtime_size) {
		mt->has_shared_mtime = false;
		sbi_list_for_each_entry(n, &mtn_list, head) {
			എങ്കിൽ (n->data.mtime_addr == mt->mtime_addr) {
				mt->has_shared_mtime = true;
				നിർത്തുക;
			}
		}
	} അല്ലെങ്കിൽ {
		/* Assume shared time CSR */
		mt->has_shared_mtime = true;
	}

	/* Initialize the MTIMER device */
	rc = aclint_mtimer_cold_init(mt, mt_reference);
	എങ്കിൽ (rc) {
		sbi_free(mtn);
		തിരികെ rc;
	}

	/*
	 * If we have a DT property to indicate which MTIMER is the reference,
	 * select the first MTIMER device that has it. Otherwise, select the
	 * first MTIMER device with no associated HARTs as our reference.
	 */
	എങ്കിൽ (fdt_getprop(fdt, nodeoff, "riscv,reference-mtimer", ശൂന്യ))
		is_ref = true;
	അല്ലെങ്കിൽ എങ്കിൽ (!mt->hart_എണ്ണം)
		is_ref = true;

	എങ്കിൽ (is_ref && !mt_reference) {
		mt_reference = mt;
		/*
		 * Set reference for already propbed MTIMER devices
		 * with non-shared MTIME
		 */
		sbi_list_for_each_entry(n, &mtn_list, head) {
			എങ്കിൽ (!n->data.has_shared_mtime)
				aclint_mtimer_set_reference(&n->data, mt);
		}
	}

	/* Explicitly sync-up MTIMER devices not associated with any HARTs */
	എങ്കിൽ (!mt->hart_എണ്ണം)
		aclint_mtimer_sync(mt);

	sbi_list_add_tail(&mtn->head, &mtn_list);
	തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം ഘടന timer_mtimer_quirks sifive_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_offset	= CLINT_MTIMER_OFFSET,
	.has_64bit_mmio		= true,
};

സ്ഥിതം സ്ഥിരം ഘടന timer_mtimer_quirks thead_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_offset	= CLINT_MTIMER_OFFSET,
	.clint_without_mtime	= true,
};

സ്ഥിതം സ്ഥിരം ഘടന timer_mtimer_quirks thead_aclint_quirks = {
	.has_64bit_mmio		= false,
};

സ്ഥിതം സ്ഥിരം ഘടന fdt_match timer_mtimer_match[] = {
	{ .compatible = "mips,p8700-aclint-mtimer" },
	{ .compatible = "riscv,clint0", .data = &sifive_clint_quirks },
	{ .compatible = "sifive,clint0", .data = &sifive_clint_quirks },
	{ .compatible = "sifive,clint2", .data = &sifive_clint_quirks },
	{ .compatible = "thead,c900-clint", .data = &thead_clint_quirks },
	{ .compatible = "thead,c900-aclint-mtimer",
	  .data = &thead_aclint_quirks },
	{ .compatible = "riscv,aclint-mtimer" },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_timer_mtimer = {
	.match_table = timer_mtimer_match,
	.init = timer_mtimer_cold_init,
};
