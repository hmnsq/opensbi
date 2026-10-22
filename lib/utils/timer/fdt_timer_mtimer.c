/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi/sbi_list.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/timer/fdt_timer.h>
#అన్తర్భావ <sbi_utils/timer/aclint_mtimer.h>

నిర్మాణం timer_mtimer_quirks {
	bool		is_clint;
	గుర్తులేని పూర్ణాంకం	clint_mtime_స్థానభ్రంశం;
	bool		clint_without_mtime;
	bool		has_64bit_mmio;
};

నిర్మాణం timer_mtimer_కణుపు {
	నిర్మాణం sbi_dlist తల;
	నిర్మాణం aclint_mtimer_దత్తాంశం దత్తాంశం;
};
స్థిర SBI_LIST_తల(mtn_list);

స్థిర నిర్మాణం aclint_mtimer_దత్తాంశం *mt_reference = ఖాళీ;

స్థిర పూర్ణాంకం timer_mtimer_cold_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
				  స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం rc;
	గుర్తులేని దీర్ఘ addr[2], కొలత[2];
	నిర్మాణం timer_mtimer_కణుపు *mtn, *n;
	నిర్మాణం aclint_mtimer_దత్తాంశం *mt;
	స్థిరరాశి నిర్మాణం timer_mtimer_quirks *quirks = సరిపోలిక->దత్తాంశం;
	bool is_clint = quirks && quirks->is_clint;
	bool is_ref = false;

	mtn = sbi_zalloc(పరిమాణం(*mtn));
	ఒకవేళ (!mtn)
		తిరిగి SBI_ENOMEM;
	mt = &mtn->దత్తాంశం;

	rc = fdt_parse_aclint_కణుపు(fdt, nodeoff, true, !is_clint,
				   &addr[0], &కొలత[0], &addr[1], &కొలత[1],
				   &mt->మొదటి_hartid, &mt->hart_లెక్క);
	ఒకవేళ (rc) {
		sbi_free(mtn);
		తిరిగి rc;
	}
	mt->has_64bit_mmio = true;
	mt->has_shared_mtime = false;

	rc = fdt_parse_timebase_frequency(fdt, &mt->mtime_freq);
	ఒకవేళ (rc) {
		sbi_free(mtn);
		తిరిగి rc;
	}

	ఒకవేళ (is_clint) { /* SiFive CLINT */
		/* Set CLINT addresses */
		mt->mtimecmp_addr = addr[0] + ACLINT_DEFAULT_MTIMECMP_స్థానభ్రంశం;
		mt->mtimecmp_కొలత = ACLINT_DEFAULT_MTIMECMP_కొలత;
		ఒకవేళ (!quirks->clint_without_mtime) {
			mt->mtime_addr = addr[0] + ACLINT_DEFAULT_MTIME_స్థానభ్రంశం;
			mt->mtime_కొలత = కొలత[0] - mt->mtimecmp_కొలత;
			/* Adjust MTIMER address and size for CLINT device */
			mt->mtime_addr += quirks->clint_mtime_స్థానభ్రంశం;
			mt->mtime_కొలత -= quirks->clint_mtime_స్థానభ్రంశం;
		} లేకపోతే {
			mt->mtime_addr = mt->mtime_కొలత = 0;
		}
		mt->mtimecmp_addr += quirks->clint_mtime_స్థానభ్రంశం;
	} లేకపోతే { /* RISC-V ACLINT MTIMER */
		/* Set ACLINT MTIMER addresses */
		mt->mtime_addr = addr[0];
		mt->mtime_కొలత = కొలత[0];
		mt->mtimecmp_addr = addr[1];
		mt->mtimecmp_కొలత = కొలత[1];
	}

	/* Apply additional quirks */
	ఒకవేళ (quirks) {
		mt->has_64bit_mmio = quirks->has_64bit_mmio;
	}

	/* Check if MTIMER device has shared MTIME address */
	ఒకవేళ (mt->mtime_కొలత) {
		mt->has_shared_mtime = false;
		sbi_list_for_each_నమోదు(n, &mtn_list, తల) {
			ఒకవేళ (n->దత్తాంశం.mtime_addr == mt->mtime_addr) {
				mt->has_shared_mtime = true;
				ఆపు;
			}
		}
	} లేకపోతే {
		/* Assume shared time CSR */
		mt->has_shared_mtime = true;
	}

	/* Initialize the MTIMER device */
	rc = aclint_mtimer_cold_init(mt, mt_reference);
	ఒకవేళ (rc) {
		sbi_free(mtn);
		తిరిగి rc;
	}

	/*
	 * If we have a DT property to indicate which MTIMER is the reference,
	 * select the first MTIMER device that has it. Otherwise, select the
	 * first MTIMER device with no associated HARTs as our reference.
	 */
	ఒకవేళ (fdt_getprop(fdt, nodeoff, "riscv,reference-mtimer", ఖాళీ))
		is_ref = true;
	లేకపోతే ఒకవేళ (!mt->hart_లెక్క)
		is_ref = true;

	ఒకవేళ (is_ref && !mt_reference) {
		mt_reference = mt;
		/*
		 * Set reference for already propbed MTIMER devices
		 * with non-shared MTIME
		 */
		sbi_list_for_each_నమోదు(n, &mtn_list, తల) {
			ఒకవేళ (!n->దత్తాంశం.has_shared_mtime)
				aclint_mtimer_సమితి_reference(&n->దత్తాంశం, mt);
		}
	}

	/* Explicitly sync-up MTIMER devices not associated with any HARTs */
	ఒకవేళ (!mt->hart_లెక్క)
		aclint_mtimer_sync(mt);

	sbi_list_కూడు_తోక(&mtn->తల, &mtn_list);
	తిరిగి 0;
}

స్థిర స్థిరరాశి నిర్మాణం timer_mtimer_quirks sifive_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_స్థానభ్రంశం	= CLINT_MTIMER_స్థానభ్రంశం,
	.has_64bit_mmio		= true,
};

స్థిర స్థిరరాశి నిర్మాణం timer_mtimer_quirks thead_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_స్థానభ్రంశం	= CLINT_MTIMER_స్థానభ్రంశం,
	.clint_without_mtime	= true,
};

స్థిర స్థిరరాశి నిర్మాణం timer_mtimer_quirks thead_aclint_quirks = {
	.has_64bit_mmio		= false,
};

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక timer_mtimer_సరిపోలిక[] = {
	{ .compatible = "mips,p8700-aclint-mtimer" },
	{ .compatible = "riscv,clint0", .దత్తాంశం = &sifive_clint_quirks },
	{ .compatible = "sifive,clint0", .దత్తాంశం = &sifive_clint_quirks },
	{ .compatible = "sifive,clint2", .దత్తాంశం = &sifive_clint_quirks },
	{ .compatible = "thead,c900-clint", .దత్తాంశం = &thead_clint_quirks },
	{ .compatible = "thead,c900-aclint-mtimer",
	  .దత్తాంశం = &thead_aclint_quirks },
	{ .compatible = "riscv,aclint-mtimer" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_timer_mtimer = {
	.సరిపోలిక_పట్టిక = timer_mtimer_సరిపోలిక,
	.init = timer_mtimer_cold_init,
};
