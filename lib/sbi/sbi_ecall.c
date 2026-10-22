/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>

ਬਾਹਰੀ ਬਣਤਰ sbi_ecall_extension *ਅਚਲ sbi_ecall_exts[];

u16 sbi_ecall_ਸੰਸਕਰਣ_major(ਖਾਲੀ)
{
	ਵਾਪਸ SBI_ECALL_ਸੰਸਕਰਣ_MAJOR;
}

u16 sbi_ecall_ਸੰਸਕਰਣ_minor(ਖਾਲੀ)
{
	ਵਾਪਸ SBI_ECALL_ਸੰਸਕਰਣ_MINOR;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ecall_impid = SBI_OPENSBI_IMPID;

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sbi_ecall_get_impid(ਖਾਲੀ)
{
	ਵਾਪਸ ecall_impid;
}

ਖਾਲੀ sbi_ecall_ਸੈੱਟ_impid(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ impid)
{
	ecall_impid = impid;
}

ਸਥਿਰ SBI_LIST_ਸਿਰ(ecall_exts_list);

ਬਣਤਰ sbi_ecall_extension *sbi_ecall_ਲੱਭੋ_extension(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ extid)
{
	ਬਣਤਰ sbi_ecall_extension *t, *ret = ਨਿਰਾ;

	sbi_list_for_each_ਇੰਦਰਾਜ(t, &ecall_exts_list, ਸਿਰ) {
		ਜੇ (t->extid_ਸ਼ੁਰੂ <= extid && extid <= t->extid_ਸਿਰਾ) {
			ret = t;
			ਰੋਕੋ;
		}
	}

	ਵਾਪਸ ret;
}

ਖਾਲੀ sbi_ecall_get_extensions_str(ਅੱਖਰ *exts_str, ਪੂਰਨ exts_str_ਮਾਪ, bool experimental)
{
	ਬਣਤਰ sbi_ecall_extension *t;
	ਪੂਰਨ ਵਿੱਥ = 0;

	ਜੇ (!exts_str || exts_str_ਮਾਪ <= 0)
		ਵਾਪਸ;
	sbi_memset(exts_str, 0, exts_str_ਮਾਪ);

	sbi_list_for_each_ਇੰਦਰਾਜ(t, &ecall_exts_list, ਸਿਰ) {
		ਜੇ (experimental != t->experimental)
			ਜਾਰੀ;
		ਜੇ (ਵਿੱਥ + sbi_strlen(t->name) + 1 > exts_str_ਮਾਪ)
			ਰੋਕੋ;
		sbi_snprintf(exts_str + ਵਿੱਥ, exts_str_ਮਾਪ - ਵਿੱਥ,
			     "%s,", t->name);
		ਵਿੱਥ = ਵਿੱਥ + sbi_strlen(t->name) + 1;
	}

	ਜੇ (ਵਿੱਥ)
		exts_str[ਵਿੱਥ - 1] = '\0';
	ਨਹੀਂਤਾਂ
		sbi_strncpy(exts_str, "none", exts_str_ਮਾਪ);
}

ਪੂਰਨ sbi_ecall_ਰਜਿਸਟਰ_extension(ਬਣਤਰ sbi_ecall_extension *ext)
{
	ਬਣਤਰ sbi_ecall_extension *t;

	ਜੇ (!ext || (ext->extid_ਸਿਰਾ < ext->extid_ਸ਼ੁਰੂ) || !ext->handle)
		ਵਾਪਸ SBI_EINVAL;

	sbi_list_for_each_ਇੰਦਰਾਜ(t, &ecall_exts_list, ਸਿਰ) {
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਸ਼ੁਰੂ = t->extid_ਸ਼ੁਰੂ;
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਸਿਰਾ = t->extid_ਸਿਰਾ;
		ਜੇ (ਸਿਰਾ < ext->extid_ਸ਼ੁਰੂ || ext->extid_ਸਿਰਾ < ਸ਼ੁਰੂ)
			/* no overlap */;
		ਨਹੀਂਤਾਂ
			ਵਾਪਸ SBI_EINVAL;
	}

	sbi_list_ਜੋੜੋ_ਪੂਛ(&ext->ਸਿਰ, &ecall_exts_list);

	ਵਾਪਸ 0;
}

ਖਾਲੀ sbi_ecall_unregister_extension(ਬਣਤਰ sbi_ecall_extension *ext)
{
	bool ਮਿਲਿਆ = false;
	ਬਣਤਰ sbi_ecall_extension *t;

	ਜੇ (!ext)
		ਵਾਪਸ;

	sbi_list_for_each_ਇੰਦਰਾਜ(t, &ecall_exts_list, ਸਿਰ) {
		ਜੇ (t == ext) {
			ਮਿਲਿਆ = true;
			ਰੋਕੋ;
		}
	}

	ਜੇ (ਮਿਲਿਆ)
		sbi_list_del_init(&ext->ਸਿਰ);
}

ਪੂਰਨ sbi_ecall_handler(ਬਣਤਰ sbi_trap_context *tcntx)
{
	ਪੂਰਨ ret = 0;
	ਬਣਤਰ sbi_trap_regs *regs = &tcntx->regs;
	ਬਣਤਰ sbi_ecall_extension *ext;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ extension_id = regs->a7;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ func_id = regs->a6;
	ਬਣਤਰ sbi_ecall_return ਬਾਹਰਲਾ = {0};
	bool is_0_1_spec = 0;

	ext = sbi_ecall_ਲੱਭੋ_extension(extension_id);
	ਜੇ (ext && ext->handle) {
		ret = ext->handle(extension_id, func_id, regs, &ਬਾਹਰਲਾ);
		ਜੇ (extension_id >= SBI_EXT_0_1_ਸੈੱਟ_TIMER &&
		    extension_id <= SBI_EXT_0_1_SHUTDOWN)
			is_0_1_spec = 1;
	} ਨਹੀਂਤਾਂ {
		ret = SBI_ENOTSUPP;
	}

	ਜੇ (!ਬਾਹਰਲਾ.skip_regs_update) {
		ਜੇ (ret < SBI_ਆਖਰੀ_ERR ||
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
		ਜੇ (!is_0_1_spec)
			regs->a1 = ਬਾਹਰਲਾ.ਮੁੱਲ;
	}

	ਵਾਪਸ 0;
}

ਪੂਰਨ sbi_ecall_init(ਖਾਲੀ)
{
	ਪੂਰਨ ret;
	ਬਣਤਰ sbi_ecall_extension *ext;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ i;

	ਲਈ (i = 0; sbi_ecall_exts[i]; i++) {
		ext = sbi_ecall_exts[i];
		ret = SBI_ENODEV;

		ਜੇ (ext->ਰਜਿਸਟਰ_extensions)
			ret = ext->ਰਜਿਸਟਰ_extensions();
		ਜੇ (ret)
			ਵਾਪਸ ret;
	}

	ਵਾਪਸ 0;
}
