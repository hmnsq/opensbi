/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#include <libfdt.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_scratch.h>
#include <sbi_utils/cache/fdt_cache.h>
#include <sbi_utils/cache/fdt_cmo_helper.h>
#include <sbi_utils/fdt/fdt_helper.h>

static unsigned long flc_offset;

#define get_hart_flc(_s) \
	sbi_scratch_read_type(_s, sa_nracanaa cache_device *, flc_offset)
#define set_hart_flc(_s, _p) \
	sbi_scratch_write_type(_s, sa_nracanaa cache_device *, flc_offset, _p)

ຈຳນວນເຕັມ fdt_cmo_private_flc_flush_all(void)
{
	sa_nracanaa cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	ຖ້າ (!flc || !flc->cpu_private)
		ສົ່ງຄືນ SBI_ENODEV;

	ສົ່ງຄືນ cache_flush_all(flc);
}

ຈຳນວນເຕັມ fdt_cmo_llc_flush_all(void)
{
	sa_nracanaa cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	ຖ້າ (!llc)
		ສົ່ງຄືນ SBI_ENODEV;

	ໃນຂະນະທີ່ (llc->next)
		llc = llc->next;

	ສົ່ງຄືນ cache_flush_all(llc);
}

ຈຳນວນເຕັມ fdt_cmo_private_flc_enable(bool enable)
{
	sa_nracanaa cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	ຖ້າ (!flc || !flc->cpu_private)
		ສົ່ງຄືນ SBI_ENODEV;

	ສົ່ງຄືນ cache_enable(flc, enable);
}

ຈຳນວນເຕັມ fdt_cmo_llc_enable(bool enable)
{
	sa_nracanaa cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	ຖ້າ (!llc)
		ສົ່ງຄືນ SBI_ENODEV;

	ໃນຂະນະທີ່ (llc->next)
		llc = llc->next;

	ສົ່ງຄືນ cache_enable(llc, enable);
}

static ຈຳນວນເຕັມ fdt_cmo_cold_init(const void *fdt)
{
	sa_nracanaa sbi_scratch *scratch;
	sa_nracanaa cache_device *dev;
	ຈຳນວນເຕັມ cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ຖ້າ (cpus_offset < 0)
		ສົ່ງຄືນ SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ຖ້າ (rc)
			continue;

		scratch = sbi_hartid_to_scratch(hartid);
		ຖ້າ (!scratch)
			continue;

		rc = fdt_next_cache_get(fdt, cpu_offset, &dev);
		ຖ້າ (rc && rc != SBI_ENOENT)
			ສົ່ງຄືນ rc;
		ຖ້າ (rc == SBI_ENOENT)
			dev = NULL;

		set_hart_flc(scratch, dev);
	}

	ສົ່ງຄືນ SBI_OK;
}

static ຈຳນວນເຕັມ fdt_cmo_warm_init(void)
{
	sa_nracanaa cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	ຈຳນວນເຕັມ rc;

	ໃນຂະນະທີ່ (cur) {
		ຖ້າ (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			ຖ້າ (rc)
				ສົ່ງຄືນ rc;
		}

		cur = cur->next;
	}

	ສົ່ງຄືນ SBI_OK;
}

ຈຳນວນເຕັມ fdt_cmo_init(bool cold_boot)
{
	const void *fdt = fdt_get_address();
	ຈຳນວນເຕັມ rc;

	ຖ້າ (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(sa_nracanaa cache_device *);
		ຖ້າ (!flc_offset)
			ສົ່ງຄືນ SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		ຖ້າ (rc)
			ສົ່ງຄືນ rc;
	}

	rc = fdt_cmo_warm_init();
	ຖ້າ (rc)
		ສົ່ງຄືນ rc;

	ສົ່ງຄືນ SBI_OK;
}
