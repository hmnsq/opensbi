/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <libfdt.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_heap.h>
#include <sbi/sbi_list.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/timer/fdt_timer.h>
#include <sbi_utils/timer/aclint_mtimer.h>

sa_nracanaa timer_mtimer_quirks {
	bool		is_clint;
	unsigned ཧྲིལ	clint_mtime_offset;
	bool		clint_without_mtime;
	bool		has_64bit_mmio;
};

sa_nracanaa timer_mtimer_node {
	sa_nracanaa sbi_dlist head;
	sa_nracanaa aclint_mtimer_data data;
};
static SBI_LIST_HEAD(mtn_list);

static sa_nracanaa aclint_mtimer_data *mt_reference = NULL;

static ཧྲིལ timer_mtimer_cold_init(const void *fdt, ཧྲིལ nodeoff,
				  const sa_nracanaa fdt_match *match)
{
	ཧྲིལ rc;
	unsigned long addr[2], size[2];
	sa_nracanaa timer_mtimer_node *mtn, *n;
	sa_nracanaa aclint_mtimer_data *mt;
	const sa_nracanaa timer_mtimer_quirks *quirks = match->data;
	bool is_clint = quirks && quirks->is_clint;
	bool is_ref = false;

	mtn = sbi_zalloc(parimaa_na(*mtn));
	གལ་ཏེ (!mtn)
		ཕྱིར SBI_ENOMEM;
	mt = &mtn->data;

	rc = fdt_parse_aclint_node(fdt, nodeoff, true, !is_clint,
				   &addr[0], &size[0], &addr[1], &size[1],
				   &mt->first_hartid, &mt->hart_count);
	གལ་ཏེ (rc) {
		sbi_free(mtn);
		ཕྱིར rc;
	}
	mt->has_64bit_mmio = true;
	mt->has_shared_mtime = false;

	rc = fdt_parse_timebase_frequency(fdt, &mt->mtime_freq);
	གལ་ཏེ (rc) {
		sbi_free(mtn);
		ཕྱིར rc;
	}

	གལ་ཏེ (is_clint) { /* SiFive CLINT */
		/* Set CLINT addresses */
		mt->mtimecmp_addr = addr[0] + ACLINT_DEFAULT_MTIMECMP_OFFSET;
		mt->mtimecmp_size = ACLINT_DEFAULT_MTIMECMP_SIZE;
		གལ་ཏེ (!quirks->clint_without_mtime) {
			mt->mtime_addr = addr[0] + ACLINT_DEFAULT_MTIME_OFFSET;
			mt->mtime_size = size[0] - mt->mtimecmp_size;
			/* Adjust MTIMER address and size for CLINT device */
			mt->mtime_addr += quirks->clint_mtime_offset;
			mt->mtime_size -= quirks->clint_mtime_offset;
		} དེ་མིན {
			mt->mtime_addr = mt->mtime_size = 0;
		}
		mt->mtimecmp_addr += quirks->clint_mtime_offset;
	} དེ་མིན { /* RISC-V ACLINT MTIMER */
		/* Set ACLINT MTIMER addresses */
		mt->mtime_addr = addr[0];
		mt->mtime_size = size[0];
		mt->mtimecmp_addr = addr[1];
		mt->mtimecmp_size = size[1];
	}

	/* Apply additional quirks */
	གལ་ཏེ (quirks) {
		mt->has_64bit_mmio = quirks->has_64bit_mmio;
	}

	/* Check if MTIMER device has shared MTIME address */
	གལ་ཏེ (mt->mtime_size) {
		mt->has_shared_mtime = false;
		sbi_list_for_each_entry(n, &mtn_list, head) {
			གལ་ཏེ (n->data.mtime_addr == mt->mtime_addr) {
				mt->has_shared_mtime = true;
				bha_nga;
			}
		}
	} དེ་མིན {
		/* Assume shared time CSR */
		mt->has_shared_mtime = true;
	}

	/* Initialize the MTIMER device */
	rc = aclint_mtimer_cold_init(mt, mt_reference);
	གལ་ཏེ (rc) {
		sbi_free(mtn);
		ཕྱིར rc;
	}

	/*
	 * If we have a DT property to indicate which MTIMER is the reference,
	 * select the first MTIMER device that has it. Otherwise, select the
	 * first MTIMER device with no associated HARTs as our reference.
	 */
	གལ་ཏེ (fdt_getprop(fdt, nodeoff, "riscv,reference-mtimer", NULL))
		is_ref = true;
	དེ་མིན གལ་ཏེ (!mt->hart_count)
		is_ref = true;

	གལ་ཏེ (is_ref && !mt_reference) {
		mt_reference = mt;
		/*
		 * Set reference for already propbed MTIMER devices
		 * with non-shared MTIME
		 */
		sbi_list_for_each_entry(n, &mtn_list, head) {
			གལ་ཏེ (!n->data.has_shared_mtime)
				aclint_mtimer_set_reference(&n->data, mt);
		}
	}

	/* Explicitly sync-up MTIMER devices not associated with any HARTs */
	གལ་ཏེ (!mt->hart_count)
		aclint_mtimer_sync(mt);

	sbi_list_add_tail(&mtn->head, &mtn_list);
	ཕྱིར 0;
}

static const sa_nracanaa timer_mtimer_quirks sifive_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_offset	= CLINT_MTIMER_OFFSET,
	.has_64bit_mmio		= true,
};

static const sa_nracanaa timer_mtimer_quirks thead_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_offset	= CLINT_MTIMER_OFFSET,
	.clint_without_mtime	= true,
};

static const sa_nracanaa timer_mtimer_quirks thead_aclint_quirks = {
	.has_64bit_mmio		= false,
};

static const sa_nracanaa fdt_match timer_mtimer_match[] = {
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

const sa_nracanaa fdt_driver fdt_timer_mtimer = {
	.match_table = timer_mtimer_match,
	.init = timer_mtimer_cold_init,
};
