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

ساکن SBI_LIST_سر(hart_protection_list);

ساکن ساخت sbi_hart_protection *__hart_memory_protection_best(خالی)
{
	ساخت sbi_hart_protection *pos;

	sbi_list_for_each_اندراج(pos, &hart_protection_list, سر) {
		اگر (pos->type == SBI_HART_PROTECTION_TYPE_حافظہ)
			واپس pos;
	}

	واپس تہی;
}

خالی sbi_hart_protection_get_str(حرف *برآمد_str, صحیح برآمد_str_سائز)
{
	bool حافظہ_protect_مکمل = false;
	ساخت sbi_hart_protection *pos;
	صحیح انحراف = 0;

	اگر (!برآمد_str || برآمد_str_سائز <= 0)
		واپس;
	sbi_memset(برآمد_str, 0, برآمد_str_سائز);

	sbi_list_for_each_اندراج(pos, &hart_protection_list, سر) {
		اگر (pos->type == SBI_HART_PROTECTION_TYPE_حافظہ) {
			اگر (حافظہ_protect_مکمل)
				جاری;
			حافظہ_protect_مکمل = true;
		}
		sbi_snprintf(برآمد_str + انحراف, برآمد_str_سائز - انحراف, "%s,", pos->name);
		انحراف = انحراف + sbi_strlen(pos->name) + 1;
	}

	اگر (انحراف)
		برآمد_str[انحراف - 1] = '\0';
	ورنہ
		sbi_strncpy(برآمد_str, "none", برآمد_str_سائز);
}

صحیح sbi_hart_protection_رجسٹر(ساخت sbi_hart_protection *hprot)
{
	ساخت sbi_hart_protection *pos = تہی;
	bool ملا_pos = false;

	اگر (!hprot)
		واپس SBI_EINVAL;
	اگر (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		واپس SBI_EINVAL;

	sbi_list_for_each_اندراج(pos, &hart_protection_list, سر) {
		اگر (hprot->rating > pos->rating) {
			ملا_pos = true;
			توڑو;
		}
	}

	اگر (ملا_pos)
		sbi_list_جوڑو_دم(&hprot->سر, &pos->سر);
	ورنہ
		sbi_list_جوڑو_دم(&hprot->سر, &hart_protection_list);

	واپس 0;
}

خالی sbi_hart_protection_unregister(ساخت sbi_hart_protection *hprot)
{
	اگر (!hprot)
		واپس;

	sbi_list_del(&hprot->سر);
}

ساکن صحیح __hart_protection_configure(ساخت sbi_scratch *scratch,
				       ساخت sbi_hart_protection *hprot,
				       ساخت sbi_domain *dom)
{
	اگر (!hprot)
		واپس 0;
	اگر (!hprot->configure)
		واپس SBI_ENOSYS;

	واپس hprot->configure(scratch, dom);
}

ساکن خالی __hart_protection_unconfigure(ساخت sbi_scratch *scratch,
					  ساخت sbi_hart_protection *hprot,
					  ساخت sbi_domain *dom)
{
	اگر (!hprot || !hprot->unconfigure)
		واپس;

	hprot->unconfigure(scratch, dom);
}

صحیح sbi_hart_protection_configure(ساخت sbi_scratch *scratch,
				  ساخت sbi_domain *dom)
{
	bool do_configure, حافظہ_protect_مکمل = false;
	ساخت sbi_hart_protection *hprot;
	صحیح ret;

	sbi_list_for_each_اندراج(hprot, &hart_protection_list, سر) {
		do_configure = false;
		انتخاب (hprot->type) {
		حالت SBI_HART_PROTECTION_TYPE_حافظہ:
			do_configure = !حافظہ_protect_مکمل;
			حافظہ_protect_مکمل = true;
			توڑو;
		حالت SBI_HART_PROTECTION_TYPE_ID:
			do_configure = true;
			توڑو;
		طےشدہ:
			توڑو;
		}
		اگر (!do_configure)
			جاری;

		ret = __hart_protection_configure(scratch, hprot, dom);
		اگر (ret)
			واپس ret;
	}

	واپس 0;
}

خالی sbi_hart_protection_unconfigure(ساخت sbi_scratch *scratch,
				     ساخت sbi_domain *dom)
{

	bool do_unconfigure, حافظہ_protect_مکمل = false;
	ساخت sbi_hart_protection *hprot;

	sbi_list_for_each_اندراج(hprot, &hart_protection_list, سر) {
		do_unconfigure = false;
		انتخاب (hprot->type) {
		حالت SBI_HART_PROTECTION_TYPE_حافظہ:
			do_unconfigure = !حافظہ_protect_مکمل;
			حافظہ_protect_مکمل = true;
			توڑو;
		حالت SBI_HART_PROTECTION_TYPE_ID:
			do_unconfigure = true;
			توڑو;
		طےشدہ:
			توڑو;
		}
		اگر (!do_unconfigure)
			جاری;

		__hart_protection_unconfigure(scratch, hprot, dom);
	}
}

صحیح sbi_hart_protection_reconfigure(ساخت sbi_scratch *scratch,
				    ساخت sbi_domain *current_dom,
				    ساخت sbi_domain *اگلا_dom)
{
	bool do_reconfigure, حافظہ_protect_مکمل = false;
	ساخت sbi_hart_protection *hprot;
	صحیح ret;

	sbi_list_for_each_اندراج(hprot, &hart_protection_list, سر) {
		do_reconfigure = false;
		انتخاب (hprot->type) {
		حالت SBI_HART_PROTECTION_TYPE_حافظہ:
			do_reconfigure = !حافظہ_protect_مکمل;
			حافظہ_protect_مکمل = true;
			توڑو;
		حالت SBI_HART_PROTECTION_TYPE_ID:
			do_reconfigure = true;
			توڑو;
		طےشدہ:
			توڑو;
		}
		اگر (!do_reconfigure)
			جاری;

		__hart_protection_unconfigure(scratch, hprot, current_dom);
		ret = __hart_protection_configure(scratch, hprot, اگلا_dom);
		اگر (ret)
			واپس ret;
	}

	واپس 0;
}

صحیح sbi_hart_protection_عارضی_map_range(غیرعلامتی طویل بنیاد_2, غیرعلامتی طویل سائز)
{
	ساخت sbi_hart_protection *hprot = __hart_memory_protection_best();

	اگر (!hprot || !hprot->عارضی_map_range)
		واپس 0;

	واپس hprot->عارضی_map_range(sbi_scratch_thishart_ptr(), بنیاد_2, سائز);
}

صحیح sbi_hart_protection_عارضی_unmap_range(غیرعلامتی طویل بنیاد_2, غیرعلامتی طویل سائز)
{
	ساخت sbi_hart_protection *hprot = __hart_memory_protection_best();

	اگر (!hprot || !hprot->عارضی_unmap_range)
		واپس 0;

	واپس hprot->عارضی_unmap_range(sbi_scratch_thishart_ptr(), بنیاد_2, سائز);
}
