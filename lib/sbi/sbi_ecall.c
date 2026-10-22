/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_ecall.h>
#അന്തര്ഭാവ <sbi/sbi_ecall_interface.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_string.h>
#അന്തര്ഭാവ <sbi/sbi_trap.h>

ബാഹ്യ ഘടന sbi_ecall_extension *സ്ഥിരം sbi_ecall_exts[];

u16 sbi_ecall_version_major(ശൂന്യം)
{
	തിരികെ SBI_ECALL_VERSION_MAJOR;
}

u16 sbi_ecall_version_minor(ശൂന്യം)
{
	തിരികെ SBI_ECALL_VERSION_MINOR;
}

സ്ഥിതം unsigned long ecall_impid = SBI_OPENSBI_IMPID;

unsigned long sbi_ecall_get_impid(ശൂന്യം)
{
	തിരികെ ecall_impid;
}

ശൂന്യം sbi_ecall_set_impid(unsigned long impid)
{
	ecall_impid = impid;
}

സ്ഥിതം SBI_LIST_HEAD(ecall_exts_list);

ഘടന sbi_ecall_extension *sbi_ecall_find_extension(unsigned long extid)
{
	ഘടന sbi_ecall_extension *t, *ret = ശൂന്യ;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		എങ്കിൽ (t->extid_start <= extid && extid <= t->extid_end) {
			ret = t;
			നിർത്തുക;
		}
	}

	തിരികെ ret;
}

ശൂന്യം sbi_ecall_get_extensions_str(അക്ഷരം *exts_str, പൂർണ്ണസംഖ്യ exts_str_size, bool experimental)
{
	ഘടന sbi_ecall_extension *t;
	പൂർണ്ണസംഖ്യ offset = 0;

	എങ്കിൽ (!exts_str || exts_str_size <= 0)
		തിരികെ;
	sbi_memset(exts_str, 0, exts_str_size);

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		എങ്കിൽ (experimental != t->experimental)
			തുടരുക;
		എങ്കിൽ (offset + sbi_strlen(t->name) + 1 > exts_str_size)
			നിർത്തുക;
		sbi_snprintf(exts_str + offset, exts_str_size - offset,
			     "%s,", t->name);
		offset = offset + sbi_strlen(t->name) + 1;
	}

	എങ്കിൽ (offset)
		exts_str[offset - 1] = '\0';
	അല്ലെങ്കിൽ
		sbi_strncpy(exts_str, "none", exts_str_size);
}

പൂർണ്ണസംഖ്യ sbi_ecall_register_extension(ഘടന sbi_ecall_extension *ext)
{
	ഘടന sbi_ecall_extension *t;

	എങ്കിൽ (!ext || (ext->extid_end < ext->extid_start) || !ext->handle)
		തിരികെ SBI_EINVAL;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		unsigned long start = t->extid_start;
		unsigned long end = t->extid_end;
		എങ്കിൽ (end < ext->extid_start || ext->extid_end < start)
			/* no overlap */;
		അല്ലെങ്കിൽ
			തിരികെ SBI_EINVAL;
	}

	sbi_list_add_tail(&ext->head, &ecall_exts_list);

	തിരികെ 0;
}

ശൂന്യം sbi_ecall_unregister_extension(ഘടന sbi_ecall_extension *ext)
{
	bool found = false;
	ഘടന sbi_ecall_extension *t;

	എങ്കിൽ (!ext)
		തിരികെ;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		എങ്കിൽ (t == ext) {
			found = true;
			നിർത്തുക;
		}
	}

	എങ്കിൽ (found)
		sbi_list_del_init(&ext->head);
}

പൂർണ്ണസംഖ്യ sbi_ecall_handler(ഘടന sbi_trap_context *tcntx)
{
	പൂർണ്ണസംഖ്യ ret = 0;
	ഘടന sbi_trap_regs *regs = &tcntx->regs;
	ഘടന sbi_ecall_extension *ext;
	unsigned long extension_id = regs->a7;
	unsigned long func_id = regs->a6;
	ഘടന sbi_ecall_return out = {0};
	bool is_0_1_spec = 0;

	ext = sbi_ecall_find_extension(extension_id);
	എങ്കിൽ (ext && ext->handle) {
		ret = ext->handle(extension_id, func_id, regs, &out);
		എങ്കിൽ (extension_id >= SBI_EXT_0_1_SET_TIMER &&
		    extension_id <= SBI_EXT_0_1_SHUTDOWN)
			is_0_1_spec = 1;
	} അല്ലെങ്കിൽ {
		ret = SBI_ENOTSUPP;
	}

	എങ്കിൽ (!out.skip_regs_update) {
		എങ്കിൽ (ret < SBI_LAST_ERR ||
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
		എങ്കിൽ (!is_0_1_spec)
			regs->a1 = out.മൂല്യം;
	}

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ sbi_ecall_init(ശൂന്യം)
{
	പൂർണ്ണസംഖ്യ ret;
	ഘടന sbi_ecall_extension *ext;
	unsigned long i;

	വേണ്ടി (i = 0; sbi_ecall_exts[i]; i++) {
		ext = sbi_ecall_exts[i];
		ret = SBI_ENODEV;

		എങ്കിൽ (ext->register_extensions)
			ret = ext->register_extensions();
		എങ്കിൽ (ret)
			തിരികെ ret;
	}

	തിരികെ 0;
}
