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

extern struct sbi_ecall_extension *const sbi_ecall_exts[];

u16 sbi_ecall_version_major(void)
{
	반환 SBI_ECALL_VERSION_MAJOR;
}

u16 sbi_ecall_version_minor(void)
{
	반환 SBI_ECALL_VERSION_MINOR;
}

static unsigned long ecall_impid = SBI_OPENSBI_IMPID;

unsigned long sbi_ecall_get_impid(void)
{
	반환 ecall_impid;
}

void sbi_ecall_set_impid(unsigned long impid)
{
	ecall_impid = impid;
}

static SBI_LIST_HEAD(ecall_exts_list);

struct sbi_ecall_extension *sbi_ecall_find_extension(unsigned long extid)
{
	struct sbi_ecall_extension *t, *ret = NULL;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		만약 (t->extid_start <= extid && extid <= t->extid_end) {
			ret = t;
			break;
		}
	}

	반환 ret;
}

void sbi_ecall_get_extensions_str(char *exts_str, 정수 exts_str_size, bool experimental)
{
	struct sbi_ecall_extension *t;
	정수 offset = 0;

	만약 (!exts_str || exts_str_size <= 0)
		반환;
	sbi_memset(exts_str, 0, exts_str_size);

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		만약 (experimental != t->experimental)
			continue;
		만약 (offset + sbi_strlen(t->name) + 1 > exts_str_size)
			break;
		sbi_snprintf(exts_str + offset, exts_str_size - offset,
			     "%s,", t->name);
		offset = offset + sbi_strlen(t->name) + 1;
	}

	만약 (offset)
		exts_str[offset - 1] = '\0';
	아니면
		sbi_strncpy(exts_str, "none", exts_str_size);
}

정수 sbi_ecall_register_extension(struct sbi_ecall_extension *ext)
{
	struct sbi_ecall_extension *t;

	만약 (!ext || (ext->extid_end < ext->extid_start) || !ext->handle)
		반환 SBI_EINVAL;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		unsigned long start = t->extid_start;
		unsigned long end = t->extid_end;
		만약 (end < ext->extid_start || ext->extid_end < start)
			/* no overlap */;
		아니면
			반환 SBI_EINVAL;
	}

	sbi_list_add_tail(&ext->head, &ecall_exts_list);

	반환 0;
}

void sbi_ecall_unregister_extension(struct sbi_ecall_extension *ext)
{
	bool found = false;
	struct sbi_ecall_extension *t;

	만약 (!ext)
		반환;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		만약 (t == ext) {
			found = true;
			break;
		}
	}

	만약 (found)
		sbi_list_del_init(&ext->head);
}

정수 sbi_ecall_handler(struct sbi_trap_context *tcntx)
{
	정수 ret = 0;
	struct sbi_trap_regs *regs = &tcntx->regs;
	struct sbi_ecall_extension *ext;
	unsigned long extension_id = regs->a7;
	unsigned long func_id = regs->a6;
	struct sbi_ecall_return out = {0};
	bool is_0_1_spec = 0;

	ext = sbi_ecall_find_extension(extension_id);
	만약 (ext && ext->handle) {
		ret = ext->handle(extension_id, func_id, regs, &out);
		만약 (extension_id >= SBI_EXT_0_1_SET_TIMER &&
		    extension_id <= SBI_EXT_0_1_SHUTDOWN)
			is_0_1_spec = 1;
	} 아니면 {
		ret = SBI_ENOTSUPP;
	}

	만약 (!out.skip_regs_update) {
		만약 (ret < SBI_LAST_ERR ||
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
		만약 (!is_0_1_spec)
			regs->a1 = out.value;
	}

	반환 0;
}

정수 sbi_ecall_init(void)
{
	정수 ret;
	struct sbi_ecall_extension *ext;
	unsigned long i;

	위해 (i = 0; sbi_ecall_exts[i]; i++) {
		ext = sbi_ecall_exts[i];
		ret = SBI_ENODEV;

		만약 (ext->register_extensions)
			ret = ext->register_extensions();
		만약 (ret)
			반환 ret;
	}

	반환 0;
}
