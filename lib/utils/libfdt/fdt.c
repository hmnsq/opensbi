// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

/*
 * Minimal sanity check for a read-only tree. fdt_ro_probe_() checks
 * that the given buffer contains what appears to be a flattened
 * device tree with sane information in its header.
 */
int32_t fdt_ro_probe_(const void *fdt)
{
	uint32_t totalsize = fdt_totalsize(fdt);

	གལ་ཏེ (can_assume(VALID_DTB))
		ཕྱིར totalsize;

	/* The device tree must be at an 8-byte aligned address */
	གལ་ཏེ ((uintptr_t)fdt & 7)
		ཕྱིར -FDT_ERR_ALIGNMENT;

	གལ་ཏེ (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		གལ་ཏེ (!can_assume(LATEST)) {
			གལ་ཏེ (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				ཕྱིར -FDT_ERR_BADVERSION;
			གལ་ཏེ (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				ཕྱིར -FDT_ERR_BADVERSION;
		}
	} དེ་མིན གལ་ཏེ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		གལ་ཏེ (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			ཕྱིར -FDT_ERR_BADSTATE;
	} དེ་མིན {
		ཕྱིར -FDT_ERR_BADMAGIC;
	}

	གལ་ཏེ (totalsize < INT32_MAX)
		ཕྱིར totalsize;
	དེ་མིན
		ཕྱིར -FDT_ERR_TRUNCATED;
}

static ཧྲིལ check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	ཕྱིར (off >= hdrsize) && (off <= totalsize);
}

static ཧྲིལ check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	གལ་ཏེ (!check_off_(hdrsize, totalsize, base))
		ཕྱིར 0; /* block start out of bounds */
	གལ་ཏེ ((base + size) < base)
		ཕྱིར 0; /* overflow */
	གལ་ཏེ (!check_off_(hdrsize, totalsize, base + size))
		ཕྱིར 0; /* block end out of bounds */
	ཕྱིར 1;
}

size_t fdt_header_size_(uint32_t version)
{
	གལ་ཏེ (version <= 1)
		ཕྱིར FDT_V1_SIZE;
	དེ་མིན གལ་ཏེ (version <= 2)
		ཕྱིར FDT_V2_SIZE;
	དེ་མིན གལ་ཏེ (version <= 3)
		ཕྱིར FDT_V3_SIZE;
	དེ་མིན གལ་ཏེ (version <= 16)
		ཕྱིར FDT_V16_SIZE;
	དེ་མིན
		ཕྱིར FDT_V17_SIZE;
}

size_t fdt_header_size(const void *fdt)
{
	ཕྱིར can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

ཧྲིལ fdt_check_header(const void *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	གལ་ཏེ ((uintptr_t)fdt & 7)
		ཕྱིར -FDT_ERR_ALIGNMENT;

	གལ་ཏེ (fdt_magic(fdt) != FDT_MAGIC)
		ཕྱིར -FDT_ERR_BADMAGIC;
	གལ་ཏེ (!can_assume(LATEST)) {
		གལ་ཏེ ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			ཕྱིར -FDT_ERR_BADVERSION;
		གལ་ཏེ (fdt_version(fdt) < fdt_last_comp_version(fdt))
			ཕྱིར -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	གལ་ཏེ (!can_assume(VALID_DTB)) {

		གལ་ཏེ ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			ཕྱིར -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		གལ་ཏེ (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			ཕྱིར -FDT_ERR_TRUNCATED;
	}

	གལ་ཏེ (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		གལ་ཏེ (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			གལ་ཏེ (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				ཕྱིར -FDT_ERR_TRUNCATED;
		} དེ་མིན {
			གལ་ཏེ (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				ཕྱིར -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		གལ་ཏེ (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			ཕྱིར -FDT_ERR_TRUNCATED;
	}

	ཕྱིར 0;
}

const void *fdt_offset_ptr(const void *fdt, ཧྲིལ offset, unsigned ཧྲིལ len)
{
	unsigned ཧྲིལ uoffset = offset;
	unsigned ཧྲིལ absoffset = offset + fdt_off_dt_struct(fdt);

	གལ་ཏེ (offset < 0)
		ཕྱིར NULL;

	གལ་ཏེ (!can_assume(VALID_INPUT))
		གལ་ཏེ ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			ཕྱིར NULL;

	གལ་ཏེ (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		གལ་ཏེ (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			ཕྱིར NULL;

	ཕྱིར fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(const void *fdt, ཧྲིལ startoffset, ཧྲིལ *nextoffset)
{
	const fdt32_t *tagp, *lenp;
	uint32_t tag;
	ཧྲིལ offset = startoffset;
	const var_na *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	གལ་ཏེ (!can_assume(VALID_DTB) && !tagp)
		ཕྱིར FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	switch (tag) {
	pak_sa FDT_BEGIN_NODE:
		/* skip name */
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} སྐབས (p && (*p != '\0'));
		གལ་ཏེ (!can_assume(VALID_DTB) && !p)
			ཕྱིར FDT_END; /* premature end */
		bha_nga;

	pak_sa FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, parimaa_na(*lenp));
		གལ་ཏེ (!can_assume(VALID_DTB) && !lenp)
			ཕྱིར FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += parimaa_na(sa_nracanaa fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		གལ་ཏེ (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		bha_nga;

	pak_sa FDT_END:
	pak_sa FDT_END_NODE:
	pak_sa FDT_NOP:
		bha_nga;

	default:
		ཕྱིར FDT_END;
	}

	གལ་ཏེ (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		ཕྱིར FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	ཕྱིར tag;
}

ཧྲིལ fdt_check_node_offset_(const void *fdt, ཧྲིལ offset)
{
	གལ་ཏེ (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ཕྱིར -FDT_ERR_BADOFFSET;

	གལ་ཏེ (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		ཕྱིར -FDT_ERR_BADOFFSET;

	ཕྱིར offset;
}

ཧྲིལ fdt_check_prop_offset_(const void *fdt, ཧྲིལ offset)
{
	གལ་ཏེ (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ཕྱིར -FDT_ERR_BADOFFSET;

	གལ་ཏེ (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		ཕྱིར -FDT_ERR_BADOFFSET;

	ཕྱིར offset;
}

ཧྲིལ fdt_next_node(const void *fdt, ཧྲིལ offset, ཧྲིལ *depth)
{
	ཧྲིལ nextoffset = 0;
	uint32_t tag;

	གལ་ཏེ (offset >= 0)
		གལ་ཏེ ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			ཕྱིར nextoffset;

	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_PROP:
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_BEGIN_NODE:
			གལ་ཏེ (depth)
				(*depth)++;
			bha_nga;

		pak_sa FDT_END_NODE:
			གལ་ཏེ (depth && ((--(*depth)) < 0))
				ཕྱིར nextoffset;
			bha_nga;

		pak_sa FDT_END:
			གལ་ཏེ ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				ཕྱིར -FDT_ERR_NOTFOUND;
			དེ་མིན
				ཕྱིར nextoffset;
		}
	} སྐབས (tag != FDT_BEGIN_NODE);

	ཕྱིར offset;
}

ཧྲིལ fdt_first_subnode(const void *fdt, ཧྲིལ offset)
{
	ཧྲིལ depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	གལ་ཏེ (offset < 0 || depth != 1)
		ཕྱིར -FDT_ERR_NOTFOUND;

	ཕྱིར offset;
}

ཧྲིལ fdt_next_subnode(const void *fdt, ཧྲིལ offset)
{
	ཧྲིལ depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	do {
		offset = fdt_next_node(fdt, offset, &depth);
		གལ་ཏེ (offset < 0 || depth < 1)
			ཕྱིར -FDT_ERR_NOTFOUND;
	} སྐབས (depth > 1);

	ཕྱིར offset;
}

const var_na *fdt_find_string_(const var_na *strtab, ཧྲིལ tabsize, const var_na *s)
{
	ཧྲིལ len = strlen(s) + 1;
	const var_na *last = strtab + tabsize - len;
	const var_na *p;

	ཆེད་དུ (p = strtab; p <= last; p++)
		གལ་ཏེ (memcmp(p, s, len) == 0)
			ཕྱིར p;
	ཕྱིར NULL;
}

ཧྲིལ fdt_move(const void *fdt, void *buf, ཧྲིལ bufsize)
{
	གལ་ཏེ (!can_assume(VALID_INPUT) && bufsize < 0)
		ཕྱིར -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	གལ་ཏེ (fdt_totalsize(fdt) > (unsigned ཧྲིལ)bufsize)
		ཕྱིར -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	ཕྱིར 0;
}
