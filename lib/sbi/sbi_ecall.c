/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_ecall.h>
#અન્તર્ભાવ <sbi/sbi_ecall_interface.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_string.h>
#અન્તર્ભાવ <sbi/sbi_trap.h>

બાહ્ય સંરચના sbi_ecall_extension *અચલ sbi_ecall_exts[];

u16 sbi_ecall_version_major(શૂન્ય)
{
	પાછા SBI_ECALL_VERSION_MAJOR;
}

u16 sbi_ecall_version_minor(શૂન્ય)
{
	પાછા SBI_ECALL_VERSION_MINOR;
}

સ્થિર unsigned long ecall_impid = SBI_OPENSBI_IMPID;

unsigned long sbi_ecall_get_impid(શૂન્ય)
{
	પાછા ecall_impid;
}

શૂન્ય sbi_ecall_set_impid(unsigned long impid)
{
	ecall_impid = impid;
}

સ્થિર SBI_LIST_HEAD(ecall_exts_list);

સંરચના sbi_ecall_extension *sbi_ecall_find_extension(unsigned long extid)
{
	સંરચના sbi_ecall_extension *t, *ret = ખાલી;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		જો (t->extid_start <= extid && extid <= t->extid_end) {
			ret = t;
			તોડો;
		}
	}

	પાછા ret;
}

શૂન્ય sbi_ecall_get_extensions_str(અક્ષર *exts_str, પૂર્ણાંક exts_str_size, bool experimental)
{
	સંરચના sbi_ecall_extension *t;
	પૂર્ણાંક offset = 0;

	જો (!exts_str || exts_str_size <= 0)
		પાછા;
	sbi_memset(exts_str, 0, exts_str_size);

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		જો (experimental != t->experimental)
			ચાલુ;
		જો (offset + sbi_strlen(t->name) + 1 > exts_str_size)
			તોડો;
		sbi_snprintf(exts_str + offset, exts_str_size - offset,
			     "%s,", t->name);
		offset = offset + sbi_strlen(t->name) + 1;
	}

	જો (offset)
		exts_str[offset - 1] = '\0';
	નહીંતર
		sbi_strncpy(exts_str, "none", exts_str_size);
}

પૂર્ણાંક sbi_ecall_register_extension(સંરચના sbi_ecall_extension *ext)
{
	સંરચના sbi_ecall_extension *t;

	જો (!ext || (ext->extid_end < ext->extid_start) || !ext->handle)
		પાછા SBI_EINVAL;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		unsigned long start = t->extid_start;
		unsigned long end = t->extid_end;
		જો (end < ext->extid_start || ext->extid_end < start)
			/* no overlap */;
		નહીંતર
			પાછા SBI_EINVAL;
	}

	sbi_list_add_tail(&ext->head, &ecall_exts_list);

	પાછા 0;
}

શૂન્ય sbi_ecall_unregister_extension(સંરચના sbi_ecall_extension *ext)
{
	bool found = false;
	સંરચના sbi_ecall_extension *t;

	જો (!ext)
		પાછા;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		જો (t == ext) {
			found = true;
			તોડો;
		}
	}

	જો (found)
		sbi_list_del_init(&ext->head);
}

પૂર્ણાંક sbi_ecall_handler(સંરચના sbi_trap_context *tcntx)
{
	પૂર્ણાંક ret = 0;
	સંરચના sbi_trap_regs *regs = &tcntx->regs;
	સંરચના sbi_ecall_extension *ext;
	unsigned long extension_id = regs->a7;
	unsigned long func_id = regs->a6;
	સંરચના sbi_ecall_return out = {0};
	bool is_0_1_spec = 0;

	ext = sbi_ecall_find_extension(extension_id);
	જો (ext && ext->handle) {
		ret = ext->handle(extension_id, func_id, regs, &out);
		જો (extension_id >= SBI_EXT_0_1_SET_TIMER &&
		    extension_id <= SBI_EXT_0_1_SHUTDOWN)
			is_0_1_spec = 1;
	} નહીંતર {
		ret = SBI_ENOTSUPP;
	}

	જો (!out.skip_regs_update) {
		જો (ret < SBI_LAST_ERR ||
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
		જો (!is_0_1_spec)
			regs->a1 = out.કિંમત;
	}

	પાછા 0;
}

પૂર્ણાંક sbi_ecall_init(શૂન્ય)
{
	પૂર્ણાંક ret;
	સંરચના sbi_ecall_extension *ext;
	unsigned long i;

	માટે (i = 0; sbi_ecall_exts[i]; i++) {
		ext = sbi_ecall_exts[i];
		ret = SBI_ENODEV;

		જો (ext->register_extensions)
			ret = ext->register_extensions();
		જો (ret)
			પાછા ret;
	}

	પાછા 0;
}
