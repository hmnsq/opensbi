// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#அந்தர்பாவ "libfdt_env.h"

#அந்தர்பாவ <fdt.h>
#அந்தர்பாவ <libfdt.h>

#அந்தர்பாவ "libfdt_internal.h"

/*
 * Minimal sanity check for a read-only tree. fdt_ro_probe_() checks
 * that the given buffer contains what appears to be a flattened
 * device tree with sane information in its header.
 */
int32_t fdt_ro_probe_(மாறிலி வெறுமை *fdt)
{
	uint32_t totalsize = fdt_totalsize(fdt);

	எனில் (can_assume(VALID_DTB))
		திரும்பு totalsize;

	/* The device tree must be at an 8-byte aligned address */
	எனில் ((uintptr_t)fdt & 7)
		திரும்பு -FDT_ERR_ALIGNMENT;

	எனில் (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		எனில் (!can_assume(LATEST)) {
			எனில் (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				திரும்பு -FDT_ERR_BADVERSION;
			எனில் (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				திரும்பு -FDT_ERR_BADVERSION;
		}
	} இல்லையெனில் எனில் (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		எனில் (!can_assume(VALID_உள்ளீடு) && fdt_size_dt_struct(fdt) == 0)
			திரும்பு -FDT_ERR_BADSTATE;
	} இல்லையெனில் {
		திரும்பு -FDT_ERR_BADMAGIC;
	}

	எனில் (totalsize < INT32_MAX)
		திரும்பு totalsize;
	இல்லையெனில்
		திரும்பு -FDT_ERR_TRUNCATED;
}

நிலையான முழு check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	திரும்பு (off >= hdrsize) && (off <= totalsize);
}

நிலையான முழு check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	எனில் (!check_off_(hdrsize, totalsize, base))
		திரும்பு 0; /* block start out of bounds */
	எனில் ((base + size) < base)
		திரும்பு 0; /* overflow */
	எனில் (!check_off_(hdrsize, totalsize, base + size))
		திரும்பு 0; /* block end out of bounds */
	திரும்பு 1;
}

size_t fdt_header_size_(uint32_t version)
{
	எனில் (version <= 1)
		திரும்பு FDT_V1_SIZE;
	இல்லையெனில் எனில் (version <= 2)
		திரும்பு FDT_V2_SIZE;
	இல்லையெனில் எனில் (version <= 3)
		திரும்பு FDT_V3_SIZE;
	இல்லையெனில் எனில் (version <= 16)
		திரும்பு FDT_V16_SIZE;
	இல்லையெனில்
		திரும்பு FDT_V17_SIZE;
}

size_t fdt_header_size(மாறிலி வெறுமை *fdt)
{
	திரும்பு can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

முழு fdt_check_header(மாறிலி வெறுமை *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	எனில் ((uintptr_t)fdt & 7)
		திரும்பு -FDT_ERR_ALIGNMENT;

	எனில் (fdt_magic(fdt) != FDT_MAGIC)
		திரும்பு -FDT_ERR_BADMAGIC;
	எனில் (!can_assume(LATEST)) {
		எனில் ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			திரும்பு -FDT_ERR_BADVERSION;
		எனில் (fdt_version(fdt) < fdt_last_comp_version(fdt))
			திரும்பு -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	எனில் (!can_assume(VALID_DTB)) {

		எனில் ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			திரும்பு -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		எனில் (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			திரும்பு -FDT_ERR_TRUNCATED;
	}

	எனில் (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		எனில் (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			எனில் (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				திரும்பு -FDT_ERR_TRUNCATED;
		} இல்லையெனில் {
			எனில் (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				திரும்பு -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		எனில் (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			திரும்பு -FDT_ERR_TRUNCATED;
	}

	திரும்பு 0;
}

மாறிலி வெறுமை *fdt_offset_ptr(மாறிலி வெறுமை *fdt, முழு offset, unsigned முழு len)
{
	unsigned முழு uoffset = offset;
	unsigned முழு absoffset = offset + fdt_off_dt_struct(fdt);

	எனில் (offset < 0)
		திரும்பு வெற்று;

	எனில் (!can_assume(VALID_உள்ளீடு))
		எனில் ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			திரும்பு வெற்று;

	எனில் (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		எனில் (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			திரும்பு வெற்று;

	திரும்பு fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(மாறிலி வெறுமை *fdt, முழு startoffset, முழு *nextoffset)
{
	மாறிலி fdt32_t *tagp, *lenp;
	uint32_t tag;
	முழு offset = startoffset;
	மாறிலி எழுத்து *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	எனில் (!can_assume(VALID_DTB) && !tagp)
		திரும்பு FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	தேர்வு (tag) {
	நிலை FDT_BEGIN_முனை:
		/* skip name */
		செய் {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} போது (p && (*p != '\0'));
		எனில் (!can_assume(VALID_DTB) && !p)
			திரும்பு FDT_END; /* premature end */
		நிறுத்து;

	நிலை FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, அளவு(*lenp));
		எனில் (!can_assume(VALID_DTB) && !lenp)
			திரும்பு FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += அளவு(கட்டமைப்பு fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		எனில் (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		நிறுத்து;

	நிலை FDT_END:
	நிலை FDT_END_முனை:
	நிலை FDT_NOP:
		நிறுத்து;

	இயல்பு:
		திரும்பு FDT_END;
	}

	எனில் (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		திரும்பு FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	திரும்பு tag;
}

முழு fdt_check_முனை_offset(மாறிலி வெறுமை *fdt, முழு offset)
{
	எனில் (!can_assume(VALID_உள்ளீடு)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		திரும்பு -FDT_ERR_BADOFFSET;

	எனில் (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_முனை)
		திரும்பு -FDT_ERR_BADOFFSET;

	திரும்பு offset;
}

முழு fdt_check_prop_offset_(மாறிலி வெறுமை *fdt, முழு offset)
{
	எனில் (!can_assume(VALID_உள்ளீடு)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		திரும்பு -FDT_ERR_BADOFFSET;

	எனில் (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		திரும்பு -FDT_ERR_BADOFFSET;

	திரும்பு offset;
}

முழு fdt_next_முனை(மாறிலி வெறுமை *fdt, முழு offset, முழு *depth)
{
	முழு nextoffset = 0;
	uint32_t tag;

	எனில் (offset >= 0)
		எனில் ((nextoffset = fdt_check_முனை_offset(fdt, offset)) < 0)
			திரும்பு nextoffset;

	செய் {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		தேர்வு (tag) {
		நிலை FDT_PROP:
		நிலை FDT_NOP:
			நிறுத்து;

		நிலை FDT_BEGIN_முனை:
			எனில் (depth)
				(*depth)++;
			நிறுத்து;

		நிலை FDT_END_முனை:
			எனில் (depth && ((--(*depth)) < 0))
				திரும்பு nextoffset;
			நிறுத்து;

		நிலை FDT_END:
			எனில் ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				திரும்பு -FDT_ERR_NOTFOUND;
			இல்லையெனில்
				திரும்பு nextoffset;
		}
	} போது (tag != FDT_BEGIN_முனை);

	திரும்பு offset;
}

முழு fdt_first_subnode(மாறிலி வெறுமை *fdt, முழு offset)
{
	முழு depth = 0;

	offset = fdt_next_முனை(fdt, offset, &depth);
	எனில் (offset < 0 || depth != 1)
		திரும்பு -FDT_ERR_NOTFOUND;

	திரும்பு offset;
}

முழு fdt_next_subnode(மாறிலி வெறுமை *fdt, முழு offset)
{
	முழு depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	செய் {
		offset = fdt_next_முனை(fdt, offset, &depth);
		எனில் (offset < 0 || depth < 1)
			திரும்பு -FDT_ERR_NOTFOUND;
	} போது (depth > 1);

	திரும்பு offset;
}

மாறிலி எழுத்து *fdt_find_string_(மாறிலி எழுத்து *strtab, முழு tabsize, மாறிலி எழுத்து *s)
{
	முழு len = strlen(s) + 1;
	மாறிலி எழுத்து *last = strtab + tabsize - len;
	மாறிலி எழுத்து *p;

	க்கு (p = strtab; p <= last; p++)
		எனில் (memcmp(p, s, len) == 0)
			திரும்பு p;
	திரும்பு வெற்று;
}

முழு fdt_move(மாறிலி வெறுமை *fdt, வெறுமை *buf, முழு bufsize)
{
	எனில் (!can_assume(VALID_உள்ளீடு) && bufsize < 0)
		திரும்பு -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	எனில் (fdt_totalsize(fdt) > (unsigned முழு)bufsize)
		திரும்பு -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	திரும்பு 0;
}
