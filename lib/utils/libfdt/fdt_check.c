// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#അന്തര്ഭാവ "libfdt_env.h"

#അന്തര്ഭാവ <fdt.h>
#അന്തര്ഭാവ <libfdt.h>

#അന്തര്ഭാവ "libfdt_internal.h"

പൂർണ്ണസംഖ്യ fdt_check_full(സ്ഥിരം ശൂന്യം *fdt, size_t bufsize)
{
	പൂർണ്ണസംഖ്യ err;
	പൂർണ്ണസംഖ്യ അക്കം_memrsv;
	പൂർണ്ണസംഖ്യ offset, nextoffset = 0;
	uint32_t tag;
	unsigned പൂർണ്ണസംഖ്യ depth = 0;
	സ്ഥിരം ശൂന്യം *prop;
	സ്ഥിരം അക്ഷരം *propname;
	bool expect_end = false;

	എങ്കിൽ (bufsize < FDT_V1_SIZE)
		തിരികെ -FDT_ERR_TRUNCATED;
	എങ്കിൽ (bufsize < fdt_header_size(fdt))
		തിരികെ -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	എങ്കിൽ (err != 0)
		തിരികെ err;
	എങ്കിൽ (bufsize < fdt_totalsize(fdt))
		തിരികെ -FDT_ERR_TRUNCATED;

	അക്കം_memrsv = fdt_അക്കം_mem_rsv(fdt);
	എങ്കിൽ (അക്കം_memrsv < 0)
		തിരികെ അക്കം_memrsv;

	ആകുമ്പോൾ (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		എങ്കിൽ (nextoffset < 0)
			തിരികെ nextoffset;

		/* If we see two root nodes, something is wrong */
		എങ്കിൽ (expect_end && tag != FDT_END)
			തിരികെ -FDT_ERR_BADSTRUCTURE;

		തിരഞ്ഞെടുപ്പ് (tag) {
		അവസ്ഥ FDT_NOP:
			നിർത്തുക;

		അവസ്ഥ FDT_END:
			എങ്കിൽ (depth != 0)
				തിരികെ -FDT_ERR_BADSTRUCTURE;
			തിരികെ 0;

		അവസ്ഥ FDT_BEGIN_കണ്ണി:
			depth++;
			എങ്കിൽ (depth > INT_MAX)
				തിരികെ -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			എങ്കിൽ (depth == 1) {
				സ്ഥിരം അക്ഷരം *name;
				പൂർണ്ണസംഖ്യ len;

				name = fdt_get_name(fdt, offset, &len);
				എങ്കിൽ (*name || len)
					തിരികെ -FDT_ERR_BADSTRUCTURE;
			}
			നിർത്തുക;

		അവസ്ഥ FDT_END_കണ്ണി:
			എങ്കിൽ (depth == 0)
				തിരികെ -FDT_ERR_BADSTRUCTURE;
			depth--;
			എങ്കിൽ (depth == 0)
				expect_end = true;
			നിർത്തുക;

		അവസ്ഥ FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			എങ്കിൽ (!prop)
				തിരികെ err;
			നിർത്തുക;

		സ്ഥിരസ്ഥിതി:
			തിരികെ -FDT_ERR_INTERNAL;
		}
	}
}
