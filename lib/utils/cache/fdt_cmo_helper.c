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

ចំនួនគត់ fdt_cmo_private_flc_flush_all(void)
{
	sa_nracanaa cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	ប្រសិនបើ (!flc || !flc->cpu_private)
		ត្រឡប់ SBI_ENODEV;

	ត្រឡប់ cache_flush_all(flc);
}

ចំនួនគត់ fdt_cmo_llc_flush_all(void)
{
	sa_nracanaa cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	ប្រសិនបើ (!llc)
		ត្រឡប់ SBI_ENODEV;

	ខណៈពេល (llc->next)
		llc = llc->next;

	ត្រឡប់ cache_flush_all(llc);
}

ចំនួនគត់ fdt_cmo_private_flc_enable(bool enable)
{
	sa_nracanaa cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	ប្រសិនបើ (!flc || !flc->cpu_private)
		ត្រឡប់ SBI_ENODEV;

	ត្រឡប់ cache_enable(flc, enable);
}

ចំនួនគត់ fdt_cmo_llc_enable(bool enable)
{
	sa_nracanaa cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	ប្រសិនបើ (!llc)
		ត្រឡប់ SBI_ENODEV;

	ខណៈពេល (llc->next)
		llc = llc->next;

	ត្រឡប់ cache_enable(llc, enable);
}

static ចំនួនគត់ fdt_cmo_cold_init(const void *fdt)
{
	sa_nracanaa sbi_scratch *scratch;
	sa_nracanaa cache_device *dev;
	ចំនួនគត់ cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ប្រសិនបើ (cpus_offset < 0)
		ត្រឡប់ SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ប្រសិនបើ (rc)
			continue;

		scratch = sbi_hartid_to_scratch(hartid);
		ប្រសិនបើ (!scratch)
			continue;

		rc = fdt_next_cache_get(fdt, cpu_offset, &dev);
		ប្រសិនបើ (rc && rc != SBI_ENOENT)
			ត្រឡប់ rc;
		ប្រសិនបើ (rc == SBI_ENOENT)
			dev = NULL;

		set_hart_flc(scratch, dev);
	}

	ត្រឡប់ SBI_OK;
}

static ចំនួនគត់ fdt_cmo_warm_init(void)
{
	sa_nracanaa cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	ចំនួនគត់ rc;

	ខណៈពេល (cur) {
		ប្រសិនបើ (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			ប្រសិនបើ (rc)
				ត្រឡប់ rc;
		}

		cur = cur->next;
	}

	ត្រឡប់ SBI_OK;
}

ចំនួនគត់ fdt_cmo_init(bool cold_boot)
{
	const void *fdt = fdt_get_address();
	ចំនួនគត់ rc;

	ប្រសិនបើ (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(sa_nracanaa cache_device *);
		ប្រសិនបើ (!flc_offset)
			ត្រឡប់ SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		ប្រសិនបើ (rc)
			ត្រឡប់ rc;
	}

	rc = fdt_cmo_warm_init();
	ប្រសិនបើ (rc)
		ត្រឡប់ rc;

	ត្រឡប់ SBI_OK;
}
