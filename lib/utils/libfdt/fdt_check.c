// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#अन्तर्भाव "libfdt_env.h"

#अन्तर्भाव <fdt.h>
#अन्तर्भाव <libfdt.h>

#अन्तर्भाव "libfdt_internal.h"

पूर्णाङ्क fdt_check_पूर्णम्(ध्रुव शून्य *fdt, size_t bufsize)
{
	पूर्णाङ्क err;
	पूर्णाङ्क अङ्कः_memrsv;
	पूर्णाङ्क विस्थापनम्, nextoffset = 0;
	uint32_t चिह्नकम्;
	अचिह्नित पूर्णाङ्क गभीरता = 0;
	ध्रुव शून्य *prop;
	ध्रुव वर्ण *propname;
	bool expect_अन्तः = false;

	यदि (bufsize < FDT_V1_आकार)
		प्रत्यागम -FDT_ERR_TRUNCATED;
	यदि (bufsize < fdt_शीर्षकम्_आकार_2(fdt))
		प्रत्यागम -FDT_ERR_TRUNCATED;
	err = fdt_check_शीर्षकम्(fdt);
	यदि (err != 0)
		प्रत्यागम err;
	यदि (bufsize < fdt_totalsize(fdt))
		प्रत्यागम -FDT_ERR_TRUNCATED;

	अङ्कः_memrsv = fdt_अङ्कः_mem_rsv(fdt);
	यदि (अङ्कः_memrsv < 0)
		प्रत्यागम अङ्कः_memrsv;

	यावत् (1) {
		विस्थापनम् = nextoffset;
		चिह्नकम् = fdt_अग्र_चिह्नकम्(fdt, विस्थापनम्, &nextoffset);

		यदि (nextoffset < 0)
			प्रत्यागम nextoffset;

		/* If we see two root nodes, something is wrong */
		यदि (expect_अन्तः && चिह्नकम् != FDT_अन्तः)
			प्रत्यागम -FDT_ERR_BADSTRUCTURE;

		विकल्प (चिह्नकम्) {
		पक्ष FDT_NOP:
			भङ्ग;

		पक्ष FDT_अन्तः:
			यदि (गभीरता != 0)
				प्रत्यागम -FDT_ERR_BADSTRUCTURE;
			प्रत्यागम 0;

		पक्ष FDT_BEGIN_ग्रन्थिः:
			गभीरता++;
			यदि (गभीरता > अधिकतम_पूर्णाङ्कः)
				प्रत्यागम -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			यदि (गभीरता == 1) {
				ध्रुव वर्ण *name;
				पूर्णाङ्क len;

				name = fdt_get_name(fdt, विस्थापनम्, &len);
				यदि (*name || len)
					प्रत्यागम -FDT_ERR_BADSTRUCTURE;
			}
			भङ्ग;

		पक्ष FDT_अन्तः_ग्रन्थिः:
			यदि (गभीरता == 0)
				प्रत्यागम -FDT_ERR_BADSTRUCTURE;
			गभीरता--;
			यदि (गभीरता == 0)
				expect_अन्तः = true;
			भङ्ग;

		पक्ष FDT_PROP:
			prop = fdt_getprop_by_विस्थापनम्(fdt, विस्थापनम्, &propname,
						     &err);
			यदि (!prop)
				प्रत्यागम err;
			भङ्ग;

		स्वभाव:
			प्रत्यागम -FDT_ERR_आन्तरिकम्;
		}
	}
}
