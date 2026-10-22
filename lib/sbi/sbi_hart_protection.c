/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart_protection.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_string.h>

ਸਥਿਰ SBI_LIST_ਸਿਰ(hart_protection_list);

ਸਥਿਰ ਬਣਤਰ sbi_hart_protection *__hart_memory_protection_best(ਖਾਲੀ)
{
	ਬਣਤਰ sbi_hart_protection *pos;

	sbi_list_for_each_ਇੰਦਰਾਜ(pos, &hart_protection_list, ਸਿਰ) {
		ਜੇ (pos->type == SBI_HART_PROTECTION_TYPE_ਸਮ੍ਰਿਤੀ)
			ਵਾਪਸ pos;
	}

	ਵਾਪਸ ਨਿਰਾ;
}

ਖਾਲੀ sbi_hart_protection_get_str(ਅੱਖਰ *ਬਾਹਰਲਾ_str, ਪੂਰਨ ਬਾਹਰਲਾ_str_ਮਾਪ)
{
	bool ਸਮ੍ਰਿਤੀ_protect_ਪੂਰਾ = false;
	ਬਣਤਰ sbi_hart_protection *pos;
	ਪੂਰਨ ਵਿੱਥ = 0;

	ਜੇ (!ਬਾਹਰਲਾ_str || ਬਾਹਰਲਾ_str_ਮਾਪ <= 0)
		ਵਾਪਸ;
	sbi_memset(ਬਾਹਰਲਾ_str, 0, ਬਾਹਰਲਾ_str_ਮਾਪ);

	sbi_list_for_each_ਇੰਦਰਾਜ(pos, &hart_protection_list, ਸਿਰ) {
		ਜੇ (pos->type == SBI_HART_PROTECTION_TYPE_ਸਮ੍ਰਿਤੀ) {
			ਜੇ (ਸਮ੍ਰਿਤੀ_protect_ਪੂਰਾ)
				ਜਾਰੀ;
			ਸਮ੍ਰਿਤੀ_protect_ਪੂਰਾ = true;
		}
		sbi_snprintf(ਬਾਹਰਲਾ_str + ਵਿੱਥ, ਬਾਹਰਲਾ_str_ਮਾਪ - ਵਿੱਥ, "%s,", pos->name);
		ਵਿੱਥ = ਵਿੱਥ + sbi_strlen(pos->name) + 1;
	}

	ਜੇ (ਵਿੱਥ)
		ਬਾਹਰਲਾ_str[ਵਿੱਥ - 1] = '\0';
	ਨਹੀਂਤਾਂ
		sbi_strncpy(ਬਾਹਰਲਾ_str, "none", ਬਾਹਰਲਾ_str_ਮਾਪ);
}

ਪੂਰਨ sbi_hart_protection_ਰਜਿਸਟਰ(ਬਣਤਰ sbi_hart_protection *hprot)
{
	ਬਣਤਰ sbi_hart_protection *pos = ਨਿਰਾ;
	bool ਮਿਲਿਆ_pos = false;

	ਜੇ (!hprot)
		ਵਾਪਸ SBI_EINVAL;
	ਜੇ (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		ਵਾਪਸ SBI_EINVAL;

	sbi_list_for_each_ਇੰਦਰਾਜ(pos, &hart_protection_list, ਸਿਰ) {
		ਜੇ (hprot->rating > pos->rating) {
			ਮਿਲਿਆ_pos = true;
			ਰੋਕੋ;
		}
	}

	ਜੇ (ਮਿਲਿਆ_pos)
		sbi_list_ਜੋੜੋ_ਪੂਛ(&hprot->ਸਿਰ, &pos->ਸਿਰ);
	ਨਹੀਂਤਾਂ
		sbi_list_ਜੋੜੋ_ਪੂਛ(&hprot->ਸਿਰ, &hart_protection_list);

	ਵਾਪਸ 0;
}

ਖਾਲੀ sbi_hart_protection_unregister(ਬਣਤਰ sbi_hart_protection *hprot)
{
	ਜੇ (!hprot)
		ਵਾਪਸ;

	sbi_list_del(&hprot->ਸਿਰ);
}

ਸਥਿਰ ਪੂਰਨ __hart_protection_configure(ਬਣਤਰ sbi_scratch *scratch,
				       ਬਣਤਰ sbi_hart_protection *hprot,
				       ਬਣਤਰ sbi_domain *dom)
{
	ਜੇ (!hprot)
		ਵਾਪਸ 0;
	ਜੇ (!hprot->configure)
		ਵਾਪਸ SBI_ENOSYS;

	ਵਾਪਸ hprot->configure(scratch, dom);
}

ਸਥਿਰ ਖਾਲੀ __hart_protection_unconfigure(ਬਣਤਰ sbi_scratch *scratch,
					  ਬਣਤਰ sbi_hart_protection *hprot,
					  ਬਣਤਰ sbi_domain *dom)
{
	ਜੇ (!hprot || !hprot->unconfigure)
		ਵਾਪਸ;

	hprot->unconfigure(scratch, dom);
}

ਪੂਰਨ sbi_hart_protection_configure(ਬਣਤਰ sbi_scratch *scratch,
				  ਬਣਤਰ sbi_domain *dom)
{
	bool do_configure, ਸਮ੍ਰਿਤੀ_protect_ਪੂਰਾ = false;
	ਬਣਤਰ sbi_hart_protection *hprot;
	ਪੂਰਨ ret;

	sbi_list_for_each_ਇੰਦਰਾਜ(hprot, &hart_protection_list, ਸਿਰ) {
		do_configure = false;
		ਚੋਣ (hprot->type) {
		ਹਾਲਤ SBI_HART_PROTECTION_TYPE_ਸਮ੍ਰਿਤੀ:
			do_configure = !ਸਮ੍ਰਿਤੀ_protect_ਪੂਰਾ;
			ਸਮ੍ਰਿਤੀ_protect_ਪੂਰਾ = true;
			ਰੋਕੋ;
		ਹਾਲਤ SBI_HART_PROTECTION_TYPE_ID:
			do_configure = true;
			ਰੋਕੋ;
		ਮੂਲ:
			ਰੋਕੋ;
		}
		ਜੇ (!do_configure)
			ਜਾਰੀ;

		ret = __hart_protection_configure(scratch, hprot, dom);
		ਜੇ (ret)
			ਵਾਪਸ ret;
	}

	ਵਾਪਸ 0;
}

ਖਾਲੀ sbi_hart_protection_unconfigure(ਬਣਤਰ sbi_scratch *scratch,
				     ਬਣਤਰ sbi_domain *dom)
{

	bool do_unconfigure, ਸਮ੍ਰਿਤੀ_protect_ਪੂਰਾ = false;
	ਬਣਤਰ sbi_hart_protection *hprot;

	sbi_list_for_each_ਇੰਦਰਾਜ(hprot, &hart_protection_list, ਸਿਰ) {
		do_unconfigure = false;
		ਚੋਣ (hprot->type) {
		ਹਾਲਤ SBI_HART_PROTECTION_TYPE_ਸਮ੍ਰਿਤੀ:
			do_unconfigure = !ਸਮ੍ਰਿਤੀ_protect_ਪੂਰਾ;
			ਸਮ੍ਰਿਤੀ_protect_ਪੂਰਾ = true;
			ਰੋਕੋ;
		ਹਾਲਤ SBI_HART_PROTECTION_TYPE_ID:
			do_unconfigure = true;
			ਰੋਕੋ;
		ਮੂਲ:
			ਰੋਕੋ;
		}
		ਜੇ (!do_unconfigure)
			ਜਾਰੀ;

		__hart_protection_unconfigure(scratch, hprot, dom);
	}
}

ਪੂਰਨ sbi_hart_protection_reconfigure(ਬਣਤਰ sbi_scratch *scratch,
				    ਬਣਤਰ sbi_domain *current_dom,
				    ਬਣਤਰ sbi_domain *ਅਗਲਾ_dom)
{
	bool do_reconfigure, ਸਮ੍ਰਿਤੀ_protect_ਪੂਰਾ = false;
	ਬਣਤਰ sbi_hart_protection *hprot;
	ਪੂਰਨ ret;

	sbi_list_for_each_ਇੰਦਰਾਜ(hprot, &hart_protection_list, ਸਿਰ) {
		do_reconfigure = false;
		ਚੋਣ (hprot->type) {
		ਹਾਲਤ SBI_HART_PROTECTION_TYPE_ਸਮ੍ਰਿਤੀ:
			do_reconfigure = !ਸਮ੍ਰਿਤੀ_protect_ਪੂਰਾ;
			ਸਮ੍ਰਿਤੀ_protect_ਪੂਰਾ = true;
			ਰੋਕੋ;
		ਹਾਲਤ SBI_HART_PROTECTION_TYPE_ID:
			do_reconfigure = true;
			ਰੋਕੋ;
		ਮੂਲ:
			ਰੋਕੋ;
		}
		ਜੇ (!do_reconfigure)
			ਜਾਰੀ;

		__hart_protection_unconfigure(scratch, hprot, current_dom);
		ret = __hart_protection_configure(scratch, hprot, ਅਗਲਾ_dom);
		ਜੇ (ret)
			ਵਾਪਸ ret;
	}

	ਵਾਪਸ 0;
}

ਪੂਰਨ sbi_hart_protection_ਆਰਜ਼ੀ_map_range(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅਧਾਰ_2, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮਾਪ)
{
	ਬਣਤਰ sbi_hart_protection *hprot = __hart_memory_protection_best();

	ਜੇ (!hprot || !hprot->ਆਰਜ਼ੀ_map_range)
		ਵਾਪਸ 0;

	ਵਾਪਸ hprot->ਆਰਜ਼ੀ_map_range(sbi_scratch_thishart_ptr(), ਅਧਾਰ_2, ਮਾਪ);
}

ਪੂਰਨ sbi_hart_protection_ਆਰਜ਼ੀ_unmap_range(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅਧਾਰ_2, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮਾਪ)
{
	ਬਣਤਰ sbi_hart_protection *hprot = __hart_memory_protection_best();

	ਜੇ (!hprot || !hprot->ਆਰਜ਼ੀ_unmap_range)
		ਵਾਪਸ 0;

	ਵਾਪਸ hprot->ਆਰਜ਼ੀ_unmap_range(sbi_scratch_thishart_ptr(), ਅਧਾਰ_2, ਮਾਪ);
}
