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
	返回 SBI_ECALL_VERSION_MAJOR;
}

u16 sbi_ecall_version_minor(void)
{
	返回 SBI_ECALL_VERSION_MINOR;
}

static unsigned long ecall_impid = SBI_OPENSBI_IMPID;

unsigned long sbi_ecall_get_impid(void)
{
	返回 ecall_impid;
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
		如果 (t->extid_start <= extid && extid <= t->extid_end) {
			ret = t;
			break;
		}
	}

	返回 ret;
}

void sbi_ecall_get_extensions_str(char *exts_str, 整数 exts_str_size, bool experimental)
{
	struct sbi_ecall_extension *t;
	整数 offset = 0;

	如果 (!exts_str || exts_str_size <= 0)
		返回;
	sbi_memset(exts_str, 0, exts_str_size);

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		如果 (experimental != t->experimental)
			continue;
		如果 (offset + sbi_strlen(t->name) + 1 > exts_str_size)
			break;
		sbi_snprintf(exts_str + offset, exts_str_size - offset,
			     "%s,", t->name);
		offset = offset + sbi_strlen(t->name) + 1;
	}

	如果 (offset)
		exts_str[offset - 1] = '\0';
	否则
		sbi_strncpy(exts_str, "none", exts_str_size);
}

整数 sbi_ecall_register_extension(struct sbi_ecall_extension *ext)
{
	struct sbi_ecall_extension *t;

	如果 (!ext || (ext->extid_end < ext->extid_start) || !ext->handle)
		返回 SBI_EINVAL;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		unsigned long start = t->extid_start;
		unsigned long end = t->extid_end;
		如果 (end < ext->extid_start || ext->extid_end < start)
			/* no overlap */;
		否则
			返回 SBI_EINVAL;
	}

	sbi_list_add_tail(&ext->head, &ecall_exts_list);

	返回 0;
}

void sbi_ecall_unregister_extension(struct sbi_ecall_extension *ext)
{
	bool found = false;
	struct sbi_ecall_extension *t;

	如果 (!ext)
		返回;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		如果 (t == ext) {
			found = true;
			break;
		}
	}

	如果 (found)
		sbi_list_del_init(&ext->head);
}

整数 sbi_ecall_handler(struct sbi_trap_context *tcntx)
{
	整数 ret = 0;
	struct sbi_trap_regs *regs = &tcntx->regs;
	struct sbi_ecall_extension *ext;
	unsigned long extension_id = regs->a7;
	unsigned long func_id = regs->a6;
	struct sbi_ecall_return out = {0};
	bool is_0_1_spec = 0;

	ext = sbi_ecall_find_extension(extension_id);
	如果 (ext && ext->handle) {
		ret = ext->handle(extension_id, func_id, regs, &out);
		如果 (extension_id >= SBI_EXT_0_1_SET_TIMER &&
		    extension_id <= SBI_EXT_0_1_SHUTDOWN)
			is_0_1_spec = 1;
	} 否则 {
		ret = SBI_ENOTSUPP;
	}

	如果 (!out.skip_regs_update) {
		如果 (ret < SBI_LAST_ERR ||
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
		如果 (!is_0_1_spec)
			regs->a1 = out.value;
	}

	返回 0;
}

整数 sbi_ecall_init(void)
{
	整数 ret;
	struct sbi_ecall_extension *ext;
	unsigned long i;

	为 (i = 0; sbi_ecall_exts[i]; i++) {
		ext = sbi_ecall_exts[i];
		ret = SBI_ENODEV;

		如果 (ext->register_extensions)
			ret = ext->register_extensions();
		如果 (ret)
			返回 ret;
	}

	返回 0;
}
