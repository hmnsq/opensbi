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
		nếu (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY)
			trả_về pos;
	}

	trả_về NULL;
}

void sbi_hart_protection_get_str(char *out_str, số_nguyên out_str_size)
{
	bool memory_protect_done = false;
	struct sbi_hart_protection *pos;
	số_nguyên offset = 0;

	nếu (!out_str || out_str_size <= 0)
		trả_về;
	sbi_memset(out_str, 0, out_str_size);

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		nếu (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY) {
			nếu (memory_protect_done)
				continue;
			memory_protect_done = true;
		}
		sbi_snprintf(out_str + offset, out_str_size - offset, "%s,", pos->name);
		offset = offset + sbi_strlen(pos->name) + 1;
	}

	nếu (offset)
		out_str[offset - 1] = '\0';
	không_thì
		sbi_strncpy(out_str, "none", out_str_size);
}

số_nguyên sbi_hart_protection_register(struct sbi_hart_protection *hprot)
{
	struct sbi_hart_protection *pos = NULL;
	bool found_pos = false;

	nếu (!hprot)
		trả_về SBI_EINVAL;
	nếu (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		trả_về SBI_EINVAL;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		nếu (hprot->rating > pos->rating) {
			found_pos = true;
			break;
		}
	}

	nếu (found_pos)
		sbi_list_add_tail(&hprot->head, &pos->head);
	không_thì
		sbi_list_add_tail(&hprot->head, &hart_protection_list);

	trả_về 0;
}

void sbi_hart_protection_unregister(struct sbi_hart_protection *hprot)
{
	nếu (!hprot)
		trả_về;

	sbi_list_del(&hprot->head);
}

static số_nguyên __hart_protection_configure(struct sbi_scratch *scratch,
				       struct sbi_hart_protection *hprot,
				       struct sbi_domain *dom)
{
	nếu (!hprot)
		trả_về 0;
	nếu (!hprot->configure)
		trả_về SBI_ENOSYS;

	trả_về hprot->configure(scratch, dom);
}

static void __hart_protection_unconfigure(struct sbi_scratch *scratch,
					  struct sbi_hart_protection *hprot,
					  struct sbi_domain *dom)
{
	nếu (!hprot || !hprot->unconfigure)
		trả_về;

	hprot->unconfigure(scratch, dom);
}

số_nguyên sbi_hart_protection_configure(struct sbi_scratch *scratch,
				  struct sbi_domain *dom)
{
	bool do_configure, memory_protect_done = false;
	struct sbi_hart_protection *hprot;
	số_nguyên ret;

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
		nếu (!do_configure)
			continue;

		ret = __hart_protection_configure(scratch, hprot, dom);
		nếu (ret)
			trả_về ret;
	}

	trả_về 0;
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
		nếu (!do_unconfigure)
			continue;

		__hart_protection_unconfigure(scratch, hprot, dom);
	}
}

số_nguyên sbi_hart_protection_reconfigure(struct sbi_scratch *scratch,
				    struct sbi_domain *current_dom,
				    struct sbi_domain *next_dom)
{
	bool do_reconfigure, memory_protect_done = false;
	struct sbi_hart_protection *hprot;
	số_nguyên ret;

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
		nếu (!do_reconfigure)
			continue;

		__hart_protection_unconfigure(scratch, hprot, current_dom);
		ret = __hart_protection_configure(scratch, hprot, next_dom);
		nếu (ret)
			trả_về ret;
	}

	trả_về 0;
}

số_nguyên sbi_hart_protection_temp_map_range(unsigned long base, unsigned long size)
{
	struct sbi_hart_protection *hprot = __hart_memory_protection_best();

	nếu (!hprot || !hprot->temp_map_range)
		trả_về 0;

	trả_về hprot->temp_map_range(sbi_scratch_thishart_ptr(), base, size);
}

số_nguyên sbi_hart_protection_temp_unmap_range(unsigned long base, unsigned long size)
{
	struct sbi_hart_protection *hprot = __hart_memory_protection_best();

	nếu (!hprot || !hprot->temp_unmap_range)
		trả_về 0;

	trả_về hprot->temp_unmap_range(sbi_scratch_thishart_ptr(), base, size);
}
