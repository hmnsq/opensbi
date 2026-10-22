/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_ecall.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_string.h>
#شامل <sbi/sbi_trap.h>

خارجی ساخت sbi_ecall_extension *ثابت sbi_ecall_exts[];

u16 sbi_ecall_نسخہ_major(خالی)
{
	واپس SBI_ECALL_نسخہ_MAJOR;
}

u16 sbi_ecall_نسخہ_minor(خالی)
{
	واپس SBI_ECALL_نسخہ_MINOR;
}

ساکن غیرعلامتی طویل ecall_impid = SBI_OPENSBI_IMPID;

غیرعلامتی طویل sbi_ecall_get_impid(خالی)
{
	واپس ecall_impid;
}

خالی sbi_ecall_سیٹ_impid(غیرعلامتی طویل impid)
{
	ecall_impid = impid;
}

ساکن SBI_LIST_سر(ecall_exts_list);

ساخت sbi_ecall_extension *sbi_ecall_ڈھونڈو_extension(غیرعلامتی طویل extid)
{
	ساخت sbi_ecall_extension *t, *ret = تہی;

	sbi_list_for_each_اندراج(t, &ecall_exts_list, سر) {
		اگر (t->extid_آغاز <= extid && extid <= t->extid_انتہا) {
			ret = t;
			توڑو;
		}
	}

	واپس ret;
}

خالی sbi_ecall_get_extensions_str(حرف *exts_str, صحیح exts_str_سائز, bool experimental)
{
	ساخت sbi_ecall_extension *t;
	صحیح انحراف = 0;

	اگر (!exts_str || exts_str_سائز <= 0)
		واپس;
	sbi_memset(exts_str, 0, exts_str_سائز);

	sbi_list_for_each_اندراج(t, &ecall_exts_list, سر) {
		اگر (experimental != t->experimental)
			جاری;
		اگر (انحراف + sbi_strlen(t->name) + 1 > exts_str_سائز)
			توڑو;
		sbi_snprintf(exts_str + انحراف, exts_str_سائز - انحراف,
			     "%s,", t->name);
		انحراف = انحراف + sbi_strlen(t->name) + 1;
	}

	اگر (انحراف)
		exts_str[انحراف - 1] = '\0';
	ورنہ
		sbi_strncpy(exts_str, "none", exts_str_سائز);
}

صحیح sbi_ecall_رجسٹر_extension(ساخت sbi_ecall_extension *ext)
{
	ساخت sbi_ecall_extension *t;

	اگر (!ext || (ext->extid_انتہا < ext->extid_آغاز) || !ext->handle)
		واپس SBI_EINVAL;

	sbi_list_for_each_اندراج(t, &ecall_exts_list, سر) {
		غیرعلامتی طویل آغاز = t->extid_آغاز;
		غیرعلامتی طویل انتہا = t->extid_انتہا;
		اگر (انتہا < ext->extid_آغاز || ext->extid_انتہا < آغاز)
			/* no overlap */;
		ورنہ
			واپس SBI_EINVAL;
	}

	sbi_list_جوڑو_دم(&ext->سر, &ecall_exts_list);

	واپس 0;
}

خالی sbi_ecall_unregister_extension(ساخت sbi_ecall_extension *ext)
{
	bool ملا = false;
	ساخت sbi_ecall_extension *t;

	اگر (!ext)
		واپس;

	sbi_list_for_each_اندراج(t, &ecall_exts_list, سر) {
		اگر (t == ext) {
			ملا = true;
			توڑو;
		}
	}

	اگر (ملا)
		sbi_list_del_init(&ext->سر);
}

صحیح sbi_ecall_handler(ساخت sbi_trap_context *tcntx)
{
	صحیح ret = 0;
	ساخت sbi_trap_regs *regs = &tcntx->regs;
	ساخت sbi_ecall_extension *ext;
	غیرعلامتی طویل extension_id = regs->a7;
	غیرعلامتی طویل func_id = regs->a6;
	ساخت sbi_ecall_return برآمد = {0};
	bool is_0_1_spec = 0;

	ext = sbi_ecall_ڈھونڈو_extension(extension_id);
	اگر (ext && ext->handle) {
		ret = ext->handle(extension_id, func_id, regs, &برآمد);
		اگر (extension_id >= SBI_EXT_0_1_سیٹ_TIMER &&
		    extension_id <= SBI_EXT_0_1_SHUTDOWN)
			is_0_1_spec = 1;
	} ورنہ {
		ret = SBI_ENOTSUPP;
	}

	اگر (!برآمد.skip_regs_update) {
		اگر (ret < SBI_آخری_ERR ||
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
		اگر (!is_0_1_spec)
			regs->a1 = برآمد.قدر;
	}

	واپس 0;
}

صحیح sbi_ecall_init(خالی)
{
	صحیح ret;
	ساخت sbi_ecall_extension *ext;
	غیرعلامتی طویل i;

	برائے (i = 0; sbi_ecall_exts[i]; i++) {
		ext = sbi_ecall_exts[i];
		ret = SBI_ENODEV;

		اگر (ext->رجسٹر_extensions)
			ret = ext->رجسٹر_extensions();
		اگر (ret)
			واپس ret;
	}

	واپس 0;
}
