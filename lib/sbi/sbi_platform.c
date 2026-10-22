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

स्थिर ओळीत अक्षर *sbi_platform_feature_id2string(अचिन्हित दीर्घ feature)
{
	अक्षर *fstr = रिक्त;

	जर (!feature)
		परत रिक्त;

	निवड (feature) {
	स्थिती SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		थांबा;
	पूर्वनिर्धारित:
		थांबा;
	}

	परत fstr;
}

शून्य sbi_platform_get_features_str(स्थिरांक संरचना sbi_platform *plat,
				   अक्षर *features_str, पूर्णांक nfstr)
{
	अचिन्हित दीर्घ features, feat = 1UL;
	अक्षर *temp;
	पूर्णांक offset = 0;

	जर (!plat || !features_str || !nfstr)
		परत;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	जर (!features)
		जा done;

	करा {
		जर (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			जर (temp) {
				पूर्णांक len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				जर (len < 0)
					थांबा;

				जर (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					थांबा;
				} अन्यथा
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} पर्यंत (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	जर (offset)
		features_str[offset - 1] = '\0';
	अन्यथा
		sbi_strncpy(features_str, "none", nfstr);
}
