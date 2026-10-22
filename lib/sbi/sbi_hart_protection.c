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

स्थिर SBI_LIST_शीर्ष(hart_protection_list);

स्थिर संरचना sbi_hart_protection *__hart_memory_protection_best(शून्य)
{
	संरचना sbi_hart_protection *pos;

	sbi_list_for_each_प्रविष्टिः(pos, &hart_protection_list, शीर्ष) {
		यदि (pos->type == SBI_HART_PROTECTION_TYPE_स्मृतिः)
			प्रत्यागम pos;
	}

	प्रत्यागम रिक्त;
}

शून्य sbi_hart_protection_get_str(वर्ण *बहिर्गमनम्_str, पूर्णाङ्क बहिर्गमनम्_str_आकार)
{
	bool स्मृतिः_protect_समाप्तम् = false;
	संरचना sbi_hart_protection *pos;
	पूर्णाङ्क विस्थापनम् = 0;

	यदि (!बहिर्गमनम्_str || बहिर्गमनम्_str_आकार <= 0)
		प्रत्यागम;
	sbi_memset(बहिर्गमनम्_str, 0, बहिर्गमनम्_str_आकार);

	sbi_list_for_each_प्रविष्टिः(pos, &hart_protection_list, शीर्ष) {
		यदि (pos->type == SBI_HART_PROTECTION_TYPE_स्मृतिः) {
			यदि (स्मृतिः_protect_समाप्तम्)
				अनुवर्त;
			स्मृतिः_protect_समाप्तम् = true;
		}
		sbi_snprintf(बहिर्गमनम्_str + विस्थापनम्, बहिर्गमनम्_str_आकार - विस्थापनम्, "%s,", pos->name);
		विस्थापनम् = विस्थापनम् + sbi_strlen(pos->name) + 1;
	}

	यदि (विस्थापनम्)
		बहिर्गमनम्_str[विस्थापनम् - 1] = '\0';
	अन्यथा
		sbi_strncpy(बहिर्गमनम्_str, "none", बहिर्गमनम्_str_आकार);
}

पूर्णाङ्क sbi_hart_protection_पञ्जिकाकोशः(संरचना sbi_hart_protection *hprot)
{
	संरचना sbi_hart_protection *pos = रिक्त;
	bool प्राप्तम्_pos = false;

	यदि (!hprot)
		प्रत्यागम SBI_EINVAL;
	यदि (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		प्रत्यागम SBI_EINVAL;

	sbi_list_for_each_प्रविष्टिः(pos, &hart_protection_list, शीर्ष) {
		यदि (hprot->rating > pos->rating) {
			प्राप्तम्_pos = true;
			भङ्ग;
		}
	}

	यदि (प्राप्तम्_pos)
		sbi_list_सङ्कलय_पुच्छ(&hprot->शीर्ष, &pos->शीर्ष);
	अन्यथा
		sbi_list_सङ्कलय_पुच्छ(&hprot->शीर्ष, &hart_protection_list);

	प्रत्यागम 0;
}

शून्य sbi_hart_protection_unregister(संरचना sbi_hart_protection *hprot)
{
	यदि (!hprot)
		प्रत्यागम;

	sbi_list_del(&hprot->शीर्ष);
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
	bool do_configure, स्मृतिः_protect_समाप्तम् = false;
	संरचना sbi_hart_protection *hprot;
	पूर्णाङ्क ret;

	sbi_list_for_each_प्रविष्टिः(hprot, &hart_protection_list, शीर्ष) {
		do_configure = false;
		विकल्प (hprot->type) {
		पक्ष SBI_HART_PROTECTION_TYPE_स्मृतिः:
			do_configure = !स्मृतिः_protect_समाप्तम्;
			स्मृतिः_protect_समाप्तम् = true;
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

	bool do_unconfigure, स्मृतिः_protect_समाप्तम् = false;
	संरचना sbi_hart_protection *hprot;

	sbi_list_for_each_प्रविष्टिः(hprot, &hart_protection_list, शीर्ष) {
		do_unconfigure = false;
		विकल्प (hprot->type) {
		पक्ष SBI_HART_PROTECTION_TYPE_स्मृतिः:
			do_unconfigure = !स्मृतिः_protect_समाप्तम्;
			स्मृतिः_protect_समाप्तम् = true;
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
				    संरचना sbi_domain *अग्र_dom)
{
	bool do_reconfigure, स्मृतिः_protect_समाप्तम् = false;
	संरचना sbi_hart_protection *hprot;
	पूर्णाङ्क ret;

	sbi_list_for_each_प्रविष्टिः(hprot, &hart_protection_list, शीर्ष) {
		do_reconfigure = false;
		विकल्प (hprot->type) {
		पक्ष SBI_HART_PROTECTION_TYPE_स्मृतिः:
			do_reconfigure = !स्मृतिः_protect_समाप्तम्;
			स्मृतिः_protect_समाप्तम् = true;
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
		ret = __hart_protection_configure(scratch, hprot, अग्र_dom);
		यदि (ret)
			प्रत्यागम ret;
	}

	प्रत्यागम 0;
}

पूर्णाङ्क sbi_hart_protection_अस्थायी_map_range(अचिह्नित दीर्घ आधारः_2, अचिह्नित दीर्घ आकार)
{
	संरचना sbi_hart_protection *hprot = __hart_memory_protection_best();

	यदि (!hprot || !hprot->अस्थायी_map_range)
		प्रत्यागम 0;

	प्रत्यागम hprot->अस्थायी_map_range(sbi_scratch_thishart_ptr(), आधारः_2, आकार);
}

पूर्णाङ्क sbi_hart_protection_अस्थायी_unmap_range(अचिह्नित दीर्घ आधारः_2, अचिह्नित दीर्घ आकार)
{
	संरचना sbi_hart_protection *hprot = __hart_memory_protection_best();

	यदि (!hprot || !hprot->अस्थायी_unmap_range)
		प्रत्यागम 0;

	प्रत्यागम hprot->अस्थायी_unmap_range(sbi_scratch_thishart_ptr(), आधारः_2, आकार);
}
