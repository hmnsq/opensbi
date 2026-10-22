// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ಅನ್ತರ್ಭಾವ "libfdt_env.h"

#ಅನ್ತರ್ಭಾವ <fdt.h>
#ಅನ್ತರ್ಭಾವ <libfdt.h>

#ಅನ್ತರ್ಭಾವ "libfdt_internal.h"

/*
 * Minimal sanity check for a read-only tree. fdt_ro_probe_() checks
 * that the given buffer contains what appears to be a flattened
 * device tree with sane information in its header.
 */
int32_t fdt_ro_probe_(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt)
{
	uint32_t totalsize = fdt_totalsize(fdt);

	ಆದರೆ (can_assume(VALID_DTB))
		ಹಿಂತಿರುಗಿ totalsize;

	/* The device tree must be at an 8-byte aligned address */
	ಆದರೆ ((uintptr_t)fdt & 7)
		ಹಿಂತಿರುಗಿ -FDT_ERR_ALIGNMENT;

	ಆದರೆ (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		ಆದರೆ (!can_assume(LATEST)) {
			ಆದರೆ (fdt_ಆವೃತ್ತಿ(fdt) < FDT_ಮೊದಲ_SUPPORTED_ಆವೃತ್ತಿ)
				ಹಿಂತಿರುಗಿ -FDT_ERR_BADVERSION;
			ಆದರೆ (fdt_ಕೊನೆಯ_comp_ಆವೃತ್ತಿ(fdt) >
					FDT_ಕೊನೆಯ_SUPPORTED_ಆವೃತ್ತಿ)
				ಹಿಂತಿರುಗಿ -FDT_ERR_BADVERSION;
		}
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		ಆದರೆ (!can_assume(VALID_ಒಳಹರಿವು) && fdt_ಗಾತ್ರ_dt_struct(fdt) == 0)
			ಹಿಂತಿರುಗಿ -FDT_ERR_BADSTATE;
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಹಿಂತಿರುಗಿ -FDT_ERR_BADMAGIC;
	}

	ಆದರೆ (totalsize < INT32_MAX)
		ಹಿಂತಿರುಗಿ totalsize;
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ -FDT_ERR_TRUNCATED;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	ಹಿಂತಿರುಗಿ (off >= hdrsize) && (off <= totalsize);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ check_ಖಂಡ(uint32_t hdrsize, uint32_t totalsize,
			uint32_t ತಳ_2, uint32_t ಗಾತ್ರ_2)
{
	ಆದರೆ (!check_off_(hdrsize, totalsize, ತಳ_2))
		ಹಿಂತಿರುಗಿ 0; /* block start out of bounds */
	ಆದರೆ ((ತಳ_2 + ಗಾತ್ರ_2) < ತಳ_2)
		ಹಿಂತಿರುಗಿ 0; /*ಅತಿಪ್ರವಾಹ*/
	ಆದರೆ (!check_off_(hdrsize, totalsize, ತಳ_2 + ಗಾತ್ರ_2))
		ಹಿಂತಿರುಗಿ 0; /* block end out of bounds */
	ಹಿಂತಿರುಗಿ 1;
}

size_t fdt_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ(uint32_t ಆವೃತ್ತಿ)
{
	ಆದರೆ (ಆವೃತ್ತಿ <= 1)
		ಹಿಂತಿರುಗಿ FDT_V1_ಗಾತ್ರ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಆವೃತ್ತಿ <= 2)
		ಹಿಂತಿರುಗಿ FDT_V2_ಗಾತ್ರ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಆವೃತ್ತಿ <= 3)
		ಹಿಂತಿರುಗಿ FDT_V3_ಗಾತ್ರ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಆವೃತ್ತಿ <= 16)
		ಹಿಂತಿರುಗಿ FDT_V16_ಗಾತ್ರ;
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ FDT_V17_ಗಾತ್ರ;
}

size_t fdt_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ_2(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt)
{
	ಹಿಂತಿರುಗಿ can_assume(LATEST) ? FDT_V17_ಗಾತ್ರ :
		fdt_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ(fdt_ಆವೃತ್ತಿ(fdt));
}

ಪೂರ್ಣಾಂಕ fdt_check_ಶೀರ್ಷಿಕೆ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	ಆದರೆ ((uintptr_t)fdt & 7)
		ಹಿಂತಿರುಗಿ -FDT_ERR_ALIGNMENT;

	ಆದರೆ (fdt_magic(fdt) != FDT_MAGIC)
		ಹಿಂತಿರುಗಿ -FDT_ERR_BADMAGIC;
	ಆದರೆ (!can_assume(LATEST)) {
		ಆದರೆ ((fdt_ಆವೃತ್ತಿ(fdt) < FDT_ಮೊದಲ_SUPPORTED_ಆವೃತ್ತಿ)
		    || (fdt_ಕೊನೆಯ_comp_ಆವೃತ್ತಿ(fdt) >
			FDT_ಕೊನೆಯ_SUPPORTED_ಆವೃತ್ತಿ))
			ಹಿಂತಿರುಗಿ -FDT_ERR_BADVERSION;
		ಆದರೆ (fdt_ಆವೃತ್ತಿ(fdt) < fdt_ಕೊನೆಯ_comp_ಆವೃತ್ತಿ(fdt))
			ಹಿಂತಿರುಗಿ -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ_2(fdt);
	ಆದರೆ (!can_assume(VALID_DTB)) {

		ಆದರೆ ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ))
			ಹಿಂತಿರುಗಿ -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		ಆದರೆ (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			ಹಿಂತಿರುಗಿ -FDT_ERR_TRUNCATED;
	}

	ಆದರೆ (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		ಆದರೆ (!can_assume(LATEST) && fdt_ಆವೃತ್ತಿ(fdt) < 17) {
			ಆದರೆ (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				ಹಿಂತಿರುಗಿ -FDT_ERR_TRUNCATED;
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಆದರೆ (!check_ಖಂಡ(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_ಗಾತ್ರ_dt_struct(fdt)))
				ಹಿಂತಿರುಗಿ -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		ಆದರೆ (!check_ಖಂಡ(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt),
				  fdt_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt)))
			ಹಿಂತಿರುಗಿ -FDT_ERR_TRUNCATED;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt_ಸ್ಥಾನಾಂತರ_ptr(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ len)
{
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ uoffset = ಸ್ಥಾನಾಂತರ;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ absoffset = ಸ್ಥಾನಾಂತರ + fdt_off_dt_struct(fdt);

	ಆದರೆ (ಸ್ಥಾನಾಂತರ < 0)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಆದರೆ (!can_assume(VALID_ಒಳಹರಿವು))
		ಆದರೆ ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಆದರೆ (can_assume(LATEST) || fdt_ಆವೃತ್ತಿ(fdt) >= 0x11)
		ಆದರೆ (((uoffset + len) < uoffset)
		    || ((ಸ್ಥಾನಾಂತರ + len) > fdt_ಗಾತ್ರ_dt_struct(fdt)))
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಹಿಂತಿರುಗಿ fdt_ಸ್ಥಾನಾಂತರ_ptr_2(fdt, ಸ್ಥಾನಾಂತರ);
}

uint32_t fdt_ಮುಂದಿನ_ಟ್ಯಾಗ್(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ startoffset, ಪೂರ್ಣಾಂಕ *nextoffset)
{
	ಸ್ಥಿರಾಂಕ fdt32_t *tagp, *lenp;
	uint32_t ಟ್ಯಾಗ್;
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = startoffset;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_ಸ್ಥಾನಾಂತರ_ptr(fdt, ಸ್ಥಾನಾಂತರ, FDT_TAGSIZE);
	ಆದರೆ (!can_assume(VALID_DTB) && !tagp)
		ಹಿಂತಿರುಗಿ FDT_ಕೊನೆ; /* premature end */
	ಟ್ಯಾಗ್ = fdt32_ಗಮ್ಯ_cpu(*tagp);
	ಸ್ಥಾನಾಂತರ += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	ಆಯ್ಕೆ (ಟ್ಯಾಗ್) {
	ಸ್ಥಿತಿ FDT_BEGIN_ಗಂಟು:
		/* skip name */
		ಮಾಡು {
			p = fdt_ಸ್ಥಾನಾಂತರ_ptr(fdt, ಸ್ಥಾನಾಂತರ++, 1);
		} ಆಗ (p && (*p != '\0'));
		ಆದರೆ (!can_assume(VALID_DTB) && !p)
			ಹಿಂತಿರುಗಿ FDT_ಕೊನೆ; /* premature end */
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ FDT_PROP:
		lenp = fdt_ಸ್ಥಾನಾಂತರ_ptr(fdt, ಸ್ಥಾನಾಂತರ, ಗಾತ್ರ(*lenp));
		ಆದರೆ (!can_assume(VALID_DTB) && !lenp)
			ಹಿಂತಿರುಗಿ FDT_ಕೊನೆ; /* premature end */
		/* skip-name offset, length and value */
		ಸ್ಥಾನಾಂತರ += ಗಾತ್ರ(ರಚನೆ fdt_property) - FDT_TAGSIZE
			+ fdt32_ಗಮ್ಯ_cpu(*lenp);
		ಆದರೆ (!can_assume(LATEST) &&
		    fdt_ಆವೃತ್ತಿ(fdt) < 0x10 && fdt32_ಗಮ್ಯ_cpu(*lenp) >= 8 &&
		    ((ಸ್ಥಾನಾಂತರ - fdt32_ಗಮ್ಯ_cpu(*lenp)) % 8) != 0)
			ಸ್ಥಾನಾಂತರ += 4;
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ FDT_ಕೊನೆ:
	ಸ್ಥಿತಿ FDT_ಕೊನೆ_ಗಂಟು:
	ಸ್ಥಿತಿ FDT_NOP:
		ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
		ಹಿಂತಿರುಗಿ FDT_ಕೊನೆ;
	}

	ಆದರೆ (!fdt_ಸ್ಥಾನಾಂತರ_ptr(fdt, startoffset, ಸ್ಥಾನಾಂತರ - startoffset))
		ಹಿಂತಿರುಗಿ FDT_ಕೊನೆ; /* premature end */

	*nextoffset = FDT_TAGALIGN(ಸ್ಥಾನಾಂತರ);
	ಹಿಂತಿರುಗಿ ಟ್ಯಾಗ್;
}

ಪೂರ್ಣಾಂಕ fdt_check_ಗಂಟು_ಸ್ಥಾನಾಂತರ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ)
{
	ಆದರೆ (!can_assume(VALID_ಒಳಹರಿವು)
	    && ((ಸ್ಥಾನಾಂತರ < 0) || (ಸ್ಥಾನಾಂತರ % FDT_TAGSIZE)))
		ಹಿಂತಿರುಗಿ -FDT_ERR_BADOFFSET;

	ಆದರೆ (fdt_ಮುಂದಿನ_ಟ್ಯಾಗ್(fdt, ಸ್ಥಾನಾಂತರ, &ಸ್ಥಾನಾಂತರ) != FDT_BEGIN_ಗಂಟು)
		ಹಿಂತಿರುಗಿ -FDT_ERR_BADOFFSET;

	ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;
}

ಪೂರ್ಣಾಂಕ fdt_check_prop_ಸ್ಥಾನಾಂತರ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ)
{
	ಆದರೆ (!can_assume(VALID_ಒಳಹರಿವು)
	    && ((ಸ್ಥಾನಾಂತರ < 0) || (ಸ್ಥಾನಾಂತರ % FDT_TAGSIZE)))
		ಹಿಂತಿರುಗಿ -FDT_ERR_BADOFFSET;

	ಆದರೆ (fdt_ಮುಂದಿನ_ಟ್ಯಾಗ್(fdt, ಸ್ಥಾನಾಂತರ, &ಸ್ಥಾನಾಂತರ) != FDT_PROP)
		ಹಿಂತಿರುಗಿ -FDT_ERR_BADOFFSET;

	ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;
}

ಪೂರ್ಣಾಂಕ fdt_ಮುಂದಿನ_ಗಂಟು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ *ಆಳ)
{
	ಪೂರ್ಣಾಂಕ nextoffset = 0;
	uint32_t ಟ್ಯಾಗ್;

	ಆದರೆ (ಸ್ಥಾನಾಂತರ >= 0)
		ಆದರೆ ((nextoffset = fdt_check_ಗಂಟು_ಸ್ಥಾನಾಂತರ(fdt, ಸ್ಥಾನಾಂತರ)) < 0)
			ಹಿಂತಿರುಗಿ nextoffset;

	ಮಾಡು {
		ಸ್ಥಾನಾಂತರ = nextoffset;
		ಟ್ಯಾಗ್ = fdt_ಮುಂದಿನ_ಟ್ಯಾಗ್(fdt, ಸ್ಥಾನಾಂತರ, &nextoffset);

		ಆಯ್ಕೆ (ಟ್ಯಾಗ್) {
		ಸ್ಥಿತಿ FDT_PROP:
		ಸ್ಥಿತಿ FDT_NOP:
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ FDT_BEGIN_ಗಂಟು:
			ಆದರೆ (ಆಳ)
				(*ಆಳ)++;
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ FDT_ಕೊನೆ_ಗಂಟು:
			ಆದರೆ (ಆಳ && ((--(*ಆಳ)) < 0))
				ಹಿಂತಿರುಗಿ nextoffset;
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ FDT_ಕೊನೆ:
			ಆದರೆ ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !ಆಳ))
				ಹಿಂತಿರುಗಿ -FDT_ERR_NOTFOUND;
			ಇಲ್ಲದಿದ್ದರೆ
				ಹಿಂತಿರುಗಿ nextoffset;
		}
	} ಆಗ (ಟ್ಯಾಗ್ != FDT_BEGIN_ಗಂಟು);

	ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;
}

ಪೂರ್ಣಾಂಕ fdt_ಮೊದಲ_subnode(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ)
{
	ಪೂರ್ಣಾಂಕ ಆಳ = 0;

	ಸ್ಥಾನಾಂತರ = fdt_ಮುಂದಿನ_ಗಂಟು(fdt, ಸ್ಥಾನಾಂತರ, &ಆಳ);
	ಆದರೆ (ಸ್ಥಾನಾಂತರ < 0 || ಆಳ != 1)
		ಹಿಂತಿರುಗಿ -FDT_ERR_NOTFOUND;

	ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;
}

ಪೂರ್ಣಾಂಕ fdt_ಮುಂದಿನ_subnode(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ)
{
	ಪೂರ್ಣಾಂಕ ಆಳ = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	ಮಾಡು {
		ಸ್ಥಾನಾಂತರ = fdt_ಮುಂದಿನ_ಗಂಟು(fdt, ಸ್ಥಾನಾಂತರ, &ಆಳ);
		ಆದರೆ (ಸ್ಥಾನಾಂತರ < 0 || ಆಳ < 1)
			ಹಿಂತಿರುಗಿ -FDT_ERR_NOTFOUND;
	} ಆಗ (ಆಳ > 1);

	ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fdt_ಕಂಡುಹಿಡಿ_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *strtab, ಪೂರ್ಣಾಂಕ tabsize, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s)
{
	ಪೂರ್ಣಾಂಕ len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s) + 1;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆಯ = strtab + tabsize - len;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;

	ಗಾಗಿ (p = strtab; p <= ಕೊನೆಯ; p++)
		ಆದರೆ (memcmp(p, s, len) == 0)
			ಹಿಂತಿರುಗಿ p;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಪೂರ್ಣಾಂಕ fdt_move(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಶೂನ್ಯ *buf, ಪೂರ್ಣಾಂಕ bufsize)
{
	ಆದರೆ (!can_assume(VALID_ಒಳಹರಿವು) && bufsize < 0)
		ಹಿಂತಿರುಗಿ -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	ಆದರೆ (fdt_totalsize(fdt) > (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)bufsize)
		ಹಿಂತಿರುಗಿ -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	ಹಿಂತಿರುಗಿ 0;
}
