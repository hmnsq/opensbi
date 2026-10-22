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

ساكن مضمن حرف *sbi_platform_feature_id2string(بیعلامت دراز feature)
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
	بیعلامت دراز features, feat = 1UL;
	حرف *موقت;
	صحيح جابه_جایی = 0;

	إذا (!plat || !features_str || !nfstr)
		أرجع;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	إذا (!features)
		اذهب انجامشده;

	افعل {
		إذا (features & feat) {
			موقت = sbi_platform_feature_id2string(feat);
			إذا (موقت) {
				صحيح len = sbi_snprintf(features_str + جابه_جایی,
						       nfstr - جابه_جایی,
						       "%s,", موقت);
				إذا (len < 0)
					اكسر;

				إذا (جابه_جایی + len >= nfstr) {
					/* No more space for features */
					جابه_جایی = nfstr;
					اكسر;
				} وإلا
					جابه_جایی = جابه_جایی + len;
			}
		}
		feat = feat << 1;
	} طالما (feat <= SBI_PLATFORM_HAS_آخرین_FEATURE);

انجامشده:
	إذا (جابه_جایی)
		features_str[جابه_جایی - 1] = '\0';
	وإلا
		sbi_strncpy(features_str, "none", nfstr);
}
