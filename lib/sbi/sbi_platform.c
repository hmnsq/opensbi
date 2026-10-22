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

	якщо (!feature)
		повернути NULL;

	switch (feature) {
	case SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		break;
	default:
		break;
	}

	повернути fstr;
}

void sbi_platform_get_features_str(const struct sbi_platform *plat,
				   char *features_str, ціле nfstr)
{
	unsigned long features, feat = 1UL;
	char *temp;
	ціле offset = 0;

	якщо (!plat || !features_str || !nfstr)
		повернути;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	якщо (!features)
		goto done;

	do {
		якщо (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			якщо (temp) {
				ціле len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				якщо (len < 0)
					break;

				якщо (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					break;
				} інакше
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} поки (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	якщо (offset)
		features_str[offset - 1] = '\0';
	інакше
		sbi_strncpy(features_str, "none", nfstr);
}
