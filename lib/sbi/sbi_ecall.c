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

बाह्य संरचना sbi_ecall_extension *स्थिरांक sbi_ecall_exts[];

u16 sbi_ecall_version_major(शून्य)
{
	परत SBI_ECALL_VERSION_MAJOR;
}

u16 sbi_ecall_version_minor(शून्य)
{
	परत SBI_ECALL_VERSION_MINOR;
}

स्थिर अचिन्हित दीर्घ ecall_impid = SBI_OPENSBI_IMPID;

अचिन्हित दीर्घ sbi_ecall_get_impid(शून्य)
{
	परत ecall_impid;
}

शून्य sbi_ecall_set_impid(अचिन्हित दीर्घ impid)
{
	ecall_impid = impid;
}

स्थिर SBI_LIST_HEAD(ecall_exts_list);

संरचना sbi_ecall_extension *sbi_ecall_find_extension(अचिन्हित दीर्घ extid)
{
	संरचना sbi_ecall_extension *t, *ret = रिक्त;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		जर (t->extid_start <= extid && extid <= t->extid_end) {
			ret = t;
			थांबा;
		}
	}

	परत ret;
}

शून्य sbi_ecall_get_extensions_str(अक्षर *exts_str, पूर्णांक exts_str_size, bool experimental)
{
	संरचना sbi_ecall_extension *t;
	पूर्णांक offset = 0;

	जर (!exts_str || exts_str_size <= 0)
		परत;
	sbi_memset(exts_str, 0, exts_str_size);

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		जर (experimental != t->experimental)
			सुरूठेवा;
		जर (offset + sbi_strlen(t->name) + 1 > exts_str_size)
			थांबा;
		sbi_snprintf(exts_str + offset, exts_str_size - offset,
			     "%s,", t->name);
		offset = offset + sbi_strlen(t->name) + 1;
	}

	जर (offset)
		exts_str[offset - 1] = '\0';
	अन्यथा
		sbi_strncpy(exts_str, "none", exts_str_size);
}

पूर्णांक sbi_ecall_register_extension(संरचना sbi_ecall_extension *ext)
{
	संरचना sbi_ecall_extension *t;

	जर (!ext || (ext->extid_end < ext->extid_start) || !ext->handle)
		परत SBI_EINVAL;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		अचिन्हित दीर्घ start = t->extid_start;
		अचिन्हित दीर्घ end = t->extid_end;
		जर (end < ext->extid_start || ext->extid_end < start)
			/* no overlap */;
		अन्यथा
			परत SBI_EINVAL;
	}

	sbi_list_add_tail(&ext->head, &ecall_exts_list);

	परत 0;
}

शून्य sbi_ecall_unregister_extension(संरचना sbi_ecall_extension *ext)
{
	bool found = false;
	संरचना sbi_ecall_extension *t;

	जर (!ext)
		परत;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		जर (t == ext) {
			found = true;
			थांबा;
		}
	}

	जर (found)
		sbi_list_del_init(&ext->head);
}

पूर्णांक sbi_ecall_handler(संरचना sbi_trap_context *tcntx)
{
	पूर्णांक ret = 0;
	संरचना sbi_trap_regs *regs = &tcntx->regs;
	संरचना sbi_ecall_extension *ext;
	अचिन्हित दीर्घ extension_id = regs->a7;
	अचिन्हित दीर्घ func_id = regs->a6;
	संरचना sbi_ecall_return out = {0};
	bool is_0_1_spec = 0;

	ext = sbi_ecall_find_extension(extension_id);
	जर (ext && ext->handle) {
		ret = ext->handle(extension_id, func_id, regs, &out);
		जर (extension_id >= SBI_EXT_0_1_SET_TIMER &&
		    extension_id <= SBI_EXT_0_1_SHUTDOWN)
			is_0_1_spec = 1;
	} अन्यथा {
		ret = SBI_ENOTSUPP;
	}

	जर (!out.skip_regs_update) {
		जर (ret < SBI_LAST_ERR ||
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
		जर (!is_0_1_spec)
			regs->a1 = out.मूल्य;
	}

	परत 0;
}

पूर्णांक sbi_ecall_init(शून्य)
{
	पूर्णांक ret;
	संरचना sbi_ecall_extension *ext;
	अचिन्हित दीर्घ i;

	साठी (i = 0; sbi_ecall_exts[i]; i++) {
		ext = sbi_ecall_exts[i];
		ret = SBI_ENODEV;

		जर (ext->register_extensions)
			ret = ext->register_extensions();
		जर (ret)
			परत ret;
	}

	परत 0;
}
