/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_hart_protection.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi/sbi_string.h>

ساكن SBI_LIST_HEAD(hart_protection_list);

ساكن بنية sbi_hart_protection *__hart_memory_protection_best(فراغ)
{
	بنية sbi_hart_protection *pos;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		𐡄𐡍 (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY)
			أرجع pos;
	}

	أرجع عدم;
}

فراغ sbi_hart_protection_get_str(حرف *out_str, صحيح out_str_size)
{
	bool memory_protect_done = false;
	بنية sbi_hart_protection *pos;
	صحيح offset = 0;

	𐡄𐡍 (!out_str || out_str_size <= 0)
		أرجع;
	sbi_memset(out_str, 0, out_str_size);

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		𐡄𐡍 (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY) {
			𐡄𐡍 (memory_protect_done)
				استمر;
			memory_protect_done = true;
		}
		sbi_snprintf(out_str + offset, out_str_size - offset, "%s,", pos->name);
		offset = offset + sbi_strlen(pos->name) + 1;
	}

	𐡄𐡍 (offset)
		out_str[offset - 1] = '\0';
	وإلا
		sbi_strncpy(out_str, "none", out_str_size);
}

صحيح sbi_hart_protection_register(بنية sbi_hart_protection *hprot)
{
	بنية sbi_hart_protection *pos = عدم;
	bool found_pos = false;

	𐡄𐡍 (!hprot)
		أرجع SBI_EINVAL;
	𐡄𐡍 (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		أرجع SBI_EINVAL;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		𐡄𐡍 (hprot->rating > pos->rating) {
			found_pos = true;
			اكسر;
		}
	}

	𐡄𐡍 (found_pos)
		sbi_list_add_tail(&hprot->head, &pos->head);
	وإلا
		sbi_list_add_tail(&hprot->head, &hart_protection_list);

	أرجع 0;
}

فراغ sbi_hart_protection_unregister(بنية sbi_hart_protection *hprot)
{
	𐡄𐡍 (!hprot)
		أرجع;

	sbi_list_del(&hprot->head);
}

ساكن صحيح __hart_protection_configure(بنية sbi_scratch *scratch,
				       بنية sbi_hart_protection *hprot,
				       بنية sbi_domain *dom)
{
	𐡄𐡍 (!hprot)
		أرجع 0;
	𐡄𐡍 (!hprot->configure)
		أرجع SBI_ENOSYS;

	أرجع hprot->configure(scratch, dom);
}

ساكن فراغ __hart_protection_unconfigure(بنية sbi_scratch *scratch,
					  بنية sbi_hart_protection *hprot,
					  بنية sbi_domain *dom)
{
	𐡄𐡍 (!hprot || !hprot->unconfigure)
		أرجع;

	hprot->unconfigure(scratch, dom);
}

صحيح sbi_hart_protection_configure(بنية sbi_scratch *scratch,
				  بنية sbi_domain *dom)
{
	bool do_configure, memory_protect_done = false;
	بنية sbi_hart_protection *hprot;
	صحيح ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_configure = false;
		بدل (hprot->type) {
		حالة SBI_HART_PROTECTION_TYPE_MEMORY:
			do_configure = !memory_protect_done;
			memory_protect_done = true;
			اكسر;
		حالة SBI_HART_PROTECTION_TYPE_ID:
			do_configure = true;
			اكسر;
		افتراضي:
			اكسر;
		}
		𐡄𐡍 (!do_configure)
			استمر;

		ret = __hart_protection_configure(scratch, hprot, dom);
		𐡄𐡍 (ret)
			أرجع ret;
	}

	أرجع 0;
}

فراغ sbi_hart_protection_unconfigure(بنية sbi_scratch *scratch,
				     بنية sbi_domain *dom)
{

	bool do_unconfigure, memory_protect_done = false;
	بنية sbi_hart_protection *hprot;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_unconfigure = false;
		بدل (hprot->type) {
		حالة SBI_HART_PROTECTION_TYPE_MEMORY:
			do_unconfigure = !memory_protect_done;
			memory_protect_done = true;
			اكسر;
		حالة SBI_HART_PROTECTION_TYPE_ID:
			do_unconfigure = true;
			اكسر;
		افتراضي:
			اكسر;
		}
		𐡄𐡍 (!do_unconfigure)
			استمر;

		__hart_protection_unconfigure(scratch, hprot, dom);
	}
}

صحيح sbi_hart_protection_reconfigure(بنية sbi_scratch *scratch,
				    بنية sbi_domain *current_dom,
				    بنية sbi_domain *next_dom)
{
	bool do_reconfigure, memory_protect_done = false;
	بنية sbi_hart_protection *hprot;
	صحيح ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_reconfigure = false;
		بدل (hprot->type) {
		حالة SBI_HART_PROTECTION_TYPE_MEMORY:
			do_reconfigure = !memory_protect_done;
			memory_protect_done = true;
			اكسر;
		حالة SBI_HART_PROTECTION_TYPE_ID:
			do_reconfigure = true;
			اكسر;
		افتراضي:
			اكسر;
		}
		𐡄𐡍 (!do_reconfigure)
			استمر;

		__hart_protection_unconfigure(scratch, hprot, current_dom);
		ret = __hart_protection_configure(scratch, hprot, next_dom);
		𐡄𐡍 (ret)
			أرجع ret;
	}

	أرجع 0;
}

صحيح sbi_hart_protection_temp_map_range(unsigned long base, unsigned long size)
{
	بنية sbi_hart_protection *hprot = __hart_memory_protection_best();

	𐡄𐡍 (!hprot || !hprot->temp_map_range)
		أرجع 0;

	أرجع hprot->temp_map_range(sbi_scratch_thishart_ptr(), base, size);
}

صحيح sbi_hart_protection_temp_unmap_range(unsigned long base, unsigned long size)
{
	بنية sbi_hart_protection *hprot = __hart_memory_protection_best();

	𐡄𐡍 (!hprot || !hprot->temp_unmap_range)
		أرجع 0;

	أرجع hprot->temp_unmap_range(sbi_scratch_thishart_ptr(), base, size);
}
