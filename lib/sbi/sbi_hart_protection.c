/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_hart_protection.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi/sbi_string.h>

స్థిర SBI_LIST_తల(hart_protection_list);

స్థిర నిర్మాణం sbi_hart_protection *__hart_memory_protection_best(శూన్య)
{
	నిర్మాణం sbi_hart_protection *pos;

	sbi_list_for_each_నమోదు(pos, &hart_protection_list, తల) {
		ఒకవేళ (pos->type == SBI_HART_PROTECTION_TYPE_జ్ఞాపకం)
			తిరిగి pos;
	}

	తిరిగి ఖాళీ;
}

శూన్య sbi_hart_protection_get_str(అక్షరం *బహిర్గతం_str, పూర్ణాంకం బహిర్గతం_str_కొలత)
{
	bool జ్ఞాపకం_protect_పూర్తి = false;
	నిర్మాణం sbi_hart_protection *pos;
	పూర్ణాంకం స్థానభ్రంశం = 0;

	ఒకవేళ (!బహిర్గతం_str || బహిర్గతం_str_కొలత <= 0)
		తిరిగి;
	sbi_memset(బహిర్గతం_str, 0, బహిర్గతం_str_కొలత);

	sbi_list_for_each_నమోదు(pos, &hart_protection_list, తల) {
		ఒకవేళ (pos->type == SBI_HART_PROTECTION_TYPE_జ్ఞాపకం) {
			ఒకవేళ (జ్ఞాపకం_protect_పూర్తి)
				కొనసాగు;
			జ్ఞాపకం_protect_పూర్తి = true;
		}
		sbi_snprintf(బహిర్గతం_str + స్థానభ్రంశం, బహిర్గతం_str_కొలత - స్థానభ్రంశం, "%s,", pos->name);
		స్థానభ్రంశం = స్థానభ్రంశం + sbi_strlen(pos->name) + 1;
	}

	ఒకవేళ (స్థానభ్రంశం)
		బహిర్గతం_str[స్థానభ్రంశం - 1] = '\0';
	లేకపోతే
		sbi_strncpy(బహిర్గతం_str, "none", బహిర్గతం_str_కొలత);
}

పూర్ణాంకం sbi_hart_protection_రిజిస్టరు(నిర్మాణం sbi_hart_protection *hprot)
{
	నిర్మాణం sbi_hart_protection *pos = ఖాళీ;
	bool దొరికింది_pos = false;

	ఒకవేళ (!hprot)
		తిరిగి SBI_EINVAL;
	ఒకవేళ (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		తిరిగి SBI_EINVAL;

	sbi_list_for_each_నమోదు(pos, &hart_protection_list, తల) {
		ఒకవేళ (hprot->rating > pos->rating) {
			దొరికింది_pos = true;
			ఆపు;
		}
	}

	ఒకవేళ (దొరికింది_pos)
		sbi_list_కూడు_తోక(&hprot->తల, &pos->తల);
	లేకపోతే
		sbi_list_కూడు_తోక(&hprot->తల, &hart_protection_list);

	తిరిగి 0;
}

శూన్య sbi_hart_protection_unregister(నిర్మాణం sbi_hart_protection *hprot)
{
	ఒకవేళ (!hprot)
		తిరిగి;

	sbi_list_del(&hprot->తల);
}

స్థిర పూర్ణాంకం __hart_protection_configure(నిర్మాణం sbi_scratch *scratch,
				       నిర్మాణం sbi_hart_protection *hprot,
				       నిర్మాణం sbi_domain *dom)
{
	ఒకవేళ (!hprot)
		తిరిగి 0;
	ఒకవేళ (!hprot->configure)
		తిరిగి SBI_ENOSYS;

	తిరిగి hprot->configure(scratch, dom);
}

స్థిర శూన్య __hart_protection_unconfigure(నిర్మాణం sbi_scratch *scratch,
					  నిర్మాణం sbi_hart_protection *hprot,
					  నిర్మాణం sbi_domain *dom)
{
	ఒకవేళ (!hprot || !hprot->unconfigure)
		తిరిగి;

	hprot->unconfigure(scratch, dom);
}

పూర్ణాంకం sbi_hart_protection_configure(నిర్మాణం sbi_scratch *scratch,
				  నిర్మాణం sbi_domain *dom)
{
	bool do_configure, జ్ఞాపకం_protect_పూర్తి = false;
	నిర్మాణం sbi_hart_protection *hprot;
	పూర్ణాంకం ret;

	sbi_list_for_each_నమోదు(hprot, &hart_protection_list, తల) {
		do_configure = false;
		ఎంపిక (hprot->type) {
		స్థితి SBI_HART_PROTECTION_TYPE_జ్ఞాపకం:
			do_configure = !జ్ఞాపకం_protect_పూర్తి;
			జ్ఞాపకం_protect_పూర్తి = true;
			ఆపు;
		స్థితి SBI_HART_PROTECTION_TYPE_ID:
			do_configure = true;
			ఆపు;
		అప్రమేయ:
			ఆపు;
		}
		ఒకవేళ (!do_configure)
			కొనసాగు;

		ret = __hart_protection_configure(scratch, hprot, dom);
		ఒకవేళ (ret)
			తిరిగి ret;
	}

	తిరిగి 0;
}

శూన్య sbi_hart_protection_unconfigure(నిర్మాణం sbi_scratch *scratch,
				     నిర్మాణం sbi_domain *dom)
{

	bool do_unconfigure, జ్ఞాపకం_protect_పూర్తి = false;
	నిర్మాణం sbi_hart_protection *hprot;

	sbi_list_for_each_నమోదు(hprot, &hart_protection_list, తల) {
		do_unconfigure = false;
		ఎంపిక (hprot->type) {
		స్థితి SBI_HART_PROTECTION_TYPE_జ్ఞాపకం:
			do_unconfigure = !జ్ఞాపకం_protect_పూర్తి;
			జ్ఞాపకం_protect_పూర్తి = true;
			ఆపు;
		స్థితి SBI_HART_PROTECTION_TYPE_ID:
			do_unconfigure = true;
			ఆపు;
		అప్రమేయ:
			ఆపు;
		}
		ఒకవేళ (!do_unconfigure)
			కొనసాగు;

		__hart_protection_unconfigure(scratch, hprot, dom);
	}
}

పూర్ణాంకం sbi_hart_protection_reconfigure(నిర్మాణం sbi_scratch *scratch,
				    నిర్మాణం sbi_domain *current_dom,
				    నిర్మాణం sbi_domain *తదుపరి_dom)
{
	bool do_reconfigure, జ్ఞాపకం_protect_పూర్తి = false;
	నిర్మాణం sbi_hart_protection *hprot;
	పూర్ణాంకం ret;

	sbi_list_for_each_నమోదు(hprot, &hart_protection_list, తల) {
		do_reconfigure = false;
		ఎంపిక (hprot->type) {
		స్థితి SBI_HART_PROTECTION_TYPE_జ్ఞాపకం:
			do_reconfigure = !జ్ఞాపకం_protect_పూర్తి;
			జ్ఞాపకం_protect_పూర్తి = true;
			ఆపు;
		స్థితి SBI_HART_PROTECTION_TYPE_ID:
			do_reconfigure = true;
			ఆపు;
		అప్రమేయ:
			ఆపు;
		}
		ఒకవేళ (!do_reconfigure)
			కొనసాగు;

		__hart_protection_unconfigure(scratch, hprot, current_dom);
		ret = __hart_protection_configure(scratch, hprot, తదుపరి_dom);
		ఒకవేళ (ret)
			తిరిగి ret;
	}

	తిరిగి 0;
}

పూర్ణాంకం sbi_hart_protection_తాత్కాలిక_map_range(గుర్తులేని దీర్ఘ పునాది_2, గుర్తులేని దీర్ఘ కొలత)
{
	నిర్మాణం sbi_hart_protection *hprot = __hart_memory_protection_best();

	ఒకవేళ (!hprot || !hprot->తాత్కాలిక_map_range)
		తిరిగి 0;

	తిరిగి hprot->తాత్కాలిక_map_range(sbi_scratch_thishart_ptr(), పునాది_2, కొలత);
}

పూర్ణాంకం sbi_hart_protection_తాత్కాలిక_unmap_range(గుర్తులేని దీర్ఘ పునాది_2, గుర్తులేని దీర్ఘ కొలత)
{
	నిర్మాణం sbi_hart_protection *hprot = __hart_memory_protection_best();

	ఒకవేళ (!hprot || !hprot->తాత్కాలిక_unmap_range)
		తిరిగి 0;

	తిరిగి hprot->తాత్కాలిక_unmap_range(sbi_scratch_thishart_ptr(), పునాది_2, కొలత);
}
