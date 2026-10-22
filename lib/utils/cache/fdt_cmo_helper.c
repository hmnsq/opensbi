/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/cache/fdt_cache.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/cache/fdt_cmo_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flc_ਵਿੱਥ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_hart_flc(_s) \
	sbi_scratch_read_type(_s, ਬਣਤਰ cache_device *, flc_ਵਿੱਥ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੈੱਟ_hart_flc(_s, _p) \
	sbi_scratch_write_type(_s, ਬਣਤਰ cache_device *, flc_ਵਿੱਥ, _p)

ਪੂਰਨ fdt_cmo_ਨਿੱਜੀ_flc_flush_all(ਖਾਲੀ)
{
	ਬਣਤਰ cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	ਜੇ (!flc || !flc->cpu_ਨਿੱਜੀ)
		ਵਾਪਸ SBI_ENODEV;

	ਵਾਪਸ cache_flush_all(flc);
}

ਪੂਰਨ fdt_cmo_llc_flush_all(ਖਾਲੀ)
{
	ਬਣਤਰ cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	ਜੇ (!llc)
		ਵਾਪਸ SBI_ENODEV;

	ਜਦੋਂ (llc->ਅਗਲਾ)
		llc = llc->ਅਗਲਾ;

	ਵਾਪਸ cache_flush_all(llc);
}

ਪੂਰਨ fdt_cmo_ਨਿੱਜੀ_flc_enable(bool enable)
{
	ਬਣਤਰ cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	ਜੇ (!flc || !flc->cpu_ਨਿੱਜੀ)
		ਵਾਪਸ SBI_ENODEV;

	ਵਾਪਸ cache_enable(flc, enable);
}

ਪੂਰਨ fdt_cmo_llc_enable(bool enable)
{
	ਬਣਤਰ cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	ਜੇ (!llc)
		ਵਾਪਸ SBI_ENODEV;

	ਜਦੋਂ (llc->ਅਗਲਾ)
		llc = llc->ਅਗਲਾ;

	ਵਾਪਸ cache_enable(llc, enable);
}

ਸਥਿਰ ਪੂਰਨ fdt_cmo_cold_init(ਅਚਲ ਖਾਲੀ *fdt)
{
	ਬਣਤਰ sbi_scratch *scratch;
	ਬਣਤਰ cache_device *dev;
	ਪੂਰਨ cpu_ਵਿੱਥ, cpus_ਵਿੱਥ, rc;
	u32 hartid;

	cpus_ਵਿੱਥ = fdt_ਰਸਤਾ_ਵਿੱਥ(fdt, "/cpus");
	ਜੇ (cpus_ਵਿੱਥ < 0)
		ਵਾਪਸ SBI_EINVAL;

	fdt_for_each_subnode(cpu_ਵਿੱਥ, fdt, cpus_ਵਿੱਥ) {
		rc = fdt_parse_hart_id(fdt, cpu_ਵਿੱਥ, &hartid);
		ਜੇ (rc)
			ਜਾਰੀ;

		scratch = sbi_hartid_ਮੰਜ਼ਿਲ_scratch(hartid);
		ਜੇ (!scratch)
			ਜਾਰੀ;

		rc = fdt_ਅਗਲਾ_cache_get(fdt, cpu_ਵਿੱਥ, &dev);
		ਜੇ (rc && rc != SBI_ENOENT)
			ਵਾਪਸ rc;
		ਜੇ (rc == SBI_ENOENT)
			dev = ਨਿਰਾ;

		ਸੈੱਟ_hart_flc(scratch, dev);
	}

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਪੂਰਨ fdt_cmo_warm_init(ਖਾਲੀ)
{
	ਬਣਤਰ cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	ਪੂਰਨ rc;

	ਜਦੋਂ (cur) {
		ਜੇ (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			ਜੇ (rc)
				ਵਾਪਸ rc;
		}

		cur = cur->ਅਗਲਾ;
	}

	ਵਾਪਸ SBI_OK;
}

ਪੂਰਨ fdt_cmo_init(bool cold_boot)
{
	ਅਚਲ ਖਾਲੀ *fdt = fdt_get_ਪਤਾ();
	ਪੂਰਨ rc;

	ਜੇ (cold_boot) {
		flc_ਵਿੱਥ = sbi_scratch_alloc_type_ਵਿੱਥ(ਬਣਤਰ cache_device *);
		ਜੇ (!flc_ਵਿੱਥ)
			ਵਾਪਸ SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		ਜੇ (rc)
			ਵਾਪਸ rc;
	}

	rc = fdt_cmo_warm_init();
	ਜੇ (rc)
		ਵਾਪਸ rc;

	ਵਾਪਸ SBI_OK;
}
