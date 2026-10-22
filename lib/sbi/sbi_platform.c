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

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ *sbi_platform_feature_id2string(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ feature)
{
	ಅಕ್ಷರ *fstr = ಖಾಲಿ;

	ಆದರೆ (!feature)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಆಯ್ಕೆ (feature) {
	ಸ್ಥಿತಿ SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}

	ಹಿಂತಿರುಗಿ fstr;
}

ಶೂನ್ಯ sbi_platform_get_features_str(ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_platform *plat,
				   ಅಕ್ಷರ *features_str, ಪೂರ್ಣಾಂಕ nfstr)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ features, feat = 1UL;
	ಅಕ್ಷರ *ತಾತ್ಕಾಲಿಕ;
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = 0;

	ಆದರೆ (!plat || !features_str || !nfstr)
		ಹಿಂತಿರುಗಿ;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	ಆದರೆ (!features)
		ಹೋಗು ಮುಗಿದ;

	ಮಾಡು {
		ಆದರೆ (features & feat) {
			ತಾತ್ಕಾಲಿಕ = sbi_platform_feature_id2string(feat);
			ಆದರೆ (ತಾತ್ಕಾಲಿಕ) {
				ಪೂರ್ಣಾಂಕ len = sbi_snprintf(features_str + ಸ್ಥಾನಾಂತರ,
						       nfstr - ಸ್ಥಾನಾಂತರ,
						       "%s,", ತಾತ್ಕಾಲಿಕ);
				ಆದರೆ (len < 0)
					ನಿಲ್ಲಿಸು;

				ಆದರೆ (ಸ್ಥಾನಾಂತರ + len >= nfstr) {
					/* No more space for features */
					ಸ್ಥಾನಾಂತರ = nfstr;
					ನಿಲ್ಲಿಸು;
				} ಇಲ್ಲದಿದ್ದರೆ
					ಸ್ಥಾನಾಂತರ = ಸ್ಥಾನಾಂತರ + len;
			}
		}
		feat = feat << 1;
	} ಆಗ (feat <= SBI_PLATFORM_HAS_ಕೊನೆಯ_FEATURE);

ಮುಗಿದ:
	ಆದರೆ (ಸ್ಥಾನಾಂತರ)
		features_str[ಸ್ಥಾನಾಂತರ - 1] = '\0';
	ಇಲ್ಲದಿದ್ದರೆ
		sbi_strncpy(features_str, "none", nfstr);
}
