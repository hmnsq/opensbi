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

ساكن مضمن حرف *sbi_platform_feature_id2string(unsigned long feature)
{
	حرف *fstr = عدم;

	𐡄𐡍 (!feature)
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
	unsigned long features, feat = 1UL;
	حرف *temp;
	صحيح offset = 0;

	𐡄𐡍 (!plat || !features_str || !nfstr)
		أرجع;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	𐡄𐡍 (!features)
		اذهب done;

	𐡏𐡁𐡃 {
		𐡄𐡍 (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			𐡄𐡍 (temp) {
				صحيح len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				𐡄𐡍 (len < 0)
					اكسر;

				𐡄𐡍 (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					اكسر;
				} وإلا
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} طالما (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	𐡄𐡍 (offset)
		features_str[offset - 1] = '\0';
	وإلا
		sbi_strncpy(features_str, "none", nfstr);
}
