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

	ပေႃး (!feature)
		ပွၵ်ႈ NULL;

	switch (feature) {
	pak_sa SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		bha_nga;
	default:
		bha_nga;
	}

	ပွၵ်ႈ fstr;
}

void sbi_platform_get_features_str(const sa_nracanaa sbi_platform *plat,
				   var_na *features_str, puur_naa_nka nfstr)
{
	unsigned long features, feat = 1UL;
	var_na *temp;
	puur_naa_nka offset = 0;

	ပေႃး (!plat || !features_str || !nfstr)
		ပွၵ်ႈ;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	ပေႃး (!features)
		goto done;

	do {
		ပေႃး (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			ပေႃး (temp) {
				puur_naa_nka len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				ပေႃး (len < 0)
					bha_nga;

				ပေႃး (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					bha_nga;
				} ပေႃးဢမ်ႇ
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} ၶၢဝ်း (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	ပေႃး (offset)
		features_str[offset - 1] = '\0';
	ပေႃးဢမ်ႇ
		sbi_strncpy(features_str, "none", nfstr);
}
