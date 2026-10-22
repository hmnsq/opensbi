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
int32_t fdt_ro_probe_(अचर शून्य *fdt)
{
	uint32_t totalsize = fdt_totalsize(fdt);

	यदि (can_assume(VALID_DTB))
		फर्काऊ totalsize;

	/* The device tree must be at an 8-byte aligned address */
	यदि ((uintptr_t)fdt & 7)
		फर्काऊ -FDT_ERR_ALIGNMENT;

	यदि (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		यदि (!can_assume(LATEST)) {
			यदि (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				फर्काऊ -FDT_ERR_BADVERSION;
			यदि (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				फर्काऊ -FDT_ERR_BADVERSION;
		}
	} अन्यथा यदि (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		यदि (!can_assume(VALID_आगत) && fdt_size_dt_struct(fdt) == 0)
			फर्काऊ -FDT_ERR_BADSTATE;
	} अन्यथा {
		फर्काऊ -FDT_ERR_BADMAGIC;
	}

	यदि (totalsize < INT32_MAX)
		फर्काऊ totalsize;
	अन्यथा
		फर्काऊ -FDT_ERR_TRUNCATED;
}

स्थिर पूर्णाङ्क check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	फर्काऊ (off >= hdrsize) && (off <= totalsize);
}

स्थिर पूर्णाङ्क check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	यदि (!check_off_(hdrsize, totalsize, base))
		फर्काऊ 0; /* block start out of bounds */
	यदि ((base + size) < base)
		फर्काऊ 0; /* overflow */
	यदि (!check_off_(hdrsize, totalsize, base + size))
		फर्काऊ 0; /* block end out of bounds */
	फर्काऊ 1;
}

माप_प्रकार fdt_header_size_(uint32_t version)
{
	यदि (version <= 1)
		फर्काऊ FDT_V1_SIZE;
	अन्यथा यदि (version <= 2)
		फर्काऊ FDT_V2_SIZE;
	अन्यथा यदि (version <= 3)
		फर्काऊ FDT_V3_SIZE;
	अन्यथा यदि (version <= 16)
		फर्काऊ FDT_V16_SIZE;
	अन्यथा
		फर्काऊ FDT_V17_SIZE;
}

माप_प्रकार fdt_header_size(अचर शून्य *fdt)
{
	फर्काऊ can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

पूर्णाङ्क fdt_check_header(अचर शून्य *fdt)
{
	माप_प्रकार hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	यदि ((uintptr_t)fdt & 7)
		फर्काऊ -FDT_ERR_ALIGNMENT;

	यदि (fdt_magic(fdt) != FDT_MAGIC)
		फर्काऊ -FDT_ERR_BADMAGIC;
	यदि (!can_assume(LATEST)) {
		यदि ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			फर्काऊ -FDT_ERR_BADVERSION;
		यदि (fdt_version(fdt) < fdt_last_comp_version(fdt))
			फर्काऊ -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	यदि (!can_assume(VALID_DTB)) {

		यदि ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > पूर्णांक_उच्च))
			फर्काऊ -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		यदि (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			फर्काऊ -FDT_ERR_TRUNCATED;
	}

	यदि (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		यदि (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			यदि (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				फर्काऊ -FDT_ERR_TRUNCATED;
		} अन्यथा {
			यदि (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				फर्काऊ -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		यदि (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			फर्काऊ -FDT_ERR_TRUNCATED;
	}

	फर्काऊ 0;
}

अचर शून्य *fdt_offset_ptr(अचर शून्य *fdt, पूर्णाङ्क offset, अचिन्हित पूर्णाङ्क len)
{
	अचिन्हित पूर्णाङ्क uoffset = offset;
	अचिन्हित पूर्णाङ्क absoffset = offset + fdt_off_dt_struct(fdt);

	यदि (offset < 0)
		फर्काऊ रिक्त;

	यदि (!can_assume(VALID_आगत))
		यदि ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			फर्काऊ रिक्त;

	यदि (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		यदि (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			फर्काऊ रिक्त;

	फर्काऊ fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(अचर शून्य *fdt, पूर्णाङ्क startoffset, पूर्णाङ्क *nextoffset)
{
	अचर fdt32_t *tagp, *lenp;
	uint32_t tag;
	पूर्णाङ्क offset = startoffset;
	अचर वर्ण *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	यदि (!can_assume(VALID_DTB) && !tagp)
		फर्काऊ FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	विकल्प (tag) {
	पक्ष FDT_BEGIN_गाँठो:
		/* skip name */
		गर {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} जबसम्म (p && (*p != '\0'));
		यदि (!can_assume(VALID_DTB) && !p)
			फर्काऊ FDT_END; /* premature end */
		रोक;

	पक्ष FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, परिमाण(*lenp));
		यदि (!can_assume(VALID_DTB) && !lenp)
			फर्काऊ FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += परिमाण(संरचना fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		यदि (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		रोक;

	पक्ष FDT_END:
	पक्ष FDT_END_गाँठो:
	पक्ष FDT_NOP:
		रोक;

	स्वभाव:
		फर्काऊ FDT_END;
	}

	यदि (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		फर्काऊ FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	फर्काऊ tag;
}

पूर्णाङ्क fdt_check_गाँठो_offset(अचर शून्य *fdt, पूर्णाङ्क offset)
{
	यदि (!can_assume(VALID_आगत)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		फर्काऊ -FDT_ERR_BADOFFSET;

	यदि (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_गाँठो)
		फर्काऊ -FDT_ERR_BADOFFSET;

	फर्काऊ offset;
}

पूर्णाङ्क fdt_check_prop_offset_(अचर शून्य *fdt, पूर्णाङ्क offset)
{
	यदि (!can_assume(VALID_आगत)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		फर्काऊ -FDT_ERR_BADOFFSET;

	यदि (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		फर्काऊ -FDT_ERR_BADOFFSET;

	फर्काऊ offset;
}

पूर्णाङ्क fdt_next_गाँठो(अचर शून्य *fdt, पूर्णाङ्क offset, पूर्णाङ्क *depth)
{
	पूर्णाङ्क nextoffset = 0;
	uint32_t tag;

	यदि (offset >= 0)
		यदि ((nextoffset = fdt_check_गाँठो_offset(fdt, offset)) < 0)
			फर्काऊ nextoffset;

	गर {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		विकल्प (tag) {
		पक्ष FDT_PROP:
		पक्ष FDT_NOP:
			रोक;

		पक्ष FDT_BEGIN_गाँठो:
			यदि (depth)
				(*depth)++;
			रोक;

		पक्ष FDT_END_गाँठो:
			यदि (depth && ((--(*depth)) < 0))
				फर्काऊ nextoffset;
			रोक;

		पक्ष FDT_END:
			यदि ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				फर्काऊ -FDT_ERR_NOTFOUND;
			अन्यथा
				फर्काऊ nextoffset;
		}
	} जबसम्म (tag != FDT_BEGIN_गाँठो);

	फर्काऊ offset;
}

पूर्णाङ्क fdt_first_subnode(अचर शून्य *fdt, पूर्णाङ्क offset)
{
	पूर्णाङ्क depth = 0;

	offset = fdt_next_गाँठो(fdt, offset, &depth);
	यदि (offset < 0 || depth != 1)
		फर्काऊ -FDT_ERR_NOTFOUND;

	फर्काऊ offset;
}

पूर्णाङ्क fdt_next_subnode(अचर शून्य *fdt, पूर्णाङ्क offset)
{
	पूर्णाङ्क depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	गर {
		offset = fdt_next_गाँठो(fdt, offset, &depth);
		यदि (offset < 0 || depth < 1)
			फर्काऊ -FDT_ERR_NOTFOUND;
	} जबसम्म (depth > 1);

	फर्काऊ offset;
}

अचर वर्ण *fdt_find_string_(अचर वर्ण *strtab, पूर्णाङ्क tabsize, अचर वर्ण *s)
{
	पूर्णाङ्क len = म_माप(s) + 1;
	अचर वर्ण *last = strtab + tabsize - len;
	अचर वर्ण *p;

	लागि (p = strtab; p <= last; p++)
		यदि (स_भेद(p, s, len) == 0)
			फर्काऊ p;
	फर्काऊ रिक्त;
}

पूर्णाङ्क fdt_move(अचर शून्य *fdt, शून्य *buf, पूर्णाङ्क bufsize)
{
	यदि (!can_assume(VALID_आगत) && bufsize < 0)
		फर्काऊ -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	यदि (fdt_totalsize(fdt) > (अचिन्हित पूर्णाङ्क)bufsize)
		फर्काऊ -FDT_ERR_NOSPACE;

	स_हटाओ(buf, fdt, fdt_totalsize(fdt));
	फर्काऊ 0;
}
