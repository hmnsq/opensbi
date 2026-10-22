/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_platform.h>
#ضمن <sbi/sbi_string.h>

ساكن مضمن حرف *sbi_platform_feature_id2string(بنانشانی لما feature)
{
	حرف *fstr = عدم;

	إذا (!feature)
		أرجع عدم;

	بدل (feature) {
	حالة SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		اكسر;
	افتراضي:
		اكسر;
	}

	أرجع fstr;
}

فراغ sbi_platform_get_features_str(ثابت بنية sbi_platform *plat,
				   حرف *features_str, صحيح nfstr)
{
	بنانشانی لما features, feat = 1UL;
	حرف *عارضی;
	صحيح وتھ = 0;

	إذا (!plat || !features_str || !nfstr)
		أرجع;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	إذا (!features)
		اذهب پورا;

	افعل {
		إذا (features & feat) {
			عارضی = sbi_platform_feature_id2string(feat);
			إذا (عارضی) {
				صحيح len = sbi_snprintf(features_str + وتھ,
						       nfstr - وتھ,
						       "%s,", عارضی);
				إذا (len < 0)
					اكسر;

				إذا (وتھ + len >= nfstr) {
					/* No more space for features */
					وتھ = nfstr;
					اكسر;
				} وإلا
					وتھ = وتھ + len;
			}
		}
		feat = feat << 1;
	} طالما (feat <= SBI_PLATFORM_HAS_آخری_FEATURE);

پورا:
	إذا (وتھ)
		features_str[وتھ - 1] = '\0';
	وإلا
		sbi_strncpy(features_str, "none", nfstr);
}
