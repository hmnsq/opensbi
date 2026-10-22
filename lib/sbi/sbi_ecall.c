/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_ecall.h>
#অন্তর্ভাব <sbi/sbi_ecall_interface.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_string.h>
#অন্তর্ভাব <sbi/sbi_trap.h>

বাহ্য সংরচনা sbi_ecall_extension *ধ্রুব sbi_ecall_exts[];

u16 sbi_ecall_version_major(শূন্য)
{
	প্রত্যাগম SBI_ECALL_VERSION_MAJOR;
}

u16 sbi_ecall_version_minor(শূন্য)
{
	প্রত্যাগম SBI_ECALL_VERSION_MINOR;
}

স্থির unsigned long ecall_impid = SBI_OPENSBI_IMPID;

unsigned long sbi_ecall_get_impid(শূন্য)
{
	প্রত্যাগম ecall_impid;
}

শূন্য sbi_ecall_set_impid(unsigned long impid)
{
	ecall_impid = impid;
}

স্থির SBI_LIST_HEAD(ecall_exts_list);

সংরচনা sbi_ecall_extension *sbi_ecall_find_extension(unsigned long extid)
{
	সংরচনা sbi_ecall_extension *t, *ret = রিক্ত;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		যদি (t->extid_start <= extid && extid <= t->extid_end) {
			ret = t;
			ভঙ্গ;
		}
	}

	প্রত্যাগম ret;
}

শূন্য sbi_ecall_get_extensions_str(বর্ণ *exts_str, পূর্ণাঙ্ক exts_str_size, bool experimental)
{
	সংরচনা sbi_ecall_extension *t;
	পূর্ণাঙ্ক offset = 0;

	যদি (!exts_str || exts_str_size <= 0)
		প্রত্যাগম;
	sbi_memset(exts_str, 0, exts_str_size);

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		যদি (experimental != t->experimental)
			অনুবর্ত;
		যদি (offset + sbi_strlen(t->name) + 1 > exts_str_size)
			ভঙ্গ;
		sbi_snprintf(exts_str + offset, exts_str_size - offset,
			     "%s,", t->name);
		offset = offset + sbi_strlen(t->name) + 1;
	}

	যদি (offset)
		exts_str[offset - 1] = '\0';
	অন্যথা
		sbi_strncpy(exts_str, "none", exts_str_size);
}

পূর্ণাঙ্ক sbi_ecall_register_extension(সংরচনা sbi_ecall_extension *ext)
{
	সংরচনা sbi_ecall_extension *t;

	যদি (!ext || (ext->extid_end < ext->extid_start) || !ext->handle)
		প্রত্যাগম SBI_EINVAL;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		unsigned long start = t->extid_start;
		unsigned long end = t->extid_end;
		যদি (end < ext->extid_start || ext->extid_end < start)
			/* no overlap */;
		অন্যথা
			প্রত্যাগম SBI_EINVAL;
	}

	sbi_list_add_tail(&ext->head, &ecall_exts_list);

	প্রত্যাগম 0;
}

শূন্য sbi_ecall_unregister_extension(সংরচনা sbi_ecall_extension *ext)
{
	bool found = false;
	সংরচনা sbi_ecall_extension *t;

	যদি (!ext)
		প্রত্যাগম;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		যদি (t == ext) {
			found = true;
			ভঙ্গ;
		}
	}

	যদি (found)
		sbi_list_del_init(&ext->head);
}

পূর্ণাঙ্ক sbi_ecall_handler(সংরচনা sbi_trap_context *tcntx)
{
	পূর্ণাঙ্ক ret = 0;
	সংরচনা sbi_trap_regs *regs = &tcntx->regs;
	সংরচনা sbi_ecall_extension *ext;
	unsigned long extension_id = regs->a7;
	unsigned long func_id = regs->a6;
	সংরচনা sbi_ecall_return out = {0};
	bool is_0_1_spec = 0;

	ext = sbi_ecall_find_extension(extension_id);
	যদি (ext && ext->handle) {
		ret = ext->handle(extension_id, func_id, regs, &out);
		যদি (extension_id >= SBI_EXT_0_1_SET_TIMER &&
		    extension_id <= SBI_EXT_0_1_SHUTDOWN)
			is_0_1_spec = 1;
	} অন্যথা {
		ret = SBI_ENOTSUPP;
	}

	যদি (!out.skip_regs_update) {
		যদি (ret < SBI_LAST_ERR ||
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
		যদি (!is_0_1_spec)
			regs->a1 = out.মান;
	}

	প্রত্যাগম 0;
}

পূর্ণাঙ্ক sbi_ecall_init(শূন্য)
{
	পূর্ণাঙ্ক ret;
	সংরচনা sbi_ecall_extension *ext;
	unsigned long i;

	প্রতি (i = 0; sbi_ecall_exts[i]; i++) {
		ext = sbi_ecall_exts[i];
		ret = SBI_ENODEV;

		যদি (ext->register_extensions)
			ret = ext->register_extensions();
		যদি (ret)
			প্রত্যাগম ret;
	}

	প্রত্যাগম 0;
}
