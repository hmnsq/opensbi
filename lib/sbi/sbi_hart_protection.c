/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_hart_protection.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>
#অন্তর্ভাব <sbi/sbi_string.h>

স্থির SBI_LIST_HEAD(hart_protection_list);

স্থির সংরচনা sbi_hart_protection *__hart_memory_protection_best(শূন্য)
{
	সংরচনা sbi_hart_protection *pos;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		যদি (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY)
			প্রত্যাগম pos;
	}

	প্রত্যাগম রিক্ত;
}

শূন্য sbi_hart_protection_get_str(বর্ণ *out_str, পূর্ণাঙ্ক out_str_size)
{
	bool memory_protect_done = false;
	সংরচনা sbi_hart_protection *pos;
	পূর্ণাঙ্ক offset = 0;

	যদি (!out_str || out_str_size <= 0)
		প্রত্যাগম;
	sbi_memset(out_str, 0, out_str_size);

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		যদি (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY) {
			যদি (memory_protect_done)
				অনুবর্ত;
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
		প্রত্যাগম SBI_EINVAL;
	যদি (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		প্রত্যাগম SBI_EINVAL;

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

	প্রত্যাগম 0;
}

শূন্য sbi_hart_protection_unregister(সংরচনা sbi_hart_protection *hprot)
{
	যদি (!hprot)
		প্রত্যাগম;

	sbi_list_del(&hprot->head);
}

স্থির পূর্ণাঙ্ক __hart_protection_configure(সংরচনা sbi_scratch *scratch,
				       সংরচনা sbi_hart_protection *hprot,
				       সংরচনা sbi_domain *dom)
{
	যদি (!hprot)
		প্রত্যাগম 0;
	যদি (!hprot->configure)
		প্রত্যাগম SBI_ENOSYS;

	প্রত্যাগম hprot->configure(scratch, dom);
}

স্থির শূন্য __hart_protection_unconfigure(সংরচনা sbi_scratch *scratch,
					  সংরচনা sbi_hart_protection *hprot,
					  সংরচনা sbi_domain *dom)
{
	যদি (!hprot || !hprot->unconfigure)
		প্রত্যাগম;

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
		বিকল্প (hprot->type) {
		পক্ষ SBI_HART_PROTECTION_TYPE_MEMORY:
			do_configure = !memory_protect_done;
			memory_protect_done = true;
			ভঙ্গ;
		পক্ষ SBI_HART_PROTECTION_TYPE_ID:
			do_configure = true;
			ভঙ্গ;
		স্বভাব:
			ভঙ্গ;
		}
		যদি (!do_configure)
			অনুবর্ত;

		ret = __hart_protection_configure(scratch, hprot, dom);
		যদি (ret)
			প্রত্যাগম ret;
	}

	প্রত্যাগম 0;
}

শূন্য sbi_hart_protection_unconfigure(সংরচনা sbi_scratch *scratch,
				     সংরচনা sbi_domain *dom)
{

	bool do_unconfigure, memory_protect_done = false;
	সংরচনা sbi_hart_protection *hprot;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_unconfigure = false;
		বিকল্প (hprot->type) {
		পক্ষ SBI_HART_PROTECTION_TYPE_MEMORY:
			do_unconfigure = !memory_protect_done;
			memory_protect_done = true;
			ভঙ্গ;
		পক্ষ SBI_HART_PROTECTION_TYPE_ID:
			do_unconfigure = true;
			ভঙ্গ;
		স্বভাব:
			ভঙ্গ;
		}
		যদি (!do_unconfigure)
			অনুবর্ত;

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
		বিকল্প (hprot->type) {
		পক্ষ SBI_HART_PROTECTION_TYPE_MEMORY:
			do_reconfigure = !memory_protect_done;
			memory_protect_done = true;
			ভঙ্গ;
		পক্ষ SBI_HART_PROTECTION_TYPE_ID:
			do_reconfigure = true;
			ভঙ্গ;
		স্বভাব:
			ভঙ্গ;
		}
		যদি (!do_reconfigure)
			অনুবর্ত;

		__hart_protection_unconfigure(scratch, hprot, current_dom);
		ret = __hart_protection_configure(scratch, hprot, next_dom);
		যদি (ret)
			প্রত্যাগম ret;
	}

	প্রত্যাগম 0;
}

পূর্ণাঙ্ক sbi_hart_protection_temp_map_range(unsigned long base, unsigned long size)
{
	সংরচনা sbi_hart_protection *hprot = __hart_memory_protection_best();

	যদি (!hprot || !hprot->temp_map_range)
		প্রত্যাগম 0;

	প্রত্যাগম hprot->temp_map_range(sbi_scratch_thishart_ptr(), base, size);
}

পূর্ণাঙ্ক sbi_hart_protection_temp_unmap_range(unsigned long base, unsigned long size)
{
	সংরচনা sbi_hart_protection *hprot = __hart_memory_protection_best();

	যদি (!hprot || !hprot->temp_unmap_range)
		প্রত্যাগম 0;

	প্রত্যাগম hprot->temp_unmap_range(sbi_scratch_thishart_ptr(), base, size);
}
