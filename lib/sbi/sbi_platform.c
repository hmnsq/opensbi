/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#include <sbi/sbi_console.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_string.h>

static anta_hstha var_na *sbi_platform_feature_id2string(unsigned long feature)
{
	var_na *fstr = NULL;

	གལ་སྲིད (!feature)
		ལོག NULL;

	switch (feature) {
	pak_sa SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		bha_nga;
	default:
		bha_nga;
	}

	ལོག fstr;
}

void sbi_platform_get_features_str(const sa_nracanaa sbi_platform *plat,
				   var_na *features_str, ཧྲིལ nfstr)
{
	unsigned long features, feat = 1UL;
	var_na *temp;
	ཧྲིལ offset = 0;

	གལ་སྲིད (!plat || !features_str || !nfstr)
		ལོག;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	གལ་སྲིད (!features)
		goto done;

	do {
		གལ་སྲིད (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			གལ་སྲིད (temp) {
				ཧྲིལ len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				གལ་སྲིད (len < 0)
					bha_nga;

				གལ་སྲིད (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					bha_nga;
				} དེ་མིན
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} སྐབས (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	གལ་སྲིད (offset)
		features_str[offset - 1] = '\0';
	དེ་མིན
		sbi_strncpy(features_str, "none", nfstr);
}
