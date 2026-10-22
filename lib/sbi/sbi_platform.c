/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_platform.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_string.h>

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ *sbi_platform_feature_id2string(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ feature)
{
	ਅੱਖਰ *fstr = ਨਿਰਾ;

	ਜੇ (!feature)
		ਵਾਪਸ ਨਿਰਾ;

	ਚੋਣ (feature) {
	ਹਾਲਤ SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		ਰੋਕੋ;
	ਮੂਲ:
		ਰੋਕੋ;
	}

	ਵਾਪਸ fstr;
}

ਖਾਲੀ sbi_platform_get_features_str(ਅਚਲ ਬਣਤਰ sbi_platform *plat,
				   ਅੱਖਰ *features_str, ਪੂਰਨ nfstr)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ features, feat = 1UL;
	ਅੱਖਰ *ਆਰਜ਼ੀ;
	ਪੂਰਨ ਵਿੱਥ = 0;

	ਜੇ (!plat || !features_str || !nfstr)
		ਵਾਪਸ;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	ਜੇ (!features)
		ਜਾਓ ਪੂਰਾ;

	ਕਰੋ {
		ਜੇ (features & feat) {
			ਆਰਜ਼ੀ = sbi_platform_feature_id2string(feat);
			ਜੇ (ਆਰਜ਼ੀ) {
				ਪੂਰਨ len = sbi_snprintf(features_str + ਵਿੱਥ,
						       nfstr - ਵਿੱਥ,
						       "%s,", ਆਰਜ਼ੀ);
				ਜੇ (len < 0)
					ਰੋਕੋ;

				ਜੇ (ਵਿੱਥ + len >= nfstr) {
					/* No more space for features */
					ਵਿੱਥ = nfstr;
					ਰੋਕੋ;
				} ਨਹੀਂਤਾਂ
					ਵਿੱਥ = ਵਿੱਥ + len;
			}
		}
		feat = feat << 1;
	} ਜਦੋਂ (feat <= SBI_PLATFORM_HAS_ਆਖਰੀ_FEATURE);

ਪੂਰਾ:
	ਜੇ (ਵਿੱਥ)
		features_str[ਵਿੱਥ - 1] = '\0';
	ਨਹੀਂਤਾਂ
		sbi_strncpy(features_str, "none", nfstr);
}
