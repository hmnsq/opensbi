// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#समावेश "libfdt_env.h"

#समावेश <fdt.h>
#समावेश <libfdt.h>

#समावेश "libfdt_internal.h"

/*
 * Minimal sanity check for a read-only tree. fdt_ro_probe_() checks
 * that the given buffer contains what appears to be a flattened
 * device tree with sane information in its header.
 */
int32_t fdt_ro_probe_(स्थिर व्योम *fdt)
{
	uint32_t totalsize = fdt_totalsize(fdt);

	अगर (can_assume(VALID_DTB))
		वापस totalsize;

	/* The device tree must be at an 8-byte aligned address */
	अगर ((uintptr_t)fdt & 7)
		वापस -FDT_ERR_ALIGNMENT;

	अगर (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		अगर (!can_assume(LATEST)) {
			अगर (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				वापस -FDT_ERR_BADVERSION;
			अगर (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				वापस -FDT_ERR_BADVERSION;
		}
	} अन्यथा अगर (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		अगर (!can_assume(VALID_निवेश) && fdt_आकार_dt_struct(fdt) == 0)
			वापस -FDT_ERR_BADSTATE;
	} अन्यथा {
		वापस -FDT_ERR_BADMAGIC;
	}

	अगर (totalsize < INT32_MAX)
		वापस totalsize;
	अन्यथा
		वापस -FDT_ERR_TRUNCATED;
}

अटल पूर्णांक check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	वापस (off >= hdrsize) && (off <= totalsize);
}

अटल पूर्णांक check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t आकार)
{
	अगर (!check_off_(hdrsize, totalsize, base))
		वापस 0; /* block start out of bounds */
	अगर ((base + आकार) < base)
		वापस 0; /* overflow */
	अगर (!check_off_(hdrsize, totalsize, base + आकार))
		वापस 0; /* block end out of bounds */
	वापस 1;
}

माप_प्रकार fdt_header_आकार(uint32_t version)
{
	अगर (version <= 1)
		वापस FDT_V1_आकार;
	अन्यथा अगर (version <= 2)
		वापस FDT_V2_आकार;
	अन्यथा अगर (version <= 3)
		वापस FDT_V3_आकार;
	अन्यथा अगर (version <= 16)
		वापस FDT_V16_आकार;
	अन्यथा
		वापस FDT_V17_आकार;
}

माप_प्रकार fdt_header_आकार_2(स्थिर व्योम *fdt)
{
	वापस can_assume(LATEST) ? FDT_V17_आकार :
		fdt_header_आकार(fdt_version(fdt));
}

पूर्णांक fdt_check_header(स्थिर व्योम *fdt)
{
	माप_प्रकार hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	अगर ((uintptr_t)fdt & 7)
		वापस -FDT_ERR_ALIGNMENT;

	अगर (fdt_magic(fdt) != FDT_MAGIC)
		वापस -FDT_ERR_BADMAGIC;
	अगर (!can_assume(LATEST)) {
		अगर ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			वापस -FDT_ERR_BADVERSION;
		अगर (fdt_version(fdt) < fdt_last_comp_version(fdt))
			वापस -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_आकार_2(fdt);
	अगर (!can_assume(VALID_DTB)) {

		अगर ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > पूर्णांक_उच्च))
			वापस -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		अगर (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			वापस -FDT_ERR_TRUNCATED;
	}

	अगर (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		अगर (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			अगर (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				वापस -FDT_ERR_TRUNCATED;
		} अन्यथा {
			अगर (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_आकार_dt_struct(fdt)))
				वापस -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		अगर (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_आकार_dt_strings(fdt)))
			वापस -FDT_ERR_TRUNCATED;
	}

	वापस 0;
}

स्थिर व्योम *fdt_offset_ptr(स्थिर व्योम *fdt, पूर्णांक offset, अचिन्हित पूर्णांक len)
{
	अचिन्हित पूर्णांक uoffset = offset;
	अचिन्हित पूर्णांक absoffset = offset + fdt_off_dt_struct(fdt);

	अगर (offset < 0)
		वापस शून्य;

	अगर (!can_assume(VALID_निवेश))
		अगर ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			वापस शून्य;

	अगर (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		अगर (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_आकार_dt_struct(fdt)))
			वापस शून्य;

	वापस fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_अग्र_tag(स्थिर व्योम *fdt, पूर्णांक startoffset, पूर्णांक *nextoffset)
{
	स्थिर fdt32_t *tagp, *lenp;
	uint32_t tag;
	पूर्णांक offset = startoffset;
	स्थिर अक्षर *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	अगर (!can_assume(VALID_DTB) && !tagp)
		वापस FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	चयन (tag) {
	हाल FDT_BEGIN_गाँठ:
		/* skip name */
		करो {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} जबतक (p && (*p != '\0'));
		अगर (!can_assume(VALID_DTB) && !p)
			वापस FDT_END; /* premature end */
		अवरोध;

	हाल FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, माप(*lenp));
		अगर (!can_assume(VALID_DTB) && !lenp)
			वापस FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += माप(काष्ठा fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		अगर (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		अवरोध;

	हाल FDT_END:
	हाल FDT_END_गाँठ:
	हाल FDT_NOP:
		अवरोध;

	शेष:
		वापस FDT_END;
	}

	अगर (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		वापस FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	वापस tag;
}

पूर्णांक fdt_check_गाँठ_offset(स्थिर व्योम *fdt, पूर्णांक offset)
{
	अगर (!can_assume(VALID_निवेश)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		वापस -FDT_ERR_BADOFFSET;

	अगर (fdt_अग्र_tag(fdt, offset, &offset) != FDT_BEGIN_गाँठ)
		वापस -FDT_ERR_BADOFFSET;

	वापस offset;
}

पूर्णांक fdt_check_prop_offset_(स्थिर व्योम *fdt, पूर्णांक offset)
{
	अगर (!can_assume(VALID_निवेश)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		वापस -FDT_ERR_BADOFFSET;

	अगर (fdt_अग्र_tag(fdt, offset, &offset) != FDT_PROP)
		वापस -FDT_ERR_BADOFFSET;

	वापस offset;
}

पूर्णांक fdt_अग्र_गाँठ(स्थिर व्योम *fdt, पूर्णांक offset, पूर्णांक *depth)
{
	पूर्णांक nextoffset = 0;
	uint32_t tag;

	अगर (offset >= 0)
		अगर ((nextoffset = fdt_check_गाँठ_offset(fdt, offset)) < 0)
			वापस nextoffset;

	करो {
		offset = nextoffset;
		tag = fdt_अग्र_tag(fdt, offset, &nextoffset);

		चयन (tag) {
		हाल FDT_PROP:
		हाल FDT_NOP:
			अवरोध;

		हाल FDT_BEGIN_गाँठ:
			अगर (depth)
				(*depth)++;
			अवरोध;

		हाल FDT_END_गाँठ:
			अगर (depth && ((--(*depth)) < 0))
				वापस nextoffset;
			अवरोध;

		हाल FDT_END:
			अगर ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				वापस -FDT_ERR_NOTFOUND;
			अन्यथा
				वापस nextoffset;
		}
	} जबतक (tag != FDT_BEGIN_गाँठ);

	वापस offset;
}

पूर्णांक fdt_first_subnode(स्थिर व्योम *fdt, पूर्णांक offset)
{
	पूर्णांक depth = 0;

	offset = fdt_अग्र_गाँठ(fdt, offset, &depth);
	अगर (offset < 0 || depth != 1)
		वापस -FDT_ERR_NOTFOUND;

	वापस offset;
}

पूर्णांक fdt_अग्र_subnode(स्थिर व्योम *fdt, पूर्णांक offset)
{
	पूर्णांक depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	करो {
		offset = fdt_अग्र_गाँठ(fdt, offset, &depth);
		अगर (offset < 0 || depth < 1)
			वापस -FDT_ERR_NOTFOUND;
	} जबतक (depth > 1);

	वापस offset;
}

स्थिर अक्षर *fdt_find_string_(स्थिर अक्षर *strtab, पूर्णांक tabsize, स्थिर अक्षर *s)
{
	पूर्णांक len = म_माप(s) + 1;
	स्थिर अक्षर *last = strtab + tabsize - len;
	स्थिर अक्षर *p;

	क्रम (p = strtab; p <= last; p++)
		अगर (स_भेद(p, s, len) == 0)
			वापस p;
	वापस शून्य;
}

पूर्णांक fdt_move(स्थिर व्योम *fdt, व्योम *buf, पूर्णांक bufsize)
{
	अगर (!can_assume(VALID_निवेश) && bufsize < 0)
		वापस -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	अगर (fdt_totalsize(fdt) > (अचिन्हित पूर्णांक)bufsize)
		वापस -FDT_ERR_NOSPACE;

	स_हटाओ(buf, fdt, fdt_totalsize(fdt));
	वापस 0;
}
