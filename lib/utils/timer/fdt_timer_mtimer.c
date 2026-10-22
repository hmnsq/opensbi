/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_list.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/timer/fdt_timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/timer/aclint_mtimer.h>

ਬਣਤਰ timer_mtimer_quirks {
	bool		is_clint;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ	clint_mtime_ਵਿੱਥ;
	bool		clint_without_mtime;
	bool		has_64bit_mmio;
};

ਬਣਤਰ timer_mtimer_ਗੰਢ {
	ਬਣਤਰ sbi_dlist ਸਿਰ;
	ਬਣਤਰ aclint_mtimer_ਅੰਕੜੇ ਅੰਕੜੇ;
};
ਸਥਿਰ SBI_LIST_ਸਿਰ(mtn_list);

ਸਥਿਰ ਬਣਤਰ aclint_mtimer_ਅੰਕੜੇ *mt_reference = ਨਿਰਾ;

ਸਥਿਰ ਪੂਰਨ timer_mtimer_cold_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
				  ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ rc;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr[2], ਮਾਪ[2];
	ਬਣਤਰ timer_mtimer_ਗੰਢ *mtn, *n;
	ਬਣਤਰ aclint_mtimer_ਅੰਕੜੇ *mt;
	ਅਚਲ ਬਣਤਰ timer_mtimer_quirks *quirks = ਮੇਲ->ਅੰਕੜੇ;
	bool is_clint = quirks && quirks->is_clint;
	bool is_ref = false;

	mtn = sbi_zalloc(ਆਕਾਰ(*mtn));
	ਜੇ (!mtn)
		ਵਾਪਸ SBI_ENOMEM;
	mt = &mtn->ਅੰਕੜੇ;

	rc = fdt_parse_aclint_ਗੰਢ(fdt, nodeoff, true, !is_clint,
				   &addr[0], &ਮਾਪ[0], &addr[1], &ਮਾਪ[1],
				   &mt->ਪਹਿਲਾ_hartid, &mt->hart_ਗਿਣਤੀ);
	ਜੇ (rc) {
		sbi_free(mtn);
		ਵਾਪਸ rc;
	}
	mt->has_64bit_mmio = true;
	mt->has_shared_mtime = false;

	rc = fdt_parse_timebase_frequency(fdt, &mt->mtime_freq);
	ਜੇ (rc) {
		sbi_free(mtn);
		ਵਾਪਸ rc;
	}

	ਜੇ (is_clint) { /* SiFive CLINT */
		/* Set CLINT addresses */
		mt->mtimecmp_addr = addr[0] + ACLINT_DEFAULT_MTIMECMP_ਵਿੱਥ;
		mt->mtimecmp_ਮਾਪ = ACLINT_DEFAULT_MTIMECMP_ਮਾਪ;
		ਜੇ (!quirks->clint_without_mtime) {
			mt->mtime_addr = addr[0] + ACLINT_DEFAULT_MTIME_ਵਿੱਥ;
			mt->mtime_ਮਾਪ = ਮਾਪ[0] - mt->mtimecmp_ਮਾਪ;
			/* Adjust MTIMER address and size for CLINT device */
			mt->mtime_addr += quirks->clint_mtime_ਵਿੱਥ;
			mt->mtime_ਮਾਪ -= quirks->clint_mtime_ਵਿੱਥ;
		} ਨਹੀਂਤਾਂ {
			mt->mtime_addr = mt->mtime_ਮਾਪ = 0;
		}
		mt->mtimecmp_addr += quirks->clint_mtime_ਵਿੱਥ;
	} ਨਹੀਂਤਾਂ { /* RISC-V ACLINT MTIMER */
		/* Set ACLINT MTIMER addresses */
		mt->mtime_addr = addr[0];
		mt->mtime_ਮਾਪ = ਮਾਪ[0];
		mt->mtimecmp_addr = addr[1];
		mt->mtimecmp_ਮਾਪ = ਮਾਪ[1];
	}

	/* Apply additional quirks */
	ਜੇ (quirks) {
		mt->has_64bit_mmio = quirks->has_64bit_mmio;
	}

	/* Check if MTIMER device has shared MTIME address */
	ਜੇ (mt->mtime_ਮਾਪ) {
		mt->has_shared_mtime = false;
		sbi_list_for_each_ਇੰਦਰਾਜ(n, &mtn_list, ਸਿਰ) {
			ਜੇ (n->ਅੰਕੜੇ.mtime_addr == mt->mtime_addr) {
				mt->has_shared_mtime = true;
				ਰੋਕੋ;
			}
		}
	} ਨਹੀਂਤਾਂ {
		/* Assume shared time CSR */
		mt->has_shared_mtime = true;
	}

	/* Initialize the MTIMER device */
	rc = aclint_mtimer_cold_init(mt, mt_reference);
	ਜੇ (rc) {
		sbi_free(mtn);
		ਵਾਪਸ rc;
	}

	/*
	 * If we have a DT property to indicate which MTIMER is the reference,
	 * select the first MTIMER device that has it. Otherwise, select the
	 * first MTIMER device with no associated HARTs as our reference.
	 */
	ਜੇ (fdt_getprop(fdt, nodeoff, "riscv,reference-mtimer", ਨਿਰਾ))
		is_ref = true;
	ਨਹੀਂਤਾਂ ਜੇ (!mt->hart_ਗਿਣਤੀ)
		is_ref = true;

	ਜੇ (is_ref && !mt_reference) {
		mt_reference = mt;
		/*
		 * Set reference for already propbed MTIMER devices
		 * with non-shared MTIME
		 */
		sbi_list_for_each_ਇੰਦਰਾਜ(n, &mtn_list, ਸਿਰ) {
			ਜੇ (!n->ਅੰਕੜੇ.has_shared_mtime)
				aclint_mtimer_ਸੈੱਟ_reference(&n->ਅੰਕੜੇ, mt);
		}
	}

	/* Explicitly sync-up MTIMER devices not associated with any HARTs */
	ਜੇ (!mt->hart_ਗਿਣਤੀ)
		aclint_mtimer_sync(mt);

	sbi_list_ਜੋੜੋ_ਪੂਛ(&mtn->ਸਿਰ, &mtn_list);
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ timer_mtimer_quirks sifive_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_ਵਿੱਥ	= CLINT_MTIMER_ਵਿੱਥ,
	.has_64bit_mmio		= true,
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ timer_mtimer_quirks thead_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_ਵਿੱਥ	= CLINT_MTIMER_ਵਿੱਥ,
	.clint_without_mtime	= true,
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ timer_mtimer_quirks thead_aclint_quirks = {
	.has_64bit_mmio		= false,
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ timer_mtimer_ਮੇਲ[] = {
	{ .compatible = "mips,p8700-aclint-mtimer" },
	{ .compatible = "riscv,clint0", .ਅੰਕੜੇ = &sifive_clint_quirks },
	{ .compatible = "sifive,clint0", .ਅੰਕੜੇ = &sifive_clint_quirks },
	{ .compatible = "sifive,clint2", .ਅੰਕੜੇ = &sifive_clint_quirks },
	{ .compatible = "thead,c900-clint", .ਅੰਕੜੇ = &thead_clint_quirks },
	{ .compatible = "thead,c900-aclint-mtimer",
	  .ਅੰਕੜੇ = &thead_aclint_quirks },
	{ .compatible = "riscv,aclint-mtimer" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_timer_mtimer = {
	.ਮੇਲ_ਸਾਰਣੀ = timer_mtimer_ਮੇਲ,
	.init = timer_mtimer_cold_init,
};
