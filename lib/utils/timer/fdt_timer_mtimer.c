/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <libfdt.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_list.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/timer/fdt_timer.h>
#شامل <sbi_utils/timer/aclint_mtimer.h>

ساخت timer_mtimer_quirks {
	bool		is_clint;
	غیرعلامتی صحیح	clint_mtime_انحراف;
	bool		clint_without_mtime;
	bool		has_64bit_mmio;
};

ساخت timer_mtimer_گرہ {
	ساخت sbi_dlist سر;
	ساخت aclint_mtimer_ڈیٹا ڈیٹا;
};
ساکن SBI_LIST_سر(mtn_list);

ساکن ساخت aclint_mtimer_ڈیٹا *mt_reference = تہی;

ساکن صحیح timer_mtimer_cold_init(ثابت خالی *fdt, صحیح nodeoff,
				  ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح rc;
	غیرعلامتی طویل addr[2], سائز[2];
	ساخت timer_mtimer_گرہ *mtn, *n;
	ساخت aclint_mtimer_ڈیٹا *mt;
	ثابت ساخت timer_mtimer_quirks *quirks = مطابقت->ڈیٹا;
	bool is_clint = quirks && quirks->is_clint;
	bool is_ref = false;

	mtn = sbi_zalloc(حجم(*mtn));
	اگر (!mtn)
		واپس SBI_ENOMEM;
	mt = &mtn->ڈیٹا;

	rc = fdt_parse_aclint_گرہ(fdt, nodeoff, true, !is_clint,
				   &addr[0], &سائز[0], &addr[1], &سائز[1],
				   &mt->پہلا_hartid, &mt->hart_گنتی);
	اگر (rc) {
		sbi_free(mtn);
		واپس rc;
	}
	mt->has_64bit_mmio = true;
	mt->has_shared_mtime = false;

	rc = fdt_parse_timebase_frequency(fdt, &mt->mtime_freq);
	اگر (rc) {
		sbi_free(mtn);
		واپس rc;
	}

	اگر (is_clint) { /* SiFive CLINT */
		/* Set CLINT addresses */
		mt->mtimecmp_addr = addr[0] + ACLINT_DEFAULT_MTIMECMP_انحراف;
		mt->mtimecmp_سائز = ACLINT_DEFAULT_MTIMECMP_سائز;
		اگر (!quirks->clint_without_mtime) {
			mt->mtime_addr = addr[0] + ACLINT_DEFAULT_MTIME_انحراف;
			mt->mtime_سائز = سائز[0] - mt->mtimecmp_سائز;
			/* Adjust MTIMER address and size for CLINT device */
			mt->mtime_addr += quirks->clint_mtime_انحراف;
			mt->mtime_سائز -= quirks->clint_mtime_انحراف;
		} ورنہ {
			mt->mtime_addr = mt->mtime_سائز = 0;
		}
		mt->mtimecmp_addr += quirks->clint_mtime_انحراف;
	} ورنہ { /* RISC-V ACLINT MTIMER */
		/* Set ACLINT MTIMER addresses */
		mt->mtime_addr = addr[0];
		mt->mtime_سائز = سائز[0];
		mt->mtimecmp_addr = addr[1];
		mt->mtimecmp_سائز = سائز[1];
	}

	/* Apply additional quirks */
	اگر (quirks) {
		mt->has_64bit_mmio = quirks->has_64bit_mmio;
	}

	/* Check if MTIMER device has shared MTIME address */
	اگر (mt->mtime_سائز) {
		mt->has_shared_mtime = false;
		sbi_list_for_each_اندراج(n, &mtn_list, سر) {
			اگر (n->ڈیٹا.mtime_addr == mt->mtime_addr) {
				mt->has_shared_mtime = true;
				توڑو;
			}
		}
	} ورنہ {
		/* Assume shared time CSR */
		mt->has_shared_mtime = true;
	}

	/* Initialize the MTIMER device */
	rc = aclint_mtimer_cold_init(mt, mt_reference);
	اگر (rc) {
		sbi_free(mtn);
		واپس rc;
	}

	/*
	 * If we have a DT property to indicate which MTIMER is the reference,
	 * select the first MTIMER device that has it. Otherwise, select the
	 * first MTIMER device with no associated HARTs as our reference.
	 */
	اگر (fdt_getprop(fdt, nodeoff, "riscv,reference-mtimer", تہی))
		is_ref = true;
	ورنہ اگر (!mt->hart_گنتی)
		is_ref = true;

	اگر (is_ref && !mt_reference) {
		mt_reference = mt;
		/*
		 * Set reference for already propbed MTIMER devices
		 * with non-shared MTIME
		 */
		sbi_list_for_each_اندراج(n, &mtn_list, سر) {
			اگر (!n->ڈیٹا.has_shared_mtime)
				aclint_mtimer_سیٹ_reference(&n->ڈیٹا, mt);
		}
	}

	/* Explicitly sync-up MTIMER devices not associated with any HARTs */
	اگر (!mt->hart_گنتی)
		aclint_mtimer_sync(mt);

	sbi_list_جوڑو_دم(&mtn->سر, &mtn_list);
	واپس 0;
}

ساکن ثابت ساخت timer_mtimer_quirks sifive_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_انحراف	= CLINT_MTIMER_انحراف,
	.has_64bit_mmio		= true,
};

ساکن ثابت ساخت timer_mtimer_quirks thead_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_انحراف	= CLINT_MTIMER_انحراف,
	.clint_without_mtime	= true,
};

ساکن ثابت ساخت timer_mtimer_quirks thead_aclint_quirks = {
	.has_64bit_mmio		= false,
};

ساکن ثابت ساخت fdt_مطابقت timer_mtimer_مطابقت[] = {
	{ .compatible = "mips,p8700-aclint-mtimer" },
	{ .compatible = "riscv,clint0", .ڈیٹا = &sifive_clint_quirks },
	{ .compatible = "sifive,clint0", .ڈیٹا = &sifive_clint_quirks },
	{ .compatible = "sifive,clint2", .ڈیٹا = &sifive_clint_quirks },
	{ .compatible = "thead,c900-clint", .ڈیٹا = &thead_clint_quirks },
	{ .compatible = "thead,c900-aclint-mtimer",
	  .ڈیٹا = &thead_aclint_quirks },
	{ .compatible = "riscv,aclint-mtimer" },
	{ },
};

ثابت ساخت fdt_driver fdt_timer_mtimer = {
	.مطابقت_جدول = timer_mtimer_مطابقت,
	.init = timer_mtimer_cold_init,
};
