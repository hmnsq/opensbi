/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_platform.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_string.h>

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର *sbi_platform_feature_id2string(unsigned long feature)
{
	ଅକ୍ଷର *fstr = ଖାଲି;

	ଯଦି (!feature)
		ଫେରନ୍ତୁ ଖାଲି;

	ବାଛ (feature) {
	ସ୍ଥିତି SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଭାଙ୍ଗ;
	}

	ଫେରନ୍ତୁ fstr;
}

ଶୂନ୍ୟ sbi_platform_get_features_str(ଅଚଳ ସଂରଚନା sbi_platform *plat,
				   ଅକ୍ଷର *features_str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nfstr)
{
	unsigned long features, feat = 1UL;
	ଅକ୍ଷର *temp;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset = 0;

	ଯଦି (!plat || !features_str || !nfstr)
		ଫେରନ୍ତୁ;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	ଯଦି (!features)
		ଯାଅ done;

	କର {
		ଯଦି (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			ଯଦି (temp) {
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				ଯଦି (len < 0)
					ଭାଙ୍ଗ;

				ଯଦି (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					ଭାଙ୍ଗ;
				} ଅନ୍ୟଥା
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} ଯେତେବେଳେ (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	ଯଦି (offset)
		features_str[offset - 1] = '\0';
	ଅନ୍ୟଥା
		sbi_strncpy(features_str, "none", nfstr);
}
