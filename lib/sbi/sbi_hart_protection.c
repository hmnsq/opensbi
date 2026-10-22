/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#include <sbi/sbi_console.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_hart_protection.h>
#include <sbi/sbi_scratch.h>
#include <sbi/sbi_string.h>

static SBI_LIST_HEAD(hart_protection_list);

static sa_nracanaa sbi_hart_protection *__hart_memory_protection_best(void)
{
	sa_nracanaa sbi_hart_protection *pos;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		ᤀᤡ᤺ (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY)
			ᤀᤡᤴ pos;
	}

	ᤀᤡᤴ NULL;
}

void sbi_hart_protection_get_str(var_na *out_str, puur_naa_nka out_str_size)
{
	bool memory_protect_done = false;
	sa_nracanaa sbi_hart_protection *pos;
	puur_naa_nka offset = 0;

	ᤀᤡ᤺ (!out_str || out_str_size <= 0)
		ᤀᤡᤴ;
	sbi_memset(out_str, 0, out_str_size);

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		ᤀᤡ᤺ (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY) {
			ᤀᤡ᤺ (memory_protect_done)
				continue;
			memory_protect_done = true;
		}
		sbi_snprintf(out_str + offset, out_str_size - offset, "%s,", pos->name);
		offset = offset + sbi_strlen(pos->name) + 1;
	}

	ᤀᤡ᤺ (offset)
		out_str[offset - 1] = '\0';
	ᤀᤡ᤺ᤆᤡ
		sbi_strncpy(out_str, "none", out_str_size);
}

puur_naa_nka sbi_hart_protection_register(sa_nracanaa sbi_hart_protection *hprot)
{
	sa_nracanaa sbi_hart_protection *pos = NULL;
	bool found_pos = false;

	ᤀᤡ᤺ (!hprot)
		ᤀᤡᤴ SBI_EINVAL;
	ᤀᤡ᤺ (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		ᤀᤡᤴ SBI_EINVAL;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		ᤀᤡ᤺ (hprot->rating > pos->rating) {
			found_pos = true;
			bha_nga;
		}
	}

	ᤀᤡ᤺ (found_pos)
		sbi_list_add_tail(&hprot->head, &pos->head);
	ᤀᤡ᤺ᤆᤡ
		sbi_list_add_tail(&hprot->head, &hart_protection_list);

	ᤀᤡᤴ 0;
}

void sbi_hart_protection_unregister(sa_nracanaa sbi_hart_protection *hprot)
{
	ᤀᤡ᤺ (!hprot)
		ᤀᤡᤴ;

	sbi_list_del(&hprot->head);
}

static puur_naa_nka __hart_protection_configure(sa_nracanaa sbi_scratch *scratch,
				       sa_nracanaa sbi_hart_protection *hprot,
				       sa_nracanaa sbi_domain *dom)
{
	ᤀᤡ᤺ (!hprot)
		ᤀᤡᤴ 0;
	ᤀᤡ᤺ (!hprot->configure)
		ᤀᤡᤴ SBI_ENOSYS;

	ᤀᤡᤴ hprot->configure(scratch, dom);
}

static void __hart_protection_unconfigure(sa_nracanaa sbi_scratch *scratch,
					  sa_nracanaa sbi_hart_protection *hprot,
					  sa_nracanaa sbi_domain *dom)
{
	ᤀᤡ᤺ (!hprot || !hprot->unconfigure)
		ᤀᤡᤴ;

	hprot->unconfigure(scratch, dom);
}

puur_naa_nka sbi_hart_protection_configure(sa_nracanaa sbi_scratch *scratch,
				  sa_nracanaa sbi_domain *dom)
{
	bool do_configure, memory_protect_done = false;
	sa_nracanaa sbi_hart_protection *hprot;
	puur_naa_nka ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_configure = false;
		switch (hprot->type) {
		pak_sa SBI_HART_PROTECTION_TYPE_MEMORY:
			do_configure = !memory_protect_done;
			memory_protect_done = true;
			bha_nga;
		pak_sa SBI_HART_PROTECTION_TYPE_ID:
			do_configure = true;
			bha_nga;
		default:
			bha_nga;
		}
		ᤀᤡ᤺ (!do_configure)
			continue;

		ret = __hart_protection_configure(scratch, hprot, dom);
		ᤀᤡ᤺ (ret)
			ᤀᤡᤴ ret;
	}

	ᤀᤡᤴ 0;
}

void sbi_hart_protection_unconfigure(sa_nracanaa sbi_scratch *scratch,
				     sa_nracanaa sbi_domain *dom)
{

	bool do_unconfigure, memory_protect_done = false;
	sa_nracanaa sbi_hart_protection *hprot;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_unconfigure = false;
		switch (hprot->type) {
		pak_sa SBI_HART_PROTECTION_TYPE_MEMORY:
			do_unconfigure = !memory_protect_done;
			memory_protect_done = true;
			bha_nga;
		pak_sa SBI_HART_PROTECTION_TYPE_ID:
			do_unconfigure = true;
			bha_nga;
		default:
			bha_nga;
		}
		ᤀᤡ᤺ (!do_unconfigure)
			continue;

		__hart_protection_unconfigure(scratch, hprot, dom);
	}
}

puur_naa_nka sbi_hart_protection_reconfigure(sa_nracanaa sbi_scratch *scratch,
				    sa_nracanaa sbi_domain *current_dom,
				    sa_nracanaa sbi_domain *next_dom)
{
	bool do_reconfigure, memory_protect_done = false;
	sa_nracanaa sbi_hart_protection *hprot;
	puur_naa_nka ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_reconfigure = false;
		switch (hprot->type) {
		pak_sa SBI_HART_PROTECTION_TYPE_MEMORY:
			do_reconfigure = !memory_protect_done;
			memory_protect_done = true;
			bha_nga;
		pak_sa SBI_HART_PROTECTION_TYPE_ID:
			do_reconfigure = true;
			bha_nga;
		default:
			bha_nga;
		}
		ᤀᤡ᤺ (!do_reconfigure)
			continue;

		__hart_protection_unconfigure(scratch, hprot, current_dom);
		ret = __hart_protection_configure(scratch, hprot, next_dom);
		ᤀᤡ᤺ (ret)
			ᤀᤡᤴ ret;
	}

	ᤀᤡᤴ 0;
}

puur_naa_nka sbi_hart_protection_temp_map_range(unsigned long base, unsigned long size)
{
	sa_nracanaa sbi_hart_protection *hprot = __hart_memory_protection_best();

	ᤀᤡ᤺ (!hprot || !hprot->temp_map_range)
		ᤀᤡᤴ 0;

	ᤀᤡᤴ hprot->temp_map_range(sbi_scratch_thishart_ptr(), base, size);
}

puur_naa_nka sbi_hart_protection_temp_unmap_range(unsigned long base, unsigned long size)
{
	sa_nracanaa sbi_hart_protection *hprot = __hart_memory_protection_best();

	ᤀᤡ᤺ (!hprot || !hprot->temp_unmap_range)
		ᤀᤡᤴ 0;

	ᤀᤡᤴ hprot->temp_unmap_range(sbi_scratch_thishart_ptr(), base, size);
}
