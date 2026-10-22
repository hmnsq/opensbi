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

	सचे (!feature)
		पटिनिवत्त रिक्त;

	विकल्प (feature) {
	पक्ष SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		भङ्ग;
	स्वभाव:
		भङ्ग;
	}

	पटिनिवत्त fstr;
}

शून्य sbi_platform_get_features_str(ध्रुव संरचना sbi_platform *plat,
				   वर्ण *features_str, पूर्णाङ्क nfstr)
{
	unsigned long features, feat = 1UL;
	वर्ण *temp;
	पूर्णाङ्क offset = 0;

	सचे (!plat || !features_str || !nfstr)
		पटिनिवत्त;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	सचे (!features)
		गच्छ done;

	करोहि {
		सचे (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			सचे (temp) {
				पूर्णाङ्क len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				सचे (len < 0)
					भङ्ग;

				सचे (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					भङ्ग;
				} अञ्ञथा
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} याव (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	सचे (offset)
		features_str[offset - 1] = '\0';
	अञ्ञथा
		sbi_strncpy(features_str, "none", nfstr);
}
