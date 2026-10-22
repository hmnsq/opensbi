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

u16 sbi_ecall_نسخو_major(فراغ)
{
	أرجع SBI_ECALL_نسخو_MAJOR;
}

u16 sbi_ecall_نسخو_minor(فراغ)
{
	أرجع SBI_ECALL_نسخو_MINOR;
}

ساكن بنانشان ڊگهو ecall_impid = SBI_OPENSBI_IMPID;

بنانشان ڊگهو sbi_ecall_get_impid(فراغ)
{
	أرجع ecall_impid;
}

فراغ sbi_ecall_سيٽ_impid(بنانشان ڊگهو impid)
{
	ecall_impid = impid;
}

ساكن SBI_LIST_سر(ecall_exts_list);

بنية sbi_ecall_extension *sbi_ecall_ڳوليو_extension(بنانشان ڊگهو extid)
{
	بنية sbi_ecall_extension *t, *ret = عدم;

	sbi_list_for_each_داخلا(t, &ecall_exts_list, سر) {
		إذا (t->extid_شروعات <= extid && extid <= t->extid_پڇاڙي) {
			ret = t;
			اكسر;
		}
	}

	أرجع ret;
}

فراغ sbi_ecall_get_extensions_str(حرف *exts_str, صحيح exts_str_سائيز, bool experimental)
{
	بنية sbi_ecall_extension *t;
	صحيح وٿي = 0;

	إذا (!exts_str || exts_str_سائيز <= 0)
		أرجع;
	sbi_memset(exts_str, 0, exts_str_سائيز);

	sbi_list_for_each_داخلا(t, &ecall_exts_list, سر) {
		إذا (experimental != t->experimental)
			استمر;
		إذا (وٿي + sbi_strlen(t->name) + 1 > exts_str_سائيز)
			اكسر;
		sbi_snprintf(exts_str + وٿي, exts_str_سائيز - وٿي,
			     "%s,", t->name);
		وٿي = وٿي + sbi_strlen(t->name) + 1;
	}

	إذا (وٿي)
		exts_str[وٿي - 1] = '\0';
	وإلا
		sbi_strncpy(exts_str, "none", exts_str_سائيز);
}

صحيح sbi_ecall_رجسٽر_extension(بنية sbi_ecall_extension *ext)
{
	بنية sbi_ecall_extension *t;

	إذا (!ext || (ext->extid_پڇاڙي < ext->extid_شروعات) || !ext->handle)
		أرجع SBI_EINVAL;

	sbi_list_for_each_داخلا(t, &ecall_exts_list, سر) {
		بنانشان ڊگهو شروعات = t->extid_شروعات;
		بنانشان ڊگهو پڇاڙي = t->extid_پڇاڙي;
		إذا (پڇاڙي < ext->extid_شروعات || ext->extid_پڇاڙي < شروعات)
			/* no overlap */;
		وإلا
			أرجع SBI_EINVAL;
	}

	sbi_list_جوڙيو_پڇ(&ext->سر, &ecall_exts_list);

	أرجع 0;
}

فراغ sbi_ecall_unregister_extension(بنية sbi_ecall_extension *ext)
{
	bool مليو = false;
	بنية sbi_ecall_extension *t;

	إذا (!ext)
		أرجع;

	sbi_list_for_each_داخلا(t, &ecall_exts_list, سر) {
		إذا (t == ext) {
			مليو = true;
			اكسر;
		}
	}

	إذا (مليو)
		sbi_list_del_init(&ext->سر);
}

صحيح sbi_ecall_handler(بنية sbi_trap_context *tcntx)
{
	صحيح ret = 0;
	بنية sbi_trap_regs *regs = &tcntx->regs;
	بنية sbi_ecall_extension *ext;
	بنانشان ڊگهو extension_id = regs->a7;
	بنانشان ڊگهو func_id = regs->a6;
	بنية sbi_ecall_return ٻاهريون = {0};
	bool is_0_1_spec = 0;

	ext = sbi_ecall_ڳوليو_extension(extension_id);
	إذا (ext && ext->handle) {
		ret = ext->handle(extension_id, func_id, regs, &ٻاهريون);
		إذا (extension_id >= SBI_EXT_0_1_سيٽ_TIMER &&
		    extension_id <= SBI_EXT_0_1_SHUTDOWN)
			is_0_1_spec = 1;
	} وإلا {
		ret = SBI_ENOTSUPP;
	}

	إذا (!ٻاهريون.skip_regs_update) {
		إذا (ret < SBI_آخري_ERR ||
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
			regs->a1 = ٻاهريون.قدر;
	}

	أرجع 0;
}

صحيح sbi_ecall_init(فراغ)
{
	صحيح ret;
	بنية sbi_ecall_extension *ext;
	بنانشان ڊگهو i;

	لكل (i = 0; sbi_ecall_exts[i]; i++) {
		ext = sbi_ecall_exts[i];
		ret = SBI_ENODEV;

		إذا (ext->رجسٽر_extensions)
			ret = ext->رجسٽر_extensions();
		إذا (ret)
			أرجع ret;
	}

	أرجع 0;
}
