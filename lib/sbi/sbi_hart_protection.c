/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_hart_protection.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi/sbi_string.h>

સ્થિર SBI_LIST_HEAD(hart_protection_list);

સ્થિર સંરચના sbi_hart_protection *__hart_memory_protection_best(શૂન્ય)
{
	સંરચના sbi_hart_protection *pos;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		જો (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY)
			પાછા pos;
	}

	પાછા ખાલી;
}

શૂન્ય sbi_hart_protection_get_str(અક્ષર *out_str, પૂર્ણાંક out_str_size)
{
	bool memory_protect_done = false;
	સંરચના sbi_hart_protection *pos;
	પૂર્ણાંક offset = 0;

	જો (!out_str || out_str_size <= 0)
		પાછા;
	sbi_memset(out_str, 0, out_str_size);

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		જો (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY) {
			જો (memory_protect_done)
				ચાલુ;
			memory_protect_done = true;
		}
		sbi_snprintf(out_str + offset, out_str_size - offset, "%s,", pos->name);
		offset = offset + sbi_strlen(pos->name) + 1;
	}

	જો (offset)
		out_str[offset - 1] = '\0';
	નહીંતર
		sbi_strncpy(out_str, "none", out_str_size);
}

પૂર્ણાંક sbi_hart_protection_register(સંરચના sbi_hart_protection *hprot)
{
	સંરચના sbi_hart_protection *pos = ખાલી;
	bool found_pos = false;

	જો (!hprot)
		પાછા SBI_EINVAL;
	જો (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		પાછા SBI_EINVAL;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		જો (hprot->rating > pos->rating) {
			found_pos = true;
			તોડો;
		}
	}

	જો (found_pos)
		sbi_list_add_tail(&hprot->head, &pos->head);
	નહીંતર
		sbi_list_add_tail(&hprot->head, &hart_protection_list);

	પાછા 0;
}

શૂન્ય sbi_hart_protection_unregister(સંરચના sbi_hart_protection *hprot)
{
	જો (!hprot)
		પાછા;

	sbi_list_del(&hprot->head);
}

સ્થિર પૂર્ણાંક __hart_protection_configure(સંરચના sbi_scratch *scratch,
				       સંરચના sbi_hart_protection *hprot,
				       સંરચના sbi_domain *dom)
{
	જો (!hprot)
		પાછા 0;
	જો (!hprot->configure)
		પાછા SBI_ENOSYS;

	પાછા hprot->configure(scratch, dom);
}

સ્થિર શૂન્ય __hart_protection_unconfigure(સંરચના sbi_scratch *scratch,
					  સંરચના sbi_hart_protection *hprot,
					  સંરચના sbi_domain *dom)
{
	જો (!hprot || !hprot->unconfigure)
		પાછા;

	hprot->unconfigure(scratch, dom);
}

પૂર્ણાંક sbi_hart_protection_configure(સંરચના sbi_scratch *scratch,
				  સંરચના sbi_domain *dom)
{
	bool do_configure, memory_protect_done = false;
	સંરચના sbi_hart_protection *hprot;
	પૂર્ણાંક ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_configure = false;
		પસંદગી (hprot->type) {
		સ્થિતિ SBI_HART_PROTECTION_TYPE_MEMORY:
			do_configure = !memory_protect_done;
			memory_protect_done = true;
			તોડો;
		સ્થિતિ SBI_HART_PROTECTION_TYPE_ID:
			do_configure = true;
			તોડો;
		મૂળભૂત:
			તોડો;
		}
		જો (!do_configure)
			ચાલુ;

		ret = __hart_protection_configure(scratch, hprot, dom);
		જો (ret)
			પાછા ret;
	}

	પાછા 0;
}

શૂન્ય sbi_hart_protection_unconfigure(સંરચના sbi_scratch *scratch,
				     સંરચના sbi_domain *dom)
{

	bool do_unconfigure, memory_protect_done = false;
	સંરચના sbi_hart_protection *hprot;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_unconfigure = false;
		પસંદગી (hprot->type) {
		સ્થિતિ SBI_HART_PROTECTION_TYPE_MEMORY:
			do_unconfigure = !memory_protect_done;
			memory_protect_done = true;
			તોડો;
		સ્થિતિ SBI_HART_PROTECTION_TYPE_ID:
			do_unconfigure = true;
			તોડો;
		મૂળભૂત:
			તોડો;
		}
		જો (!do_unconfigure)
			ચાલુ;

		__hart_protection_unconfigure(scratch, hprot, dom);
	}
}

પૂર્ણાંક sbi_hart_protection_reconfigure(સંરચના sbi_scratch *scratch,
				    સંરચના sbi_domain *current_dom,
				    સંરચના sbi_domain *next_dom)
{
	bool do_reconfigure, memory_protect_done = false;
	સંરચના sbi_hart_protection *hprot;
	પૂર્ણાંક ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_reconfigure = false;
		પસંદગી (hprot->type) {
		સ્થિતિ SBI_HART_PROTECTION_TYPE_MEMORY:
			do_reconfigure = !memory_protect_done;
			memory_protect_done = true;
			તોડો;
		સ્થિતિ SBI_HART_PROTECTION_TYPE_ID:
			do_reconfigure = true;
			તોડો;
		મૂળભૂત:
			તોડો;
		}
		જો (!do_reconfigure)
			ચાલુ;

		__hart_protection_unconfigure(scratch, hprot, current_dom);
		ret = __hart_protection_configure(scratch, hprot, next_dom);
		જો (ret)
			પાછા ret;
	}

	પાછા 0;
}

પૂર્ણાંક sbi_hart_protection_temp_map_range(unsigned long base, unsigned long size)
{
	સંરચના sbi_hart_protection *hprot = __hart_memory_protection_best();

	જો (!hprot || !hprot->temp_map_range)
		પાછા 0;

	પાછા hprot->temp_map_range(sbi_scratch_thishart_ptr(), base, size);
}

પૂર્ણાંક sbi_hart_protection_temp_unmap_range(unsigned long base, unsigned long size)
{
	સંરચના sbi_hart_protection *hprot = __hart_memory_protection_best();

	જો (!hprot || !hprot->temp_unmap_range)
		પાછા 0;

	પાછા hprot->temp_unmap_range(sbi_scratch_thishart_ptr(), base, size);
}
