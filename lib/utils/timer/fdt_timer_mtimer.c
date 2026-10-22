/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_list.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/timer/fdt_timer.h>
#अन्तर्भाव <sbi_utils/timer/aclint_mtimer.h>

संरचना timer_mtimer_quirks {
	bool		is_clint;
	unsigned पूर्णाङ्क	clint_mtime_offset;
	bool		clint_without_mtime;
	bool		has_64bit_mmio;
};

संरचना timer_mtimer_गाठ {
	संरचना sbi_dlist head;
	संरचना aclint_mtimer_data data;
};
स्थिर SBI_LIST_HEAD(mtn_list);

स्थिर संरचना aclint_mtimer_data *mt_reference = रिक्त;

स्थिर पूर्णाङ्क timer_mtimer_cold_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
				  ध्रुव संरचना fdt_match *match)
{
	पूर्णाङ्क rc;
	unsigned long addr[2], size[2];
	संरचना timer_mtimer_गाठ *mtn, *n;
	संरचना aclint_mtimer_data *mt;
	ध्रुव संरचना timer_mtimer_quirks *quirks = match->data;
	bool is_clint = quirks && quirks->is_clint;
	bool is_ref = false;

	mtn = sbi_zalloc(परिमाण(*mtn));
	यदि (!mtn)
		प्रत्यागम SBI_ENOMEM;
	mt = &mtn->data;

	rc = fdt_parse_aclint_गाठ(fdt, nodeoff, true, !is_clint,
				   &addr[0], &size[0], &addr[1], &size[1],
				   &mt->first_hartid, &mt->hart_मोजणी);
	यदि (rc) {
		sbi_free(mtn);
		प्रत्यागम rc;
	}
	mt->has_64bit_mmio = true;
	mt->has_shared_mtime = false;

	rc = fdt_parse_timebase_frequency(fdt, &mt->mtime_freq);
	यदि (rc) {
		sbi_free(mtn);
		प्रत्यागम rc;
	}

	यदि (is_clint) { /* SiFive CLINT */
		/* Set CLINT addresses */
		mt->mtimecmp_addr = addr[0] + ACLINT_DEFAULT_MTIMECMP_OFFSET;
		mt->mtimecmp_size = ACLINT_DEFAULT_MTIMECMP_SIZE;
		यदि (!quirks->clint_without_mtime) {
			mt->mtime_addr = addr[0] + ACLINT_DEFAULT_MTIME_OFFSET;
			mt->mtime_size = size[0] - mt->mtimecmp_size;
			/* Adjust MTIMER address and size for CLINT device */
			mt->mtime_addr += quirks->clint_mtime_offset;
			mt->mtime_size -= quirks->clint_mtime_offset;
		} अन्यथा {
			mt->mtime_addr = mt->mtime_size = 0;
		}
		mt->mtimecmp_addr += quirks->clint_mtime_offset;
	} अन्यथा { /* RISC-V ACLINT MTIMER */
		/* Set ACLINT MTIMER addresses */
		mt->mtime_addr = addr[0];
		mt->mtime_size = size[0];
		mt->mtimecmp_addr = addr[1];
		mt->mtimecmp_size = size[1];
	}

	/* Apply additional quirks */
	यदि (quirks) {
		mt->has_64bit_mmio = quirks->has_64bit_mmio;
	}

	/* Check if MTIMER device has shared MTIME address */
	यदि (mt->mtime_size) {
		mt->has_shared_mtime = false;
		sbi_list_for_each_entry(n, &mtn_list, head) {
			यदि (n->data.mtime_addr == mt->mtime_addr) {
				mt->has_shared_mtime = true;
				भङ्ग;
			}
		}
	} अन्यथा {
		/* Assume shared time CSR */
		mt->has_shared_mtime = true;
	}

	/* Initialize the MTIMER device */
	rc = aclint_mtimer_cold_init(mt, mt_reference);
	यदि (rc) {
		sbi_free(mtn);
		प्रत्यागम rc;
	}

	/*
	 * If we have a DT property to indicate which MTIMER is the reference,
	 * select the first MTIMER device that has it. Otherwise, select the
	 * first MTIMER device with no associated HARTs as our reference.
	 */
	यदि (fdt_getprop(fdt, nodeoff, "riscv,reference-mtimer", रिक्त))
		is_ref = true;
	अन्यथा यदि (!mt->hart_मोजणी)
		is_ref = true;

	यदि (is_ref && !mt_reference) {
		mt_reference = mt;
		/*
		 * Set reference for already propbed MTIMER devices
		 * with non-shared MTIME
		 */
		sbi_list_for_each_entry(n, &mtn_list, head) {
			यदि (!n->data.has_shared_mtime)
				aclint_mtimer_set_reference(&n->data, mt);
		}
	}

	/* Explicitly sync-up MTIMER devices not associated with any HARTs */
	यदि (!mt->hart_मोजणी)
		aclint_mtimer_sync(mt);

	sbi_list_add_tail(&mtn->head, &mtn_list);
	प्रत्यागम 0;
}

स्थिर ध्रुव संरचना timer_mtimer_quirks sifive_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_offset	= CLINT_MTIMER_OFFSET,
	.has_64bit_mmio		= true,
};

स्थिर ध्रुव संरचना timer_mtimer_quirks thead_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_offset	= CLINT_MTIMER_OFFSET,
	.clint_without_mtime	= true,
};

स्थिर ध्रुव संरचना timer_mtimer_quirks thead_aclint_quirks = {
	.has_64bit_mmio		= false,
};

स्थिर ध्रुव संरचना fdt_match timer_mtimer_match[] = {
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

ध्रुव संरचना fdt_driver fdt_timer_mtimer = {
	.match_table = timer_mtimer_match,
	.init = timer_mtimer_cold_init,
};
