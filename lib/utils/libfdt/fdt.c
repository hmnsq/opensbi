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
int32_t fdt_ro_probe_(ಧ್ರುವ ಶೂನ್ಯ *fdt)
{
	uint32_t totalsize = fdt_totalsize(fdt);

	ಯದಿ (can_assume(VALID_DTB))
		ಪ್ರತ್ಯಾಗಮ totalsize;

	/* The device tree must be at an 8-byte aligned address */
	ಯದಿ ((uintptr_t)fdt & 7)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_ALIGNMENT;

	ಯದಿ (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		ಯದಿ (!can_assume(LATEST)) {
			ಯದಿ (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADVERSION;
			ಯದಿ (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADVERSION;
		}
	} ಅನ್ಯಥಾ ಯದಿ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		ಯದಿ (!can_assume(VALID_ಒಳಹರಿವು) && fdt_size_dt_struct(fdt) == 0)
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADSTATE;
	} ಅನ್ಯಥಾ {
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADMAGIC;
	}

	ಯದಿ (totalsize < INT32_MAX)
		ಪ್ರತ್ಯಾಗಮ totalsize;
	ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_TRUNCATED;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	ಪ್ರತ್ಯಾಗಮ (off >= hdrsize) && (off <= totalsize);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	ಯದಿ (!check_off_(hdrsize, totalsize, base))
		ಪ್ರತ್ಯಾಗಮ 0; /* block start out of bounds */
	ಯದಿ ((base + size) < base)
		ಪ್ರತ್ಯಾಗಮ 0; /* overflow */
	ಯದಿ (!check_off_(hdrsize, totalsize, base + size))
		ಪ್ರತ್ಯಾಗಮ 0; /* block end out of bounds */
	ಪ್ರತ್ಯಾಗಮ 1;
}

size_t fdt_header_size_(uint32_t version)
{
	ಯದಿ (version <= 1)
		ಪ್ರತ್ಯಾಗಮ FDT_V1_SIZE;
	ಅನ್ಯಥಾ ಯದಿ (version <= 2)
		ಪ್ರತ್ಯಾಗಮ FDT_V2_SIZE;
	ಅನ್ಯಥಾ ಯದಿ (version <= 3)
		ಪ್ರತ್ಯಾಗಮ FDT_V3_SIZE;
	ಅನ್ಯಥಾ ಯದಿ (version <= 16)
		ಪ್ರತ್ಯಾಗಮ FDT_V16_SIZE;
	ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ FDT_V17_SIZE;
}

size_t fdt_header_size(ಧ್ರುವ ಶೂನ್ಯ *fdt)
{
	ಪ್ರತ್ಯಾಗಮ can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

ಪೂರ್ಣಾಙ್ಕ fdt_check_header(ಧ್ರುವ ಶೂನ್ಯ *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	ಯದಿ ((uintptr_t)fdt & 7)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_ALIGNMENT;

	ಯದಿ (fdt_magic(fdt) != FDT_MAGIC)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADMAGIC;
	ಯದಿ (!can_assume(LATEST)) {
		ಯದಿ ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADVERSION;
		ಯದಿ (fdt_version(fdt) < fdt_last_comp_version(fdt))
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	ಯದಿ (!can_assume(VALID_DTB)) {

		ಯದಿ ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		ಯದಿ (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_TRUNCATED;
	}

	ಯದಿ (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		ಯದಿ (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			ಯದಿ (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				ಪ್ರತ್ಯಾಗಮ -FDT_ERR_TRUNCATED;
		} ಅನ್ಯಥಾ {
			ಯದಿ (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				ಪ್ರತ್ಯಾಗಮ -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		ಯದಿ (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_TRUNCATED;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಧ್ರುವ ಶೂನ್ಯ *fdt_offset_ptr(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ offset, unsigned ಪೂರ್ಣಾಙ್ಕ len)
{
	unsigned ಪೂರ್ಣಾಙ್ಕ uoffset = offset;
	unsigned ಪೂರ್ಣಾಙ್ಕ absoffset = offset + fdt_off_dt_struct(fdt);

	ಯದಿ (offset < 0)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ಯದಿ (!can_assume(VALID_ಒಳಹರಿವು))
		ಯದಿ ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ಯದಿ (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		ಯದಿ (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ಪ್ರತ್ಯಾಗಮ fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ startoffset, ಪೂರ್ಣಾಙ್ಕ *nextoffset)
{
	ಧ್ರುವ fdt32_t *tagp, *lenp;
	uint32_t tag;
	ಪೂರ್ಣಾಙ್ಕ offset = startoffset;
	ಧ್ರುವ ವರ್ಣ *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	ಯದಿ (!can_assume(VALID_DTB) && !tagp)
		ಪ್ರತ್ಯಾಗಮ FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	ವಿಕಲ್ಪ (tag) {
	ಪಕ್ಷ FDT_BEGIN_ಗಂಟು:
		/* skip name */
		ಕುರು {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} ಯಾವತ್ (p && (*p != '\0'));
		ಯದಿ (!can_assume(VALID_DTB) && !p)
			ಪ್ರತ್ಯಾಗಮ FDT_END; /* premature end */
		ಭಙ್ಗ;

	ಪಕ್ಷ FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, ಪರಿಮಾಣ(*lenp));
		ಯದಿ (!can_assume(VALID_DTB) && !lenp)
			ಪ್ರತ್ಯಾಗಮ FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += ಪರಿಮಾಣ(ಸಂರಚನಾ fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		ಯದಿ (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		ಭಙ್ಗ;

	ಪಕ್ಷ FDT_END:
	ಪಕ್ಷ FDT_END_ಗಂಟು:
	ಪಕ್ಷ FDT_NOP:
		ಭಙ್ಗ;

	ಸ್ವಭಾವ:
		ಪ್ರತ್ಯಾಗಮ FDT_END;
	}

	ಯದಿ (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		ಪ್ರತ್ಯಾಗಮ FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	ಪ್ರತ್ಯಾಗಮ tag;
}

ಪೂರ್ಣಾಙ್ಕ fdt_check_ಗಂಟು_offset(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ offset)
{
	ಯದಿ (!can_assume(VALID_ಒಳಹರಿವು)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADOFFSET;

	ಯದಿ (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_ಗಂಟು)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADOFFSET;

	ಪ್ರತ್ಯಾಗಮ offset;
}

ಪೂರ್ಣಾಙ್ಕ fdt_check_prop_offset_(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ offset)
{
	ಯದಿ (!can_assume(VALID_ಒಳಹರಿವು)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADOFFSET;

	ಯದಿ (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADOFFSET;

	ಪ್ರತ್ಯಾಗಮ offset;
}

ಪೂರ್ಣಾಙ್ಕ fdt_next_ಗಂಟು(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ offset, ಪೂರ್ಣಾಙ್ಕ *depth)
{
	ಪೂರ್ಣಾಙ್ಕ nextoffset = 0;
	uint32_t tag;

	ಯದಿ (offset >= 0)
		ಯದಿ ((nextoffset = fdt_check_ಗಂಟು_offset(fdt, offset)) < 0)
			ಪ್ರತ್ಯಾಗಮ nextoffset;

	ಕುರು {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		ವಿಕಲ್ಪ (tag) {
		ಪಕ್ಷ FDT_PROP:
		ಪಕ್ಷ FDT_NOP:
			ಭಙ್ಗ;

		ಪಕ್ಷ FDT_BEGIN_ಗಂಟು:
			ಯದಿ (depth)
				(*depth)++;
			ಭಙ್ಗ;

		ಪಕ್ಷ FDT_END_ಗಂಟು:
			ಯದಿ (depth && ((--(*depth)) < 0))
				ಪ್ರತ್ಯಾಗಮ nextoffset;
			ಭಙ್ಗ;

		ಪಕ್ಷ FDT_END:
			ಯದಿ ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOTFOUND;
			ಅನ್ಯಥಾ
				ಪ್ರತ್ಯಾಗಮ nextoffset;
		}
	} ಯಾವತ್ (tag != FDT_BEGIN_ಗಂಟು);

	ಪ್ರತ್ಯಾಗಮ offset;
}

ಪೂರ್ಣಾಙ್ಕ fdt_first_subnode(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ offset)
{
	ಪೂರ್ಣಾಙ್ಕ depth = 0;

	offset = fdt_next_ಗಂಟು(fdt, offset, &depth);
	ಯದಿ (offset < 0 || depth != 1)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOTFOUND;

	ಪ್ರತ್ಯಾಗಮ offset;
}

ಪೂರ್ಣಾಙ್ಕ fdt_next_subnode(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ offset)
{
	ಪೂರ್ಣಾಙ್ಕ depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	ಕುರು {
		offset = fdt_next_ಗಂಟು(fdt, offset, &depth);
		ಯದಿ (offset < 0 || depth < 1)
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOTFOUND;
	} ಯಾವತ್ (depth > 1);

	ಪ್ರತ್ಯಾಗಮ offset;
}

ಧ್ರುವ ವರ್ಣ *fdt_find_string_(ಧ್ರುವ ವರ್ಣ *strtab, ಪೂರ್ಣಾಙ್ಕ tabsize, ಧ್ರುವ ವರ್ಣ *s)
{
	ಪೂರ್ಣಾಙ್ಕ len = strlen(s) + 1;
	ಧ್ರುವ ವರ್ಣ *last = strtab + tabsize - len;
	ಧ್ರುವ ವರ್ಣ *p;

	ಪ್ರತಿ (p = strtab; p <= last; p++)
		ಯದಿ (memcmp(p, s, len) == 0)
			ಪ್ರತ್ಯಾಗಮ p;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಪೂರ್ಣಾಙ್ಕ fdt_move(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಶೂನ್ಯ *buf, ಪೂರ್ಣಾಙ್ಕ bufsize)
{
	ಯದಿ (!can_assume(VALID_ಒಳಹರಿವು) && bufsize < 0)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	ಯದಿ (fdt_totalsize(fdt) > (unsigned ಪೂರ್ಣಾಙ್ಕ)bufsize)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	ಪ್ರತ್ಯಾಗಮ 0;
}
