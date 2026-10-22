/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_string.h>

ساکن اندرونی حرف *sbi_platform_feature_id2string(غیرعلامتی طویل feature)
{
	حرف *fstr = تہی;

	اگر (!feature)
		واپس تہی;

	انتخاب (feature) {
	حالت SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		توڑو;
	طےشدہ:
		توڑو;
	}

	واپس fstr;
}

خالی sbi_platform_get_features_str(ثابت ساخت sbi_platform *plat,
				   حرف *features_str, صحیح nfstr)
{
	غیرعلامتی طویل features, feat = 1UL;
	حرف *عارضی;
	صحیح انحراف = 0;

	اگر (!plat || !features_str || !nfstr)
		واپس;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	اگر (!features)
		جاؤ مکمل;

	کرو {
		اگر (features & feat) {
			عارضی = sbi_platform_feature_id2string(feat);
			اگر (عارضی) {
				صحیح len = sbi_snprintf(features_str + انحراف,
						       nfstr - انحراف,
						       "%s,", عارضی);
				اگر (len < 0)
					توڑو;

				اگر (انحراف + len >= nfstr) {
					/* No more space for features */
					انحراف = nfstr;
					توڑو;
				} ورنہ
					انحراف = انحراف + len;
			}
		}
		feat = feat << 1;
	} جب (feat <= SBI_PLATFORM_HAS_آخری_FEATURE);

مکمل:
	اگر (انحراف)
		features_str[انحراف - 1] = '\0';
	ورنہ
		sbi_strncpy(features_str, "none", nfstr);
}
