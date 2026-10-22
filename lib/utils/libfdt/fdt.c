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
		върни totalsize;

	/* The device tree must be at an 8-byte aligned address */
	ако ((uintptr_t)fdt & 7)
		върни -FDT_ERR_ALIGNMENT;

	ако (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		ако (!can_assume(LATEST)) {
			ако (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				върни -FDT_ERR_BADVERSION;
			ако (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				върни -FDT_ERR_BADVERSION;
		}
	} иначе ако (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		ако (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			върни -FDT_ERR_BADSTATE;
	} иначе {
		върни -FDT_ERR_BADMAGIC;
	}

	ако (totalsize < INT32_MAX)
		върни totalsize;
	иначе
		върни -FDT_ERR_TRUNCATED;
}

static цяло check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	върни (off >= hdrsize) && (off <= totalsize);
}

static цяло check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	ако (!check_off_(hdrsize, totalsize, base))
		върни 0; /* block start out of bounds */
	ако ((base + size) < base)
		върни 0; /* overflow */
	ако (!check_off_(hdrsize, totalsize, base + size))
		върни 0; /* block end out of bounds */
	върни 1;
}

size_t fdt_header_size_(uint32_t version)
{
	ако (version <= 1)
		върни FDT_V1_SIZE;
	иначе ако (version <= 2)
		върни FDT_V2_SIZE;
	иначе ако (version <= 3)
		върни FDT_V3_SIZE;
	иначе ако (version <= 16)
		върни FDT_V16_SIZE;
	иначе
		върни FDT_V17_SIZE;
}

size_t fdt_header_size(const void *fdt)
{
	върни can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

цяло fdt_check_header(const void *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	ако ((uintptr_t)fdt & 7)
		върни -FDT_ERR_ALIGNMENT;

	ако (fdt_magic(fdt) != FDT_MAGIC)
		върни -FDT_ERR_BADMAGIC;
	ако (!can_assume(LATEST)) {
		ако ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			върни -FDT_ERR_BADVERSION;
		ако (fdt_version(fdt) < fdt_last_comp_version(fdt))
			върни -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	ако (!can_assume(VALID_DTB)) {

		ако ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			върни -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		ако (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			върни -FDT_ERR_TRUNCATED;
	}

	ако (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		ако (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			ако (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				върни -FDT_ERR_TRUNCATED;
		} иначе {
			ако (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				върни -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		ако (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			върни -FDT_ERR_TRUNCATED;
	}

	върни 0;
}

const void *fdt_offset_ptr(const void *fdt, цяло offset, unsigned цяло len)
{
	unsigned цяло uoffset = offset;
	unsigned цяло absoffset = offset + fdt_off_dt_struct(fdt);

	ако (offset < 0)
		върни NULL;

	ако (!can_assume(VALID_INPUT))
		ако ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			върни NULL;

	ако (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		ако (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			върни NULL;

	върни fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(const void *fdt, цяло startoffset, цяло *nextoffset)
{
	const fdt32_t *tagp, *lenp;
	uint32_t tag;
	цяло offset = startoffset;
	const char *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	ако (!can_assume(VALID_DTB) && !tagp)
		върни FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	switch (tag) {
	case FDT_BEGIN_NODE:
		/* skip name */
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} докато (p && (*p != '\0'));
		ако (!can_assume(VALID_DTB) && !p)
			върни FDT_END; /* premature end */
		break;

	case FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, sizeof(*lenp));
		ако (!can_assume(VALID_DTB) && !lenp)
			върни FDT_END; /* premature end */
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
		върни FDT_END;
	}

	ако (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		върни FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	върни tag;
}

цяло fdt_check_node_offset_(const void *fdt, цяло offset)
{
	ако (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		върни -FDT_ERR_BADOFFSET;

	ако (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		върни -FDT_ERR_BADOFFSET;

	върни offset;
}

цяло fdt_check_prop_offset_(const void *fdt, цяло offset)
{
	ако (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		върни -FDT_ERR_BADOFFSET;

	ако (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		върни -FDT_ERR_BADOFFSET;

	върни offset;
}

цяло fdt_next_node(const void *fdt, цяло offset, цяло *depth)
{
	цяло nextoffset = 0;
	uint32_t tag;

	ако (offset >= 0)
		ако ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			върни nextoffset;

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
				върни nextoffset;
			break;

		case FDT_END:
			ако ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				върни -FDT_ERR_NOTFOUND;
			иначе
				върни nextoffset;
		}
	} докато (tag != FDT_BEGIN_NODE);

	върни offset;
}

цяло fdt_first_subnode(const void *fdt, цяло offset)
{
	цяло depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	ако (offset < 0 || depth != 1)
		върни -FDT_ERR_NOTFOUND;

	върни offset;
}

цяло fdt_next_subnode(const void *fdt, цяло offset)
{
	цяло depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	do {
		offset = fdt_next_node(fdt, offset, &depth);
		ако (offset < 0 || depth < 1)
			върни -FDT_ERR_NOTFOUND;
	} докато (depth > 1);

	върни offset;
}

const char *fdt_find_string_(const char *strtab, цяло tabsize, const char *s)
{
	цяло len = strlen(s) + 1;
	const char *last = strtab + tabsize - len;
	const char *p;

	за (p = strtab; p <= last; p++)
		ако (memcmp(p, s, len) == 0)
			върни p;
	върни NULL;
}

цяло fdt_move(const void *fdt, void *buf, цяло bufsize)
{
	ако (!can_assume(VALID_INPUT) && bufsize < 0)
		върни -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	ако (fdt_totalsize(fdt) > (unsigned цяло)bufsize)
		върни -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	върни 0;
}
