/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <libfdt.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_list.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/timer/fdt_timer.h>
#ضمن <sbi_utils/timer/aclint_mtimer.h>

بنية timer_mtimer_quirks {
	bool		is_clint;
	بنانشانی صحيح	clint_mtime_وتھ;
	bool		clint_without_mtime;
	bool		has_64bit_mmio;
};

بنية timer_mtimer_گرہ {
	بنية sbi_dlist سر;
	بنية aclint_mtimer_انکڑے انکڑے;
};
ساكن SBI_LIST_سر(mtn_list);

ساكن بنية aclint_mtimer_انکڑے *mt_reference = عدم;

ساكن صحيح timer_mtimer_cold_init(ثابت فراغ *fdt, صحيح nodeoff,
				  ثابت بنية fdt_میل *میل)
{
	صحيح rc;
	بنانشانی لما addr[2], ناپ[2];
	بنية timer_mtimer_گرہ *mtn, *n;
	بنية aclint_mtimer_انکڑے *mt;
	ثابت بنية timer_mtimer_quirks *quirks = میل->انکڑے;
	bool is_clint = quirks && quirks->is_clint;
	bool is_ref = false;

	mtn = sbi_zalloc(حجم(*mtn));
	إذا (!mtn)
		أرجع SBI_ENOMEM;
	mt = &mtn->انکڑے;

	rc = fdt_parse_aclint_گرہ(fdt, nodeoff, true, !is_clint,
				   &addr[0], &ناپ[0], &addr[1], &ناپ[1],
				   &mt->پہلا_hartid, &mt->hart_گنتی);
	إذا (rc) {
		sbi_free(mtn);
		أرجع rc;
	}
	mt->has_64bit_mmio = true;
	mt->has_shared_mtime = false;

	rc = fdt_parse_timebase_frequency(fdt, &mt->mtime_freq);
	إذا (rc) {
		sbi_free(mtn);
		أرجع rc;
	}

	إذا (is_clint) { /* SiFive CLINT */
		/* Set CLINT addresses */
		mt->mtimecmp_addr = addr[0] + ACLINT_DEFAULT_MTIMECMP_وتھ;
		mt->mtimecmp_ناپ = ACLINT_DEFAULT_MTIMECMP_ناپ;
		إذا (!quirks->clint_without_mtime) {
			mt->mtime_addr = addr[0] + ACLINT_DEFAULT_MTIME_وتھ;
			mt->mtime_ناپ = ناپ[0] - mt->mtimecmp_ناپ;
			/* Adjust MTIMER address and size for CLINT device */
			mt->mtime_addr += quirks->clint_mtime_وتھ;
			mt->mtime_ناپ -= quirks->clint_mtime_وتھ;
		} وإلا {
			mt->mtime_addr = mt->mtime_ناپ = 0;
		}
		mt->mtimecmp_addr += quirks->clint_mtime_وتھ;
	} وإلا { /* RISC-V ACLINT MTIMER */
		/* Set ACLINT MTIMER addresses */
		mt->mtime_addr = addr[0];
		mt->mtime_ناپ = ناپ[0];
		mt->mtimecmp_addr = addr[1];
		mt->mtimecmp_ناپ = ناپ[1];
	}

	/* Apply additional quirks */
	إذا (quirks) {
		mt->has_64bit_mmio = quirks->has_64bit_mmio;
	}

	/* Check if MTIMER device has shared MTIME address */
	إذا (mt->mtime_ناپ) {
		mt->has_shared_mtime = false;
		sbi_list_for_each_اندراج(n, &mtn_list, سر) {
			إذا (n->انکڑے.mtime_addr == mt->mtime_addr) {
				mt->has_shared_mtime = true;
				اكسر;
			}
		}
	} وإلا {
		/* Assume shared time CSR */
		mt->has_shared_mtime = true;
	}

	/* Initialize the MTIMER device */
	rc = aclint_mtimer_cold_init(mt, mt_reference);
	إذا (rc) {
		sbi_free(mtn);
		أرجع rc;
	}

	/*
	 * If we have a DT property to indicate which MTIMER is the reference,
	 * select the first MTIMER device that has it. Otherwise, select the
	 * first MTIMER device with no associated HARTs as our reference.
	 */
	إذا (fdt_getprop(fdt, nodeoff, "riscv,reference-mtimer", عدم))
		is_ref = true;
	وإلا إذا (!mt->hart_گنتی)
		is_ref = true;

	إذا (is_ref && !mt_reference) {
		mt_reference = mt;
		/*
		 * Set reference for already propbed MTIMER devices
		 * with non-shared MTIME
		 */
		sbi_list_for_each_اندراج(n, &mtn_list, سر) {
			إذا (!n->انکڑے.has_shared_mtime)
				aclint_mtimer_سیٹ_reference(&n->انکڑے, mt);
		}
	}

	/* Explicitly sync-up MTIMER devices not associated with any HARTs */
	إذا (!mt->hart_گنتی)
		aclint_mtimer_sync(mt);

	sbi_list_جوڑو_پوچھ(&mtn->سر, &mtn_list);
	أرجع 0;
}

ساكن ثابت بنية timer_mtimer_quirks sifive_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_وتھ	= CLINT_MTIMER_وتھ,
	.has_64bit_mmio		= true,
};

ساكن ثابت بنية timer_mtimer_quirks thead_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_وتھ	= CLINT_MTIMER_وتھ,
	.clint_without_mtime	= true,
};

ساكن ثابت بنية timer_mtimer_quirks thead_aclint_quirks = {
	.has_64bit_mmio		= false,
};

ساكن ثابت بنية fdt_میل timer_mtimer_میل[] = {
	{ .compatible = "mips,p8700-aclint-mtimer" },
	{ .compatible = "riscv,clint0", .انکڑے = &sifive_clint_quirks },
	{ .compatible = "sifive,clint0", .انکڑے = &sifive_clint_quirks },
	{ .compatible = "sifive,clint2", .انکڑے = &sifive_clint_quirks },
	{ .compatible = "thead,c900-clint", .انکڑے = &thead_clint_quirks },
	{ .compatible = "thead,c900-aclint-mtimer",
	  .انکڑے = &thead_aclint_quirks },
	{ .compatible = "riscv,aclint-mtimer" },
	{ },
};

ثابت بنية fdt_driver fdt_timer_mtimer = {
	.میل_جدول = timer_mtimer_میل,
	.init = timer_mtimer_cold_init,
};
