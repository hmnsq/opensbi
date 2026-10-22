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

	만약 (can_assume(VALID_DTB))
		반환 totalsize;

	/* The device tree must be at an 8-byte aligned address */
	만약 ((uintptr_t)fdt & 7)
		반환 -FDT_ERR_ALIGNMENT;

	만약 (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		만약 (!can_assume(LATEST)) {
			만약 (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				반환 -FDT_ERR_BADVERSION;
			만약 (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				반환 -FDT_ERR_BADVERSION;
		}
	} 아니면 만약 (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		만약 (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			반환 -FDT_ERR_BADSTATE;
	} 아니면 {
		반환 -FDT_ERR_BADMAGIC;
	}

	만약 (totalsize < INT32_MAX)
		반환 totalsize;
	아니면
		반환 -FDT_ERR_TRUNCATED;
}

static 정수 check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	반환 (off >= hdrsize) && (off <= totalsize);
}

static 정수 check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	만약 (!check_off_(hdrsize, totalsize, base))
		반환 0; /* block start out of bounds */
	만약 ((base + size) < base)
		반환 0; /* overflow */
	만약 (!check_off_(hdrsize, totalsize, base + size))
		반환 0; /* block end out of bounds */
	반환 1;
}

size_t fdt_header_size_(uint32_t version)
{
	만약 (version <= 1)
		반환 FDT_V1_SIZE;
	아니면 만약 (version <= 2)
		반환 FDT_V2_SIZE;
	아니면 만약 (version <= 3)
		반환 FDT_V3_SIZE;
	아니면 만약 (version <= 16)
		반환 FDT_V16_SIZE;
	아니면
		반환 FDT_V17_SIZE;
}

size_t fdt_header_size(const void *fdt)
{
	반환 can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

정수 fdt_check_header(const void *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	만약 ((uintptr_t)fdt & 7)
		반환 -FDT_ERR_ALIGNMENT;

	만약 (fdt_magic(fdt) != FDT_MAGIC)
		반환 -FDT_ERR_BADMAGIC;
	만약 (!can_assume(LATEST)) {
		만약 ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			반환 -FDT_ERR_BADVERSION;
		만약 (fdt_version(fdt) < fdt_last_comp_version(fdt))
			반환 -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	만약 (!can_assume(VALID_DTB)) {

		만약 ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			반환 -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		만약 (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			반환 -FDT_ERR_TRUNCATED;
	}

	만약 (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		만약 (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			만약 (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				반환 -FDT_ERR_TRUNCATED;
		} 아니면 {
			만약 (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				반환 -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		만약 (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			반환 -FDT_ERR_TRUNCATED;
	}

	반환 0;
}

const void *fdt_offset_ptr(const void *fdt, 정수 offset, unsigned 정수 len)
{
	unsigned 정수 uoffset = offset;
	unsigned 정수 absoffset = offset + fdt_off_dt_struct(fdt);

	만약 (offset < 0)
		반환 NULL;

	만약 (!can_assume(VALID_INPUT))
		만약 ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			반환 NULL;

	만약 (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		만약 (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			반환 NULL;

	반환 fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(const void *fdt, 정수 startoffset, 정수 *nextoffset)
{
	const fdt32_t *tagp, *lenp;
	uint32_t tag;
	정수 offset = startoffset;
	const char *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	만약 (!can_assume(VALID_DTB) && !tagp)
		반환 FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	switch (tag) {
	case FDT_BEGIN_NODE:
		/* skip name */
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} 동안 (p && (*p != '\0'));
		만약 (!can_assume(VALID_DTB) && !p)
			반환 FDT_END; /* premature end */
		break;

	case FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, sizeof(*lenp));
		만약 (!can_assume(VALID_DTB) && !lenp)
			반환 FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += sizeof(struct fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		만약 (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		break;

	case FDT_END:
	case FDT_END_NODE:
	case FDT_NOP:
		break;

	default:
		반환 FDT_END;
	}

	만약 (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		반환 FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	반환 tag;
}

정수 fdt_check_node_offset_(const void *fdt, 정수 offset)
{
	만약 (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		반환 -FDT_ERR_BADOFFSET;

	만약 (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		반환 -FDT_ERR_BADOFFSET;

	반환 offset;
}

정수 fdt_check_prop_offset_(const void *fdt, 정수 offset)
{
	만약 (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		반환 -FDT_ERR_BADOFFSET;

	만약 (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		반환 -FDT_ERR_BADOFFSET;

	반환 offset;
}

정수 fdt_next_node(const void *fdt, 정수 offset, 정수 *depth)
{
	정수 nextoffset = 0;
	uint32_t tag;

	만약 (offset >= 0)
		만약 ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			반환 nextoffset;

	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		case FDT_PROP:
		case FDT_NOP:
			break;

		case FDT_BEGIN_NODE:
			만약 (depth)
				(*depth)++;
			break;

		case FDT_END_NODE:
			만약 (depth && ((--(*depth)) < 0))
				반환 nextoffset;
			break;

		case FDT_END:
			만약 ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				반환 -FDT_ERR_NOTFOUND;
			아니면
				반환 nextoffset;
		}
	} 동안 (tag != FDT_BEGIN_NODE);

	반환 offset;
}

정수 fdt_first_subnode(const void *fdt, 정수 offset)
{
	정수 depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	만약 (offset < 0 || depth != 1)
		반환 -FDT_ERR_NOTFOUND;

	반환 offset;
}

정수 fdt_next_subnode(const void *fdt, 정수 offset)
{
	정수 depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	do {
		offset = fdt_next_node(fdt, offset, &depth);
		만약 (offset < 0 || depth < 1)
			반환 -FDT_ERR_NOTFOUND;
	} 동안 (depth > 1);

	반환 offset;
}

const char *fdt_find_string_(const char *strtab, 정수 tabsize, const char *s)
{
	정수 len = strlen(s) + 1;
	const char *last = strtab + tabsize - len;
	const char *p;

	위해 (p = strtab; p <= last; p++)
		만약 (memcmp(p, s, len) == 0)
			반환 p;
	반환 NULL;
}

정수 fdt_move(const void *fdt, void *buf, 정수 bufsize)
{
	만약 (!can_assume(VALID_INPUT) && bufsize < 0)
		반환 -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	만약 (fdt_totalsize(fdt) > (unsigned 정수)bufsize)
		반환 -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	반환 0;
}
