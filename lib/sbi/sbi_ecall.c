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

u16 sbi_ecall_संस्करणम्_major(शून्य)
{
	प्रत्यागम SBI_ECALL_संस्करणम्_MAJOR;
}

u16 sbi_ecall_संस्करणम्_minor(शून्य)
{
	प्रत्यागम SBI_ECALL_संस्करणम्_MINOR;
}

स्थिर अचिह्नित दीर्घ ecall_impid = SBI_OPENSBI_IMPID;

अचिह्नित दीर्घ sbi_ecall_get_impid(शून्य)
{
	प्रत्यागम ecall_impid;
}

शून्य sbi_ecall_समुच्चयः_impid(अचिह्नित दीर्घ impid)
{
	ecall_impid = impid;
}

स्थिर SBI_LIST_शीर्ष(ecall_exts_list);

संरचना sbi_ecall_extension *sbi_ecall_find_extension(अचिह्नित दीर्घ extid)
{
	संरचना sbi_ecall_extension *t, *ret = रिक्त;

	sbi_list_for_each_प्रविष्टिः(t, &ecall_exts_list, शीर्ष) {
		यदि (t->extid_start <= extid && extid <= t->extid_अन्तः) {
			ret = t;
			भङ्ग;
		}
	}

	प्रत्यागम ret;
}

शून्य sbi_ecall_get_extensions_str(वर्ण *exts_str, पूर्णाङ्क exts_str_आकार, bool experimental)
{
	संरचना sbi_ecall_extension *t;
	पूर्णाङ्क विस्थापनम् = 0;

	यदि (!exts_str || exts_str_आकार <= 0)
		प्रत्यागम;
	sbi_memset(exts_str, 0, exts_str_आकार);

	sbi_list_for_each_प्रविष्टिः(t, &ecall_exts_list, शीर्ष) {
		यदि (experimental != t->experimental)
			अनुवर्त;
		यदि (विस्थापनम् + sbi_strlen(t->name) + 1 > exts_str_आकार)
			भङ्ग;
		sbi_snprintf(exts_str + विस्थापनम्, exts_str_आकार - विस्थापनम्,
			     "%s,", t->name);
		विस्थापनम् = विस्थापनम् + sbi_strlen(t->name) + 1;
	}

	यदि (विस्थापनम्)
		exts_str[विस्थापनम् - 1] = '\0';
	अन्यथा
		sbi_strncpy(exts_str, "none", exts_str_आकार);
}

पूर्णाङ्क sbi_ecall_पञ्जिकाकोशः_extension(संरचना sbi_ecall_extension *ext)
{
	संरचना sbi_ecall_extension *t;

	यदि (!ext || (ext->extid_अन्तः < ext->extid_start) || !ext->handle)
		प्रत्यागम SBI_EINVAL;

	sbi_list_for_each_प्रविष्टिः(t, &ecall_exts_list, शीर्ष) {
		अचिह्नित दीर्घ start = t->extid_start;
		अचिह्नित दीर्घ अन्तः_2 = t->extid_अन्तः;
		यदि (अन्तः_2 < ext->extid_start || ext->extid_अन्तः < start)
			/* no overlap */;
		अन्यथा
			प्रत्यागम SBI_EINVAL;
	}

	sbi_list_सङ्कलय_पुच्छ(&ext->शीर्ष, &ecall_exts_list);

	प्रत्यागम 0;
}

शून्य sbi_ecall_unregister_extension(संरचना sbi_ecall_extension *ext)
{
	bool प्राप्तम् = false;
	संरचना sbi_ecall_extension *t;

	यदि (!ext)
		प्रत्यागम;

	sbi_list_for_each_प्रविष्टिः(t, &ecall_exts_list, शीर्ष) {
		यदि (t == ext) {
			प्राप्तम् = true;
			भङ्ग;
		}
	}

	यदि (प्राप्तम्)
		sbi_list_del_init(&ext->शीर्ष);
}

पूर्णाङ्क sbi_ecall_handler(संरचना sbi_trap_context *tcntx)
{
	पूर्णाङ्क ret = 0;
	संरचना sbi_trap_regs *regs = &tcntx->regs;
	संरचना sbi_ecall_extension *ext;
	अचिह्नित दीर्घ extension_id = regs->a7;
	अचिह्नित दीर्घ func_id = regs->a6;
	संरचना sbi_ecall_return बहिर्गमनम् = {0};
	bool is_0_1_spec = 0;

	ext = sbi_ecall_find_extension(extension_id);
	यदि (ext && ext->handle) {
		ret = ext->handle(extension_id, func_id, regs, &बहिर्गमनम्);
		यदि (extension_id >= SBI_EXT_0_1_समुच्चयः_TIMER &&
		    extension_id <= SBI_EXT_0_1_SHUTDOWN)
			is_0_1_spec = 1;
	} अन्यथा {
		ret = SBI_ENOTSUPP;
	}

	यदि (!बहिर्गमनम्.skip_regs_update) {
		यदि (ret < SBI_चरमः_ERR ||
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
		यदि (!is_0_1_spec)
			regs->a1 = बहिर्गमनम्.मानम्;
	}

	प्रत्यागम 0;
}

पूर्णाङ्क sbi_ecall_init(शून्य)
{
	पूर्णाङ्क ret;
	संरचना sbi_ecall_extension *ext;
	अचिह्नित दीर्घ i;

	प्रति (i = 0; sbi_ecall_exts[i]; i++) {
		ext = sbi_ecall_exts[i];
		ret = SBI_ENODEV;

		यदि (ext->पञ्जिकाकोशः_extensions)
			ret = ext->पञ्जिकाकोशः_extensions();
		यदि (ret)
			प्रत्यागम ret;
	}

	प्रत्यागम 0;
}
