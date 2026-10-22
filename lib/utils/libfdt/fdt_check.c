// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ಅನ್ತರ್ಭಾವ "libfdt_env.h"

#ಅನ್ತರ್ಭಾವ <fdt.h>
#ಅನ್ತರ್ಭಾವ <libfdt.h>

#ಅನ್ತರ್ಭಾವ "libfdt_internal.h"

ಪೂರ್ಣಾಙ್ಕ fdt_check_full(ಧ್ರುವ ಶೂನ್ಯ *fdt, size_t bufsize)
{
	ಪೂರ್ಣಾಙ್ಕ err;
	ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_memrsv;
	ಪೂರ್ಣಾಙ್ಕ offset, nextoffset = 0;
	uint32_t tag;
	unsigned ಪೂರ್ಣಾಙ್ಕ depth = 0;
	ಧ್ರುವ ಶೂನ್ಯ *prop;
	ಧ್ರುವ ವರ್ಣ *propname;
	bool expect_end = false;

	ಯದಿ (bufsize < FDT_V1_SIZE)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_TRUNCATED;
	ಯದಿ (bufsize < fdt_header_size(fdt))
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_TRUNCATED;
	err = fdt_check_header(fdt);
	ಯದಿ (err != 0)
		ಪ್ರತ್ಯಾಗಮ err;
	ಯದಿ (bufsize < fdt_totalsize(fdt))
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_TRUNCATED;

	ಅಂಕೆ_memrsv = fdt_ಅಂಕೆ_mem_rsv(fdt);
	ಯದಿ (ಅಂಕೆ_memrsv < 0)
		ಪ್ರತ್ಯಾಗಮ ಅಂಕೆ_memrsv;

	ಯಾವತ್ (1) {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		ಯದಿ (nextoffset < 0)
			ಪ್ರತ್ಯಾಗಮ nextoffset;

		/* If we see two root nodes, something is wrong */
		ಯದಿ (expect_end && tag != FDT_END)
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADSTRUCTURE;

		ವಿಕಲ್ಪ (tag) {
		ಪಕ್ಷ FDT_NOP:
			ಭಙ್ಗ;

		ಪಕ್ಷ FDT_END:
			ಯದಿ (depth != 0)
				ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADSTRUCTURE;
			ಪ್ರತ್ಯಾಗಮ 0;

		ಪಕ್ಷ FDT_BEGIN_ಗಂಟು:
			depth++;
			ಯದಿ (depth > INT_MAX)
				ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			ಯದಿ (depth == 1) {
				ಧ್ರುವ ವರ್ಣ *name;
				ಪೂರ್ಣಾಙ್ಕ len;

				name = fdt_get_name(fdt, offset, &len);
				ಯದಿ (*name || len)
					ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADSTRUCTURE;
			}
			ಭಙ್ಗ;

		ಪಕ್ಷ FDT_END_ಗಂಟು:
			ಯದಿ (depth == 0)
				ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADSTRUCTURE;
			depth--;
			ಯದಿ (depth == 0)
				expect_end = true;
			ಭಙ್ಗ;

		ಪಕ್ಷ FDT_PROP:
			prop = fdt_getprop_by_offset(fdt, offset, &propname,
						     &err);
			ಯದಿ (!prop)
				ಪ್ರತ್ಯಾಗಮ err;
			ಭಙ್ಗ;

		ಸ್ವಭಾವ:
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_INTERNAL;
		}
	}
}
