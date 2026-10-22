/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_platform.h>
#அந்தர்பாவ <sbi/sbi_string.h>

நிலையான உள்வரி எழுத்து *sbi_platform_feature_id2string(unsigned long feature)
{
	எழுத்து *fstr = வெற்று;

	எனில் (!feature)
		திரும்பு வெற்று;

	தேர்வு (feature) {
	நிலை SBI_PLATFORM_HAS_MFAULTS_DELEGATION:
		fstr = "medeleg";
		நிறுத்து;
	இயல்பு:
		நிறுத்து;
	}

	திரும்பு fstr;
}

வெறுமை sbi_platform_get_features_str(மாறிலி கட்டமைப்பு sbi_platform *plat,
				   எழுத்து *features_str, முழு nfstr)
{
	unsigned long features, feat = 1UL;
	எழுத்து *temp;
	முழு offset = 0;

	எனில் (!plat || !features_str || !nfstr)
		திரும்பு;
	sbi_memset(features_str, 0, nfstr);

	features = sbi_platform_get_features(plat);
	எனில் (!features)
		செல் done;

	செய் {
		எனில் (features & feat) {
			temp = sbi_platform_feature_id2string(feat);
			எனில் (temp) {
				முழு len = sbi_snprintf(features_str + offset,
						       nfstr - offset,
						       "%s,", temp);
				எனில் (len < 0)
					நிறுத்து;

				எனில் (offset + len >= nfstr) {
					/* No more space for features */
					offset = nfstr;
					நிறுத்து;
				} இல்லையெனில்
					offset = offset + len;
			}
		}
		feat = feat << 1;
	} போது (feat <= SBI_PLATFORM_HAS_LAST_FEATURE);

done:
	எனில் (offset)
		features_str[offset - 1] = '\0';
	இல்லையெனில்
		sbi_strncpy(features_str, "none", nfstr);
}
