/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_ecall.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_string.h>
#अन्तर्भाव <sbi/sbi_trap.h>

बाह्य संरचना sbi_ecall_extension *ध्रुव sbi_ecall_exts[];

u16 sbi_ecall_version_major(शून्य)
{
	पटिनिवत्त SBI_ECALL_VERSION_MAJOR;
}

u16 sbi_ecall_version_minor(शून्य)
{
	पटिनिवत्त SBI_ECALL_VERSION_MINOR;
}

स्थिर unsigned long ecall_impid = SBI_OPENSBI_IMPID;

unsigned long sbi_ecall_get_impid(शून्य)
{
	पटिनिवत्त ecall_impid;
}

शून्य sbi_ecall_set_impid(unsigned long impid)
{
	ecall_impid = impid;
}

स्थिर SBI_LIST_HEAD(ecall_exts_list);

संरचना sbi_ecall_extension *sbi_ecall_find_extension(unsigned long extid)
{
	संरचना sbi_ecall_extension *t, *ret = रिक्त;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		सचे (t->extid_start <= extid && extid <= t->extid_end) {
			ret = t;
			भङ्ग;
		}
	}

	पटिनिवत्त ret;
}

शून्य sbi_ecall_get_extensions_str(वर्ण *exts_str, पूर्णाङ्क exts_str_size, bool experimental)
{
	संरचना sbi_ecall_extension *t;
	पूर्णाङ्क offset = 0;

	सचे (!exts_str || exts_str_size <= 0)
		पटिनिवत्त;
	sbi_memset(exts_str, 0, exts_str_size);

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		सचे (experimental != t->experimental)
			अनुवर्त;
		सचे (offset + sbi_strlen(t->name) + 1 > exts_str_size)
			भङ्ग;
		sbi_snprintf(exts_str + offset, exts_str_size - offset,
			     "%s,", t->name);
		offset = offset + sbi_strlen(t->name) + 1;
	}

	सचे (offset)
		exts_str[offset - 1] = '\0';
	अञ्ञथा
		sbi_strncpy(exts_str, "none", exts_str_size);
}

पूर्णाङ्क sbi_ecall_register_extension(संरचना sbi_ecall_extension *ext)
{
	संरचना sbi_ecall_extension *t;

	सचे (!ext || (ext->extid_end < ext->extid_start) || !ext->handle)
		पटिनिवत्त SBI_EINVAL;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		unsigned long start = t->extid_start;
		unsigned long end = t->extid_end;
		सचे (end < ext->extid_start || ext->extid_end < start)
			/* no overlap */;
		अञ्ञथा
			पटिनिवत्त SBI_EINVAL;
	}

	sbi_list_add_tail(&ext->head, &ecall_exts_list);

	पटिनिवत्त 0;
}

शून्य sbi_ecall_unregister_extension(संरचना sbi_ecall_extension *ext)
{
	bool found = false;
	संरचना sbi_ecall_extension *t;

	सचे (!ext)
		पटिनिवत्त;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		सचे (t == ext) {
			found = true;
			भङ्ग;
		}
	}

	सचे (found)
		sbi_list_del_init(&ext->head);
}

पूर्णाङ्क sbi_ecall_handler(संरचना sbi_trap_context *tcntx)
{
	पूर्णाङ्क ret = 0;
	संरचना sbi_trap_regs *regs = &tcntx->regs;
	संरचना sbi_ecall_extension *ext;
	unsigned long extension_id = regs->a7;
	unsigned long func_id = regs->a6;
	संरचना sbi_ecall_return out = {0};
	bool is_0_1_spec = 0;

	ext = sbi_ecall_find_extension(extension_id);
	सचे (ext && ext->handle) {
		ret = ext->handle(extension_id, func_id, regs, &out);
		सचे (extension_id >= SBI_EXT_0_1_SET_TIMER &&
		    extension_id <= SBI_EXT_0_1_SHUTDOWN)
			is_0_1_spec = 1;
	} अञ्ञथा {
		ret = SBI_ENOTSUPP;
	}

	सचे (!out.skip_regs_update) {
		सचे (ret < SBI_LAST_ERR ||
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
		सचे (!is_0_1_spec)
			regs->a1 = out.मानम्;
	}

	पटिनिवत्त 0;
}

पूर्णाङ्क sbi_ecall_init(शून्य)
{
	पूर्णाङ्क ret;
	संरचना sbi_ecall_extension *ext;
	unsigned long i;

	अत्थाय (i = 0; sbi_ecall_exts[i]; i++) {
		ext = sbi_ecall_exts[i];
		ret = SBI_ENODEV;

		सचे (ext->register_extensions)
			ret = ext->register_extensions();
		सचे (ret)
			पटिनिवत्त ret;
	}

	पटिनिवत्त 0;
}
