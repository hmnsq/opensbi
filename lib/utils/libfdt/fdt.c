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

	ако (can_assume(VALID_DTB))
		врати totalsize;

	/* The device tree must be at an 8-byte aligned address */
	ако ((uintptr_t)fdt & 7)
		врати -FDT_ERR_ALIGNMENT;

	ако (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		ако (!can_assume(LATEST)) {
			ако (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				врати -FDT_ERR_BADVERSION;
			ако (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				врати -FDT_ERR_BADVERSION;
		}
	} инако ако (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		ако (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			врати -FDT_ERR_BADSTATE;
	} инако {
		врати -FDT_ERR_BADMAGIC;
	}

	ако (totalsize < INT32_MAX)
		врати totalsize;
	инако
		врати -FDT_ERR_TRUNCATED;
}

static цео check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	врати (off >= hdrsize) && (off <= totalsize);
}

static цео check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	ако (!check_off_(hdrsize, totalsize, base))
		врати 0; /* block start out of bounds */
	ако ((base + size) < base)
		врати 0; /* overflow */
	ако (!check_off_(hdrsize, totalsize, base + size))
		врати 0; /* block end out of bounds */
	врати 1;
}

size_t fdt_header_size_(uint32_t version)
{
	ако (version <= 1)
		врати FDT_V1_SIZE;
	инако ако (version <= 2)
		врати FDT_V2_SIZE;
	инако ако (version <= 3)
		врати FDT_V3_SIZE;
	инако ако (version <= 16)
		врати FDT_V16_SIZE;
	инако
		врати FDT_V17_SIZE;
}

size_t fdt_header_size(const void *fdt)
{
	врати can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

цео fdt_check_header(const void *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	ако ((uintptr_t)fdt & 7)
		врати -FDT_ERR_ALIGNMENT;

	ако (fdt_magic(fdt) != FDT_MAGIC)
		врати -FDT_ERR_BADMAGIC;
	ако (!can_assume(LATEST)) {
		ако ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			врати -FDT_ERR_BADVERSION;
		ако (fdt_version(fdt) < fdt_last_comp_version(fdt))
			врати -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	ако (!can_assume(VALID_DTB)) {

		ако ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			врати -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		ако (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			врати -FDT_ERR_TRUNCATED;
	}

	ако (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		ако (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			ако (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				врати -FDT_ERR_TRUNCATED;
		} инако {
			ако (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				врати -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		ако (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			врати -FDT_ERR_TRUNCATED;
	}

	врати 0;
}

const void *fdt_offset_ptr(const void *fdt, цео offset, unsigned цео len)
{
	unsigned цео uoffset = offset;
	unsigned цео absoffset = offset + fdt_off_dt_struct(fdt);

	ако (offset < 0)
		врати NULL;

	ако (!can_assume(VALID_INPUT))
		ако ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			врати NULL;

	ако (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		ако (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			врати NULL;

	врати fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(const void *fdt, цео startoffset, цео *nextoffset)
{
	const fdt32_t *tagp, *lenp;
	uint32_t tag;
	цео offset = startoffset;
	const char *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	ако (!can_assume(VALID_DTB) && !tagp)
		врати FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	switch (tag) {
	case FDT_BEGIN_NODE:
		/* skip name */
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} док (p && (*p != '\0'));
		ако (!can_assume(VALID_DTB) && !p)
			врати FDT_END; /* premature end */
		break;

	case FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, sizeof(*lenp));
		ако (!can_assume(VALID_DTB) && !lenp)
			врати FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += sizeof(struct fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		ако (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		break;

	case FDT_END:
	case FDT_END_NODE:
	case FDT_NOP:
		break;

	default:
		врати FDT_END;
	}

	ако (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		врати FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	врати tag;
}

цео fdt_check_node_offset_(const void *fdt, цео offset)
{
	ако (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		врати -FDT_ERR_BADOFFSET;

	ако (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		врати -FDT_ERR_BADOFFSET;

	врати offset;
}

цео fdt_check_prop_offset_(const void *fdt, цео offset)
{
	ако (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		врати -FDT_ERR_BADOFFSET;

	ако (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		врати -FDT_ERR_BADOFFSET;

	врати offset;
}

цео fdt_next_node(const void *fdt, цео offset, цео *depth)
{
	цео nextoffset = 0;
	uint32_t tag;

	ако (offset >= 0)
		ако ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			врати nextoffset;

	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		case FDT_PROP:
		case FDT_NOP:
			break;

		case FDT_BEGIN_NODE:
			ако (depth)
				(*depth)++;
			break;

		case FDT_END_NODE:
			ако (depth && ((--(*depth)) < 0))
				врати nextoffset;
			break;

		case FDT_END:
			ако ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				врати -FDT_ERR_NOTFOUND;
			инако
				врати nextoffset;
		}
	} док (tag != FDT_BEGIN_NODE);

	врати offset;
}

цео fdt_first_subnode(const void *fdt, цео offset)
{
	цео depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	ако (offset < 0 || depth != 1)
		врати -FDT_ERR_NOTFOUND;

	врати offset;
}

цео fdt_next_subnode(const void *fdt, цео offset)
{
	цео depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	do {
		offset = fdt_next_node(fdt, offset, &depth);
		ако (offset < 0 || depth < 1)
			врати -FDT_ERR_NOTFOUND;
	} док (depth > 1);

	врати offset;
}

const char *fdt_find_string_(const char *strtab, цео tabsize, const char *s)
{
	цео len = strlen(s) + 1;
	const char *last = strtab + tabsize - len;
	const char *p;

	за (p = strtab; p <= last; p++)
		ако (memcmp(p, s, len) == 0)
			врати p;
	врати NULL;
}

цео fdt_move(const void *fdt, void *buf, цео bufsize)
{
	ако (!can_assume(VALID_INPUT) && bufsize < 0)
		врати -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	ако (fdt_totalsize(fdt) > (unsigned цео)bufsize)
		врати -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	врати 0;
}
