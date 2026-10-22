/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_hart_protection.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi/sbi_string.h>

நிலையான SBI_LIST_HEAD(hart_protection_list);

நிலையான கட்டமைப்பு sbi_hart_protection *__hart_memory_protection_best(வெறுமை)
{
	கட்டமைப்பு sbi_hart_protection *pos;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		எனில் (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY)
			திரும்பு pos;
	}

	திரும்பு வெற்று;
}

வெறுமை sbi_hart_protection_get_str(எழுத்து *out_str, முழு out_str_size)
{
	bool memory_protect_done = false;
	கட்டமைப்பு sbi_hart_protection *pos;
	முழு offset = 0;

	எனில் (!out_str || out_str_size <= 0)
		திரும்பு;
	sbi_memset(out_str, 0, out_str_size);

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		எனில் (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY) {
			எனில் (memory_protect_done)
				தொடர்;
			memory_protect_done = true;
		}
		sbi_snprintf(out_str + offset, out_str_size - offset, "%s,", pos->name);
		offset = offset + sbi_strlen(pos->name) + 1;
	}

	எனில் (offset)
		out_str[offset - 1] = '\0';
	இல்லையெனில்
		sbi_strncpy(out_str, "none", out_str_size);
}

முழு sbi_hart_protection_register(கட்டமைப்பு sbi_hart_protection *hprot)
{
	கட்டமைப்பு sbi_hart_protection *pos = வெற்று;
	bool found_pos = false;

	எனில் (!hprot)
		திரும்பு SBI_EINVAL;
	எனில் (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		திரும்பு SBI_EINVAL;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		எனில் (hprot->rating > pos->rating) {
			found_pos = true;
			நிறுத்து;
		}
	}

	எனில் (found_pos)
		sbi_list_add_tail(&hprot->head, &pos->head);
	இல்லையெனில்
		sbi_list_add_tail(&hprot->head, &hart_protection_list);

	திரும்பு 0;
}

வெறுமை sbi_hart_protection_unregister(கட்டமைப்பு sbi_hart_protection *hprot)
{
	எனில் (!hprot)
		திரும்பு;

	sbi_list_del(&hprot->head);
}

நிலையான முழு __hart_protection_configure(கட்டமைப்பு sbi_scratch *scratch,
				       கட்டமைப்பு sbi_hart_protection *hprot,
				       கட்டமைப்பு sbi_domain *dom)
{
	எனில் (!hprot)
		திரும்பு 0;
	எனில் (!hprot->configure)
		திரும்பு SBI_ENOSYS;

	திரும்பு hprot->configure(scratch, dom);
}

நிலையான வெறுமை __hart_protection_unconfigure(கட்டமைப்பு sbi_scratch *scratch,
					  கட்டமைப்பு sbi_hart_protection *hprot,
					  கட்டமைப்பு sbi_domain *dom)
{
	எனில் (!hprot || !hprot->unconfigure)
		திரும்பு;

	hprot->unconfigure(scratch, dom);
}

முழு sbi_hart_protection_configure(கட்டமைப்பு sbi_scratch *scratch,
				  கட்டமைப்பு sbi_domain *dom)
{
	bool do_configure, memory_protect_done = false;
	கட்டமைப்பு sbi_hart_protection *hprot;
	முழு ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_configure = false;
		தேர்வு (hprot->type) {
		நிலை SBI_HART_PROTECTION_TYPE_MEMORY:
			do_configure = !memory_protect_done;
			memory_protect_done = true;
			நிறுத்து;
		நிலை SBI_HART_PROTECTION_TYPE_ID:
			do_configure = true;
			நிறுத்து;
		இயல்பு:
			நிறுத்து;
		}
		எனில் (!do_configure)
			தொடர்;

		ret = __hart_protection_configure(scratch, hprot, dom);
		எனில் (ret)
			திரும்பு ret;
	}

	திரும்பு 0;
}

வெறுமை sbi_hart_protection_unconfigure(கட்டமைப்பு sbi_scratch *scratch,
				     கட்டமைப்பு sbi_domain *dom)
{

	bool do_unconfigure, memory_protect_done = false;
	கட்டமைப்பு sbi_hart_protection *hprot;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_unconfigure = false;
		தேர்வு (hprot->type) {
		நிலை SBI_HART_PROTECTION_TYPE_MEMORY:
			do_unconfigure = !memory_protect_done;
			memory_protect_done = true;
			நிறுத்து;
		நிலை SBI_HART_PROTECTION_TYPE_ID:
			do_unconfigure = true;
			நிறுத்து;
		இயல்பு:
			நிறுத்து;
		}
		எனில் (!do_unconfigure)
			தொடர்;

		__hart_protection_unconfigure(scratch, hprot, dom);
	}
}

முழு sbi_hart_protection_reconfigure(கட்டமைப்பு sbi_scratch *scratch,
				    கட்டமைப்பு sbi_domain *current_dom,
				    கட்டமைப்பு sbi_domain *next_dom)
{
	bool do_reconfigure, memory_protect_done = false;
	கட்டமைப்பு sbi_hart_protection *hprot;
	முழு ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_reconfigure = false;
		தேர்வு (hprot->type) {
		நிலை SBI_HART_PROTECTION_TYPE_MEMORY:
			do_reconfigure = !memory_protect_done;
			memory_protect_done = true;
			நிறுத்து;
		நிலை SBI_HART_PROTECTION_TYPE_ID:
			do_reconfigure = true;
			நிறுத்து;
		இயல்பு:
			நிறுத்து;
		}
		எனில் (!do_reconfigure)
			தொடர்;

		__hart_protection_unconfigure(scratch, hprot, current_dom);
		ret = __hart_protection_configure(scratch, hprot, next_dom);
		எனில் (ret)
			திரும்பு ret;
	}

	திரும்பு 0;
}

முழு sbi_hart_protection_temp_map_range(unsigned long base, unsigned long size)
{
	கட்டமைப்பு sbi_hart_protection *hprot = __hart_memory_protection_best();

	எனில் (!hprot || !hprot->temp_map_range)
		திரும்பு 0;

	திரும்பு hprot->temp_map_range(sbi_scratch_thishart_ptr(), base, size);
}

முழு sbi_hart_protection_temp_unmap_range(unsigned long base, unsigned long size)
{
	கட்டமைப்பு sbi_hart_protection *hprot = __hart_memory_protection_best();

	எனில் (!hprot || !hprot->temp_unmap_range)
		திரும்பு 0;

	திரும்பு hprot->temp_unmap_range(sbi_scratch_thishart_ptr(), base, size);
}
