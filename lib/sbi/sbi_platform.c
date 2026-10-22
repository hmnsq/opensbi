/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_platform.h>
#समावेश <sbi/sbi_string.h>

अटल अंतरभूत अक्षर *sbi_platform_feature_id2string(अचिन्हित दीर्घ feature)
{
	अक्षर *fstr = शून्य;

	अगर (!feature)
		वापस शून्य;

	चयन (feature) {
	हाल SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		अवरोध;
	शेष:
		अवरोध;
	}

	वापस fstr;
}

व्योम sbi_platform_get_features_str(स्थिर काष्ठा sbi_platform *plat,
				   अक्षर *features_str, पूर्णांक nfstr)
{
	अचिन्हित दीर्घ features, feat = 1UL;
	अक्षर *अस्थायी;
	पूर्णांक offset = 0;

	अगर (!plat || !features_str || !nfstr)
		वापस;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	अगर (!features)
		जाओ done;

	करो {
		अगर (features & feat) {
			अस्थायी = sbi_platform_feature_id2string(feat);
			अगर (अस्थायी) {
				पूर्णांक len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", अस्थायी);
				अगर (len < 0)
					अवरोध;

				अगर (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					अवरोध;
				} अन्यथा
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} जबतक (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	अगर (offset)
		features_str[offset - 1] = '\0';
	अन्यथा
		sbi_strncpy(features_str, "none", nfstr);
}
