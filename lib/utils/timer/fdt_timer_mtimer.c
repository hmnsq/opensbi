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

ರಚನೆ timer_mtimer_quirks {
	bool		is_clint;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ	clint_mtime_ಸ್ಥಾನಾಂತರ;
	bool		clint_without_mtime;
	bool		has_64bit_mmio;
};

ರಚನೆ timer_mtimer_ಗಂಟು {
	ರಚನೆ sbi_dlist ತಲೆ;
	ರಚನೆ aclint_mtimer_ದತ್ತಾಂಶ ದತ್ತಾಂಶ;
};
ಸ್ಥಿರ SBI_LIST_ತಲೆ(mtn_list);

ಸ್ಥಿರ ರಚನೆ aclint_mtimer_ದತ್ತಾಂಶ *mt_reference = ಖಾಲಿ;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ timer_mtimer_cold_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
				  ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಪೂರ್ಣಾಂಕ rc;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr[2], ಗಾತ್ರ_2[2];
	ರಚನೆ timer_mtimer_ಗಂಟು *mtn, *n;
	ರಚನೆ aclint_mtimer_ದತ್ತಾಂಶ *mt;
	ಸ್ಥಿರಾಂಕ ರಚನೆ timer_mtimer_quirks *quirks = ಹೊಂದಿಕೆ->ದತ್ತಾಂಶ;
	bool is_clint = quirks && quirks->is_clint;
	bool is_ref = false;

	mtn = sbi_zalloc(ಗಾತ್ರ(*mtn));
	ಆದರೆ (!mtn)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;
	mt = &mtn->ದತ್ತಾಂಶ;

	rc = fdt_parse_aclint_ಗಂಟು(fdt, nodeoff, true, !is_clint,
				   &addr[0], &ಗಾತ್ರ_2[0], &addr[1], &ಗಾತ್ರ_2[1],
				   &mt->ಮೊದಲ_hartid, &mt->hart_ಎಣಿಕೆ);
	ಆದರೆ (rc) {
		sbi_free(mtn);
		ಹಿಂತಿರುಗಿ rc;
	}
	mt->has_64bit_mmio = true;
	mt->has_shared_mtime = false;

	rc = fdt_parse_timebase_frequency(fdt, &mt->mtime_freq);
	ಆದರೆ (rc) {
		sbi_free(mtn);
		ಹಿಂತಿರುಗಿ rc;
	}

	ಆದರೆ (is_clint) { /* SiFive CLINT */
		/* Set CLINT addresses */
		mt->mtimecmp_addr = addr[0] + ACLINT_DEFAULT_MTIMECMP_ಸ್ಥಾನಾಂತರ;
		mt->mtimecmp_ಗಾತ್ರ = ACLINT_DEFAULT_MTIMECMP_ಗಾತ್ರ;
		ಆದರೆ (!quirks->clint_without_mtime) {
			mt->mtime_addr = addr[0] + ACLINT_DEFAULT_MTIME_ಸ್ಥಾನಾಂತರ;
			mt->mtime_ಗಾತ್ರ = ಗಾತ್ರ_2[0] - mt->mtimecmp_ಗಾತ್ರ;
			/* Adjust MTIMER address and size for CLINT device */
			mt->mtime_addr += quirks->clint_mtime_ಸ್ಥಾನಾಂತರ;
			mt->mtime_ಗಾತ್ರ -= quirks->clint_mtime_ಸ್ಥಾನಾಂತರ;
		} ಇಲ್ಲದಿದ್ದರೆ {
			mt->mtime_addr = mt->mtime_ಗಾತ್ರ = 0;
		}
		mt->mtimecmp_addr += quirks->clint_mtime_ಸ್ಥಾನಾಂತರ;
	} ಇಲ್ಲದಿದ್ದರೆ { /* RISC-V ACLINT MTIMER */
		/* Set ACLINT MTIMER addresses */
		mt->mtime_addr = addr[0];
		mt->mtime_ಗಾತ್ರ = ಗಾತ್ರ_2[0];
		mt->mtimecmp_addr = addr[1];
		mt->mtimecmp_ಗಾತ್ರ = ಗಾತ್ರ_2[1];
	}

	/* Apply additional quirks */
	ಆದರೆ (quirks) {
		mt->has_64bit_mmio = quirks->has_64bit_mmio;
	}

	/* Check if MTIMER device has shared MTIME address */
	ಆದರೆ (mt->mtime_ಗಾತ್ರ) {
		mt->has_shared_mtime = false;
		sbi_list_for_each_ನಮೂದು(n, &mtn_list, ತಲೆ) {
			ಆದರೆ (n->ದತ್ತಾಂಶ.mtime_addr == mt->mtime_addr) {
				mt->has_shared_mtime = true;
				ನಿಲ್ಲಿಸು;
			}
		}
	} ಇಲ್ಲದಿದ್ದರೆ {
		/* Assume shared time CSR */
		mt->has_shared_mtime = true;
	}

	/* Initialize the MTIMER device */
	rc = aclint_mtimer_cold_init(mt, mt_reference);
	ಆದರೆ (rc) {
		sbi_free(mtn);
		ಹಿಂತಿರುಗಿ rc;
	}

	/*
	 * If we have a DT property to indicate which MTIMER is the reference,
	 * select the first MTIMER device that has it. Otherwise, select the
	 * first MTIMER device with no associated HARTs as our reference.
	 */
	ಆದರೆ (fdt_getprop(fdt, nodeoff, "riscv,reference-mtimer", ಖಾಲಿ))
		is_ref = true;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!mt->hart_ಎಣಿಕೆ)
		is_ref = true;

	ಆದರೆ (is_ref && !mt_reference) {
		mt_reference = mt;
		/*
		 * Set reference for already propbed MTIMER devices
		 * with non-shared MTIME
		 */
		sbi_list_for_each_ನಮೂದು(n, &mtn_list, ತಲೆ) {
			ಆದರೆ (!n->ದತ್ತಾಂಶ.has_shared_mtime)
				aclint_mtimer_ಗಣ_reference(&n->ದತ್ತಾಂಶ, mt);
		}
	}

	/* Explicitly sync-up MTIMER devices not associated with any HARTs */
	ಆದರೆ (!mt->hart_ಎಣಿಕೆ)
		aclint_mtimer_sync(mt);

	sbi_list_ಕೂಡಿಸು_ಬಾಲ(&mtn->ತಲೆ, &mtn_list);
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ timer_mtimer_quirks sifive_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_ಸ್ಥಾನಾಂತರ	= CLINT_MTIMER_ಸ್ಥಾನಾಂತರ,
	.has_64bit_mmio		= true,
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ timer_mtimer_quirks thead_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_ಸ್ಥಾನಾಂತರ	= CLINT_MTIMER_ಸ್ಥಾನಾಂತರ,
	.clint_without_mtime	= true,
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ timer_mtimer_quirks thead_aclint_quirks = {
	.has_64bit_mmio		= false,
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ timer_mtimer_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "mips,p8700-aclint-mtimer" },
	{ .compatible = "riscv,clint0", .ದತ್ತಾಂಶ = &sifive_clint_quirks },
	{ .compatible = "sifive,clint0", .ದತ್ತಾಂಶ = &sifive_clint_quirks },
	{ .compatible = "sifive,clint2", .ದತ್ತಾಂಶ = &sifive_clint_quirks },
	{ .compatible = "thead,c900-clint", .ದತ್ತಾಂಶ = &thead_clint_quirks },
	{ .compatible = "thead,c900-aclint-mtimer",
	  .ದತ್ತಾಂಶ = &thead_aclint_quirks },
	{ .compatible = "riscv,aclint-mtimer" },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_timer_mtimer = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = timer_mtimer_ಹೊಂದಿಕೆ,
	.init = timer_mtimer_cold_init,
};
