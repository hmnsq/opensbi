/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart_protection.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_string.h>

ସ୍ଥିର SBI_LIST_HEAD(hart_protection_list);

ସ୍ଥିର ସଂରଚନା sbi_hart_protection *__hart_memory_protection_best(ଶୂନ୍ୟ)
{
	ସଂରଚନା sbi_hart_protection *pos;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		ଯଦି (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY)
			ଫେରନ୍ତୁ pos;
	}

	ଫେରନ୍ତୁ ଖାଲି;
}

ଶୂନ୍ୟ sbi_hart_protection_get_str(ଅକ୍ଷର *out_str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା out_str_size)
{
	bool memory_protect_done = false;
	ସଂରଚନା sbi_hart_protection *pos;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset = 0;

	ଯଦି (!out_str || out_str_size <= 0)
		ଫେରନ୍ତୁ;
	sbi_memset(out_str, 0, out_str_size);

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		ଯଦି (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY) {
			ଯଦି (memory_protect_done)
				ଜାରି;
			memory_protect_done = true;
		}
		sbi_snprintf(out_str + offset, out_str_size - offset, "%s,", pos->name);
		offset = offset + sbi_strlen(pos->name) + 1;
	}

	ଯଦି (offset)
		out_str[offset - 1] = '\0';
	ଅନ୍ୟଥା
		sbi_strncpy(out_str, "none", out_str_size);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hart_protection_register(ସଂରଚନା sbi_hart_protection *hprot)
{
	ସଂରଚନା sbi_hart_protection *pos = ଖାଲି;
	bool found_pos = false;

	ଯଦି (!hprot)
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଯଦି (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		ଫେରନ୍ତୁ SBI_EINVAL;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		ଯଦି (hprot->rating > pos->rating) {
			found_pos = true;
			ଭାଙ୍ଗ;
		}
	}

	ଯଦି (found_pos)
		sbi_list_add_tail(&hprot->head, &pos->head);
	ଅନ୍ୟଥା
		sbi_list_add_tail(&hprot->head, &hart_protection_list);

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ sbi_hart_protection_unregister(ସଂରଚନା sbi_hart_protection *hprot)
{
	ଯଦି (!hprot)
		ଫେରନ୍ତୁ;

	sbi_list_del(&hprot->head);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା __hart_protection_configure(ସଂରଚନା sbi_scratch *scratch,
				       ସଂରଚନା sbi_hart_protection *hprot,
				       ସଂରଚନା sbi_domain *dom)
{
	ଯଦି (!hprot)
		ଫେରନ୍ତୁ 0;
	ଯଦି (!hprot->configure)
		ଫେରନ୍ତୁ SBI_ENOSYS;

	ଫେରନ୍ତୁ hprot->configure(scratch, dom);
}

ସ୍ଥିର ଶୂନ୍ୟ __hart_protection_unconfigure(ସଂରଚନା sbi_scratch *scratch,
					  ସଂରଚନା sbi_hart_protection *hprot,
					  ସଂରଚନା sbi_domain *dom)
{
	ଯଦି (!hprot || !hprot->unconfigure)
		ଫେରନ୍ତୁ;

	hprot->unconfigure(scratch, dom);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hart_protection_configure(ସଂରଚନା sbi_scratch *scratch,
				  ସଂରଚନା sbi_domain *dom)
{
	bool do_configure, memory_protect_done = false;
	ସଂରଚନା sbi_hart_protection *hprot;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_configure = false;
		ବାଛ (hprot->type) {
		ସ୍ଥିତି SBI_HART_PROTECTION_TYPE_MEMORY:
			do_configure = !memory_protect_done;
			memory_protect_done = true;
			ଭାଙ୍ଗ;
		ସ୍ଥିତି SBI_HART_PROTECTION_TYPE_ID:
			do_configure = true;
			ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ଭାଙ୍ଗ;
		}
		ଯଦି (!do_configure)
			ଜାରି;

		ret = __hart_protection_configure(scratch, hprot, dom);
		ଯଦି (ret)
			ଫେରନ୍ତୁ ret;
	}

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ sbi_hart_protection_unconfigure(ସଂରଚନା sbi_scratch *scratch,
				     ସଂରଚନା sbi_domain *dom)
{

	bool do_unconfigure, memory_protect_done = false;
	ସଂରଚନା sbi_hart_protection *hprot;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_unconfigure = false;
		ବାଛ (hprot->type) {
		ସ୍ଥିତି SBI_HART_PROTECTION_TYPE_MEMORY:
			do_unconfigure = !memory_protect_done;
			memory_protect_done = true;
			ଭାଙ୍ଗ;
		ସ୍ଥିତି SBI_HART_PROTECTION_TYPE_ID:
			do_unconfigure = true;
			ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ଭାଙ୍ଗ;
		}
		ଯଦି (!do_unconfigure)
			ଜାରି;

		__hart_protection_unconfigure(scratch, hprot, dom);
	}
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hart_protection_reconfigure(ସଂରଚନା sbi_scratch *scratch,
				    ସଂରଚନା sbi_domain *current_dom,
				    ସଂରଚନା sbi_domain *next_dom)
{
	bool do_reconfigure, memory_protect_done = false;
	ସଂରଚନା sbi_hart_protection *hprot;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_reconfigure = false;
		ବାଛ (hprot->type) {
		ସ୍ଥିତି SBI_HART_PROTECTION_TYPE_MEMORY:
			do_reconfigure = !memory_protect_done;
			memory_protect_done = true;
			ଭାଙ୍ଗ;
		ସ୍ଥିତି SBI_HART_PROTECTION_TYPE_ID:
			do_reconfigure = true;
			ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ଭାଙ୍ଗ;
		}
		ଯଦି (!do_reconfigure)
			ଜାରି;

		__hart_protection_unconfigure(scratch, hprot, current_dom);
		ret = __hart_protection_configure(scratch, hprot, next_dom);
		ଯଦି (ret)
			ଫେରନ୍ତୁ ret;
	}

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hart_protection_temp_map_range(unsigned long base, unsigned long size)
{
	ସଂରଚନା sbi_hart_protection *hprot = __hart_memory_protection_best();

	ଯଦି (!hprot || !hprot->temp_map_range)
		ଫେରନ୍ତୁ 0;

	ଫେରନ୍ତୁ hprot->temp_map_range(sbi_scratch_thishart_ptr(), base, size);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hart_protection_temp_unmap_range(unsigned long base, unsigned long size)
{
	ସଂରଚନା sbi_hart_protection *hprot = __hart_memory_protection_best();

	ଯଦି (!hprot || !hprot->temp_unmap_range)
		ଫେରନ୍ତୁ 0;

	ଫେରନ୍ତୁ hprot->temp_unmap_range(sbi_scratch_thishart_ptr(), base, size);
}
