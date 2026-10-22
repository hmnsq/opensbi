/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>

ಬಾಹ್ಯ ರಚನೆ sbi_ecall_extension *ಸ್ಥಿರಾಂಕ sbi_ecall_exts[];

u16 sbi_ecall_ಆವೃತ್ತಿ_major(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ SBI_ECALL_ಆವೃತ್ತಿ_MAJOR;
}

u16 sbi_ecall_ಆವೃತ್ತಿ_minor(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ SBI_ECALL_ಆವೃತ್ತಿ_MINOR;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ecall_impid = SBI_OPENSBI_IMPID;

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sbi_ecall_get_impid(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ ecall_impid;
}

ಶೂನ್ಯ sbi_ecall_ಗಣ_impid(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ impid)
{
	ecall_impid = impid;
}

ಸ್ಥಿರ SBI_LIST_ತಲೆ(ecall_exts_list);

ರಚನೆ sbi_ecall_extension *sbi_ecall_ಕಂಡುಹಿಡಿ_extension(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ extid)
{
	ರಚನೆ sbi_ecall_extension *t, *ret = ಖಾಲಿ;

	sbi_list_for_each_ನಮೂದು(t, &ecall_exts_list, ತಲೆ) {
		ಆದರೆ (t->extid_ಆರಂಭ <= extid && extid <= t->extid_ಕೊನೆ) {
			ret = t;
			ನಿಲ್ಲಿಸು;
		}
	}

	ಹಿಂತಿರುಗಿ ret;
}

ಶೂನ್ಯ sbi_ecall_get_extensions_str(ಅಕ್ಷರ *exts_str, ಪೂರ್ಣಾಂಕ exts_str_ಗಾತ್ರ, bool experimental)
{
	ರಚನೆ sbi_ecall_extension *t;
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = 0;

	ಆದರೆ (!exts_str || exts_str_ಗಾತ್ರ <= 0)
		ಹಿಂತಿರುಗಿ;
	sbi_memset(exts_str, 0, exts_str_ಗಾತ್ರ);

	sbi_list_for_each_ನಮೂದು(t, &ecall_exts_list, ತಲೆ) {
		ಆದರೆ (experimental != t->experimental)
			ಮುಂದುವರಿ;
		ಆದರೆ (ಸ್ಥಾನಾಂತರ + sbi_strlen(t->name) + 1 > exts_str_ಗಾತ್ರ)
			ನಿಲ್ಲಿಸು;
		sbi_snprintf(exts_str + ಸ್ಥಾನಾಂತರ, exts_str_ಗಾತ್ರ - ಸ್ಥಾನಾಂತರ,
			     "%s,", t->name);
		ಸ್ಥಾನಾಂತರ = ಸ್ಥಾನಾಂತರ + sbi_strlen(t->name) + 1;
	}

	ಆದರೆ (ಸ್ಥಾನಾಂತರ)
		exts_str[ಸ್ಥಾನಾಂತರ - 1] = '\0';
	ಇಲ್ಲದಿದ್ದರೆ
		sbi_strncpy(exts_str, "none", exts_str_ಗಾತ್ರ);
}

ಪೂರ್ಣಾಂಕ sbi_ecall_ರಿಜಿಸ್ಟರ್_extension(ರಚನೆ sbi_ecall_extension *ext)
{
	ರಚನೆ sbi_ecall_extension *t;

	ಆದರೆ (!ext || (ext->extid_ಕೊನೆ < ext->extid_ಆರಂಭ) || !ext->handle)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	sbi_list_for_each_ನಮೂದು(t, &ecall_exts_list, ತಲೆ) {
		ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಆರಂಭ = t->extid_ಆರಂಭ;
		ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಕೊನೆ = t->extid_ಕೊನೆ;
		ಆದರೆ (ಕೊನೆ < ext->extid_ಆರಂಭ || ext->extid_ಕೊನೆ < ಆರಂಭ)
			/* no overlap */;
		ಇಲ್ಲದಿದ್ದರೆ
			ಹಿಂತಿರುಗಿ SBI_EINVAL;
	}

	sbi_list_ಕೂಡಿಸು_ಬಾಲ(&ext->ತಲೆ, &ecall_exts_list);

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ sbi_ecall_unregister_extension(ರಚನೆ sbi_ecall_extension *ext)
{
	bool ಸಿಕ್ಕಿತು = false;
	ರಚನೆ sbi_ecall_extension *t;

	ಆದರೆ (!ext)
		ಹಿಂತಿರುಗಿ;

	sbi_list_for_each_ನಮೂದು(t, &ecall_exts_list, ತಲೆ) {
		ಆದರೆ (t == ext) {
			ಸಿಕ್ಕಿತು = true;
			ನಿಲ್ಲಿಸು;
		}
	}

	ಆದರೆ (ಸಿಕ್ಕಿತು)
		sbi_list_del_init(&ext->ತಲೆ);
}

ಪೂರ್ಣಾಂಕ sbi_ecall_handler(ರಚನೆ sbi_trap_context *tcntx)
{
	ಪೂರ್ಣಾಂಕ ret = 0;
	ರಚನೆ sbi_trap_regs *regs = &tcntx->regs;
	ರಚನೆ sbi_ecall_extension *ext;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ extension_id = regs->a7;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ func_id = regs->a6;
	ರಚನೆ sbi_ecall_return ಹೊರಹೋಗು = {0};
	bool is_0_1_spec = 0;

	ext = sbi_ecall_ಕಂಡುಹಿಡಿ_extension(extension_id);
	ಆದರೆ (ext && ext->handle) {
		ret = ext->handle(extension_id, func_id, regs, &ಹೊರಹೋಗು);
		ಆದರೆ (extension_id >= SBI_EXT_0_1_ಗಣ_TIMER &&
		    extension_id <= SBI_EXT_0_1_SHUTDOWN)
			is_0_1_spec = 1;
	} ಇಲ್ಲದಿದ್ದರೆ {
		ret = SBI_ENOTSUPP;
	}

	ಆದರೆ (!ಹೊರಹೋಗು.skip_regs_update) {
		ಆದರೆ (ret < SBI_ಕೊನೆಯ_ERR ||
		    (extension_id != SBI_EXT_0_1_CONSOLE_GETCHAR &&
		     SBI_SUCCESS < ret)) {
			sbi_printf("%s: Invalid error %d for ext=0x%lx "
				   "func=0x%lx\n", __func__, ret,
				   extension_id, func_id);
			ret = SBI_ERR_FAILED;
		}

		/*
		 * This function should return non-zero value only in case of
		 * fatal error. However, there is no good way to distinguish
		 * between a fatal and non-fatal errors yet. That's why we treat
		 * every return value except ETRAP as non-fatal and just return
		 * accordingly for now. Once fatal errors are defined, that
		 * case should be handled differently.
		 */
		regs->mepc += 4;
		regs->a0 = ret;
		ಆದರೆ (!is_0_1_spec)
			regs->a1 = ಹೊರಹೋಗು.ಮೌಲ್ಯ;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ sbi_ecall_init(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ ret;
	ರಚನೆ sbi_ecall_extension *ext;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ i;

	ಗಾಗಿ (i = 0; sbi_ecall_exts[i]; i++) {
		ext = sbi_ecall_exts[i];
		ret = SBI_ENODEV;

		ಆದರೆ (ext->ರಿಜಿಸ್ಟರ್_extensions)
			ret = ext->ರಿಜಿಸ್ಟರ್_extensions();
		ಆದರೆ (ret)
			ಹಿಂತಿರುಗಿ ret;
	}

	ಹಿಂತಿರುಗಿ 0;
}
