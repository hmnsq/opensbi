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

static struct sbi_hart_protection *__hart_memory_protection_best(void)
{
	struct sbi_hart_protection *pos;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		ако (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY)
			врати pos;
	}

	врати NULL;
}

void sbi_hart_protection_get_str(char *out_str, цео out_str_size)
{
	bool memory_protect_done = false;
	struct sbi_hart_protection *pos;
	цео offset = 0;

	ако (!out_str || out_str_size <= 0)
		врати;
	sbi_memset(out_str, 0, out_str_size);

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		ако (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY) {
			ако (memory_protect_done)
				continue;
			memory_protect_done = true;
		}
		sbi_snprintf(out_str + offset, out_str_size - offset, "%s,", pos->name);
		offset = offset + sbi_strlen(pos->name) + 1;
	}

	ако (offset)
		out_str[offset - 1] = '\0';
	инако
		sbi_strncpy(out_str, "none", out_str_size);
}

цео sbi_hart_protection_register(struct sbi_hart_protection *hprot)
{
	struct sbi_hart_protection *pos = NULL;
	bool found_pos = false;

	ако (!hprot)
		врати SBI_EINVAL;
	ако (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		врати SBI_EINVAL;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		ако (hprot->rating > pos->rating) {
			found_pos = true;
			break;
		}
	}

	ако (found_pos)
		sbi_list_add_tail(&hprot->head, &pos->head);
	инако
		sbi_list_add_tail(&hprot->head, &hart_protection_list);

	врати 0;
}

void sbi_hart_protection_unregister(struct sbi_hart_protection *hprot)
{
	ако (!hprot)
		врати;

	sbi_list_del(&hprot->head);
}

static цео __hart_protection_configure(struct sbi_scratch *scratch,
				       struct sbi_hart_protection *hprot,
				       struct sbi_domain *dom)
{
	ако (!hprot)
		врати 0;
	ако (!hprot->configure)
		врати SBI_ENOSYS;

	врати hprot->configure(scratch, dom);
}

static void __hart_protection_unconfigure(struct sbi_scratch *scratch,
					  struct sbi_hart_protection *hprot,
					  struct sbi_domain *dom)
{
	ако (!hprot || !hprot->unconfigure)
		врати;

	hprot->unconfigure(scratch, dom);
}

цео sbi_hart_protection_configure(struct sbi_scratch *scratch,
				  struct sbi_domain *dom)
{
	bool do_configure, memory_protect_done = false;
	struct sbi_hart_protection *hprot;
	цео ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_configure = false;
		switch (hprot->type) {
		case SBI_HART_PROTECTION_TYPE_MEMORY:
			do_configure = !memory_protect_done;
			memory_protect_done = true;
			break;
		case SBI_HART_PROTECTION_TYPE_ID:
			do_configure = true;
			break;
		default:
			break;
		}
		ако (!do_configure)
			continue;

		ret = __hart_protection_configure(scratch, hprot, dom);
		ако (ret)
			врати ret;
	}

	врати 0;
}

void sbi_hart_protection_unconfigure(struct sbi_scratch *scratch,
				     struct sbi_domain *dom)
{

	bool do_unconfigure, memory_protect_done = false;
	struct sbi_hart_protection *hprot;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_unconfigure = false;
		switch (hprot->type) {
		case SBI_HART_PROTECTION_TYPE_MEMORY:
			do_unconfigure = !memory_protect_done;
			memory_protect_done = true;
			break;
		case SBI_HART_PROTECTION_TYPE_ID:
			do_unconfigure = true;
			break;
		default:
			break;
		}
		ако (!do_unconfigure)
			continue;

		__hart_protection_unconfigure(scratch, hprot, dom);
	}
}

цео sbi_hart_protection_reconfigure(struct sbi_scratch *scratch,
				    struct sbi_domain *current_dom,
				    struct sbi_domain *next_dom)
{
	bool do_reconfigure, memory_protect_done = false;
	struct sbi_hart_protection *hprot;
	цео ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_reconfigure = false;
		switch (hprot->type) {
		case SBI_HART_PROTECTION_TYPE_MEMORY:
			do_reconfigure = !memory_protect_done;
			memory_protect_done = true;
			break;
		case SBI_HART_PROTECTION_TYPE_ID:
			do_reconfigure = true;
			break;
		default:
			break;
		}
		ако (!do_reconfigure)
			continue;

		__hart_protection_unconfigure(scratch, hprot, current_dom);
		ret = __hart_protection_configure(scratch, hprot, next_dom);
		ако (ret)
			врати ret;
	}

	врати 0;
}

цео sbi_hart_protection_temp_map_range(unsigned long base, unsigned long size)
{
	struct sbi_hart_protection *hprot = __hart_memory_protection_best();

	ако (!hprot || !hprot->temp_map_range)
		врати 0;

	врати hprot->temp_map_range(sbi_scratch_thishart_ptr(), base, size);
}

цео sbi_hart_protection_temp_unmap_range(unsigned long base, unsigned long size)
{
	struct sbi_hart_protection *hprot = __hart_memory_protection_best();

	ако (!hprot || !hprot->temp_unmap_range)
		врати 0;

	врати hprot->temp_unmap_range(sbi_scratch_thishart_ptr(), base, size);
}
