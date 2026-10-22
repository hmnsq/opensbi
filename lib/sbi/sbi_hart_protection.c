/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_hart_protection.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi/sbi_string.h>

സ്ഥിതം SBI_LIST_HEAD(hart_protection_list);

സ്ഥിതം ഘടന sbi_hart_protection *__hart_memory_protection_best(ശൂന്യം)
{
	ഘടന sbi_hart_protection *pos;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		എങ്കിൽ (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY)
			തിരികെ pos;
	}

	തിരികെ ശൂന്യ;
}

ശൂന്യം sbi_hart_protection_get_str(അക്ഷരം *out_str, പൂർണ്ണസംഖ്യ out_str_size)
{
	bool memory_protect_done = false;
	ഘടന sbi_hart_protection *pos;
	പൂർണ്ണസംഖ്യ offset = 0;

	എങ്കിൽ (!out_str || out_str_size <= 0)
		തിരികെ;
	sbi_memset(out_str, 0, out_str_size);

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		എങ്കിൽ (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY) {
			എങ്കിൽ (memory_protect_done)
				തുടരുക;
			memory_protect_done = true;
		}
		sbi_snprintf(out_str + offset, out_str_size - offset, "%s,", pos->name);
		offset = offset + sbi_strlen(pos->name) + 1;
	}

	എങ്കിൽ (offset)
		out_str[offset - 1] = '\0';
	അല്ലെങ്കിൽ
		sbi_strncpy(out_str, "none", out_str_size);
}

പൂർണ്ണസംഖ്യ sbi_hart_protection_register(ഘടന sbi_hart_protection *hprot)
{
	ഘടന sbi_hart_protection *pos = ശൂന്യ;
	bool found_pos = false;

	എങ്കിൽ (!hprot)
		തിരികെ SBI_EINVAL;
	എങ്കിൽ (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		തിരികെ SBI_EINVAL;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		എങ്കിൽ (hprot->rating > pos->rating) {
			found_pos = true;
			നിർത്തുക;
		}
	}

	എങ്കിൽ (found_pos)
		sbi_list_add_tail(&hprot->head, &pos->head);
	അല്ലെങ്കിൽ
		sbi_list_add_tail(&hprot->head, &hart_protection_list);

	തിരികെ 0;
}

ശൂന്യം sbi_hart_protection_unregister(ഘടന sbi_hart_protection *hprot)
{
	എങ്കിൽ (!hprot)
		തിരികെ;

	sbi_list_del(&hprot->head);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ __hart_protection_configure(ഘടന sbi_scratch *scratch,
				       ഘടന sbi_hart_protection *hprot,
				       ഘടന sbi_domain *dom)
{
	എങ്കിൽ (!hprot)
		തിരികെ 0;
	എങ്കിൽ (!hprot->configure)
		തിരികെ SBI_ENOSYS;

	തിരികെ hprot->configure(scratch, dom);
}

സ്ഥിതം ശൂന്യം __hart_protection_unconfigure(ഘടന sbi_scratch *scratch,
					  ഘടന sbi_hart_protection *hprot,
					  ഘടന sbi_domain *dom)
{
	എങ്കിൽ (!hprot || !hprot->unconfigure)
		തിരികെ;

	hprot->unconfigure(scratch, dom);
}

പൂർണ്ണസംഖ്യ sbi_hart_protection_configure(ഘടന sbi_scratch *scratch,
				  ഘടന sbi_domain *dom)
{
	bool do_configure, memory_protect_done = false;
	ഘടന sbi_hart_protection *hprot;
	പൂർണ്ണസംഖ്യ ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_configure = false;
		തിരഞ്ഞെടുപ്പ് (hprot->type) {
		അവസ്ഥ SBI_HART_PROTECTION_TYPE_MEMORY:
			do_configure = !memory_protect_done;
			memory_protect_done = true;
			നിർത്തുക;
		അവസ്ഥ SBI_HART_PROTECTION_TYPE_ID:
			do_configure = true;
			നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
			നിർത്തുക;
		}
		എങ്കിൽ (!do_configure)
			തുടരുക;

		ret = __hart_protection_configure(scratch, hprot, dom);
		എങ്കിൽ (ret)
			തിരികെ ret;
	}

	തിരികെ 0;
}

ശൂന്യം sbi_hart_protection_unconfigure(ഘടന sbi_scratch *scratch,
				     ഘടന sbi_domain *dom)
{

	bool do_unconfigure, memory_protect_done = false;
	ഘടന sbi_hart_protection *hprot;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_unconfigure = false;
		തിരഞ്ഞെടുപ്പ് (hprot->type) {
		അവസ്ഥ SBI_HART_PROTECTION_TYPE_MEMORY:
			do_unconfigure = !memory_protect_done;
			memory_protect_done = true;
			നിർത്തുക;
		അവസ്ഥ SBI_HART_PROTECTION_TYPE_ID:
			do_unconfigure = true;
			നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
			നിർത്തുക;
		}
		എങ്കിൽ (!do_unconfigure)
			തുടരുക;

		__hart_protection_unconfigure(scratch, hprot, dom);
	}
}

പൂർണ്ണസംഖ്യ sbi_hart_protection_reconfigure(ഘടന sbi_scratch *scratch,
				    ഘടന sbi_domain *current_dom,
				    ഘടന sbi_domain *next_dom)
{
	bool do_reconfigure, memory_protect_done = false;
	ഘടന sbi_hart_protection *hprot;
	പൂർണ്ണസംഖ്യ ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_reconfigure = false;
		തിരഞ്ഞെടുപ്പ് (hprot->type) {
		അവസ്ഥ SBI_HART_PROTECTION_TYPE_MEMORY:
			do_reconfigure = !memory_protect_done;
			memory_protect_done = true;
			നിർത്തുക;
		അവസ്ഥ SBI_HART_PROTECTION_TYPE_ID:
			do_reconfigure = true;
			നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
			നിർത്തുക;
		}
		എങ്കിൽ (!do_reconfigure)
			തുടരുക;

		__hart_protection_unconfigure(scratch, hprot, current_dom);
		ret = __hart_protection_configure(scratch, hprot, next_dom);
		എങ്കിൽ (ret)
			തിരികെ ret;
	}

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ sbi_hart_protection_temp_map_range(unsigned long base, unsigned long size)
{
	ഘടന sbi_hart_protection *hprot = __hart_memory_protection_best();

	എങ്കിൽ (!hprot || !hprot->temp_map_range)
		തിരികെ 0;

	തിരികെ hprot->temp_map_range(sbi_scratch_thishart_ptr(), base, size);
}

പൂർണ്ണസംഖ്യ sbi_hart_protection_temp_unmap_range(unsigned long base, unsigned long size)
{
	ഘടന sbi_hart_protection *hprot = __hart_memory_protection_best();

	എങ്കിൽ (!hprot || !hprot->temp_unmap_range)
		തിരികെ 0;

	തിരികെ hprot->temp_unmap_range(sbi_scratch_thishart_ptr(), base, size);
}
