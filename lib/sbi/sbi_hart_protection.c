/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_hart_protection.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_string.h>

ایستا SBI_LIST_HEAD(hart_protection_list);

ایستا ساختار sbi_hart_protection *__hart_memory_protection_best(تهی)
{
	ساختار sbi_hart_protection *pos;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		اگر (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY)
			بازگشت pos;
	}

	بازگشت پوچ;
}

تهی sbi_hart_protection_get_str(نویسه *out_str, صحیح out_str_size)
{
	bool memory_protect_done = false;
	ساختار sbi_hart_protection *pos;
	صحیح offset = 0;

	اگر (!out_str || out_str_size <= 0)
		بازگشت;
	sbi_memset(out_str, 0, out_str_size);

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		اگر (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY) {
			اگر (memory_protect_done)
				ادامه;
			memory_protect_done = true;
		}
		sbi_snprintf(out_str + offset, out_str_size - offset, "%s,", pos->name);
		offset = offset + sbi_strlen(pos->name) + 1;
	}

	اگر (offset)
		out_str[offset - 1] = '\0';
	وگرنه
		sbi_strncpy(out_str, "none", out_str_size);
}

صحیح sbi_hart_protection_register(ساختار sbi_hart_protection *hprot)
{
	ساختار sbi_hart_protection *pos = پوچ;
	bool found_pos = false;

	اگر (!hprot)
		بازگشت SBI_EINVAL;
	اگر (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		بازگشت SBI_EINVAL;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		اگر (hprot->rating > pos->rating) {
			found_pos = true;
			بشکن;
		}
	}

	اگر (found_pos)
		sbi_list_add_tail(&hprot->head, &pos->head);
	وگرنه
		sbi_list_add_tail(&hprot->head, &hart_protection_list);

	بازگشت 0;
}

تهی sbi_hart_protection_unregister(ساختار sbi_hart_protection *hprot)
{
	اگر (!hprot)
		بازگشت;

	sbi_list_del(&hprot->head);
}

ایستا صحیح __hart_protection_configure(ساختار sbi_scratch *scratch,
				       ساختار sbi_hart_protection *hprot,
				       ساختار sbi_domain *dom)
{
	اگر (!hprot)
		بازگشت 0;
	اگر (!hprot->configure)
		بازگشت SBI_ENOSYS;

	بازگشت hprot->configure(scratch, dom);
}

ایستا تهی __hart_protection_unconfigure(ساختار sbi_scratch *scratch,
					  ساختار sbi_hart_protection *hprot,
					  ساختار sbi_domain *dom)
{
	اگر (!hprot || !hprot->unconfigure)
		بازگشت;

	hprot->unconfigure(scratch, dom);
}

صحیح sbi_hart_protection_configure(ساختار sbi_scratch *scratch,
				  ساختار sbi_domain *dom)
{
	bool do_configure, memory_protect_done = false;
	ساختار sbi_hart_protection *hprot;
	صحیح ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_configure = false;
		انتخاب (hprot->type) {
		حالت SBI_HART_PROTECTION_TYPE_MEMORY:
			do_configure = !memory_protect_done;
			memory_protect_done = true;
			بشکن;
		حالت SBI_HART_PROTECTION_TYPE_ID:
			do_configure = true;
			بشکن;
		پیشفرض:
			بشکن;
		}
		اگر (!do_configure)
			ادامه;

		ret = __hart_protection_configure(scratch, hprot, dom);
		اگر (ret)
			بازگشت ret;
	}

	بازگشت 0;
}

تهی sbi_hart_protection_unconfigure(ساختار sbi_scratch *scratch,
				     ساختار sbi_domain *dom)
{

	bool do_unconfigure, memory_protect_done = false;
	ساختار sbi_hart_protection *hprot;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_unconfigure = false;
		انتخاب (hprot->type) {
		حالت SBI_HART_PROTECTION_TYPE_MEMORY:
			do_unconfigure = !memory_protect_done;
			memory_protect_done = true;
			بشکن;
		حالت SBI_HART_PROTECTION_TYPE_ID:
			do_unconfigure = true;
			بشکن;
		پیشفرض:
			بشکن;
		}
		اگر (!do_unconfigure)
			ادامه;

		__hart_protection_unconfigure(scratch, hprot, dom);
	}
}

صحیح sbi_hart_protection_reconfigure(ساختار sbi_scratch *scratch,
				    ساختار sbi_domain *current_dom,
				    ساختار sbi_domain *next_dom)
{
	bool do_reconfigure, memory_protect_done = false;
	ساختار sbi_hart_protection *hprot;
	صحیح ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_reconfigure = false;
		انتخاب (hprot->type) {
		حالت SBI_HART_PROTECTION_TYPE_MEMORY:
			do_reconfigure = !memory_protect_done;
			memory_protect_done = true;
			بشکن;
		حالت SBI_HART_PROTECTION_TYPE_ID:
			do_reconfigure = true;
			بشکن;
		پیشفرض:
			بشکن;
		}
		اگر (!do_reconfigure)
			ادامه;

		__hart_protection_unconfigure(scratch, hprot, current_dom);
		ret = __hart_protection_configure(scratch, hprot, next_dom);
		اگر (ret)
			بازگشت ret;
	}

	بازگشت 0;
}

صحیح sbi_hart_protection_temp_map_range(unsigned long base, unsigned long size)
{
	ساختار sbi_hart_protection *hprot = __hart_memory_protection_best();

	اگر (!hprot || !hprot->temp_map_range)
		بازگشت 0;

	بازگشت hprot->temp_map_range(sbi_scratch_thishart_ptr(), base, size);
}

صحیح sbi_hart_protection_temp_unmap_range(unsigned long base, unsigned long size)
{
	ساختار sbi_hart_protection *hprot = __hart_memory_protection_best();

	اگر (!hprot || !hprot->temp_unmap_range)
		بازگشت 0;

	بازگشت hprot->temp_unmap_range(sbi_scratch_thishart_ptr(), base, size);
}
