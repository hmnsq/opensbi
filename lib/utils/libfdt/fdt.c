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

	если (can_assume(VALID_DTB))
		вернуть totalsize;

	/* The device tree must be at an 8-byte aligned address */
	если ((uintptr_t)fdt & 7)
		вернуть -FDT_ERR_ALIGNMENT;

	если (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		если (!can_assume(LATEST)) {
			если (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				вернуть -FDT_ERR_BADVERSION;
			если (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				вернуть -FDT_ERR_BADVERSION;
		}
	} иначе если (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		если (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			вернуть -FDT_ERR_BADSTATE;
	} иначе {
		вернуть -FDT_ERR_BADMAGIC;
	}

	если (totalsize < INT32_MAX)
		вернуть totalsize;
	иначе
		вернуть -FDT_ERR_TRUNCATED;
}

static целое check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	вернуть (off >= hdrsize) && (off <= totalsize);
}

static целое check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	если (!check_off_(hdrsize, totalsize, base))
		вернуть 0; /* block start out of bounds */
	если ((base + size) < base)
		вернуть 0; /* overflow */
	если (!check_off_(hdrsize, totalsize, base + size))
		вернуть 0; /* block end out of bounds */
	вернуть 1;
}

size_t fdt_header_size_(uint32_t version)
{
	если (version <= 1)
		вернуть FDT_V1_SIZE;
	иначе если (version <= 2)
		вернуть FDT_V2_SIZE;
	иначе если (version <= 3)
		вернуть FDT_V3_SIZE;
	иначе если (version <= 16)
		вернуть FDT_V16_SIZE;
	иначе
		вернуть FDT_V17_SIZE;
}

size_t fdt_header_size(const void *fdt)
{
	вернуть can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

целое fdt_check_header(const void *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	если ((uintptr_t)fdt & 7)
		вернуть -FDT_ERR_ALIGNMENT;

	если (fdt_magic(fdt) != FDT_MAGIC)
		вернуть -FDT_ERR_BADMAGIC;
	если (!can_assume(LATEST)) {
		если ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			вернуть -FDT_ERR_BADVERSION;
		если (fdt_version(fdt) < fdt_last_comp_version(fdt))
			вернуть -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	если (!can_assume(VALID_DTB)) {

		если ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			вернуть -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		если (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			вернуть -FDT_ERR_TRUNCATED;
	}

	если (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		если (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			если (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				вернуть -FDT_ERR_TRUNCATED;
		} иначе {
			если (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				вернуть -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		если (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			вернуть -FDT_ERR_TRUNCATED;
	}

	вернуть 0;
}

const void *fdt_offset_ptr(const void *fdt, целое offset, unsigned целое len)
{
	unsigned целое uoffset = offset;
	unsigned целое absoffset = offset + fdt_off_dt_struct(fdt);

	если (offset < 0)
		вернуть NULL;

	если (!can_assume(VALID_INPUT))
		если ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			вернуть NULL;

	если (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		если (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			вернуть NULL;

	вернуть fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(const void *fdt, целое startoffset, целое *nextoffset)
{
	const fdt32_t *tagp, *lenp;
	uint32_t tag;
	целое offset = startoffset;
	const char *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	если (!can_assume(VALID_DTB) && !tagp)
		вернуть FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	switch (tag) {
	case FDT_BEGIN_NODE:
		/* skip name */
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} пока (p && (*p != '\0'));
		если (!can_assume(VALID_DTB) && !p)
			вернуть FDT_END; /* premature end */
		break;

	case FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, sizeof(*lenp));
		если (!can_assume(VALID_DTB) && !lenp)
			вернуть FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += sizeof(struct fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		если (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		break;

	case FDT_END:
	case FDT_END_NODE:
	case FDT_NOP:
		break;

	default:
		вернуть FDT_END;
	}

	если (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		вернуть FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	вернуть tag;
}

целое fdt_check_node_offset_(const void *fdt, целое offset)
{
	если (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		вернуть -FDT_ERR_BADOFFSET;

	если (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		вернуть -FDT_ERR_BADOFFSET;

	вернуть offset;
}

целое fdt_check_prop_offset_(const void *fdt, целое offset)
{
	если (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		вернуть -FDT_ERR_BADOFFSET;

	если (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		вернуть -FDT_ERR_BADOFFSET;

	вернуть offset;
}

целое fdt_next_node(const void *fdt, целое offset, целое *depth)
{
	целое nextoffset = 0;
	uint32_t tag;

	если (offset >= 0)
		если ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			вернуть nextoffset;

	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		case FDT_PROP:
		case FDT_NOP:
			break;

		case FDT_BEGIN_NODE:
			если (depth)
				(*depth)++;
			break;

		case FDT_END_NODE:
			если (depth && ((--(*depth)) < 0))
				вернуть nextoffset;
			break;

		case FDT_END:
			если ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				вернуть -FDT_ERR_NOTFOUND;
			иначе
				вернуть nextoffset;
		}
	} пока (tag != FDT_BEGIN_NODE);

	вернуть offset;
}

целое fdt_first_subnode(const void *fdt, целое offset)
{
	целое depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	если (offset < 0 || depth != 1)
		вернуть -FDT_ERR_NOTFOUND;

	вернуть offset;
}

целое fdt_next_subnode(const void *fdt, целое offset)
{
	целое depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	do {
		offset = fdt_next_node(fdt, offset, &depth);
		если (offset < 0 || depth < 1)
			вернуть -FDT_ERR_NOTFOUND;
	} пока (depth > 1);

	вернуть offset;
}

const char *fdt_find_string_(const char *strtab, целое tabsize, const char *s)
{
	целое len = strlen(s) + 1;
	const char *last = strtab + tabsize - len;
	const char *p;

	для (p = strtab; p <= last; p++)
		если (memcmp(p, s, len) == 0)
			вернуть p;
	вернуть NULL;
}

целое fdt_move(const void *fdt, void *buf, целое bufsize)
{
	если (!can_assume(VALID_INPUT) && bufsize < 0)
		вернуть -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	если (fdt_totalsize(fdt) > (unsigned целое)bufsize)
		вернуть -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	вернуть 0;
}
