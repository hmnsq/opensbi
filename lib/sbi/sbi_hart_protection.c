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
		إذا (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY)
			إرجاع pos;
	}

	إرجاع عدم;
}

فراغ sbi_hart_protection_get_str(حرف *out_str, صحيح out_str_size)
{
	bool memory_protect_done = false;
	بنية sbi_hart_protection *pos;
	صحيح offset = 0;

	إذا (!out_str || out_str_size <= 0)
		إرجاع;
	sbi_memset(out_str, 0, out_str_size);

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		إذا (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY) {
			إذا (memory_protect_done)
				استمر;
			memory_protect_done = true;
		}
		sbi_snprintf(out_str + offset, out_str_size - offset, "%s,", pos->name);
		offset = offset + sbi_strlen(pos->name) + 1;
	}

	إذا (offset)
		out_str[offset - 1] = '\0';
	وإلا
		sbi_strncpy(out_str, "none", out_str_size);
}

صحيح sbi_hart_protection_register(بنية sbi_hart_protection *hprot)
{
	بنية sbi_hart_protection *pos = عدم;
	bool found_pos = false;

	إذا (!hprot)
		إرجاع SBI_EINVAL;
	إذا (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		إرجاع SBI_EINVAL;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		إذا (hprot->rating > pos->rating) {
			found_pos = true;
			اكسر;
		}
	}

	إذا (found_pos)
		sbi_list_add_tail(&hprot->head, &pos->head);
	وإلا
		sbi_list_add_tail(&hprot->head, &hart_protection_list);

	إرجاع 0;
}

فراغ sbi_hart_protection_unregister(بنية sbi_hart_protection *hprot)
{
	إذا (!hprot)
		إرجاع;

	sbi_list_del(&hprot->head);
}

ساكن صحيح __hart_protection_configure(بنية sbi_scratch *scratch,
				       بنية sbi_hart_protection *hprot,
				       بنية sbi_domain *dom)
{
	إذا (!hprot)
		إرجاع 0;
	إذا (!hprot->configure)
		إرجاع SBI_ENOSYS;

	إرجاع hprot->configure(scratch, dom);
}

ساكن فراغ __hart_protection_unconfigure(بنية sbi_scratch *scratch,
					  بنية sbi_hart_protection *hprot,
					  بنية sbi_domain *dom)
{
	إذا (!hprot || !hprot->unconfigure)
		إرجاع;

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
		إذا (!do_configure)
			استمر;

		ret = __hart_protection_configure(scratch, hprot, dom);
		إذا (ret)
			إرجاع ret;
	}

	إرجاع 0;
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
		إذا (!do_unconfigure)
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
		إذا (!do_reconfigure)
			استمر;

		__hart_protection_unconfigure(scratch, hprot, current_dom);
		ret = __hart_protection_configure(scratch, hprot, next_dom);
		إذا (ret)
			إرجاع ret;
	}

	إرجاع 0;
}

صحيح sbi_hart_protection_temp_map_range(unsigned long base, unsigned long size)
{
	بنية sbi_hart_protection *hprot = __hart_memory_protection_best();

	إذا (!hprot || !hprot->temp_map_range)
		إرجاع 0;

	إرجاع hprot->temp_map_range(sbi_scratch_thishart_ptr(), base, size);
}

صحيح sbi_hart_protection_temp_unmap_range(unsigned long base, unsigned long size)
{
	بنية sbi_hart_protection *hprot = __hart_memory_protection_best();

	إذا (!hprot || !hprot->temp_unmap_range)
		إرجاع 0;

	إرجاع hprot->temp_unmap_range(sbi_scratch_thishart_ptr(), base, size);
}
