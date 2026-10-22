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

static inline char *sbi_platform_feature_id2string(unsigned long feature)
{
	char *fstr = NULL;

	ако (!feature)
		врати NULL;

	switch (feature) {
	case SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		break;
	default:
		break;
	}

	врати fstr;
}

void sbi_platform_get_features_str(const struct sbi_platform *plat,
				   char *features_str, цео nfstr)
{
	unsigned long features, feat = 1UL;
	char *temp;
	цео offset = 0;

	ако (!plat || !features_str || !nfstr)
		врати;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	ако (!features)
		goto done;

	do {
		ако (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			ако (temp) {
				цео len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				ако (len < 0)
					break;

				ако (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					break;
				} инако
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} док (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	ако (offset)
		features_str[offset - 1] = '\0';
	инако
		sbi_strncpy(features_str, "none", nfstr);
}
