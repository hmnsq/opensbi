/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_platform.h>
#అన్తర్భావ <sbi/sbi_string.h>

స్థిర అంతఃవరుస అక్షరం *sbi_platform_feature_id2string(గుర్తులేని దీర్ఘ feature)
{
	అక్షరం *fstr = ఖాళీ;

	ఒకవేళ (!feature)
		తిరిగి ఖాళీ;

	ఎంపిక (feature) {
	స్థితి SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		ఆపు;
	అప్రమేయ:
		ఆపు;
	}

	తిరిగి fstr;
}

శూన్య sbi_platform_get_features_str(స్థిరరాశి నిర్మాణం sbi_platform *plat,
				   అక్షరం *features_str, పూర్ణాంకం nfstr)
{
	గుర్తులేని దీర్ఘ features, feat = 1UL;
	అక్షరం *తాత్కాలిక;
	పూర్ణాంకం స్థానభ్రంశం = 0;

	ఒకవేళ (!plat || !features_str || !nfstr)
		తిరిగి;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	ఒకవేళ (!features)
		వెళ్ళు పూర్తి;

	చేయి {
		ఒకవేళ (features & feat) {
			తాత్కాలిక = sbi_platform_feature_id2string(feat);
			ఒకవేళ (తాత్కాలిక) {
				పూర్ణాంకం len = sbi_snprintf(features_str + స్థానభ్రంశం,
						       nfstr - స్థానభ్రంశం,
						       "%s,", తాత్కాలిక);
				ఒకవేళ (len < 0)
					ఆపు;

				ఒకవేళ (స్థానభ్రంశం + len >= nfstr) {
					/* No more space for features */
					స్థానభ్రంశం = nfstr;
					ఆపు;
				} లేకపోతే
					స్థానభ్రంశం = స్థానభ్రంశం + len;
			}
		}
		feat = feat << 1;
	} అయితే (feat <= SBI_PLATFORM_HAS_చివరి_FEATURE);

పూర్తి:
	ఒకవేళ (స్థానభ్రంశం)
		features_str[స్థానభ్రంశం - 1] = '\0';
	లేకపోతే
		sbi_strncpy(features_str, "none", nfstr);
}
