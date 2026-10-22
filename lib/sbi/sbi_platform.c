/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_platform.h>
#അന്തര്ഭാവ <sbi/sbi_string.h>

സ്ഥിതം വരിയിൽ അക്ഷരം *sbi_platform_feature_id2string(unsigned long feature)
{
	അക്ഷരം *fstr = ശൂന്യ;

	എങ്കിൽ (!feature)
		തിരികെ ശൂന്യ;

	തിരഞ്ഞെടുപ്പ് (feature) {
	അവസ്ഥ SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
	}

	തിരികെ fstr;
}

ശൂന്യം sbi_platform_get_features_str(സ്ഥിരം ഘടന sbi_platform *plat,
				   അക്ഷരം *features_str, പൂർണ്ണസംഖ്യ nfstr)
{
	unsigned long features, feat = 1UL;
	അക്ഷരം *temp;
	പൂർണ്ണസംഖ്യ offset = 0;

	എങ്കിൽ (!plat || !features_str || !nfstr)
		തിരികെ;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	എങ്കിൽ (!features)
		പോകുക done;

	ചെയ്യുക {
		എങ്കിൽ (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			എങ്കിൽ (temp) {
				പൂർണ്ണസംഖ്യ len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				എങ്കിൽ (len < 0)
					നിർത്തുക;

				എങ്കിൽ (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					നിർത്തുക;
				} അല്ലെങ്കിൽ
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} ആകുമ്പോൾ (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	എങ്കിൽ (offset)
		features_str[offset - 1] = '\0';
	അല്ലെങ്കിൽ
		sbi_strncpy(features_str, "none", nfstr);
}
