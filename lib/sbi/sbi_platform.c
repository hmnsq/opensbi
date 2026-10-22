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

	အကယ်၍ (!feature)
		ပြန်ပေး NULL;

	switch (feature) {
	pak_sa SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		bha_nga;
	default:
		bha_nga;
	}

	ပြန်ပေး fstr;
}

void sbi_platform_get_features_str(const sa_nracanaa sbi_platform *plat,
				   var_na *features_str, ကိန်းပြည့် nfstr)
{
	unsigned long features, feat = 1UL;
	var_na *temp;
	ကိန်းပြည့် offset = 0;

	အကယ်၍ (!plat || !features_str || !nfstr)
		ပြန်ပေး;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	အကယ်၍ (!features)
		goto done;

	do {
		အကယ်၍ (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			အကယ်၍ (temp) {
				ကိန်းပြည့် len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				အကယ်၍ (len < 0)
					bha_nga;

				အကယ်၍ (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					bha_nga;
				} မဟုတ်ပါက
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} နေစဉ် (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	အကယ်၍ (offset)
		features_str[offset - 1] = '\0';
	မဟုတ်ပါက
		sbi_strncpy(features_str, "none", nfstr);
}
