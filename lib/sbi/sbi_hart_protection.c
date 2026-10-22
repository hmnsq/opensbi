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
		यदि (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY)
			प्रत्यागम pos;
	}

	प्रत्यागम रिक्त;
}

शून्य sbi_hart_protection_get_str(वर्ण *out_str, पूर्णाङ्क out_str_size)
{
	bool memory_protect_done = false;
	संरचना sbi_hart_protection *pos;
	पूर्णाङ्क offset = 0;

	यदि (!out_str || out_str_size <= 0)
		प्रत्यागम;
	sbi_memset(out_str, 0, out_str_size);

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		यदि (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY) {
			यदि (memory_protect_done)
				अनुवर्त;
			memory_protect_done = true;
		}
		sbi_snprintf(out_str + offset, out_str_size - offset, "%s,", pos->name);
		offset = offset + sbi_strlen(pos->name) + 1;
	}

	यदि (offset)
		out_str[offset - 1] = '\0';
	अन्यथा
		sbi_strncpy(out_str, "none", out_str_size);
}

पूर्णाङ्क sbi_hart_protection_register(संरचना sbi_hart_protection *hprot)
{
	संरचना sbi_hart_protection *pos = रिक्त;
	bool found_pos = false;

	यदि (!hprot)
		प्रत्यागम SBI_EINVAL;
	यदि (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		प्रत्यागम SBI_EINVAL;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		यदि (hprot->rating > pos->rating) {
			found_pos = true;
			भङ्ग;
		}
	}

	यदि (found_pos)
		sbi_list_add_tail(&hprot->head, &pos->head);
	अन्यथा
		sbi_list_add_tail(&hprot->head, &hart_protection_list);

	प्रत्यागम 0;
}

शून्य sbi_hart_protection_unregister(संरचना sbi_hart_protection *hprot)
{
	यदि (!hprot)
		प्रत्यागम;

	sbi_list_del(&hprot->head);
}

स्थिर पूर्णाङ्क __hart_protection_configure(संरचना sbi_scratch *scratch,
				       संरचना sbi_hart_protection *hprot,
				       संरचना sbi_domain *dom)
{
	यदि (!hprot)
		प्रत्यागम 0;
	यदि (!hprot->configure)
		प्रत्यागम SBI_ENOSYS;

	प्रत्यागम hprot->configure(scratch, dom);
}

स्थिर शून्य __hart_protection_unconfigure(संरचना sbi_scratch *scratch,
					  संरचना sbi_hart_protection *hprot,
					  संरचना sbi_domain *dom)
{
	यदि (!hprot || !hprot->unconfigure)
		प्रत्यागम;

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
		यदि (!do_configure)
			अनुवर्त;

		ret = __hart_protection_configure(scratch, hprot, dom);
		यदि (ret)
			प्रत्यागम ret;
	}

	प्रत्यागम 0;
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
		यदि (!do_unconfigure)
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
		यदि (!do_reconfigure)
			अनुवर्त;

		__hart_protection_unconfigure(scratch, hprot, current_dom);
		ret = __hart_protection_configure(scratch, hprot, next_dom);
		यदि (ret)
			प्रत्यागम ret;
	}

	प्रत्यागम 0;
}

पूर्णाङ्क sbi_hart_protection_temp_map_range(अचिन्हित दीर्घ base, अचिन्हित दीर्घ size)
{
	संरचना sbi_hart_protection *hprot = __hart_memory_protection_best();

	यदि (!hprot || !hprot->temp_map_range)
		प्रत्यागम 0;

	प्रत्यागम hprot->temp_map_range(sbi_scratch_thishart_ptr(), base, size);
}

पूर्णाङ्क sbi_hart_protection_temp_unmap_range(अचिन्हित दीर्घ base, अचिन्हित दीर्घ size)
{
	संरचना sbi_hart_protection *hprot = __hart_memory_protection_best();

	यदि (!hprot || !hprot->temp_unmap_range)
		प्रत्यागम 0;

	प्रत्यागम hprot->temp_unmap_range(sbi_scratch_thishart_ptr(), base, size);
}
