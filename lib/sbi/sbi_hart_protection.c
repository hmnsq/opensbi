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

ಸ್ಥಿರ SBI_LIST_HEAD(hart_protection_list);

ಸ್ಥಿರ ಸಂರಚನಾ sbi_hart_protection *__hart_memory_protection_best(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ sbi_hart_protection *pos;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		ಯದಿ (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY)
			ಪ್ರತ್ಯಾಗಮ pos;
	}

	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಶೂನ್ಯ sbi_hart_protection_get_str(ವರ್ಣ *out_str, ಪೂರ್ಣಾಙ್ಕ out_str_size)
{
	bool memory_protect_done = false;
	ಸಂರಚನಾ sbi_hart_protection *pos;
	ಪೂರ್ಣಾಙ್ಕ offset = 0;

	ಯದಿ (!out_str || out_str_size <= 0)
		ಪ್ರತ್ಯಾಗಮ;
	sbi_memset(out_str, 0, out_str_size);

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		ಯದಿ (pos->type == SBI_HART_PROTECTION_TYPE_MEMORY) {
			ಯದಿ (memory_protect_done)
				ಅನುವರ್ತ;
			memory_protect_done = true;
		}
		sbi_snprintf(out_str + offset, out_str_size - offset, "%s,", pos->name);
		offset = offset + sbi_strlen(pos->name) + 1;
	}

	ಯದಿ (offset)
		out_str[offset - 1] = '\0';
	ಅನ್ಯಥಾ
		sbi_strncpy(out_str, "none", out_str_size);
}

ಪೂರ್ಣಾಙ್ಕ sbi_hart_protection_register(ಸಂರಚನಾ sbi_hart_protection *hprot)
{
	ಸಂರಚನಾ sbi_hart_protection *pos = ರಿಕ್ತ;
	bool found_pos = false;

	ಯದಿ (!hprot)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಯದಿ (hprot->type >= SBI_HART_PROTECTION_TYPE_MAX)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	sbi_list_for_each_entry(pos, &hart_protection_list, head) {
		ಯದಿ (hprot->rating > pos->rating) {
			found_pos = true;
			ಭಙ್ಗ;
		}
	}

	ಯದಿ (found_pos)
		sbi_list_add_tail(&hprot->head, &pos->head);
	ಅನ್ಯಥಾ
		sbi_list_add_tail(&hprot->head, &hart_protection_list);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ sbi_hart_protection_unregister(ಸಂರಚನಾ sbi_hart_protection *hprot)
{
	ಯದಿ (!hprot)
		ಪ್ರತ್ಯಾಗಮ;

	sbi_list_del(&hprot->head);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ __hart_protection_configure(ಸಂರಚನಾ sbi_scratch *scratch,
				       ಸಂರಚನಾ sbi_hart_protection *hprot,
				       ಸಂರಚನಾ sbi_domain *dom)
{
	ಯದಿ (!hprot)
		ಪ್ರತ್ಯಾಗಮ 0;
	ಯದಿ (!hprot->configure)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOSYS;

	ಪ್ರತ್ಯಾಗಮ hprot->configure(scratch, dom);
}

ಸ್ಥಿರ ಶೂನ್ಯ __hart_protection_unconfigure(ಸಂರಚನಾ sbi_scratch *scratch,
					  ಸಂರಚನಾ sbi_hart_protection *hprot,
					  ಸಂರಚನಾ sbi_domain *dom)
{
	ಯದಿ (!hprot || !hprot->unconfigure)
		ಪ್ರತ್ಯಾಗಮ;

	hprot->unconfigure(scratch, dom);
}

ಪೂರ್ಣಾಙ್ಕ sbi_hart_protection_configure(ಸಂರಚನಾ sbi_scratch *scratch,
				  ಸಂರಚನಾ sbi_domain *dom)
{
	bool do_configure, memory_protect_done = false;
	ಸಂರಚನಾ sbi_hart_protection *hprot;
	ಪೂರ್ಣಾಙ್ಕ ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_configure = false;
		ವಿಕಲ್ಪ (hprot->type) {
		ಪಕ್ಷ SBI_HART_PROTECTION_TYPE_MEMORY:
			do_configure = !memory_protect_done;
			memory_protect_done = true;
			ಭಙ್ಗ;
		ಪಕ್ಷ SBI_HART_PROTECTION_TYPE_ID:
			do_configure = true;
			ಭಙ್ಗ;
		ಸ್ವಭಾವ:
			ಭಙ್ಗ;
		}
		ಯದಿ (!do_configure)
			ಅನುವರ್ತ;

		ret = __hart_protection_configure(scratch, hprot, dom);
		ಯದಿ (ret)
			ಪ್ರತ್ಯಾಗಮ ret;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ sbi_hart_protection_unconfigure(ಸಂರಚನಾ sbi_scratch *scratch,
				     ಸಂರಚನಾ sbi_domain *dom)
{

	bool do_unconfigure, memory_protect_done = false;
	ಸಂರಚನಾ sbi_hart_protection *hprot;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_unconfigure = false;
		ವಿಕಲ್ಪ (hprot->type) {
		ಪಕ್ಷ SBI_HART_PROTECTION_TYPE_MEMORY:
			do_unconfigure = !memory_protect_done;
			memory_protect_done = true;
			ಭಙ್ಗ;
		ಪಕ್ಷ SBI_HART_PROTECTION_TYPE_ID:
			do_unconfigure = true;
			ಭಙ್ಗ;
		ಸ್ವಭಾವ:
			ಭಙ್ಗ;
		}
		ಯದಿ (!do_unconfigure)
			ಅನುವರ್ತ;

		__hart_protection_unconfigure(scratch, hprot, dom);
	}
}

ಪೂರ್ಣಾಙ್ಕ sbi_hart_protection_reconfigure(ಸಂರಚನಾ sbi_scratch *scratch,
				    ಸಂರಚನಾ sbi_domain *current_dom,
				    ಸಂರಚನಾ sbi_domain *next_dom)
{
	bool do_reconfigure, memory_protect_done = false;
	ಸಂರಚನಾ sbi_hart_protection *hprot;
	ಪೂರ್ಣಾಙ್ಕ ret;

	sbi_list_for_each_entry(hprot, &hart_protection_list, head) {
		do_reconfigure = false;
		ವಿಕಲ್ಪ (hprot->type) {
		ಪಕ್ಷ SBI_HART_PROTECTION_TYPE_MEMORY:
			do_reconfigure = !memory_protect_done;
			memory_protect_done = true;
			ಭಙ್ಗ;
		ಪಕ್ಷ SBI_HART_PROTECTION_TYPE_ID:
			do_reconfigure = true;
			ಭಙ್ಗ;
		ಸ್ವಭಾವ:
			ಭಙ್ಗ;
		}
		ಯದಿ (!do_reconfigure)
			ಅನುವರ್ತ;

		__hart_protection_unconfigure(scratch, hprot, current_dom);
		ret = __hart_protection_configure(scratch, hprot, next_dom);
		ಯದಿ (ret)
			ಪ್ರತ್ಯಾಗಮ ret;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ sbi_hart_protection_temp_map_range(unsigned long base, unsigned long size)
{
	ಸಂರಚನಾ sbi_hart_protection *hprot = __hart_memory_protection_best();

	ಯದಿ (!hprot || !hprot->temp_map_range)
		ಪ್ರತ್ಯಾಗಮ 0;

	ಪ್ರತ್ಯಾಗಮ hprot->temp_map_range(sbi_scratch_thishart_ptr(), base, size);
}

ಪೂರ್ಣಾಙ್ಕ sbi_hart_protection_temp_unmap_range(unsigned long base, unsigned long size)
{
	ಸಂರಚನಾ sbi_hart_protection *hprot = __hart_memory_protection_best();

	ಯದಿ (!hprot || !hprot->temp_unmap_range)
		ಪ್ರತ್ಯಾಗಮ 0;

	ಪ್ರತ್ಯಾಗಮ hprot->temp_unmap_range(sbi_scratch_thishart_ptr(), base, size);
}
