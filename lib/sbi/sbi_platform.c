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
		върни NULL;

	switch (feature) {
	case SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		break;
	default:
		break;
	}

	върни fstr;
}

void sbi_platform_get_features_str(const struct sbi_platform *plat,
				   char *features_str, цяло nfstr)
{
	unsigned long features, feat = 1UL;
	char *temp;
	цяло offset = 0;

	ако (!plat || !features_str || !nfstr)
		върни;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	ако (!features)
		goto done;

	do {
		ако (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			ако (temp) {
				цяло len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				ако (len < 0)
					break;

				ако (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					break;
				} иначе
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} докато (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	ако (offset)
		features_str[offset - 1] = '\0';
	иначе
		sbi_strncpy(features_str, "none", nfstr);
}
