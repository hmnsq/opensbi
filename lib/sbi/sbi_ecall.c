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
	trả_về SBI_ECALL_VERSION_MAJOR;
}

u16 sbi_ecall_version_minor(void)
{
	trả_về SBI_ECALL_VERSION_MINOR;
}

static unsigned long ecall_impid = SBI_OPENSBI_IMPID;

unsigned long sbi_ecall_get_impid(void)
{
	trả_về ecall_impid;
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
		nếu (t->extid_start <= extid && extid <= t->extid_end) {
			ret = t;
			break;
		}
	}

	trả_về ret;
}

void sbi_ecall_get_extensions_str(char *exts_str, số_nguyên exts_str_size, bool experimental)
{
	struct sbi_ecall_extension *t;
	số_nguyên offset = 0;

	nếu (!exts_str || exts_str_size <= 0)
		trả_về;
	sbi_memset(exts_str, 0, exts_str_size);

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		nếu (experimental != t->experimental)
			continue;
		nếu (offset + sbi_strlen(t->name) + 1 > exts_str_size)
			break;
		sbi_snprintf(exts_str + offset, exts_str_size - offset,
			     "%s,", t->name);
		offset = offset + sbi_strlen(t->name) + 1;
	}

	nếu (offset)
		exts_str[offset - 1] = '\0';
	không_thì
		sbi_strncpy(exts_str, "none", exts_str_size);
}

số_nguyên sbi_ecall_register_extension(struct sbi_ecall_extension *ext)
{
	struct sbi_ecall_extension *t;

	nếu (!ext || (ext->extid_end < ext->extid_start) || !ext->handle)
		trả_về SBI_EINVAL;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		unsigned long start = t->extid_start;
		unsigned long end = t->extid_end;
		nếu (end < ext->extid_start || ext->extid_end < start)
			/* no overlap */;
		không_thì
			trả_về SBI_EINVAL;
	}

	sbi_list_add_tail(&ext->head, &ecall_exts_list);

	trả_về 0;
}

void sbi_ecall_unregister_extension(struct sbi_ecall_extension *ext)
{
	bool found = false;
	struct sbi_ecall_extension *t;

	nếu (!ext)
		trả_về;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		nếu (t == ext) {
			found = true;
			break;
		}
	}

	nếu (found)
		sbi_list_del_init(&ext->head);
}

số_nguyên sbi_ecall_handler(struct sbi_trap_context *tcntx)
{
	số_nguyên ret = 0;
	struct sbi_trap_regs *regs = &tcntx->regs;
	struct sbi_ecall_extension *ext;
	unsigned long extension_id = regs->a7;
	unsigned long func_id = regs->a6;
	struct sbi_ecall_return out = {0};
	bool is_0_1_spec = 0;

	ext = sbi_ecall_find_extension(extension_id);
	nếu (ext && ext->handle) {
		ret = ext->handle(extension_id, func_id, regs, &out);
		nếu (extension_id >= SBI_EXT_0_1_SET_TIMER &&
		    extension_id <= SBI_EXT_0_1_SHUTDOWN)
			is_0_1_spec = 1;
	} không_thì {
		ret = SBI_ENOTSUPP;
	}

	nếu (!out.skip_regs_update) {
		nếu (ret < SBI_LAST_ERR ||
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
		nếu (!is_0_1_spec)
			regs->a1 = out.value;
	}

	trả_về 0;
}

số_nguyên sbi_ecall_init(void)
{
	số_nguyên ret;
	struct sbi_ecall_extension *ext;
	unsigned long i;

	cho (i = 0; sbi_ecall_exts[i]; i++) {
		ext = sbi_ecall_exts[i];
		ret = SBI_ENODEV;

		nếu (ext->register_extensions)
			ret = ext->register_extensions();
		nếu (ret)
			trả_về ret;
	}

	trả_về 0;
}
