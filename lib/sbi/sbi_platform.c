/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_platform.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ *sbi_platform_feature_id2string(unsigned long feature)
{
	ವರ್ಣ *fstr = ರಿಕ್ತ;

	ಯದಿ (!feature)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ವಿಕಲ್ಪ (feature) {
	ಪಕ್ಷ SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		ಭಙ್ಗ;
	ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}

	ಪ್ರತ್ಯಾಗಮ fstr;
}

ಶೂನ್ಯ sbi_platform_get_features_str(ಧ್ರುವ ಸಂರಚನಾ sbi_platform *plat,
				   ವರ್ಣ *features_str, ಪೂರ್ಣಾಙ್ಕ nfstr)
{
	unsigned long features, feat = 1UL;
	ವರ್ಣ *temp;
	ಪೂರ್ಣಾಙ್ಕ offset = 0;

	ಯದಿ (!plat || !features_str || !nfstr)
		ಪ್ರತ್ಯಾಗಮ;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	ಯದಿ (!features)
		ಗಚ್ಛ done;

	ಕುರು {
		ಯದಿ (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			ಯದಿ (temp) {
				ಪೂರ್ಣಾಙ್ಕ len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				ಯದಿ (len < 0)
					ಭಙ್ಗ;

				ಯದಿ (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					ಭಙ್ಗ;
				} ಅನ್ಯಥಾ
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} ಯಾವತ್ (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	ಯದಿ (offset)
		features_str[offset - 1] = '\0';
	ಅನ್ಯಥಾ
		sbi_strncpy(features_str, "none", nfstr);
}
