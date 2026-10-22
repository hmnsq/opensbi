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
int32_t fdt_ro_probe_(स्थिरांक शून्य *fdt)
{
	uint32_t totalsize = fdt_totalsize(fdt);

	जर (can_assume(VALID_DTB))
		परत totalsize;

	/* The device tree must be at an 8-byte aligned address */
	जर ((uintptr_t)fdt & 7)
		परत -FDT_ERR_ALIGNMENT;

	जर (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		जर (!can_assume(LATEST)) {
			जर (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				परत -FDT_ERR_BADVERSION;
			जर (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				परत -FDT_ERR_BADVERSION;
		}
	} अन्यथा जर (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		जर (!can_assume(VALID_आदान) && fdt_size_dt_struct(fdt) == 0)
			परत -FDT_ERR_BADSTATE;
	} अन्यथा {
		परत -FDT_ERR_BADMAGIC;
	}

	जर (totalsize < INT32_MAX)
		परत totalsize;
	अन्यथा
		परत -FDT_ERR_TRUNCATED;
}

स्थिर पूर्णांक check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	परत (off >= hdrsize) && (off <= totalsize);
}

स्थिर पूर्णांक check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	जर (!check_off_(hdrsize, totalsize, base))
		परत 0; /* block start out of bounds */
	जर ((base + size) < base)
		परत 0; /* overflow */
	जर (!check_off_(hdrsize, totalsize, base + size))
		परत 0; /* block end out of bounds */
	परत 1;
}

माप_प्रकार fdt_header_size_(uint32_t version)
{
	जर (version <= 1)
		परत FDT_V1_SIZE;
	अन्यथा जर (version <= 2)
		परत FDT_V2_SIZE;
	अन्यथा जर (version <= 3)
		परत FDT_V3_SIZE;
	अन्यथा जर (version <= 16)
		परत FDT_V16_SIZE;
	अन्यथा
		परत FDT_V17_SIZE;
}

माप_प्रकार fdt_header_size(स्थिरांक शून्य *fdt)
{
	परत can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

पूर्णांक fdt_check_header(स्थिरांक शून्य *fdt)
{
	माप_प्रकार hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	जर ((uintptr_t)fdt & 7)
		परत -FDT_ERR_ALIGNMENT;

	जर (fdt_magic(fdt) != FDT_MAGIC)
		परत -FDT_ERR_BADMAGIC;
	जर (!can_assume(LATEST)) {
		जर ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			परत -FDT_ERR_BADVERSION;
		जर (fdt_version(fdt) < fdt_last_comp_version(fdt))
			परत -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	जर (!can_assume(VALID_DTB)) {

		जर ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > पूर्णांक_उच्च))
			परत -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		जर (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			परत -FDT_ERR_TRUNCATED;
	}

	जर (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		जर (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			जर (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				परत -FDT_ERR_TRUNCATED;
		} अन्यथा {
			जर (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				परत -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		जर (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			परत -FDT_ERR_TRUNCATED;
	}

	परत 0;
}

स्थिरांक शून्य *fdt_offset_ptr(स्थिरांक शून्य *fdt, पूर्णांक offset, अचिन्हित पूर्णांक len)
{
	अचिन्हित पूर्णांक uoffset = offset;
	अचिन्हित पूर्णांक absoffset = offset + fdt_off_dt_struct(fdt);

	जर (offset < 0)
		परत रिक्त;

	जर (!can_assume(VALID_आदान))
		जर ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			परत रिक्त;

	जर (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		जर (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			परत रिक्त;

	परत fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(स्थिरांक शून्य *fdt, पूर्णांक startoffset, पूर्णांक *nextoffset)
{
	स्थिरांक fdt32_t *tagp, *lenp;
	uint32_t tag;
	पूर्णांक offset = startoffset;
	स्थिरांक अक्षर *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	जर (!can_assume(VALID_DTB) && !tagp)
		परत FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	निवड (tag) {
	स्थिती FDT_BEGIN_गाठ:
		/* skip name */
		करा {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} पर्यंत (p && (*p != '\0'));
		जर (!can_assume(VALID_DTB) && !p)
			परत FDT_END; /* premature end */
		थांबा;

	स्थिती FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, आकार(*lenp));
		जर (!can_assume(VALID_DTB) && !lenp)
			परत FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += आकार(संरचना fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		जर (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		थांबा;

	स्थिती FDT_END:
	स्थिती FDT_END_गाठ:
	स्थिती FDT_NOP:
		थांबा;

	पूर्वनिर्धारित:
		परत FDT_END;
	}

	जर (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		परत FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	परत tag;
}

पूर्णांक fdt_check_गाठ_offset(स्थिरांक शून्य *fdt, पूर्णांक offset)
{
	जर (!can_assume(VALID_आदान)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		परत -FDT_ERR_BADOFFSET;

	जर (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_गाठ)
		परत -FDT_ERR_BADOFFSET;

	परत offset;
}

पूर्णांक fdt_check_prop_offset_(स्थिरांक शून्य *fdt, पूर्णांक offset)
{
	जर (!can_assume(VALID_आदान)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		परत -FDT_ERR_BADOFFSET;

	जर (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		परत -FDT_ERR_BADOFFSET;

	परत offset;
}

पूर्णांक fdt_next_गाठ(स्थिरांक शून्य *fdt, पूर्णांक offset, पूर्णांक *depth)
{
	पूर्णांक nextoffset = 0;
	uint32_t tag;

	जर (offset >= 0)
		जर ((nextoffset = fdt_check_गाठ_offset(fdt, offset)) < 0)
			परत nextoffset;

	करा {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		निवड (tag) {
		स्थिती FDT_PROP:
		स्थिती FDT_NOP:
			थांबा;

		स्थिती FDT_BEGIN_गाठ:
			जर (depth)
				(*depth)++;
			थांबा;

		स्थिती FDT_END_गाठ:
			जर (depth && ((--(*depth)) < 0))
				परत nextoffset;
			थांबा;

		स्थिती FDT_END:
			जर ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				परत -FDT_ERR_NOTFOUND;
			अन्यथा
				परत nextoffset;
		}
	} पर्यंत (tag != FDT_BEGIN_गाठ);

	परत offset;
}

पूर्णांक fdt_first_subnode(स्थिरांक शून्य *fdt, पूर्णांक offset)
{
	पूर्णांक depth = 0;

	offset = fdt_next_गाठ(fdt, offset, &depth);
	जर (offset < 0 || depth != 1)
		परत -FDT_ERR_NOTFOUND;

	परत offset;
}

पूर्णांक fdt_next_subnode(स्थिरांक शून्य *fdt, पूर्णांक offset)
{
	पूर्णांक depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	करा {
		offset = fdt_next_गाठ(fdt, offset, &depth);
		जर (offset < 0 || depth < 1)
			परत -FDT_ERR_NOTFOUND;
	} पर्यंत (depth > 1);

	परत offset;
}

स्थिरांक अक्षर *fdt_find_string_(स्थिरांक अक्षर *strtab, पूर्णांक tabsize, स्थिरांक अक्षर *s)
{
	पूर्णांक len = म_माप(s) + 1;
	स्थिरांक अक्षर *last = strtab + tabsize - len;
	स्थिरांक अक्षर *p;

	साठी (p = strtab; p <= last; p++)
		जर (स_भेद(p, s, len) == 0)
			परत p;
	परत रिक्त;
}

पूर्णांक fdt_move(स्थिरांक शून्य *fdt, शून्य *buf, पूर्णांक bufsize)
{
	जर (!can_assume(VALID_आदान) && bufsize < 0)
		परत -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	जर (fdt_totalsize(fdt) > (अचिन्हित पूर्णांक)bufsize)
		परत -FDT_ERR_NOSPACE;

	स_हटाओ(buf, fdt, fdt_totalsize(fdt));
	परत 0;
}
