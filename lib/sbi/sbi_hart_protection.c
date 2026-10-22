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

ساكن SBI_LIST_سر(hart_protection_list);

ساكن بنية sbi_hart_protection *__hart_memory_protection_best(فراغ)
{
	بنية sbi_hart_protection *pos;

	sbi_list_for_each_مدخل(pos, &hart_protection_list, سر) {
		إذا (pos->type == SBI_HART_PROTECTION_TYPE_حافظه)
			أرجع pos;
	}

	أرجع عدم;
}

فراغ sbi_hart_protection_get_str(حرف *بیرون_str, صحيح بیرون_str_اندازه)
{
	bool حافظه_protect_انجامشده = false;
	بنية sbi_hart_protection *pos;
	صحيح جابه_جایی = 0;

	إذا (!بیرون_str || بیرون_str_اندازه <= 0)
		أرجع;
	sbi_memset(بیرون_str, 0, بیرون_str_اندازه);

	sbi_list_for_each_مدخل(pos, &hart_protection_list, سر) {
		إذا (pos->type == SBI_HART_PROTECTION_TYPE_حافظه) {
			إذا (حافظه_protect_انجامشده)
				استمر;
			حافظه_protect_انجامشده = true;
		}
		sbi_snprintf(بیرون_str + جابه_جایی, بیرون_str_اندازه - جابه_جایی, "%s,", pos->name);
		جابه_جایی = جابه_جایی + sbi_strlen(pos->name) + 1;
	}

	إذا (جابه_جایی)
		بیرون_str[جابه_جایی - 1] = '\0';
	وإلا
		sbi_strncpy(بیرون_str, "none", بیرون_str_اندازه);
}

صحيح sbi_hart_protection_ثبات(بنية sbi_hart_protection *hprot)
{
	بنية sbi_hart_protection *pos = عدم;
	bool یافت_pos = false;

	إذا (!hprot)
		أرجع SBI_EINVAL;
	إذا (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		أرجع SBI_EINVAL;

	sbi_list_for_each_مدخل(pos, &hart_protection_list, سر) {
		إذا (hprot->rating > pos->rating) {
			یافت_pos = true;
			اكسر;
		}
	}

	إذا (یافت_pos)
		sbi_list_جمع_کن_دم(&hprot->سر, &pos->سر);
	وإلا
		sbi_list_جمع_کن_دم(&hprot->سر, &hart_protection_list);

	أرجع 0;
}

فراغ sbi_hart_protection_unregister(بنية sbi_hart_protection *hprot)
{
	إذا (!hprot)
		أرجع;

	sbi_list_del(&hprot->سر);
}

ساكن صحيح __hart_protection_configure(بنية sbi_scratch *scratch,
				       بنية sbi_hart_protection *hprot,
				       بنية sbi_domain *dom)
{
	إذا (!hprot)
		أرجع 0;
	إذا (!hprot->configure)
		أرجع SBI_ENOSYS;

	أرجع hprot->configure(scratch, dom);
}

ساكن فراغ __hart_protection_unconfigure(بنية sbi_scratch *scratch,
					  بنية sbi_hart_protection *hprot,
					  بنية sbi_domain *dom)
{
	إذا (!hprot || !hprot->unconfigure)
		أرجع;

	hprot->unconfigure(scratch, dom);
}

صحيح sbi_hart_protection_configure(بنية sbi_scratch *scratch,
				  بنية sbi_domain *dom)
{
	bool do_configure, حافظه_protect_انجامشده = false;
	بنية sbi_hart_protection *hprot;
	صحيح ret;

	sbi_list_for_each_مدخل(hprot, &hart_protection_list, سر) {
		do_configure = false;
		بدل (hprot->type) {
		حالة SBI_HART_PROTECTION_TYPE_حافظه:
			do_configure = !حافظه_protect_انجامشده;
			حافظه_protect_انجامشده = true;
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
			أرجع ret;
	}

	أرجع 0;
}

فراغ sbi_hart_protection_unconfigure(بنية sbi_scratch *scratch,
				     بنية sbi_domain *dom)
{

	bool do_unconfigure, حافظه_protect_انجامشده = false;
	بنية sbi_hart_protection *hprot;

	sbi_list_for_each_مدخل(hprot, &hart_protection_list, سر) {
		do_unconfigure = false;
		بدل (hprot->type) {
		حالة SBI_HART_PROTECTION_TYPE_حافظه:
			do_unconfigure = !حافظه_protect_انجامشده;
			حافظه_protect_انجامشده = true;
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
				    بنية sbi_domain *بعدی_dom)
{
	bool do_reconfigure, حافظه_protect_انجامشده = false;
	بنية sbi_hart_protection *hprot;
	صحيح ret;

	sbi_list_for_each_مدخل(hprot, &hart_protection_list, سر) {
		do_reconfigure = false;
		بدل (hprot->type) {
		حالة SBI_HART_PROTECTION_TYPE_حافظه:
			do_reconfigure = !حافظه_protect_انجامشده;
			حافظه_protect_انجامشده = true;
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
		ret = __hart_protection_configure(scratch, hprot, بعدی_dom);
		إذا (ret)
			أرجع ret;
	}

	أرجع 0;
}

صحيح sbi_hart_protection_موقت_map_range(بیعلامت دراز پایه_2, بیعلامت دراز اندازه)
{
	بنية sbi_hart_protection *hprot = __hart_memory_protection_best();

	إذا (!hprot || !hprot->موقت_map_range)
		أرجع 0;

	أرجع hprot->موقت_map_range(sbi_scratch_thishart_ptr(), پایه_2, اندازه);
}

صحيح sbi_hart_protection_موقت_unmap_range(بیعلامت دراز پایه_2, بیعلامت دراز اندازه)
{
	بنية sbi_hart_protection *hprot = __hart_memory_protection_best();

	إذا (!hprot || !hprot->موقت_unmap_range)
		أرجع 0;

	أرجع hprot->موقت_unmap_range(sbi_scratch_thishart_ptr(), پایه_2, اندازه);
}
