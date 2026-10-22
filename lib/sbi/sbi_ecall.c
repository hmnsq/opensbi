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

u16 sbi_ecall_version_major(فراغ)
{
	החזר SBI_ECALL_VERSION_MAJOR;
}

u16 sbi_ecall_version_minor(فراغ)
{
	החזר SBI_ECALL_VERSION_MINOR;
}

ساكن unsigned long ecall_impid = SBI_OPENSBI_IMPID;

unsigned long sbi_ecall_get_impid(فراغ)
{
	החזר ecall_impid;
}

فراغ sbi_ecall_set_impid(unsigned long impid)
{
	ecall_impid = impid;
}

ساكن SBI_LIST_HEAD(ecall_exts_list);

بنية sbi_ecall_extension *sbi_ecall_find_extension(unsigned long extid)
{
	بنية sbi_ecall_extension *t, *ret = عدم;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		אם (t->extid_start <= extid && extid <= t->extid_end) {
			ret = t;
			اكسر;
		}
	}

	החזר ret;
}

فراغ sbi_ecall_get_extensions_str(حرف *exts_str, صحيح exts_str_size, bool experimental)
{
	بنية sbi_ecall_extension *t;
	صحيح offset = 0;

	אם (!exts_str || exts_str_size <= 0)
		החזר;
	sbi_memset(exts_str, 0, exts_str_size);

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		אם (experimental != t->experimental)
			استمر;
		אם (offset + sbi_strlen(t->name) + 1 > exts_str_size)
			اكسر;
		sbi_snprintf(exts_str + offset, exts_str_size - offset,
			     "%s,", t->name);
		offset = offset + sbi_strlen(t->name) + 1;
	}

	אם (offset)
		exts_str[offset - 1] = '\0';
	אחרת
		sbi_strncpy(exts_str, "none", exts_str_size);
}

صحيح sbi_ecall_register_extension(بنية sbi_ecall_extension *ext)
{
	بنية sbi_ecall_extension *t;

	אם (!ext || (ext->extid_end < ext->extid_start) || !ext->handle)
		החזר SBI_EINVAL;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		unsigned long start = t->extid_start;
		unsigned long end = t->extid_end;
		אם (end < ext->extid_start || ext->extid_end < start)
			/* no overlap */;
		אחרת
			החזר SBI_EINVAL;
	}

	sbi_list_add_tail(&ext->head, &ecall_exts_list);

	החזר 0;
}

فراغ sbi_ecall_unregister_extension(بنية sbi_ecall_extension *ext)
{
	bool found = false;
	بنية sbi_ecall_extension *t;

	אם (!ext)
		החזר;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		אם (t == ext) {
			found = true;
			اكسر;
		}
	}

	אם (found)
		sbi_list_del_init(&ext->head);
}

صحيح sbi_ecall_handler(بنية sbi_trap_context *tcntx)
{
	صحيح ret = 0;
	بنية sbi_trap_regs *regs = &tcntx->regs;
	بنية sbi_ecall_extension *ext;
	unsigned long extension_id = regs->a7;
	unsigned long func_id = regs->a6;
	بنية sbi_ecall_return out = {0};
	bool is_0_1_spec = 0;

	ext = sbi_ecall_find_extension(extension_id);
	אם (ext && ext->handle) {
		ret = ext->handle(extension_id, func_id, regs, &out);
		אם (extension_id >= SBI_EXT_0_1_SET_TIMER &&
		    extension_id <= SBI_EXT_0_1_SHUTDOWN)
			is_0_1_spec = 1;
	} אחרת {
		ret = SBI_ENOTSUPP;
	}

	אם (!out.skip_regs_update) {
		אם (ret < SBI_LAST_ERR ||
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
		אם (!is_0_1_spec)
			regs->a1 = out.value;
	}

	החזר 0;
}

صحيح sbi_ecall_init(فراغ)
{
	صحيح ret;
	بنية sbi_ecall_extension *ext;
	unsigned long i;

	עבור (i = 0; sbi_ecall_exts[i]; i++) {
		ext = sbi_ecall_exts[i];
		ret = SBI_ENODEV;

		אם (ext->register_extensions)
			ret = ext->register_extensions();
		אם (ret)
			החזר ret;
	}

	החזר 0;
}
