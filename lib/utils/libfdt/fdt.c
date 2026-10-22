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

	nếu (can_assume(VALID_DTB))
		trả_về totalsize;

	/* The device tree must be at an 8-byte aligned address */
	nếu ((uintptr_t)fdt & 7)
		trả_về -FDT_ERR_ALIGNMENT;

	nếu (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		nếu (!can_assume(LATEST)) {
			nếu (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				trả_về -FDT_ERR_BADVERSION;
			nếu (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				trả_về -FDT_ERR_BADVERSION;
		}
	} không_thì nếu (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		nếu (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			trả_về -FDT_ERR_BADSTATE;
	} không_thì {
		trả_về -FDT_ERR_BADMAGIC;
	}

	nếu (totalsize < INT32_MAX)
		trả_về totalsize;
	không_thì
		trả_về -FDT_ERR_TRUNCATED;
}

static số_nguyên check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	trả_về (off >= hdrsize) && (off <= totalsize);
}

static số_nguyên check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	nếu (!check_off_(hdrsize, totalsize, base))
		trả_về 0; /* block start out of bounds */
	nếu ((base + size) < base)
		trả_về 0; /* overflow */
	nếu (!check_off_(hdrsize, totalsize, base + size))
		trả_về 0; /* block end out of bounds */
	trả_về 1;
}

size_t fdt_header_size_(uint32_t version)
{
	nếu (version <= 1)
		trả_về FDT_V1_SIZE;
	không_thì nếu (version <= 2)
		trả_về FDT_V2_SIZE;
	không_thì nếu (version <= 3)
		trả_về FDT_V3_SIZE;
	không_thì nếu (version <= 16)
		trả_về FDT_V16_SIZE;
	không_thì
		trả_về FDT_V17_SIZE;
}

size_t fdt_header_size(const void *fdt)
{
	trả_về can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

số_nguyên fdt_check_header(const void *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	nếu ((uintptr_t)fdt & 7)
		trả_về -FDT_ERR_ALIGNMENT;

	nếu (fdt_magic(fdt) != FDT_MAGIC)
		trả_về -FDT_ERR_BADMAGIC;
	nếu (!can_assume(LATEST)) {
		nếu ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			trả_về -FDT_ERR_BADVERSION;
		nếu (fdt_version(fdt) < fdt_last_comp_version(fdt))
			trả_về -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	nếu (!can_assume(VALID_DTB)) {

		nếu ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			trả_về -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		nếu (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			trả_về -FDT_ERR_TRUNCATED;
	}

	nếu (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		nếu (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			nếu (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				trả_về -FDT_ERR_TRUNCATED;
		} không_thì {
			nếu (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				trả_về -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		nếu (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			trả_về -FDT_ERR_TRUNCATED;
	}

	trả_về 0;
}

const void *fdt_offset_ptr(const void *fdt, số_nguyên offset, unsigned số_nguyên len)
{
	unsigned số_nguyên uoffset = offset;
	unsigned số_nguyên absoffset = offset + fdt_off_dt_struct(fdt);

	nếu (offset < 0)
		trả_về NULL;

	nếu (!can_assume(VALID_INPUT))
		nếu ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			trả_về NULL;

	nếu (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		nếu (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			trả_về NULL;

	trả_về fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(const void *fdt, số_nguyên startoffset, số_nguyên *nextoffset)
{
	const fdt32_t *tagp, *lenp;
	uint32_t tag;
	số_nguyên offset = startoffset;
	const char *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	nếu (!can_assume(VALID_DTB) && !tagp)
		trả_về FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	switch (tag) {
	case FDT_BEGIN_NODE:
		/* skip name */
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} trong_khi (p && (*p != '\0'));
		nếu (!can_assume(VALID_DTB) && !p)
			trả_về FDT_END; /* premature end */
		break;

	case FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, sizeof(*lenp));
		nếu (!can_assume(VALID_DTB) && !lenp)
			trả_về FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += sizeof(struct fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		nếu (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		break;

	case FDT_END:
	case FDT_END_NODE:
	case FDT_NOP:
		break;

	default:
		trả_về FDT_END;
	}

	nếu (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		trả_về FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	trả_về tag;
}

số_nguyên fdt_check_node_offset_(const void *fdt, số_nguyên offset)
{
	nếu (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		trả_về -FDT_ERR_BADOFFSET;

	nếu (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		trả_về -FDT_ERR_BADOFFSET;

	trả_về offset;
}

số_nguyên fdt_check_prop_offset_(const void *fdt, số_nguyên offset)
{
	nếu (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		trả_về -FDT_ERR_BADOFFSET;

	nếu (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		trả_về -FDT_ERR_BADOFFSET;

	trả_về offset;
}

số_nguyên fdt_next_node(const void *fdt, số_nguyên offset, số_nguyên *depth)
{
	số_nguyên nextoffset = 0;
	uint32_t tag;

	nếu (offset >= 0)
		nếu ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			trả_về nextoffset;

	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		case FDT_PROP:
		case FDT_NOP:
			break;

		case FDT_BEGIN_NODE:
			nếu (depth)
				(*depth)++;
			break;

		case FDT_END_NODE:
			nếu (depth && ((--(*depth)) < 0))
				trả_về nextoffset;
			break;

		case FDT_END:
			nếu ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				trả_về -FDT_ERR_NOTFOUND;
			không_thì
				trả_về nextoffset;
		}
	} trong_khi (tag != FDT_BEGIN_NODE);

	trả_về offset;
}

số_nguyên fdt_first_subnode(const void *fdt, số_nguyên offset)
{
	số_nguyên depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	nếu (offset < 0 || depth != 1)
		trả_về -FDT_ERR_NOTFOUND;

	trả_về offset;
}

số_nguyên fdt_next_subnode(const void *fdt, số_nguyên offset)
{
	số_nguyên depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	do {
		offset = fdt_next_node(fdt, offset, &depth);
		nếu (offset < 0 || depth < 1)
			trả_về -FDT_ERR_NOTFOUND;
	} trong_khi (depth > 1);

	trả_về offset;
}

const char *fdt_find_string_(const char *strtab, số_nguyên tabsize, const char *s)
{
	số_nguyên len = strlen(s) + 1;
	const char *last = strtab + tabsize - len;
	const char *p;

	cho (p = strtab; p <= last; p++)
		nếu (memcmp(p, s, len) == 0)
			trả_về p;
	trả_về NULL;
}

số_nguyên fdt_move(const void *fdt, void *buf, số_nguyên bufsize)
{
	nếu (!can_assume(VALID_INPUT) && bufsize < 0)
		trả_về -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	nếu (fdt_totalsize(fdt) > (unsigned số_nguyên)bufsize)
		trả_về -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	trả_về 0;
}
