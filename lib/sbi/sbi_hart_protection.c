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
		ຖ້າ (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY)
			ສົ່ງຄືນ pos;
	}

	ສົ່ງຄືນ NULL;
}

void sbi_hart_protection_get_str(var_na *out_str, ຈຳນວນເຕັມ out_str_size)
{
	bool memory_protect_done = false;
	sa_nracanaa sbi_hart_protection *pos;
	ຈຳນວນເຕັມ offset = 0;

	ຖ້າ (!out_str || out_str_size <= 0)
		ສົ່ງຄືນ;
	sbi_memset(out_str, 0, out_str_size);

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		ຖ້າ (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY) {
			ຖ້າ (memory_protect_done)
				continue;
			memory_protect_done = true;
		}
		sbi_snprintf(out_str + offset, out_str_size - offset, "%s,", pos->name);
		offset = offset + sbi_strlen(pos->name) + 1;
	}

	ຖ້າ (offset)
		out_str[offset - 1] = '\0';
	ບໍ່ດັ່ງນັ້ນ
		sbi_strncpy(out_str, "none", out_str_size);
}

ຈຳນວນເຕັມ sbi_hart_protection_register(sa_nracanaa sbi_hart_protection *hprot)
{
	sa_nracanaa sbi_hart_protection *pos = NULL;
	bool found_pos = false;

	ຖ້າ (!hprot)
		ສົ່ງຄືນ SBI_EINVAL;
	ຖ້າ (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		ສົ່ງຄືນ SBI_EINVAL;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		ຖ້າ (hprot->rating > pos->rating) {
			found_pos = true;
			bha_nga;
		}
	}

	ຖ້າ (found_pos)
		sbi_list_add_tail(&hprot->head, &pos->head);
	ບໍ່ດັ່ງນັ້ນ
		sbi_list_add_tail(&hprot->head, &hart_protection_list);

	ສົ່ງຄືນ 0;
}

void sbi_hart_protection_unregister(sa_nracanaa sbi_hart_protection *hprot)
{
	ຖ້າ (!hprot)
		ສົ່ງຄືນ;

	sbi_list_del(&hprot->head);
}

static ຈຳນວນເຕັມ __hart_protection_configure(sa_nracanaa sbi_scratch *scratch,
				       sa_nracanaa sbi_hart_protection *hprot,
				       sa_nracanaa sbi_domain *dom)
{
	ຖ້າ (!hprot)
		ສົ່ງຄືນ 0;
	ຖ້າ (!hprot->configure)
		ສົ່ງຄືນ SBI_ENOSYS;

	ສົ່ງຄືນ hprot->configure(scratch, dom);
}

static void __hart_protection_unconfigure(sa_nracanaa sbi_scratch *scratch,
					  sa_nracanaa sbi_hart_protection *hprot,
					  sa_nracanaa sbi_domain *dom)
{
	ຖ້າ (!hprot || !hprot->unconfigure)
		ສົ່ງຄືນ;

	hprot->unconfigure(scratch, dom);
}

ຈຳນວນເຕັມ sbi_hart_protection_configure(sa_nracanaa sbi_scratch *scratch,
				  sa_nracanaa sbi_domain *dom)
{
	bool do_configure, memory_protect_done = false;
	sa_nracanaa sbi_hart_protection *hprot;
	ຈຳນວນເຕັມ ret;

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
		ຖ້າ (!do_configure)
			continue;

		ret = __hart_protection_configure(scratch, hprot, dom);
		ຖ້າ (ret)
			ສົ່ງຄືນ ret;
	}

	ສົ່ງຄືນ 0;
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
		ຖ້າ (!do_unconfigure)
			continue;

		__hart_protection_unconfigure(scratch, hprot, dom);
	}
}

ຈຳນວນເຕັມ sbi_hart_protection_reconfigure(sa_nracanaa sbi_scratch *scratch,
				    sa_nracanaa sbi_domain *current_dom,
				    sa_nracanaa sbi_domain *next_dom)
{
	bool do_reconfigure, memory_protect_done = false;
	sa_nracanaa sbi_hart_protection *hprot;
	ຈຳນວນເຕັມ ret;

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
		ຖ້າ (!do_reconfigure)
			continue;

		__hart_protection_unconfigure(scratch, hprot, current_dom);
		ret = __hart_protection_configure(scratch, hprot, next_dom);
		ຖ້າ (ret)
			ສົ່ງຄືນ ret;
	}

	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ sbi_hart_protection_temp_map_range(unsigned long base, unsigned long size)
{
	sa_nracanaa sbi_hart_protection *hprot = __hart_memory_protection_best();

	ຖ້າ (!hprot || !hprot->temp_map_range)
		ສົ່ງຄືນ 0;

	ສົ່ງຄືນ hprot->temp_map_range(sbi_scratch_thishart_ptr(), base, size);
}

ຈຳນວນເຕັມ sbi_hart_protection_temp_unmap_range(unsigned long base, unsigned long size)
{
	sa_nracanaa sbi_hart_protection *hprot = __hart_memory_protection_best();

	ຖ້າ (!hprot || !hprot->temp_unmap_range)
		ສົ່ງຄືນ 0;

	ສົ່ງຄືນ hprot->temp_unmap_range(sbi_scratch_thishart_ptr(), base, size);
}
