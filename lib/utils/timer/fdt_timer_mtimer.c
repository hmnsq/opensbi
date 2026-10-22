/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_list.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/timer/fdt_timer.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/timer/aclint_mtimer.h>

ಸಂರಚನಾ timer_mtimer_quirks {
	bool		is_clint;
	unsigned ಪೂರ್ಣಾಙ್ಕ	clint_mtime_offset;
	bool		clint_without_mtime;
	bool		has_64bit_mmio;
};

ಸಂರಚನಾ timer_mtimer_ಗಂಟು {
	ಸಂರಚನಾ sbi_dlist head;
	ಸಂರಚನಾ aclint_mtimer_data data;
};
ಸ್ಥಿರ SBI_LIST_HEAD(mtn_list);

ಸ್ಥಿರ ಸಂರಚನಾ aclint_mtimer_data *mt_reference = ರಿಕ್ತ;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ timer_mtimer_cold_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
				  ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	unsigned long addr[2], size[2];
	ಸಂರಚನಾ timer_mtimer_ಗಂಟು *mtn, *n;
	ಸಂರಚನಾ aclint_mtimer_data *mt;
	ಧ್ರುವ ಸಂರಚನಾ timer_mtimer_quirks *quirks = match->data;
	bool is_clint = quirks && quirks->is_clint;
	bool is_ref = false;

	mtn = sbi_zalloc(ಪರಿಮಾಣ(*mtn));
	ಯದಿ (!mtn)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;
	mt = &mtn->data;

	rc = fdt_parse_aclint_ಗಂಟು(fdt, nodeoff, true, !is_clint,
				   &addr[0], &size[0], &addr[1], &size[1],
				   &mt->first_hartid, &mt->hart_ಎಣಿಕೆ);
	ಯದಿ (rc) {
		sbi_free(mtn);
		ಪ್ರತ್ಯಾಗಮ rc;
	}
	mt->has_64bit_mmio = true;
	mt->has_shared_mtime = false;

	rc = fdt_parse_timebase_frequency(fdt, &mt->mtime_freq);
	ಯದಿ (rc) {
		sbi_free(mtn);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಯದಿ (is_clint) { /* SiFive CLINT */
		/* Set CLINT addresses */
		mt->mtimecmp_addr = addr[0] + ACLINT_DEFAULT_MTIMECMP_OFFSET;
		mt->mtimecmp_size = ACLINT_DEFAULT_MTIMECMP_SIZE;
		ಯದಿ (!quirks->clint_without_mtime) {
			mt->mtime_addr = addr[0] + ACLINT_DEFAULT_MTIME_OFFSET;
			mt->mtime_size = size[0] - mt->mtimecmp_size;
			/* Adjust MTIMER address and size for CLINT device */
			mt->mtime_addr += quirks->clint_mtime_offset;
			mt->mtime_size -= quirks->clint_mtime_offset;
		} ಅನ್ಯಥಾ {
			mt->mtime_addr = mt->mtime_size = 0;
		}
		mt->mtimecmp_addr += quirks->clint_mtime_offset;
	} ಅನ್ಯಥಾ { /* RISC-V ACLINT MTIMER */
		/* Set ACLINT MTIMER addresses */
		mt->mtime_addr = addr[0];
		mt->mtime_size = size[0];
		mt->mtimecmp_addr = addr[1];
		mt->mtimecmp_size = size[1];
	}

	/* Apply additional quirks */
	ಯದಿ (quirks) {
		mt->has_64bit_mmio = quirks->has_64bit_mmio;
	}

	/* Check if MTIMER device has shared MTIME address */
	ಯದಿ (mt->mtime_size) {
		mt->has_shared_mtime = false;
		sbi_list_for_each_entry(n, &mtn_list, head) {
			ಯದಿ (n->data.mtime_addr == mt->mtime_addr) {
				mt->has_shared_mtime = true;
				ಭಙ್ಗ;
			}
		}
	} ಅನ್ಯಥಾ {
		/* Assume shared time CSR */
		mt->has_shared_mtime = true;
	}

	/* Initialize the MTIMER device */
	rc = aclint_mtimer_cold_init(mt, mt_reference);
	ಯದಿ (rc) {
		sbi_free(mtn);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	/*
	 * If we have a DT property to indicate which MTIMER is the reference,
	 * select the first MTIMER device that has it. Otherwise, select the
	 * first MTIMER device with no associated HARTs as our reference.
	 */
	ಯದಿ (fdt_getprop(fdt, nodeoff, "riscv,reference-mtimer", ರಿಕ್ತ))
		is_ref = true;
	ಅನ್ಯಥಾ ಯದಿ (!mt->hart_ಎಣಿಕೆ)
		is_ref = true;

	ಯದಿ (is_ref && !mt_reference) {
		mt_reference = mt;
		/*
		 * Set reference for already propbed MTIMER devices
		 * with non-shared MTIME
		 */
		sbi_list_for_each_entry(n, &mtn_list, head) {
			ಯದಿ (!n->data.has_shared_mtime)
				aclint_mtimer_set_reference(&n->data, mt);
		}
	}

	/* Explicitly sync-up MTIMER devices not associated with any HARTs */
	ಯದಿ (!mt->hart_ಎಣಿಕೆ)
		aclint_mtimer_sync(mt);

	sbi_list_add_tail(&mtn->head, &mtn_list);
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ timer_mtimer_quirks sifive_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_offset	= CLINT_MTIMER_OFFSET,
	.has_64bit_mmio		= true,
};

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ timer_mtimer_quirks thead_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_offset	= CLINT_MTIMER_OFFSET,
	.clint_without_mtime	= true,
};

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ timer_mtimer_quirks thead_aclint_quirks = {
	.has_64bit_mmio		= false,
};

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match timer_mtimer_match[] = {
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

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_timer_mtimer = {
	.match_table = timer_mtimer_match,
	.init = timer_mtimer_cold_init,
};
