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

স্থির SBI_LIST_শীর্ষ(hart_protection_list);

স্থির কাঠামো sbi_hart_protection *__hart_memory_protection_best(শূন্য)
{
	কাঠামো sbi_hart_protection *pos;

	sbi_list_for_each_entry(pos, &hart_protection_list, শীর্ষ) {
		যদি (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY)
			ফেরত pos;
	}

	ফেরত ফাঁকা;
}

শূন্য sbi_hart_protection_get_str(অক্ষর *out_str, পূর্ণসংখ্যা out_str_আকার)
{
	bool memory_protect_done = false;
	কাঠামো sbi_hart_protection *pos;
	পূর্ণসংখ্যা offset = 0;

	যদি (!out_str || out_str_আকার <= 0)
		ফেরত;
	sbi_memset(out_str, 0, out_str_আকার);

	sbi_list_for_each_entry(pos, &hart_protection_list, শীর্ষ) {
		যদি (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY) {
			যদি (memory_protect_done)
				চালু;
			memory_protect_done = true;
		}
		sbi_snprintf(out_str + offset, out_str_আকার - offset, "%s,", pos->name);
		offset = offset + sbi_strlen(pos->name) + 1;
	}

	যদি (offset)
		out_str[offset - 1] = '\0';
	অন্যথা
		sbi_strncpy(out_str, "none", out_str_আকার);
}

পূর্ণসংখ্যা sbi_hart_protection_register(কাঠামো sbi_hart_protection *hprot)
{
	কাঠামো sbi_hart_protection *pos = ফাঁকা;
	bool found_pos = false;

	যদি (!hprot)
		ফেরত SBI_EINVAL;
	যদি (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		ফেরত SBI_EINVAL;

	sbi_list_for_each_entry(pos, &hart_protection_list, শীর্ষ) {
		যদি (hprot->rating > pos->rating) {
			found_pos = true;
			বিরতি;
		}
	}

	যদি (found_pos)
		sbi_list_add_লেজ(&hprot->শীর্ষ, &pos->শীর্ষ);
	অন্যথা
		sbi_list_add_লেজ(&hprot->শীর্ষ, &hart_protection_list);

	ফেরত 0;
}

শূন্য sbi_hart_protection_unregister(কাঠামো sbi_hart_protection *hprot)
{
	যদি (!hprot)
		ফেরত;

	sbi_list_del(&hprot->শীর্ষ);
}

স্থির পূর্ণসংখ্যা __hart_protection_configure(কাঠামো sbi_scratch *scratch,
				       কাঠামো sbi_hart_protection *hprot,
				       কাঠামো sbi_domain *dom)
{
	যদি (!hprot)
		ফেরত 0;
	যদি (!hprot->configure)
		ফেরত SBI_ENOSYS;

	ফেরত hprot->configure(scratch, dom);
}

স্থির শূন্য __hart_protection_unconfigure(কাঠামো sbi_scratch *scratch,
					  কাঠামো sbi_hart_protection *hprot,
					  কাঠামো sbi_domain *dom)
{
	যদি (!hprot || !hprot->unconfigure)
		ফেরত;

	hprot->unconfigure(scratch, dom);
}

পূর্ণসংখ্যা sbi_hart_protection_configure(কাঠামো sbi_scratch *scratch,
				  কাঠামো sbi_domain *dom)
{
	bool do_configure, memory_protect_done = false;
	কাঠামো sbi_hart_protection *hprot;
	পূর্ণসংখ্যা ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, শীর্ষ) {
		do_configure = false;
		নির্বাচন (hprot->type) {
		ক্ষেত্র SBI_HART_PROTECTION_TYPE_MEMORY:
			do_configure = !memory_protect_done;
			memory_protect_done = true;
			বিরতি;
		ক্ষেত্র SBI_HART_PROTECTION_TYPE_ID:
			do_configure = true;
			বিরতি;
		পূর্বনির্ধারিত:
			বিরতি;
		}
		যদি (!do_configure)
			চালু;

		ret = __hart_protection_configure(scratch, hprot, dom);
		যদি (ret)
			ফেরত ret;
	}

	ফেরত 0;
}

শূন্য sbi_hart_protection_unconfigure(কাঠামো sbi_scratch *scratch,
				     কাঠামো sbi_domain *dom)
{

	bool do_unconfigure, memory_protect_done = false;
	কাঠামো sbi_hart_protection *hprot;

	sbi_list_for_each_entry(hprot, &hart_protection_list, শীর্ষ) {
		do_unconfigure = false;
		নির্বাচন (hprot->type) {
		ক্ষেত্র SBI_HART_PROTECTION_TYPE_MEMORY:
			do_unconfigure = !memory_protect_done;
			memory_protect_done = true;
			বিরতি;
		ক্ষেত্র SBI_HART_PROTECTION_TYPE_ID:
			do_unconfigure = true;
			বিরতি;
		পূর্বনির্ধারিত:
			বিরতি;
		}
		যদি (!do_unconfigure)
			চালু;

		__hart_protection_unconfigure(scratch, hprot, dom);
	}
}

পূর্ণসংখ্যা sbi_hart_protection_reconfigure(কাঠামো sbi_scratch *scratch,
				    কাঠামো sbi_domain *current_dom,
				    কাঠামো sbi_domain *পরবর্তী_dom)
{
	bool do_reconfigure, memory_protect_done = false;
	কাঠামো sbi_hart_protection *hprot;
	পূর্ণসংখ্যা ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, শীর্ষ) {
		do_reconfigure = false;
		নির্বাচন (hprot->type) {
		ক্ষেত্র SBI_HART_PROTECTION_TYPE_MEMORY:
			do_reconfigure = !memory_protect_done;
			memory_protect_done = true;
			বিরতি;
		ক্ষেত্র SBI_HART_PROTECTION_TYPE_ID:
			do_reconfigure = true;
			বিরতি;
		পূর্বনির্ধারিত:
			বিরতি;
		}
		যদি (!do_reconfigure)
			চালু;

		__hart_protection_unconfigure(scratch, hprot, current_dom);
		ret = __hart_protection_configure(scratch, hprot, পরবর্তী_dom);
		যদি (ret)
			ফেরত ret;
	}

	ফেরত 0;
}

পূর্ণসংখ্যা sbi_hart_protection_অস্থায়ী_map_range(unsigned long base, unsigned long আকার_2)
{
	কাঠামো sbi_hart_protection *hprot = __hart_memory_protection_best();

	যদি (!hprot || !hprot->অস্থায়ী_map_range)
		ফেরত 0;

	ফেরত hprot->অস্থায়ী_map_range(sbi_scratch_thishart_ptr(), base, আকার_2);
}

পূর্ণসংখ্যা sbi_hart_protection_অস্থায়ী_unmap_range(unsigned long base, unsigned long আকার_2)
{
	কাঠামো sbi_hart_protection *hprot = __hart_memory_protection_best();

	যদি (!hprot || !hprot->অস্থায়ী_unmap_range)
		ফেরত 0;

	ফেরত hprot->অস্থায়ী_unmap_range(sbi_scratch_thishart_ptr(), base, আকার_2);
}
