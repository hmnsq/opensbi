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
		만약 (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY)
			반환 pos;
	}

	반환 NULL;
}

void sbi_hart_protection_get_str(char *out_str, 정수 out_str_size)
{
	bool memory_protect_done = false;
	struct sbi_hart_protection *pos;
	정수 offset = 0;

	만약 (!out_str || out_str_size <= 0)
		반환;
	sbi_memset(out_str, 0, out_str_size);

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		만약 (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY) {
			만약 (memory_protect_done)
				continue;
			memory_protect_done = true;
		}
		sbi_snprintf(out_str + offset, out_str_size - offset, "%s,", pos->name);
		offset = offset + sbi_strlen(pos->name) + 1;
	}

	만약 (offset)
		out_str[offset - 1] = '\0';
	아니면
		sbi_strncpy(out_str, "none", out_str_size);
}

정수 sbi_hart_protection_register(struct sbi_hart_protection *hprot)
{
	struct sbi_hart_protection *pos = NULL;
	bool found_pos = false;

	만약 (!hprot)
		반환 SBI_EINVAL;
	만약 (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		반환 SBI_EINVAL;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		만약 (hprot->rating > pos->rating) {
			found_pos = true;
			break;
		}
	}

	만약 (found_pos)
		sbi_list_add_tail(&hprot->head, &pos->head);
	아니면
		sbi_list_add_tail(&hprot->head, &hart_protection_list);

	반환 0;
}

void sbi_hart_protection_unregister(struct sbi_hart_protection *hprot)
{
	만약 (!hprot)
		반환;

	sbi_list_del(&hprot->head);
}

static 정수 __hart_protection_configure(struct sbi_scratch *scratch,
				       struct sbi_hart_protection *hprot,
				       struct sbi_domain *dom)
{
	만약 (!hprot)
		반환 0;
	만약 (!hprot->configure)
		반환 SBI_ENOSYS;

	반환 hprot->configure(scratch, dom);
}

static void __hart_protection_unconfigure(struct sbi_scratch *scratch,
					  struct sbi_hart_protection *hprot,
					  struct sbi_domain *dom)
{
	만약 (!hprot || !hprot->unconfigure)
		반환;

	hprot->unconfigure(scratch, dom);
}

정수 sbi_hart_protection_configure(struct sbi_scratch *scratch,
				  struct sbi_domain *dom)
{
	bool do_configure, memory_protect_done = false;
	struct sbi_hart_protection *hprot;
	정수 ret;

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
		만약 (!do_configure)
			continue;

		ret = __hart_protection_configure(scratch, hprot, dom);
		만약 (ret)
			반환 ret;
	}

	반환 0;
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
		만약 (!do_unconfigure)
			continue;

		__hart_protection_unconfigure(scratch, hprot, dom);
	}
}

정수 sbi_hart_protection_reconfigure(struct sbi_scratch *scratch,
				    struct sbi_domain *current_dom,
				    struct sbi_domain *next_dom)
{
	bool do_reconfigure, memory_protect_done = false;
	struct sbi_hart_protection *hprot;
	정수 ret;

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
		만약 (!do_reconfigure)
			continue;

		__hart_protection_unconfigure(scratch, hprot, current_dom);
		ret = __hart_protection_configure(scratch, hprot, next_dom);
		만약 (ret)
			반환 ret;
	}

	반환 0;
}

정수 sbi_hart_protection_temp_map_range(unsigned long base, unsigned long size)
{
	struct sbi_hart_protection *hprot = __hart_memory_protection_best();

	만약 (!hprot || !hprot->temp_map_range)
		반환 0;

	반환 hprot->temp_map_range(sbi_scratch_thishart_ptr(), base, size);
}

정수 sbi_hart_protection_temp_unmap_range(unsigned long base, unsigned long size)
{
	struct sbi_hart_protection *hprot = __hart_memory_protection_best();

	만약 (!hprot || !hprot->temp_unmap_range)
		반환 0;

	반환 hprot->temp_unmap_range(sbi_scratch_thishart_ptr(), base, size);
}
