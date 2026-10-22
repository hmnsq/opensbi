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

	ถ้า (!feature)
		คืน NULL;

	switch (feature) {
	pak_sa SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		bha_nga;
	default:
		bha_nga;
	}

	คืน fstr;
}

void sbi_platform_get_features_str(const sa_nracanaa sbi_platform *plat,
				   var_na *features_str, จำนวนเต็ม nfstr)
{
	unsigned long features, feat = 1UL;
	var_na *temp;
	จำนวนเต็ม offset = 0;

	ถ้า (!plat || !features_str || !nfstr)
		คืน;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	ถ้า (!features)
		goto done;

	do {
		ถ้า (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			ถ้า (temp) {
				จำนวนเต็ม len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				ถ้า (len < 0)
					bha_nga;

				ถ้า (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					bha_nga;
				} มิฉะนั้น
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} ขณะ (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	ถ้า (offset)
		features_str[offset - 1] = '\0';
	มิฉะนั้น
		sbi_strncpy(features_str, "none", nfstr);
}
