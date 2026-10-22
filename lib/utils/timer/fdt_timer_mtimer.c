/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_list.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/timer/fdt_timer.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/timer/aclint_mtimer.h>

ସଂରଚନା timer_mtimer_quirks {
	bool		is_clint;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା	clint_mtime_offset;
	bool		clint_without_mtime;
	bool		has_64bit_mmio;
};

ସଂରଚନା timer_mtimer_ଗଣ୍ଠି {
	ସଂରଚନା sbi_dlist head;
	ସଂରଚନା aclint_mtimer_data data;
};
ସ୍ଥିର SBI_LIST_HEAD(mtn_list);

ସ୍ଥିର ସଂରଚନା aclint_mtimer_data *mt_reference = ଖାଲି;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା timer_mtimer_cold_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
				  ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	unsigned long addr[2], size[2];
	ସଂରଚନା timer_mtimer_ଗଣ୍ଠି *mtn, *n;
	ସଂରଚନା aclint_mtimer_data *mt;
	ଅଚଳ ସଂରଚନା timer_mtimer_quirks *quirks = match->data;
	bool is_clint = quirks && quirks->is_clint;
	bool is_ref = false;

	mtn = sbi_zalloc(ଆକାର(*mtn));
	ଯଦି (!mtn)
		ଫେରନ୍ତୁ SBI_ENOMEM;
	mt = &mtn->data;

	rc = fdt_parse_aclint_ଗଣ୍ଠି(fdt, nodeoff, true, !is_clint,
				   &addr[0], &size[0], &addr[1], &size[1],
				   &mt->first_hartid, &mt->hart_ଗଣନା);
	ଯଦି (rc) {
		sbi_free(mtn);
		ଫେରନ୍ତୁ rc;
	}
	mt->has_64bit_mmio = true;
	mt->has_shared_mtime = false;

	rc = fdt_parse_timebase_frequency(fdt, &mt->mtime_freq);
	ଯଦି (rc) {
		sbi_free(mtn);
		ଫେରନ୍ତୁ rc;
	}

	ଯଦି (is_clint) { /* SiFive CLINT */
		/* Set CLINT addresses */
		mt->mtimecmp_addr = addr[0] + ACLINT_DEFAULT_MTIMECMP_OFFSET;
		mt->mtimecmp_size = ACLINT_DEFAULT_MTIMECMP_SIZE;
		ଯଦି (!quirks->clint_without_mtime) {
			mt->mtime_addr = addr[0] + ACLINT_DEFAULT_MTIME_OFFSET;
			mt->mtime_size = size[0] - mt->mtimecmp_size;
			/* Adjust MTIMER address and size for CLINT device */
			mt->mtime_addr += quirks->clint_mtime_offset;
			mt->mtime_size -= quirks->clint_mtime_offset;
		} ଅନ୍ୟଥା {
			mt->mtime_addr = mt->mtime_size = 0;
		}
		mt->mtimecmp_addr += quirks->clint_mtime_offset;
	} ଅନ୍ୟଥା { /* RISC-V ACLINT MTIMER */
		/* Set ACLINT MTIMER addresses */
		mt->mtime_addr = addr[0];
		mt->mtime_size = size[0];
		mt->mtimecmp_addr = addr[1];
		mt->mtimecmp_size = size[1];
	}

	/* Apply additional quirks */
	ଯଦି (quirks) {
		mt->has_64bit_mmio = quirks->has_64bit_mmio;
	}

	/* Check if MTIMER device has shared MTIME address */
	ଯଦି (mt->mtime_size) {
		mt->has_shared_mtime = false;
		sbi_list_for_each_entry(n, &mtn_list, head) {
			ଯଦି (n->data.mtime_addr == mt->mtime_addr) {
				mt->has_shared_mtime = true;
				ଭାଙ୍ଗ;
			}
		}
	} ଅନ୍ୟଥା {
		/* Assume shared time CSR */
		mt->has_shared_mtime = true;
	}

	/* Initialize the MTIMER device */
	rc = aclint_mtimer_cold_init(mt, mt_reference);
	ଯଦି (rc) {
		sbi_free(mtn);
		ଫେରନ୍ତୁ rc;
	}

	/*
	 * If we have a DT property to indicate which MTIMER is the reference,
	 * select the first MTIMER device that has it. Otherwise, select the
	 * first MTIMER device with no associated HARTs as our reference.
	 */
	ଯଦି (fdt_getprop(fdt, nodeoff, "riscv,reference-mtimer", ଖାଲି))
		is_ref = true;
	ଅନ୍ୟଥା ଯଦି (!mt->hart_ଗଣନା)
		is_ref = true;

	ଯଦି (is_ref && !mt_reference) {
		mt_reference = mt;
		/*
		 * Set reference for already propbed MTIMER devices
		 * with non-shared MTIME
		 */
		sbi_list_for_each_entry(n, &mtn_list, head) {
			ଯଦି (!n->data.has_shared_mtime)
				aclint_mtimer_set_reference(&n->data, mt);
		}
	}

	/* Explicitly sync-up MTIMER devices not associated with any HARTs */
	ଯଦି (!mt->hart_ଗଣନା)
		aclint_mtimer_sync(mt);

	sbi_list_add_tail(&mtn->head, &mtn_list);
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା timer_mtimer_quirks sifive_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_offset	= CLINT_MTIMER_OFFSET,
	.has_64bit_mmio		= true,
};

ସ୍ଥିର ଅଚଳ ସଂରଚନା timer_mtimer_quirks thead_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_offset	= CLINT_MTIMER_OFFSET,
	.clint_without_mtime	= true,
};

ସ୍ଥିର ଅଚଳ ସଂରଚନା timer_mtimer_quirks thead_aclint_quirks = {
	.has_64bit_mmio		= false,
};

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match timer_mtimer_match[] = {
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

ଅଚଳ ସଂରଚନା fdt_driver fdt_timer_mtimer = {
	.match_table = timer_mtimer_match,
	.init = timer_mtimer_cold_init,
};
