/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_hart_protection.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi/sbi_string.h>

अटल SBI_LIST_शीर्ष(hart_protection_list);

अटल काष्ठा sbi_hart_protection *__hart_memory_protection_best(व्योम)
{
	काष्ठा sbi_hart_protection *pos;

	sbi_list_for_each_entry(pos, &hart_protection_list, शीर्ष) {
		अगर (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY)
			वापस pos;
	}

	वापस शून्य;
}

व्योम sbi_hart_protection_get_str(अक्षर *out_str, पूर्णांक out_str_आकार)
{
	bool memory_protect_done = false;
	काष्ठा sbi_hart_protection *pos;
	पूर्णांक offset = 0;

	अगर (!out_str || out_str_आकार <= 0)
		वापस;
	sbi_memset(out_str, 0, out_str_आकार);

	sbi_list_for_each_entry(pos, &hart_protection_list, शीर्ष) {
		अगर (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY) {
			अगर (memory_protect_done)
				जारी;
			memory_protect_done = true;
		}
		sbi_snprintf(out_str + offset, out_str_आकार - offset, "%s,", pos->name);
		offset = offset + sbi_strlen(pos->name) + 1;
	}

	अगर (offset)
		out_str[offset - 1] = '\0';
	अन्यथा
		sbi_strncpy(out_str, "none", out_str_आकार);
}

पूर्णांक sbi_hart_protection_register(काष्ठा sbi_hart_protection *hprot)
{
	काष्ठा sbi_hart_protection *pos = शून्य;
	bool found_pos = false;

	अगर (!hprot)
		वापस SBI_EINVAL;
	अगर (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		वापस SBI_EINVAL;

	sbi_list_for_each_entry(pos, &hart_protection_list, शीर्ष) {
		अगर (hprot->rating > pos->rating) {
			found_pos = true;
			अवरोध;
		}
	}

	अगर (found_pos)
		sbi_list_add_पुच्छ(&hprot->शीर्ष, &pos->शीर्ष);
	अन्यथा
		sbi_list_add_पुच्छ(&hprot->शीर्ष, &hart_protection_list);

	वापस 0;
}

व्योम sbi_hart_protection_unregister(काष्ठा sbi_hart_protection *hprot)
{
	अगर (!hprot)
		वापस;

	sbi_list_del(&hprot->शीर्ष);
}

अटल पूर्णांक __hart_protection_configure(काष्ठा sbi_scratch *scratch,
				       काष्ठा sbi_hart_protection *hprot,
				       काष्ठा sbi_domain *dom)
{
	अगर (!hprot)
		वापस 0;
	अगर (!hprot->configure)
		वापस SBI_ENOSYS;

	वापस hprot->configure(scratch, dom);
}

अटल व्योम __hart_protection_unconfigure(काष्ठा sbi_scratch *scratch,
					  काष्ठा sbi_hart_protection *hprot,
					  काष्ठा sbi_domain *dom)
{
	अगर (!hprot || !hprot->unconfigure)
		वापस;

	hprot->unconfigure(scratch, dom);
}

पूर्णांक sbi_hart_protection_configure(काष्ठा sbi_scratch *scratch,
				  काष्ठा sbi_domain *dom)
{
	bool do_configure, memory_protect_done = false;
	काष्ठा sbi_hart_protection *hprot;
	पूर्णांक ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, शीर्ष) {
		do_configure = false;
		चयन (hprot->type) {
		हाल SBI_HART_PROTECTION_TYPE_MEMORY:
			do_configure = !memory_protect_done;
			memory_protect_done = true;
			अवरोध;
		हाल SBI_HART_PROTECTION_TYPE_ID:
			do_configure = true;
			अवरोध;
		शेष:
			अवरोध;
		}
		अगर (!do_configure)
			जारी;

		ret = __hart_protection_configure(scratch, hprot, dom);
		अगर (ret)
			वापस ret;
	}

	वापस 0;
}

व्योम sbi_hart_protection_unconfigure(काष्ठा sbi_scratch *scratch,
				     काष्ठा sbi_domain *dom)
{

	bool do_unconfigure, memory_protect_done = false;
	काष्ठा sbi_hart_protection *hprot;

	sbi_list_for_each_entry(hprot, &hart_protection_list, शीर्ष) {
		do_unconfigure = false;
		चयन (hprot->type) {
		हाल SBI_HART_PROTECTION_TYPE_MEMORY:
			do_unconfigure = !memory_protect_done;
			memory_protect_done = true;
			अवरोध;
		हाल SBI_HART_PROTECTION_TYPE_ID:
			do_unconfigure = true;
			अवरोध;
		शेष:
			अवरोध;
		}
		अगर (!do_unconfigure)
			जारी;

		__hart_protection_unconfigure(scratch, hprot, dom);
	}
}

पूर्णांक sbi_hart_protection_reconfigure(काष्ठा sbi_scratch *scratch,
				    काष्ठा sbi_domain *current_dom,
				    काष्ठा sbi_domain *अग्र_dom)
{
	bool do_reconfigure, memory_protect_done = false;
	काष्ठा sbi_hart_protection *hprot;
	पूर्णांक ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, शीर्ष) {
		do_reconfigure = false;
		चयन (hprot->type) {
		हाल SBI_HART_PROTECTION_TYPE_MEMORY:
			do_reconfigure = !memory_protect_done;
			memory_protect_done = true;
			अवरोध;
		हाल SBI_HART_PROTECTION_TYPE_ID:
			do_reconfigure = true;
			अवरोध;
		शेष:
			अवरोध;
		}
		अगर (!do_reconfigure)
			जारी;

		__hart_protection_unconfigure(scratch, hprot, current_dom);
		ret = __hart_protection_configure(scratch, hprot, अग्र_dom);
		अगर (ret)
			वापस ret;
	}

	वापस 0;
}

पूर्णांक sbi_hart_protection_अस्थायी_map_range(अचिन्हित दीर्घ base, अचिन्हित दीर्घ आकार)
{
	काष्ठा sbi_hart_protection *hprot = __hart_memory_protection_best();

	अगर (!hprot || !hprot->अस्थायी_map_range)
		वापस 0;

	वापस hprot->अस्थायी_map_range(sbi_scratch_thishart_ptr(), base, आकार);
}

पूर्णांक sbi_hart_protection_अस्थायी_unmap_range(अचिन्हित दीर्घ base, अचिन्हित दीर्घ आकार)
{
	काष्ठा sbi_hart_protection *hprot = __hart_memory_protection_best();

	अगर (!hprot || !hprot->अस्थायी_unmap_range)
		वापस 0;

	वापस hprot->अस्थायी_unmap_range(sbi_scratch_thishart_ptr(), base, आकार);
}
