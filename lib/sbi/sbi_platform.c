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

	如果 (!feature)
		返回 NULL;

	switch (feature) {
	case SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		break;
	default:
		break;
	}

	返回 fstr;
}

void sbi_platform_get_features_str(const struct sbi_platform *plat,
				   char *features_str, 整数 nfstr)
{
	unsigned long features, feat = 1UL;
	char *temp;
	整数 offset = 0;

	如果 (!plat || !features_str || !nfstr)
		返回;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	如果 (!features)
		goto done;

	do {
		如果 (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			如果 (temp) {
				整数 len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				如果 (len < 0)
					break;

				如果 (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					break;
				} 否则
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} 当 (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	如果 (offset)
		features_str[offset - 1] = '\0';
	否则
		sbi_strncpy(features_str, "none", nfstr);
}
