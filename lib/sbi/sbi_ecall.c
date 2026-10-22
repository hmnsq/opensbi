/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_ecall.h>
#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_string.h>
#समावेश <sbi/sbi_trap.h>

बाह्य काष्ठा sbi_ecall_extension *स्थिर sbi_ecall_exts[];

u16 sbi_ecall_version_major(व्योम)
{
	वापस SBI_ECALL_VERSION_MAJOR;
}

u16 sbi_ecall_version_minor(व्योम)
{
	वापस SBI_ECALL_VERSION_MINOR;
}

अटल अचिन्हित दीर्घ ecall_impid = SBI_OPENSBI_IMPID;

अचिन्हित दीर्घ sbi_ecall_get_impid(व्योम)
{
	वापस ecall_impid;
}

व्योम sbi_ecall_set_impid(अचिन्हित दीर्घ impid)
{
	ecall_impid = impid;
}

अटल SBI_LIST_शीर्ष(ecall_exts_list);

काष्ठा sbi_ecall_extension *sbi_ecall_find_extension(अचिन्हित दीर्घ extid)
{
	काष्ठा sbi_ecall_extension *t, *ret = शून्य;

	sbi_list_for_each_entry(t, &ecall_exts_list, शीर्ष) {
		अगर (t->extid_start <= extid && extid <= t->extid_end) {
			ret = t;
			अवरोध;
		}
	}

	वापस ret;
}

व्योम sbi_ecall_get_extensions_str(अक्षर *exts_str, पूर्णांक exts_str_आकार, bool experimental)
{
	काष्ठा sbi_ecall_extension *t;
	पूर्णांक offset = 0;

	अगर (!exts_str || exts_str_आकार <= 0)
		वापस;
	sbi_memset(exts_str, 0, exts_str_आकार);

	sbi_list_for_each_entry(t, &ecall_exts_list, शीर्ष) {
		अगर (experimental != t->experimental)
			जारी;
		अगर (offset + sbi_strlen(t->name) + 1 > exts_str_आकार)
			अवरोध;
		sbi_snprintf(exts_str + offset, exts_str_आकार - offset,
			     "%s,", t->name);
		offset = offset + sbi_strlen(t->name) + 1;
	}

	अगर (offset)
		exts_str[offset - 1] = '\0';
	अन्यथा
		sbi_strncpy(exts_str, "none", exts_str_आकार);
}

पूर्णांक sbi_ecall_register_extension(काष्ठा sbi_ecall_extension *ext)
{
	काष्ठा sbi_ecall_extension *t;

	अगर (!ext || (ext->extid_end < ext->extid_start) || !ext->handle)
		वापस SBI_EINVAL;

	sbi_list_for_each_entry(t, &ecall_exts_list, शीर्ष) {
		अचिन्हित दीर्घ start = t->extid_start;
		अचिन्हित दीर्घ end = t->extid_end;
		अगर (end < ext->extid_start || ext->extid_end < start)
			/* no overlap */;
		अन्यथा
			वापस SBI_EINVAL;
	}

	sbi_list_add_पुच्छ(&ext->शीर्ष, &ecall_exts_list);

	वापस 0;
}

व्योम sbi_ecall_unregister_extension(काष्ठा sbi_ecall_extension *ext)
{
	bool found = false;
	काष्ठा sbi_ecall_extension *t;

	अगर (!ext)
		वापस;

	sbi_list_for_each_entry(t, &ecall_exts_list, शीर्ष) {
		अगर (t == ext) {
			found = true;
			अवरोध;
		}
	}

	अगर (found)
		sbi_list_del_init(&ext->शीर्ष);
}

पूर्णांक sbi_ecall_handler(काष्ठा sbi_trap_context *tcntx)
{
	पूर्णांक ret = 0;
	काष्ठा sbi_trap_regs *regs = &tcntx->regs;
	काष्ठा sbi_ecall_extension *ext;
	अचिन्हित दीर्घ extension_id = regs->a7;
	अचिन्हित दीर्घ func_id = regs->a6;
	काष्ठा sbi_ecall_return out = {0};
	bool is_0_1_spec = 0;

	ext = sbi_ecall_find_extension(extension_id);
	अगर (ext && ext->handle) {
		ret = ext->handle(extension_id, func_id, regs, &out);
		अगर (extension_id >= SBI_EXT_0_1_SET_TIMER &&
		    extension_id <= SBI_EXT_0_1_SHUTDOWN)
			is_0_1_spec = 1;
	} अन्यथा {
		ret = SBI_ENOTSUPP;
	}

	अगर (!out.skip_regs_update) {
		अगर (ret < SBI_LAST_ERR ||
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
		अगर (!is_0_1_spec)
			regs->a1 = out.मूल्य;
	}

	वापस 0;
}

पूर्णांक sbi_ecall_init(व्योम)
{
	पूर्णांक ret;
	काष्ठा sbi_ecall_extension *ext;
	अचिन्हित दीर्घ i;

	क्रम (i = 0; sbi_ecall_exts[i]; i++) {
		ext = sbi_ecall_exts[i];
		ret = SBI_ENODEV;

		अगर (ext->register_extensions)
			ret = ext->register_extensions();
		अगर (ret)
			वापस ret;
	}

	वापस 0;
}
