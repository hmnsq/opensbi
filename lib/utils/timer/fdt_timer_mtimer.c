/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi/sbi_list.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/timer/fdt_timer.h>
#அந்தர்பாவ <sbi_utils/timer/aclint_mtimer.h>

கட்டமைப்பு timer_mtimer_quirks {
	bool		is_clint;
	unsigned முழு	clint_mtime_offset;
	bool		clint_without_mtime;
	bool		has_64bit_mmio;
};

கட்டமைப்பு timer_mtimer_முனை {
	கட்டமைப்பு sbi_dlist head;
	கட்டமைப்பு aclint_mtimer_data data;
};
நிலையான SBI_LIST_HEAD(mtn_list);

நிலையான கட்டமைப்பு aclint_mtimer_data *mt_reference = வெற்று;

நிலையான முழு timer_mtimer_cold_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
				  மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு rc;
	unsigned long addr[2], size[2];
	கட்டமைப்பு timer_mtimer_முனை *mtn, *n;
	கட்டமைப்பு aclint_mtimer_data *mt;
	மாறிலி கட்டமைப்பு timer_mtimer_quirks *quirks = match->data;
	bool is_clint = quirks && quirks->is_clint;
	bool is_ref = false;

	mtn = sbi_zalloc(அளவு(*mtn));
	எனில் (!mtn)
		திரும்பு SBI_ENOMEM;
	mt = &mtn->data;

	rc = fdt_parse_aclint_முனை(fdt, nodeoff, true, !is_clint,
				   &addr[0], &size[0], &addr[1], &size[1],
				   &mt->first_hartid, &mt->hart_மொத்தம்);
	எனில் (rc) {
		sbi_free(mtn);
		திரும்பு rc;
	}
	mt->has_64bit_mmio = true;
	mt->has_shared_mtime = false;

	rc = fdt_parse_timebase_frequency(fdt, &mt->mtime_freq);
	எனில் (rc) {
		sbi_free(mtn);
		திரும்பு rc;
	}

	எனில் (is_clint) { /* SiFive CLINT */
		/* Set CLINT addresses */
		mt->mtimecmp_addr = addr[0] + ACLINT_DEFAULT_MTIMECMP_OFFSET;
		mt->mtimecmp_size = ACLINT_DEFAULT_MTIMECMP_SIZE;
		எனில் (!quirks->clint_without_mtime) {
			mt->mtime_addr = addr[0] + ACLINT_DEFAULT_MTIME_OFFSET;
			mt->mtime_size = size[0] - mt->mtimecmp_size;
			/* Adjust MTIMER address and size for CLINT device */
			mt->mtime_addr += quirks->clint_mtime_offset;
			mt->mtime_size -= quirks->clint_mtime_offset;
		} இல்லையெனில் {
			mt->mtime_addr = mt->mtime_size = 0;
		}
		mt->mtimecmp_addr += quirks->clint_mtime_offset;
	} இல்லையெனில் { /* RISC-V ACLINT MTIMER */
		/* Set ACLINT MTIMER addresses */
		mt->mtime_addr = addr[0];
		mt->mtime_size = size[0];
		mt->mtimecmp_addr = addr[1];
		mt->mtimecmp_size = size[1];
	}

	/* Apply additional quirks */
	எனில் (quirks) {
		mt->has_64bit_mmio = quirks->has_64bit_mmio;
	}

	/* Check if MTIMER device has shared MTIME address */
	எனில் (mt->mtime_size) {
		mt->has_shared_mtime = false;
		sbi_list_for_each_entry(n, &mtn_list, head) {
			எனில் (n->data.mtime_addr == mt->mtime_addr) {
				mt->has_shared_mtime = true;
				நிறுத்து;
			}
		}
	} இல்லையெனில் {
		/* Assume shared time CSR */
		mt->has_shared_mtime = true;
	}

	/* Initialize the MTIMER device */
	rc = aclint_mtimer_cold_init(mt, mt_reference);
	எனில் (rc) {
		sbi_free(mtn);
		திரும்பு rc;
	}

	/*
	 * If we have a DT property to indicate which MTIMER is the reference,
	 * select the first MTIMER device that has it. Otherwise, select the
	 * first MTIMER device with no associated HARTs as our reference.
	 */
	எனில் (fdt_getprop(fdt, nodeoff, "riscv,reference-mtimer", வெற்று))
		is_ref = true;
	இல்லையெனில் எனில் (!mt->hart_மொத்தம்)
		is_ref = true;

	எனில் (is_ref && !mt_reference) {
		mt_reference = mt;
		/*
		 * Set reference for already propbed MTIMER devices
		 * with non-shared MTIME
		 */
		sbi_list_for_each_entry(n, &mtn_list, head) {
			எனில் (!n->data.has_shared_mtime)
				aclint_mtimer_set_reference(&n->data, mt);
		}
	}

	/* Explicitly sync-up MTIMER devices not associated with any HARTs */
	எனில் (!mt->hart_மொத்தம்)
		aclint_mtimer_sync(mt);

	sbi_list_add_tail(&mtn->head, &mtn_list);
	திரும்பு 0;
}

நிலையான மாறிலி கட்டமைப்பு timer_mtimer_quirks sifive_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_offset	= CLINT_MTIMER_OFFSET,
	.has_64bit_mmio		= true,
};

நிலையான மாறிலி கட்டமைப்பு timer_mtimer_quirks thead_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_offset	= CLINT_MTIMER_OFFSET,
	.clint_without_mtime	= true,
};

நிலையான மாறிலி கட்டமைப்பு timer_mtimer_quirks thead_aclint_quirks = {
	.has_64bit_mmio		= false,
};

நிலையான மாறிலி கட்டமைப்பு fdt_match timer_mtimer_match[] = {
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

மாறிலி கட்டமைப்பு fdt_driver fdt_timer_mtimer = {
	.match_table = timer_mtimer_match,
	.init = timer_mtimer_cold_init,
};
