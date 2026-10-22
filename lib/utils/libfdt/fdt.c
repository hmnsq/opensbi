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

	如果 (can_assume(VALID_DTB))
		返回 totalsize;

	/* The device tree must be at an 8-byte aligned address */
	如果 ((uintptr_t)fdt & 7)
		返回 -FDT_ERR_ALIGNMENT;

	如果 (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		如果 (!can_assume(LATEST)) {
			如果 (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				返回 -FDT_ERR_BADVERSION;
			如果 (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				返回 -FDT_ERR_BADVERSION;
		}
	} 否则 如果 (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		如果 (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			返回 -FDT_ERR_BADSTATE;
	} 否则 {
		返回 -FDT_ERR_BADMAGIC;
	}

	如果 (totalsize < INT32_MAX)
		返回 totalsize;
	否则
		返回 -FDT_ERR_TRUNCATED;
}

static 整数 check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	返回 (off >= hdrsize) && (off <= totalsize);
}

static 整数 check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	如果 (!check_off_(hdrsize, totalsize, base))
		返回 0; /* block start out of bounds */
	如果 ((base + size) < base)
		返回 0; /* overflow */
	如果 (!check_off_(hdrsize, totalsize, base + size))
		返回 0; /* block end out of bounds */
	返回 1;
}

size_t fdt_header_size_(uint32_t version)
{
	如果 (version <= 1)
		返回 FDT_V1_SIZE;
	否则 如果 (version <= 2)
		返回 FDT_V2_SIZE;
	否则 如果 (version <= 3)
		返回 FDT_V3_SIZE;
	否则 如果 (version <= 16)
		返回 FDT_V16_SIZE;
	否则
		返回 FDT_V17_SIZE;
}

size_t fdt_header_size(const void *fdt)
{
	返回 can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

整数 fdt_check_header(const void *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	如果 ((uintptr_t)fdt & 7)
		返回 -FDT_ERR_ALIGNMENT;

	如果 (fdt_magic(fdt) != FDT_MAGIC)
		返回 -FDT_ERR_BADMAGIC;
	如果 (!can_assume(LATEST)) {
		如果 ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			返回 -FDT_ERR_BADVERSION;
		如果 (fdt_version(fdt) < fdt_last_comp_version(fdt))
			返回 -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	如果 (!can_assume(VALID_DTB)) {

		如果 ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			返回 -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		如果 (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			返回 -FDT_ERR_TRUNCATED;
	}

	如果 (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		如果 (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			如果 (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				返回 -FDT_ERR_TRUNCATED;
		} 否则 {
			如果 (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				返回 -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		如果 (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			返回 -FDT_ERR_TRUNCATED;
	}

	返回 0;
}

const void *fdt_offset_ptr(const void *fdt, 整数 offset, unsigned 整数 len)
{
	unsigned 整数 uoffset = offset;
	unsigned 整数 absoffset = offset + fdt_off_dt_struct(fdt);

	如果 (offset < 0)
		返回 NULL;

	如果 (!can_assume(VALID_INPUT))
		如果 ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			返回 NULL;

	如果 (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		如果 (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			返回 NULL;

	返回 fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(const void *fdt, 整数 startoffset, 整数 *nextoffset)
{
	const fdt32_t *tagp, *lenp;
	uint32_t tag;
	整数 offset = startoffset;
	const char *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	如果 (!can_assume(VALID_DTB) && !tagp)
		返回 FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	switch (tag) {
	case FDT_BEGIN_NODE:
		/* skip name */
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} 当 (p && (*p != '\0'));
		如果 (!can_assume(VALID_DTB) && !p)
			返回 FDT_END; /* premature end */
		break;

	case FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, sizeof(*lenp));
		如果 (!can_assume(VALID_DTB) && !lenp)
			返回 FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += sizeof(struct fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		如果 (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		break;

	case FDT_END:
	case FDT_END_NODE:
	case FDT_NOP:
		break;

	default:
		返回 FDT_END;
	}

	如果 (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		返回 FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	返回 tag;
}

整数 fdt_check_node_offset_(const void *fdt, 整数 offset)
{
	如果 (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		返回 -FDT_ERR_BADOFFSET;

	如果 (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		返回 -FDT_ERR_BADOFFSET;

	返回 offset;
}

整数 fdt_check_prop_offset_(const void *fdt, 整数 offset)
{
	如果 (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		返回 -FDT_ERR_BADOFFSET;

	如果 (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		返回 -FDT_ERR_BADOFFSET;

	返回 offset;
}

整数 fdt_next_node(const void *fdt, 整数 offset, 整数 *depth)
{
	整数 nextoffset = 0;
	uint32_t tag;

	如果 (offset >= 0)
		如果 ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			返回 nextoffset;

	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		case FDT_PROP:
		case FDT_NOP:
			break;

		case FDT_BEGIN_NODE:
			如果 (depth)
				(*depth)++;
			break;

		case FDT_END_NODE:
			如果 (depth && ((--(*depth)) < 0))
				返回 nextoffset;
			break;

		case FDT_END:
			如果 ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				返回 -FDT_ERR_NOTFOUND;
			否则
				返回 nextoffset;
		}
	} 当 (tag != FDT_BEGIN_NODE);

	返回 offset;
}

整数 fdt_first_subnode(const void *fdt, 整数 offset)
{
	整数 depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	如果 (offset < 0 || depth != 1)
		返回 -FDT_ERR_NOTFOUND;

	返回 offset;
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
		如果 (offset < 0 || depth < 1)
			返回 -FDT_ERR_NOTFOUND;
	} 当 (depth > 1);

	返回 offset;
}

const char *fdt_find_string_(const char *strtab, 整数 tabsize, const char *s)
{
	整数 len = strlen(s) + 1;
	const char *last = strtab + tabsize - len;
	const char *p;

	为 (p = strtab; p <= last; p++)
		如果 (memcmp(p, s, len) == 0)
			返回 p;
	返回 NULL;
}

整数 fdt_move(const void *fdt, void *buf, 整数 bufsize)
{
	如果 (!can_assume(VALID_INPUT) && bufsize < 0)
		返回 -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	如果 (fdt_totalsize(fdt) > (unsigned 整数)bufsize)
		返回 -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	返回 0;
}
