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

	אם (!feature)
		החזר عدم;

	بدل (feature) {
	حالة SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		اكسر;
	افتراضي:
		اكسر;
	}

	החזר fstr;
}

فراغ sbi_platform_get_features_str(ثابت بنية sbi_platform *plat,
				   حرف *features_str, صحيح nfstr)
{
	unsigned long features, feat = 1UL;
	حرف *temp;
	صحيح offset = 0;

	אם (!plat || !features_str || !nfstr)
		החזר;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	אם (!features)
		اذهب done;

	עשה {
		אם (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			אם (temp) {
				صحيح len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				אם (len < 0)
					اكسر;

				אם (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					اكسر;
				} אחרת
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} כלעוד (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	אם (offset)
		features_str[offset - 1] = '\0';
	אחרת
		sbi_strncpy(features_str, "none", nfstr);
}
