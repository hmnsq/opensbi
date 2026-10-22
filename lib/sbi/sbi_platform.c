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

স্থির অন্তঃস্থ বর্ণ *sbi_platform_feature_id2string(unsigned long feature)
{
	বর্ণ *fstr = রিক্ত;

	যদি (!feature)
		প্রত্যাগম রিক্ত;

	বিকল্প (feature) {
	পক্ষ SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		ভঙ্গ;
	স্বভাব:
		ভঙ্গ;
	}

	প্রত্যাগম fstr;
}

শূন্য sbi_platform_get_features_str(ধ্রুব সংরচনা sbi_platform *plat,
				   বর্ণ *features_str, পূর্ণাঙ্ক nfstr)
{
	unsigned long features, feat = 1UL;
	বর্ণ *temp;
	পূর্ণাঙ্ক offset = 0;

	যদি (!plat || !features_str || !nfstr)
		প্রত্যাগম;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	যদি (!features)
		গচ্ছ done;

	কুরু {
		যদি (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			যদি (temp) {
				পূর্ণাঙ্ক len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				যদি (len < 0)
					ভঙ্গ;

				যদি (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					ভঙ্গ;
				} অন্যথা
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} যাবত্ (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	যদি (offset)
		features_str[offset - 1] = '\0';
	অন্যথা
		sbi_strncpy(features_str, "none", nfstr);
}
