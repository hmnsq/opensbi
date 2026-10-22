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

स्थिर अन्तःस्थ वर्ण *sbi_platform_feature_id2string(अचिह्नित दीर्घ feature)
{
	वर्ण *fstr = रिक्त;

	यदि (!feature)
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
	अचिह्नित दीर्घ features, feat = 1UL;
	वर्ण *अस्थायी;
	पूर्णाङ्क विस्थापनम् = 0;

	यदि (!plat || !features_str || !nfstr)
		प्रत्यागम;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	यदि (!features)
		गच्छ समाप्तम्;

	कुरु {
		यदि (features & feat) {
			अस्थायी = sbi_platform_feature_id2string(feat);
			यदि (अस्थायी) {
				पूर्णाङ्क len = sbi_snprintf(features_str + विस्थापनम्,
						       nfstr - विस्थापनम्,
						       "%s,", अस्थायी);
				यदि (len < 0)
					भङ्ग;

				यदि (विस्थापनम् + len >= nfstr) {
					/* No more space for features */
					विस्थापनम् = nfstr;
					भङ्ग;
				} अन्यथा
					विस्थापनम् = विस्थापनम् + len;
			}
		}
		feat = feat << 1;
	} यावत् (feat <= SBI_PLATFORM_HAS_चरमः_FEATURE);

समाप्तम्:
	यदि (विस्थापनम्)
		features_str[विस्थापनम् - 1] = '\0';
	अन्यथा
		sbi_strncpy(features_str, "none", nfstr);
}
