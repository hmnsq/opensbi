// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ಅನ್ತರ್ಭಾವ "libfdt_env.h"

#ಅನ್ತರ್ಭಾವ <fdt.h>
#ಅನ್ತರ್ಭಾವ <libfdt.h>

#ಅನ್ತರ್ಭಾವ "libfdt_internal.h"

ಪೂರ್ಣಾಂಕ fdt_check_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, size_t bufsize)
{
	ಪೂರ್ಣಾಂಕ err;
	ಪೂರ್ಣಾಂಕ ಅಂಕೆ_memrsv;
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, nextoffset = 0;
	uint32_t ಟ್ಯಾಗ್;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಆಳ = 0;
	ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *prop;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *propname;
	bool expect_ಕೊನೆ = false;

	ಆದರೆ (bufsize < FDT_V1_ಗಾತ್ರ)
		ಹಿಂತಿರುಗಿ -FDT_ERR_TRUNCATED;
	ಆದರೆ (bufsize < fdt_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ_2(fdt))
		ಹಿಂತಿರುಗಿ -FDT_ERR_TRUNCATED;
	err = fdt_check_ಶೀರ್ಷಿಕೆ(fdt);
	ಆದರೆ (err != 0)
		ಹಿಂತಿರುಗಿ err;
	ಆದರೆ (bufsize < fdt_totalsize(fdt))
		ಹಿಂತಿರುಗಿ -FDT_ERR_TRUNCATED;

	ಅಂಕೆ_memrsv = fdt_ಅಂಕೆ_mem_rsv(fdt);
	ಆದರೆ (ಅಂಕೆ_memrsv < 0)
		ಹಿಂತಿರುಗಿ ಅಂಕೆ_memrsv;

	ಆಗ (1) {
		ಸ್ಥಾನಾಂತರ = nextoffset;
		ಟ್ಯಾಗ್ = fdt_ಮುಂದಿನ_ಟ್ಯಾಗ್(fdt, ಸ್ಥಾನಾಂತರ, &nextoffset);

		ಆದರೆ (nextoffset < 0)
			ಹಿಂತಿರುಗಿ nextoffset;

		/* If we see two root nodes, something is wrong */
		ಆದರೆ (expect_ಕೊನೆ && ಟ್ಯಾಗ್ != FDT_ಕೊನೆ)
			ಹಿಂತಿರುಗಿ -FDT_ERR_BADSTRUCTURE;

		ಆಯ್ಕೆ (ಟ್ಯಾಗ್) {
		ಸ್ಥಿತಿ FDT_NOP:
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ FDT_ಕೊನೆ:
			ಆದರೆ (ಆಳ != 0)
				ಹಿಂತಿರುಗಿ -FDT_ERR_BADSTRUCTURE;
			ಹಿಂತಿರುಗಿ 0;

		ಸ್ಥಿತಿ FDT_BEGIN_ಗಂಟು:
			ಆಳ++;
			ಆದರೆ (ಆಳ > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
				ಹಿಂತಿರುಗಿ -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			ಆದರೆ (ಆಳ == 1) {
				ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
				ಪೂರ್ಣಾಂಕ len;

				name = fdt_get_name(fdt, ಸ್ಥಾನಾಂತರ, &len);
				ಆದರೆ (*name || len)
					ಹಿಂತಿರುಗಿ -FDT_ERR_BADSTRUCTURE;
			}
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ FDT_ಕೊನೆ_ಗಂಟು:
			ಆದರೆ (ಆಳ == 0)
				ಹಿಂತಿರುಗಿ -FDT_ERR_BADSTRUCTURE;
			ಆಳ--;
			ಆದರೆ (ಆಳ == 0)
				expect_ಕೊನೆ = true;
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ FDT_PROP:
			prop = fdt_getprop_by_ಸ್ಥಾನಾಂತರ(fdt, ಸ್ಥಾನಾಂತರ, &propname,
						     &err);
			ಆದರೆ (!prop)
				ಹಿಂತಿರುಗಿ err;
			ನಿಲ್ಲಿಸು;

		ಪೂರ್ವನಿಯೋಜಿತ:
			ಹಿಂತಿರುಗಿ -FDT_ERR_ಆಂತರಿಕ;
		}
	}
}
