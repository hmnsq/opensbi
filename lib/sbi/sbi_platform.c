/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_platform.h>
#অন্তর্ভাব <sbi/sbi_string.h>

স্থির অন্তঃরেখ অক্ষর *sbi_platform_feature_id2string(unsigned long feature)
{
	অক্ষর *fstr = ফাঁকা;

	যদি (!feature)
		ফেরত ফাঁকা;

	নির্বাচন (feature) {
	ক্ষেত্র SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		বিরতি;
	পূর্বনির্ধারিত:
		বিরতি;
	}

	ফেরত fstr;
}

শূন্য sbi_platform_get_features_str(ধ্রুবক কাঠামো sbi_platform *plat,
				   অক্ষর *features_str, পূর্ণসংখ্যা nfstr)
{
	unsigned long features, feat = 1UL;
	অক্ষর *অস্থায়ী;
	পূর্ণসংখ্যা offset = 0;

	যদি (!plat || !features_str || !nfstr)
		ফেরত;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	যদি (!features)
		যাও done;

	করো {
		যদি (features & feat) {
			অস্থায়ী = sbi_platform_feature_id2string(feat);
			যদি (অস্থায়ী) {
				পূর্ণসংখ্যা len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", অস্থায়ী);
				যদি (len < 0)
					বিরতি;

				যদি (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					বিরতি;
				} অন্যথা
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} যখন (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	যদি (offset)
		features_str[offset - 1] = '\0';
	অন্যথা
		sbi_strncpy(features_str, "none", nfstr);
}
