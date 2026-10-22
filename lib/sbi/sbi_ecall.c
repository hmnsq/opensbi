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

u16 sbi_ecall_نسخہ_major(فراغ)
{
	أرجع SBI_ECALL_نسخہ_MAJOR;
}

u16 sbi_ecall_نسخہ_minor(فراغ)
{
	أرجع SBI_ECALL_نسخہ_MINOR;
}

ساكن بنانشانی لما ecall_impid = SBI_OPENSBI_IMPID;

بنانشانی لما sbi_ecall_get_impid(فراغ)
{
	أرجع ecall_impid;
}

فراغ sbi_ecall_سیٹ_impid(بنانشانی لما impid)
{
	ecall_impid = impid;
}

ساكن SBI_LIST_سر(ecall_exts_list);

بنية sbi_ecall_extension *sbi_ecall_لبھو_extension(بنانشانی لما extid)
{
	بنية sbi_ecall_extension *t, *ret = عدم;

	sbi_list_for_each_اندراج(t, &ecall_exts_list, سر) {
		إذا (t->extid_شروع <= extid && extid <= t->extid_سرا) {
			ret = t;
			اكسر;
		}
	}

	أرجع ret;
}

فراغ sbi_ecall_get_extensions_str(حرف *exts_str, صحيح exts_str_ناپ, bool experimental)
{
	بنية sbi_ecall_extension *t;
	صحيح وتھ = 0;

	إذا (!exts_str || exts_str_ناپ <= 0)
		أرجع;
	sbi_memset(exts_str, 0, exts_str_ناپ);

	sbi_list_for_each_اندراج(t, &ecall_exts_list, سر) {
		إذا (experimental != t->experimental)
			استمر;
		إذا (وتھ + sbi_strlen(t->name) + 1 > exts_str_ناپ)
			اكسر;
		sbi_snprintf(exts_str + وتھ, exts_str_ناپ - وتھ,
			     "%s,", t->name);
		وتھ = وتھ + sbi_strlen(t->name) + 1;
	}

	إذا (وتھ)
		exts_str[وتھ - 1] = '\0';
	وإلا
		sbi_strncpy(exts_str, "none", exts_str_ناپ);
}

صحيح sbi_ecall_رجسٹر_extension(بنية sbi_ecall_extension *ext)
{
	بنية sbi_ecall_extension *t;

	إذا (!ext || (ext->extid_سرا < ext->extid_شروع) || !ext->handle)
		أرجع SBI_EINVAL;

	sbi_list_for_each_اندراج(t, &ecall_exts_list, سر) {
		بنانشانی لما شروع = t->extid_شروع;
		بنانشانی لما سرا = t->extid_سرا;
		إذا (سرا < ext->extid_شروع || ext->extid_سرا < شروع)
			/* no overlap */;
		وإلا
			أرجع SBI_EINVAL;
	}

	sbi_list_جوڑو_پوچھ(&ext->سر, &ecall_exts_list);

	أرجع 0;
}

فراغ sbi_ecall_unregister_extension(بنية sbi_ecall_extension *ext)
{
	bool لبھیا = false;
	بنية sbi_ecall_extension *t;

	إذا (!ext)
		أرجع;

	sbi_list_for_each_اندراج(t, &ecall_exts_list, سر) {
		إذا (t == ext) {
			لبھیا = true;
			اكسر;
		}
	}

	إذا (لبھیا)
		sbi_list_del_init(&ext->سر);
}

صحيح sbi_ecall_handler(بنية sbi_trap_context *tcntx)
{
	صحيح ret = 0;
	بنية sbi_trap_regs *regs = &tcntx->regs;
	بنية sbi_ecall_extension *ext;
	بنانشانی لما extension_id = regs->a7;
	بنانشانی لما func_id = regs->a6;
	بنية sbi_ecall_return باہرلا = {0};
	bool is_0_1_spec = 0;

	ext = sbi_ecall_لبھو_extension(extension_id);
	إذا (ext && ext->handle) {
		ret = ext->handle(extension_id, func_id, regs, &باہرلا);
		إذا (extension_id >= SBI_EXT_0_1_سیٹ_TIMER &&
		    extension_id <= SBI_EXT_0_1_SHUTDOWN)
			is_0_1_spec = 1;
	} وإلا {
		ret = SBI_ENOTSUPP;
	}

	إذا (!باہرلا.skip_regs_update) {
		إذا (ret < SBI_آخری_ERR ||
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
			regs->a1 = باہرلا.قدر;
	}

	أرجع 0;
}

صحيح sbi_ecall_init(فراغ)
{
	صحيح ret;
	بنية sbi_ecall_extension *ext;
	بنانشانی لما i;

	لكل (i = 0; sbi_ecall_exts[i]; i++) {
		ext = sbi_ecall_exts[i];
		ret = SBI_ENODEV;

		إذا (ext->رجسٹر_extensions)
			ret = ext->رجسٹر_extensions();
		إذا (ret)
			أرجع ret;
	}

	أرجع 0;
}
