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

	ប្រសិនបើ (!feature)
		ត្រឡប់ NULL;

	switch (feature) {
	pak_sa SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		bha_nga;
	default:
		bha_nga;
	}

	ត្រឡប់ fstr;
}

void sbi_platform_get_features_str(const sa_nracanaa sbi_platform *plat,
				   var_na *features_str, ចំនួនគត់ nfstr)
{
	unsigned long features, feat = 1UL;
	var_na *temp;
	ចំនួនគត់ offset = 0;

	ប្រសិនបើ (!plat || !features_str || !nfstr)
		ត្រឡប់;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	ប្រសិនបើ (!features)
		goto done;

	do {
		ប្រសិនបើ (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			ប្រសិនបើ (temp) {
				ចំនួនគត់ len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				ប្រសិនបើ (len < 0)
					bha_nga;

				ប្រសិនបើ (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					bha_nga;
				} បើមិនដូច្នេះ
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} ខណៈពេល (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	ប្រសិនបើ (offset)
		features_str[offset - 1] = '\0';
	បើមិនដូច្នេះ
		sbi_strncpy(features_str, "none", nfstr);
}
