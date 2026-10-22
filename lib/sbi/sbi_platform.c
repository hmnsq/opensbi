/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_string.h>

ایستا درونخطی نویسه *sbi_platform_feature_id2string(unsigned long feature)
{
	نویسه *fstr = پوچ;

	اگر (!feature)
		بازگشت پوچ;

	انتخاب (feature) {
	حالت SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		بشکن;
	پیشفرض:
		بشکن;
	}

	بازگشت fstr;
}

تهی sbi_platform_get_features_str(ثابت ساختار sbi_platform *plat,
				   نویسه *features_str, صحیح nfstr)
{
	unsigned long features, feat = 1UL;
	نویسه *temp;
	صحیح offset = 0;

	اگر (!plat || !features_str || !nfstr)
		بازگشت;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	اگر (!features)
		برو done;

	انجامبده {
		اگر (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			اگر (temp) {
				صحیح len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				اگر (len < 0)
					بشکن;

				اگر (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					بشکن;
				} وگرنه
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} وقتی (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	اگر (offset)
		features_str[offset - 1] = '\0';
	وگرنه
		sbi_strncpy(features_str, "none", nfstr);
}
