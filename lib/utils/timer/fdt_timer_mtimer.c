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
	अचिह्नित पूर्णाङ्क	clint_mtime_विस्थापनम्;
	bool		clint_without_mtime;
	bool		has_64bit_mmio;
};

संरचना timer_mtimer_ग्रन्थिः {
	संरचना sbi_dlist शीर्ष;
	संरचना aclint_mtimer_दत्त दत्त;
};
स्थिर SBI_LIST_शीर्ष(mtn_list);

स्थिर संरचना aclint_mtimer_दत्त *mt_reference = रिक्त;

स्थिर पूर्णाङ्क timer_mtimer_cold_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
				  ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	पूर्णाङ्क rc;
	अचिह्नित दीर्घ addr[2], आकार[2];
	संरचना timer_mtimer_ग्रन्थिः *mtn, *n;
	संरचना aclint_mtimer_दत्त *mt;
	ध्रुव संरचना timer_mtimer_quirks *quirks = मेलनम्->दत्त;
	bool is_clint = quirks && quirks->is_clint;
	bool is_ref = false;

	mtn = sbi_zalloc(परिमाण(*mtn));
	यदि (!mtn)
		प्रत्यागम SBI_ENOMEM;
	mt = &mtn->दत्त;

	rc = fdt_parse_aclint_ग्रन्थिः(fdt, nodeoff, true, !is_clint,
				   &addr[0], &आकार[0], &addr[1], &आकार[1],
				   &mt->first_hartid, &mt->hart_गणनम्);
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
		mt->mtimecmp_addr = addr[0] + ACLINT_DEFAULT_MTIMECMP_विस्थापनम्;
		mt->mtimecmp_आकार = ACLINT_DEFAULT_MTIMECMP_आकार;
		यदि (!quirks->clint_without_mtime) {
			mt->mtime_addr = addr[0] + ACLINT_DEFAULT_MTIME_विस्थापनम्;
			mt->mtime_आकार = आकार[0] - mt->mtimecmp_आकार;
			/* Adjust MTIMER address and size for CLINT device */
			mt->mtime_addr += quirks->clint_mtime_विस्थापनम्;
			mt->mtime_आकार -= quirks->clint_mtime_विस्थापनम्;
		} अन्यथा {
			mt->mtime_addr = mt->mtime_आकार = 0;
		}
		mt->mtimecmp_addr += quirks->clint_mtime_विस्थापनम्;
	} अन्यथा { /* RISC-V ACLINT MTIMER */
		/* Set ACLINT MTIMER addresses */
		mt->mtime_addr = addr[0];
		mt->mtime_आकार = आकार[0];
		mt->mtimecmp_addr = addr[1];
		mt->mtimecmp_आकार = आकार[1];
	}

	/* Apply additional quirks */
	यदि (quirks) {
		mt->has_64bit_mmio = quirks->has_64bit_mmio;
	}

	/* Check if MTIMER device has shared MTIME address */
	यदि (mt->mtime_आकार) {
		mt->has_shared_mtime = false;
		sbi_list_for_each_प्रविष्टिः(n, &mtn_list, शीर्ष) {
			यदि (n->दत्त.mtime_addr == mt->mtime_addr) {
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
	अन्यथा यदि (!mt->hart_गणनम्)
		is_ref = true;

	यदि (is_ref && !mt_reference) {
		mt_reference = mt;
		/*
		 * Set reference for already propbed MTIMER devices
		 * with non-shared MTIME
		 */
		sbi_list_for_each_प्रविष्टिः(n, &mtn_list, शीर्ष) {
			यदि (!n->दत्त.has_shared_mtime)
				aclint_mtimer_समुच्चयः_reference(&n->दत्त, mt);
		}
	}

	/* Explicitly sync-up MTIMER devices not associated with any HARTs */
	यदि (!mt->hart_गणनम्)
		aclint_mtimer_sync(mt);

	sbi_list_सङ्कलय_पुच्छ(&mtn->शीर्ष, &mtn_list);
	प्रत्यागम 0;
}

स्थिर ध्रुव संरचना timer_mtimer_quirks sifive_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_विस्थापनम्	= CLINT_MTIMER_विस्थापनम्,
	.has_64bit_mmio		= true,
};

स्थिर ध्रुव संरचना timer_mtimer_quirks thead_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_विस्थापनम्	= CLINT_MTIMER_विस्थापनम्,
	.clint_without_mtime	= true,
};

स्थिर ध्रुव संरचना timer_mtimer_quirks thead_aclint_quirks = {
	.has_64bit_mmio		= false,
};

स्थिर ध्रुव संरचना fdt_मेलनम् timer_mtimer_मेलनम्[] = {
	{ .compatible = "mips,p8700-aclint-mtimer" },
	{ .compatible = "riscv,clint0", .दत्त = &sifive_clint_quirks },
	{ .compatible = "sifive,clint0", .दत्त = &sifive_clint_quirks },
	{ .compatible = "sifive,clint2", .दत्त = &sifive_clint_quirks },
	{ .compatible = "thead,c900-clint", .दत्त = &thead_clint_quirks },
	{ .compatible = "thead,c900-aclint-mtimer",
	  .दत्त = &thead_aclint_quirks },
	{ .compatible = "riscv,aclint-mtimer" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_timer_mtimer = {
	.मेलनम्_सारणी = timer_mtimer_मेलनम्,
	.init = timer_mtimer_cold_init,
};
