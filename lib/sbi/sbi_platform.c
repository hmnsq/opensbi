/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_platform.h>
#अन्तर्भाव <sbi/sbi_string.h>

स्थिर अन्तःस्थ वर्ण *sbi_platform_feature_id2string(unsigned long feature)
{
	वर्ण *fstr = रिक्त;

	जइ (!feature)
		प्रत्यागम रिक्त;

	विकल्प (feature) {
	पक्ष SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		भङ्ग;
	स्वभाव:
		भङ्ग;
	}

	प्रत्यागम fstr;
}

शून्य sbi_platform_get_features_str(ध्रुव संरचना sbi_platform *plat,
				   वर्ण *features_str, पूर्णाङ्क nfstr)
{
	unsigned long features, feat = 1UL;
	वर्ण *temp;
	पूर्णाङ्क offset = 0;

	जइ (!plat || !features_str || !nfstr)
		प्रत्यागम;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	जइ (!features)
		गच्छ done;

	करेहि {
		जइ (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			जइ (temp) {
				पूर्णाङ्क len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				जइ (len < 0)
					भङ्ग;

				जइ (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					भङ्ग;
				} अण्णहा
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} जाव (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	जइ (offset)
		features_str[offset - 1] = '\0';
	अण्णहा
		sbi_strncpy(features_str, "none", nfstr);
}
