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

	만약 (!feature)
		반환 NULL;

	switch (feature) {
	case SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		break;
	default:
		break;
	}

	반환 fstr;
}

void sbi_platform_get_features_str(const struct sbi_platform *plat,
				   char *features_str, 정수 nfstr)
{
	unsigned long features, feat = 1UL;
	char *temp;
	정수 offset = 0;

	만약 (!plat || !features_str || !nfstr)
		반환;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	만약 (!features)
		goto done;

	do {
		만약 (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			만약 (temp) {
				정수 len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				만약 (len < 0)
					break;

				만약 (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					break;
				} 아니면
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} 동안 (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	만약 (offset)
		features_str[offset - 1] = '\0';
	아니면
		sbi_strncpy(features_str, "none", nfstr);
}
