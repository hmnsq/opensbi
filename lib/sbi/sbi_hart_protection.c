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
		जर (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY)
			परत pos;
	}

	परत रिक्त;
}

शून्य sbi_hart_protection_get_str(अक्षर *out_str, पूर्णांक out_str_size)
{
	bool memory_protect_done = false;
	संरचना sbi_hart_protection *pos;
	पूर्णांक offset = 0;

	जर (!out_str || out_str_size <= 0)
		परत;
	sbi_memset(out_str, 0, out_str_size);

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		जर (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY) {
			जर (memory_protect_done)
				सुरूठेवा;
			memory_protect_done = true;
		}
		sbi_snprintf(out_str + offset, out_str_size - offset, "%s,", pos->name);
		offset = offset + sbi_strlen(pos->name) + 1;
	}

	जर (offset)
		out_str[offset - 1] = '\0';
	अन्यथा
		sbi_strncpy(out_str, "none", out_str_size);
}

पूर्णांक sbi_hart_protection_register(संरचना sbi_hart_protection *hprot)
{
	संरचना sbi_hart_protection *pos = रिक्त;
	bool found_pos = false;

	जर (!hprot)
		परत SBI_EINVAL;
	जर (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		परत SBI_EINVAL;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		जर (hprot->rating > pos->rating) {
			found_pos = true;
			थांबा;
		}
	}

	जर (found_pos)
		sbi_list_add_tail(&hprot->head, &pos->head);
	अन्यथा
		sbi_list_add_tail(&hprot->head, &hart_protection_list);

	परत 0;
}

शून्य sbi_hart_protection_unregister(संरचना sbi_hart_protection *hprot)
{
	जर (!hprot)
		परत;

	sbi_list_del(&hprot->head);
}

स्थिर पूर्णांक __hart_protection_configure(संरचना sbi_scratch *scratch,
				       संरचना sbi_hart_protection *hprot,
				       संरचना sbi_domain *dom)
{
	जर (!hprot)
		परत 0;
	जर (!hprot->configure)
		परत SBI_ENOSYS;

	परत hprot->configure(scratch, dom);
}

स्थिर शून्य __hart_protection_unconfigure(संरचना sbi_scratch *scratch,
					  संरचना sbi_hart_protection *hprot,
					  संरचना sbi_domain *dom)
{
	जर (!hprot || !hprot->unconfigure)
		परत;

	hprot->unconfigure(scratch, dom);
}

पूर्णांक sbi_hart_protection_configure(संरचना sbi_scratch *scratch,
				  संरचना sbi_domain *dom)
{
	bool do_configure, memory_protect_done = false;
	संरचना sbi_hart_protection *hprot;
	पूर्णांक ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_configure = false;
		निवड (hprot->type) {
		स्थिती SBI_HART_PROTECTION_TYPE_MEMORY:
			do_configure = !memory_protect_done;
			memory_protect_done = true;
			थांबा;
		स्थिती SBI_HART_PROTECTION_TYPE_ID:
			do_configure = true;
			थांबा;
		पूर्वनिर्धारित:
			थांबा;
		}
		जर (!do_configure)
			सुरूठेवा;

		ret = __hart_protection_configure(scratch, hprot, dom);
		जर (ret)
			परत ret;
	}

	परत 0;
}

शून्य sbi_hart_protection_unconfigure(संरचना sbi_scratch *scratch,
				     संरचना sbi_domain *dom)
{

	bool do_unconfigure, memory_protect_done = false;
	संरचना sbi_hart_protection *hprot;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_unconfigure = false;
		निवड (hprot->type) {
		स्थिती SBI_HART_PROTECTION_TYPE_MEMORY:
			do_unconfigure = !memory_protect_done;
			memory_protect_done = true;
			थांबा;
		स्थिती SBI_HART_PROTECTION_TYPE_ID:
			do_unconfigure = true;
			थांबा;
		पूर्वनिर्धारित:
			थांबा;
		}
		जर (!do_unconfigure)
			सुरूठेवा;

		__hart_protection_unconfigure(scratch, hprot, dom);
	}
}

पूर्णांक sbi_hart_protection_reconfigure(संरचना sbi_scratch *scratch,
				    संरचना sbi_domain *current_dom,
				    संरचना sbi_domain *next_dom)
{
	bool do_reconfigure, memory_protect_done = false;
	संरचना sbi_hart_protection *hprot;
	पूर्णांक ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_reconfigure = false;
		निवड (hprot->type) {
		स्थिती SBI_HART_PROTECTION_TYPE_MEMORY:
			do_reconfigure = !memory_protect_done;
			memory_protect_done = true;
			थांबा;
		स्थिती SBI_HART_PROTECTION_TYPE_ID:
			do_reconfigure = true;
			थांबा;
		पूर्वनिर्धारित:
			थांबा;
		}
		जर (!do_reconfigure)
			सुरूठेवा;

		__hart_protection_unconfigure(scratch, hprot, current_dom);
		ret = __hart_protection_configure(scratch, hprot, next_dom);
		जर (ret)
			परत ret;
	}

	परत 0;
}

पूर्णांक sbi_hart_protection_temp_map_range(अचिन्हित दीर्घ base, अचिन्हित दीर्घ size)
{
	संरचना sbi_hart_protection *hprot = __hart_memory_protection_best();

	जर (!hprot || !hprot->temp_map_range)
		परत 0;

	परत hprot->temp_map_range(sbi_scratch_thishart_ptr(), base, size);
}

पूर्णांक sbi_hart_protection_temp_unmap_range(अचिन्हित दीर्घ base, अचिन्हित दीर्घ size)
{
	संरचना sbi_hart_protection *hprot = __hart_memory_protection_best();

	जर (!hprot || !hprot->temp_unmap_range)
		परत 0;

	परत hprot->temp_unmap_range(sbi_scratch_thishart_ptr(), base, size);
}
