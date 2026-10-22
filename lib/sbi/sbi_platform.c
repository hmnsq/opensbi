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

	nếu (!feature)
		trả_về NULL;

	switch (feature) {
	case SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		break;
	default:
		break;
	}

	trả_về fstr;
}

void sbi_platform_get_features_str(const struct sbi_platform *plat,
				   char *features_str, số_nguyên nfstr)
{
	unsigned long features, feat = 1UL;
	char *temp;
	số_nguyên offset = 0;

	nếu (!plat || !features_str || !nfstr)
		trả_về;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	nếu (!features)
		goto done;

	do {
		nếu (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			nếu (temp) {
				số_nguyên len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				nếu (len < 0)
					break;

				nếu (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					break;
				} không_thì
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} trong_khi (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	nếu (offset)
		features_str[offset - 1] = '\0';
	không_thì
		sbi_strncpy(features_str, "none", nfstr);
}
