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
	أرجع SBI_ECALL_VERSION_MAJOR;
}

u16 sbi_ecall_version_minor(فراغ)
{
	أرجع SBI_ECALL_VERSION_MINOR;
}

ساكن unsigned long ecall_impid = SBI_OPENSBI_IMPID;

unsigned long sbi_ecall_get_impid(فراغ)
{
	أرجع ecall_impid;
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
		إذا (t->extid_start <= extid && extid <= t->extid_end) {
			ret = t;
			اكسر;
		}
	}

	أرجع ret;
}

فراغ sbi_ecall_get_extensions_str(حرف *exts_str, صحيح exts_str_size, bool experimental)
{
	بنية sbi_ecall_extension *t;
	صحيح offset = 0;

	إذا (!exts_str || exts_str_size <= 0)
		أرجع;
	sbi_memset(exts_str, 0, exts_str_size);

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		إذا (experimental != t->experimental)
			استمر;
		إذا (offset + sbi_strlen(t->name) + 1 > exts_str_size)
			اكسر;
		sbi_snprintf(exts_str + offset, exts_str_size - offset,
			     "%s,", t->name);
		offset = offset + sbi_strlen(t->name) + 1;
	}

	إذا (offset)
		exts_str[offset - 1] = '\0';
	وإلا
		sbi_strncpy(exts_str, "none", exts_str_size);
}

صحيح sbi_ecall_register_extension(بنية sbi_ecall_extension *ext)
{
	بنية sbi_ecall_extension *t;

	إذا (!ext || (ext->extid_end < ext->extid_start) || !ext->handle)
		أرجع SBI_EINVAL;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		unsigned long start = t->extid_start;
		unsigned long end = t->extid_end;
		إذا (end < ext->extid_start || ext->extid_end < start)
			/* no overlap */;
		وإلا
			أرجع SBI_EINVAL;
	}

	sbi_list_add_tail(&ext->head, &ecall_exts_list);

	أرجع 0;
}

فراغ sbi_ecall_unregister_extension(بنية sbi_ecall_extension *ext)
{
	bool found = false;
	بنية sbi_ecall_extension *t;

	إذا (!ext)
		أرجع;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		إذا (t == ext) {
			found = true;
			اكسر;
		}
	}

	إذا (found)
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
	إذا (ext && ext->handle) {
		ret = ext->handle(extension_id, func_id, regs, &out);
		إذا (extension_id >= SBI_EXT_0_1_SET_TIMER &&
		    extension_id <= SBI_EXT_0_1_SHUTDOWN)
			is_0_1_spec = 1;
	} وإلا {
		ret = SBI_ENOTSUPP;
	}

	إذا (!out.skip_regs_update) {
		إذا (ret < SBI_LAST_ERR ||
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
			regs->a1 = out.درج;
	}

	أرجع 0;
}

صحيح sbi_ecall_init(فراغ)
{
	صحيح ret;
	بنية sbi_ecall_extension *ext;
	unsigned long i;

	لكل (i = 0; sbi_ecall_exts[i]; i++) {
		ext = sbi_ecall_exts[i];
		ret = SBI_ENODEV;

		إذا (ext->register_extensions)
			ret = ext->register_extensions();
		إذا (ret)
			أرجع ret;
	}

	أرجع 0;
}
