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

	もし (can_assume(VALID_DTB))
		戻る totalsize;

	/* The device tree must be at an 8-byte aligned address */
	もし ((uintptr_t)fdt & 7)
		戻る -FDT_ERR_ALIGNMENT;

	もし (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		もし (!can_assume(LATEST)) {
			もし (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				戻る -FDT_ERR_BADVERSION;
			もし (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				戻る -FDT_ERR_BADVERSION;
		}
	} 否则 もし (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		もし (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			戻る -FDT_ERR_BADSTATE;
	} 否则 {
		戻る -FDT_ERR_BADMAGIC;
	}

	もし (totalsize < INT32_MAX)
		戻る totalsize;
	否则
		戻る -FDT_ERR_TRUNCATED;
}

static 整数 check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	戻る (off >= hdrsize) && (off <= totalsize);
}

static 整数 check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	もし (!check_off_(hdrsize, totalsize, base))
		戻る 0; /* block start out of bounds */
	もし ((base + size) < base)
		戻る 0; /* overflow */
	もし (!check_off_(hdrsize, totalsize, base + size))
		戻る 0; /* block end out of bounds */
	戻る 1;
}

size_t fdt_header_size_(uint32_t version)
{
	もし (version <= 1)
		戻る FDT_V1_SIZE;
	否则 もし (version <= 2)
		戻る FDT_V2_SIZE;
	否则 もし (version <= 3)
		戻る FDT_V3_SIZE;
	否则 もし (version <= 16)
		戻る FDT_V16_SIZE;
	否则
		戻る FDT_V17_SIZE;
}

size_t fdt_header_size(const void *fdt)
{
	戻る can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

整数 fdt_check_header(const void *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	もし ((uintptr_t)fdt & 7)
		戻る -FDT_ERR_ALIGNMENT;

	もし (fdt_magic(fdt) != FDT_MAGIC)
		戻る -FDT_ERR_BADMAGIC;
	もし (!can_assume(LATEST)) {
		もし ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			戻る -FDT_ERR_BADVERSION;
		もし (fdt_version(fdt) < fdt_last_comp_version(fdt))
			戻る -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	もし (!can_assume(VALID_DTB)) {

		もし ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			戻る -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		もし (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			戻る -FDT_ERR_TRUNCATED;
	}

	もし (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		もし (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			もし (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				戻る -FDT_ERR_TRUNCATED;
		} 否则 {
			もし (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				戻る -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		もし (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			戻る -FDT_ERR_TRUNCATED;
	}

	戻る 0;
}

const void *fdt_offset_ptr(const void *fdt, 整数 offset, unsigned 整数 len)
{
	unsigned 整数 uoffset = offset;
	unsigned 整数 absoffset = offset + fdt_off_dt_struct(fdt);

	もし (offset < 0)
		戻る NULL;

	もし (!can_assume(VALID_INPUT))
		もし ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			戻る NULL;

	もし (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		もし (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			戻る NULL;

	戻る fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(const void *fdt, 整数 startoffset, 整数 *nextoffset)
{
	const fdt32_t *tagp, *lenp;
	uint32_t tag;
	整数 offset = startoffset;
	const char *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	もし (!can_assume(VALID_DTB) && !tagp)
		戻る FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	switch (tag) {
	case FDT_BEGIN_NODE:
		/* skip name */
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} ながら (p && (*p != '\0'));
		もし (!can_assume(VALID_DTB) && !p)
			戻る FDT_END; /* premature end */
		break;

	case FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, sizeof(*lenp));
		もし (!can_assume(VALID_DTB) && !lenp)
			戻る FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += sizeof(struct fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		もし (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		break;

	case FDT_END:
	case FDT_END_NODE:
	case FDT_NOP:
		break;

	default:
		戻る FDT_END;
	}

	もし (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		戻る FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	戻る tag;
}

整数 fdt_check_node_offset_(const void *fdt, 整数 offset)
{
	もし (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		戻る -FDT_ERR_BADOFFSET;

	もし (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		戻る -FDT_ERR_BADOFFSET;

	戻る offset;
}

整数 fdt_check_prop_offset_(const void *fdt, 整数 offset)
{
	もし (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		戻る -FDT_ERR_BADOFFSET;

	もし (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		戻る -FDT_ERR_BADOFFSET;

	戻る offset;
}

整数 fdt_next_node(const void *fdt, 整数 offset, 整数 *depth)
{
	整数 nextoffset = 0;
	uint32_t tag;

	もし (offset >= 0)
		もし ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			戻る nextoffset;

	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		case FDT_PROP:
		case FDT_NOP:
			break;

		case FDT_BEGIN_NODE:
			もし (depth)
				(*depth)++;
			break;

		case FDT_END_NODE:
			もし (depth && ((--(*depth)) < 0))
				戻る nextoffset;
			break;

		case FDT_END:
			もし ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				戻る -FDT_ERR_NOTFOUND;
			否则
				戻る nextoffset;
		}
	} ながら (tag != FDT_BEGIN_NODE);

	戻る offset;
}

整数 fdt_first_subnode(const void *fdt, 整数 offset)
{
	整数 depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	もし (offset < 0 || depth != 1)
		戻る -FDT_ERR_NOTFOUND;

	戻る offset;
}

整数 fdt_next_subnode(const void *fdt, 整数 offset)
{
	整数 depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	do {
		offset = fdt_next_node(fdt, offset, &depth);
		もし (offset < 0 || depth < 1)
			戻る -FDT_ERR_NOTFOUND;
	} ながら (depth > 1);

	戻る offset;
}

const char *fdt_find_string_(const char *strtab, 整数 tabsize, const char *s)
{
	整数 len = strlen(s) + 1;
	const char *last = strtab + tabsize - len;
	const char *p;

	ために (p = strtab; p <= last; p++)
		もし (memcmp(p, s, len) == 0)
			戻る p;
	戻る NULL;
}

整数 fdt_move(const void *fdt, void *buf, 整数 bufsize)
{
	もし (!can_assume(VALID_INPUT) && bufsize < 0)
		戻る -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	もし (fdt_totalsize(fdt) > (unsigned 整数)bufsize)
		戻る -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	戻る 0;
}
