/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi/sbi_list.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/timer/fdt_timer.h>
#অন্তর্ভাব <sbi_utils/timer/aclint_mtimer.h>

কাঠামো timer_mtimer_quirks {
	bool		is_clint;
	unsigned পূর্ণসংখ্যা	clint_mtime_offset;
	bool		clint_without_mtime;
	bool		has_64bit_mmio;
};

কাঠামো timer_mtimer_গ্রন্থি {
	কাঠামো sbi_dlist শীর্ষ;
	কাঠামো aclint_mtimer_তথ্য তথ্য;
};
স্থির SBI_LIST_শীর্ষ(mtn_list);

স্থির কাঠামো aclint_mtimer_তথ্য *mt_reference = ফাঁকা;

স্থির পূর্ণসংখ্যা timer_mtimer_cold_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
				  ধ্রুবক কাঠামো fdt_match *match)
{
	পূর্ণসংখ্যা rc;
	unsigned long addr[2], আকার_2[2];
	কাঠামো timer_mtimer_গ্রন্থি *mtn, *n;
	কাঠামো aclint_mtimer_তথ্য *mt;
	ধ্রুবক কাঠামো timer_mtimer_quirks *quirks = match->তথ্য;
	bool is_clint = quirks && quirks->is_clint;
	bool is_ref = false;

	mtn = sbi_zalloc(আকার(*mtn));
	যদি (!mtn)
		ফেরত SBI_ENOMEM;
	mt = &mtn->তথ্য;

	rc = fdt_parse_aclint_গ্রন্থি(fdt, nodeoff, true, !is_clint,
				   &addr[0], &আকার_2[0], &addr[1], &আকার_2[1],
				   &mt->first_hartid, &mt->hart_গুনতি);
	যদি (rc) {
		sbi_free(mtn);
		ফেরত rc;
	}
	mt->has_64bit_mmio = true;
	mt->has_shared_mtime = false;

	rc = fdt_parse_timebase_frequency(fdt, &mt->mtime_freq);
	যদি (rc) {
		sbi_free(mtn);
		ফেরত rc;
	}

	যদি (is_clint) { /* SiFive CLINT */
		/* Set CLINT addresses */
		mt->mtimecmp_addr = addr[0] + ACLINT_DEFAULT_MTIMECMP_OFFSET;
		mt->mtimecmp_আকার = ACLINT_DEFAULT_MTIMECMP_আকার;
		যদি (!quirks->clint_without_mtime) {
			mt->mtime_addr = addr[0] + ACLINT_DEFAULT_MTIME_OFFSET;
			mt->mtime_আকার = আকার_2[0] - mt->mtimecmp_আকার;
			/* Adjust MTIMER address and size for CLINT device */
			mt->mtime_addr += quirks->clint_mtime_offset;
			mt->mtime_আকার -= quirks->clint_mtime_offset;
		} অন্যথা {
			mt->mtime_addr = mt->mtime_আকার = 0;
		}
		mt->mtimecmp_addr += quirks->clint_mtime_offset;
	} অন্যথা { /* RISC-V ACLINT MTIMER */
		/* Set ACLINT MTIMER addresses */
		mt->mtime_addr = addr[0];
		mt->mtime_আকার = আকার_2[0];
		mt->mtimecmp_addr = addr[1];
		mt->mtimecmp_আকার = আকার_2[1];
	}

	/* Apply additional quirks */
	যদি (quirks) {
		mt->has_64bit_mmio = quirks->has_64bit_mmio;
	}

	/* Check if MTIMER device has shared MTIME address */
	যদি (mt->mtime_আকার) {
		mt->has_shared_mtime = false;
		sbi_list_for_each_entry(n, &mtn_list, শীর্ষ) {
			যদি (n->তথ্য.mtime_addr == mt->mtime_addr) {
				mt->has_shared_mtime = true;
				বিরতি;
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
		ফেরত rc;
	}

	/*
	 * If we have a DT property to indicate which MTIMER is the reference,
	 * select the first MTIMER device that has it. Otherwise, select the
	 * first MTIMER device with no associated HARTs as our reference.
	 */
	যদি (fdt_getprop(fdt, nodeoff, "riscv,reference-mtimer", ফাঁকা))
		is_ref = true;
	অন্যথা যদি (!mt->hart_গুনতি)
		is_ref = true;

	যদি (is_ref && !mt_reference) {
		mt_reference = mt;
		/*
		 * Set reference for already propbed MTIMER devices
		 * with non-shared MTIME
		 */
		sbi_list_for_each_entry(n, &mtn_list, শীর্ষ) {
			যদি (!n->তথ্য.has_shared_mtime)
				aclint_mtimer_set_reference(&n->তথ্য, mt);
		}
	}

	/* Explicitly sync-up MTIMER devices not associated with any HARTs */
	যদি (!mt->hart_গুনতি)
		aclint_mtimer_sync(mt);

	sbi_list_add_লেজ(&mtn->শীর্ষ, &mtn_list);
	ফেরত 0;
}

স্থির ধ্রুবক কাঠামো timer_mtimer_quirks sifive_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_offset	= CLINT_MTIMER_OFFSET,
	.has_64bit_mmio		= true,
};

স্থির ধ্রুবক কাঠামো timer_mtimer_quirks thead_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_offset	= CLINT_MTIMER_OFFSET,
	.clint_without_mtime	= true,
};

স্থির ধ্রুবক কাঠামো timer_mtimer_quirks thead_aclint_quirks = {
	.has_64bit_mmio		= false,
};

স্থির ধ্রুবক কাঠামো fdt_match timer_mtimer_match[] = {
	{ .compatible = "mips,p8700-aclint-mtimer" },
	{ .compatible = "riscv,clint0", .তথ্য = &sifive_clint_quirks },
	{ .compatible = "sifive,clint0", .তথ্য = &sifive_clint_quirks },
	{ .compatible = "sifive,clint2", .তথ্য = &sifive_clint_quirks },
	{ .compatible = "thead,c900-clint", .তথ্য = &thead_clint_quirks },
	{ .compatible = "thead,c900-aclint-mtimer",
	  .তথ্য = &thead_aclint_quirks },
	{ .compatible = "riscv,aclint-mtimer" },
	{ },
};

ধ্রুবক কাঠামো fdt_driver fdt_timer_mtimer = {
	.match_table = timer_mtimer_match,
	.init = timer_mtimer_cold_init,
};
