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

	ܐܢ (!feature)
		ܦܢܐ عدم;

	بدل (feature) {
	حالة SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		اكسر;
	افتراضي:
		اكسر;
	}

	ܦܢܐ fstr;
}

فراغ sbi_platform_get_features_str(ثابت بنية sbi_platform *plat,
				   حرف *features_str, صحيح nfstr)
{
	unsigned long features, feat = 1UL;
	حرف *temp;
	صحيح offset = 0;

	ܐܢ (!plat || !features_str || !nfstr)
		ܦܢܐ;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	ܐܢ (!features)
		اذهب done;

	ܥܒܕ {
		ܐܢ (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			ܐܢ (temp) {
				صحيح len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				ܐܢ (len < 0)
					اكسر;

				ܐܢ (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					اكسر;
				} ܐܚܪܢܐ
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} ܟܡܐ (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	ܐܢ (offset)
		features_str[offset - 1] = '\0';
	ܐܚܪܢܐ
		sbi_strncpy(features_str, "none", nfstr);
}
