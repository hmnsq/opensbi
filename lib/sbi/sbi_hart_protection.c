/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart_protection.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>

ಸ್ಥಿರ SBI_LIST_ತಲೆ(hart_protection_list);

ಸ್ಥಿರ ರಚನೆ sbi_hart_protection *__hart_memory_protection_best(ಶೂನ್ಯ)
{
	ರಚನೆ sbi_hart_protection *pos;

	sbi_list_for_each_ನಮೂದು(pos, &hart_protection_list, ತಲೆ) {
		ಆದರೆ (pos->type == SBI_HART_PROTECTION_TYPE_ಸ್ಮರಣೆ)
			ಹಿಂತಿರುಗಿ pos;
	}

	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಶೂನ್ಯ sbi_hart_protection_get_str(ಅಕ್ಷರ *ಹೊರಹೋಗು_str, ಪೂರ್ಣಾಂಕ ಹೊರಹೋಗು_str_ಗಾತ್ರ)
{
	bool ಸ್ಮರಣೆ_protect_ಮುಗಿದ = false;
	ರಚನೆ sbi_hart_protection *pos;
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = 0;

	ಆದರೆ (!ಹೊರಹೋಗು_str || ಹೊರಹೋಗು_str_ಗಾತ್ರ <= 0)
		ಹಿಂತಿರುಗಿ;
	sbi_memset(ಹೊರಹೋಗು_str, 0, ಹೊರಹೋಗು_str_ಗಾತ್ರ);

	sbi_list_for_each_ನಮೂದು(pos, &hart_protection_list, ತಲೆ) {
		ಆದರೆ (pos->type == SBI_HART_PROTECTION_TYPE_ಸ್ಮರಣೆ) {
			ಆದರೆ (ಸ್ಮರಣೆ_protect_ಮುಗಿದ)
				ಮುಂದುವರಿ;
			ಸ್ಮರಣೆ_protect_ಮುಗಿದ = true;
		}
		sbi_snprintf(ಹೊರಹೋಗು_str + ಸ್ಥಾನಾಂತರ, ಹೊರಹೋಗು_str_ಗಾತ್ರ - ಸ್ಥಾನಾಂತರ, "%s,", pos->name);
		ಸ್ಥಾನಾಂತರ = ಸ್ಥಾನಾಂತರ + sbi_strlen(pos->name) + 1;
	}

	ಆದರೆ (ಸ್ಥಾನಾಂತರ)
		ಹೊರಹೋಗು_str[ಸ್ಥಾನಾಂತರ - 1] = '\0';
	ಇಲ್ಲದಿದ್ದರೆ
		sbi_strncpy(ಹೊರಹೋಗು_str, "none", ಹೊರಹೋಗು_str_ಗಾತ್ರ);
}

ಪೂರ್ಣಾಂಕ sbi_hart_protection_ರಿಜಿಸ್ಟರ್(ರಚನೆ sbi_hart_protection *hprot)
{
	ರಚನೆ sbi_hart_protection *pos = ಖಾಲಿ;
	bool ಸಿಕ್ಕಿತು_pos = false;

	ಆದರೆ (!hprot)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಆದರೆ (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	sbi_list_for_each_ನಮೂದು(pos, &hart_protection_list, ತಲೆ) {
		ಆದರೆ (hprot->rating > pos->rating) {
			ಸಿಕ್ಕಿತು_pos = true;
			ನಿಲ್ಲಿಸು;
		}
	}

	ಆದರೆ (ಸಿಕ್ಕಿತು_pos)
		sbi_list_ಕೂಡಿಸು_ಬಾಲ(&hprot->ತಲೆ, &pos->ತಲೆ);
	ಇಲ್ಲದಿದ್ದರೆ
		sbi_list_ಕೂಡಿಸು_ಬಾಲ(&hprot->ತಲೆ, &hart_protection_list);

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ sbi_hart_protection_unregister(ರಚನೆ sbi_hart_protection *hprot)
{
	ಆದರೆ (!hprot)
		ಹಿಂತಿರುಗಿ;

	sbi_list_del(&hprot->ತಲೆ);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ __hart_protection_configure(ರಚನೆ sbi_scratch *scratch,
				       ರಚನೆ sbi_hart_protection *hprot,
				       ರಚನೆ sbi_domain *dom)
{
	ಆದರೆ (!hprot)
		ಹಿಂತಿರುಗಿ 0;
	ಆದರೆ (!hprot->configure)
		ಹಿಂತಿರುಗಿ SBI_ENOSYS;

	ಹಿಂತಿರುಗಿ hprot->configure(scratch, dom);
}

ಸ್ಥಿರ ಶೂನ್ಯ __hart_protection_unconfigure(ರಚನೆ sbi_scratch *scratch,
					  ರಚನೆ sbi_hart_protection *hprot,
					  ರಚನೆ sbi_domain *dom)
{
	ಆದರೆ (!hprot || !hprot->unconfigure)
		ಹಿಂತಿರುಗಿ;

	hprot->unconfigure(scratch, dom);
}

ಪೂರ್ಣಾಂಕ sbi_hart_protection_configure(ರಚನೆ sbi_scratch *scratch,
				  ರಚನೆ sbi_domain *dom)
{
	bool do_configure, ಸ್ಮರಣೆ_protect_ಮುಗಿದ = false;
	ರಚನೆ sbi_hart_protection *hprot;
	ಪೂರ್ಣಾಂಕ ret;

	sbi_list_for_each_ನಮೂದು(hprot, &hart_protection_list, ತಲೆ) {
		do_configure = false;
		ಆಯ್ಕೆ (hprot->type) {
		ಸ್ಥಿತಿ SBI_HART_PROTECTION_TYPE_ಸ್ಮರಣೆ:
			do_configure = !ಸ್ಮರಣೆ_protect_ಮುಗಿದ;
			ಸ್ಮರಣೆ_protect_ಮುಗಿದ = true;
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ SBI_HART_PROTECTION_TYPE_ID:
			do_configure = true;
			ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
			ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (!do_configure)
			ಮುಂದುವರಿ;

		ret = __hart_protection_configure(scratch, hprot, dom);
		ಆದರೆ (ret)
			ಹಿಂತಿರುಗಿ ret;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ sbi_hart_protection_unconfigure(ರಚನೆ sbi_scratch *scratch,
				     ರಚನೆ sbi_domain *dom)
{

	bool do_unconfigure, ಸ್ಮರಣೆ_protect_ಮುಗಿದ = false;
	ರಚನೆ sbi_hart_protection *hprot;

	sbi_list_for_each_ನಮೂದು(hprot, &hart_protection_list, ತಲೆ) {
		do_unconfigure = false;
		ಆಯ್ಕೆ (hprot->type) {
		ಸ್ಥಿತಿ SBI_HART_PROTECTION_TYPE_ಸ್ಮರಣೆ:
			do_unconfigure = !ಸ್ಮರಣೆ_protect_ಮುಗಿದ;
			ಸ್ಮರಣೆ_protect_ಮುಗಿದ = true;
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ SBI_HART_PROTECTION_TYPE_ID:
			do_unconfigure = true;
			ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
			ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (!do_unconfigure)
			ಮುಂದುವರಿ;

		__hart_protection_unconfigure(scratch, hprot, dom);
	}
}

ಪೂರ್ಣಾಂಕ sbi_hart_protection_reconfigure(ರಚನೆ sbi_scratch *scratch,
				    ರಚನೆ sbi_domain *current_dom,
				    ರಚನೆ sbi_domain *ಮುಂದಿನ_dom)
{
	bool do_reconfigure, ಸ್ಮರಣೆ_protect_ಮುಗಿದ = false;
	ರಚನೆ sbi_hart_protection *hprot;
	ಪೂರ್ಣಾಂಕ ret;

	sbi_list_for_each_ನಮೂದು(hprot, &hart_protection_list, ತಲೆ) {
		do_reconfigure = false;
		ಆಯ್ಕೆ (hprot->type) {
		ಸ್ಥಿತಿ SBI_HART_PROTECTION_TYPE_ಸ್ಮರಣೆ:
			do_reconfigure = !ಸ್ಮರಣೆ_protect_ಮುಗಿದ;
			ಸ್ಮರಣೆ_protect_ಮುಗಿದ = true;
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ SBI_HART_PROTECTION_TYPE_ID:
			do_reconfigure = true;
			ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
			ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (!do_reconfigure)
			ಮುಂದುವರಿ;

		__hart_protection_unconfigure(scratch, hprot, current_dom);
		ret = __hart_protection_configure(scratch, hprot, ಮುಂದಿನ_dom);
		ಆದರೆ (ret)
			ಹಿಂತಿರುಗಿ ret;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_map_range(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ತಳ_2, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಗಾತ್ರ_2)
{
	ರಚನೆ sbi_hart_protection *hprot = __hart_memory_protection_best();

	ಆದರೆ (!hprot || !hprot->ತಾತ್ಕಾಲಿಕ_map_range)
		ಹಿಂತಿರುಗಿ 0;

	ಹಿಂತಿರುಗಿ hprot->ತಾತ್ಕಾಲಿಕ_map_range(sbi_scratch_thishart_ptr(), ತಳ_2, ಗಾತ್ರ_2);
}

ಪೂರ್ಣಾಂಕ sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_unmap_range(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ತಳ_2, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಗಾತ್ರ_2)
{
	ರಚನೆ sbi_hart_protection *hprot = __hart_memory_protection_best();

	ಆದರೆ (!hprot || !hprot->ತಾತ್ಕಾಲಿಕ_unmap_range)
		ಹಿಂತಿರುಗಿ 0;

	ಹಿಂತಿರುಗಿ hprot->ತಾತ್ಕಾಲಿಕ_unmap_range(sbi_scratch_thishart_ptr(), ತಳ_2, ಗಾತ್ರ_2);
}
