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

	もし (!feature)
		戻る NULL;

	switch (feature) {
	case SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		break;
	default:
		break;
	}

	戻る fstr;
}

void sbi_platform_get_features_str(const struct sbi_platform *plat,
				   char *features_str, 整数 nfstr)
{
	unsigned long features, feat = 1UL;
	char *temp;
	整数 offset = 0;

	もし (!plat || !features_str || !nfstr)
		戻る;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	もし (!features)
		goto done;

	do {
		もし (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			もし (temp) {
				整数 len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				もし (len < 0)
					break;

				もし (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					break;
				} 否则
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} ながら (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	もし (offset)
		features_str[offset - 1] = '\0';
	否则
		sbi_strncpy(features_str, "none", nfstr);
}
