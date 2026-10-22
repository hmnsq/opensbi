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

بیرونی ساختار sbi_ecall_extension *ثابت sbi_ecall_exts[];

u16 sbi_ecall_version_major(تهی)
{
	بازگشت SBI_ECALL_VERSION_MAJOR;
}

u16 sbi_ecall_version_minor(تهی)
{
	بازگشت SBI_ECALL_VERSION_MINOR;
}

ایستا unsigned long ecall_impid = SBI_OPENSBI_IMPID;

unsigned long sbi_ecall_get_impid(تهی)
{
	بازگشت ecall_impid;
}

تهی sbi_ecall_set_impid(unsigned long impid)
{
	ecall_impid = impid;
}

ایستا SBI_LIST_HEAD(ecall_exts_list);

ساختار sbi_ecall_extension *sbi_ecall_find_extension(unsigned long extid)
{
	ساختار sbi_ecall_extension *t, *ret = پوچ;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		اگر (t->extid_start <= extid && extid <= t->extid_end) {
			ret = t;
			بشکن;
		}
	}

	بازگشت ret;
}

تهی sbi_ecall_get_extensions_str(نویسه *exts_str, صحیح exts_str_size, bool experimental)
{
	ساختار sbi_ecall_extension *t;
	صحیح offset = 0;

	اگر (!exts_str || exts_str_size <= 0)
		بازگشت;
	sbi_memset(exts_str, 0, exts_str_size);

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		اگر (experimental != t->experimental)
			ادامه;
		اگر (offset + sbi_strlen(t->name) + 1 > exts_str_size)
			بشکن;
		sbi_snprintf(exts_str + offset, exts_str_size - offset,
			     "%s,", t->name);
		offset = offset + sbi_strlen(t->name) + 1;
	}

	اگر (offset)
		exts_str[offset - 1] = '\0';
	وگرنه
		sbi_strncpy(exts_str, "none", exts_str_size);
}

صحیح sbi_ecall_register_extension(ساختار sbi_ecall_extension *ext)
{
	ساختار sbi_ecall_extension *t;

	اگر (!ext || (ext->extid_end < ext->extid_start) || !ext->handle)
		بازگشت SBI_EINVAL;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		unsigned long start = t->extid_start;
		unsigned long end = t->extid_end;
		اگر (end < ext->extid_start || ext->extid_end < start)
			/* no overlap */;
		وگرنه
			بازگشت SBI_EINVAL;
	}

	sbi_list_add_tail(&ext->head, &ecall_exts_list);

	بازگشت 0;
}

تهی sbi_ecall_unregister_extension(ساختار sbi_ecall_extension *ext)
{
	bool found = false;
	ساختار sbi_ecall_extension *t;

	اگر (!ext)
		بازگشت;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		اگر (t == ext) {
			found = true;
			بشکن;
		}
	}

	اگر (found)
		sbi_list_del_init(&ext->head);
}

صحیح sbi_ecall_handler(ساختار sbi_trap_context *tcntx)
{
	صحیح ret = 0;
	ساختار sbi_trap_regs *regs = &tcntx->regs;
	ساختار sbi_ecall_extension *ext;
	unsigned long extension_id = regs->a7;
	unsigned long func_id = regs->a6;
	ساختار sbi_ecall_return out = {0};
	bool is_0_1_spec = 0;

	ext = sbi_ecall_find_extension(extension_id);
	اگر (ext && ext->handle) {
		ret = ext->handle(extension_id, func_id, regs, &out);
		اگر (extension_id >= SBI_EXT_0_1_SET_TIMER &&
		    extension_id <= SBI_EXT_0_1_SHUTDOWN)
			is_0_1_spec = 1;
	} وگرنه {
		ret = SBI_ENOTSUPP;
	}

	اگر (!out.skip_regs_update) {
		اگر (ret < SBI_LAST_ERR ||
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
			regs->a1 = out.مقدار;
	}

	بازگشت 0;
}

صحیح sbi_ecall_init(تهی)
{
	صحیح ret;
	ساختار sbi_ecall_extension *ext;
	unsigned long i;

	برای (i = 0; sbi_ecall_exts[i]; i++) {
		ext = sbi_ecall_exts[i];
		ret = SBI_ENODEV;

		اگر (ext->register_extensions)
			ret = ext->register_extensions();
		اگر (ret)
			بازگشت ret;
	}

	بازگشت 0;
}
