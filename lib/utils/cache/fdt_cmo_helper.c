/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi_utils/cache/fdt_cache.h>
#అన్తర్భావ <sbi_utils/cache/fdt_cmo_helper.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>

స్థిర గుర్తులేని దీర్ఘ flc_స్థానభ్రంశం;

#నిర్వచించు get_hart_flc(_s) \
	sbi_scratch_read_type(_s, నిర్మాణం cache_device *, flc_స్థానభ్రంశం)
#నిర్వచించు సమితి_hart_flc(_s, _p) \
	sbi_scratch_write_type(_s, నిర్మాణం cache_device *, flc_స్థానభ్రంశం, _p)

పూర్ణాంకం fdt_cmo_వ్యక్తిగత_flc_flush_all(శూన్య)
{
	నిర్మాణం cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	ఒకవేళ (!flc || !flc->cpu_వ్యక్తిగత)
		తిరిగి SBI_ENODEV;

	తిరిగి cache_flush_all(flc);
}

పూర్ణాంకం fdt_cmo_llc_flush_all(శూన్య)
{
	నిర్మాణం cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	ఒకవేళ (!llc)
		తిరిగి SBI_ENODEV;

	అయితే (llc->తదుపరి)
		llc = llc->తదుపరి;

	తిరిగి cache_flush_all(llc);
}

పూర్ణాంకం fdt_cmo_వ్యక్తిగత_flc_enable(bool enable)
{
	నిర్మాణం cache_device *flc = get_hart_flc(sbi_scratch_thishart_ptr());

	ఒకవేళ (!flc || !flc->cpu_వ్యక్తిగత)
		తిరిగి SBI_ENODEV;

	తిరిగి cache_enable(flc, enable);
}

పూర్ణాంకం fdt_cmo_llc_enable(bool enable)
{
	నిర్మాణం cache_device *llc = get_hart_flc(sbi_scratch_thishart_ptr());

	ఒకవేళ (!llc)
		తిరిగి SBI_ENODEV;

	అయితే (llc->తదుపరి)
		llc = llc->తదుపరి;

	తిరిగి cache_enable(llc, enable);
}

స్థిర పూర్ణాంకం fdt_cmo_cold_init(స్థిరరాశి శూన్య *fdt)
{
	నిర్మాణం sbi_scratch *scratch;
	నిర్మాణం cache_device *dev;
	పూర్ణాంకం cpu_స్థానభ్రంశం, cpus_స్థానభ్రంశం, rc;
	u32 hartid;

	cpus_స్థానభ్రంశం = fdt_మార్గం_స్థానభ్రంశం(fdt, "/cpus");
	ఒకవేళ (cpus_స్థానభ్రంశం < 0)
		తిరిగి SBI_EINVAL;

	fdt_for_each_subnode(cpu_స్థానభ్రంశం, fdt, cpus_స్థానభ్రంశం) {
		rc = fdt_parse_hart_id(fdt, cpu_స్థానభ్రంశం, &hartid);
		ఒకవేళ (rc)
			కొనసాగు;

		scratch = sbi_hartid_గమ్యం_scratch(hartid);
		ఒకవేళ (!scratch)
			కొనసాగు;

		rc = fdt_తదుపరి_cache_get(fdt, cpu_స్థానభ్రంశం, &dev);
		ఒకవేళ (rc && rc != SBI_ENOENT)
			తిరిగి rc;
		ఒకవేళ (rc == SBI_ENOENT)
			dev = ఖాళీ;

		సమితి_hart_flc(scratch, dev);
	}

	తిరిగి SBI_OK;
}

స్థిర పూర్ణాంకం fdt_cmo_warm_init(శూన్య)
{
	నిర్మాణం cache_device *cur = get_hart_flc(sbi_scratch_thishart_ptr());
	పూర్ణాంకం rc;

	అయితే (cur) {
		ఒకవేళ (cur->ops && cur->ops->warm_init) {
			rc = cur->ops->warm_init(cur);
			ఒకవేళ (rc)
				తిరిగి rc;
		}

		cur = cur->తదుపరి;
	}

	తిరిగి SBI_OK;
}

పూర్ణాంకం fdt_cmo_init(bool cold_boot)
{
	స్థిరరాశి శూన్య *fdt = fdt_get_చిరునామా();
	పూర్ణాంకం rc;

	ఒకవేళ (cold_boot) {
		flc_స్థానభ్రంశం = sbi_scratch_alloc_type_స్థానభ్రంశం(నిర్మాణం cache_device *);
		ఒకవేళ (!flc_స్థానభ్రంశం)
			తిరిగి SBI_ENOMEM;

		rc = fdt_cmo_cold_init(fdt);
		ఒకవేళ (rc)
			తిరిగి rc;
	}

	rc = fdt_cmo_warm_init();
	ఒకవేళ (rc)
		తిరిగి rc;

	తిరిగి SBI_OK;
}
