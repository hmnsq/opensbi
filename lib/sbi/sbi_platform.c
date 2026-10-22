/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_platform.h>
#અન્તર્ભાવ <sbi/sbi_string.h>

સ્થિર અંતઃરેખ અક્ષર *sbi_platform_feature_id2string(unsigned long feature)
{
	અક્ષર *fstr = ખાલી;

	જો (!feature)
		પાછા ખાલી;

	પસંદગી (feature) {
	સ્થિતિ SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		તોડો;
	મૂળભૂત:
		તોડો;
	}

	પાછા fstr;
}

શૂન્ય sbi_platform_get_features_str(અચલ સંરચના sbi_platform *plat,
				   અક્ષર *features_str, પૂર્ણાંક nfstr)
{
	unsigned long features, feat = 1UL;
	અક્ષર *temp;
	પૂર્ણાંક offset = 0;

	જો (!plat || !features_str || !nfstr)
		પાછા;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	જો (!features)
		જાઓ done;

	કરો {
		જો (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			જો (temp) {
				પૂર્ણાંક len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				જો (len < 0)
					તોડો;

				જો (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					તોડો;
				} નહીંતર
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} જ્યારે (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	જો (offset)
		features_str[offset - 1] = '\0';
	નહીંતર
		sbi_strncpy(features_str, "none", nfstr);
}
