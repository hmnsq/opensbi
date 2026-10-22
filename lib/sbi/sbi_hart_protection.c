/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#অন্তর্ভাৱ <sbi/sbi_console.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_hart_protection.h>
#অন্তর্ভাৱ <sbi/sbi_scratch.h>
#অন্তর্ভাৱ <sbi/sbi_string.h>

স্থির SBI_LIST_HEAD(hart_protection_list);

স্থির সংরচনা sbi_hart_protection *__hart_memory_protection_best(শূন্য)
{
	সংরচনা sbi_hart_protection *pos;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		যদি (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY)
			ঘূৰাই pos;
	}

	ঘূৰাই রিক্ত;
}

শূন্য sbi_hart_protection_get_str(ৱর্ণ *out_str, পূর্ণাঙ্ক out_str_size)
{
	bool memory_protect_done = false;
	সংরচনা sbi_hart_protection *pos;
	পূর্ণাঙ্ক offset = 0;

	যদি (!out_str || out_str_size <= 0)
		ঘূৰাই;
	sbi_memset(out_str, 0, out_str_size);

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		যদি (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY) {
			যদি (memory_protect_done)
				অনুৱর্ত;
			memory_protect_done = true;
		}
		sbi_snprintf(out_str + offset, out_str_size - offset, "%s,", pos->name);
		offset = offset + sbi_strlen(pos->name) + 1;
	}

	যদি (offset)
		out_str[offset - 1] = '\0';
	অন্যথা
		sbi_strncpy(out_str, "none", out_str_size);
}

পূর্ণাঙ্ক sbi_hart_protection_register(সংরচনা sbi_hart_protection *hprot)
{
	সংরচনা sbi_hart_protection *pos = রিক্ত;
	bool found_pos = false;

	যদি (!hprot)
		ঘূৰাই SBI_EINVAL;
	যদি (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		ঘূৰাই SBI_EINVAL;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		যদি (hprot->rating > pos->rating) {
			found_pos = true;
			ভঙ্গ;
		}
	}

	যদি (found_pos)
		sbi_list_add_tail(&hprot->head, &pos->head);
	অন্যথা
		sbi_list_add_tail(&hprot->head, &hart_protection_list);

	ঘূৰাই 0;
}

শূন্য sbi_hart_protection_unregister(সংরচনা sbi_hart_protection *hprot)
{
	যদি (!hprot)
		ঘূৰাই;

	sbi_list_del(&hprot->head);
}

স্থির পূর্ণাঙ্ক __hart_protection_configure(সংরচনা sbi_scratch *scratch,
				       সংরচনা sbi_hart_protection *hprot,
				       সংরচনা sbi_domain *dom)
{
	যদি (!hprot)
		ঘূৰাই 0;
	যদি (!hprot->configure)
		ঘূৰাই SBI_ENOSYS;

	ঘূৰাই hprot->configure(scratch, dom);
}

স্থির শূন্য __hart_protection_unconfigure(সংরচনা sbi_scratch *scratch,
					  সংরচনা sbi_hart_protection *hprot,
					  সংরচনা sbi_domain *dom)
{
	যদি (!hprot || !hprot->unconfigure)
		ঘূৰাই;

	hprot->unconfigure(scratch, dom);
}

পূর্ণাঙ্ক sbi_hart_protection_configure(সংরচনা sbi_scratch *scratch,
				  সংরচনা sbi_domain *dom)
{
	bool do_configure, memory_protect_done = false;
	সংরচনা sbi_hart_protection *hprot;
	পূর্ণাঙ্ক ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_configure = false;
		ৱিকল্প (hprot->type) {
		পক্ষ SBI_HART_PROTECTION_TYPE_MEMORY:
			do_configure = !memory_protect_done;
			memory_protect_done = true;
			ভঙ্গ;
		পক্ষ SBI_HART_PROTECTION_TYPE_ID:
			do_configure = true;
			ভঙ্গ;
		স্ৱভাৱ:
			ভঙ্গ;
		}
		যদি (!do_configure)
			অনুৱর্ত;

		ret = __hart_protection_configure(scratch, hprot, dom);
		যদি (ret)
			ঘূৰাই ret;
	}

	ঘূৰাই 0;
}

শূন্য sbi_hart_protection_unconfigure(সংরচনা sbi_scratch *scratch,
				     সংরচনা sbi_domain *dom)
{

	bool do_unconfigure, memory_protect_done = false;
	সংরচনা sbi_hart_protection *hprot;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_unconfigure = false;
		ৱিকল্প (hprot->type) {
		পক্ষ SBI_HART_PROTECTION_TYPE_MEMORY:
			do_unconfigure = !memory_protect_done;
			memory_protect_done = true;
			ভঙ্গ;
		পক্ষ SBI_HART_PROTECTION_TYPE_ID:
			do_unconfigure = true;
			ভঙ্গ;
		স্ৱভাৱ:
			ভঙ্গ;
		}
		যদি (!do_unconfigure)
			অনুৱর্ত;

		__hart_protection_unconfigure(scratch, hprot, dom);
	}
}

পূর্ণাঙ্ক sbi_hart_protection_reconfigure(সংরচনা sbi_scratch *scratch,
				    সংরচনা sbi_domain *current_dom,
				    সংরচনা sbi_domain *next_dom)
{
	bool do_reconfigure, memory_protect_done = false;
	সংরচনা sbi_hart_protection *hprot;
	পূর্ণাঙ্ক ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_reconfigure = false;
		ৱিকল্প (hprot->type) {
		পক্ষ SBI_HART_PROTECTION_TYPE_MEMORY:
			do_reconfigure = !memory_protect_done;
			memory_protect_done = true;
			ভঙ্গ;
		পক্ষ SBI_HART_PROTECTION_TYPE_ID:
			do_reconfigure = true;
			ভঙ্গ;
		স্ৱভাৱ:
			ভঙ্গ;
		}
		যদি (!do_reconfigure)
			অনুৱর্ত;

		__hart_protection_unconfigure(scratch, hprot, current_dom);
		ret = __hart_protection_configure(scratch, hprot, next_dom);
		যদি (ret)
			ঘূৰাই ret;
	}

	ঘূৰাই 0;
}

পূর্ণাঙ্ক sbi_hart_protection_temp_map_range(unsigned long base, unsigned long size)
{
	সংরচনা sbi_hart_protection *hprot = __hart_memory_protection_best();

	যদি (!hprot || !hprot->temp_map_range)
		ঘূৰাই 0;

	ঘূৰাই hprot->temp_map_range(sbi_scratch_thishart_ptr(), base, size);
}

পূর্ণাঙ্ক sbi_hart_protection_temp_unmap_range(unsigned long base, unsigned long size)
{
	সংরচনা sbi_hart_protection *hprot = __hart_memory_protection_best();

	যদি (!hprot || !hprot->temp_unmap_range)
		ঘূৰাই 0;

	ঘূৰাই hprot->temp_unmap_range(sbi_scratch_thishart_ptr(), base, size);
}
