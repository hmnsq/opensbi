/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_ecall.h>
#అన్తర్భావ <sbi/sbi_ecall_interface.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_string.h>
#అన్తర్భావ <sbi/sbi_trap.h>

బాహ్య నిర్మాణం sbi_ecall_extension *స్థిరరాశి sbi_ecall_exts[];

u16 sbi_ecall_సంస్కరణ_major(శూన్య)
{
	తిరిగి SBI_ECALL_సంస్కరణ_MAJOR;
}

u16 sbi_ecall_సంస్కరణ_minor(శూన్య)
{
	తిరిగి SBI_ECALL_సంస్కరణ_MINOR;
}

స్థిర గుర్తులేని దీర్ఘ ecall_impid = SBI_OPENSBI_IMPID;

గుర్తులేని దీర్ఘ sbi_ecall_get_impid(శూన్య)
{
	తిరిగి ecall_impid;
}

శూన్య sbi_ecall_సమితి_impid(గుర్తులేని దీర్ఘ impid)
{
	ecall_impid = impid;
}

స్థిర SBI_LIST_తల(ecall_exts_list);

నిర్మాణం sbi_ecall_extension *sbi_ecall_కనుగొను_extension(గుర్తులేని దీర్ఘ extid)
{
	నిర్మాణం sbi_ecall_extension *t, *ret = ఖాళీ;

	sbi_list_for_each_నమోదు(t, &ecall_exts_list, తల) {
		ఒకవేళ (t->extid_ప్రారంభం <= extid && extid <= t->extid_చివర) {
			ret = t;
			ఆపు;
		}
	}

	తిరిగి ret;
}

శూన్య sbi_ecall_get_extensions_str(అక్షరం *exts_str, పూర్ణాంకం exts_str_కొలత, bool experimental)
{
	నిర్మాణం sbi_ecall_extension *t;
	పూర్ణాంకం స్థానభ్రంశం = 0;

	ఒకవేళ (!exts_str || exts_str_కొలత <= 0)
		తిరిగి;
	sbi_memset(exts_str, 0, exts_str_కొలత);

	sbi_list_for_each_నమోదు(t, &ecall_exts_list, తల) {
		ఒకవేళ (experimental != t->experimental)
			కొనసాగు;
		ఒకవేళ (స్థానభ్రంశం + sbi_strlen(t->name) + 1 > exts_str_కొలత)
			ఆపు;
		sbi_snprintf(exts_str + స్థానభ్రంశం, exts_str_కొలత - స్థానభ్రంశం,
			     "%s,", t->name);
		స్థానభ్రంశం = స్థానభ్రంశం + sbi_strlen(t->name) + 1;
	}

	ఒకవేళ (స్థానభ్రంశం)
		exts_str[స్థానభ్రంశం - 1] = '\0';
	లేకపోతే
		sbi_strncpy(exts_str, "none", exts_str_కొలత);
}

పూర్ణాంకం sbi_ecall_రిజిస్టరు_extension(నిర్మాణం sbi_ecall_extension *ext)
{
	నిర్మాణం sbi_ecall_extension *t;

	ఒకవేళ (!ext || (ext->extid_చివర < ext->extid_ప్రారంభం) || !ext->handle)
		తిరిగి SBI_EINVAL;

	sbi_list_for_each_నమోదు(t, &ecall_exts_list, తల) {
		గుర్తులేని దీర్ఘ ప్రారంభం = t->extid_ప్రారంభం;
		గుర్తులేని దీర్ఘ చివర = t->extid_చివర;
		ఒకవేళ (చివర < ext->extid_ప్రారంభం || ext->extid_చివర < ప్రారంభం)
			/* no overlap */;
		లేకపోతే
			తిరిగి SBI_EINVAL;
	}

	sbi_list_కూడు_తోక(&ext->తల, &ecall_exts_list);

	తిరిగి 0;
}

శూన్య sbi_ecall_unregister_extension(నిర్మాణం sbi_ecall_extension *ext)
{
	bool దొరికింది = false;
	నిర్మాణం sbi_ecall_extension *t;

	ఒకవేళ (!ext)
		తిరిగి;

	sbi_list_for_each_నమోదు(t, &ecall_exts_list, తల) {
		ఒకవేళ (t == ext) {
			దొరికింది = true;
			ఆపు;
		}
	}

	ఒకవేళ (దొరికింది)
		sbi_list_del_init(&ext->తల);
}

పూర్ణాంకం sbi_ecall_handler(నిర్మాణం sbi_trap_context *tcntx)
{
	పూర్ణాంకం ret = 0;
	నిర్మాణం sbi_trap_regs *regs = &tcntx->regs;
	నిర్మాణం sbi_ecall_extension *ext;
	గుర్తులేని దీర్ఘ extension_id = regs->a7;
	గుర్తులేని దీర్ఘ func_id = regs->a6;
	నిర్మాణం sbi_ecall_return బహిర్గతం = {0};
	bool is_0_1_spec = 0;

	ext = sbi_ecall_కనుగొను_extension(extension_id);
	ఒకవేళ (ext && ext->handle) {
		ret = ext->handle(extension_id, func_id, regs, &బహిర్గతం);
		ఒకవేళ (extension_id >= SBI_EXT_0_1_సమితి_TIMER &&
		    extension_id <= SBI_EXT_0_1_SHUTDOWN)
			is_0_1_spec = 1;
	} లేకపోతే {
		ret = SBI_ENOTSUPP;
	}

	ఒకవేళ (!బహిర్గతం.skip_regs_update) {
		ఒకవేళ (ret < SBI_చివరి_ERR ||
		    (extension_id != SBI_EXT_0_1_CONSOLE_GETCHAR &&
		     SBI_SUCCESS < ret)) {
			sbi_printf("%s: Invalid error %d for ext=0x%lx "
				   "func=0x%lx\n", __func__, ret,
				   extension_id, func_id);
			ret = SBI_ERR_FAILED;
		}

		/*
		 * This function should return non-zero value only in case of
		 * fatal error. However, there is no good way to distinguish
		 * between a fatal and non-fatal errors yet. That's why we treat
		 * every return value except ETRAP as non-fatal and just return
		 * accordingly for now. Once fatal errors are defined, that
		 * case should be handled differently.
		 */
		regs->mepc += 4;
		regs->a0 = ret;
		ఒకవేళ (!is_0_1_spec)
			regs->a1 = బహిర్గతం.విలువ;
	}

	తిరిగి 0;
}

పూర్ణాంకం sbi_ecall_init(శూన్య)
{
	పూర్ణాంకం ret;
	నిర్మాణం sbi_ecall_extension *ext;
	గుర్తులేని దీర్ఘ i;

	కొరకు (i = 0; sbi_ecall_exts[i]; i++) {
		ext = sbi_ecall_exts[i];
		ret = SBI_ENODEV;

		ఒకవేళ (ext->రిజిస్టరు_extensions)
			ret = ext->రిజిస్టరు_extensions();
		ఒకవేళ (ret)
			తిరిగి ret;
	}

	తిరిగి 0;
}
