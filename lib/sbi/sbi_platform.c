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

	නම් (!feature)
		ආපසු NULL;

	switch (feature) {
	pak_sa SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		bha_nga;
	default:
		bha_nga;
	}

	ආපසු fstr;
}

void sbi_platform_get_features_str(const sa_nracanaa sbi_platform *plat,
				   var_na *features_str, පූර්ණ_සංඛ්‍යා nfstr)
{
	unsigned long features, feat = 1UL;
	var_na *temp;
	පූර්ණ_සංඛ්‍යා offset = 0;

	නම් (!plat || !features_str || !nfstr)
		ආපසු;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	නම් (!features)
		goto done;

	do {
		නම් (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			නම් (temp) {
				පූර්ණ_සංඛ්‍යා len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				නම් (len < 0)
					bha_nga;

				නම් (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					bha_nga;
				} නැතහොත්
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} අතරතුර (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	නම් (offset)
		features_str[offset - 1] = '\0';
	නැතහොත්
		sbi_strncpy(features_str, "none", nfstr);
}
