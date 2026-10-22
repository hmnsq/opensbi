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

	sbi_list_for_each_داخلا(pos, &hart_protection_list, سر) {
		إذا (pos->type == SBI_HART_PROTECTION_TYPE_يادگيري)
			أرجع pos;
	}

	أرجع عدم;
}

فراغ sbi_hart_protection_get_str(حرف *ٻاهريون_str, صحيح ٻاهريون_str_سائيز)
{
	bool يادگيري_protect_پورو = false;
	بنية sbi_hart_protection *pos;
	صحيح وٿي = 0;

	إذا (!ٻاهريون_str || ٻاهريون_str_سائيز <= 0)
		أرجع;
	sbi_memset(ٻاهريون_str, 0, ٻاهريون_str_سائيز);

	sbi_list_for_each_داخلا(pos, &hart_protection_list, سر) {
		إذا (pos->type == SBI_HART_PROTECTION_TYPE_يادگيري) {
			إذا (يادگيري_protect_پورو)
				استمر;
			يادگيري_protect_پورو = true;
		}
		sbi_snprintf(ٻاهريون_str + وٿي, ٻاهريون_str_سائيز - وٿي, "%s,", pos->name);
		وٿي = وٿي + sbi_strlen(pos->name) + 1;
	}

	إذا (وٿي)
		ٻاهريون_str[وٿي - 1] = '\0';
	وإلا
		sbi_strncpy(ٻاهريون_str, "none", ٻاهريون_str_سائيز);
}

صحيح sbi_hart_protection_رجسٽر(بنية sbi_hart_protection *hprot)
{
	بنية sbi_hart_protection *pos = عدم;
	bool مليو_pos = false;

	إذا (!hprot)
		أرجع SBI_EINVAL;
	إذا (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		أرجع SBI_EINVAL;

	sbi_list_for_each_داخلا(pos, &hart_protection_list, سر) {
		إذا (hprot->rating > pos->rating) {
			مليو_pos = true;
			اكسر;
		}
	}

	إذا (مليو_pos)
		sbi_list_جوڙيو_پڇ(&hprot->سر, &pos->سر);
	وإلا
		sbi_list_جوڙيو_پڇ(&hprot->سر, &hart_protection_list);

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
	bool do_configure, يادگيري_protect_پورو = false;
	بنية sbi_hart_protection *hprot;
	صحيح ret;

	sbi_list_for_each_داخلا(hprot, &hart_protection_list, سر) {
		do_configure = false;
		بدل (hprot->type) {
		حالة SBI_HART_PROTECTION_TYPE_يادگيري:
			do_configure = !يادگيري_protect_پورو;
			يادگيري_protect_پورو = true;
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

	bool do_unconfigure, يادگيري_protect_پورو = false;
	بنية sbi_hart_protection *hprot;

	sbi_list_for_each_داخلا(hprot, &hart_protection_list, سر) {
		do_unconfigure = false;
		بدل (hprot->type) {
		حالة SBI_HART_PROTECTION_TYPE_يادگيري:
			do_unconfigure = !يادگيري_protect_پورو;
			يادگيري_protect_پورو = true;
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
				    بنية sbi_domain *ايندڙ_dom)
{
	bool do_reconfigure, يادگيري_protect_پورو = false;
	بنية sbi_hart_protection *hprot;
	صحيح ret;

	sbi_list_for_each_داخلا(hprot, &hart_protection_list, سر) {
		do_reconfigure = false;
		بدل (hprot->type) {
		حالة SBI_HART_PROTECTION_TYPE_يادگيري:
			do_reconfigure = !يادگيري_protect_پورو;
			يادگيري_protect_پورو = true;
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
		ret = __hart_protection_configure(scratch, hprot, ايندڙ_dom);
		إذا (ret)
			أرجع ret;
	}

	أرجع 0;
}

صحيح sbi_hart_protection_عارضي_map_range(بنانشان ڊگهو بنياد_2, بنانشان ڊگهو سائيز)
{
	بنية sbi_hart_protection *hprot = __hart_memory_protection_best();

	إذا (!hprot || !hprot->عارضي_map_range)
		أرجع 0;

	أرجع hprot->عارضي_map_range(sbi_scratch_thishart_ptr(), بنياد_2, سائيز);
}

صحيح sbi_hart_protection_عارضي_unmap_range(بنانشان ڊگهو بنياد_2, بنانشان ڊگهو سائيز)
{
	بنية sbi_hart_protection *hprot = __hart_memory_protection_best();

	إذا (!hprot || !hprot->عارضي_unmap_range)
		أرجع 0;

	أرجع hprot->عارضي_unmap_range(sbi_scratch_thishart_ptr(), بنياد_2, سائيز);
}
