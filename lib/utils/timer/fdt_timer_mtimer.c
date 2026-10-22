/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi/sbi_list.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/timer/fdt_timer.h>
#અન્તર્ભાવ <sbi_utils/timer/aclint_mtimer.h>

સંરચના timer_mtimer_quirks {
	bool		is_clint;
	unsigned પૂર્ણાંક	clint_mtime_offset;
	bool		clint_without_mtime;
	bool		has_64bit_mmio;
};

સંરચના timer_mtimer_ગાંઠ {
	સંરચના sbi_dlist head;
	સંરચના aclint_mtimer_data data;
};
સ્થિર SBI_LIST_HEAD(mtn_list);

સ્થિર સંરચના aclint_mtimer_data *mt_reference = ખાલી;

સ્થિર પૂર્ણાંક timer_mtimer_cold_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
				  અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક rc;
	unsigned long addr[2], size[2];
	સંરચના timer_mtimer_ગાંઠ *mtn, *n;
	સંરચના aclint_mtimer_data *mt;
	અચલ સંરચના timer_mtimer_quirks *quirks = match->data;
	bool is_clint = quirks && quirks->is_clint;
	bool is_ref = false;

	mtn = sbi_zalloc(કદ(*mtn));
	જો (!mtn)
		પાછા SBI_ENOMEM;
	mt = &mtn->data;

	rc = fdt_parse_aclint_ગાંઠ(fdt, nodeoff, true, !is_clint,
				   &addr[0], &size[0], &addr[1], &size[1],
				   &mt->first_hartid, &mt->hart_ગણતરી);
	જો (rc) {
		sbi_free(mtn);
		પાછા rc;
	}
	mt->has_64bit_mmio = true;
	mt->has_shared_mtime = false;

	rc = fdt_parse_timebase_frequency(fdt, &mt->mtime_freq);
	જો (rc) {
		sbi_free(mtn);
		પાછા rc;
	}

	જો (is_clint) { /* SiFive CLINT */
		/* Set CLINT addresses */
		mt->mtimecmp_addr = addr[0] + ACLINT_DEFAULT_MTIMECMP_OFFSET;
		mt->mtimecmp_size = ACLINT_DEFAULT_MTIMECMP_SIZE;
		જો (!quirks->clint_without_mtime) {
			mt->mtime_addr = addr[0] + ACLINT_DEFAULT_MTIME_OFFSET;
			mt->mtime_size = size[0] - mt->mtimecmp_size;
			/* Adjust MTIMER address and size for CLINT device */
			mt->mtime_addr += quirks->clint_mtime_offset;
			mt->mtime_size -= quirks->clint_mtime_offset;
		} નહીંતર {
			mt->mtime_addr = mt->mtime_size = 0;
		}
		mt->mtimecmp_addr += quirks->clint_mtime_offset;
	} નહીંતર { /* RISC-V ACLINT MTIMER */
		/* Set ACLINT MTIMER addresses */
		mt->mtime_addr = addr[0];
		mt->mtime_size = size[0];
		mt->mtimecmp_addr = addr[1];
		mt->mtimecmp_size = size[1];
	}

	/* Apply additional quirks */
	જો (quirks) {
		mt->has_64bit_mmio = quirks->has_64bit_mmio;
	}

	/* Check if MTIMER device has shared MTIME address */
	જો (mt->mtime_size) {
		mt->has_shared_mtime = false;
		sbi_list_for_each_entry(n, &mtn_list, head) {
			જો (n->data.mtime_addr == mt->mtime_addr) {
				mt->has_shared_mtime = true;
				તોડો;
			}
		}
	} નહીંતર {
		/* Assume shared time CSR */
		mt->has_shared_mtime = true;
	}

	/* Initialize the MTIMER device */
	rc = aclint_mtimer_cold_init(mt, mt_reference);
	જો (rc) {
		sbi_free(mtn);
		પાછા rc;
	}

	/*
	 * If we have a DT property to indicate which MTIMER is the reference,
	 * select the first MTIMER device that has it. Otherwise, select the
	 * first MTIMER device with no associated HARTs as our reference.
	 */
	જો (fdt_getprop(fdt, nodeoff, "riscv,reference-mtimer", ખાલી))
		is_ref = true;
	નહીંતર જો (!mt->hart_ગણતરી)
		is_ref = true;

	જો (is_ref && !mt_reference) {
		mt_reference = mt;
		/*
		 * Set reference for already propbed MTIMER devices
		 * with non-shared MTIME
		 */
		sbi_list_for_each_entry(n, &mtn_list, head) {
			જો (!n->data.has_shared_mtime)
				aclint_mtimer_set_reference(&n->data, mt);
		}
	}

	/* Explicitly sync-up MTIMER devices not associated with any HARTs */
	જો (!mt->hart_ગણતરી)
		aclint_mtimer_sync(mt);

	sbi_list_add_tail(&mtn->head, &mtn_list);
	પાછા 0;
}

સ્થિર અચલ સંરચના timer_mtimer_quirks sifive_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_offset	= CLINT_MTIMER_OFFSET,
	.has_64bit_mmio		= true,
};

સ્થિર અચલ સંરચના timer_mtimer_quirks thead_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_offset	= CLINT_MTIMER_OFFSET,
	.clint_without_mtime	= true,
};

સ્થિર અચલ સંરચના timer_mtimer_quirks thead_aclint_quirks = {
	.has_64bit_mmio		= false,
};

સ્થિર અચલ સંરચના fdt_match timer_mtimer_match[] = {
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

અચલ સંરચના fdt_driver fdt_timer_mtimer = {
	.match_table = timer_mtimer_match,
	.init = timer_mtimer_cold_init,
};
