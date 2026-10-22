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
	sbi_scratch_read_type(_s, struct cache_device *, flc_offset)
#define set_hart_flc(_s, _p) \
	sbi_scratch_write_type(_s, struct cache_device *, flc_offset, _p)

ціле fdt_cmo_private_flc_flush_all(void)
{
	struct cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	якщо (!flc || !flc->cpu_private)
		повернути SBI_ENODEV;

	повернути cache_flush_all(flc);
}

ціле fdt_cmo_llc_flush_all(void)
{
	struct cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	якщо (!llc)
		повернути SBI_ENODEV;

	поки (llc->next)
		llc = llc->next;

	повернути cache_flush_all(llc);
}

ціле fdt_cmo_private_flc_enable(bool enable)
{
	struct cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	якщо (!flc || !flc->cpu_private)
		повернути SBI_ENODEV;

	повернути cache_enable(flc, enable);
}

ціле fdt_cmo_llc_enable(bool enable)
{
	struct cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	якщо (!llc)
		повернути SBI_ENODEV;

	поки (llc->next)
		llc = llc->next;

	повернути cache_enable(llc, enable);
}

static ціле fdt_cmo_cold_init(const void *fdt)
{
	struct sbi_scratch *scratch;
	struct cache_device *dev;
	ціле cpu_offset, cpus_offset, rc;
	u32 hartid;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	якщо (cpus_offset < 0)
		повернути SBI_EINVAL;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		якщо (rc)
			continue;

		scratch = sbi_hartid_to_scratch(hartid);
		якщо (!scratch)
			continue;

		rc = fdt_next_cache_get(fdt, cpu_offset, &dev);
		якщо (rc && rc != SBI_ENOENT)
			повернути rc;
		якщо (rc == SBI_ENOENT)
			dev = NULL;

		set_hart_flc(scratch, dev);
	}

	повернути SBI_OK;
}

static ціле fdt_cmo_warm_init(void)
{
	struct cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	ціле rc;

	поки (cur) {
		якщо (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			якщо (rc)
				повернути rc;
		}

		cur = cur->next;
	}

	повернути SBI_OK;
}

ціле fdt_cmo_init(bool cold_boot)
{
	const void *fdt = fdt_get_address();
	ціле rc;

	якщо (cold_boot) {
		flc_offset = sbi_scratch_alloc_type_offset(struct cache_device *);
		якщо (!flc_offset)
			повернути SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		якщо (rc)
			повернути rc;
	}

	rc = fdt_cmo_warm_init();
	якщо (rc)
		повернути rc;

	повернути SBI_OK;
}
