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

	सचे (can_assume(VALID_DTB))
		पटिनिवत्त totalsize;

	/* The device tree must be at an 8-byte aligned address */
	सचे ((uintptr_t)fdt & 7)
		पटिनिवत्त -FDT_ERR_ALIGNMENT;

	सचे (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		सचे (!can_assume(LATEST)) {
			सचे (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				पटिनिवत्त -FDT_ERR_BADVERSION;
			सचे (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				पटिनिवत्त -FDT_ERR_BADVERSION;
		}
	} अञ्ञथा सचे (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		सचे (!can_assume(VALID_निवेशः) && fdt_size_dt_struct(fdt) == 0)
			पटिनिवत्त -FDT_ERR_BADSTATE;
	} अञ्ञथा {
		पटिनिवत्त -FDT_ERR_BADMAGIC;
	}

	सचे (totalsize < INT32_MAX)
		पटिनिवत्त totalsize;
	अञ्ञथा
		पटिनिवत्त -FDT_ERR_TRUNCATED;
}

स्थिर पूर्णाङ्क check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	पटिनिवत्त (off >= hdrsize) && (off <= totalsize);
}

स्थिर पूर्णाङ्क check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	सचे (!check_off_(hdrsize, totalsize, base))
		पटिनिवत्त 0; /* block start out of bounds */
	सचे ((base + size) < base)
		पटिनिवत्त 0; /* overflow */
	सचे (!check_off_(hdrsize, totalsize, base + size))
		पटिनिवत्त 0; /* block end out of bounds */
	पटिनिवत्त 1;
}

size_t fdt_header_size_(uint32_t version)
{
	सचे (version <= 1)
		पटिनिवत्त FDT_V1_SIZE;
	अञ्ञथा सचे (version <= 2)
		पटिनिवत्त FDT_V2_SIZE;
	अञ्ञथा सचे (version <= 3)
		पटिनिवत्त FDT_V3_SIZE;
	अञ्ञथा सचे (version <= 16)
		पटिनिवत्त FDT_V16_SIZE;
	अञ्ञथा
		पटिनिवत्त FDT_V17_SIZE;
}

size_t fdt_header_size(ध्रुव शून्य *fdt)
{
	पटिनिवत्त can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

पूर्णाङ्क fdt_check_header(ध्रुव शून्य *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	सचे ((uintptr_t)fdt & 7)
		पटिनिवत्त -FDT_ERR_ALIGNMENT;

	सचे (fdt_magic(fdt) != FDT_MAGIC)
		पटिनिवत्त -FDT_ERR_BADMAGIC;
	सचे (!can_assume(LATEST)) {
		सचे ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			पटिनिवत्त -FDT_ERR_BADVERSION;
		सचे (fdt_version(fdt) < fdt_last_comp_version(fdt))
			पटिनिवत्त -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	सचे (!can_assume(VALID_DTB)) {

		सचे ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			पटिनिवत्त -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		सचे (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			पटिनिवत्त -FDT_ERR_TRUNCATED;
	}

	सचे (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		सचे (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			सचे (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				पटिनिवत्त -FDT_ERR_TRUNCATED;
		} अञ्ञथा {
			सचे (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				पटिनिवत्त -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		सचे (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			पटिनिवत्त -FDT_ERR_TRUNCATED;
	}

	पटिनिवत्त 0;
}

ध्रुव शून्य *fdt_offset_ptr(ध्रुव शून्य *fdt, पूर्णाङ्क offset, unsigned पूर्णाङ्क len)
{
	unsigned पूर्णाङ्क uoffset = offset;
	unsigned पूर्णाङ्क absoffset = offset + fdt_off_dt_struct(fdt);

	सचे (offset < 0)
		पटिनिवत्त रिक्त;

	सचे (!can_assume(VALID_निवेशः))
		सचे ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			पटिनिवत्त रिक्त;

	सचे (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		सचे (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			पटिनिवत्त रिक्त;

	पटिनिवत्त fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(ध्रुव शून्य *fdt, पूर्णाङ्क startoffset, पूर्णाङ्क *nextoffset)
{
	ध्रुव fdt32_t *tagp, *lenp;
	uint32_t tag;
	पूर्णाङ्क offset = startoffset;
	ध्रुव वर्ण *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	सचे (!can_assume(VALID_DTB) && !tagp)
		पटिनिवत्त FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	विकल्प (tag) {
	पक्ष FDT_BEGIN_ग्रन्थिः:
		/* skip name */
		करोहि {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} याव (p && (*p != '\0'));
		सचे (!can_assume(VALID_DTB) && !p)
			पटिनिवत्त FDT_END; /* premature end */
		भङ्ग;

	पक्ष FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, परिमाण(*lenp));
		सचे (!can_assume(VALID_DTB) && !lenp)
			पटिनिवत्त FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += परिमाण(संरचना fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		सचे (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		भङ्ग;

	पक्ष FDT_END:
	पक्ष FDT_END_ग्रन्थिः:
	पक्ष FDT_NOP:
		भङ्ग;

	स्वभाव:
		पटिनिवत्त FDT_END;
	}

	सचे (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		पटिनिवत्त FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	पटिनिवत्त tag;
}

पूर्णाङ्क fdt_check_ग्रन्थिः_offset(ध्रुव शून्य *fdt, पूर्णाङ्क offset)
{
	सचे (!can_assume(VALID_निवेशः)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		पटिनिवत्त -FDT_ERR_BADOFFSET;

	सचे (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_ग्रन्थिः)
		पटिनिवत्त -FDT_ERR_BADOFFSET;

	पटिनिवत्त offset;
}

पूर्णाङ्क fdt_check_prop_offset_(ध्रुव शून्य *fdt, पूर्णाङ्क offset)
{
	सचे (!can_assume(VALID_निवेशः)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		पटिनिवत्त -FDT_ERR_BADOFFSET;

	सचे (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		पटिनिवत्त -FDT_ERR_BADOFFSET;

	पटिनिवत्त offset;
}

पूर्णाङ्क fdt_next_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क offset, पूर्णाङ्क *depth)
{
	पूर्णाङ्क nextoffset = 0;
	uint32_t tag;

	सचे (offset >= 0)
		सचे ((nextoffset = fdt_check_ग्रन्थिः_offset(fdt, offset)) < 0)
			पटिनिवत्त nextoffset;

	करोहि {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		विकल्प (tag) {
		पक्ष FDT_PROP:
		पक्ष FDT_NOP:
			भङ्ग;

		पक्ष FDT_BEGIN_ग्रन्थिः:
			सचे (depth)
				(*depth)++;
			भङ्ग;

		पक्ष FDT_END_ग्रन्थिः:
			सचे (depth && ((--(*depth)) < 0))
				पटिनिवत्त nextoffset;
			भङ्ग;

		पक्ष FDT_END:
			सचे ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				पटिनिवत्त -FDT_ERR_NOTFOUND;
			अञ्ञथा
				पटिनिवत्त nextoffset;
		}
	} याव (tag != FDT_BEGIN_ग्रन्थिः);

	पटिनिवत्त offset;
}

पूर्णाङ्क fdt_first_subnode(ध्रुव शून्य *fdt, पूर्णाङ्क offset)
{
	पूर्णाङ्क depth = 0;

	offset = fdt_next_ग्रन्थिः(fdt, offset, &depth);
	सचे (offset < 0 || depth != 1)
		पटिनिवत्त -FDT_ERR_NOTFOUND;

	पटिनिवत्त offset;
}

पूर्णाङ्क fdt_next_subnode(ध्रुव शून्य *fdt, पूर्णाङ्क offset)
{
	पूर्णाङ्क depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	करोहि {
		offset = fdt_next_ग्रन्थिः(fdt, offset, &depth);
		सचे (offset < 0 || depth < 1)
			पटिनिवत्त -FDT_ERR_NOTFOUND;
	} याव (depth > 1);

	पटिनिवत्त offset;
}

ध्रुव वर्ण *fdt_find_string_(ध्रुव वर्ण *strtab, पूर्णाङ्क tabsize, ध्रुव वर्ण *s)
{
	पूर्णाङ्क len = strlen(s) + 1;
	ध्रुव वर्ण *last = strtab + tabsize - len;
	ध्रुव वर्ण *p;

	अत्थाय (p = strtab; p <= last; p++)
		सचे (memcmp(p, s, len) == 0)
			पटिनिवत्त p;
	पटिनिवत्त रिक्त;
}

पूर्णाङ्क fdt_move(ध्रुव शून्य *fdt, शून्य *buf, पूर्णाङ्क bufsize)
{
	सचे (!can_assume(VALID_निवेशः) && bufsize < 0)
		पटिनिवत्त -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	सचे (fdt_totalsize(fdt) > (unsigned पूर्णाङ्क)bufsize)
		पटिनिवत्त -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	पटिनिवत्त 0;
}
