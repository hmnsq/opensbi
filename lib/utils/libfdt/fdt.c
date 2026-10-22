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

	གལ་སྲིད (can_assume(VALID_DTB))
		ལོག totalsize;

	/* The device tree must be at an 8-byte aligned address */
	གལ་སྲིད ((uintptr_t)fdt & 7)
		ལོག -FDT_ERR_ALIGNMENT;

	གལ་སྲིད (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		གལ་སྲིད (!can_assume(LATEST)) {
			གལ་སྲིད (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				ལོག -FDT_ERR_BADVERSION;
			གལ་སྲིད (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				ལོག -FDT_ERR_BADVERSION;
		}
	} དེ་མིན གལ་སྲིད (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		གལ་སྲིད (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			ལོག -FDT_ERR_BADSTATE;
	} དེ་མིན {
		ལོག -FDT_ERR_BADMAGIC;
	}

	གལ་སྲིད (totalsize < INT32_MAX)
		ལོག totalsize;
	དེ་མིན
		ལོག -FDT_ERR_TRUNCATED;
}

static ཧྲིལ check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	ལོག (off >= hdrsize) && (off <= totalsize);
}

static ཧྲིལ check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	གལ་སྲིད (!check_off_(hdrsize, totalsize, base))
		ལོག 0; /* block start out of bounds */
	གལ་སྲིད ((base + size) < base)
		ལོག 0; /* overflow */
	གལ་སྲིད (!check_off_(hdrsize, totalsize, base + size))
		ལོག 0; /* block end out of bounds */
	ལོག 1;
}

size_t fdt_header_size_(uint32_t version)
{
	གལ་སྲིད (version <= 1)
		ལོག FDT_V1_SIZE;
	དེ་མིན གལ་སྲིད (version <= 2)
		ལོག FDT_V2_SIZE;
	དེ་མིན གལ་སྲིད (version <= 3)
		ལོག FDT_V3_SIZE;
	དེ་མིན གལ་སྲིད (version <= 16)
		ལོག FDT_V16_SIZE;
	དེ་མིན
		ལོག FDT_V17_SIZE;
}

size_t fdt_header_size(const void *fdt)
{
	ལོག can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

ཧྲིལ fdt_check_header(const void *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	གལ་སྲིད ((uintptr_t)fdt & 7)
		ལོག -FDT_ERR_ALIGNMENT;

	གལ་སྲིད (fdt_magic(fdt) != FDT_MAGIC)
		ལོག -FDT_ERR_BADMAGIC;
	གལ་སྲིད (!can_assume(LATEST)) {
		གལ་སྲིད ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			ལོག -FDT_ERR_BADVERSION;
		གལ་སྲིད (fdt_version(fdt) < fdt_last_comp_version(fdt))
			ལོག -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	གལ་སྲིད (!can_assume(VALID_DTB)) {

		གལ་སྲིད ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			ལོག -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		གལ་སྲིད (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			ལོག -FDT_ERR_TRUNCATED;
	}

	གལ་སྲིད (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		གལ་སྲིད (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			གལ་སྲིད (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				ལོག -FDT_ERR_TRUNCATED;
		} དེ་མིན {
			གལ་སྲིད (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				ལོག -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		གལ་སྲིད (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			ལོག -FDT_ERR_TRUNCATED;
	}

	ལོག 0;
}

const void *fdt_offset_ptr(const void *fdt, ཧྲིལ offset, unsigned ཧྲིལ len)
{
	unsigned ཧྲིལ uoffset = offset;
	unsigned ཧྲིལ absoffset = offset + fdt_off_dt_struct(fdt);

	གལ་སྲིད (offset < 0)
		ལོག NULL;

	གལ་སྲིད (!can_assume(VALID_INPUT))
		གལ་སྲིད ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			ལོག NULL;

	གལ་སྲིད (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		གལ་སྲིད (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			ལོག NULL;

	ལོག fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(const void *fdt, ཧྲིལ startoffset, ཧྲིལ *nextoffset)
{
	const fdt32_t *tagp, *lenp;
	uint32_t tag;
	ཧྲིལ offset = startoffset;
	const var_na *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	གལ་སྲིད (!can_assume(VALID_DTB) && !tagp)
		ལོག FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	switch (tag) {
	pak_sa FDT_BEGIN_NODE:
		/* skip name */
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} སྐབས (p && (*p != '\0'));
		གལ་སྲིད (!can_assume(VALID_DTB) && !p)
			ལོག FDT_END; /* premature end */
		bha_nga;

	pak_sa FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, parimaa_na(*lenp));
		གལ་སྲིད (!can_assume(VALID_DTB) && !lenp)
			ལོག FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += parimaa_na(sa_nracanaa fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		གལ་སྲིད (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		bha_nga;

	pak_sa FDT_END:
	pak_sa FDT_END_NODE:
	pak_sa FDT_NOP:
		bha_nga;

	default:
		ལོག FDT_END;
	}

	གལ་སྲིད (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		ལོག FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	ལོག tag;
}

ཧྲིལ fdt_check_node_offset_(const void *fdt, ཧྲིལ offset)
{
	གལ་སྲིད (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ལོག -FDT_ERR_BADOFFSET;

	གལ་སྲིད (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		ལོག -FDT_ERR_BADOFFSET;

	ལོག offset;
}

ཧྲིལ fdt_check_prop_offset_(const void *fdt, ཧྲིལ offset)
{
	གལ་སྲིད (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ལོག -FDT_ERR_BADOFFSET;

	གལ་སྲིད (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		ལོག -FDT_ERR_BADOFFSET;

	ལོག offset;
}

ཧྲིལ fdt_next_node(const void *fdt, ཧྲིལ offset, ཧྲིལ *depth)
{
	ཧྲིལ nextoffset = 0;
	uint32_t tag;

	གལ་སྲིད (offset >= 0)
		གལ་སྲིད ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			ལོག nextoffset;

	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_PROP:
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_BEGIN_NODE:
			གལ་སྲིད (depth)
				(*depth)++;
			bha_nga;

		pak_sa FDT_END_NODE:
			གལ་སྲིད (depth && ((--(*depth)) < 0))
				ལོག nextoffset;
			bha_nga;

		pak_sa FDT_END:
			གལ་སྲིད ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				ལོག -FDT_ERR_NOTFOUND;
			དེ་མིན
				ལོག nextoffset;
		}
	} སྐབས (tag != FDT_BEGIN_NODE);

	ལོག offset;
}

ཧྲིལ fdt_first_subnode(const void *fdt, ཧྲིལ offset)
{
	ཧྲིལ depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	གལ་སྲིད (offset < 0 || depth != 1)
		ལོག -FDT_ERR_NOTFOUND;

	ལོག offset;
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
		གལ་སྲིད (offset < 0 || depth < 1)
			ལོག -FDT_ERR_NOTFOUND;
	} སྐབས (depth > 1);

	ལོག offset;
}

const var_na *fdt_find_string_(const var_na *strtab, ཧྲིལ tabsize, const var_na *s)
{
	ཧྲིལ len = strlen(s) + 1;
	const var_na *last = strtab + tabsize - len;
	const var_na *p;

	དོན་ལུ (p = strtab; p <= last; p++)
		གལ་སྲིད (memcmp(p, s, len) == 0)
			ལོག p;
	ལོག NULL;
}

ཧྲིལ fdt_move(const void *fdt, void *buf, ཧྲིལ bufsize)
{
	གལ་སྲིད (!can_assume(VALID_INPUT) && bufsize < 0)
		ལོག -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	གལ་སྲིད (fdt_totalsize(fdt) > (unsigned ཧྲིལ)bufsize)
		ལོག -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	ལོག 0;
}
