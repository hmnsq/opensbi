/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_hart_protection.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi/sbi_string.h>

स्थिर SBI_LIST_HEAD(hart_protection_list);

स्थिर संरचना sbi_hart_protection *__hart_memory_protection_best(शून्य)
{
	संरचना sbi_hart_protection *pos;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		सचे (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY)
			पटिनिवत्त pos;
	}

	पटिनिवत्त रिक्त;
}

शून्य sbi_hart_protection_get_str(वर्ण *out_str, पूर्णाङ्क out_str_size)
{
	bool memory_protect_done = false;
	संरचना sbi_hart_protection *pos;
	पूर्णाङ्क offset = 0;

	सचे (!out_str || out_str_size <= 0)
		पटिनिवत्त;
	sbi_memset(out_str, 0, out_str_size);

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		सचे (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY) {
			सचे (memory_protect_done)
				अनुवर्त;
			memory_protect_done = true;
		}
		sbi_snprintf(out_str + offset, out_str_size - offset, "%s,", pos->name);
		offset = offset + sbi_strlen(pos->name) + 1;
	}

	सचे (offset)
		out_str[offset - 1] = '\0';
	अञ्ञथा
		sbi_strncpy(out_str, "none", out_str_size);
}

पूर्णाङ्क sbi_hart_protection_register(संरचना sbi_hart_protection *hprot)
{
	संरचना sbi_hart_protection *pos = रिक्त;
	bool found_pos = false;

	सचे (!hprot)
		पटिनिवत्त SBI_EINVAL;
	सचे (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		पटिनिवत्त SBI_EINVAL;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		सचे (hprot->rating > pos->rating) {
			found_pos = true;
			भङ्ग;
		}
	}

	सचे (found_pos)
		sbi_list_add_tail(&hprot->head, &pos->head);
	अञ्ञथा
		sbi_list_add_tail(&hprot->head, &hart_protection_list);

	पटिनिवत्त 0;
}

शून्य sbi_hart_protection_unregister(संरचना sbi_hart_protection *hprot)
{
	सचे (!hprot)
		पटिनिवत्त;

	sbi_list_del(&hprot->head);
}

स्थिर पूर्णाङ्क __hart_protection_configure(संरचना sbi_scratch *scratch,
				       संरचना sbi_hart_protection *hprot,
				       संरचना sbi_domain *dom)
{
	सचे (!hprot)
		पटिनिवत्त 0;
	सचे (!hprot->configure)
		पटिनिवत्त SBI_ENOSYS;

	पटिनिवत्त hprot->configure(scratch, dom);
}

स्थिर शून्य __hart_protection_unconfigure(संरचना sbi_scratch *scratch,
					  संरचना sbi_hart_protection *hprot,
					  संरचना sbi_domain *dom)
{
	सचे (!hprot || !hprot->unconfigure)
		पटिनिवत्त;

	hprot->unconfigure(scratch, dom);
}

पूर्णाङ्क sbi_hart_protection_configure(संरचना sbi_scratch *scratch,
				  संरचना sbi_domain *dom)
{
	bool do_configure, memory_protect_done = false;
	संरचना sbi_hart_protection *hprot;
	पूर्णाङ्क ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_configure = false;
		विकल्प (hprot->type) {
		पक्ष SBI_HART_PROTECTION_TYPE_MEMORY:
			do_configure = !memory_protect_done;
			memory_protect_done = true;
			भङ्ग;
		पक्ष SBI_HART_PROTECTION_TYPE_ID:
			do_configure = true;
			भङ्ग;
		स्वभाव:
			भङ्ग;
		}
		सचे (!do_configure)
			अनुवर्त;

		ret = __hart_protection_configure(scratch, hprot, dom);
		सचे (ret)
			पटिनिवत्त ret;
	}

	पटिनिवत्त 0;
}

शून्य sbi_hart_protection_unconfigure(संरचना sbi_scratch *scratch,
				     संरचना sbi_domain *dom)
{

	bool do_unconfigure, memory_protect_done = false;
	संरचना sbi_hart_protection *hprot;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_unconfigure = false;
		विकल्प (hprot->type) {
		पक्ष SBI_HART_PROTECTION_TYPE_MEMORY:
			do_unconfigure = !memory_protect_done;
			memory_protect_done = true;
			भङ्ग;
		पक्ष SBI_HART_PROTECTION_TYPE_ID:
			do_unconfigure = true;
			भङ्ग;
		स्वभाव:
			भङ्ग;
		}
		सचे (!do_unconfigure)
			अनुवर्त;

		__hart_protection_unconfigure(scratch, hprot, dom);
	}
}

पूर्णाङ्क sbi_hart_protection_reconfigure(संरचना sbi_scratch *scratch,
				    संरचना sbi_domain *current_dom,
				    संरचना sbi_domain *next_dom)
{
	bool do_reconfigure, memory_protect_done = false;
	संरचना sbi_hart_protection *hprot;
	पूर्णाङ्क ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_reconfigure = false;
		विकल्प (hprot->type) {
		पक्ष SBI_HART_PROTECTION_TYPE_MEMORY:
			do_reconfigure = !memory_protect_done;
			memory_protect_done = true;
			भङ्ग;
		पक्ष SBI_HART_PROTECTION_TYPE_ID:
			do_reconfigure = true;
			भङ्ग;
		स्वभाव:
			भङ्ग;
		}
		सचे (!do_reconfigure)
			अनुवर्त;

		__hart_protection_unconfigure(scratch, hprot, current_dom);
		ret = __hart_protection_configure(scratch, hprot, next_dom);
		सचे (ret)
			पटिनिवत्त ret;
	}

	पटिनिवत्त 0;
}

पूर्णाङ्क sbi_hart_protection_temp_map_range(unsigned long base, unsigned long size)
{
	संरचना sbi_hart_protection *hprot = __hart_memory_protection_best();

	सचे (!hprot || !hprot->temp_map_range)
		पटिनिवत्त 0;

	पटिनिवत्त hprot->temp_map_range(sbi_scratch_thishart_ptr(), base, size);
}

पूर्णाङ्क sbi_hart_protection_temp_unmap_range(unsigned long base, unsigned long size)
{
	संरचना sbi_hart_protection *hprot = __hart_memory_protection_best();

	सचे (!hprot || !hprot->temp_unmap_range)
		पटिनिवत्त 0;

	पटिनिवत्त hprot->temp_unmap_range(sbi_scratch_thishart_ptr(), base, size);
}
