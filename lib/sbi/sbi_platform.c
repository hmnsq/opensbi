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

ساكن مضمن حرف *sbi_platform_feature_id2string(بنانشان ڊگهو feature)
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
	بنانشان ڊگهو features, feat = 1UL;
	حرف *عارضي;
	صحيح وٿي = 0;

	إذا (!plat || !features_str || !nfstr)
		أرجع;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	إذا (!features)
		اذهب پورو;

	افعل {
		إذا (features & feat) {
			عارضي = sbi_platform_feature_id2string(feat);
			إذا (عارضي) {
				صحيح len = sbi_snprintf(features_str + وٿي,
						       nfstr - وٿي,
						       "%s,", عارضي);
				إذا (len < 0)
					اكسر;

				إذا (وٿي + len >= nfstr) {
					/* No more space for features */
					وٿي = nfstr;
					اكسر;
				} وإلا
					وٿي = وٿي + len;
			}
		}
		feat = feat << 1;
	} طالما (feat <= SBI_PLATFORM_HAS_آخري_FEATURE);

پورو:
	إذا (وٿي)
		features_str[وٿي - 1] = '\0';
	وإلا
		sbi_strncpy(features_str, "none", nfstr);
}
