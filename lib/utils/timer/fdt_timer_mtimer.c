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
	بنانشان صحيح	clint_mtime_وٿي;
	bool		clint_without_mtime;
	bool		has_64bit_mmio;
};

بنية timer_mtimer_ڳنڍ {
	بنية sbi_dlist سر;
	بنية aclint_mtimer_معلومات معلومات;
};
ساكن SBI_LIST_سر(mtn_list);

ساكن بنية aclint_mtimer_معلومات *mt_reference = عدم;

ساكن صحيح timer_mtimer_cold_init(ثابت فراغ *fdt, صحيح nodeoff,
				  ثابت بنية fdt_ميلاپ *ميلاپ)
{
	صحيح rc;
	بنانشان ڊگهو addr[2], سائيز[2];
	بنية timer_mtimer_ڳنڍ *mtn, *n;
	بنية aclint_mtimer_معلومات *mt;
	ثابت بنية timer_mtimer_quirks *quirks = ميلاپ->معلومات;
	bool is_clint = quirks && quirks->is_clint;
	bool is_ref = false;

	mtn = sbi_zalloc(حجم(*mtn));
	إذا (!mtn)
		أرجع SBI_ENOMEM;
	mt = &mtn->معلومات;

	rc = fdt_parse_aclint_ڳنڍ(fdt, nodeoff, true, !is_clint,
				   &addr[0], &سائيز[0], &addr[1], &سائيز[1],
				   &mt->پهريون_hartid, &mt->hart_ڳڻپ);
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
		mt->mtimecmp_addr = addr[0] + ACLINT_DEFAULT_MTIMECMP_وٿي;
		mt->mtimecmp_سائيز = ACLINT_DEFAULT_MTIMECMP_سائيز;
		إذا (!quirks->clint_without_mtime) {
			mt->mtime_addr = addr[0] + ACLINT_DEFAULT_MTIME_وٿي;
			mt->mtime_سائيز = سائيز[0] - mt->mtimecmp_سائيز;
			/* Adjust MTIMER address and size for CLINT device */
			mt->mtime_addr += quirks->clint_mtime_وٿي;
			mt->mtime_سائيز -= quirks->clint_mtime_وٿي;
		} وإلا {
			mt->mtime_addr = mt->mtime_سائيز = 0;
		}
		mt->mtimecmp_addr += quirks->clint_mtime_وٿي;
	} وإلا { /* RISC-V ACLINT MTIMER */
		/* Set ACLINT MTIMER addresses */
		mt->mtime_addr = addr[0];
		mt->mtime_سائيز = سائيز[0];
		mt->mtimecmp_addr = addr[1];
		mt->mtimecmp_سائيز = سائيز[1];
	}

	/* Apply additional quirks */
	إذا (quirks) {
		mt->has_64bit_mmio = quirks->has_64bit_mmio;
	}

	/* Check if MTIMER device has shared MTIME address */
	إذا (mt->mtime_سائيز) {
		mt->has_shared_mtime = false;
		sbi_list_for_each_داخلا(n, &mtn_list, سر) {
			إذا (n->معلومات.mtime_addr == mt->mtime_addr) {
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
	وإلا إذا (!mt->hart_ڳڻپ)
		is_ref = true;

	إذا (is_ref && !mt_reference) {
		mt_reference = mt;
		/*
		 * Set reference for already propbed MTIMER devices
		 * with non-shared MTIME
		 */
		sbi_list_for_each_داخلا(n, &mtn_list, سر) {
			إذا (!n->معلومات.has_shared_mtime)
				aclint_mtimer_سيٽ_reference(&n->معلومات, mt);
		}
	}

	/* Explicitly sync-up MTIMER devices not associated with any HARTs */
	إذا (!mt->hart_ڳڻپ)
		aclint_mtimer_sync(mt);

	sbi_list_جوڙيو_پڇ(&mtn->سر, &mtn_list);
	أرجع 0;
}

ساكن ثابت بنية timer_mtimer_quirks sifive_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_وٿي	= CLINT_MTIMER_وٿي,
	.has_64bit_mmio		= true,
};

ساكن ثابت بنية timer_mtimer_quirks thead_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_وٿي	= CLINT_MTIMER_وٿي,
	.clint_without_mtime	= true,
};

ساكن ثابت بنية timer_mtimer_quirks thead_aclint_quirks = {
	.has_64bit_mmio		= false,
};

ساكن ثابت بنية fdt_ميلاپ timer_mtimer_ميلاپ[] = {
	{ .compatible = "mips,p8700-aclint-mtimer" },
	{ .compatible = "riscv,clint0", .معلومات = &sifive_clint_quirks },
	{ .compatible = "sifive,clint0", .معلومات = &sifive_clint_quirks },
	{ .compatible = "sifive,clint2", .معلومات = &sifive_clint_quirks },
	{ .compatible = "thead,c900-clint", .معلومات = &thead_clint_quirks },
	{ .compatible = "thead,c900-aclint-mtimer",
	  .معلومات = &thead_aclint_quirks },
	{ .compatible = "riscv,aclint-mtimer" },
	{ },
};

ثابت بنية fdt_driver fdt_timer_mtimer = {
	.ميلاپ_جدول = timer_mtimer_ميلاپ,
	.init = timer_mtimer_cold_init,
};
