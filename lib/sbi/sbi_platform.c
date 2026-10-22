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

ساكن مضمن حرف *sbi_platform_feature_id2string(بینښه اوږد feature)
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
	بینښه اوږد features, feat = 1UL;
	حرف *لنډمهاله;
	صحيح دننه_واټن = 0;

	إذا (!plat || !features_str || !nfstr)
		أرجع;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	إذا (!features)
		اذهب بشپړ;

	افعل {
		إذا (features & feat) {
			لنډمهاله = sbi_platform_feature_id2string(feat);
			إذا (لنډمهاله) {
				صحيح len = sbi_snprintf(features_str + دننه_واټن,
						       nfstr - دننه_واټن,
						       "%s,", لنډمهاله);
				إذا (len < 0)
					اكسر;

				إذا (دننه_واټن + len >= nfstr) {
					/* No more space for features */
					دننه_واټن = nfstr;
					اكسر;
				} وإلا
					دننه_واټن = دننه_واټن + len;
			}
		}
		feat = feat << 1;
	} طالما (feat <= SBI_PLATFORM_HAS_وروستی_FEATURE);

بشپړ:
	إذا (دننه_واټن)
		features_str[دننه_واټن - 1] = '\0';
	وإلا
		sbi_strncpy(features_str, "none", nfstr);
}
