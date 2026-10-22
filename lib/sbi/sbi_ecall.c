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

বাহ্যিক কাঠামো sbi_ecall_extension *ধ্রুবক sbi_ecall_exts[];

u16 sbi_ecall_version_major(শূন্য)
{
	ফেরত SBI_ECALL_VERSION_MAJOR;
}

u16 sbi_ecall_version_minor(শূন্য)
{
	ফেরত SBI_ECALL_VERSION_MINOR;
}

স্থির unsigned long ecall_impid = SBI_OPENSBI_IMPID;

unsigned long sbi_ecall_get_impid(শূন্য)
{
	ফেরত ecall_impid;
}

শূন্য sbi_ecall_set_impid(unsigned long impid)
{
	ecall_impid = impid;
}

স্থির SBI_LIST_শীর্ষ(ecall_exts_list);

কাঠামো sbi_ecall_extension *sbi_ecall_find_extension(unsigned long extid)
{
	কাঠামো sbi_ecall_extension *t, *ret = ফাঁকা;

	sbi_list_for_each_entry(t, &ecall_exts_list, শীর্ষ) {
		যদি (t->extid_start <= extid && extid <= t->extid_end) {
			ret = t;
			বিরতি;
		}
	}

	ফেরত ret;
}

শূন্য sbi_ecall_get_extensions_str(অক্ষর *exts_str, পূর্ণসংখ্যা exts_str_আকার, bool experimental)
{
	কাঠামো sbi_ecall_extension *t;
	পূর্ণসংখ্যা offset = 0;

	যদি (!exts_str || exts_str_আকার <= 0)
		ফেরত;
	sbi_memset(exts_str, 0, exts_str_আকার);

	sbi_list_for_each_entry(t, &ecall_exts_list, শীর্ষ) {
		যদি (experimental != t->experimental)
			চালু;
		যদি (offset + sbi_strlen(t->name) + 1 > exts_str_আকার)
			বিরতি;
		sbi_snprintf(exts_str + offset, exts_str_আকার - offset,
			     "%s,", t->name);
		offset = offset + sbi_strlen(t->name) + 1;
	}

	যদি (offset)
		exts_str[offset - 1] = '\0';
	অন্যথা
		sbi_strncpy(exts_str, "none", exts_str_আকার);
}

পূর্ণসংখ্যা sbi_ecall_register_extension(কাঠামো sbi_ecall_extension *ext)
{
	কাঠামো sbi_ecall_extension *t;

	যদি (!ext || (ext->extid_end < ext->extid_start) || !ext->handle)
		ফেরত SBI_EINVAL;

	sbi_list_for_each_entry(t, &ecall_exts_list, শীর্ষ) {
		unsigned long start = t->extid_start;
		unsigned long end = t->extid_end;
		যদি (end < ext->extid_start || ext->extid_end < start)
			/* no overlap */;
		অন্যথা
			ফেরত SBI_EINVAL;
	}

	sbi_list_add_লেজ(&ext->শীর্ষ, &ecall_exts_list);

	ফেরত 0;
}

শূন্য sbi_ecall_unregister_extension(কাঠামো sbi_ecall_extension *ext)
{
	bool found = false;
	কাঠামো sbi_ecall_extension *t;

	যদি (!ext)
		ফেরত;

	sbi_list_for_each_entry(t, &ecall_exts_list, শীর্ষ) {
		যদি (t == ext) {
			found = true;
			বিরতি;
		}
	}

	যদি (found)
		sbi_list_del_init(&ext->শীর্ষ);
}

পূর্ণসংখ্যা sbi_ecall_handler(কাঠামো sbi_trap_context *tcntx)
{
	পূর্ণসংখ্যা ret = 0;
	কাঠামো sbi_trap_regs *regs = &tcntx->regs;
	কাঠামো sbi_ecall_extension *ext;
	unsigned long extension_id = regs->a7;
	unsigned long func_id = regs->a6;
	কাঠামো sbi_ecall_return out = {0};
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

	ফেরত 0;
}

পূর্ণসংখ্যা sbi_ecall_init(শূন্য)
{
	পূর্ণসংখ্যা ret;
	কাঠামো sbi_ecall_extension *ext;
	unsigned long i;

	জন্য (i = 0; sbi_ecall_exts[i]; i++) {
		ext = sbi_ecall_exts[i];
		ret = SBI_ENODEV;

		যদি (ext->register_extensions)
			ret = ext->register_extensions();
		যদি (ret)
			ফেরত ret;
	}

	ফেরত 0;
}
