/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_ecall.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_string.h>
#ضمن <sbi/sbi_trap.h>

خارجي بنية sbi_ecall_extension *ثابت sbi_ecall_exts[];

u16 sbi_ecall_نسخه_major(فراغ)
{
	أرجع SBI_ECALL_نسخه_MAJOR;
}

u16 sbi_ecall_نسخه_minor(فراغ)
{
	أرجع SBI_ECALL_نسخه_MINOR;
}

ساكن بیعلامت دراز ecall_impid = SBI_OPENSBI_IMPID;

بیعلامت دراز sbi_ecall_get_impid(فراغ)
{
	أرجع ecall_impid;
}

فراغ sbi_ecall_مجموعه_impid(بیعلامت دراز impid)
{
	ecall_impid = impid;
}

ساكن SBI_LIST_سر(ecall_exts_list);

بنية sbi_ecall_extension *sbi_ecall_پیدا_کن_extension(بیعلامت دراز extid)
{
	بنية sbi_ecall_extension *t, *ret = عدم;

	sbi_list_for_each_مدخل(t, &ecall_exts_list, سر) {
		إذا (t->extid_شروع <= extid && extid <= t->extid_انتها) {
			ret = t;
			اكسر;
		}
	}

	أرجع ret;
}

فراغ sbi_ecall_get_extensions_str(حرف *exts_str, صحيح exts_str_اندازه, bool experimental)
{
	بنية sbi_ecall_extension *t;
	صحيح جابه_جایی = 0;

	إذا (!exts_str || exts_str_اندازه <= 0)
		أرجع;
	sbi_memset(exts_str, 0, exts_str_اندازه);

	sbi_list_for_each_مدخل(t, &ecall_exts_list, سر) {
		إذا (experimental != t->experimental)
			استمر;
		إذا (جابه_جایی + sbi_strlen(t->name) + 1 > exts_str_اندازه)
			اكسر;
		sbi_snprintf(exts_str + جابه_جایی, exts_str_اندازه - جابه_جایی,
			     "%s,", t->name);
		جابه_جایی = جابه_جایی + sbi_strlen(t->name) + 1;
	}

	إذا (جابه_جایی)
		exts_str[جابه_جایی - 1] = '\0';
	وإلا
		sbi_strncpy(exts_str, "none", exts_str_اندازه);
}

صحيح sbi_ecall_ثبات_extension(بنية sbi_ecall_extension *ext)
{
	بنية sbi_ecall_extension *t;

	إذا (!ext || (ext->extid_انتها < ext->extid_شروع) || !ext->handle)
		أرجع SBI_EINVAL;

	sbi_list_for_each_مدخل(t, &ecall_exts_list, سر) {
		بیعلامت دراز شروع = t->extid_شروع;
		بیعلامت دراز انتها = t->extid_انتها;
		إذا (انتها < ext->extid_شروع || ext->extid_انتها < شروع)
			/* no overlap */;
		وإلا
			أرجع SBI_EINVAL;
	}

	sbi_list_جمع_کن_دم(&ext->سر, &ecall_exts_list);

	أرجع 0;
}

فراغ sbi_ecall_unregister_extension(بنية sbi_ecall_extension *ext)
{
	bool یافت = false;
	بنية sbi_ecall_extension *t;

	إذا (!ext)
		أرجع;

	sbi_list_for_each_مدخل(t, &ecall_exts_list, سر) {
		إذا (t == ext) {
			یافت = true;
			اكسر;
		}
	}

	إذا (یافت)
		sbi_list_del_init(&ext->سر);
}

صحيح sbi_ecall_handler(بنية sbi_trap_context *tcntx)
{
	صحيح ret = 0;
	بنية sbi_trap_regs *regs = &tcntx->regs;
	بنية sbi_ecall_extension *ext;
	بیعلامت دراز extension_id = regs->a7;
	بیعلامت دراز func_id = regs->a6;
	بنية sbi_ecall_return بیرون = {0};
	bool is_0_1_spec = 0;

	ext = sbi_ecall_پیدا_کن_extension(extension_id);
	إذا (ext && ext->handle) {
		ret = ext->handle(extension_id, func_id, regs, &بیرون);
		إذا (extension_id >= SBI_EXT_0_1_مجموعه_TIMER &&
		    extension_id <= SBI_EXT_0_1_SHUTDOWN)
			is_0_1_spec = 1;
	} وإلا {
		ret = SBI_ENOTSUPP;
	}

	إذا (!بیرون.skip_regs_update) {
		إذا (ret < SBI_آخرین_ERR ||
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
		إذا (!is_0_1_spec)
			regs->a1 = بیرون.مقدار;
	}

	أرجع 0;
}

صحيح sbi_ecall_init(فراغ)
{
	صحيح ret;
	بنية sbi_ecall_extension *ext;
	بیعلامت دراز i;

	لكل (i = 0; sbi_ecall_exts[i]; i++) {
		ext = sbi_ecall_exts[i];
		ret = SBI_ENODEV;

		إذا (ext->ثبات_extensions)
			ret = ext->ثبات_extensions();
		إذا (ret)
			أرجع ret;
	}

	أرجع 0;
}
