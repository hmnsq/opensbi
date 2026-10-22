/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall_interface.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_string.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_trap.h>

ବାହ୍ୟ ସଂରଚନା sbi_ecall_extension *ଅଚଳ sbi_ecall_exts[];

u16 sbi_ecall_version_major(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ SBI_ECALL_VERSION_MAJOR;
}

u16 sbi_ecall_version_minor(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ SBI_ECALL_VERSION_MINOR;
}

ସ୍ଥିର unsigned long ecall_impid = SBI_OPENSBI_IMPID;

unsigned long sbi_ecall_get_impid(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ ecall_impid;
}

ଶୂନ୍ୟ sbi_ecall_set_impid(unsigned long impid)
{
	ecall_impid = impid;
}

ସ୍ଥିର SBI_LIST_HEAD(ecall_exts_list);

ସଂରଚନା sbi_ecall_extension *sbi_ecall_find_extension(unsigned long extid)
{
	ସଂରଚନା sbi_ecall_extension *t, *ret = ଖାଲି;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		ଯଦି (t->extid_start <= extid && extid <= t->extid_end) {
			ret = t;
			ଭାଙ୍ଗ;
		}
	}

	ଫେରନ୍ତୁ ret;
}

ଶୂନ୍ୟ sbi_ecall_get_extensions_str(ଅକ୍ଷର *exts_str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା exts_str_size, bool experimental)
{
	ସଂରଚନା sbi_ecall_extension *t;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset = 0;

	ଯଦି (!exts_str || exts_str_size <= 0)
		ଫେରନ୍ତୁ;
	sbi_memset(exts_str, 0, exts_str_size);

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		ଯଦି (experimental != t->experimental)
			ଜାରି;
		ଯଦି (offset + sbi_strlen(t->name) + 1 > exts_str_size)
			ଭାଙ୍ଗ;
		sbi_snprintf(exts_str + offset, exts_str_size - offset,
			     "%s,", t->name);
		offset = offset + sbi_strlen(t->name) + 1;
	}

	ଯଦି (offset)
		exts_str[offset - 1] = '\0';
	ଅନ୍ୟଥା
		sbi_strncpy(exts_str, "none", exts_str_size);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_register_extension(ସଂରଚନା sbi_ecall_extension *ext)
{
	ସଂରଚନା sbi_ecall_extension *t;

	ଯଦି (!ext || (ext->extid_end < ext->extid_start) || !ext->handle)
		ଫେରନ୍ତୁ SBI_EINVAL;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		unsigned long start = t->extid_start;
		unsigned long end = t->extid_end;
		ଯଦି (end < ext->extid_start || ext->extid_end < start)
			/* no overlap */;
		ଅନ୍ୟଥା
			ଫେରନ୍ତୁ SBI_EINVAL;
	}

	sbi_list_add_tail(&ext->head, &ecall_exts_list);

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ sbi_ecall_unregister_extension(ସଂରଚନା sbi_ecall_extension *ext)
{
	bool found = false;
	ସଂରଚନା sbi_ecall_extension *t;

	ଯଦି (!ext)
		ଫେରନ୍ତୁ;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		ଯଦି (t == ext) {
			found = true;
			ଭାଙ୍ଗ;
		}
	}

	ଯଦି (found)
		sbi_list_del_init(&ext->head);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_handler(ସଂରଚନା sbi_trap_context *tcntx)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 0;
	ସଂରଚନା sbi_trap_regs *regs = &tcntx->regs;
	ସଂରଚନା sbi_ecall_extension *ext;
	unsigned long extension_id = regs->a7;
	unsigned long func_id = regs->a6;
	ସଂରଚନା sbi_ecall_return out = {0};
	bool is_0_1_spec = 0;

	ext = sbi_ecall_find_extension(extension_id);
	ଯଦି (ext && ext->handle) {
		ret = ext->handle(extension_id, func_id, regs, &out);
		ଯଦି (extension_id >= SBI_EXT_0_1_SET_TIMER &&
		    extension_id <= SBI_EXT_0_1_SHUTDOWN)
			is_0_1_spec = 1;
	} ଅନ୍ୟଥା {
		ret = SBI_ENOTSUPP;
	}

	ଯଦି (!out.skip_regs_update) {
		ଯଦି (ret < SBI_LAST_ERR ||
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
		ଯଦି (!is_0_1_spec)
			regs->a1 = out.ମୂଲ୍ୟ;
	}

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_init(ଶୂନ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
	ସଂରଚନା sbi_ecall_extension *ext;
	unsigned long i;

	ପାଇଁ (i = 0; sbi_ecall_exts[i]; i++) {
		ext = sbi_ecall_exts[i];
		ret = SBI_ENODEV;

		ଯଦି (ext->register_extensions)
			ret = ext->register_extensions();
		ଯଦି (ret)
			ଫେରନ୍ତୁ ret;
	}

	ଫେରନ୍ତୁ 0;
}
