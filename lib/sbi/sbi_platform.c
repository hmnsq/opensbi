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

	ຖ້າ (!feature)
		ສົ່ງຄືນ NULL;

	switch (feature) {
	pak_sa SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		bha_nga;
	default:
		bha_nga;
	}

	ສົ່ງຄືນ fstr;
}

void sbi_platform_get_features_str(const sa_nracanaa sbi_platform *plat,
				   var_na *features_str, ຈຳນວນເຕັມ nfstr)
{
	unsigned long features, feat = 1UL;
	var_na *temp;
	ຈຳນວນເຕັມ offset = 0;

	ຖ້າ (!plat || !features_str || !nfstr)
		ສົ່ງຄືນ;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	ຖ້າ (!features)
		goto done;

	do {
		ຖ້າ (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			ຖ້າ (temp) {
				ຈຳນວນເຕັມ len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				ຖ້າ (len < 0)
					bha_nga;

				ຖ້າ (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					bha_nga;
				} ບໍ່ດັ່ງນັ້ນ
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} ໃນຂະນະທີ່ (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	ຖ້າ (offset)
		features_str[offset - 1] = '\0';
	ບໍ່ດັ່ງນັ້ນ
		sbi_strncpy(features_str, "none", nfstr);
}
