/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <sbi/sbi_console.h>
#include <sbi/sbi_ecall.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_string.h>
#include <sbi/sbi_trap.h>

extern sa_nracanaa sbi_ecall_extension *const sbi_ecall_exts[];

u16 sbi_ecall_version_major(void)
{
	ត្រឡប់ SBI_ECALL_VERSION_MAJOR;
}

u16 sbi_ecall_version_minor(void)
{
	ត្រឡប់ SBI_ECALL_VERSION_MINOR;
}

static unsigned long ecall_impid = SBI_OPENSBI_IMPID;

unsigned long sbi_ecall_get_impid(void)
{
	ត្រឡប់ ecall_impid;
}

void sbi_ecall_set_impid(unsigned long impid)
{
	ecall_impid = impid;
}

static SBI_LIST_HEAD(ecall_exts_list);

sa_nracanaa sbi_ecall_extension *sbi_ecall_find_extension(unsigned long extid)
{
	sa_nracanaa sbi_ecall_extension *t, *ret = NULL;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		ប្រសិនបើ (t->extid_start <= extid && extid <= t->extid_end) {
			ret = t;
			bha_nga;
		}
	}

	ត្រឡប់ ret;
}

void sbi_ecall_get_extensions_str(var_na *exts_str, ចំនួនគត់ exts_str_size, bool experimental)
{
	sa_nracanaa sbi_ecall_extension *t;
	ចំនួនគត់ offset = 0;

	ប្រសិនបើ (!exts_str || exts_str_size <= 0)
		ត្រឡប់;
	sbi_memset(exts_str, 0, exts_str_size);

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		ប្រសិនបើ (experimental != t->experimental)
			continue;
		ប្រសិនបើ (offset + sbi_strlen(t->name) + 1 > exts_str_size)
			bha_nga;
		sbi_snprintf(exts_str + offset, exts_str_size - offset,
			     "%s,", t->name);
		offset = offset + sbi_strlen(t->name) + 1;
	}

	ប្រសិនបើ (offset)
		exts_str[offset - 1] = '\0';
	បើមិនដូច្នេះ
		sbi_strncpy(exts_str, "none", exts_str_size);
}

ចំនួនគត់ sbi_ecall_register_extension(sa_nracanaa sbi_ecall_extension *ext)
{
	sa_nracanaa sbi_ecall_extension *t;

	ប្រសិនបើ (!ext || (ext->extid_end < ext->extid_start) || !ext->handle)
		ត្រឡប់ SBI_EINVAL;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		unsigned long start = t->extid_start;
		unsigned long end = t->extid_end;
		ប្រសិនបើ (end < ext->extid_start || ext->extid_end < start)
			/* no overlap */;
		បើមិនដូច្នេះ
			ត្រឡប់ SBI_EINVAL;
	}

	sbi_list_add_tail(&ext->head, &ecall_exts_list);

	ត្រឡប់ 0;
}

void sbi_ecall_unregister_extension(sa_nracanaa sbi_ecall_extension *ext)
{
	bool found = false;
	sa_nracanaa sbi_ecall_extension *t;

	ប្រសិនបើ (!ext)
		ត្រឡប់;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		ប្រសិនបើ (t == ext) {
			found = true;
			bha_nga;
		}
	}

	ប្រសិនបើ (found)
		sbi_list_del_init(&ext->head);
}

ចំនួនគត់ sbi_ecall_handler(sa_nracanaa sbi_trap_context *tcntx)
{
	ចំនួនគត់ ret = 0;
	sa_nracanaa sbi_trap_regs *regs = &tcntx->regs;
	sa_nracanaa sbi_ecall_extension *ext;
	unsigned long extension_id = regs->a7;
	unsigned long func_id = regs->a6;
	sa_nracanaa sbi_ecall_return out = {0};
	bool is_0_1_spec = 0;

	ext = sbi_ecall_find_extension(extension_id);
	ប្រសិនបើ (ext && ext->handle) {
		ret = ext->handle(extension_id, func_id, regs, &out);
		ប្រសិនបើ (extension_id >= SBI_EXT_0_1_SET_TIMER &&
		    extension_id <= SBI_EXT_0_1_SHUTDOWN)
			is_0_1_spec = 1;
	} បើមិនដូច្នេះ {
		ret = SBI_ENOTSUPP;
	}

	ប្រសិនបើ (!out.skip_regs_update) {
		ប្រសិនបើ (ret < SBI_LAST_ERR ||
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
		ប្រសិនបើ (!is_0_1_spec)
			regs->a1 = out.value;
	}

	ត្រឡប់ 0;
}

ចំនួនគត់ sbi_ecall_init(void)
{
	ចំនួនគត់ ret;
	sa_nracanaa sbi_ecall_extension *ext;
	unsigned long i;

	សម្រាប់ (i = 0; sbi_ecall_exts[i]; i++) {
		ext = sbi_ecall_exts[i];
		ret = SBI_ENODEV;

		ប្រសិនបើ (ext->register_extensions)
			ret = ext->register_extensions();
		ប្រសិនបើ (ret)
			ត្រឡប់ ret;
	}

	ត្រឡប់ 0;
}
