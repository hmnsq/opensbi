/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাৱ <libfdt.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_heap.h>
#অন্তর্ভাৱ <sbi/sbi_list.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাৱ <sbi_utils/timer/fdt_timer.h>
#অন্তর্ভাৱ <sbi_utils/timer/aclint_mtimer.h>

সংরচনা timer_mtimer_quirks {
	bool		is_clint;
	unsigned পূর্ণাঙ্ক	clint_mtime_offset;
	bool		clint_without_mtime;
	bool		has_64bit_mmio;
};

সংরচনা timer_mtimer_গ্ৰন্থি {
	সংরচনা sbi_dlist head;
	সংরচনা aclint_mtimer_data data;
};
স্থির SBI_LIST_HEAD(mtn_list);

স্থির সংরচনা aclint_mtimer_data *mt_reference = রিক্ত;

স্থির পূর্ণাঙ্ক timer_mtimer_cold_init(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
				  ধ্রুৱ সংরচনা fdt_match *match)
{
	পূর্ণাঙ্ক rc;
	unsigned long addr[2], size[2];
	সংরচনা timer_mtimer_গ্ৰন্থি *mtn, *n;
	সংরচনা aclint_mtimer_data *mt;
	ধ্রুৱ সংরচনা timer_mtimer_quirks *quirks = match->data;
	bool is_clint = quirks && quirks->is_clint;
	bool is_ref = false;

	mtn = sbi_zalloc(পরিমাণ(*mtn));
	যদি (!mtn)
		ঘূৰাই SBI_ENOMEM;
	mt = &mtn->data;

	rc = fdt_parse_aclint_গ্ৰন্থি(fdt, nodeoff, true, !is_clint,
				   &addr[0], &size[0], &addr[1], &size[1],
				   &mt->first_hartid, &mt->hart_গুনতি);
	যদি (rc) {
		sbi_free(mtn);
		ঘূৰাই rc;
	}
	mt->has_64bit_mmio = true;
	mt->has_shared_mtime = false;

	rc = fdt_parse_timebase_frequency(fdt, &mt->mtime_freq);
	যদি (rc) {
		sbi_free(mtn);
		ঘূৰাই rc;
	}

	যদি (is_clint) { /* SiFive CLINT */
		/* Set CLINT addresses */
		mt->mtimecmp_addr = addr[0] + ACLINT_DEFAULT_MTIMECMP_OFFSET;
		mt->mtimecmp_size = ACLINT_DEFAULT_MTIMECMP_SIZE;
		যদি (!quirks->clint_without_mtime) {
			mt->mtime_addr = addr[0] + ACLINT_DEFAULT_MTIME_OFFSET;
			mt->mtime_size = size[0] - mt->mtimecmp_size;
			/* Adjust MTIMER address and size for CLINT device */
			mt->mtime_addr += quirks->clint_mtime_offset;
			mt->mtime_size -= quirks->clint_mtime_offset;
		} অন্যথা {
			mt->mtime_addr = mt->mtime_size = 0;
		}
		mt->mtimecmp_addr += quirks->clint_mtime_offset;
	} অন্যথা { /* RISC-V ACLINT MTIMER */
		/* Set ACLINT MTIMER addresses */
		mt->mtime_addr = addr[0];
		mt->mtime_size = size[0];
		mt->mtimecmp_addr = addr[1];
		mt->mtimecmp_size = size[1];
	}

	/* Apply additional quirks */
	যদি (quirks) {
		mt->has_64bit_mmio = quirks->has_64bit_mmio;
	}

	/* Check if MTIMER device has shared MTIME address */
	যদি (mt->mtime_size) {
		mt->has_shared_mtime = false;
		sbi_list_for_each_entry(n, &mtn_list, head) {
			যদি (n->data.mtime_addr == mt->mtime_addr) {
				mt->has_shared_mtime = true;
				ভঙ্গ;
			}
		}
	} অন্যথা {
		/* Assume shared time CSR */
		mt->has_shared_mtime = true;
	}

	/* Initialize the MTIMER device */
	rc = aclint_mtimer_cold_init(mt, mt_reference);
	যদি (rc) {
		sbi_free(mtn);
		ঘূৰাই rc;
	}

	/*
	 * If we have a DT property to indicate which MTIMER is the reference,
	 * select the first MTIMER device that has it. Otherwise, select the
	 * first MTIMER device with no associated HARTs as our reference.
	 */
	যদি (fdt_getprop(fdt, nodeoff, "riscv,reference-mtimer", রিক্ত))
		is_ref = true;
	অন্যথা যদি (!mt->hart_গুনতি)
		is_ref = true;

	যদি (is_ref && !mt_reference) {
		mt_reference = mt;
		/*
		 * Set reference for already propbed MTIMER devices
		 * with non-shared MTIME
		 */
		sbi_list_for_each_entry(n, &mtn_list, head) {
			যদি (!n->data.has_shared_mtime)
				aclint_mtimer_set_reference(&n->data, mt);
		}
	}

	/* Explicitly sync-up MTIMER devices not associated with any HARTs */
	যদি (!mt->hart_গুনতি)
		aclint_mtimer_sync(mt);

	sbi_list_add_tail(&mtn->head, &mtn_list);
	ঘূৰাই 0;
}

স্থির ধ্রুৱ সংরচনা timer_mtimer_quirks sifive_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_offset	= CLINT_MTIMER_OFFSET,
	.has_64bit_mmio		= true,
};

স্থির ধ্রুৱ সংরচনা timer_mtimer_quirks thead_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_offset	= CLINT_MTIMER_OFFSET,
	.clint_without_mtime	= true,
};

স্থির ধ্রুৱ সংরচনা timer_mtimer_quirks thead_aclint_quirks = {
	.has_64bit_mmio		= false,
};

স্থির ধ্রুৱ সংরচনা fdt_match timer_mtimer_match[] = {
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

ধ্রুৱ সংরচনা fdt_driver fdt_timer_mtimer = {
	.match_table = timer_mtimer_match,
	.init = timer_mtimer_cold_init,
};
