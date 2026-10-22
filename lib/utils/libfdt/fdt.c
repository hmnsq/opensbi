// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#अन्तर्भाव "libfdt_env.h"

#अन्तर्भाव <fdt.h>
#अन्तर्भाव <libfdt.h>

#अन्तर्भाव "libfdt_internal.h"

/*
 * Minimal sanity check for a read-only tree. fdt_ro_probe_() checks
 * that the given buffer contains what appears to be a flattened
 * device tree with sane information in its header.
 */
int32_t fdt_ro_probe_(ध्रुव शून्य *fdt)
{
	uint32_t totalsize = fdt_totalsize(fdt);

	जइ (can_assume(VALID_DTB))
		प्रत्यागम totalsize;

	/* The device tree must be at an 8-byte aligned address */
	जइ ((uintptr_t)fdt & 7)
		प्रत्यागम -FDT_ERR_ALIGNMENT;

	जइ (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		जइ (!can_assume(LATEST)) {
			जइ (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				प्रत्यागम -FDT_ERR_BADVERSION;
			जइ (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				प्रत्यागम -FDT_ERR_BADVERSION;
		}
	} अण्णहा जइ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		जइ (!can_assume(VALID_निवेशः) && fdt_size_dt_struct(fdt) == 0)
			प्रत्यागम -FDT_ERR_BADSTATE;
	} अण्णहा {
		प्रत्यागम -FDT_ERR_BADMAGIC;
	}

	जइ (totalsize < INT32_MAX)
		प्रत्यागम totalsize;
	अण्णहा
		प्रत्यागम -FDT_ERR_TRUNCATED;
}

स्थिर पूर्णाङ्क check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	प्रत्यागम (off >= hdrsize) && (off <= totalsize);
}

स्थिर पूर्णाङ्क check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	जइ (!check_off_(hdrsize, totalsize, base))
		प्रत्यागम 0; /* block start out of bounds */
	जइ ((base + size) < base)
		प्रत्यागम 0; /* overflow */
	जइ (!check_off_(hdrsize, totalsize, base + size))
		प्रत्यागम 0; /* block end out of bounds */
	प्रत्यागम 1;
}

size_t fdt_header_size_(uint32_t version)
{
	जइ (version <= 1)
		प्रत्यागम FDT_V1_SIZE;
	अण्णहा जइ (version <= 2)
		प्रत्यागम FDT_V2_SIZE;
	अण्णहा जइ (version <= 3)
		प्रत्यागम FDT_V3_SIZE;
	अण्णहा जइ (version <= 16)
		प्रत्यागम FDT_V16_SIZE;
	अण्णहा
		प्रत्यागम FDT_V17_SIZE;
}

size_t fdt_header_size(ध्रुव शून्य *fdt)
{
	प्रत्यागम can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

पूर्णाङ्क fdt_check_header(ध्रुव शून्य *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	जइ ((uintptr_t)fdt & 7)
		प्रत्यागम -FDT_ERR_ALIGNMENT;

	जइ (fdt_magic(fdt) != FDT_MAGIC)
		प्रत्यागम -FDT_ERR_BADMAGIC;
	जइ (!can_assume(LATEST)) {
		जइ ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			प्रत्यागम -FDT_ERR_BADVERSION;
		जइ (fdt_version(fdt) < fdt_last_comp_version(fdt))
			प्रत्यागम -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	जइ (!can_assume(VALID_DTB)) {

		जइ ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			प्रत्यागम -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		जइ (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			प्रत्यागम -FDT_ERR_TRUNCATED;
	}

	जइ (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		जइ (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			जइ (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				प्रत्यागम -FDT_ERR_TRUNCATED;
		} अण्णहा {
			जइ (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				प्रत्यागम -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		जइ (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			प्रत्यागम -FDT_ERR_TRUNCATED;
	}

	प्रत्यागम 0;
}

ध्रुव शून्य *fdt_offset_ptr(ध्रुव शून्य *fdt, पूर्णाङ्क offset, unsigned पूर्णाङ्क len)
{
	unsigned पूर्णाङ्क uoffset = offset;
	unsigned पूर्णाङ्क absoffset = offset + fdt_off_dt_struct(fdt);

	जइ (offset < 0)
		प्रत्यागम रिक्त;

	जइ (!can_assume(VALID_निवेशः))
		जइ ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			प्रत्यागम रिक्त;

	जइ (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		जइ (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			प्रत्यागम रिक्त;

	प्रत्यागम fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(ध्रुव शून्य *fdt, पूर्णाङ्क startoffset, पूर्णाङ्क *nextoffset)
{
	ध्रुव fdt32_t *tagp, *lenp;
	uint32_t tag;
	पूर्णाङ्क offset = startoffset;
	ध्रुव वर्ण *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	जइ (!can_assume(VALID_DTB) && !tagp)
		प्रत्यागम FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	विकल्प (tag) {
	पक्ष FDT_BEGIN_ग्रन्थिः:
		/* skip name */
		करेहि {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} जाव (p && (*p != '\0'));
		जइ (!can_assume(VALID_DTB) && !p)
			प्रत्यागम FDT_END; /* premature end */
		भङ्ग;

	पक्ष FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, परिमाण(*lenp));
		जइ (!can_assume(VALID_DTB) && !lenp)
			प्रत्यागम FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += परिमाण(संरचना fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		जइ (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		भङ्ग;

	पक्ष FDT_END:
	पक्ष FDT_END_ग्रन्थिः:
	पक्ष FDT_NOP:
		भङ्ग;

	स्वभाव:
		प्रत्यागम FDT_END;
	}

	जइ (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		प्रत्यागम FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	प्रत्यागम tag;
}

पूर्णाङ्क fdt_check_ग्रन्थिः_offset(ध्रुव शून्य *fdt, पूर्णाङ्क offset)
{
	जइ (!can_assume(VALID_निवेशः)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		प्रत्यागम -FDT_ERR_BADOFFSET;

	जइ (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_ग्रन्थिः)
		प्रत्यागम -FDT_ERR_BADOFFSET;

	प्रत्यागम offset;
}

पूर्णाङ्क fdt_check_prop_offset_(ध्रुव शून्य *fdt, पूर्णाङ्क offset)
{
	जइ (!can_assume(VALID_निवेशः)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		प्रत्यागम -FDT_ERR_BADOFFSET;

	जइ (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		प्रत्यागम -FDT_ERR_BADOFFSET;

	प्रत्यागम offset;
}

पूर्णाङ्क fdt_next_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क offset, पूर्णाङ्क *depth)
{
	पूर्णाङ्क nextoffset = 0;
	uint32_t tag;

	जइ (offset >= 0)
		जइ ((nextoffset = fdt_check_ग्रन्थिः_offset(fdt, offset)) < 0)
			प्रत्यागम nextoffset;

	करेहि {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		विकल्प (tag) {
		पक्ष FDT_PROP:
		पक्ष FDT_NOP:
			भङ्ग;

		पक्ष FDT_BEGIN_ग्रन्थिः:
			जइ (depth)
				(*depth)++;
			भङ्ग;

		पक्ष FDT_END_ग्रन्थिः:
			जइ (depth && ((--(*depth)) < 0))
				प्रत्यागम nextoffset;
			भङ्ग;

		पक्ष FDT_END:
			जइ ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				प्रत्यागम -FDT_ERR_NOTFOUND;
			अण्णहा
				प्रत्यागम nextoffset;
		}
	} जाव (tag != FDT_BEGIN_ग्रन्थिः);

	प्रत्यागम offset;
}

पूर्णाङ्क fdt_first_subnode(ध्रुव शून्य *fdt, पूर्णाङ्क offset)
{
	पूर्णाङ्क depth = 0;

	offset = fdt_next_ग्रन्थिः(fdt, offset, &depth);
	जइ (offset < 0 || depth != 1)
		प्रत्यागम -FDT_ERR_NOTFOUND;

	प्रत्यागम offset;
}

पूर्णाङ्क fdt_next_subnode(ध्रुव शून्य *fdt, पूर्णाङ्क offset)
{
	पूर्णाङ्क depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	करेहि {
		offset = fdt_next_ग्रन्थिः(fdt, offset, &depth);
		जइ (offset < 0 || depth < 1)
			प्रत्यागम -FDT_ERR_NOTFOUND;
	} जाव (depth > 1);

	प्रत्यागम offset;
}

ध्रुव वर्ण *fdt_find_string_(ध्रुव वर्ण *strtab, पूर्णाङ्क tabsize, ध्रुव वर्ण *s)
{
	पूर्णाङ्क len = strlen(s) + 1;
	ध्रुव वर्ण *last = strtab + tabsize - len;
	ध्रुव वर्ण *p;

	प्रति (p = strtab; p <= last; p++)
		जइ (memcmp(p, s, len) == 0)
			प्रत्यागम p;
	प्रत्यागम रिक्त;
}

पूर्णाङ्क fdt_move(ध्रुव शून्य *fdt, शून्य *buf, पूर्णाङ्क bufsize)
{
	जइ (!can_assume(VALID_निवेशः) && bufsize < 0)
		प्रत्यागम -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	जइ (fdt_totalsize(fdt) > (unsigned पूर्णाङ्क)bufsize)
		प्रत्यागम -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	प्रत्यागम 0;
}
