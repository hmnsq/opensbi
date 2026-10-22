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

	sbi_list_for_each_اندراج(pos, &hart_protection_list, سر) {
		إذا (pos->type == SBI_HART_PROTECTION_TYPE_حافظہ)
			أرجع pos;
	}

	أرجع عدم;
}

فراغ sbi_hart_protection_get_str(حرف *باہرلا_str, صحيح باہرلا_str_ناپ)
{
	bool حافظہ_protect_پورا = false;
	بنية sbi_hart_protection *pos;
	صحيح وتھ = 0;

	إذا (!باہرلا_str || باہرلا_str_ناپ <= 0)
		أرجع;
	sbi_memset(باہرلا_str, 0, باہرلا_str_ناپ);

	sbi_list_for_each_اندراج(pos, &hart_protection_list, سر) {
		إذا (pos->type == SBI_HART_PROTECTION_TYPE_حافظہ) {
			إذا (حافظہ_protect_پورا)
				استمر;
			حافظہ_protect_پورا = true;
		}
		sbi_snprintf(باہرلا_str + وتھ, باہرلا_str_ناپ - وتھ, "%s,", pos->name);
		وتھ = وتھ + sbi_strlen(pos->name) + 1;
	}

	إذا (وتھ)
		باہرلا_str[وتھ - 1] = '\0';
	وإلا
		sbi_strncpy(باہرلا_str, "none", باہرلا_str_ناپ);
}

صحيح sbi_hart_protection_رجسٹر(بنية sbi_hart_protection *hprot)
{
	بنية sbi_hart_protection *pos = عدم;
	bool لبھیا_pos = false;

	إذا (!hprot)
		أرجع SBI_EINVAL;
	إذا (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		أرجع SBI_EINVAL;

	sbi_list_for_each_اندراج(pos, &hart_protection_list, سر) {
		إذا (hprot->rating > pos->rating) {
			لبھیا_pos = true;
			اكسر;
		}
	}

	إذا (لبھیا_pos)
		sbi_list_جوڑو_پوچھ(&hprot->سر, &pos->سر);
	وإلا
		sbi_list_جوڑو_پوچھ(&hprot->سر, &hart_protection_list);

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
	bool do_configure, حافظہ_protect_پورا = false;
	بنية sbi_hart_protection *hprot;
	صحيح ret;

	sbi_list_for_each_اندراج(hprot, &hart_protection_list, سر) {
		do_configure = false;
		بدل (hprot->type) {
		حالة SBI_HART_PROTECTION_TYPE_حافظہ:
			do_configure = !حافظہ_protect_پورا;
			حافظہ_protect_پورا = true;
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

	bool do_unconfigure, حافظہ_protect_پورا = false;
	بنية sbi_hart_protection *hprot;

	sbi_list_for_each_اندراج(hprot, &hart_protection_list, سر) {
		do_unconfigure = false;
		بدل (hprot->type) {
		حالة SBI_HART_PROTECTION_TYPE_حافظہ:
			do_unconfigure = !حافظہ_protect_پورا;
			حافظہ_protect_پورا = true;
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
				    بنية sbi_domain *اگلا_dom)
{
	bool do_reconfigure, حافظہ_protect_پورا = false;
	بنية sbi_hart_protection *hprot;
	صحيح ret;

	sbi_list_for_each_اندراج(hprot, &hart_protection_list, سر) {
		do_reconfigure = false;
		بدل (hprot->type) {
		حالة SBI_HART_PROTECTION_TYPE_حافظہ:
			do_reconfigure = !حافظہ_protect_پورا;
			حافظہ_protect_پورا = true;
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
		ret = __hart_protection_configure(scratch, hprot, اگلا_dom);
		إذا (ret)
			أرجع ret;
	}

	أرجع 0;
}

صحيح sbi_hart_protection_عارضی_map_range(بنانشانی لما ادھار_2, بنانشانی لما ناپ)
{
	بنية sbi_hart_protection *hprot = __hart_memory_protection_best();

	إذا (!hprot || !hprot->عارضی_map_range)
		أرجع 0;

	أرجع hprot->عارضی_map_range(sbi_scratch_thishart_ptr(), ادھار_2, ناپ);
}

صحيح sbi_hart_protection_عارضی_unmap_range(بنانشانی لما ادھار_2, بنانشانی لما ناپ)
{
	بنية sbi_hart_protection *hprot = __hart_memory_protection_best();

	إذا (!hprot || !hprot->عارضی_unmap_range)
		أرجع 0;

	أرجع hprot->عارضی_unmap_range(sbi_scratch_thishart_ptr(), ادھار_2, ناپ);
}
