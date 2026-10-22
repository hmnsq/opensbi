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

	sbi_list_for_each_ثبت(pos, &hart_protection_list, سر) {
		إذا (pos->type == SBI_HART_PROTECTION_TYPE_حافظه)
			أرجع pos;
	}

	أرجع عدم;
}

فراغ sbi_hart_protection_get_str(حرف *بهر_str, صحيح بهر_str_اندازه)
{
	bool حافظه_protect_بشپړ = false;
	بنية sbi_hart_protection *pos;
	صحيح دننه_واټن = 0;

	إذا (!بهر_str || بهر_str_اندازه <= 0)
		أرجع;
	sbi_memset(بهر_str, 0, بهر_str_اندازه);

	sbi_list_for_each_ثبت(pos, &hart_protection_list, سر) {
		إذا (pos->type == SBI_HART_PROTECTION_TYPE_حافظه) {
			إذا (حافظه_protect_بشپړ)
				استمر;
			حافظه_protect_بشپړ = true;
		}
		sbi_snprintf(بهر_str + دننه_واټن, بهر_str_اندازه - دننه_واټن, "%s,", pos->name);
		دننه_واټن = دننه_واټن + sbi_strlen(pos->name) + 1;
	}

	إذا (دننه_واټن)
		بهر_str[دننه_واټن - 1] = '\0';
	وإلا
		sbi_strncpy(بهر_str, "none", بهر_str_اندازه);
}

صحيح sbi_hart_protection_ثبتوونکی(بنية sbi_hart_protection *hprot)
{
	بنية sbi_hart_protection *pos = عدم;
	bool وموندل_pos = false;

	إذا (!hprot)
		أرجع SBI_EINVAL;
	إذا (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		أرجع SBI_EINVAL;

	sbi_list_for_each_ثبت(pos, &hart_protection_list, سر) {
		إذا (hprot->rating > pos->rating) {
			وموندل_pos = true;
			اكسر;
		}
	}

	إذا (وموندل_pos)
		sbi_list_جمع_کړه_لکۍ(&hprot->سر, &pos->سر);
	وإلا
		sbi_list_جمع_کړه_لکۍ(&hprot->سر, &hart_protection_list);

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
	bool do_configure, حافظه_protect_بشپړ = false;
	بنية sbi_hart_protection *hprot;
	صحيح ret;

	sbi_list_for_each_ثبت(hprot, &hart_protection_list, سر) {
		do_configure = false;
		بدل (hprot->type) {
		حالة SBI_HART_PROTECTION_TYPE_حافظه:
			do_configure = !حافظه_protect_بشپړ;
			حافظه_protect_بشپړ = true;
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

	bool do_unconfigure, حافظه_protect_بشپړ = false;
	بنية sbi_hart_protection *hprot;

	sbi_list_for_each_ثبت(hprot, &hart_protection_list, سر) {
		do_unconfigure = false;
		بدل (hprot->type) {
		حالة SBI_HART_PROTECTION_TYPE_حافظه:
			do_unconfigure = !حافظه_protect_بشپړ;
			حافظه_protect_بشپړ = true;
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
				    بنية sbi_domain *بل_dom)
{
	bool do_reconfigure, حافظه_protect_بشپړ = false;
	بنية sbi_hart_protection *hprot;
	صحيح ret;

	sbi_list_for_each_ثبت(hprot, &hart_protection_list, سر) {
		do_reconfigure = false;
		بدل (hprot->type) {
		حالة SBI_HART_PROTECTION_TYPE_حافظه:
			do_reconfigure = !حافظه_protect_بشپړ;
			حافظه_protect_بشپړ = true;
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
		ret = __hart_protection_configure(scratch, hprot, بل_dom);
		إذا (ret)
			أرجع ret;
	}

	أرجع 0;
}

صحيح sbi_hart_protection_لنډمهاله_map_range(بینښه اوږد بنسټ_2, بینښه اوږد اندازه)
{
	بنية sbi_hart_protection *hprot = __hart_memory_protection_best();

	إذا (!hprot || !hprot->لنډمهاله_map_range)
		أرجع 0;

	أرجع hprot->لنډمهاله_map_range(sbi_scratch_thishart_ptr(), بنسټ_2, اندازه);
}

صحيح sbi_hart_protection_لنډمهاله_unmap_range(بینښه اوږد بنسټ_2, بینښه اوږد اندازه)
{
	بنية sbi_hart_protection *hprot = __hart_memory_protection_best();

	إذا (!hprot || !hprot->لنډمهاله_unmap_range)
		أرجع 0;

	أرجع hprot->لنډمهاله_unmap_range(sbi_scratch_thishart_ptr(), بنسټ_2, اندازه);
}
