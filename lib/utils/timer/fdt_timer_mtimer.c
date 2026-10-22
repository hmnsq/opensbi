/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <libfdt.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi/sbi_list.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/timer/fdt_timer.h>
#समावेश <sbi_utils/timer/aclint_mtimer.h>

काष्ठा timer_mtimer_quirks {
	bool		is_clint;
	अचिन्हित पूर्णांक	clint_mtime_offset;
	bool		clint_without_mtime;
	bool		has_64bit_mmio;
};

काष्ठा timer_mtimer_गाँठ {
	काष्ठा sbi_dlist शीर्ष;
	काष्ठा aclint_mtimer_तथ्य तथ्य;
};
अटल SBI_LIST_शीर्ष(mtn_list);

अटल काष्ठा aclint_mtimer_तथ्य *mt_reference = शून्य;

अटल पूर्णांक timer_mtimer_cold_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
				  स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक rc;
	अचिन्हित दीर्घ addr[2], आकार[2];
	काष्ठा timer_mtimer_गाँठ *mtn, *n;
	काष्ठा aclint_mtimer_तथ्य *mt;
	स्थिर काष्ठा timer_mtimer_quirks *quirks = match->तथ्य;
	bool is_clint = quirks && quirks->is_clint;
	bool is_ref = false;

	mtn = sbi_zalloc(माप(*mtn));
	अगर (!mtn)
		वापस SBI_ENOMEM;
	mt = &mtn->तथ्य;

	rc = fdt_parse_aclint_गाँठ(fdt, nodeoff, true, !is_clint,
				   &addr[0], &आकार[0], &addr[1], &आकार[1],
				   &mt->first_hartid, &mt->hart_गिनती);
	अगर (rc) {
		sbi_free(mtn);
		वापस rc;
	}
	mt->has_64bit_mmio = true;
	mt->has_shared_mtime = false;

	rc = fdt_parse_timebase_frequency(fdt, &mt->mtime_freq);
	अगर (rc) {
		sbi_free(mtn);
		वापस rc;
	}

	अगर (is_clint) { /* SiFive CLINT */
		/* Set CLINT addresses */
		mt->mtimecmp_addr = addr[0] + ACLINT_DEFAULT_MTIMECMP_OFFSET;
		mt->mtimecmp_आकार = ACLINT_DEFAULT_MTIMECMP_आकार;
		अगर (!quirks->clint_without_mtime) {
			mt->mtime_addr = addr[0] + ACLINT_DEFAULT_MTIME_OFFSET;
			mt->mtime_आकार = आकार[0] - mt->mtimecmp_आकार;
			/* Adjust MTIMER address and size for CLINT device */
			mt->mtime_addr += quirks->clint_mtime_offset;
			mt->mtime_आकार -= quirks->clint_mtime_offset;
		} अन्यथा {
			mt->mtime_addr = mt->mtime_आकार = 0;
		}
		mt->mtimecmp_addr += quirks->clint_mtime_offset;
	} अन्यथा { /* RISC-V ACLINT MTIMER */
		/* Set ACLINT MTIMER addresses */
		mt->mtime_addr = addr[0];
		mt->mtime_आकार = आकार[0];
		mt->mtimecmp_addr = addr[1];
		mt->mtimecmp_आकार = आकार[1];
	}

	/* Apply additional quirks */
	अगर (quirks) {
		mt->has_64bit_mmio = quirks->has_64bit_mmio;
	}

	/* Check if MTIMER device has shared MTIME address */
	अगर (mt->mtime_आकार) {
		mt->has_shared_mtime = false;
		sbi_list_for_each_entry(n, &mtn_list, शीर्ष) {
			अगर (n->तथ्य.mtime_addr == mt->mtime_addr) {
				mt->has_shared_mtime = true;
				अवरोध;
			}
		}
	} अन्यथा {
		/* Assume shared time CSR */
		mt->has_shared_mtime = true;
	}

	/* Initialize the MTIMER device */
	rc = aclint_mtimer_cold_init(mt, mt_reference);
	अगर (rc) {
		sbi_free(mtn);
		वापस rc;
	}

	/*
	 * If we have a DT property to indicate which MTIMER is the reference,
	 * select the first MTIMER device that has it. Otherwise, select the
	 * first MTIMER device with no associated HARTs as our reference.
	 */
	अगर (fdt_getprop(fdt, nodeoff, "riscv,reference-mtimer", शून्य))
		is_ref = true;
	अन्यथा अगर (!mt->hart_गिनती)
		is_ref = true;

	अगर (is_ref && !mt_reference) {
		mt_reference = mt;
		/*
		 * Set reference for already propbed MTIMER devices
		 * with non-shared MTIME
		 */
		sbi_list_for_each_entry(n, &mtn_list, शीर्ष) {
			अगर (!n->तथ्य.has_shared_mtime)
				aclint_mtimer_set_reference(&n->तथ्य, mt);
		}
	}

	/* Explicitly sync-up MTIMER devices not associated with any HARTs */
	अगर (!mt->hart_गिनती)
		aclint_mtimer_sync(mt);

	sbi_list_add_पुच्छ(&mtn->शीर्ष, &mtn_list);
	वापस 0;
}

अटल स्थिर काष्ठा timer_mtimer_quirks sifive_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_offset	= CLINT_MTIMER_OFFSET,
	.has_64bit_mmio		= true,
};

अटल स्थिर काष्ठा timer_mtimer_quirks thead_clint_quirks = {
	.is_clint		= true,
	.clint_mtime_offset	= CLINT_MTIMER_OFFSET,
	.clint_without_mtime	= true,
};

अटल स्थिर काष्ठा timer_mtimer_quirks thead_aclint_quirks = {
	.has_64bit_mmio		= false,
};

अटल स्थिर काष्ठा fdt_match timer_mtimer_match[] = {
	{ .compatible = "mips,p8700-aclint-mtimer" },
	{ .compatible = "riscv,clint0", .तथ्य = &sifive_clint_quirks },
	{ .compatible = "sifive,clint0", .तथ्य = &sifive_clint_quirks },
	{ .compatible = "sifive,clint2", .तथ्य = &sifive_clint_quirks },
	{ .compatible = "thead,c900-clint", .तथ्य = &thead_clint_quirks },
	{ .compatible = "thead,c900-aclint-mtimer",
	  .तथ्य = &thead_aclint_quirks },
	{ .compatible = "riscv,aclint-mtimer" },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_timer_mtimer = {
	.match_table = timer_mtimer_match,
	.init = timer_mtimer_cold_init,
};
