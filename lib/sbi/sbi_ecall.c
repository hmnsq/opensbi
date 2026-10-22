/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_ecall.h>
#அந்தர்பாவ <sbi/sbi_ecall_interface.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_string.h>
#அந்தர்பாவ <sbi/sbi_trap.h>

வெளி கட்டமைப்பு sbi_ecall_extension *மாறிலி sbi_ecall_exts[];

u16 sbi_ecall_version_major(வெறுமை)
{
	திரும்பு SBI_ECALL_VERSION_MAJOR;
}

u16 sbi_ecall_version_minor(வெறுமை)
{
	திரும்பு SBI_ECALL_VERSION_MINOR;
}

நிலையான unsigned long ecall_impid = SBI_OPENSBI_IMPID;

unsigned long sbi_ecall_get_impid(வெறுமை)
{
	திரும்பு ecall_impid;
}

வெறுமை sbi_ecall_set_impid(unsigned long impid)
{
	ecall_impid = impid;
}

நிலையான SBI_LIST_HEAD(ecall_exts_list);

கட்டமைப்பு sbi_ecall_extension *sbi_ecall_find_extension(unsigned long extid)
{
	கட்டமைப்பு sbi_ecall_extension *t, *ret = வெற்று;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		எனில் (t->extid_start <= extid && extid <= t->extid_end) {
			ret = t;
			நிறுத்து;
		}
	}

	திரும்பு ret;
}

வெறுமை sbi_ecall_get_extensions_str(எழுத்து *exts_str, முழு exts_str_size, bool experimental)
{
	கட்டமைப்பு sbi_ecall_extension *t;
	முழு offset = 0;

	எனில் (!exts_str || exts_str_size <= 0)
		திரும்பு;
	sbi_memset(exts_str, 0, exts_str_size);

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		எனில் (experimental != t->experimental)
			தொடர்;
		எனில் (offset + sbi_strlen(t->name) + 1 > exts_str_size)
			நிறுத்து;
		sbi_snprintf(exts_str + offset, exts_str_size - offset,
			     "%s,", t->name);
		offset = offset + sbi_strlen(t->name) + 1;
	}

	எனில் (offset)
		exts_str[offset - 1] = '\0';
	இல்லையெனில்
		sbi_strncpy(exts_str, "none", exts_str_size);
}

முழு sbi_ecall_register_extension(கட்டமைப்பு sbi_ecall_extension *ext)
{
	கட்டமைப்பு sbi_ecall_extension *t;

	எனில் (!ext || (ext->extid_end < ext->extid_start) || !ext->handle)
		திரும்பு SBI_EINVAL;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		unsigned long start = t->extid_start;
		unsigned long end = t->extid_end;
		எனில் (end < ext->extid_start || ext->extid_end < start)
			/* no overlap */;
		இல்லையெனில்
			திரும்பு SBI_EINVAL;
	}

	sbi_list_add_tail(&ext->head, &ecall_exts_list);

	திரும்பு 0;
}

வெறுமை sbi_ecall_unregister_extension(கட்டமைப்பு sbi_ecall_extension *ext)
{
	bool found = false;
	கட்டமைப்பு sbi_ecall_extension *t;

	எனில் (!ext)
		திரும்பு;

	sbi_list_for_each_entry(t, &ecall_exts_list, head) {
		எனில் (t == ext) {
			found = true;
			நிறுத்து;
		}
	}

	எனில் (found)
		sbi_list_del_init(&ext->head);
}

முழு sbi_ecall_handler(கட்டமைப்பு sbi_trap_context *tcntx)
{
	முழு ret = 0;
	கட்டமைப்பு sbi_trap_regs *regs = &tcntx->regs;
	கட்டமைப்பு sbi_ecall_extension *ext;
	unsigned long extension_id = regs->a7;
	unsigned long func_id = regs->a6;
	கட்டமைப்பு sbi_ecall_return out = {0};
	bool is_0_1_spec = 0;

	ext = sbi_ecall_find_extension(extension_id);
	எனில் (ext && ext->handle) {
		ret = ext->handle(extension_id, func_id, regs, &out);
		எனில் (extension_id >= SBI_EXT_0_1_SET_TIMER &&
		    extension_id <= SBI_EXT_0_1_SHUTDOWN)
			is_0_1_spec = 1;
	} இல்லையெனில் {
		ret = SBI_ENOTSUPP;
	}

	எனில் (!out.skip_regs_update) {
		எனில் (ret < SBI_LAST_ERR ||
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
		எனில் (!is_0_1_spec)
			regs->a1 = out.மதிப்பு;
	}

	திரும்பு 0;
}

முழு sbi_ecall_init(வெறுமை)
{
	முழு ret;
	கட்டமைப்பு sbi_ecall_extension *ext;
	unsigned long i;

	க்கு (i = 0; sbi_ecall_exts[i]; i++) {
		ext = sbi_ecall_exts[i];
		ret = SBI_ENODEV;

		எனில் (ext->register_extensions)
			ret = ext->register_extensions();
		எனில் (ret)
			திரும்பு ret;
	}

	திரும்பு 0;
}
