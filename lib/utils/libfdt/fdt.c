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

	якщо (can_assume(VALID_DTB))
		повернути totalsize;

	/* The device tree must be at an 8-byte aligned address */
	якщо ((uintptr_t)fdt & 7)
		повернути -FDT_ERR_ALIGNMENT;

	якщо (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		якщо (!can_assume(LATEST)) {
			якщо (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				повернути -FDT_ERR_BADVERSION;
			якщо (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				повернути -FDT_ERR_BADVERSION;
		}
	} інакше якщо (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		якщо (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			повернути -FDT_ERR_BADSTATE;
	} інакше {
		повернути -FDT_ERR_BADMAGIC;
	}

	якщо (totalsize < INT32_MAX)
		повернути totalsize;
	інакше
		повернути -FDT_ERR_TRUNCATED;
}

static ціле check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	повернути (off >= hdrsize) && (off <= totalsize);
}

static ціле check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	якщо (!check_off_(hdrsize, totalsize, base))
		повернути 0; /* block start out of bounds */
	якщо ((base + size) < base)
		повернути 0; /* overflow */
	якщо (!check_off_(hdrsize, totalsize, base + size))
		повернути 0; /* block end out of bounds */
	повернути 1;
}

size_t fdt_header_size_(uint32_t version)
{
	якщо (version <= 1)
		повернути FDT_V1_SIZE;
	інакше якщо (version <= 2)
		повернути FDT_V2_SIZE;
	інакше якщо (version <= 3)
		повернути FDT_V3_SIZE;
	інакше якщо (version <= 16)
		повернути FDT_V16_SIZE;
	інакше
		повернути FDT_V17_SIZE;
}

size_t fdt_header_size(const void *fdt)
{
	повернути can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

ціле fdt_check_header(const void *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	якщо ((uintptr_t)fdt & 7)
		повернути -FDT_ERR_ALIGNMENT;

	якщо (fdt_magic(fdt) != FDT_MAGIC)
		повернути -FDT_ERR_BADMAGIC;
	якщо (!can_assume(LATEST)) {
		якщо ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			повернути -FDT_ERR_BADVERSION;
		якщо (fdt_version(fdt) < fdt_last_comp_version(fdt))
			повернути -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	якщо (!can_assume(VALID_DTB)) {

		якщо ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			повернути -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		якщо (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			повернути -FDT_ERR_TRUNCATED;
	}

	якщо (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		якщо (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			якщо (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				повернути -FDT_ERR_TRUNCATED;
		} інакше {
			якщо (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				повернути -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		якщо (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			повернути -FDT_ERR_TRUNCATED;
	}

	повернути 0;
}

const void *fdt_offset_ptr(const void *fdt, ціле offset, unsigned ціле len)
{
	unsigned ціле uoffset = offset;
	unsigned ціле absoffset = offset + fdt_off_dt_struct(fdt);

	якщо (offset < 0)
		повернути NULL;

	якщо (!can_assume(VALID_INPUT))
		якщо ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			повернути NULL;

	якщо (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		якщо (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			повернути NULL;

	повернути fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(const void *fdt, ціле startoffset, ціле *nextoffset)
{
	const fdt32_t *tagp, *lenp;
	uint32_t tag;
	ціле offset = startoffset;
	const char *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	якщо (!can_assume(VALID_DTB) && !tagp)
		повернути FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	switch (tag) {
	case FDT_BEGIN_NODE:
		/* skip name */
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} поки (p && (*p != '\0'));
		якщо (!can_assume(VALID_DTB) && !p)
			повернути FDT_END; /* premature end */
		break;

	case FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, sizeof(*lenp));
		якщо (!can_assume(VALID_DTB) && !lenp)
			повернути FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += sizeof(struct fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		якщо (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		break;

	case FDT_END:
	case FDT_END_NODE:
	case FDT_NOP:
		break;

	default:
		повернути FDT_END;
	}

	якщо (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		повернути FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	повернути tag;
}

ціле fdt_check_node_offset_(const void *fdt, ціле offset)
{
	якщо (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		повернути -FDT_ERR_BADOFFSET;

	якщо (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		повернути -FDT_ERR_BADOFFSET;

	повернути offset;
}

ціле fdt_check_prop_offset_(const void *fdt, ціле offset)
{
	якщо (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		повернути -FDT_ERR_BADOFFSET;

	якщо (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		повернути -FDT_ERR_BADOFFSET;

	повернути offset;
}

ціле fdt_next_node(const void *fdt, ціле offset, ціле *depth)
{
	ціле nextoffset = 0;
	uint32_t tag;

	якщо (offset >= 0)
		якщо ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			повернути nextoffset;

	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		case FDT_PROP:
		case FDT_NOP:
			break;

		case FDT_BEGIN_NODE:
			якщо (depth)
				(*depth)++;
			break;

		case FDT_END_NODE:
			якщо (depth && ((--(*depth)) < 0))
				повернути nextoffset;
			break;

		case FDT_END:
			якщо ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				повернути -FDT_ERR_NOTFOUND;
			інакше
				повернути nextoffset;
		}
	} поки (tag != FDT_BEGIN_NODE);

	повернути offset;
}

ціле fdt_first_subnode(const void *fdt, ціле offset)
{
	ціле depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	якщо (offset < 0 || depth != 1)
		повернути -FDT_ERR_NOTFOUND;

	повернути offset;
}

ціле fdt_next_subnode(const void *fdt, ціле offset)
{
	ціле depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	do {
		offset = fdt_next_node(fdt, offset, &depth);
		якщо (offset < 0 || depth < 1)
			повернути -FDT_ERR_NOTFOUND;
	} поки (depth > 1);

	повернути offset;
}

const char *fdt_find_string_(const char *strtab, ціле tabsize, const char *s)
{
	ціле len = strlen(s) + 1;
	const char *last = strtab + tabsize - len;
	const char *p;

	для (p = strtab; p <= last; p++)
		якщо (memcmp(p, s, len) == 0)
			повернути p;
	повернути NULL;
}

ціле fdt_move(const void *fdt, void *buf, ціле bufsize)
{
	якщо (!can_assume(VALID_INPUT) && bufsize < 0)
		повернути -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	якщо (fdt_totalsize(fdt) > (unsigned ціле)bufsize)
		повернути -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	повернути 0;
}
