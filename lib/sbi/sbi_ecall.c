/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>

ಬಾಹ್ಯ ಸಂರಚನಾ sbi_ecall_extension *ಧ್ರುವ sbi_ecall_exts[];

u16 sbi_ecall_version_major(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ SBI_ECALL_VERSION_MAJOR;
}

u16 sbi_ecall_version_minor(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ SBI_ECALL_VERSION_MINOR;
}

ಸ್ಥಿರ unsigned long ecall_impid = SBI_OPENSBI_IMPID;

unsigned long sbi_ecall_get_impid(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ ecall_impid;
}

ಶೂನ್ಯ sbi_ecall_set_impid(unsigned long impid)
{
	ecall_impid = impid;
}

ಸ್ಥಿರ SBI_LIST_HEAD(ecall_exts_list);

ಸಂರಚನಾ sbi_ecall_extension *sbi_ecall_find_extension(unsigned long extid)
{
	ಸಂರಚನಾ sbi_ecall_extension *t, *ret = ರಿಕ್ತ;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		ಯದಿ (t->extid_start <= extid && extid <= t->extid_end) {
			ret = t;
			ಭಙ್ಗ;
		}
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಶೂನ್ಯ sbi_ecall_get_extensions_str(ವರ್ಣ *exts_str, ಪೂರ್ಣಾಙ್ಕ exts_str_size, bool experimental)
{
	ಸಂರಚನಾ sbi_ecall_extension *t;
	ಪೂರ್ಣಾಙ್ಕ offset = 0;

	ಯದಿ (!exts_str || exts_str_size <= 0)
		ಪ್ರತ್ಯಾಗಮ;
	sbi_memset(exts_str, 0, exts_str_size);

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		ಯದಿ (experimental != t->experimental)
			ಅನುವರ್ತ;
		ಯದಿ (offset + sbi_strlen(t->name) + 1 > exts_str_size)
			ಭಙ್ಗ;
		sbi_snprintf(exts_str + offset, exts_str_size - offset,
			     "%s,", t->name);
		offset = offset + sbi_strlen(t->name) + 1;
	}

	ಯದಿ (offset)
		exts_str[offset - 1] = '\0';
	ಅನ್ಯಥಾ
		sbi_strncpy(exts_str, "none", exts_str_size);
}

ಪೂರ್ಣಾಙ್ಕ sbi_ecall_register_extension(ಸಂರಚನಾ sbi_ecall_extension *ext)
{
	ಸಂರಚನಾ sbi_ecall_extension *t;

	ಯದಿ (!ext || (ext->extid_end < ext->extid_start) || !ext->handle)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		unsigned long start = t->extid_start;
		unsigned long end = t->extid_end;
		ಯದಿ (end < ext->extid_start || ext->extid_end < start)
			/* no overlap */;
		ಅನ್ಯಥಾ
			ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	}

	sbi_list_add_tail(&ext->head, &ecall_exts_list);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ sbi_ecall_unregister_extension(ಸಂರಚನಾ sbi_ecall_extension *ext)
{
	bool found = false;
	ಸಂರಚನಾ sbi_ecall_extension *t;

	ಯದಿ (!ext)
		ಪ್ರತ್ಯಾಗಮ;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		ಯದಿ (t == ext) {
			found = true;
			ಭಙ್ಗ;
		}
	}

	ಯದಿ (found)
		sbi_list_del_init(&ext->head);
}

ಪೂರ್ಣಾಙ್ಕ sbi_ecall_handler(ಸಂರಚನಾ sbi_trap_context *tcntx)
{
	ಪೂರ್ಣಾಙ್ಕ ret = 0;
	ಸಂರಚನಾ sbi_trap_regs *regs = &tcntx->regs;
	ಸಂರಚನಾ sbi_ecall_extension *ext;
	unsigned long extension_id = regs->a7;
	unsigned long func_id = regs->a6;
	ಸಂರಚನಾ sbi_ecall_return out = {0};
	bool is_0_1_spec = 0;

	ext = sbi_ecall_find_extension(extension_id);
	ಯದಿ (ext && ext->handle) {
		ret = ext->handle(extension_id, func_id, regs, &out);
		ಯದಿ (extension_id >= SBI_EXT_0_1_SET_TIMER &&
		    extension_id <= SBI_EXT_0_1_SHUTDOWN)
			is_0_1_spec = 1;
	} ಅನ್ಯಥಾ {
		ret = SBI_ENOTSUPP;
	}

	ಯದಿ (!out.skip_regs_update) {
		ಯದಿ (ret < SBI_LAST_ERR ||
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
		ಯದಿ (!is_0_1_spec)
			regs->a1 = out.ಮೌಲ್ಯ;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ sbi_ecall_init(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ ret;
	ಸಂರಚನಾ sbi_ecall_extension *ext;
	unsigned long i;

	ಪ್ರತಿ (i = 0; sbi_ecall_exts[i]; i++) {
		ext = sbi_ecall_exts[i];
		ret = SBI_ENODEV;

		ಯದಿ (ext->register_extensions)
			ret = ext->register_extensions();
		ಯದಿ (ret)
			ಪ್ರತ್ಯಾಗಮ ret;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}
